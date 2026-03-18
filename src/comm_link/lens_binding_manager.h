#ifndef LENS_BINDING_MANAGER_H
#define LENS_BINDING_MANAGER_H

#include "lens_binding_types.h"

#define LENS_MAX_BINDINGS 1024

// Registry mapping a component's visual property to a data source
typedef struct {
    LensUUID node_id;         // Component being bound
    char property_name[64];   // Property (e.g., "glow_intensity")
    LensBindingHandle handle; // Subscription details
    LensNormalizedSignal signal; // Current state
    
    // History for regime detection
    float last_value;
    float last_change_time;
    uint32_t toggle_count;
    float window_start_time;
} LensBindingEntry;

typedef struct {
    LensBindingEntry entries[LENS_MAX_BINDINGS];
    uint32_t count;
    float current_sim_time;
} LensBindingManager;

// API
void lens_binding_init(LensBindingManager* manager);

// Register a binding for a specific node property
bool lens_binding_register(LensBindingManager* manager, LensUUID node_id, const char* property, LensBindingHandle handle);

// Ingest raw data from the engine
void lens_binding_ingest_bool(LensBindingManager* manager, LensBindingHandle handle, bool value, float sim_time);
void lens_binding_ingest_float(LensBindingManager* manager, LensBindingHandle handle, float value, float min_rail, float max_rail, float sim_time);

// Update all signals (process regimes and animations)
void lens_binding_update(LensBindingManager* manager, float sim_time);

// Retrieve the current normalized signal for a property
bool lens_binding_get_signal(LensBindingManager* manager, LensUUID node_id, const char* property, LensNormalizedSignal* out_signal);

#endif // LENS_BINDING_MANAGER_H
