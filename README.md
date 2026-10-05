# File Integrity & Tamper-Evidence Audit System

## 1. Project summary

`fiaudit` is a Linux C++ command-line application for detecting unauthorized
file changes. It creates a trusted baseline, calculates SHA-256 hashes during
future scans, detects additions/modifications/deletions and selected metadata
changes, and stores findings in a hash-linked audit log.

The executable uses a local TSV file repository, so it has no database
configuration or external service dependency.

## Quick run guide

Run these commands from Ubuntu, Debian, or WSL Ubuntu:

```bash
cd "/mnt/c/Users/GYANA PRAKASH/OneDrive/Desktop/wipro"
sudo apt update
sudo apt install -y g++ make
make

rm -rf trainer-demo .fiaudit-demo
mkdir -p trainer-demo
printf '%s' 'original configuration' > trainer-demo/config.txt
printf '%s' 'admin' > trainer-demo/users.txt
export FIAUDIT_HOME="$PWD/.fiaudit-demo"

./fiaudit init ./trainer-demo
./fiaudit scan
```

Test change detection:

```bash
printf '%s' 'tampered configuration' > trainer-demo/config.txt
printf '%s' 'unauthorized file' > trainer-demo/malware.txt
rm trainer-demo/users.txt

./fiaudit scan || true
./fiaudit report
./fiaudit status
./fiaudit verify-log
```

The scan reports `MODIFIED`, `ADDED`, and `DELETED` files. A scan that finds
changes intentionally exits with status `3`, which is why the demonstration
uses `./fiaudit scan || true`.

## 2. Important: use a Linux terminal

Run the commands below in Ubuntu, Debian, or WSL Ubuntu. The prompt normally
looks like this:

```text
user@computer:~$
```

Do not run Linux commands in Windows PowerShell. PowerShell commands such as
`Remove-Item` and Linux commands such as `rm -rf` are different.

## 3. Open the project

For WSL, the project created on the Windows desktop is available at:

```bash
cd "/mnt/c/Users/GYANA PRAKASH/OneDrive/Desktop/wipro"
```

For a normal Linux checkout, use the actual checkout directory, for example:

```bash
cd ~/wipro
```

Confirm the directory:

```bash
pwd
ls
```

You should see `src`, `docs`, `Makefile`, and this `README.md`.

## 4. Install the compiler

Run this once if `g++` is not installed:

```bash
sudo apt update
sudo apt install -y g++
```

## 5. Build the application

Compile all C++ modules into the Linux executable `fiaudit`:

```bash
g++ -std=c++11 -Wall -Wextra -O2 -Isrc \
  src/main.cpp \
  src/sha256.cpp \
  src/scanner.cpp \
  src/repository.cpp \
  src/audit.cpp \
  -o fiaudit
```

What this does:

| Source file | Responsibility |
|---|---|
| `src/main.cpp` | Command-line commands |
| `src/sha256.cpp` | SHA-256 content hashing |
| `src/scanner.cpp` | File and metadata scanning |
| `src/repository.cpp` | Baseline and event persistence |
| `src/audit.cpp` | Change detection and hash-chain verification |

Check the executable:

```bash
ls -l ./fiaudit
./fiaudit
```

The usage message confirms that the build succeeded.

## 6. Start a clean demonstration

These commands remove only the demonstration data and compiled executable:

```bash
rm -f fiaudit
rm -rf trainer-demo .fiaudit-demo
```

If you just built the executable, run the build command in section 5 again
after this cleanup.

Create a test directory and two trusted files:

```bash
mkdir -p trainer-demo
printf '%s' 'original configuration' > trainer-demo/config.txt
printf '%s' 'admin' > trainer-demo/users.txt
ls -l trainer-demo
```

The directory should contain `config.txt` and `users.txt`.

## 7. Select the repository location

```bash
export FIAUDIT_HOME="$PWD/.fiaudit-demo"
echo "$FIAUDIT_HOME"
```

This tells `fiaudit` to store its local data in `.fiaudit-demo`. The program
creates:

| File | Purpose |
|---|---|
| `baseline.tsv` | Trusted file hashes and metadata |
| `roots.txt` | Directories selected for monitoring |
| `events.tsv` | Hash-linked audit events |

The `export` command applies to the current terminal session.

## 8. Create the trusted baseline

```bash
./fiaudit init ./trainer-demo
```

Expected output:

```text
Baseline created for 2 path(s).
```

What it does:

1. Recursively scans `trainer-demo`.
2. Reads each regular file.
3. Calculates its SHA-256 content hash.
4. Records size, modification time, permissions, owner, and group.
5. Saves that state as the trusted baseline.

