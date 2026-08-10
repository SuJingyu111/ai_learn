#pragma once

// A minimal levelled logger for the engine.
//
// Contract
//   - A message is emitted when its level is >= the active level, so raising
//     the active level silences everything below it and kSilent silences all.
//   - The active level is read from the environment variable TT_LOG_LEVEL the
//     first time any logging entry point runs, and can be changed at any time
//     with set_log_level(). Nothing here is decided at compile time: a release
//     build can still be asked for kVerbose output.
//   - The TT_LOG* macros evaluate their arguments only when the level passes,
//     so a disabled TT_LOGD costs one relaxed atomic load.
//   - log_write() formats the whole line into a stack buffer and hands it to
//     the output stream in a single fwrite, so lines from concurrent threads
//     do not interleave. It does not take a lock of its own.
//   - kFatal is a severity, not an action: it does not abort.

#include <atomic>
#include <cstdio>

#include "tt/common.h"

namespace tt {

// The numeric order is the severity order; do not renumber without updating
// parse_log_level() and the level letters in log.cpp.
enum class LogLevel : int {
  kVerbose = 0,
  kDebug = 1,
  kInfo = 2,
  kWarn = 3,
  kError = 4,
  kFatal = 5,
  kSilent = 6,  // Not loggable: only meaningful as an active level.
};

namespace detail {

// Holds the active level as the underlying int of LogLevel. Defined out of
// line so the first call performs the one-time read of TT_LOG_LEVEL.
std::atomic<int>& active_level();

}  // namespace detail

inline LogLevel log_level() {
  return static_cast<LogLevel>(detail::active_level().load(std::memory_order_relaxed));
}

// Safe to call from any thread at any time, including before the first log.
void set_log_level(LogLevel level);

inline bool log_enabled(LogLevel level) { return level >= log_level(); }

// Accepts a level name ("verbose", "warning"), its initial ("v", "W"), the
// aliases "off"/"none" for kSilent, or the decimal value ("0".."6"), in any
// case. Returns false and leaves *level untouched when the text is not one of
// those. `text` may be null, which is not a valid level.
bool parse_log_level(const char* text, LogLevel* level);

const char* log_level_name(LogLevel level);

// Redirects output. A null stream restores stderr. The caller keeps ownership
// of the stream and must not close it while another thread may still log.
void set_log_output(std::FILE* stream);

// Formats and emits one line unconditionally; the level filter lives in the
// TT_LOG macros. `tag` may be null. Messages longer than the internal buffer
// are truncated with a trailing "...".
void log_write(LogLevel level, const char* tag, const char* file, int line,
               const char* format, ...) TT_PRINTF_FORMAT(5, 6);

}  // namespace tt

#define TT_LOG(level, tag, ...)                                          \
  do {                                                                   \
    if (::tt::log_enabled(level)) {                                      \
      ::tt::log_write((level), (tag), __FILE__, __LINE__, __VA_ARGS__);  \
    }                                                                    \
  } while (0)

#define TT_LOGV(tag, ...) TT_LOG(::tt::LogLevel::kVerbose, tag, __VA_ARGS__)
#define TT_LOGD(tag, ...) TT_LOG(::tt::LogLevel::kDebug, tag, __VA_ARGS__)
#define TT_LOGI(tag, ...) TT_LOG(::tt::LogLevel::kInfo, tag, __VA_ARGS__)
#define TT_LOGW(tag, ...) TT_LOG(::tt::LogLevel::kWarn, tag, __VA_ARGS__)
#define TT_LOGE(tag, ...) TT_LOG(::tt::LogLevel::kError, tag, __VA_ARGS__)
#define TT_LOGF(tag, ...) TT_LOG(::tt::LogLevel::kFatal, tag, __VA_ARGS__)
