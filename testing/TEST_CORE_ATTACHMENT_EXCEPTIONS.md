# Lens Test Core Attachment Exceptions Register

This file records any unavoidable temporary attachment between testing tooling
and core runtime code paths.

Default policy is zero attachments.
If there are no active exceptions, keep the register empty except for this
template header.

## Exception Entry Template

### Exception ID
- `EXC-YYYYMMDD-<slug>`

### Status
- `OPEN` | `REMOVED`

### Date Opened
- `YYYY-MM-DD HH:MM TZ`

### Scope
- Affected files:
  - `<path>`
- Affected layer/pillar:
  - `<layer name>`

### Why Attachment Is Unavoidable
- `<reason>`

### Attachment Description
- `<what is attached and where>`

### Safety Boundaries
- `<how production/runtime behavior is protected>`

### Required In-Code Markers
- Confirmation that code is heavily commented for removal.
- Confirmation that comments include explicit removal steps.

### Removal Plan
- Removal target milestone/task:
  - `<task or pillar checkpoint>`
- Exact removal steps:
  1. `<step>`
  2. `<step>`

### Verification After Removal
- Test command(s):
  - `<canonical runner command>`
- Expected result:
  - `100% pass (current + previous tests)`

### Closure Record
- Date removed:
  - `YYYY-MM-DD HH:MM TZ`
- Commit:
  - `<HASH>`
- Notes:
  - `<notes>`

---

### Exception ID
- `EXC-20260318-p1-runtime-core-ctypes`

### Status
- `OPEN`

### Date Opened
- `2026-03-18 21:58 CDT`

### Scope
- Affected files:
  - `testing/tests/test_pillar_1_transform_resolve_pass.py`
  - `testing/tests/test_pillar_1_clip_resolve_pass.py`
  - `testing/tests/test_pillar_1_ordering_resolve_pass.py`
  - `testing/tests/test_pillar_1_primitive_emit_pass.py`
  - `testing/tests/test_pillar_1_backend_boundary_interface.py`
  - `testing/tests/test_pillar_1_integration_validation.py`
- Affected layer/pillar:
  - `Pillar 1 (Native Render)`

### Why Attachment Is Unavoidable
- Current behavior-level runtime tests must execute native C resolver/emitter/
  boundary/pipeline code paths to validate deterministic runtime contracts.
- A dedicated non-attached native test adapter layer is not yet available.

### Attachment Description
- Each listed test compiles core C source files into a temporary DLL using
  `subprocess.run(... gcc ... -shared ...)`.
- The temporary DLL is loaded directly into Python tests via `ctypes.CDLL(...)`
  and invoked through explicit ctypes signatures.

### Safety Boundaries
- Attachment is test-only and lives under `testing/tests/`.
- No production runtime path imports or depends on these Python test harnesses.
- Runtime linkage is ephemeral in temporary directories and only during test
  execution.

### Required In-Code Markers
- Confirmed: all listed files include
  `TEST-CORE-ATTACHMENT-EXCEPTION: EXC-20260318-p1-runtime-core-ctypes`
  markers directly above subprocess compile and ctypes load points.
- Confirmed: each marker block includes explicit removal steps.

### Removal Plan
- Removal target milestone/task:
  - `Pillar 1 closure hardening follow-up (replace direct core ctypes runtime attachment with approved adapter)`
- Exact removal steps:
  1. Introduce approved non-attached native test adapter for Pillar 1 runtime
     behavior checks.
  2. Replace direct gcc/ctypes harness paths in all listed tests with adapter
     calls.
  3. Delete `TEST-CORE-ATTACHMENT-EXCEPTION` markers from all listed files.
  4. Re-run canonical runner evidence and raw suite verification.
  5. Mark this exception `REMOVED` with closure timestamp + commit hash.

### Verification After Removal
- Test command(s):
  - `python testing/test_runner.py --mode pillar --pillar 1`
  - `python -m unittest discover -s testing/tests -p "test_pillar_1_*.py" -v`
- Expected result:
  - `100% pass (current + previous tests)`

### Closure Record
- Date removed:
  - `YYYY-MM-DD HH:MM TZ`
- Commit:
  - `<HASH>`
- Notes:
  - `Pending removal.`
