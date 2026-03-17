# CUSTOM MATERIAL MODEL

============================================================
PURPOSE
============================================================

This document defines how custom materials should work in the LENS
renderer without exposing renderer internals directly to end users.

============================================================
PRIMARY RULE
============================================================

Materials are authored data, not renderer code.

They define a visual recipe the renderer understands.

============================================================
MATERIAL TIERS
============================================================

Engine-authoring only:

- low-level material grammar
- gradient engine
- bloom implementation
- shadow/highlight implementation

Package-authoring only:

- material definitions
- theme packs
- style families
- allowed overrides

Runtime-user-editable:

- only whitelisted material selections or safe color adjustments

============================================================
MATERIAL CONTENT
============================================================

A material definition may include:

- base color
- stroke color
- stroke width
- fill gradient
- highlight color/intensity
- shadow color/intensity
- bloom color/intensity/width
- optional 2.5D surface hints

============================================================
MATERIAL FAMILIES
============================================================

Recommended material families:

- board surfaces
- plastic housings
- metal contacts
- trace families
- indicator families
- panel/chrome UI materials
- debug overlay materials

============================================================
SAFE USER CONTROL
============================================================

Users should not get raw access to the material grammar.

Users may get:

- preset selection
- approved palette swaps
- bounded trace color changes
- bounded accent changes

Users should not get:

- arbitrary bloom model edits
- arbitrary gradient grammar edits
- low-level highlight/shadow rewiring

============================================================
2.5D MATERIAL SUPPORT
============================================================

Materials may include hints for:

- top face
- side face
- edge face
- gloss highlight
- depth shadow

But the renderer remains 2D/2.5D.

No real lighting model is required.

============================================================
THEME MODEL
============================================================

A theme should be a curated collection of materials and component variants.

Examples:

- playful game theme
- electronics lab theme
- enterprise dark/light theme

Themes belong to package-authoring tier.

============================================================
BOTTOM LINE
============================================================

Materials are powerful, but they should remain controlled authored data.

That gives you visual range without giving users the power to destabilize
the renderer or break product identity.
