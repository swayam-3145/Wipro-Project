# Stage 5 - Testing and Integration

Test cases:

| Case | Expected result |
|---|---|
| unchanged file | no event |
| changed contents | `MODIFIED` |
| deleted file | `DELETED` |
| new file | `ADDED` |
| changed mode | `PERMISSION_CHANGED` |
| edited event row | verification fails |
| unreadable file | explicit `ERROR` |

Build with warnings enabled and add unit tests around `sha256`, `detect`, and
`verify_chain`. Integration testing should cover the local repository files
and their error handling.
