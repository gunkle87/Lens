#ifndef LENS_SCENE_TYPES_H
#define LENS_SCENE_TYPES_H

#include "lens_uuid.h"
#include "../render_core/lens_math.h"
#include <stdint.h>
#include <stdbool.h>

// Structural Object Kinds
typedef enum {
    LENS_NODE_GROUP,
    LENS_NODE_COMPONENT,
    LENS_NODE_TRACE,
    LENS_NODE_TEXT
} LensNodeKind;

// Runtime Instance Parameter Overrides (Separating instance context from definition geometry)
typedef enum {
    LENS_OVERRIDE_FLOAT,
    LENS_OVERRIDE_STRING
} LensOverrideType;

typedef struct {
    char param_name[64];
    LensOverrideType type;
    union {
        float f_value;
        char s_value[128];
    } val;
} LensParameterOverride;

#define LENS_MAX_OVERRIDES 16

// DOD table-friendly inner data blocks
typedef struct {
    char def_id[64]; // Ref to external authored grammar ComponentDef
    uint32_t override_count;
    LensParameterOverride overrides[LENS_MAX_OVERRIDES];
} LensComponentInstanceData;

#define LENS_MAX_TRACE_VERTICES 32

typedef struct {
    LensUUID source_node_id;
    char source_port[32];
    
    LensUUID dest_node_id;
    char dest_port[32];
    
    char style_id[64];
    
    struct {
        float x, y;
    } vertices[LENS_MAX_TRACE_VERTICES];
    uint32_t vertex_count;
} LensTraceData;

typedef struct {
    char text_content[256];
} LensTextData;

// Scene Node representing a single mutable runtime entity
typedef struct {
    LensUUID id;
    LensUUID parent_id; // Relational hierarchy linkage (keeps structs flat)
    
    char name[64];
    LensNodeKind kind;
    bool is_visible;
    uint32_t layer_class;
    
    LensMatrix3x3 local_transform;
    
    // Tagged variant payloads
    union {
        LensComponentInstanceData component;
        LensTraceData trace;
        LensTextData text;
        // Group relies solely on parent/child linkages and transforms
    } data;
} LensSceneNode;

#define LENS_MAX_SCENE_NODES 4096

// Root document containing the dense table memory of nodes.
// Avoids "is-a" object pointer webs in favor of a flat array bucket.
typedef struct {
    LensSceneNode nodes[LENS_MAX_SCENE_NODES];
    uint32_t node_count;
} LensSceneDocument;

#endif // LENS_SCENE_TYPES_H
