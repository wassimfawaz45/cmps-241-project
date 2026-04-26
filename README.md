# cmps-241-project
Project game (making boxes)
# Sprint 3: Network Programming and Multithreading

This project extends the Sprint 1 and 2 Dots and Boxes game so that two players can
play from two different machines over TCP.

## Features

- Host/client multiplayer over IPv4 TCP sockets
- Turn-based synchronization by sending moves over the network
- Background match logger thread using `pthread`
- Mutex-protected shared logger state to avoid race conditions
- Same board rules, scoring, and extra-turn behavior from Sprint 1

## Build

```bash
make
```

## Run

Start the server on the machine that will play as Player A:

```bash
./dotsandboxes --host 5000
```

Connect from the second machine as Player B:

```bash
./dotsandboxes --connect 192.168.1.10 5000
```

Replace `192.168.1.10` with the IP address of the host machine.

## Multithreading

The program starts a background logger thread that periodically writes match
snapshots to `match_log.txt`. The main game thread updates shared logger state
after every move, and a mutex protects that shared state.

## Input Format

Players enter moves as:

```text
r1 c1 r2 c2
```

Example:

```text
0 0 0 1
```

This draws a horizontal line between two adjacent dots.
