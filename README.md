# Organized Crime-Fighting Simulation (ENCS4330 Project 3)

This project is a real-time multi-threaded and multi-process simulation of secret agents infiltrating organized crime gangs. It's developed as part of the Real-Time Applications & Embedded Systems course at Birzeit University (ENCS4330, 2nd semester 2024/2025).

## Project Description

The simulation models the interactions between criminal gangs and police agents secretly planted within them. Each gang plans and executes crimes while trying to detect moles. Meanwhile, the secret agents gather information and report back to the police, who attempt to stop the crimes without exposing their agents.

## File Structure

| File / Folder                 | Description                                                                 |
| ----------------------------- | --------------------------------------------------------------------------- |
| `main.c`                      | Entry point of the simulation, initializes modules.                         |
| `config.txt`                  | User-defined configuration file (e.g., number of gangs, ranks, thresholds). |
| `config_loader.c/h`           | Loads and parses the configuration from file.                               |
| `constants.h`                 | Global constants and macros.                                                |
| `structs.h`                   | Shared structures for agents, gangs, police, etc.                           |
| `gang.c/h`                    | Gang logic: member generation, promotion, crime planning, and execution.    |
| `police.c/h`                  | Police logic: agent behavior, suspicion evaluation, reporting.              |
| `police_process.c`            | Separate process for handling police activities and summaries.              |
| `simulation.c/h`              | Core simulation control and logic.                                          |
| `colors.h`                    | Terminal color codes for output formatting.                                 |
| `run_all.sh`                  | Bash script to compile and run the full simulation.                         |
| `Run_simulation`              | Executable output (may vary per OS).                                        |
| `sim`                         | Named pipe or executable component used in IPC.                             |
| `project3_pthread_202502.pdf` | The full project specification document.                                    |

## How to Compile and Run

1. Make sure you have the following installed:

   * GCC (with `-pthread` and `-lm` support)
   * POSIX-compliant OS (Linux recommended)
   * OpenGL (GLUT) libraries (if OpenGL visualization is added)

2. Compilation:

```bash
chmod +x run_all.sh
./run_all.sh
```

3. Execution:

```bash
./Run_simulation config.txt
```

`config.txt` must include all user-defined parameters (e.g., number of gangs, success thresholds, etc.)

## Features

* Multi-processing (e.g., separate police process)
* Multi-threading for gang member simulation
* Configurable simulation parameters (via `config.txt`)
* Realistic agent suspicion modeling and false info spread
* Gang promotion, internal investigation, agent execution
* Color-coded terminal output
* Optional OpenGL visualization (simple UI for simulation flow)

## End Conditions

Simulation ends when one of the following happens:

* Gangs successfully complete a user-defined number of missions.
* Police thwart a threshold number of gang activities.
* Too many agents get uncovered and executed.

## Notes

* Designed for academic purposes.
* The logic is modular, easily extendable, and customizable.
* Debugging info and logging can be enabled via flags in the source.

## Course Info

* Birzeit University
* ENCS4330 – Real-Time & Embedded Systems

## License

MIT License (for academic and non-commercial use)
