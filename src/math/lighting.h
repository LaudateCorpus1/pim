#pragma once

#include "math/types.h"
#include "math/float4_funcs.h"
#include "math/float2_funcs.h"
#include "math/area.h"
#include "rendering/sampler.h"

PIM_C_BEGIN

// References:
// ----------------------------------------------------------------------------
// [Burley12]: https://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
// [Karis13]: https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// [Karis14]: https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
// [Lagarde15]: https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf

// Legend:
// ----------------------------------------------------------------------------
// FRESNEL:
//   f0: fresnel reflectance at normal incidence
//   f90: fresnel reflectance at tangent incidence
// ROUGHNESS:
//   perceptually linear roughness: material.roughness
//   alpha, or a: pow(material.roughness, 2)
//   alpha_linear or a_lin: material.roughness
//   """
//     [Burley12]
//     For  roughness, we found that mapping alpha=roughness^2 results
//     in a more perceptually linear change in the roughness.
//     Without this remapping, very small and non-intuitive values were required
//     for matching shiny materials. Also, interpolating between a rough and
//     smooth material would always produce a rough result.
//   """

#define kMinDenom       (1.0f / (1<<10))
#define kMinLightDist   0.01f
#define kMinLightDistSq 0.001f
#define kMinAlpha       kMinDenom

void LightingSys_Init(void);
void LightingSys_Update(void);
void LightingSys_Shutdown(void);

extern BrdfLut g_BrdfLut;
BrdfLut BrdfLut_New(int2 size, i32 numSamples);
void BrdfLut_Del(BrdfLut* lut);
void BrdfLut_Update(BrdfLut* lut, i32 prevSamples, i32 newSamples);

// r: reflectance, integral of Fc*D*V*NoL
// g: visibility, integral of D*V*NoL
pim_inline float2 VEC_CALL BrdfLut_Sample(BrdfLut lut, float NoV, float alpha)
{
    return UvBilinearClamp_f2(lut.texels, lut.size, f2_v(NoV, alpha));
}

pim_inline float VEC_CALL BrdfAlpha(float roughness)
{
    return f1_max(roughness * roughness, kMinAlpha);
}

// Specular reflectance at normal incidence (N==L)
// """
//   [Burley12, Page 16, Section 5.5]
//   The constant, F0, represents the specular reflectance at normal incidence
//   and is achromatic for dielectrics and chromatic (tinted) for metals.
//   The actual value depends on the index of refraction.
// """
pim_inline float4 VEC_CALL F_0(float4 albedo, float metallic)
{
    return f4_lerpvs(f4_s(0.04f), albedo, metallic);
}

// any F0 lower than 0.02 can be assumed as a form of pre-baked occlusion
pim_inline float VEC_CALL F_90(float4 F0)
{
    return f1_sat(50.0f * f4_dot3(F0, f4_s(0.33f)));
}

// Specular 'F' term
// represents the ratio of reflected to refracted light
// cosTheta may be HoL or HoV (they are equivalent)
// """
//   [Burley12, Page 16, Section 5.5]
//   The  Fresnel  function  can  be  seen  as  interpolating  (non-linearly)
//   between  the  incident  specular reflectance and unity at grazing angles.
//   Note that the response becomes achromatic at grazing incidenceas all light
//   is reflected.
// """
pim_inline float4 VEC_CALL F_Schlick(float4 f0, float4 f90, float cosTheta)
{
    float t = 1.0f - cosTheta;
    float t5 = t * t * t * t * t;
    return f4_lerpvs(f0, f90, t5);
}

pim_inline float VEC_CALL F_Schlick1(float f0, float f90, float cosTheta)
{
    float t = 1.0f - cosTheta;
    float t5 = t * t * t * t * t;
    return f1_lerp(f0, f90, t5);
}

// aka 'Specular Color'
pim_inline float4 VEC_CALL F_SchlickEx(
    float4 albedo,
    float metallic,
    float cosTheta)
{
    float4 f0 = F_0(albedo, metallic);
    float4 f90 = f4_s(F_90(f0));
    return F_Schlick(f0, f90, cosTheta);
}

pim_inline float4 VEC_CALL DiffuseColor(
    float4 albedo,
    float metallic)
{
    return f4_mulvs(albedo, 1.0f - metallic);
}

// Specular 'D' term
// represents the normal distribution function
// GGX Trowbridge-Reitz approximation
// [Karis13, Page 3, Figure 3]
pim_inline float VEC_CALL D_GTR(float NoH, float alpha)
{
    float a2 = alpha * alpha;
    float f = f1_lerp(1.0f, a2, NoH * NoH);
    return a2 / f1_max(kEpsilon, f * f * kPi);
}

