#include "tt/print.h"

#include <cstdarg>
#include <cstdio>

namespace tt {

// vfprintf rather than a stack buffer: console output has no length limit to
// enforce, and silently truncating a usage message or a tensor dump would be
// worse than a long line.
void print(const char* format, ...) {
  std::va_list arguments;
  va_start(arguments, format);
  std::vfprintf(stdout, format, arguments);
  va_end(arguments);
}

void println(const char* format, ...) {
  std::va_list arguments;
  va_start(arguments, format);
  std::vfprintf(stdout, format, arguments);
  va_end(arguments);
  std::fputc('\n', stdout);
  // A terminal line-buffers stdout anyway; this makes the ordering against the
  // log on stderr hold when stdout is a pipe or a file as well.
  std::fflush(stdout);
}

}  // namespace tt
