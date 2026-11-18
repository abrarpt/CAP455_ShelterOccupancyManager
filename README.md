# Earthquake Shelter Occupancy Manager

CAP455 - Object-Oriented Programming using C++ (MCA, Session 2026-27) - Minor Project

A simple C++ console program that records earthquake relief shelters (name, area,
capacity, people inside) and shows vacancy and status (AVAILABLE / ALMOST FULL / FULL).

## Build and run
Run everything from the project root (the folder that contains `src/` and `data/`).

    g++ -std=c++11 -Wall -o shelter src/main.cpp
    ./shelter            (Windows: shelter.exe)

## Features by phase
| Phase | Features |
|---|---|
| CA1 | Shelter class (static, inline, friend), array of objects, pointers, this, std::string, validation, no duplicate IDs (in memory) |
| CA2 | Everything from CA1 (array of objects, pointers, static/inline/friend) plus: file is loaded into the array at start; default, parameterised and copy constructors, destructor, binary file `data/shelters.dat` with fixed-size records (72 bytes), search by ID with seekg, view by record number, basic update of occupancy with seekp, data survives restart |
| CA3 | planned: inheritance, operator overloading, dynamic memory, virtual functions, safe update and delete |
| CA4 | planned: full CRUD menu, exceptions, templates, STL, advanced feature |

## Menu (CA2)
1. Add new shelter  2. View all  3. Search by ID  4. View by record number
5. Update people inside  0. Exit

## Data file
`data/shelters.dat` is a binary file. Every record is 72 bytes:
id (4) + name (30) + area (30) + capacity (4) + occupied (4).
A sample file with 6 shelters is included. Delete the file to start with an empty database.
The file is meant for the same compiler/computer; it is not a text file, so do not edit it by hand.

## Tests
    bash tests/run_tests.sh
rebuilds the program, resets the data file, runs 10 test groups and writes
`tests/CA2_test_evidence.txt` (note: it leaves the sample data in `data/`).

## Repository layout
    /docs    proposal, data dictionary, UML, flowcharts (CA1), CA2 report
    /src     main.cpp
    /data    shelters.dat (sample data)
    /tests   run_tests.sh, CA2_test_evidence.txt
    README.md
