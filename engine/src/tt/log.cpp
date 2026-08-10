#include "tt/log.h"

#include <chrono>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <ctime>

namespace tt {
namespace {

constexpr LogLevel kDefaultLevel = LogLevel::kInfo;

// One line is composed in full before it reaches the stream, so these bounds
// are the hard limits on a single log record.
constexpr int kMessageCapacity = 1024;
constexpr int kLineCapacity = kMessageCapacity + 256;

std::atomic<std::FILE*>& output_stream() {
  // stderr is not a constant expression on every platform, so the pointer is
  // initialised on first use rather than at static-init time.
  static std::atomic<std::FILE*> stream{stderr};
  return stream;
}

bool equals_ignore_case(const char* text, const char* candidate) {
  for (; *text != '\0' && *candidate != '\0'; ++text, ++candidate) {
    const char lowered =
        (*text >= 'A' && *text <= 'Z') ? static_cast<char>(*text - 'A' + 'a') : *text;
    if (lowered != *candidate) {
      return false;
    }
  }
  return *text == '\0' && *candidate == '\0';
}

// Level letters indexed by LogLevel. kSilent never reaches log_write(), but a
// caller that passes it anyway should not read past the end of the table.
char level_letter(LogLevel level) {
  switch (level) {
    case LogLevel::kVerbose:
      return 'V';
    case LogLevel::kDebug:
      return 'D';
    case LogLevel::kInfo:
      return 'I';
    case LogLevel::kWarn:
      return 'W';
    case LogLevel::kError:
      return 'E';
    case LogLevel::kFatal:
      return 'F';
    case LogLevel::kSilent:
      return 'S';
  }
  return '?';
}

// __FILE__ carries whatever path the build used; only the file name is useful
// in a log line.
const char* base_name(const char* path) {
  if (path == nullptr) {
    return "?";
  }
  const char* name = path;
  for (const char* cursor = path; *cursor != '\0'; ++cursor) {
    if (*cursor == '/' || *cursor == '\\') {
      name = cursor + 1;
    }
  }
  return name;
}

LogLevel level_from_environment() {
  const char* text = std::getenv("TT_LOG_LEVEL");
  if (text == nullptr || *text == '\0') {
    return kDefaultLevel;
  }
  LogLevel level = kDefaultLevel;
  if (parse_log_level(text, &level)) {
    return level;
  }
  // Written directly rather than through log_write(), which would re-enter
  // active_level() while it is still being initialised.
  std::fprintf(stderr, "tt: ignoring TT_LOG_LEVEL=\"%s\" (not a level), using %s\n",
               text, log_level_name(kDefaultLevel));
  return kDefaultLevel;
}

}  // namespace

namespace detail {

std::atomic<int>& active_level() {
  // C++17 guarantees this initialisation runs once even under concurrent
  // first calls, so TT_LOG_LEVEL is read exactly once per process.
  static std::atomic<int> level{static_cast<int>(level_from_environment())};
  return level;
}

}  // namespace detail

void set_log_level(LogLevel level) {
  detail::active_level().store(static_cast<int>(level), std::memory_order_relaxed);
}

const char* log_level_name(LogLevel level) {
  switch (level) {
    case LogLevel::kVerbose:
      return "verbose";
    case LogLevel::kDebug:
      return "debug";
    case LogLevel::kInfo:
      return "info";
    case LogLevel::kWarn:
      return "warn";
    case LogLevel::kError:
      return "error";
    case LogLevel::kFatal:
      return "fatal";
    case LogLevel::kSilent:
      return "silent";
  }
  return "unknown";
}

bool parse_log_level(const char* text, LogLevel* level) {
  if (text == nullptr || level == nullptr || *text == '\0') {
    return false;
  }
  struct Alias {
    const char* text;
    LogLevel level;
  };
  static constexpr Alias kAliases[] = {
      {"verbose", LogLevel::kVerbose}, {"v", LogLevel::kVerbose},
      {"0", LogLevel::kVerbose},       {"debug", LogLevel::kDebug},
      {"d", LogLevel::kDebug},         {"1", LogLevel::kDebug},
      {"info", LogLevel::kInfo},       {"i", LogLevel::kInfo},
      {"2", LogLevel::kInfo},          {"warn", LogLevel::kWarn},
      {"warning", LogLevel::kWarn},    {"w", LogLevel::kWarn},
      {"3", LogLevel::kWarn},          {"error", LogLevel::kError},
      {"e", LogLevel::kError},         {"4", LogLevel::kError},
      {"fatal", LogLevel::kFatal},     {"f", LogLevel::kFatal},
      {"5", LogLevel::kFatal},         {"silent", LogLevel::kSilent},
      {"s", LogLevel::kSilent},        {"off", LogLevel::kSilent},
      {"none", LogLevel::kSilent},     {"6", LogLevel::kSilent},
  };
  for (const Alias& alias : kAliases) {
    if (equals_ignore_case(text, alias.text)) {
      *level = alias.level;
      return true;
    }
  }
  return false;
}

void set_log_output(std::FILE* stream) {
  output_stream().store(stream != nullptr ? stream : stderr, std::memory_order_relaxed);
}

void log_write(LogLevel level, const char* tag, const char* file, int line,
               const char* format, ...) {
  char message[kMessageCapacity];
  std::va_list arguments;
  va_start(arguments, format);
  const int written = std::vsnprintf(message, sizeof(message), format, arguments);
  va_end(arguments);
  if (written < 0) {
    std::snprintf(message, sizeof(message), "<invalid log format>");
  } else if (written >= static_cast<int>(sizeof(message))) {
    // vsnprintf truncated; mark the loss instead of reporting a whole message.
    std::snprintf(message + sizeof(message) - 4, 4, "...");
  }

  const auto now = std::chrono::system_clock::now();
  const std::time_t seconds = std::chrono::system_clock::to_time_t(now);
  const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                                now.time_since_epoch()) %
                            1000;
  std::tm local{};
#if defined(_WIN32)
  localtime_s(&local, &seconds);
#else
  localtime_r(&seconds, &local);
#endif

  char output[kLineCapacity];
  const int length = std::snprintf(
      output, sizeof(output), "%c %02d:%02d:%02d.%03d %s %s:%d] %s\n",
      level_letter(level), local.tm_hour, local.tm_min, local.tm_sec,
      static_cast<int>(milliseconds.count()), tag != nullptr ? tag : "-",
      base_name(file), line, message);
  if (length <= 0) {
    return;
  }
  const std::size_t emit =
      static_cast<std::size_t>(length) < sizeof(output) ? static_cast<std::size_t>(length)
                                                        : sizeof(output) - 1;

  std::FILE* stream = output_stream().load(std::memory_order_relaxed);
  // One fwrite of one complete line: concurrent writers cannot interleave
  // within a line, and the flush keeps the tail visible if the process dies.
  std::fwrite(output, 1, emit, stream);
  std::fflush(stream);
}

}  // namespace tt
