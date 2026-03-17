# LAYER CLASS POLICY

============================================================
PURPOSE
============================================================

This document defines the layer-class policy for the LENS scene/UI
renderer. It prevents the system from becoming a flat, unmanageable
Z-sorted list.

============================================================
PRIMARY RULE
============================================================

Order objects by SEMANTIC BUCKET first, then by LOCAL Z.

============================================================
ORDERING BUCKETS (Lowest to Highest)
============================================================

1. BACKGROUND
   - grid lines, board surface textures, static images

2. LOW_TRACE
   - secondary traces, back-plane wiring

3. TRACE
   - primary signal wires

4. COMPONENT_BODY
   - logic gates, housings, housings, IC footprints

5. COMPONENT_LABEL
   - text on top of components, pin names

6. UI_CHROME
   - toolbars, panels, inspectors (package-shell layer)

7. OVERLAY / CURSOR
   - selection box, drag ghost, tool cursors, hover highlights

============================================================
LAYER CLASS ASSIGNMENT
============================================================

Every SceneNode must declare its LayerClass.
The default should be COMPONENT_BODY or TRACE.

============================================================
LOCAL Z RULES
============================================================

Within a bucket, use a local Z-index for fine-tuning.
The total draw order is:
`GlobalOrder = (BucketWeight * 1000) + LocalZIndex`

Example:
- A trace (Bucket 3) always draws over the board (Bucket 1).
- Two overlapping traces (both Bucket 3) are sorted by their LocalZ.

============================================================
TRANSPARENCY & COMPOSITING
============================================================

Layer classes should be treated as potential draw-call boundaries
for optimization.

The renderer may choose to flatten a static background layer into a
texture to save draw calls.

============================================================
BOTTOM LINE
============================================================

Semantic layering is more robust than manual Z-fighting.
Assign objects to the correct bucket based on their role in the UI,
not just to "make it look right."
