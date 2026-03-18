#include "lens_render_graph.h"
#include <stdlib.h>
#include <stdbool.h>

void lens_command_buffer_init(LensCommandBuffer* buffer, uint32_t capacity) {
    buffer->capacity = capacity;
    buffer->count = 0;
    
    // Allocate SoA structure segments
    buffer->ids = (LensHandle*)calloc(capacity, sizeof(LensHandle));
    buffer->types = (LensPrimitiveType*)calloc(capacity, sizeof(LensPrimitiveType));
    buffer->primitives = (LensRenderPrimitive*)calloc(capacity, sizeof(LensRenderPrimitive));
    buffer->global_transforms = (LensMatrix3x3*)calloc(capacity, sizeof(LensMatrix3x3));
    buffer->resolved_materials = (LensMaterial*)calloc(capacity, sizeof(LensMaterial));
    buffer->global_opacities = (float*)calloc(capacity, sizeof(float));
}

void lens_command_buffer_destroy(LensCommandBuffer* buffer) {
    free(buffer->ids);
    free(buffer->types);
    free(buffer->primitives);
    free(buffer->global_transforms);
    free(buffer->resolved_materials);
    free(buffer->global_opacities);
    
    buffer->ids = NULL;
    buffer->types = NULL;
    buffer->primitives = NULL;
    buffer->global_transforms = NULL;
    buffer->resolved_materials = NULL;
    buffer->global_opacities = NULL;
    buffer->count = 0;
    buffer->capacity = 0;
}

static bool flatten_recursive(LensSceneNode* node, LensMatrix3x3 parent_transform, float parent_opacity, LensCommandBuffer* out_buffer) {
    if (!node) return true;

    // Resolve global properties
    LensMatrix3x3 global_transform = lens_matrix_multiply(parent_transform, node->local_transform);
    float global_opacity = parent_opacity * node->local_opacity;

    // Issue draw command if node carries visual semantics
    if (node->primitive.type != LENS_PRIMITIVE_NONE) {
        if (out_buffer->count >= out_buffer->capacity) {
            return false; // CAPACITY REACHED
        }
        
        uint32_t idx = out_buffer->count++;
        
        out_buffer->ids[idx] = node->id;
        out_buffer->types[idx] = node->primitive.type;
        out_buffer->primitives[idx] = node->primitive;
        out_buffer->global_transforms[idx] = global_transform;
        out_buffer->global_opacities[idx] = global_opacity;
        
        // Reconcile inherited visual properties
        LensMaterial rm = node->material;
        rm.opacity *= global_opacity;
        out_buffer->resolved_materials[idx] = rm;
    }

    // Traverse logic hierarchy
    for (uint32_t i = 0; i < node->child_count; i++) {
        if (!flatten_recursive(node->children[i], global_transform, global_opacity, out_buffer)) {
            return false;
        }
    }
    
    return true;
}

bool lens_flatten_scene(LensSceneNode* root, LensCommandBuffer* out_buffer) {
    if (!root || !out_buffer) return false;
    
    out_buffer->count = 0;
    LensMatrix3x3 identity = lens_matrix_identity();
    return flatten_recursive(root, identity, 1.0f, out_buffer);
}
