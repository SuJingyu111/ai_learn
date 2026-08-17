#pragma once

#include "common.h"

TT_NAMESPACE_BEGIN

enum class Dtype
{
    F32 = 0,
    I64,
    I32,
    U8
};

inline Status dtype_size(Dtype eType, std::uint32_t& nSize)
{
    switch (eType)
    {
    case Dtype::F32:
        nSize = 4;
        break;
    case Dtype::I64:
        nSize = 8;
        break;
    case Dtype::I32:
        nSize = 4;
        break;
    case Dtype::U8:
        nSize = 1;
        break;
    default:
        return Status::EWRONGPARAM;
    }
    return Status::SUCCESS;
}

inline Status dtype_name(Dtype eType, std::string& strName)
{
    switch (eType)
    {
    case Dtype::F32:
        strName = "f32";
        break;
    case Dtype::I64:
        strName = "i64";
        break;
    case Dtype::I32:
        strName = "i32";
        break;
    case Dtype::U8:
        strName = "u8";
        break;
    default:
        return Status::EWRONGPARAM;
    }
    return Status::SUCCESS;
}

TT_NAMESPACE_END