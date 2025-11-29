# Exam Timetable Generator in C

This project implements a complete menu-driven **Exam Timetable Generation System** using **Greedy Ordering** and **Backtracking**.  
The program assigns exams to valid rooms, timeslots, and available faculty members while ensuring that no conflicts occur.

---

## Features

- Fully menu-driven (no hard-coded values required)
- Configure:
  - Number of exams
  - Number of rooms
  - Number of faculties
  - Room names and capacities
  - Subject names and student counts
  - Faculty availability
- Optional automatic generation of:
  - Rooms
  - Faculty names
  - Subject names
  - Conflict groups
- Automatically generates weekday timeslots between two dates:
  - Morning session: 09:00–12:00
  - Afternoon session: 14:00–17:00
- Uses a combination of Greedy Optimization + Backtracking Search
- Prevents:
  - Room clashes
  - Faculty clashes
  - Exam conflicts
  - Room capacity overflow
- Prints a clean, aligned exam timetable

---

## Algorithms Used

### 1. Greedy Ordering
All exams are sorted in descending order based on:
1. Number of conflicts
2. Number of students

This reduces backtracking complexity.

### 2. Backtracking
For each exam, the solver attempts to place it in:
- Every available timeslot
- Every available room

A placement is valid only if:
- The room capacity is sufficient
- The room is free at that timeslot
- The faculty is available
- No conflicting exam is already scheduled in that timeslot

If no placement is possible, the algorithm backtracks.

---

## Project Structure

Exam_TimeTable_Generator/
│── menu_exam_timetable.c
│── README.md
└── (optional output files)

yaml
Copy code

---

## How to Compile and Run

### Compile
gcc menu_exam_timetable.c -o timetable

shell
Copy code

### Run
./timetable

makefile
Copy code

Windows:
timetable.exe

yaml
Copy code

---

## Input Flow

The program will prompt for:

1. Number of exams  
2. Number of rooms  
3. Number of faculty members  
4. Start and end dates  
5. Whether to manually enter or auto-generate:
   - Room names and capacities
   - Faculty names and availability
   - Subject names and student counts
   - Exam conflict groups

After all inputs, the program generates and prints a timetable.

---

## Sample Output Format

=== EXAM TIMETABLE GENERATED SUCCESSFULLY ===

Exam | Subject | Students | Faculty | Date & Time | Room
0 Calculus_12 50 Prof_Alex_Sharma 2025-05-03 Monday 09:00-12:00 Room_A_201
1 Physics_43 45 Prof_Dylan_Kaur 2025-05-03 Monday 14:00-17:00 Room_B_310
...

yaml
Copy code

---

## Requirements

- GCC compiler
- Windows, Linux, or macOS

---

## Possible Enhancements

- Export timetable to CSV or PDF
- Build a GUI frontend
- Add support for multiple departments
- Implement a genetic algorithm version

---

## License

This project is open-source and free to use or modify.
