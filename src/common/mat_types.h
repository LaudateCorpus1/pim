#pragma once

#include "common/vec_types.h"

// [row][column]
// [0][0] = a
// [0][1] = b
// [1][0] = c
// [1][1] = d
// [ a, b ]
// [ c, d ]
template<typename T>
struct mat2
{
    using row_t = vec2<T>;

    row_t x, y;

    inline row_t* begin() { return &x; }
    inline row_t* end() { return &x + 2; }
    inline const row_t* begin() const { return &x; }
    inline const row_t* end() const { return &x + 2; }
    inline row_t& operator[](i32 i) { DebugAssert((u32)i < 2u); return (&x)[i]; }
    inline const row_t& operator[](i32 i) const { DebugAssert((u32)i < 2u); return (&x)[i]; }
};

// [row][column]
// [0][0] = a
// [0][1] = b
// [0][2] = c
// [1][0] = d
// [1][1] = e
// [1][2] = f
// [2][0] = g
// [2][1] = h
// [2][2] = i
// [ a, b, c ]
// [ d, e, f ]
// [ g, h, i ]
template<typename T>
struct mat3
{
    using row_t = vec3<T>;

    row_t x, y, z;

    inline row_t* begin() { return &x; }
    inline row_t* end() { return &x + 3; }
    inline const row_t* begin() const { return &x; }
    inline const row_t* end() const { return &x + 3; }
    inline row_t& operator[](i32 i) { DebugAssert((u32)i < 3u); return (&x)[i]; }
    inline const row_t& operator[](i32 i) const { DebugAssert((u32)i < 3u); return (&x)[i]; }
};

// [row][column]
// [0][0] = a
// [0][1] = b
// [0][2] = c
// [0][3] = d
// [1][0] = e
// [1][1] = f
// [1][2] = g
// [1][3] = h
// [2][0] = i
// [2][1] = j
// [2][2] = k
// [2][3] = l
// [3][0] = m
// [3][1] = n
// [3][2] = o
// [3][3] = p
// [ a, b, c, d ]
// [ e, f, g, h ]
// [ i, j, k, l ]
// [ m, n, o, p ]
template<typename T>
struct mat4
{
    using row_t = vec4<T>;

    row_t x, y, z, w;

    inline row_t* begin() { return &x; }
    inline row_t* end() { return &x + 4; }
    inline const row_t* begin() const { return &x; }
    inline const row_t* end() const { return &x + 4; }
    inline row_t& operator[](i32 i) { DebugAssert((u32)i < 4u); return (&x)[i]; }
    inline const row_t& operator[](i32 i) const { DebugAssert((u32)i < 4u); return (&x)[i]; }
};

using float2x2 = mat2<f32>;
using float3x3 = mat3<f32>;
using float4x4 = mat4<f32>;

using int2x2 = mat2<i32>;
using int3x3 = mat3<i32>;
using int4x4 = mat4<i32>;
