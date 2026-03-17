# GENERIC UI FRAMEWORK SPECIFICATION

============================================================
PURPOSE
============================================================

This document defines the universal toolkit for logic-interfacing.
It handles how high-level simulation state is visualized and manipulated.

This is the bridge between the RAW RENDERER and the LOGIC ENGINE.

============================================================
CORE ARCHITECTURE
============================================================

1. UI ADAPTER (Pillar 5)
   - translates engine-specific data into normalized UI-facing state
   - provides the primary decoupling layer from engine internals

2. BINDING REGISTRY (Pillar 5)
   - manages subscriptions using `BindingHandle` (Source, Descriptor, Path)
   - maps engine state to visual properties via explicit policy

3. PRESENTATION POLICY (Package-Auth Tier)
   - defines how normalized state manifests visually (colors, regimes, animations)
   - ensures product identity (e.g., "Lab" vs "Game") without forking the engine

============================================================
UNIFIED BINDING MODEL
============================================================

A component binds to simulation state through a `BindingHandle`:

- **SourceID**: The active simulation session/instance.
- **DescriptorID**: The static identity of the signal source (e.g. Pin A, Bus 1).
- **DataPath**: The specific attribute being observed (e.g. "state", "voltage").
- **UpdatePolicy**: How the UI observes change (Monotonic vs Interval).

============================================================
COMPOSITE DATA BINDING
============================================================

A single UI component can bind to multiple signals.

Example: A Register Component
- Data Lines: Bind to current value
- Clock Pin: Bind to clock signal (Visual animation: Blink on high)
- Enable Pin: Bind to status signal (Visual: Gray out if disabled)

============================================================
SIGNAL NORMALIZATION
============================================================

The adapter should convert engine-specific values:

- Boolean -> 0 or 1
- Analog (Volt) -> 0.0 to 1.0 based on rail limits
- Logic (Tristate) -> 0, 1, or Z (Visual: Unknown color)

============================================================
VISUALIZATION REGIMES
============================================================

- PULSE: For high-frequency signals, show an animated average glow.
- STEADY: For DC signals, show a constant level.
- FLICKER: For jittery or unstable signals, show a randomized visual noise.

This prevents the renderer from trying to draw 1MHz blinking LEDs.

============================================================
ANIMATION ENGINE
============================================================

Support simple, declarative animations:

- Transition (linear, ease-in, ease-out)
- Pulse (sine, square, ramp)
- Blink (on/off at speed f)

Animations should be driven by SIMULATION TIME, not Wall-Clock Time.

============================================================
BOTTOM LINE
============================================================

Don't let the renderer become the simulation engine.
The Renderer draws what the UI Framework tells it to.
The UI Framework decides what to draw based on the Adapter's report.
Keep it separate.
