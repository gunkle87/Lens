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
