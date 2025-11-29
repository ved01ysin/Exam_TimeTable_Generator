# Exam Timetable Generator in C

A complete menu-driven **Exam Timetable Generator** built in C using **Greedy Ordering** and **Backtracking**.  
It assigns exams to valid rooms, timeslots, and faculty members while ensuring no conflicts occur.

---

## Features

- Menu-driven interface  
- User-configurable:
  - Number of exams  
  - Number of rooms  
  - Number of faculties  
  - Room names & capacities  
  - Subjects & student counts  
  - Faculty availability  
- Optional auto-generation of:
  - Rooms  
  - Faculties  
  - Subjects  
  - Conflict groups  
- Generates weekday-only timeslots:
  - 09:00–12:00  
  - 14:00–17:00  
- Ensures:
  - No room clashes  
  - No faculty clashes  
  - No exam conflicts  
  - Room capacity is respected  
- Prints a clean, aligned exam timetable  

---

## Algorithms Used

### Greedy Ordering
Exams are sorted in descending order based on:
1. Conflict count  
2. Number of students  

This reduces the time needed during backtracking.

### Backtracking
Tries to place each exam in:
- Every timeslot  
- Every room  

A placement is valid only if:
- Room capacity is sufficient  
- Room is free  
- Faculty is available  
- No conflicting exam is already in the same slot  

If a placement fails, it backtracks.

---

## Project Structure

  Exam_TimeTable_Generator/ <br>
  │── menu_exam_timetable.c <br>
  │── README.md <br>
  └── (optional files) <br>

---


---

## Input Flow

The program prompts for:

1. Number of exams  
2. Number of rooms  
3. Number of faculties  
4. Start & end dates  
5. Optional manual entry or auto-generation of:
   - Room names & capacities  
   - Faculty names & availability  
   - Subject names & student counts  
   - Exam conflict groups  

After input collection, the algorithm generates a complete timetable.

---


---

## Requirements

- GCC compiler  
- Windows / Linux / macOS  

---

## Possible Enhancements

- Export timetable to CSV/PDF  
- Add GUI interface  
- Multi-department support  
- Genetic algorithm optimization  

---

## License

Open-source and free to modify.





