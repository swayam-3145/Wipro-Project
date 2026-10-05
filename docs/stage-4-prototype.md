# Stage 4 - Prototype and Progress Evidence

## Demonstration

1. Create `demo/app.conf`.
2. Run `fiaudit init demo`.
3. Change the file.
4. Run `fiaudit scan`.
5. Show `MODIFIED` and its old/new hashes.
6. Run `fiaudit verify-log`.

## Suggested commits

```text
docs: add introduction and requirements
feat: add scanner and sha256 engine
feat: add baseline and audit repository
feat: add change detection and cli
```
