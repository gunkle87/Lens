#ifndef LENS_BINDING_TYPES_H
#define LENS_BINDING_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "lens_uuid.h"

// Visualization Regimes for signal representation
typedef enum {
    LENS_REGIME_STEADY,   // Static level (DC)
    LENS_REGIME_PULSE,    // High-frequency average glow
    LENS_REGIME_FLICKER   // Unstable/noisy signal
} LensVisualRegime;

// Animation Kinds supported by the substrate
typedef enum {
    LENS_ANIM_NONE,
    LENS_ANIM_TRANSITION, // Linear easing
    LENS_ANIM_PULSE,      // Sine wave oscillation
    LENS_ANIM_BLINK       // On/Off square wave
} LensAnimationKind;

// Identifier for a data source subscription
typedef struct {
    LensUUID source_id;      // Authoritative session/sim instance
    char descriptor_id[64];  // Static ID of pin/bus
    char data_path[64];      // Semantic endpoint (e.g., "state")
} LensBindingHandle;

// State of an active animation
typedef struct {
    LensAnimationKind kind;
    float start_time;
    float duration;
    float frequency;
    float phase;
} LensAnimationState;

// Normalized signal representing the visual manifestation of raw data
typedef struct {
    float value;              // 0.0 to 1.0 normalized value
    LensVisualRegime regime;  // Calculated regime based on history
    LensAnimationState anim;  // Current animation driver state
} LensNormalizedSignal;

#endif // LENS_BINDING_TYPES_H
