#include "tt/shape.h"
#include "tt/print.h"

TT_NAMESPACE_BEGIN

Status Shape::Create(std::vector<int64_t> dimensions, Shape& out)
{
    for (int64_t dim : dimensions)
    {
        if (dim < 0)
        {
            return Status::EWRONGPARAM;
        }
    }
    out.dimensions = dimensions;
    return Status::SUCCESS;
}

uint32_t Shape::rank()
{
    return dimensions.size();
}

int64_t Shape::dim(uint32_t dimIdx)
{
    if (dimIdx >= dimensions.size())
    {
        return -1;
    }
    else
    {
        return dimensions[dimIdx];
    }
}

Status Shape::numel(int64_t& res)
{
    if (dimensions.empty())
    {
        res = 1;
        return Status::SUCCESS;
    }
    else
    {
        res = 1;
        for (int64_t dim : dimensions)
        {
            if (dim > 0 && INT64_MAX / dim < res)
            {
                return Status::EFAIL;
            }
            res *= dim;
        }
        return Status::SUCCESS;
    }
}

void Shape::Print()
{
    print("Shape=(");
    for (int i = 0; i < (int)dimensions.size() - 1; i++)
    {
        print("%llu ", dimensions[i]);
    }
    if (!dimensions.empty())
    {
        print("%llu", dimensions[dimensions.size() - 1]);
    }
    println(")");
}

TT_NAMESPACE_END
