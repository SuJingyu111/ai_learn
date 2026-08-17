#pragma once

#include <vector>

#define TT_NAMESPACE_BEGIN namespace tt {
#define TT_NAMESPACE_END }

// Compiler plumbing shared by the engine's headers. Nothing here may pull in a
// dependency or declare anything with linkage.

// Lets the compiler check a printf-style call against its format string. Under
// -Werror a mismatched argument becomes a build failure instead of a garbage
// log line. The indices are 1-based positions in the parameter list.
#if defined(__GNUC__) || defined(__clang__)
#define TT_PRINTF_FORMAT(format_index, first_argument_index) \
  __attribute__((format(printf, format_index, first_argument_index)))
#else
#define TT_PRINTF_FORMAT(format_index, first_argument_index)
#endif

enum class Status
{
  SUCCESS     = 0,
  EFAIL       = 1,
  EWRONGPARAM = 2,
  EFATAL      = 3
};
