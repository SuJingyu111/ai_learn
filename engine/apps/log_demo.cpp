// Shows the two ways the log level is set at run time: the TT_LOG_LEVEL
// environment variable read once at start-up, and set_log_level() from code.
//
//   ./build-cpu/log_demo                      # default: info and above
//   TT_LOG_LEVEL=debug ./build-cpu/log_demo   # debug and above
//   TT_LOG_LEVEL=off ./build-cpu/log_demo     # nothing
//   ./build-cpu/log_demo error                # same, chosen in code

#include <cstdio>
#include <cstdlib>

#include "tt/log.h"
#include "tt/print.h"

namespace {

constexpr const char* kTag = "demo";

void emit_one_of_each(const char* phase) {
  TT_LOGV(kTag, "%s: verbose", phase);
  TT_LOGD(kTag, "%s: debug", phase);
  TT_LOGI(kTag, "%s: info", phase);
  TT_LOGW(kTag, "%s: warn", phase);
  TT_LOGE(kTag, "%s: error", phase);
  TT_LOGF(kTag, "%s: fatal (a severity, not an abort)", phase);
}

}  // namespace

int main(int argc, char* argv[]) {
  if (argc > 2) {
    std::fprintf(stderr, "usage: log_demo [level]\n");
    return 2;
  }

  const char* source = std::getenv("TT_LOG_LEVEL") != nullptr ? "TT_LOG_LEVEL" : "default";
  // println() flushes, so this stays ahead of the log lines on stderr even
  // when stdout is a pipe rather than a terminal.
  tt::println("active level: %s (from %s)", tt::log_level_name(tt::log_level()), source);
  emit_one_of_each("startup");

  if (argc == 2) {
    tt::LogLevel level = tt::LogLevel::kInfo;
    if (!tt::parse_log_level(argv[1], &level)) {
      std::fprintf(stderr, "log_demo: '%s' is not a level\n", argv[1]);
      return 2;
    }
    tt::set_log_level(level);
    tt::println("\nactive level: %s (from set_log_level)",
                tt::log_level_name(tt::log_level()));
    emit_one_of_each("after set_log_level");
  }
  return 0;
}
