# SCENE SCRIPTING BOUNDARY

============================================================
PURPOSE
============================================================

This document defines the boundary between scripting (logic/extension)
and the core LENS renderer/scene system.

============================================================
PRIMARY RULE
============================================================

Scripts may talk to the Scene Model, but never the Render Graph.

Scripts are for manipulation, not for low-level drawing.

============================================================
ALLOWED OPERATIONS
============================================================

A script is allowed to:

- find nodes by ID or Name
- create new SceneNodes (Instances)
- delete SceneNodes
- update LocalTransform (move, rotate, scale)
- update PropertyOverrides (change color, text, visibility)
- subscribe to node events (click, hover, state change)
- request a scene-level save or export

============================================================
FORBIDDEN OPERATIONS
============================================================

A script is NEVER allowed to:

- bypass the SceneNode model to emit raw draw opcodes
- access the GPU or low-level rendering backend directly
- redefine the core primitive grammar
- modify the Asset Registry at runtime (definitions are immutable)
- bypass the Customization Tier checks

============================================================
COMMUNICATION MODEL
============================================================

Use a MESSAGE or API layer.

Correct:
`node.setProperty("color", "#FF0000")`

Incorrect:
`renderer.drawRect(node.worldPos, ...)`

============================================================
WHY THIS BOUNDARY?
============================================================

1. Stability: A script error cannot crash the renderer.
2. Portability: The same script can work on different rendering backends.
3. Security: Limits the damage a maliciously authored component can do.
4. Predictability: The renderer always knows the state of the scene.

============================================================
BOTTOM LINE
============================================================

Treat the renderer as an authoritative black box.
Scripts provide the intent; the renderer provides the execution.
Never let scripts reach into the draw call loop.
