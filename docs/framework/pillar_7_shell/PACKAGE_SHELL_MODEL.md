# PACKAGE SHELL MODEL

============================================================
PURPOSE
============================================================

This document defines how "Packages" (applications) wrap the Lens Engine.
It defines the boundaries for toolsets, layouts, and interaction
permissions.

============================================================
PRIMARY RULE
============================================================

The Core Engine provides the capability; the Package Shell provides the
discipline and identity.

============================================================
SHELL RESPONSIBILITIES
============================================================

1. TOP-LEVEL LAYOUT
   - housing the viewport
   - placing toolbars, sidebars, properties panels
   - managing overlays

2. TOOLSET GATING
   - which tools are available (e.g., game mode may disable "Place Gate")
   - cursor behavior

3. INTERACTION PERMISSIONS
   - can the user edit this board?
   - can the user change trace colors?
   - is "Simulate" running or paused?

4. ASSET DISCOVERY
   - providing the list of available ComponentDefs
   - providing the MaterialPalettes

============================================================
DECLARATIVE LAYOUT
============================================================

Ideally, the shell layout itself should be defined in a .lscn-like format.

Example:
```
panel { id: "toolbar", dock: "top", height: "48px" }
panel { id: "viewport", dock: "fill" }
panel { id: "properties", dock: "right", width: "250px" }
```

============================================================
CROSS-PACKAGE CONSISTENCY
============================================================

All packages built on Lens should share:

- the same coordinate system
- the same transform model
- the same ID/Reference model
- the same low-level render behavior

But they can differ 100% in:

- theme (colors, fonts, materials)
- component vocabulary
- interaction "feel" (snapping rules, tool availability)

============================================================
BOTTOM LINE
============================================================

The Shell is the "Execution Frame."
Use it to differentiate between a game, an editor, and a dashboard,
without forking the renderer.