The baseline must be created when the files are known to be clean.

## 9. Perform a clean scan

```bash
./fiaudit scan
```

Expected output:

```text
Scanned 2 path(s); 0 change(s) detected.
```

What it does:

1. Scans the current files.
2. Calculates current hashes and metadata.
3. Loads the baseline.
4. Compares current values with the baseline.
5. Writes no event when everything matches.

## 10. Test modification, addition, and deletion

### 10.1 Modify an existing file

```bash
printf '%s' 'tampered configuration' > trainer-demo/config.txt
```

The content changed, so its SHA-256 hash is different from the baseline.

### 10.2 Add a new file

```bash
printf '%s' 'unauthorized file' > trainer-demo/malware.txt
```

This file was not present when the baseline was created.

### 10.3 Delete a baseline file

```bash
rm trainer-demo/users.txt
```

Check the current state:

```bash
ls -l trainer-demo
```

The directory should now contain `config.txt` and `malware.txt`; `users.txt`
should be absent.

## 11. Detect the changes

```bash
./fiaudit scan
```

Expected output, in any order:

```text
Scanned 2 path(s); 3 change(s) detected.
MODIFIED: ./trainer-demo/config.txt
DELETED: ./trainer-demo/users.txt
ADDED: ./trainer-demo/malware.txt
```

The scan returns exit status `3` when changes are found. That is intentional
and useful for shell scripts or monitoring systems; it is not a build failure.

Meaning of each event:

| Event | Meaning |
|---|---|
| `MODIFIED` | A baseline file's content hash changed |
| `DELETED` | A baseline file no longer exists |
| `ADDED` | A new file exists outside the baseline |
| `PERMISSION_CHANGED` | Permission bits changed |
| `OWNER_CHANGED` | User or group ownership changed |
| `ERROR` | The file could not be inspected |

## 12. Display reports

Human-readable report:

```bash
./fiaudit report
```

Machine-readable JSON report:

```bash
./fiaudit report --json
```

Show a summary:

```bash
./fiaudit status
```

## 13. Verify tamper evidence

```bash
./fiaudit verify-log
```

Expected output:

```text
Audit chain valid (3 event(s)).
```

Each event contains the previous event's hash. Its own hash is calculated from
the event data and that previous hash. Therefore, editing or removing an old
event causes verification to fail.

To demonstrate this safely, first back up the log:

```bash
cp "$FIAUDIT_HOME/events.tsv" "$FIAUDIT_HOME/events.backup.tsv"
```

Edit the event file:

```bash
nano "$FIAUDIT_HOME/events.tsv"
```

Change one character, save, and run:

```bash
./fiaudit verify-log
```

Expected result:

```text
INVALID: chain failure at event 1
```

Restore the valid log:

```bash
mv "$FIAUDIT_HOME/events.backup.tsv" "$FIAUDIT_HOME/events.tsv"
./fiaudit verify-log
```

It should again report that the audit chain is valid.

## 14. Complete trainer demonstration

After compiling, the following block runs the complete demonstration. The
`|| true` allows the script to continue because a changed scan intentionally
returns status `3`.

```bash
rm -rf trainer-demo .fiaudit-demo
mkdir -p trainer-demo
printf '%s' 'original configuration' > trainer-demo/config.txt
printf '%s' 'admin' > trainer-demo/users.txt
export FIAUDIT_HOME="$PWD/.fiaudit-demo"

./fiaudit init ./trainer-demo
./fiaudit scan

printf '%s' 'tampered configuration' > trainer-demo/config.txt
printf '%s' 'unauthorized file' > trainer-demo/malware.txt
rm trainer-demo/users.txt

./fiaudit scan || true
./fiaudit report
./fiaudit report --json
./fiaudit verify-log
./fiaudit status
```

## 15. Security limitation

The local hash chain is tamper-evident, not tamper-proof. A root user could
replace the application and local repository. Production hardening should use
least-privilege accounts, protected baseline backups, remote append-only
logging, and signed report exports.

## 16. Project documentation

The six project stages are documented in [`docs/`](docs/):

1. [`stage-1-introduction.md`](docs/stage-1-introduction.md)
2. [`stage-2-requirements.md`](docs/stage-2-requirements.md)
3. [`stage-3-architecture.md`](docs/stage-3-architecture.md)
4. [`stage-4-prototype.md`](docs/stage-4-prototype.md)
5. [`stage-5-testing.md`](docs/stage-5-testing.md)
6. [`stage-6-final-presentation.md`](docs/stage-6-final-presentation.md)
