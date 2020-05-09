#pragma once

#include "common/macro.h"

PIM_C_BEGIN

#include "math/types.h"
#include "rendering/mesh.h"
#include "rendering/material.h"

typedef struct ent_s
{
    i32 index;
    i32 version;
} ent_t;

typedef struct tag_s
{
    u32 Value;
} tag_t;

typedef struct position_s
{
    float3 Value;
} translation_t;

typedef struct rotation_s
{
    quat Value;
} rotation_t;

typedef struct scale_s
{
    float3 Value;
} scale_t;

typedef struct localtoworld_s
{
    float4x4 Value;         // transforms entity from local space to world space
} localtoworld_t;

typedef struct drawable_s
{
    meshid_t mesh;          // local space immutable mesh
    material_t material;    // textures and surface properties
    bool visible;           // cull result
} drawable_t;

typedef struct bounds_s
{
    box_t box;
} bounds_t;

typedef struct light_s
{
    // xyz: radiance of light, HDR value in [0, 1024]
    // w  > 0: spherical light radius
    // w == 0: directional light
    // w  < 0: spotlight angle * -1
    float4 radiance;
    // direction is given by rotation_t
    // position is given by translation_t
} light_t;

typedef enum
{
    CompId_Tag,
    CompId_Translation,
    CompId_Rotation,
    CompId_Scale,
    CompId_LocalToWorld,
    CompId_Drawable,
    CompId_Bounds,
    CompId_Light,

    CompId_COUNT
} compid_t;

PIM_C_END
