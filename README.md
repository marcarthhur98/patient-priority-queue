# Patient Priority Queue

A small C command-line project that manages an educational patient queue with
a sorted singly linked list. Patients with higher severity are treated first;
patients with equal severity are treated in arrival order.

Built from Marc Arthur Kentsa's APS105 Lab 9 implementation. The original node
structure, queue operations, insertion order, and single-file organization are
preserved. Input validation and error handling were strengthened with AI assistance.

## Build and run

Requires a C11 compiler such as GCC. From this folder:

```sh
gcc -std=c11 -Wall -Wextra -Werror lab9.c -o patient-queue
```

Run `./patient-queue` on Linux/macOS or `./patient-queue.exe` in Windows PowerShell.
The app waits for commands; it does not display an interactive menu.
For Windows, `build.bat` builds `build/patient-queue.exe` instead.

## Commands

Enter **one command per line**. Commands are uppercase.

| Command | Action |
|---|---|
| `A <ID> <name> <severity>` | Add a patient; IDs must be unique in the current queue. |
| `T` | Treat and remove the next patient. |
| `R <ID>` | Remove a patient by ID. |
| `D` | Display IDs, names, and severities in treatment order. |
| `Q` | Free the remaining queue and quit. |

- IDs: positive integers that fit in a C `int`.
- Names: one word, 1-100 bytes; use underscores for multiple words.
- Severity: **1-5**, where **5 is highest priority**. This is a project-specific
  demonstration scale, not a clinical triage standard.
- Invalid commands leave the queue unchanged. Empty lines are ignored.
- Closing standard input also frees the queue and exits normally.
- Data is stored in memory for the current run only.

## Example

Input (fictional patients):

```text
A 1 Alex 2
A 2 Blair 5
A 3 Casey 5
D
T
D
Q
```

The first display shows Blair, Casey, then Alex. `T` treats Blair. The second
display shows Casey followed by Alex: equal-severity patients preserve arrival order.

## Design and complexity

`Node` stores an ID, a fixed-size name, a severity, and a next pointer. `Queue`
stores the head pointer. `addPatient` rejects duplicates before inserting into
descending severity order. `treatPatient` removes the head. `removePatient`
searches by ID; `endProgram` releases all remaining nodes.

| Operation | Time |
|---|---|
| Add, including duplicate check | O(n) |
| Treat next patient | O(1) |
| Remove by ID | O(n) |
| Display / cleanup | O(n) |

Space is O(n). A heap or hash table is unnecessary for this small teaching project.
Queue functions still print their original status messages to keep changes small.

## Reliability changes

- Read a complete line with `fgets` before processing it.
- Parse integer tokens with `strtol`, rejecting overflow and trailing characters.
- Reject missing/extra arguments, overlong names/commands, and out-of-range values.
- Report missing IDs consistently, including when the queue is empty.
- Report allocation failure and free remaining nodes on quit, EOF, or read error.

The severity range is new; the original lab accepted any integer. Names remain
single tokens, matching the original command format. Display now includes IDs.

## Tests

Requires Python 3 and GCC on PATH, with no additional Python packages:

```sh
python test_queue.py
```

Tests compile a fresh executable into `build/` with strict warnings and check
priority ordering, FIFO ties, duplicate IDs, removal cases, invalid input,
name-length boundaries, overflow, overlong-line recovery, and EOF. A C harness
also checks node cleanup and simulated allocation failures.
