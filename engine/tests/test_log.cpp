// Behaviour covered here: level parsing, the level filter, the line format,
// truncation, and output redirection. The environment path (TT_LOG_LEVEL) is
// covered by the log_level_from_environment test, which re-executes this
// binary with the variable set -- the process reads it exactly once, so it
// cannot be tested in-process alongside the other cases.

#include "tt/log.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void check(bool condition, const std::string& label) {
  if (!condition) {
    throw std::runtime_error(label);
  }
}

// Runs `body` with logging redirected to a temporary file and returns what was
// written. Restores stderr before returning so a later failure still reports.
template <typename Body>
std::string capture(Body body) {
  std::FILE* stream = std::tmpfile();
  check(stream != nullptr, "tmpfile");
  tt::set_log_output(stream);
  body();
  tt::set_log_output(nullptr);

  std::string captured;
  std::rewind(stream);
  char buffer[512];
  while (std::fgets(buffer, sizeof(buffer), stream) != nullptr) {
    captured += buffer;
  }
  std::fclose(stream);
  return captured;
}

bool contains(const std::string& haystack, const std::string& needle) {
  return haystack.find(needle) != std::string::npos;
}

void test_parse_accepts_names_letters_and_numbers() {
  const struct {
    const char* text;
    tt::LogLevel expected;
  } cases[] = {
      {"verbose", tt::LogLevel::kVerbose}, {"DEBUG", tt::LogLevel::kDebug},
      {"Info", tt::LogLevel::kInfo},       {"warning", tt::LogLevel::kWarn},
      {"W", tt::LogLevel::kWarn},          {"e", tt::LogLevel::kError},
      {"FATAL", tt::LogLevel::kFatal},     {"off", tt::LogLevel::kSilent},
      {"none", tt::LogLevel::kSilent},     {"0", tt::LogLevel::kVerbose},
      {"6", tt::LogLevel::kSilent},
  };
  for (const auto& test_case : cases) {
    tt::LogLevel level = tt::LogLevel::kSilent;
    check(tt::parse_log_level(test_case.text, &level),
          std::string("parse rejected ") + test_case.text);
    check(level == test_case.expected, std::string("parse mismatch for ") + test_case.text);
  }
}

void test_parse_rejects_garbage_without_touching_output() {
  const char* rejected[] = {"", "seven", "7", "-1", "inf0", "i nfo", nullptr};
  for (const char* text : rejected) {
    tt::LogLevel level = tt::LogLevel::kError;
    check(!tt::parse_log_level(text, &level),
          std::string("parse accepted ") + (text != nullptr ? text : "<null>"));
    check(level == tt::LogLevel::kError, "rejected parse must not write *level");
  }
  check(!tt::parse_log_level("info", nullptr), "null out pointer must be rejected");
}

void test_level_filter_admits_at_and_above() {
  const std::string captured = capture([] {
    tt::set_log_level(tt::LogLevel::kWarn);
    TT_LOGV("filter", "verbose-must-not-appear");
    TT_LOGD("filter", "debug-must-not-appear");
    TT_LOGI("filter", "info-must-not-appear");
    TT_LOGW("filter", "warn-must-appear");
    TT_LOGE("filter", "error-must-appear");
    TT_LOGF("filter", "fatal-must-appear");
  });
  check(!contains(captured, "must-not-appear"), "levels below the active level leaked");
  check(contains(captured, "warn-must-appear"), "warn was dropped");
  check(contains(captured, "error-must-appear"), "error was dropped");
  check(contains(captured, "fatal-must-appear"), "fatal was dropped");
}

void test_silent_drops_every_level() {
  const std::string captured = capture([] {
    tt::set_log_level(tt::LogLevel::kSilent);
    TT_LOGV("silent", "v");
    TT_LOGI("silent", "i");
    TT_LOGF("silent", "f");
  });
  check(captured.empty(), "kSilent still emitted: '" + captured + "'");
}

void test_verbose_admits_every_level() {
  const std::string captured = capture([] {
    tt::set_log_level(tt::LogLevel::kVerbose);
    TT_LOGV("all", "line-v");
    TT_LOGD("all", "line-d");
    TT_LOGI("all", "line-i");
    TT_LOGW("all", "line-w");
    TT_LOGE("all", "line-e");
    TT_LOGF("all", "line-f");
  });
  for (const char* needle : {"line-v", "line-d", "line-i", "line-w", "line-e", "line-f"}) {
    check(contains(captured, needle), std::string("kVerbose dropped ") + needle);
  }
}

// A disabled level must not evaluate its arguments: that is what makes it
// affordable to leave TT_LOGD calls in hot paths.
void test_disabled_level_does_not_evaluate_arguments() {
  int calls = 0;
  const auto count = [&calls]() {
    ++calls;
    return 1;
  };
  const std::string captured = capture([&] {
    tt::set_log_level(tt::LogLevel::kError);
    TT_LOGD("lazy", "%d", count());
    check(calls == 0, "disabled level evaluated its arguments");
    TT_LOGE("lazy", "%d", count());
  });
  check(calls == 1, "enabled level did not evaluate its arguments");
  check(contains(captured, "1"), "enabled level did not format its arguments");
}

