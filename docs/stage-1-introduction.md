# Stage 1 - Introduction

## Objective

Detect unauthorized changes to selected Linux files and make the audit history
itself verifiable.

## Scope

The MVP supports baseline creation, one-shot scanning, content and metadata
comparison, hash-linked events, reports, and log verification. Real-time
`inotify`, authentication, and remote log shipping are planned extensions.

## Expected outcome

An administrator can demonstrate a clean baseline, modify a file, run a scan,
see a `MODIFIED` event, and prove that the event chain is valid.
