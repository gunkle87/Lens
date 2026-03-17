# TRACE AND PORT MODEL

============================================================
PURPOSE
============================================================

This document defines the semantic connection model for the LENS scene.
It handles how logical components are wired together.

============================================================
PRIMARY RULE
============================================================

Ports are semantic endpoints in a component.
Traces are semantic scene objects that bind two ports.

============================================================
ANCHOR AND PORT DEFINITIONS
============================================================

Inside a ComponentDef:

- Anchor: A named position (X, Y) relative to the component center.
- Port: A named semantic endpoint tied to an Anchor.

Port Fields:

- Name: string (e.g., "Input1", "Bus_A")
- Direction: Enum (INPUT, OUTPUT, BIDIR)
- Width: int (number of lines/bits)
- AnchorID: string (Reference to defined anchor)

============================================================
TRACE INSTANCE
============================================================

A TraceInstance is a first-class SceneNode.

Trace Fields:

- Source: SceneIdentity (SceneID + NodeID of component)
- SourcePort: string (Name of port on source component)
- Destination: SceneIdentity
- DestinationPort: string
- StyleID: string (Reference to TraceStyleDef)
- Vertices: list of Vector2 (Relative points or absolute board points)

============================================================
RENDERER RESPONSIBILITIES
============================================================

The renderer does NOT need to know that a trace is a "Signal."
The renderer only needs to know:

- path data (generated from vertices)
- material (from style)
- any effect (e.g., animated glow) requested by the style

============================================================
EDITOR RESPONSIBILITIES
============================================================

The editor handles trace creation and editing.

- "Rubber-banding" during creation
- Snapping to ports
- Re-routing when a component moves
- Breaking a trace if a component is deleted

============================================================
BOTTOM LINE
============================================================

Traces are not just "Lines." They are the semantic glue of the simulation.
Treat trace endpoints as stable identities, not raw coordinates.
This allows the UI to stay connected even when components are moved.
