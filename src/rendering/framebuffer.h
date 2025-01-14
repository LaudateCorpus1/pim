#pragma once

#include "common/macro.h"
#include "math/types.h"

PIM_C_BEGIN

typedef struct FrameBuf_s
{
    i32 width;
    i32 height;
    float4* pim_noalias light; // scene luminance, linear AP1
    R16G16B16A16_t* pim_noalias color; // display value, PQ encoded Rec2020
} FrameBuf;

void FrameBuf_New(FrameBuf* buf, i32 width, i32 height);
void FrameBuf_Del(FrameBuf* buf);
void FrameBuf_Reserve(FrameBuf* buf, i32 width, i32 height);

PIM_C_END