// Specular 'D' term with SphereNormalization applied
// [Karis13, Page 16, Equation 14]
pim_inline float VEC_CALL D_GTR_Sphere(
    float NoH,
    float alpha,
    float alphaPrime)
{
    // original normalization factor: 1 / (pi * a^2)
    // sphere normalization factor: (a / a')^2
    float a2 = alpha * alpha;
    float ap2 = alphaPrime * alphaPrime;
    float f = f1_lerp(1.0f, a2, NoH * NoH);
    return (a2 * ap2) / f1_max(kEpsilon, f * f);
}

// Specular 'V' term (G term / denominator)
// Correlated Smith
// represents the self shadowing and masking of microfacets in rough materials
// [Lagarde15, Page 12, Listing 2]
pim_inline float VEC_CALL V_SmithCorrelated(float NoL, float NoV, float alpha)
{
    float a2 = alpha * alpha;
    float v = NoL * sqrtf(a2 + (NoV - NoV * a2) * NoV);
    float l = NoV * sqrtf(a2 + (NoL - NoL * a2) * NoL);
    return 0.5f / f1_max(kEpsilon, v + l);
}

// Lambert diffuse brdf
// Energy conserving
// Only really valid for chalk
pim_inline float VEC_CALL Fd_Lambert()
{
    return 1.0f / kPi;
}

// Burley / Disney diffuse brdf
// Not energy conserving unless used on realistic material inputs (MERL dataset fit)
// [Burley12]
pim_inline float VEC_CALL Fd_Burley(
    float NoL,
    float NoV,
    float HoV,
    float roughness)
{
    float fd90 = 0.5f + 2.0f * HoV * HoV * roughness;
    float lightScatter = F_Schlick1(1.0f, fd90, NoL);
    float viewScatter = F_Schlick1(1.0f, fd90, NoV);
    return (lightScatter * viewScatter) / kPi;
}

pim_inline float4 VEC_CALL EnvBRDF(
    float4 f0,
    float NoV,
    float alpha)
{
    float2 dfg = BrdfLut_Sample(g_BrdfLut, NoV, alpha);
    float4 dfg1 = f4_mulvs(f4_inv(f0), dfg.x);
    float4 dfg2 = f4_mulvs(f0, dfg.y);
    return f4_add(dfg1, dfg2);
}

// https://advances.realtimerendering.com/s2018/Siggraph%202018%20HDRP%20talk_with%20notes.pdf#page=94
pim_inline float4 VEC_CALL GGXEnergyCompensation(
    float4 f0,
    float NoV,
    float alpha)
{
    float2 dfg = BrdfLut_Sample(g_BrdfLut, NoV, alpha);
    float t = (1.0f / dfg.y) - 1.0f;
    return f4_addvs(f4_mulvs(f0, t), 1.0f);
}

// multiply output by luminance
pim_inline float4 VEC_CALL DirectBRDF(
    float4 V,
    float4 L,
    float4 N,
    float4 albedo,
    float roughness,
    float metallic)
{
    float4 H = f4_normalize3(f4_add(V, L));
    float NoV = f4_dotsat(N, V);
    float NoH = f4_dotsat(N, H);
    float NoL = f4_dotsat(N, L);
    float HoV = f4_dotsat(H, V);

    float alpha = BrdfAlpha(roughness);
    float4 F = F_SchlickEx(albedo, metallic, HoV);
    float G = V_SmithCorrelated(NoL, NoV, alpha);
    float D = D_GTR(NoH, alpha);
    float4 Fr = f4_mulvs(F, D * G);
    Fr = f4_mul(Fr, GGXEnergyCompensation(F_0(albedo, metallic), NoV, alpha));

    float4 Fd = f4_mulvs(
        DiffuseColor(albedo, metallic),
        Fd_Burley(NoL, NoV, HoV, roughness));
    // diffuse term is scaled by fresnel refractance
    Fd = f4_mul(Fd, f4_inv(F));

    return f4_add(Fr, Fd);
}

