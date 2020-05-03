#pragma once

#include "math/types.h"

PIM_C_BEGIN

#include "math/scalar.h"
#include "common/random.h"

static const float4 f4_0 = { 0.0f, 0.0f, 0.0f, 0.0f };
static const float4 f4_1 = { 1.0f, 1.0f, 1.0f, 1.0f };
static const float4 f4_2 = { 2.0f, 2.0f, 2.0f, 2.0f };
static const float4 f4_rcp2 = { 0.5f, 0.5f, 0.5f, 0.5f };
static const float4 f4_rcp3 = { 0.33333333f, 0.33333333f, 0.33333333f, 0.33333333f };

pim_inline float4 VEC_CALL f4_v(float x, float y, float z, float w)
{
    float4 vec = { x, y, z, w };
    return vec;
}

pim_inline float4 VEC_CALL f4_s(float s)
{
    float4 vec = { s, s, s, s };
    return vec;
}

pim_inline float4 VEC_CALL f4_load(const float* src)
{
    float4 vec = { src[0], src[1], src[2], src[3] };
    return vec;
}

pim_inline void VEC_CALL f4_store(float4 src, float* dst)
{
    dst[0] = src.x;
    dst[1] = src.y;
    dst[2] = src.z;
    dst[3] = src.w;
}

