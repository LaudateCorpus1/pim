#pragma once

#include "common/macro.h"
#include "common/int_types.h"

enum ComponentType : u16
{
    ComponentType_Null = 0,
    ComponentType_Translation,
    ComponentType_Rotation,
    ComponentType_Scale,
    ComponentType_LocalToWorld,
    ComponentType_Children,

    ComponentType_Renderable,

    ComponentType_Count
};

// ----------------------------------------------------------------------------

namespace Component
{
    using DropFn = void(*)(void* pComponent);

    inline bool ValidType(ComponentType type)
    {
        return (type > ComponentType_Null) && (type < ComponentType_Count);
    }

    i32 SizeOf(ComponentType type);
    void Drop(ComponentType type, void* pVoid);

    template<typename T>
    inline constexpr ComponentType TypeOf()
    {
        DebugInterrupt();
        return ComponentType_Null;
    }

    // ------------------------------------------------------------------------

    void SetSize(ComponentType type, i32 size);
    void SetDropFn(ComponentType type, DropFn fn);

    struct InfoSetter
    {
        inline InfoSetter(ComponentType type, i32 sizeOf, void(*DropFn)(void*))
        {
            SetSize(type, sizeOf);
            SetDropFn(type, DropFn);
        }
    };
};

#define DeclComponent(T, DtorExpr) \
    template<> \
    inline constexpr ComponentType Component::TypeOf<T>() { return ComponentType_ ## T; } \
    Component::InfoSetter CatToken(CompInfoSetter_, __COUNTER__)(ComponentType_ ## T, sizeof(T), [](void* pVoid) { T* ptr = (T*)pVoid; DtorExpr; });
