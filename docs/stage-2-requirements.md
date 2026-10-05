# Stage 2 - Requirements and Plan

## Functional requirements

1. Register files and directories.
2. Calculate SHA-256 content hashes.
3. Store a baseline.
4. Detect added, modified, deleted, permission, owner, and read errors.
5. Store hash-linked audit records.
6. Verify the complete chain.
7. Produce text and JSON reports.

## Non-functional requirements

The tool must be modular, deterministic, explicit about errors, efficient for
large files, and suitable for Linux deployment with restricted permissions.

## Timeline

| Stage | Result |
|---|---|
| 1 | Problem, objective, scope |
| 2 | Requirements and risks |
| 3 | Architecture and repository |
| 4 | Scanner, hashes, detector, CLI |
| 5 | Tests, integration, hardening |
| 6 | Demonstration and final report |
