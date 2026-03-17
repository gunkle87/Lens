# CAPABILITY SET

============================================================
PURPOSE
============================================================

This document defines the "vocabulary" of the Lens renderer and UI.
It lists the primitive shapes, core components, and tools that must be
supported by the framework.

============================================================
PRIMITIVES
============================================================

- Rect (rounded, hollow, filled)
- Circle (hollow, filled)
- Path (multi-segment, bezier support)
- Text (variable font size, alignment)

============================================================
CORE COMPONENTS (Logic Family)
============================================================

- Gates (AND, OR, NOT, XOR, NAND, NOR)
- I/O (Switch, Button, LED, ValueDisplay)
- Subassemblies (Register, Adder, Counter)
- Pins (Input, Output, Bidirectional)

============================================================
WIRE FAMILIES
============================================================

- Logic Trace (single bit)
- Bus Trace (multi-bit)
- Power/Ground Rail
- Flexible routing path

============================================================
UNIVERSAL TOOLSET
============================================================

- Select (single/multi)
- Move (percussive or fine)
- Connect (pin-to-pin tracing)
- Inspect (property readout)
- Place (from asset registry)

============================================================
DEFAULT MATERIALS
============================================================

- Matte Plastic
- Glossy Trace
- Glowing LED (state-driven)
- Metallic Contact
- Board Surface (textured/flat)

============================================================
BOTTOM LINE
============================================================

This set defines the initial "180-in-1" scope. The framework must be
able to represent all of these using its declarative grammar.
