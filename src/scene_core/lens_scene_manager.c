#include "lens_scene_manager.h"
#include <string.h>
void lens_scene_document_init(LensSceneDocument* doc) {
    doc->node_count = 0;
    memset(doc->nodes, 0, sizeof(doc->nodes));
}

// Pseudo-UUID generation for testing logic without heavy cryptographic dependencies
static uint32_t s_uuid_counter = 1;
LensUUID lens_generate_uuid(void) {
    LensUUID id = {0};
    id.bytes[0] = (uint8_t)(s_uuid_counter & 0xFF);
    id.bytes[1] = (uint8_t)((s_uuid_counter >> 8) & 0xFF);
    id.bytes[2] = (uint8_t)((s_uuid_counter >> 16) & 0xFF);
    id.bytes[3] = (uint8_t)((s_uuid_counter >> 24) & 0xFF);
    id.bytes[4] = 0xAA; // Application magic signature bytes
    id.bytes[5] = 0xBB;
    s_uuid_counter++;
    return id;
}

LensSceneNode* lens_scene_create_node(LensSceneDocument* doc, LensNodeKind kind, LensUUID parent) {
    if (doc->node_count >= LENS_MAX_SCENE_NODES) return NULL;
    
    LensSceneNode* node = &doc->nodes[doc->node_count++];
    memset(node, 0, sizeof(LensSceneNode));
    
    node->id = lens_generate_uuid();
    node->parent_id = parent;
    node->kind = kind;
    node->is_visible = true;
    node->local_transform = lens_matrix_identity();
    
    return node;
}

LensSceneNode* lens_scene_get_node(LensSceneDocument* doc, LensUUID id) {
    if (lens_uuid_is_null(id)) return NULL;
    
    // Linear scan for now, DOD allows transforming this to parallel bucket checks
    // or hash lookups internally without changing the public UUID handle pattern.
    for (uint32_t i = 0; i < doc->node_count; i++) {
        if (lens_uuid_is_equal(doc->nodes[i].id, id)) {
            return &doc->nodes[i];
        }
    }
    return NULL;
}

LensMatrix3x3 lens_scene_get_world_transform(LensSceneDocument* doc, LensUUID id) {
    LensSceneNode* node = lens_scene_get_node(doc, id);
    if (!node) return lens_matrix_identity();
    
    if (lens_uuid_is_null(node->parent_id)) {
        return node->local_transform; // Reached Root implicitly
    }
    
    LensMatrix3x3 parent_transform = lens_scene_get_world_transform(doc, node->parent_id);
    return lens_matrix_multiply(parent_transform, node->local_transform);
}

// --- Parameter Overrides --- //

// Helper to find existing or allocate new override slot
static LensParameterOverride* get_or_add_override(LensSceneNode* node, const char* param_name) {
    if (!node || node->kind != LENS_NODE_COMPONENT) return NULL;
    LensComponentInstanceData* comp = &node->data.component;
    
    for (uint32_t i = 0; i < comp->override_count; i++) {
        if (strcmp(comp->overrides[i].param_name, param_name) == 0) {
            return &comp->overrides[i];
        }
    }
    
    if (comp->override_count < LENS_MAX_OVERRIDES) {
        uint32_t idx = comp->override_count++;
        strncpy(comp->overrides[idx].param_name, param_name, 63);
        comp->overrides[idx].param_name[63] = '\0';
        return &comp->overrides[idx];
    }
    return NULL;
}

void lens_scene_set_override_float(LensSceneNode* node, const char* param_name, float value) {
    LensParameterOverride* ov = get_or_add_override(node, param_name);
    if (ov) {
        ov->type = LENS_OVERRIDE_FLOAT;
        ov->val.f_value = value;
    }
}

void lens_scene_set_override_string(LensSceneNode* node, const char* param_name, const char* value) {
    LensParameterOverride* ov = get_or_add_override(node, param_name);
    if (ov) {
        ov->type = LENS_OVERRIDE_STRING;
        strncpy(ov->val.s_value, value, 127);
        ov->val.s_value[127] = '\0';
    }
}

bool lens_scene_get_override_float(LensSceneNode* node, const char* param_name, float* out_value) {
    if (!node || node->kind != LENS_NODE_COMPONENT || !out_value) return false;
    LensComponentInstanceData* comp = &node->data.component;
    for (uint32_t i = 0; i < comp->override_count; i++) {
        if (strcmp(comp->overrides[i].param_name, param_name) == 0 && comp->overrides[i].type == LENS_OVERRIDE_FLOAT) {
            *out_value = comp->overrides[i].val.f_value;
            return true;
        }
    }
    return false;
}

bool lens_scene_get_override_string(LensSceneNode* node, const char* param_name, char* out_buf, int max_len) {
    if (!node || node->kind != LENS_NODE_COMPONENT || !out_buf) return false;
    LensComponentInstanceData* comp = &node->data.component;
    for (uint32_t i = 0; i < comp->override_count; i++) {
        if (strcmp(comp->overrides[i].param_name, param_name) == 0 && comp->overrides[i].type == LENS_OVERRIDE_STRING) {
            strncpy(out_buf, comp->overrides[i].val.s_value, max_len);
            return true;
        }
    }
    return false;
}