pim_inline float4 VEC_CALL IndirectBRDF(
    float4 V,           // unit view vector pointing from surface to eye
    float4 N,           // unit normal vector pointing outward from surface
    float4 diffuseGI,   // diffuse brdf scene irradiance
    float4 specularGI,  // specular brdf filtered scene luminance
    float4 albedo,      // surface color
    float roughness,    // surface roughness
    float metallic,     // surface metalness
    float ao)           // 1 - ambient occlusion (affects gi only)
{
    float alpha = BrdfAlpha(roughness);
    float NoV = f4_dotsat(N, V);

    float4 F = F_SchlickEx(albedo, metallic, NoV);
    float4 Fr = EnvBRDF(F, NoV, roughness);
    Fr = f4_mul(Fr, specularGI);

    float4 Fd = f4_mul(DiffuseColor(albedo, metallic), diffuseGI);
    // diffuse term is scaled by fresnel refractance
    Fd = f4_mul(Fd, f4_inv(F));
    return f4_add(Fr, Fd);
}

// [Lagarde15]
pim_inline float VEC_CALL SmoothDistanceAtt(
    float distance, // distance between surface and light
    float attRadius) // attenuation radius
{
    float d2 = distance * distance;
    float f1 = d2 / f1_max(1.0f, attRadius * attRadius);
    float f2 = f1_saturate(1.0f - f1 * f1);
    return f2 * f2;
}

pim_inline float VEC_CALL PowerToAttRadius(float power, float thresh)
{
    return sqrtf(power / thresh);
}

pim_inline float VEC_CALL DistanceAtt(float distance)
{
    return 1.0f / f1_max(kMinLightDistSq, distance * distance);
}

// [Lagarde15]
pim_inline float VEC_CALL AngleAtt(
    float4 L, // normalized light vector
    float4 Ldir, // spotlight direction
    float cosInner, // cosine of inner angle
    float cosOuter, // cosine of outer angle
    float angleScale) // width of inner to outer transition
{
    float scale = 1.0f / (cosInner - cosOuter);
    float offset = -cosOuter * angleScale;
    float cd = f4_dot3(Ldir, L);
    float att = f1_saturate(cd * scale + offset);
    return att * att;
}

// [Lagarde15]
pim_inline float4 VEC_CALL EvalPointLight(
    float4 V,
    float4 P,
    float4 N,
    float4 albedo,
    float roughness,
    float metallic,
    float4 lightPos,    // w: attenuation radius
    float4 lightColor)  // w: actual radius
{
    float4 L = f4_sub(lightPos, P);
    if (f4_dot3(L, N) <= 0.0f)
    {
        return f4_0;
    }
    float distance = f4_length3(L);
    L = f4_divvs(L, distance);
    distance = f1_max(kMinLightDist, distance);

    float NoL = f4_dotsat(N, L);
    float distAtt = DistanceAtt(distance);
    float windowing = SmoothDistanceAtt(distance, lightPos.w);
    float att = NoL * distAtt * windowing;
    if (att < kEpsilon)
    {
        return f4_0;
    }
    float4 brdf = DirectBRDF(V, L, N, albedo, roughness, metallic);
    return f4_mul(brdf, f4_mulvs(lightColor, att));
}

// [Lagarde15]
pim_inline float VEC_CALL SphereLumensToNits(float lumens, float radius)
{
    return lumens / (SphereArea(radius) * kPi);
}

// [Lagarde15]
pim_inline float VEC_CALL DiskLumensToNits(float lumens, float radius)
{
    return lumens / (DiskArea(radius) * kPi);
}

// [Lagarde15]
pim_inline float VEC_CALL TubeLumensToNits(
    float lumens,
    float radius,
    float width)
{
    return lumens / (TubeArea(radius, width) * kPi);
}

// [Lagarde15]
pim_inline float VEC_CALL RectLumensToNits(
    float lumens,
    float width,
    float height)
{
    return lumens / (RectArea(width, height) * kPi);
}

// [Lagarde15]
pim_inline float VEC_CALL SphereSinSigmaSq(float radius, float distance)
{
    radius = f1_max(kMinLightDist, radius);
    distance = f1_max(radius, distance);

    float r2 = radius * radius;
    float d2 = distance * distance;
    float sinSigmaSq = r2 / d2;
    sinSigmaSq = f1_min(sinSigmaSq, 1.0f - kMinDenom);
    return sinSigmaSq;
}

// [Lagarde15]
pim_inline float VEC_CALL DiskSinSigmaSq(float radius, float distance)
{
    radius = f1_max(kMinLightDist, radius);
    distance = f1_max(radius, distance);

    float r2 = radius * radius;
    float d2 = distance * distance;
    float sinSigmaSq = r2 / (r2 + d2);
    sinSigmaSq = f1_min(sinSigmaSq, 1.0f - kMinDenom);
    return sinSigmaSq;
}

