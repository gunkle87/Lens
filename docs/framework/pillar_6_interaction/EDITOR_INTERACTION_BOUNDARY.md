# EDITOR INTERACTION BOUNDARY

============================================================
PURPOSE
============================================================

This document defines the boundary between the LENS renderer,
the scene core, the interaction layer, and the application shell.

This split ensures the renderer remains a "Hand" while the interaction
layer remains the "Brain" of the editing experience.

============================================================
LAYER RESPONSIBILITIES
============================================================

1. RENDERER CORE (Pillar 1/2)
   - DOES: draw pixels, handle transforms, apply materials.
   - DOES NOT: know about "Selection", "Toolbars", or "Snapping."

2. SCENE CORE (Pillar 3/4)
   - DOES: store the node tree, manage IDs, handle object links.
   - DOES NOT: know about the mouse/keyboard or active tool state.

3. INTERACTION LAYER (Pillar 6)
   - DOES: handle hit testing, manage selection state, drive tool-specific
     operation (drag, connect, place).
   - DOES NOT: own the authoritative scene data or draw pixels.

4. APPLICATION SHELL / PACKAGE (Pillar 7)
   - DOES: host the windows/viewport, provide toolbars, gate permissions.
   - DOES NOT: perform the math for dragging or snapping.

============================================================
THE INPUT LOOP
============================================================

1. Platform Shell receives OS input (MouseDown at X, Y).
2. Shell passes event to the Viewport Controller.
3. Interaction Layer requests a Hit Test from the Scene Core.
4. Scene Core uses the Render Graph bounds to identify the Node.
5. Interaction Layer updates the Selection State.
6. Scene Core marks the selected Node as 'Selected' (Prop Override).
7. Renderer Core draws the selection highlight in the next frame.

============================================================
TOOL BOUNDARIES
============================================================

Tools should be modular.

- SELECTION TOOL: handles box select, single select, move.
- WIRE TOOL: handles anchor-to-anchor path creation.
- PLACEMENT TOOL: handles "ghost" previews and coordinate snapping.

Tools interact with the scene via the Scene Core API.
Tools NEVER interact with the GPU directly.

============================================================
GHOSTS AND PREVIEWS
============================================================

"Interaction Primitives" (selection boxes, wire previews, placement ghosts)
should ideally be drawn into a separate OVERLAY SCENE.

This prevents temporary UI states from dirtying the authored scene model.

============================================================
BOTTOM LINE
============================================================

Keep interaction logic outside the renderer.
If a component needs to "Turn Red when hovered", the Interaction Layer
sets a property on the node, and the renderer simply draws it.
The renderer doesn't know it's being "Hovered."
It just knows its "Hover_State" property changed.