pim_inline float4 VEC_CALL f4_zxy(float4 v)
{
    float4 vec = { v.z, v.x, v.y, v.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_yzx(float4 v)
{
    float4 vec = { v.y, v.z, v.x, v.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_neg(float4 v)
{
    float4 vec = { -v.x, -v.y, -v.z, -v.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_rcp(float4 v)
{
    float4 vec = { 1.0f / v.x, 1.0f / v.y, 1.0f / v.z, 1.0f / v.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_add(float4 lhs, float4 rhs)
{
    float4 vec = { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_addvs(float4 lhs, float rhs)
{
    float4 vec = { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs };
    return vec;
}

pim_inline float4 VEC_CALL f4_addsv(float lhs, float4 rhs)
{
    float4 vec = { lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_sub(float4 lhs, float4 rhs)
{
    float4 vec = { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_subvs(float4 lhs, float rhs)
{
    float4 vec = { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs };
    return vec;
}

pim_inline float4 VEC_CALL f4_subsv(float lhs, float4 rhs)
{
    float4 vec = { lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_mul(float4 lhs, float4 rhs)
{
    float4 vec = { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_mulvs(float4 lhs, float rhs)
{
    float4 vec = { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
    return vec;
}

pim_inline float4 VEC_CALL f4_mulsv(float lhs, float4 rhs)
{
    float4 vec = { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_div(float4 lhs, float4 rhs)
{
    float4 vec = { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_divvs(float4 lhs, float rhs)
{
    return f4_mulvs(lhs, 1.0f / rhs);
}

pim_inline float4 VEC_CALL f4_divsv(float lhs, float4 rhs)
{
    float4 vec = { lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w };
    return vec;
}

pim_inline float4 VEC_CALL f4_eq(float4 lhs, float4 rhs)
{
    float4 vec =
    {
        lhs.x == rhs.x ? 1.0f : 0.0f,
        lhs.y == rhs.y ? 1.0f : 0.0f,
        lhs.z == rhs.z ? 1.0f : 0.0f,
        lhs.w == rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_eqvs(float4 lhs, float rhs)
{
    float4 vec =
    {
        lhs.x == rhs ? 1.0f : 0.0f,
        lhs.y == rhs ? 1.0f : 0.0f,
        lhs.z == rhs ? 1.0f : 0.0f,
        lhs.w == rhs ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_eqsv(float lhs, float4 rhs)
{
    float4 vec =
    {
        lhs == rhs.x ? 1.0f : 0.0f,
        lhs == rhs.y ? 1.0f : 0.0f,
        lhs == rhs.z ? 1.0f : 0.0f,
        lhs == rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_neq(float4 lhs, float4 rhs)
{
    float4 vec =
    {
        lhs.x != rhs.x ? 1.0f : 0.0f,
        lhs.y != rhs.y ? 1.0f : 0.0f,
        lhs.z != rhs.z ? 1.0f : 0.0f,
        lhs.w != rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_neqvs(float4 lhs, float rhs)
{
    float4 vec =
    {
        lhs.x != rhs ? 1.0f : 0.0f,
        lhs.y != rhs ? 1.0f : 0.0f,
        lhs.z != rhs ? 1.0f : 0.0f,
        lhs.w != rhs ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_neqsv(float lhs, float4 rhs)
{
    float4 vec =
    {
        lhs != rhs.x ? 1.0f : 0.0f,
        lhs != rhs.y ? 1.0f : 0.0f,
        lhs != rhs.z ? 1.0f : 0.0f,
        lhs != rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_lt(float4 lhs, float4 rhs)
{
    float4 vec =
    {
        lhs.x < rhs.x ? 1.0f : 0.0f,
        lhs.y < rhs.y ? 1.0f : 0.0f,
        lhs.z < rhs.z ? 1.0f : 0.0f,
        lhs.w < rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_ltvs(float4 lhs, float rhs)
{
    float4 vec =
    {
        lhs.x < rhs ? 1.0f : 0.0f,
        lhs.y < rhs ? 1.0f : 0.0f,
        lhs.z < rhs ? 1.0f : 0.0f,
        lhs.w < rhs ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_ltsv(float lhs, float4 rhs)
{
    float4 vec =
    {
        lhs < rhs.x ? 1.0f : 0.0f,
        lhs < rhs.y ? 1.0f : 0.0f,
        lhs < rhs.z ? 1.0f : 0.0f,
        lhs < rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_gt(float4 lhs, float4 rhs)
{
    float4 vec =
    {
        lhs.x > rhs.x ? 1.0f : 0.0f,
        lhs.y > rhs.y ? 1.0f : 0.0f,
        lhs.z > rhs.z ? 1.0f : 0.0f,
        lhs.w > rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_gtvs(float4 lhs, float rhs)
{
    float4 vec =
    {
        lhs.x > rhs ? 1.0f : 0.0f,
        lhs.y > rhs ? 1.0f : 0.0f,
        lhs.z > rhs ? 1.0f : 0.0f,
        lhs.w > rhs ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_gtsv(float lhs, float4 rhs)
{
    float4 vec =
    {
        lhs > rhs.x ? 1.0f : 0.0f,
        lhs > rhs.y ? 1.0f : 0.0f,
        lhs > rhs.z ? 1.0f : 0.0f,
        lhs > rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_lteq(float4 lhs, float4 rhs)
{
    float4 vec =
    {
        lhs.x <= rhs.x ? 1.0f : 0.0f,
        lhs.y <= rhs.y ? 1.0f : 0.0f,
        lhs.z <= rhs.z ? 1.0f : 0.0f,
        lhs.w <= rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_lteqvs(float4 lhs, float rhs)
{
    float4 vec =
    {
        lhs.x <= rhs ? 1.0f : 0.0f,
        lhs.y <= rhs ? 1.0f : 0.0f,
        lhs.z <= rhs ? 1.0f : 0.0f,
        lhs.w <= rhs ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_lteqsv(float lhs, float4 rhs)
{
    float4 vec =
    {
        lhs <= rhs.x ? 1.0f : 0.0f,
        lhs <= rhs.y ? 1.0f : 0.0f,
        lhs <= rhs.z ? 1.0f : 0.0f,
        lhs <= rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_gteq(float4 lhs, float4 rhs)
{
    float4 vec =
    {
        lhs.x >= rhs.x ? 1.0f : 0.0f,
        lhs.y >= rhs.y ? 1.0f : 0.0f,
        lhs.z >= rhs.z ? 1.0f : 0.0f,
        lhs.w >= rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_gteqvs(float4 lhs, float rhs)
{
    float4 vec =
    {
        lhs.x >= rhs ? 1.0f : 0.0f,
        lhs.y >= rhs ? 1.0f : 0.0f,
        lhs.z >= rhs ? 1.0f : 0.0f,
        lhs.w >= rhs ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_gteqsv(float lhs, float4 rhs)
{
    float4 vec =
    {
        lhs >= rhs.x ? 1.0f : 0.0f,
        lhs >= rhs.y ? 1.0f : 0.0f,
        lhs >= rhs.z ? 1.0f : 0.0f,
        lhs >= rhs.w ? 1.0f : 0.0f,
    };
    return vec;
}

pim_inline float VEC_CALL f4_sum(float4 v)
{
    return v.x + v.y + v.z + v.w;
}

pim_inline float VEC_CALL f4_sum3(float4 v)
{
    return v.x + v.y + v.z;
}

pim_inline bool VEC_CALL f4_any(float4 b)
{
    return f4_sum(b) != 0.0f;
}

pim_inline bool VEC_CALL f4_all(float4 b)
{
    return f4_sum(b) == 4.0f;
}

pim_inline float4 VEC_CALL f4_not(float4 b)
{
    return f4_subsv(1.0f, b);
}

pim_inline float4 VEC_CALL f4_min(float4 a, float4 b)
{
    float4 vec =
    {
        f1_min(a.x, b.x),
        f1_min(a.y, b.y),
        f1_min(a.z, b.z),
        f1_min(a.w, b.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_minvs(float4 a, float b)
{
    float4 vec =
    {
        f1_min(a.x, b),
        f1_min(a.y, b),
        f1_min(a.z, b),
        f1_min(a.w, b),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_minsv(float a, float4 b)
{
    float4 vec =
    {
        f1_min(a, b.x),
        f1_min(a, b.y),
        f1_min(a, b.z),
        f1_min(a, b.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_max(float4 a, float4 b)
{
    float4 vec =
    {
        f1_max(a.x, b.x),
        f1_max(a.y, b.y),
        f1_max(a.z, b.z),
        f1_max(a.w, b.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_maxvs(float4 a, float b)
{
    float4 vec =
    {
        f1_max(a.x, b),
        f1_max(a.y, b),
        f1_max(a.z, b),
        f1_max(a.w, b),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_maxsv(float a, float4 b)
{
    float4 vec =
    {
        f1_max(a, b.x),
        f1_max(a, b.y),
        f1_max(a, b.z),
        f1_max(a, b.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_select(float4 a, float4 b, float4 t)
{
    float4 c =
    {
        t.x != 0.0f ? b.x : a.x,
        t.y != 0.0f ? b.y : a.y,
        t.z != 0.0f ? b.z : a.z,
        t.w != 0.0f ? b.w : a.w,
    };
    return c;
}

pim_inline float4 VEC_CALL f4_selectvs(float4 a, float4 b, float t)
{
    float4 c =
    {
        t != 0.0f ? b.x : a.x,
        t != 0.0f ? b.y : a.y,
        t != 0.0f ? b.z : a.z,
        t != 0.0f ? b.w : a.w,
    };
    return c;
}

pim_inline float4 VEC_CALL f4_selectsv(float a, float b, float4 t)
{
    float4 c =
    {
        t.x != 0.0f ? b : a,
        t.y != 0.0f ? b : a,
        t.z != 0.0f ? b : a,
        t.w != 0.0f ? b : a,
    };
    return c;
}

pim_inline float VEC_CALL f4_hmin(float4 v)
{
    float a = f1_min(v.x, v.y);
    float b = f1_min(v.z, v.w);
    return f1_min(a, b);
}

pim_inline float VEC_CALL f4_hmax(float4 v)
{
    float a = f1_max(v.x, v.y);
    float b = f1_max(v.z, v.w);
    return f1_max(a, b);
}

pim_inline float4 VEC_CALL f4_clamp(float4 x, float4 lo, float4 hi)
{
    return f4_min(f4_max(x, lo), hi);
}

pim_inline float4 VEC_CALL f4_clampvs(float4 x, float lo, float hi)
{
    return f4_minvs(f4_maxvs(x, lo), hi);
}

pim_inline float4 VEC_CALL f4_clampsv(float x, float4 lo, float4 hi)
{
    return f4_min(f4_maxsv(x, lo), hi);
}

pim_inline float VEC_CALL f4_dot(float4 a, float4 b)
{
    return f4_sum(f4_mul(a, b));
}

pim_inline float VEC_CALL f4_dot3(float4 a, float4 b)
{
    return f4_sum3(f4_mul(a, b));
}

pim_inline float4 VEC_CALL f4_cross3(float4 a, float4 b)
{
    return f4_zxy(
        f4_sub(
            f4_mul(f4_zxy(a), b),
            f4_mul(a, f4_zxy(b))));
}

pim_inline float VEC_CALL f4_length(float4 x)
{
    return sqrtf(f4_dot(x, x));
}

pim_inline float VEC_CALL f4_length3(float4 x)
{
    return sqrtf(f4_dot3(x, x));
}

pim_inline float4 VEC_CALL f4_normalize(float4 x)
{
    return f4_mul(x, f4_s(1.0f / f4_length(x)));
}

pim_inline float4 VEC_CALL f4_normalize3(float4 x)
{
    return f4_mul(x, f4_s(1.0f / f4_length3(x)));
}

pim_inline float VEC_CALL f4_distance(float4 a, float4 b)
{
    return f4_length(f4_sub(a, b));
}

pim_inline float VEC_CALL f4_lengthsq(float4 x)
{
    return f4_dot(x, x);
}

pim_inline float VEC_CALL f4_distancesq(float4 a, float4 b)
{
    return f4_lengthsq(f4_sub(a, b));
}

pim_inline float4 VEC_CALL f4_lerp(float4 a, float4 b, float t)
{
    return f4_add(a, f4_mulvs(f4_sub(b, a), t));
}

pim_inline float4 VEC_CALL f4_lerpsv(float a, float b, float4 t)
{
    return f4_addsv(a, f4_mulsv(b - a, t));
}

pim_inline float4 VEC_CALL f4_saturate(float4 a)
{
    return f4_clampvs(a, 0.0f, 1.0f);
}

pim_inline float4 VEC_CALL f4_step(float4 a, float4 b)
{
    return f4_selectsv(0.0f, 1.0f, f4_gteq(a, b));
}

pim_inline float4 VEC_CALL f4_smoothstep(float4 a, float4 b, float4 x)
{
    float4 t = f4_saturate(f4_div(f4_sub(x, a), f4_sub(b, a)));
    float4 s = f4_subsv(3.0f, f4_mulsv(2.0f, t));
    return f4_mul(f4_mul(t, t), s);
}

pim_inline float4 VEC_CALL f4_smoothstepvs(float4 a, float4 b, float x)
{
    float4 t = f4_saturate(f4_div(f4_subsv(x, a), f4_sub(b, a)));
    float4 s = f4_subsv(3.0f, f4_mulsv(2.0f, t));
    return f4_mul(f4_mul(t, t), s);
}

pim_inline float4 VEC_CALL f4_smoothstepsv(float a, float b, float4 x)
{
    float4 t = f4_saturate(f4_divvs(f4_subvs(x, a), b - a));
    float4 s = f4_subsv(3.0f, f4_mulsv(2.0f, t));
    return f4_mul(f4_mul(t, t), s);
}

pim_inline float4 VEC_CALL f4_reflect(float4 i, float4 n)
{
    float4 nidn = f4_mulvs(n, f4_dot(i, n));
    return f4_sub(i, f4_mulsv(2.0f, nidn));
}

pim_inline float4 VEC_CALL f4_refract(float4 i, float4 n, float ior)
{
    float ndi = f4_dot(n, i);
    float ndi2 = ndi * ndi;
    float ior2 = ior * ior;
    float k = 1.0f - (ior2 * (1.0f - ndi2));
    float l = ior * ndi + sqrtf(k);
    float4 m = f4_sub(f4_mulsv(ior, i), f4_mulsv(l, n));
    return f4_mulvs(m, k >= 0.0f ? 1.0f : 0.0f);
}

pim_inline float4 VEC_CALL f4_sqrt(float4 v)
{
    float4 vec = { sqrtf(v.x), sqrtf(v.y), sqrtf(v.z), sqrtf(v.w) };
    return vec;
}

pim_inline float4 VEC_CALL f4_abs(float4 v)
{
    float4 vec = { fabsf(v.x), fabsf(v.y), fabsf(v.z), fabsf(v.w) };
    return vec;
}

pim_inline float4 VEC_CALL f4_pow(float4 v, float4 e)
{
    float4 vec =
    {
        powf(v.x, e.x),
        powf(v.y, e.y),
        powf(v.z, e.z),
        powf(v.w, e.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_powvs(float4 v, float e)
{
    float4 vec =
    {
        powf(v.x, e),
        powf(v.y, e),
        powf(v.z, e),
        powf(v.w, e),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_powsv(float v, float4 e)
{
    float4 vec =
    {
        powf(v, e.x),
        powf(v, e.y),
        powf(v, e.z),
        powf(v, e.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_exp(float4 v)
{
    float4 vec =
    {
        expf(v.x),
        expf(v.y),
        expf(v.z),
        expf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_log(float4 v)
{
    float4 vec =
    {
        logf(v.x),
        logf(v.y),
        logf(v.z),
        logf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_sin(float4 v)
{
    float4 vec =
    {
        sinf(v.x),
        sinf(v.y),
        sinf(v.z),
        sinf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_cos(float4 v)
{
    float4 vec =
    {
        cosf(v.x),
        cosf(v.y),
        cosf(v.z),
        cosf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_tan(float4 v)
{
    float4 vec =
    {
        tanf(v.x),
        tanf(v.y),
        tanf(v.z),
        tanf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_asin(float4 v)
{
    float4 vec =
    {
        asinf(v.x),
        asinf(v.y),
        asinf(v.z),
        asinf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_acos(float4 v)
{
    float4 vec =
    {
        acosf(v.x),
        acosf(v.y),
        acosf(v.z),
        acosf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_atan(float4 v)
{
    float4 vec =
    {
        atanf(v.x),
        atanf(v.y),
        atanf(v.z),
        atanf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_floor(float4 v)
{
    float4 vec =
    {
        floorf(v.x),
        floorf(v.y),
        floorf(v.z),
        floorf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_ceil(float4 v)
{
    float4 vec =
    {
        ceilf(v.x),
        ceilf(v.y),
        ceilf(v.z),
        ceilf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_trunc(float4 v)
{
    float4 vec =
    {
        truncf(v.x),
        truncf(v.y),
        truncf(v.z),
        truncf(v.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_frac(float4 v)
{
    return f4_sub(v, f4_floor(v));
}

pim_inline float4 VEC_CALL f4_fmod(float4 a, float4 b)
{
    float4 vec =
    {
        fmodf(a.x, b.x),
        fmodf(a.y, b.y),
        fmodf(a.z, b.z),
        fmodf(a.w, b.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_fmodvs(float4 a, float b)
{
    float4 vec =
    {
        fmodf(a.x, b),
        fmodf(a.y, b),
        fmodf(a.z, b),
        fmodf(a.w, b),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_fmodsv(float a, float4 b)
{
    float4 vec =
    {
        fmodf(a, b.x),
        fmodf(a, b.y),
        fmodf(a, b.z),
        fmodf(a, b.w),
    };
    return vec;
}

pim_inline float4 VEC_CALL f4_rad(float4 x)
{
    return f4_mulvs(x, kRadiansPerDegree);
}

pim_inline float4 VEC_CALL f4_deg(float4 x)
{
    return f4_mulvs(x, kDegreesPerRadian);
}

pim_inline float4 VEC_CALL f4_blend(float4 a, float4 b, float4 c, float3 wuv)
{
    float4 p = f4_mulvs(a, wuv.x);
    p = f4_add(p, f4_mulvs(b, wuv.y));
    p = f4_add(p, f4_mulvs(c, wuv.z));
    return p;
}

pim_inline float4 VEC_CALL f4_unorm(float4 s)
{
    return f4_addvs(f4_mulvs(s, 0.5f), 0.5f);
}

pim_inline float4 VEC_CALL f4_snorm(float4 u)
{
    return f4_subvs(f4_mulvs(u, 2.0f), 1.0f);
}

pim_inline float3 VEC_CALL f4_f3(float4 v)
{
    return (float3) { v.x, v.y, v.z };
}

pim_inline float2 VEC_CALL f4_f2(float4 v)
{
    return (float2) { v.x, v.y };
}

pim_inline u16 VEC_CALL f4_rgb5a1(float4 v)
{
    v = f4_mulvs(v, 31.0f);
    u16 r = (u16)v.x;
    u16 g = (u16)v.y;
    u16 b = (u16)v.z;
    u16 c = (r << 11) | (g << 6) | (b << 1) | 1;
    return c;
}

pim_inline u32 VEC_CALL f4_rgba8(float4 v)
{
    v = f4_mulvs(v, 255.0f);
    u32 r = (u32)v.x;
    u32 g = (u32)v.y;
    u32 b = (u32)v.z;
    u32 a = (u32)v.w;
    u32 c = (a << 24) | (b << 16) | (g << 8) | r;
    return c;
}

pim_inline float4 VEC_CALL rgba8_f4(u32 c)
{
    const float s = 1.0f / 255.0f;
    u32 r = c & 0xff;
    c >>= 8;
    u32 g = c & 0xff;
    c >>= 8;
    u32 b = c & 0xff;
    c >>= 8;
    u32 a = c & 0xff;
    return f4_v(r * s, g * s, b * s, a * s);
}

pim_inline float4 VEC_CALL f4_tosrgb(float4 lin)
{
    float4 srgb;
    srgb.x = f1_tosrgb(lin.x);
    srgb.y = f1_tosrgb(lin.y);
    srgb.z = f1_tosrgb(lin.z);
    srgb.w = f1_tosrgb(lin.w);
    return srgb;
}

pim_inline float4 VEC_CALL f4_tolinear(float4 srgb)
{
    float4 lin;
    lin.x = f1_tolinear(srgb.x);
    lin.y = f1_tolinear(srgb.y);
    lin.z = f1_tolinear(srgb.z);
    lin.w = f1_tolinear(srgb.w);
    return lin;
}

pim_inline float4 VEC_CALL f4_rand(prng_t* rng)
{
    return f4_v(prng_f32(rng), prng_f32(rng), prng_f32(rng), prng_f32(rng));
}

pim_inline float4 VEC_CALL f4_dither(prng_t* rng, float4 x)
{
    const float kDither = 1.0f / (1 << 8);
    return f4_lerp(x, f4_rand(rng), kDither);
}

pim_inline u32 VEC_CALL f4_color(float4 linear)
{
    return f4_rgba8(f4_tosrgb(linear));
}

pim_inline float4 VEC_CALL color_f4(u32 c)
{
    return f4_tolinear(rgba8_f4(c));
}

pim_inline float4 VEC_CALL tmap4_reinhard(float4 x)
{
    float4 y;
    y.x = tmap1_reinhard(x.x);
    y.y = tmap1_reinhard(x.y);
    y.z = tmap1_reinhard(x.z);
    y.w = tmap1_reinhard(x.w);
    return y;
}

pim_inline float4 VEC_CALL tmap4_aces(float4 x)
{
    float4 y;
    y.x = tmap1_aces(x.x);
    y.y = tmap1_aces(x.y);
    y.z = tmap1_aces(x.z);
    y.w = tmap1_aces(x.w);
    return y;
}

// note: outputs roughly gamma2.2
pim_inline float4 VEC_CALL tmap4_filmic(float4 x)
{
    float4 y;
    y.x = tmap1_filmic(x.x);
    y.y = tmap1_filmic(x.y);
    y.z = tmap1_filmic(x.z);
    y.w = tmap1_filmic(x.w);
    return y;
}

pim_inline float4 VEC_CALL tmap4_uchart2(float4 x)
{
    float4 y;
    y.x = tmap1_uchart2(x.x);
    y.y = tmap1_uchart2(x.y);
    y.z = tmap1_uchart2(x.z);
    y.w = tmap1_uchart2(x.w);
    return y;
}

PIM_C_END