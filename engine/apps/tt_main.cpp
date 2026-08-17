#include "tt/version.h"
#include "tt/common.h"
#include "tt/print.h"
#include "tt/log.h"
#include "tt/shape.h"
#include "tt/dtype.h"
#include <string_view>
#include <iostream>

using namespace std;

void shape_demo();
void dtype_demo();

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
    }
    else if (command == "shape-demo")
    {
        shape_demo();
    }
    else if (command == "dtype-demo")
    {
        dtype_demo();
    }
    else
    {
        tt::println("usage: tt <command> [args]");
        return 2;
    }

    return 0;
}

void shape_demo()
{
    std::vector<int64_t> vec = {};
    int64_t numel = 0;

    tt::Shape a;
    tt::Shape::Create(vec, a);
    a.Print();
    a.numel(numel);
    tt::println("rank=%d, numel=%llu", a.rank(), numel);
    tt::println("");

    vec.push_back(5);
    tt::Shape b;
    tt::Shape::Create(vec, b);
    b.Print();
    b.numel(numel);
    tt::println("rank=%d, numel=%llu", b.rank(), numel);
    tt::println("");

    vec.push_back(3);
    tt::Shape c;
    tt::Shape::Create(vec, c);
    c.Print();
    c.numel(numel);
    tt::println("rank=%d, numel=%llu", c.rank(), numel);
    tt::println("");

    vec.push_back(4);
    tt::Shape d;
    tt::Shape::Create(vec, d);
    d.Print();
    d.numel(numel);
    tt::println("rank=%d, numel=%llu", d.rank(), numel);
}

void dtype_demo()
{
    uint32_t nSize;
    string strName;

    tt::dtype_size(tt::Dtype::F32, nSize);
    tt::dtype_name(tt::Dtype::F32, strName);
    tt::println("%s, size=%d", strName.c_str(), nSize);

    tt::dtype_size(tt::Dtype::I64, nSize);
    tt::dtype_name(tt::Dtype::I64, strName);
    tt::println("%s, size=%d", strName.c_str(), nSize);

    tt::dtype_size(tt::Dtype::I32, nSize);
    tt::dtype_name(tt::Dtype::I32, strName);
    tt::println("%s, size=%d", strName.c_str(), nSize);

    tt::dtype_size(tt::Dtype::U8, nSize);
    tt::dtype_name(tt::Dtype::U8, strName);
    tt::println("%s, size=%d", strName.c_str(), nSize);
}
