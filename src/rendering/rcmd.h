#pragma once

#include "common/macro.h"

PIM_C_BEGIN

#include "math/types.h"
#include "rendering/mesh.h"
#include "rendering/material.h"
#include "containers/ptrqueue.h"
#include "rendering/constants.h"
#include "rendering/camera.h"

typedef enum
{
    RCmdType_NONE,

    RCmdType_Clear,
    RCmdType_View,
    RCmdType_Draw,
    RCmdType_Resolve,

    RCmdType_COUNT
} RCmdType;


typedef struct rcmd_clear_s
{
    float4 color;
    float depth;
} rcmd_clear_t;

typedef struct rcmd_view_s
{
    camera_t camera;
} rcmd_view_t;

typedef struct rcmd_draw_s
{
    float4x4 M;
    meshid_t meshid;
    material_t material;
} rcmd_draw_t;

typedef struct rcmd_resolve_s
{
    float exposure;
} rcmd_resolve_t;

typedef struct rcmd_s
{
    i32 type;
    union
    {
        rcmd_clear_t clear;
        rcmd_view_t view;
        rcmd_draw_t draw;
        rcmd_resolve_t resolve;
    };
} rcmd_t;

typedef struct rcmdbuf_s
{
    void* ptr;
    i32 length;
} rcmdbuf_t;

typedef struct rcmdqueue_s
{
    ptrqueue_t queues[kTileCount];
} rcmdqueue_t;

rcmdbuf_t* rcmdbuf_create(void);
bool rcmdbuf_read(const rcmdbuf_t* buf, i32* pCursor, rcmd_t* dst);

void rcmd_clear(rcmdbuf_t* buf, float4 color, float depth);
void rcmd_view(rcmdbuf_t* buf, camera_t camera);
void rcmd_draw(rcmdbuf_t* buf, float4x4 model, meshid_t meshid, material_t material);
// HDR linear float4 -> LDR sRGB rgba8
void rcmd_resolve(rcmdbuf_t* buf, float exposure);

void rcmdqueue_create(rcmdqueue_t* queue);
void rcmdqueue_destroy(rcmdqueue_t* queue);
void rcmdqueue_submit(rcmdqueue_t* queue, rcmdbuf_t* buf);
rcmdbuf_t* rcmdqueue_read(rcmdqueue_t* queue, i32 tile_id);

PIM_C_END