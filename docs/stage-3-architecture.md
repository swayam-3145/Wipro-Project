# Stage 3 - Architecture

```text
CLI -> AuditController -> Scanner -> HashEngine
                       -> Baseline/Repository
                       -> ChangeDetector -> Hash-linked Logger
                       -> Report Generator
```

The repository uses TSV files so the tool can be built without external
dependencies. The baseline is trusted input and must be created after the
operator has verified the system is clean.

## Event hash

`SHA256(id|type|path|old_hash|new_hash|old_meta|new_meta|timestamp|previous_hash)`

Any altered or removed event breaks verification at that event or the next one.
