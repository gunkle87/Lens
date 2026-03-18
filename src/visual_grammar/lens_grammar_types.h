#ifndef LENS_GRAMMAR_TYPES_H
#define LENS_GRAMMAR_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "../render_core/lens_math.h"

// Customization Tiers (from docs/RULES.md)
typedef enum {
    LENS_TIER_ENGINE = 0,  // Root authority: primitive grammar, renderer internals
    LENS_TIER_PACKAGE = 1, // Author authority: components, materials, themes
    LENS_TIER_USER = 2     // Runtime authority: instances, labels, allowed overrides
} LensCustomizationTier;

// Parameter Types
typedef enum {
    LENS_PARAM_FLOAT,
    LENS_PARAM_INT,
    LENS_PARAM_ENUM,
    LENS_PARAM_STRING,
    LENS_PARAM_COLOR,
    LENS_PARAM_BOOL
} LensParameterType;

// Parameter Definition (Blueprint for what can be edited)
typedef struct {
    char name[64];
    LensParameterType type;
    LensCustomizationTier tier;
    
    union {
        struct { float min, max, default_val; } f_range;
        struct { int32_t min, max, default_val; } i_range;
        struct { uint32_t default_val; } color;
        struct { bool default_val; } boolean;
    } constraints;

    bool is_instance_overridable;
} LensParameterDef;

// Primitive Kind (Subset of Render Core vocabulary)
typedef enum {
    LENS_GRAMMAR_RECT,
    LENS_GRAMMAR_CIRCLE,
    LENS_GRAMMAR_PATH,
    LENS_GRAMMAR_TEXT
} LensGrammarPrimitiveKind;

// Material Definition (Intermediate validated state)
typedef struct {
    char id[64];
    uint32_t base_color;
    float opacity;
    float stroke_width;
    LensCustomizationTier source_tier;
} LensMaterialDef;

// Internal Shape for components (Defines one piece of a compound asset)
typedef struct {
    LensGrammarPrimitiveKind kind;
    LensMatrix3x3 local_transform;
    char material_id[64]; // Ref to a material
} LensGrammarShape;

// Component Definition (Intermediate validated state)
#define LENS_MAX_PARAMS 16
#define LENS_MAX_SHAPES 8

typedef struct {
    char id[64];
    LensParameterDef params[LENS_MAX_PARAMS];
    uint32_t param_count;

    LensGrammarShape shapes[LENS_MAX_SHAPES];
    uint32_t shape_count;

    LensCustomizationTier source_tier;
} LensComponentDef;

#endif // LENS_GRAMMAR_TYPES_H
