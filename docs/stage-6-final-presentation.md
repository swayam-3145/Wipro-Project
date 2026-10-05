# Stage 6 - Final Presentation

Present the problem, architecture, data model, six-stage process, live
demonstration, test results, limitations, and future work.

## Limitations

Local storage can be changed by root. Hash chaining detects modification but
does not prevent it. A production deployment should send events to a separate
host, protect the baseline, and sign exports.

## Future improvements

- Linux `inotify`/`fanotify` watch mode
- Remote append-only logging
- Signed reports
- Authentication and roles
- Kernel event collection
