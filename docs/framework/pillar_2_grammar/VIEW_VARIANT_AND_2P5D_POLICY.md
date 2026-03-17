# VIEW VARIANT AND 2.5D POLICY

============================================================
PURPOSE
============================================================

This document defines how flat and isometric/2.5D views should work in
the LENS renderer.

============================================================
PRIMARY RULE
============================================================

The system should support 2.5D style without becoming a real 3D engine.

Use authored view variants and layered materials, not runtime 3D lighting.

============================================================
VIEW VARIANTS
============================================================

Recommended view variants:

- FLAT
- ISO_LEFT
- ISO_RIGHT
- RAISED_PANEL

These are pre-authored visual modes for components and UI assets.

============================================================
WHAT 2.5D MEANS HERE
============================================================

2.5D means:

- fixed camera assumptions
- authored side/top faces
- fake depth through layered geometry and materials
- no free camera orbit
- no dynamic lighting requirement

============================================================
WHERE VIEW VARIANTS LIVE
============================================================

View variants belong primarily to:

- component definitions
- material/theme definitions
- package-authoring layer

Runtime users may select among approved variants when allowed, but should
not author new projection rules.

============================================================
COMPONENT POLICY
============================================================

A component may define:

- one flat variant
- one or more 2.5D variants

Each variant can adjust:

- silhouette
- face layering
- edge accents
- shadow hints
- material assignments

============================================================
TRACE POLICY
============================================================

Traces may also have view-aware variants.

Examples:

- flat trace
- raised/gloss trace
- isometric route appearance

But the connection semantics do not change.

============================================================
USER ACCESS POLICY
============================================================

End users should typically be allowed:

- variant selection from approved presets
- safe view mode switching if package supports it

End users should not be allowed:

- arbitrary custom 2.5D projection authoring
- low-level extrusion rule editing

============================================================
BOTTOM LINE
============================================================

Treat 2.5D as an authored visual style system layered on top of a 2D
renderer, not as a true 3D engine problem.
