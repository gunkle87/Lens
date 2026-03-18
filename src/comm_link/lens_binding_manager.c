#include "lens_binding_manager.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define LENS_REGIME_WINDOW_SEC 1.0f
#define LENS_PULSE_THRESHOLD 100 // Toggles per second for PULSE regime
#define LENS_FLICKER_THRESHOLD 10 // Toggles per second for FLICKER regime

void lens_binding_init(LensBindingManager* manager) {
    memset(manager, 0, sizeof(LensBindingManager));
}

bool lens_binding_register(LensBindingManager* manager, LensUUID node_id, const char* property, LensBindingHandle handle) {
    if (manager->count >= LENS_MAX_BINDINGS) return false;
    
    LensBindingEntry* entry = &manager->entries[manager->count++];
    entry->node_id = node_id;
    strncpy(entry->property_name, property, 63);
    entry->handle = handle;
    entry->signal.regime = LENS_REGIME_STEADY;
    entry->window_start_time = 0.0f;
    
    return true;
}

static void calculate_regime(LensBindingEntry* entry) {
    float freq = (float)entry->toggle_count / LENS_REGIME_WINDOW_SEC;
    
    if (freq > LENS_PULSE_THRESHOLD) {
        entry->signal.regime = LENS_REGIME_PULSE;
    } else if (freq > LENS_FLICKER_THRESHOLD) {
        entry->signal.regime = LENS_REGIME_FLICKER;
    } else {
        entry->signal.regime = LENS_REGIME_STEADY;
    }
}

static void update_entry_metrology(LensBindingEntry* entry, float value, float sim_time) {
    if (sim_time - entry->window_start_time >= LENS_REGIME_WINDOW_SEC) {
        calculate_regime(entry);
        entry->toggle_count = 0;
        entry->window_start_time = sim_time;
    }
    
    if (entry->last_value != value) {
        entry->toggle_count++;
        entry->last_value = value;
        entry->last_change_time = sim_time;
    }
}

void lens_binding_ingest_bool(LensBindingManager* manager, LensBindingHandle handle, bool value, float sim_time) {
    for (uint32_t i = 0; i < manager->count; i++) {
        LensBindingEntry* entry = &manager->entries[i];
        if (lens_uuid_is_equal(entry->handle.source_id, handle.source_id) &&
            strcmp(entry->handle.descriptor_id, handle.descriptor_id) == 0 &&
            strcmp(entry->handle.data_path, handle.data_path) == 0) {
            
            float norm_val = value ? 1.0f : 0.0f;
            update_entry_metrology(entry, norm_val, sim_time);
            entry->signal.value = norm_val;
        }
    }
}

void lens_binding_ingest_float(LensBindingManager* manager, LensBindingHandle handle, float value, float min_rail, float max_rail, float sim_time) {
    for (uint32_t i = 0; i < manager->count; i++) {
        LensBindingEntry* entry = &manager->entries[i];
        if (lens_uuid_is_equal(entry->handle.source_id, handle.source_id) &&
            strcmp(entry->handle.descriptor_id, handle.descriptor_id) == 0 &&
            strcmp(entry->handle.data_path, handle.data_path) == 0) {
            
            // Normalization
            float range = max_rail - min_rail;
            float norm_val = (range > 0.0001f) ? (value - min_rail) / range : 0.0f;
            if (norm_val < 0.0f) norm_val = 0.0f;
            if (norm_val > 1.0f) norm_val = 1.0f;
            
            update_entry_metrology(entry, norm_val, sim_time);
            entry->signal.value = norm_val;
        }
    }
}

void lens_binding_update(LensBindingManager* manager, float sim_time) {
    manager->current_sim_time = sim_time;
    
    for (uint32_t i = 0; i < manager->count; i++) {
        LensBindingEntry* entry = &manager->entries[i];
        
        // Ensure regime is updated even if no new data arrived
        if (sim_time - entry->window_start_time >= LENS_REGIME_WINDOW_SEC) {
            calculate_regime(entry);
            entry->toggle_count = 0;
            entry->window_start_time = sim_time;
        }

        LensAnimationState* anim = &entry->signal.anim;
        
        switch (anim->kind) {
            case LENS_ANIM_PULSE: {
                float elapsed = sim_time - anim->start_time;
                // Sine oscillation: Value = 0.5 + 0.5 * sin(2*PI*f*t + phase)
                entry->signal.value = 0.5f + 0.5f * sinf(2.0f * 3.14159f * anim->frequency * elapsed + anim->phase);
                break;
            }
            case LENS_ANIM_BLINK: {
                float elapsed = sim_time - anim->start_time;
                // Square wave: f transitions per second
                float period = 1.0f / anim->frequency;
                float cycle = fmodf(elapsed, period);
                entry->signal.value = (cycle < period * 0.5f) ? 1.0f : 0.0f;
                break;
            }
            case LENS_ANIM_TRANSITION: {
                if (anim->duration > 0.0f) {
                    float elapsed = sim_time - anim->start_time;
                    float t = elapsed / anim->duration;
                    if (t > 1.0f) {
                        t = 1.0f;
                        anim->kind = LENS_ANIM_NONE; // End transition
                    }
                    // Linear lerp logic would ideally need start_value, 
                    // but for this substrate we simplify to active value 
                    // being controlled by the transition driver.
                    entry->signal.value = t; 
                }
                break;
            }
            default: break;
        }
        
        // Final regime override: PULSE regime forces glow value 
        if (entry->signal.regime == LENS_REGIME_PULSE) {
            entry->signal.value = 0.8f; // Standard high-freq glow
        } else if (entry->signal.regime == LENS_REGIME_FLICKER) {
            // Jittery noise for unstable signals
            float noise = (float)(rand() % 100) / 100.0f;
            entry->signal.value = 0.3f + 0.4f * noise;
        }
    }
}

bool lens_binding_get_signal(LensBindingManager* manager, LensUUID node_id, const char* property, LensNormalizedSignal* out_signal) {
    for (uint32_t i = 0; i < manager->count; i++) {
        LensBindingEntry* entry = &manager->entries[i];
        if (lens_uuid_is_equal(entry->node_id, node_id) &&
            strcmp(entry->property_name, property) == 0) {
            *out_signal = entry->signal;
            return true;
        }
    }
    return false;
}
