#pragma once

// Console output: what the program was asked to produce. This is not logging,
// and the two must not be confused -- see log.h for diagnostics.
//
// Contract
//   - Always goes to stdout. The log level does not gate it and
//     set_log_output() does not move it. `tt version` prints the version even
//     under TT_LOG_LEVEL=off, because that output is the point of the command.
//   - No prefix, no truncation: what you format is what appears.
//   - print() leaves the stream buffered, println() flushes. Compose a line or
//     a loop with print() and end it with println(): output then stays ordered
//     against the log on stderr without one flush per element.
//   - Shell redirection applies, by design: `tt dump > x.tt` writes the data to
//     the file while the log stays on the terminal.
//   - Meant for the single thread that owns the command's output. println()
//     emits the text and the newline as two stdio calls, so concurrent callers
//     can interleave; concurrent code should be logging, not printing.

#include "tt/common.h"

namespace tt {

void print(const char* format, ...) TT_PRINTF_FORMAT(1, 2);

void println(const char* format, ...) TT_PRINTF_FORMAT(1, 2);

}  // namespace tt
