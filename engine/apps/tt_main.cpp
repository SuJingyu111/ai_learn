#include "tt/version.h"
#include "tt/common.h"
#include "tt/print.h"
#include "tt/log.h"
#include <string_view>

int main(int argc, char* argv[])
{
    // Usage goes to stdout with a non-zero exit code: the later steps read the
    // exit code to tell "the command ran" from "the command was not understood".
    if (argc == 1)
    {
        tt::println("usage: tt <command> [args]");
        return 2;
    }

    const std::string_view command(argv[1]);
    if (command == "version" || command == "--version" || command == "-v")
    {
        tt::println("tt %s", tt::kVersion);
        return 0;
    }

    tt::println("usage: tt <command> [args]");
    return 2;
}