#include "lens_interaction_manager.h"

void lens_interaction_init_viewport(LensViewport* viewport, float x, float y, float w, float h) {
    if (!viewport) return;
    viewport->x = x;
    viewport->y = y;
    viewport->width = w;
    viewport->height = h;
    viewport->pan_x = 0.0f;
    viewport->pan_y = 0.0f;
    viewport->zoom = 1.0f;
}

void lens_interaction_set_viewport_transform(LensViewport* viewport, float pan_x, float pan_y, float zoom) {
    if (!viewport) return;
    viewport->pan_x = pan_x;
    viewport->pan_y = pan_y;
    viewport->zoom = zoom > 0.0f ? zoom : 1.0f;
}

bool lens_interaction_screen_to_world(const LensViewport* viewport, float screen_x, float screen_y, float* out_world_x, float* out_world_y) {
    if (!viewport) return false;
    
    // Clip Test: Screen coordinates must be within the defined viewport bounds
    if (screen_x < viewport->x || screen_x > viewport->x + viewport->width ||
        screen_y < viewport->y || screen_y > viewport->y + viewport->height) {
        return false;
    }

    if (out_world_x) {
        *out_world_x = ((screen_x - viewport->x) / viewport->zoom) - viewport->pan_x;
    }
    if (out_world_y) {
        *out_world_y = ((screen_y - viewport->y) / viewport->zoom) - viewport->pan_y;
    }
    
    return true;
}

LensHitResult lens_interaction_hit_test(LensSceneDocument* doc, const LensViewport* viewport, float screen_x, float screen_y) {
    LensHitResult hit = { .hit = false, .node_id = lens_uuid_null(), .world_x = 0.0f, .world_y = 0.0f };
    
    float world_x, world_y;
    if (!lens_interaction_screen_to_world(viewport, screen_x, screen_y, &world_x, &world_y)) {
        return hit; // Rejected by viewport clip
    }
    
    hit.world_x = world_x;
    hit.world_y = world_y;
    
    if (!doc) return hit;

    // Explicit query pass over the scene table (Topmost is expected to be latest in array)
    for (uint32_t i = 0; i < doc->node_count; i++) {
        LensSceneNode* node = &doc->nodes[i];
        
        if (!node->is_visible) continue;
        
        // AABB parameter resolution defaults
        float node_w = 100.0f;
        float node_h = 100.0f;
        lens_scene_get_override_float(node, "Width", &node_w);
        lens_scene_get_override_float(node, "Height", &node_h);

        // Resolve absolute position
        LensMatrix3x3 wt = lens_scene_get_world_transform(doc, node->id);
        float tx = wt.m[2];
        float ty = wt.m[5];
        
        // simple AABB inclusion test
        if (world_x >= tx && world_x <= tx + node_w &&
            world_y >= ty && world_y <= ty + node_h) {
            hit.hit = true;
            hit.node_id = node->id;
            // Iterate all to ensure finding the topmost (assuming painter's order in array)
        }
    }
    
    return hit;
}

void lens_interaction_selection_init(LensSelectionState* state) {
    if (!state) return;
    state->count = 0;
}

void lens_interaction_selection_clear(LensSceneDocument* doc, LensSelectionState* state) {
    if (!state) return;
    
    for (uint32_t i = 0; i < state->count; i++) {
        if (doc) {
            LensSceneNode* node = lens_scene_get_node(doc, state->selected_nodes[i]);
            if (node) {
                lens_scene_set_override_float(node, "IsSelected", 0.0f);
            }
        }
    }
    state->count = 0;
}

bool lens_interaction_selection_contains(const LensSelectionState* state, LensUUID node_id) {
    if (!state) return false;
    for (uint32_t i = 0; i < state->count; i++) {
        if (lens_uuid_is_equal(state->selected_nodes[i], node_id)) {
            return true;
        }
    }
    return false;
}

void lens_interaction_selection_add(LensSceneDocument* doc, LensSelectionState* state, LensUUID node_id) {
    if (!state || lens_uuid_is_null(node_id)) return;
    if (lens_interaction_selection_contains(state, node_id)) return;
    
    if (state->count < LENS_MAX_SELECTION) {
        state->selected_nodes[state->count++] = node_id;
        if (doc) {
            LensSceneNode* node = lens_scene_get_node(doc, node_id);
            if (node) {
                // Emitting interaction property to scene
                lens_scene_set_override_float(node, "IsSelected", 1.0f);
            }
        }
    }
}

void lens_interaction_selection_remove(LensSceneDocument* doc, LensSelectionState* state, LensUUID node_id) {
    if (!state || lens_uuid_is_null(node_id)) return;
    
    for (uint32_t i = 0; i < state->count; i++) {
        if (lens_uuid_is_equal(state->selected_nodes[i], node_id)) {
            // Swap with last mapping
            state->selected_nodes[i] = state->selected_nodes[state->count - 1];
            state->count--;
            
            if (doc) {
                LensSceneNode* node = lens_scene_get_node(doc, node_id);
                if (node) {
                    lens_scene_set_override_float(node, "IsSelected", 0.0f);
                }
            }
            return;
        }
    }
}

void lens_interaction_selection_toggle(LensSceneDocument* doc, LensSelectionState* state, LensUUID node_id) {
    if (lens_interaction_selection_contains(state, node_id)) {
        lens_interaction_selection_remove(doc, state, node_id);
    } else {
        lens_interaction_selection_add(doc, state, node_id);
    }
}

void lens_interaction_set_hover(LensSceneDocument* doc, LensUUID node_id, bool hovered) {
    if (!doc || lens_uuid_is_null(node_id)) return;
    LensSceneNode* node = lens_scene_get_node(doc, node_id);
    if (node) {
        // Applying Hover override map
        lens_scene_set_override_float(node, "IsHovered", hovered ? 1.0f : 0.0f);
    }
}