// [Lagarde15, Page 47, Listing 10]
pim_inline float VEC_CALL SphereDiskIlluminance(
    float cosTheta,
    float sinSigmaSqr)
{
    cosTheta = f1_clamp(cosTheta, -0.9999f, 0.9999f);

    float c2 = cosTheta * cosTheta;

    float illuminance = 0.0f;
    if (c2 > sinSigmaSqr)
    {
        illuminance = kPi * sinSigmaSqr * f1_sat(cosTheta);
    }
    else
    {
        float sinTheta = sqrtf(1.0f - c2);
        float x = sqrtf(1.0f / sinSigmaSqr - 1.0f);
        float y = -x * (cosTheta / sinTheta);
        float sinThetaSqrtY = sinTheta * sqrtf(1.0f - y * y);

        float t1 = (cosTheta * acosf(y) - x * sinThetaSqrtY) * sinSigmaSqr;
        float t2 = atanf(sinThetaSqrtY / x);
        illuminance = t1 + t2;
    }

    return f1_max(illuminance, 0.0f);
}

// [Karis13, Page 14, Equation 10]
pim_inline float VEC_CALL AlphaPrime(
    float alpha,
    float radius,
    float distance)
{
    return f1_saturate(alpha + (radius / (2.0f * distance)));
}

// [Lagarde15]
pim_inline float4 VEC_CALL SpecularDominantDir(
    float4 N,
    float4 R,
    float alpha)
{
    return f4_normalize3(f4_lerpvs(R, N, alpha));
}

// [Karis13, page 15, equation 11]
pim_inline float4 VEC_CALL SphereRepresentativePoint(
    float4 R,
    float4 N,
    float4 L0, // unnormalized vector from surface to center of sphere
    float radius,
    float alpha)
{
    //R = SpecularDominantDir(N, R, alpha);
    float4 c2r = f4_sub(f4_mulvs(R, f4_dot3(L0, R)), L0);
    float t = f1_sat(radius / f4_length3(c2r));
    float4 cp = f4_add(L0, f4_mulvs(c2r, t));
    return cp;
}

// [Lagarde & Karis]
pim_inline float4 VEC_CALL EvalSphereLight(
    float4 V,
    float4 P,
    float4 N,
    float4 albedo,
    float roughness,
    float metallic,
    float4 lightPos,
    float4 lightColor)
{
    float4 L0 = f4_sub(lightPos, P);
    float distance = f4_length3(L0);

    float radius = f1_max(kMinLightDist, lightPos.w);
    distance = f1_max(distance, radius);

    float alpha = BrdfAlpha(roughness);
    float NoV = f4_dotsat(N, V);

    float4 Fr = f4_0;
    {
        float4 R = f4_normalize3(f4_reflect3(f4_neg(V), N));
        float4 L = SphereRepresentativePoint(R, N, L0, radius, alpha);
        float specDist = f4_length3(L);
        L = f4_divvs(L, specDist);

        float alphaPrime = AlphaPrime(alpha, radius, distance);

        float4 H = f4_normalize3(f4_add(V, L));
        float NoH = f4_dotsat(N, H);
        float HoV = f4_dotsat(H, V);
        float NoL = f4_dotsat(N, L);
        float D = D_GTR_Sphere(NoH, alpha, alphaPrime);
        float G = V_SmithCorrelated(NoL, NoV, alpha);
        float4 F = F_SchlickEx(albedo, metallic, HoV);
        Fr = f4_mulvs(F, D * G);

        float I = NoL * DistanceAtt(distance);
        Fr = f4_mulvs(Fr, I);
    }

    float4 Fd = f4_0;
    {
        float4 L = f4_divvs(L0, distance);
        float4 H = f4_normalize3(f4_add(V, L));
        float NoL = f4_dot3(N, L);
        float HoV = f4_dotsat(H, V);

        Fd = f4_mulvs(
            DiffuseColor(albedo, metallic),
            Fd_Burley(f1_sat(NoL), NoV, HoV, roughness));

        float sinSigmaSqr = SphereSinSigmaSq(radius, distance);
        float I = SphereDiskIlluminance(NoL, sinSigmaSqr);
        Fd = f4_mulvs(Fd, I);
    }

    const float amtSpecular = 1.0f;
    float amtDiffuse = 1.0f - metallic;
    float scale = 1.0f / (amtSpecular + amtDiffuse);
    float4 brdf = f4_add(Fd, Fr);
    brdf = f4_mulvs(brdf, scale);

    float4 light = f4_mul(brdf, lightColor);

    return light;
}

PIM_C_END
