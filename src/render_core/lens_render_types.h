#ifndef LENS_RENDER_TYPES_H
#define LENS_RENDER_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "lens_math.h"

// ID handle for objects (stable reference)
typedef uint32_t LensHandle;

// Core material struct (from .lmat)
typedef struct {
    uint32_t base_color;       // ARGB/RGBA hex
    uint32_t stroke_color;
    float stroke_width;
    float glow_intensity;
    float glow_radius;
    float opacity;
    float bloom_threshold;
} LensMaterial;

typedef enum {
    LENS_ALIGN_START = 0,
    LENS_ALIGN_MIDDLE,
    LENS_ALIGN_END
} LensTextAlignment;

// Supported primitives
typedef enum {
    LENS_PRIMITIVE_NONE = 0,
    LENS_PRIMITIVE_RECT,
    LENS_PRIMITIVE_CIRCLE,
    LENS_PRIMITIVE_PATH,
    LENS_PRIMITIVE_TEXT
} LensPrimitiveType;

typedef struct {
    float x, y, w, h, r;
} LensRectPrimitive;

typedef struct {
    float x, y, r;
} LensCirclePrimitive;

typedef struct {
    const char* data; // SVG-like
} LensPathPrimitive;

typedef struct {
    const char* value;
    const char* font;
    float size;
    LensTextAlignment alignment;
} LensTextPrimitive;

// Composite primitive struct (Tagged Union)
typedef struct {
    LensPrimitiveType type;
    union {
        LensRectPrimitive rect;
        LensCirclePrimitive circle;
        LensPathPrimitive path;
        LensTextPrimitive text;
    } data;
} LensRenderPrimitive;

// Tree Node for inputs (AoS styling for Scene definition structure)
#define LENS_MAX_CHILDREN 16

typedef struct LensSceneNode {
    LensHandle id;
    LensMatrix3x3 local_transform;
    float local_opacity;
    LensMaterial material;
    LensRenderPrimitive primitive;

    // Direct pointers for hierarchical layout
    uint32_t child_count;
    struct LensSceneNode* children[LENS_MAX_CHILDREN];
} LensSceneNode;

// Global Render Command Buffer (SoA layout) replacing object pointers
typedef struct {
    uint32_t count;
    uint32_t capacity;

    // Parallel Arrays
    LensHandle* ids;
    LensPrimitiveType* types;
    LensRenderPrimitive* primitives;
    LensMatrix3x3* global_transforms;
    LensMaterial* resolved_materials;
    float* global_opacities;
} LensCommandBuffer;

#endif // LENS_RENDER_TYPES_H
