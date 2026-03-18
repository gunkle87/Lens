# Pillar 1 Plan: Native Render

This plan breaks Pillar 1 into 8 equal-effort task chunks.

## Pillar Goal

Deliver a minimal, deterministic native render path that can accept resolved
draw data, enforce ordering/clipping/transform rules, and present frames
through a bounded backend interface.

## Effort Model

- Pillar 1 is split into 8 chunks.
- Each chunk is one task.
- Each task is scoped to a similar implementation/audit/test/commit effort.

## Task Chunks

### Task 1.1 — Render Data Contracts
Define core render records and handles for draw items, transforms, clipping,
and layer order input.

### Task 1.2 — Command Input Buffer
Implement deterministic command-buffer ingest for render records with explicit
capacity checks and stable indexing.

### Task 1.3 — Transform Resolve Pass
Implement transform resolution from authored/runtime input into render-ready
transform records.

### Task 1.4 — Clip Resolve Pass
Implement clip/scissor resolution pass and deterministic clip inheritance
behavior for draw items.

### Task 1.5 — Ordering Resolve Pass
Implement stable draw ordering pass (layer + explicit order key) and verify
repeatable output ordering.

### Task 1.6 — Primitive Emit Pass
Implement primitive emission pass that converts resolved commands into backend-
ready primitive batches.

### Task 1.7 — Backend Boundary Interface
Implement a narrow backend interface for submit/present boundaries without
embedding product or engine workflow logic.

### Task 1.8 — Pillar Integration And Validation
Integrate all Pillar 1 passes end-to-end, add required tests, and produce
evidence for task and pillar closure.
