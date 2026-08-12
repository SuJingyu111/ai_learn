#pragma once

#include "common.h"
#include <vector>

TT_NAMESPACE_BEGIN

    class Shape
    {
    private:

        std::vector<int64_t> dimensions;

    public:
        Shape() = default;
        static Status Create(std::vector<int64_t> dimensions, Shape& out);

        uint32_t rank();
        int64_t  dim(uint32_t dimIdx);
        Status numel(int64_t& numel);
        void Print();
    };

TT_NAMESPACE_END