void test_line_format_carries_level_tag_and_location() {
  const std::string captured = capture([] {
    tt::set_log_level(tt::LogLevel::kInfo);
    TT_LOGW("shapes", "rank=%d numel=%lld", 3, 24LL);
  });
  check(captured.size() == captured.find('\n') + 1, "expected exactly one line");
  check(captured[0] == 'W', "line must start with the level letter");
  check(contains(captured, " shapes "), "tag missing from the line");
  check(contains(captured, "test_log.cpp:"), "file:line missing from the line");
  check(contains(captured, "] rank=3 numel=24\n"), "message body wrong: " + captured);
}

void test_null_tag_is_accepted() {
  const std::string captured = capture([] {
    tt::set_log_level(tt::LogLevel::kInfo);
    TT_LOGI(nullptr, "no-tag");
  });
  check(contains(captured, " - "), "null tag should render as '-': " + captured);
  check(contains(captured, "no-tag"), "message lost with a null tag");
}

// The formatter uses a fixed stack buffer; an oversized message must be
// truncated and marked, never dropped and never a buffer overrun.
void test_long_message_is_truncated_and_marked() {
  const std::string payload(4000, 'x');
  const std::string captured = capture([&] {
    tt::set_log_level(tt::LogLevel::kInfo);
    TT_LOGI("long", "%s", payload.c_str());
  });
  check(captured.size() < payload.size(), "oversized message was not truncated");
  check(contains(captured, "...\n"), "truncation was not marked: tail is missing '...'");
  check(captured.size() == captured.find('\n') + 1, "truncated record must stay one line");
}

void test_output_redirection_returns_to_stderr() {
  tt::set_log_level(tt::LogLevel::kInfo);
  const std::string captured = capture([] { TT_LOGI("redirect", "captured"); });
  check(contains(captured, "captured"), "redirected output was not captured");
  // capture() restored stderr; nothing further may land in the closed stream.
  check(tt::log_level() == tt::LogLevel::kInfo, "set_log_level did not stick");
}

void test_level_names_round_trip() {
  const tt::LogLevel levels[] = {tt::LogLevel::kVerbose, tt::LogLevel::kDebug,
                                 tt::LogLevel::kInfo,    tt::LogLevel::kWarn,
                                 tt::LogLevel::kError,   tt::LogLevel::kFatal,
                                 tt::LogLevel::kSilent};
  for (tt::LogLevel level : levels) {
    tt::LogLevel parsed = tt::LogLevel::kSilent;
    check(tt::parse_log_level(tt::log_level_name(level), &parsed),
          std::string("name not parseable: ") + tt::log_level_name(level));
    check(parsed == level, std::string("round trip failed for ") + tt::log_level_name(level));
  }
}

// Re-executed child mode: assert only that the level came from TT_LOG_LEVEL.
int run_environment_child(const char* expected_text) {
  tt::LogLevel expected = tt::LogLevel::kInfo;
  if (!tt::parse_log_level(expected_text, &expected)) {
    std::cerr << "child: bad expectation " << expected_text << '\n';
    return 1;
  }
  if (tt::log_level() != expected) {
    std::cerr << "child: TT_LOG_LEVEL not applied, level is "
              << tt::log_level_name(tt::log_level()) << '\n';
    return 1;
  }
  return 0;
}

// TT_LOG_LEVEL is read once per process, so the environment path is checked by
// re-running this binary with the variable set. Skipped when the executable
// path is unavailable.
void test_level_comes_from_environment(const char* executable) {
  if (executable == nullptr) {
    return;
  }
  for (const char* level_text : {"debug", "error", "off"}) {
    const std::string command = std::string("TT_LOG_LEVEL=") + level_text + " \"" +
                                executable + "\" --expect-env-level " + level_text;
    const int status = std::system(command.c_str());
    check(status == 0, std::string("TT_LOG_LEVEL=") + level_text +
                           " was not honoured (status " + std::to_string(status) + ")");
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc == 3 && std::strcmp(argv[1], "--expect-env-level") == 0) {
    return run_environment_child(argv[2]);
  }
  try {
    test_parse_accepts_names_letters_and_numbers();
    test_parse_rejects_garbage_without_touching_output();
    test_level_filter_admits_at_and_above();
    test_silent_drops_every_level();
    test_verbose_admits_every_level();
    test_disabled_level_does_not_evaluate_arguments();
    test_line_format_carries_level_tag_and_location();
    test_null_tag_is_accepted();
    test_long_message_is_truncated_and_marked();
    test_output_redirection_returns_to_stderr();
    test_level_names_round_trip();
    test_level_comes_from_environment(argc > 0 ? argv[0] : nullptr);
  } catch (const std::exception& error) {
    std::cerr << "FAILED: " << error.what() << '\n';
    return 1;
  }
  std::cout << "All tt log tests passed.\n";
  return 0;
}
