# 📅 Smart Course Scheduler (Constraint-Based Scheduling)
**Final Project: Data Structures and Algorithms**
**Institution:** VNU-HCM University of Science

**Instructor:** Lê Trung Hoàng

**Team Members:**
1. Đoàn Khánh Hưng - 25127195
2. Nguyễn Cao An Bình - 25127183
3. Tô Phúc Thành - 25127499

---

## 📝 1. Project Overview
The **Smart Course Scheduler** is an advanced C++ console application designed to solve the complex problem of university timetable generation. Moving beyond simple conflict checking, this program models the scheduling process as a **Weighted Interval Scheduling** problem.

But it doesn't stop at classes. Once the optimal academic schedule is generated, the system intelligently scans for "gaps" in your day and seamlessly integrates your Personal Activities (e.g., Gym, Reading, Part-time jobs) based on your preferred time periods.

It allows students to input their personal preferences and strict constraints. The system then evaluates the dataset (`data.csv`) and outputs a globally optimal, non-overlapping schedule in the `.ics` (iCalendar) format, which can be seamlessly imported into Google Calendar, Apple Calendar, or Microsoft Outlook.

## 🚀 2. Advanced Multi-Constraint Features
Our system implements a sophisticated two-stage filtering pipeline to handle both soft and hard constraints:

### 🎯 Stage 1: Soft Constraints (Priority Weighting)
Instead of filtering, these constraints dynamically modify the `weight` property of each course, guiding the optimization algorithm to prefer certain classes:
* **Time Preference:** Users can prioritize **Early Bird** (morning classes, `start_time < 12:00`) or **Lazy Mode** (afternoon classes).
* **Preferred Days:** Users can selectively boost the weight of courses that fall on their favorite study days.

### 🚫 Stage 2: Hard Constraints (Strict Pruning & Greedy Selection)
These constraints aggressively filter out undesirable courses or states before the main algorithm runs:
* **Banned Days (Day-Off Mode):** Completely blocks user-defined days (e.g., keeping Fridays entirely free for part-time jobs).
* **Lunch Break Saver:** Detects and drops any class that overlaps with the crucial 11:30 - 13:30 window.
* **Minimum Break Enforcement:** Dynamically ensures a user-defined minimum gap (e.g., 15 or 30 minutes) between consecutive classes for commuting or resting. This is elegantly integrated directly into the `isCompatible` validation function.
* **Max Classes Per Day:** Limits daily cognitive load by sorting courses per day by weight and trimming the excess.
* **Max Study Days Per Week:** Utilizes a **Greedy Algorithm** to find the $N$ days with the highest potential weight sum, dropping classes scheduled on all other days.

### 🏋️ Stage 3: Life Integration & Gap-Filling
Once the optimal course schedule is locked, the system switches to personal life management:

* **Smart Activity Placement:** Users can input personal activities (e.g., Gym, English) with specific durations and preferred periods (Morning, Afternoon, Evening, or Anywhere).

* **Gap Detection:** The algorithm scans the intervals between classes and safely injects personal activities without causing overlaps.

* **Free Time Generation:** Any remaining empty blocks in the active day (from ```activeStartMin``` to ```activeEndMin```) are automatically labeled as "Free Time", giving users a complete 360-degree view of their day.

## 🧠 3. Algorithms & Data Structures
This project demonstrates a deep understanding of DSA concepts, combining them to achieve optimal performance:

### The Core Engine: Dynamic Programming (O(n log n))
The backbone of the scheduler is a 1D Dynamic Programming approach. 
* Courses are grouped by day, and within each day, they are sorted by end_time to ensure the correctness of the Binary Search and DP state transitions.
* A state array `dp[i]` stores the maximum possible weight achievable using a subset of the first $i$ courses.
* **Binary Search:** To find the previous compatible course without overlapping, we implemented a custom `findPreviousNonConflict` function using Binary Search, optimizing the state transition from $O(n)$ to $O(\log n)$.

### Advanced Data Structures Utilized
* `std::vector`: For dynamic arrays and course list management.
* `std::map`: Used to group courses by day (`map<int, vector<Course>>`) and calculate daily weight totals (`map<int, int>`).
* `std::set`: Used to store the unique IDs of "allowed days" during the Greedy max-study-days calculation.

## ⚙️ 4. Program Architecture
The source code is modularized for high maintainability:
* **`main.cpp`**: Handles the UI (Console), constraint pipeline, and the main DP logic.
* **`Activity.h`**: Defines the PersonalActivity struct for user inputs and the unified ScheduleBlock struct for the final timeline.
* **`Course.h`**: Defines the `Course` struct with custom operator overloading (`operator<`) for seamless sorting by end time.
* **`DataLoader.h`**: Responsible for parsing the comma-separated `data.csv` file and converting `HH:MM` string formats into integer minutes for fast computation.
* **`SchedulePlanner.h`**: The engine for Phase 3. It converts courses into ScheduleBlocks, intelligently injects personal activities into valid time gaps, and calculates "Free Time".
* **`CalendarExport.h`**: Formats the selected courses into standard iCalendar syntax (`BEGIN:VCALENDAR`, `RRULE:FREQ=WEEKLY;COUNT=15`, etc.) and outputs `Schedule.ics`.

## 💻 5. Compilation & Execution

**Prerequisites:** Ensure you have a C++ compiler (like GCC/G++) and the `data.csv` file in the working directory.

**To compile the source code:**
```bash
g++ main.cpp -o SmartScheduler
```
To run the application:

Windows: ```.\SmartLifePlanner.exe```

macOS/Linux: ```./SmartLifePlanner```

(Follow the on-screen prompts to set your academic constraints, add your personal activities, and let the algorithm do the heavy lifting!)
