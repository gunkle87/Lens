# RENDER FORMAT SPECIFICATION

============================================================
PURPOSE
============================================================

This document defines the high-level, text-based visual format for the
LENS renderer.

It is designed to be:

- declarative
- deterministic
- easy to parse/lex
- human readable/editable
- separate from engine logic

============================================================
CORE FILE TYPES
============================================================

The system should recognize these primary definition blocks:

1. MATERIAL (.lmat)
   - colors, glows, gradients, surface rules

2. SHAPE (.lshp)
   - primitive silhouettes, fills, strokes, layering

3. COMPONENT (.lcomp)
   - composite objects, variants, ports, anchors, logic bindings

4. PALETTE (.lpal)
   - shared color tokens

5. SCENE (.lscn)
   - placed instances, transforms, state, traces

6. STATE (.lst)
   - runtime-only property overrides

============================================================
DRAWING PRIMITIVES
============================================================

The renderer core supports a constrained set of primitives:

- RECT (x, y, w, h, r)
- CIRCLE (x, y, r)
- PATH (data string) - constrained SVG-like path data
- TEXT (value, font, size, anchor)
- GROUP (children, transform)

============================================================
TRANSFORM RULES
============================================================

- Position (x, y)
- Scale (sx, sy)
- Rotation (degrees)
- Pivot (px, py)

Nested transforms are allowed.

============================================================
MATERIAL PROPERTIES
============================================================

Every shape or group can reference a MATERIAL.

Material properties:

- base_color (hex)
- stroke_color (hex)
- stroke_width (float)
- glow_intensity (0.0 - 1.0)
- glow_radius (float)
- opacity (0.0 - 1.0)
- bloom_threshold (0.0 - 1.0)

============================================================
GRADIENT MODEL
============================================================

Support simple linear and radial gradients:

- type: linear | radial
- stops: list of (offset, color)
- start/end or center/radius

============================================================
PARSER RULES
============================================================

- whitespace insensitive (mostly)
- case sensitive IDs
- support comments (# or //)
- mandatory units (default to pixels/units)

============================================================
BOTTOM LINE
============================================================

Keep the format simple. Avoid embedding scripts in visual definitions.
If it cannot be represented as a static hierarchy of properties and
primitives, it probably belongs in the logic layer.
