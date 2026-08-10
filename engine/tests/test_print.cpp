// print.h promises console output that logging cannot move or silence. Both
// halves of that need a real process to check: stdout is captured through a
// pipe, and the child runs with TT_LOG_LEVEL=off and a redirected log sink so
// a regression that routed print() through the logger would show up as missing
// or polluted output.

#include "tt/print.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#include "tt/log.h"

namespace {

void check(bool condition, const std::string& label) {
  if (!condition) {
    throw std::runtime_error(label);
  }
}

constexpr const char* kExpectedStdout = "ab2!\nsecond line\n100% done\n";

// Child mode: everything here is hostile to console output, and none of it may
// affect stdout.
int run_child() {
  std::FILE* sink = std::tmpfile();
  if (sink == nullptr) {
    return 1;
  }
  tt::set_log_output(sink);          // logs go to a file, not the console
  tt::set_log_level(tt::LogLevel::kSilent);  // and are silenced on top of that

  tt::print("a");
  tt::print("b%d", 2);
  tt::println("!");
  tt::println("second line");
  tt::println("100%% done");

  TT_LOGE("print", "this line must not reach stdout");
  std::fclose(sink);
  return 0;
}

std::string run_and_capture_stdout(const std::string& command) {
  std::FILE* pipe = popen(command.c_str(), "r");
  check(pipe != nullptr, "popen");
  std::string captured;
  char buffer[256];
  while (std::fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    captured += buffer;
  }
  const int status = pclose(pipe);
  check(status == 0, "child exited with status " + std::to_string(status));
  return captured;
}

void test_print_reaches_stdout_despite_silent_log(const char* executable) {
  // stderr is discarded so only stdout can satisfy the comparison.
  const std::string command =
      std::string("TT_LOG_LEVEL=off \"") + executable + "\" --child 2>/dev/null";
  const std::string captured = run_and_capture_stdout(command);
  check(captured == kExpectedStdout,
        "stdout was '" + captured + "', expected '" + std::string(kExpectedStdout) + "'");
}

// The distinguishing property against log.h: no level, time, tag or location.
void test_output_carries_no_log_decoration(const char* executable) {
  const std::string command =
      std::string("TT_LOG_LEVEL=verbose \"") + executable + "\" --child 2>/dev/null";
  const std::string captured = run_and_capture_stdout(command);
  check(captured == kExpectedStdout, "verbose logging changed stdout: '" + captured + "'");
  check(captured.find("test_print.cpp") == std::string::npos, "stdout carries a location");
  check(captured.find(':') == std::string::npos, "stdout carries a timestamp or prefix");
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc == 2 && std::strcmp(argv[1], "--child") == 0) {
    return run_child();
  }
  try {
    check(argc > 0 && argv[0] != nullptr, "argv[0] unavailable");
    test_print_reaches_stdout_despite_silent_log(argv[0]);
    test_output_carries_no_log_decoration(argv[0]);
  } catch (const std::exception& error) {
    std::cerr << "FAILED: " << error.what() << '\n';
    return 1;
  }
  std::cout << "All tt print tests passed.\n";
  return 0;
}
