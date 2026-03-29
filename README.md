# Smart Course Scheduler

Final Project - Data Structures and Algorithms  
Institution: VNU-HCM University of Science  
Instructor: Le Trung Hoang

Team members:
1. Doan Khanh Hung - 25127195
2. Nguyen Cao An Binh - 25127183
3. To Phuc Thanh - 25127499

## 1. Project description

This project is a course scheduling system built from the perspective of Data Structures and Algorithms. The main goal is to help a student create a weekly study plan from a dataset of available course sections while still taking personal constraints into account.

The input of the program is a CSV file containing course information such as course id, day of week, start time, end time, room, weight, semester, and type of class. Based on this data, the program selects a set of non-overlapping classes with high total priority, then continues to arrange personal activities into the remaining free time. The final result can be viewed as a weekly schedule and exported to an `.ics` calendar file.

At the moment, the folder contains two ways to use the system:

- a C++ console version in `main.cpp`
- a web interface using `index.html`, `style.css`, and `script.js`

The web version was added so the project can be demonstrated more easily and can later be deployed for public use. The core idea, however, is still the same: choose a suitable study schedule under constraints and complete the remaining time with personal activities.

## 2. Main objectives

The project is built to solve the following tasks:

- read course data from `data.csv`
- filter courses by semester
- apply both soft constraints and hard constraints
- choose a valid schedule with no overlap
- maximize the total priority of selected classes
- place personal activities into free slots
- generate a complete weekly plan including free time
- export the result to `Schedule.ics`

## 3. Input data

The main input file is `data.csv`. Each row has the following format:

`course_id, day, start_time, end_time, room, weight, semester, type`

Meaning of each column:

- `course_id`: identifier of the course or class section
- `day`: day of week, where `2 = Monday` and `8 = Sunday`
- `start_time`, `end_time`: time in `HH:MM`
- `room`: classroom or location
- `weight`: priority value used by the scheduling algorithm
- `semester`: semester number
- `type`: lecture, lab, or another type of session

The project currently includes sample data for several semesters in `data.csv`, so the program can be tested directly without preparing a new dataset.

## 4. Functionalities

### 4.1. Course scheduling

The system supports the following study-related settings:

- choose semester
- prefer early classes, late classes, or no time preference
- avoid specific days
- prioritize preferred study days
- limit the number of classes per day
- set a minimum break time between two consecutive classes
- avoid the lunch time block from 11:30 to 13:30
- limit the total number of study days in one week

After receiving these settings, the program selects a set of classes that does not overlap and has high total weight.

### 4.2. Personal activity planning

After the study schedule is chosen, the program allows the user to add repeating personal activities. For each activity, the user can provide:

- activity name
- category
- location or note
- duration
- preferred time period: morning, afternoon, evening, or any time
- list of days in the week

The scheduler scans the remaining free slots in the day and tries to place these activities without causing conflicts with the chosen classes.

### 4.3. Weekly schedule generation

The final output is not limited to selected classes only. The program also creates:

- personal activity blocks
- free time blocks between busy intervals
- warnings for activities that cannot be placed

As a result, the user receives a full weekly schedule rather than only a list of selected classes.

### 4.4. Calendar export

The project exports the schedule to `Schedule.ics`, which can be imported into common calendar tools such as Google Calendar, Apple Calendar, or Outlook. In the C++ version, the export logic is implemented in `CalendarExport.h`. In the web version, the export is handled in `script.js`.

## 5. Algorithms and data structures

This project mainly demonstrates the application of classic DSA techniques to a practical problem.

### 5.1. Weighted Interval Scheduling

The core scheduling problem is treated as a weighted interval scheduling problem. Each class is considered an interval with:

- starting time
- ending time
- weight

For each day, the classes are sorted by ending time. Then dynamic programming is used to compute the best total weight without overlap.

In the C++ version:

- `dp[i]` stores the best total weight that can be achieved using classes up to position `i`
- for each class, the program finds the nearest previous compatible class
- the transition compares two choices: skip the current class or take it and add the best compatible result before it

### 5.2. Binary search

The function `findPreviousNonConflict` is implemented to support the DP transition efficiently. Instead of scanning backward linearly, the program uses binary search on the list sorted by ending time to locate the latest class that still satisfies the minimum break condition.

This improves the transition step from linear search to logarithmic search.

### 5.3. Greedy filtering

Some constraints are handled before the DP phase. In particular, when the user limits the maximum number of study days per week, the program computes total weight by day, sorts the days by descending potential value, and keeps only the best days. This part follows a greedy idea.

### 5.4. Data structures used

The project uses the following standard structures:

- `vector` for storing courses, activities, schedule blocks, and DP values
- `map` for grouping courses by day and computing day-based totals
- `set` for storing allowed days after greedy filtering
- `string` for course information, time values, and descriptions

These structures are enough to keep the code simple while still supporting the required operations clearly.

## 6. How the scheduling process works

The overall flow of the C++ program is as follows:

1. Read all courses from `data.csv`
2. Keep only the rows that belong to the selected semester
3. Apply soft constraints by adjusting course weights
4. Apply hard constraints by removing or trimming unsuitable options
5. Group remaining classes by day
6. Run weighted interval scheduling on each day
7. Merge all selected classes into a weekly result
8. Read personal activities from the user
9. Try to place each activity into valid free slots
10. Fill the remaining time with free-time blocks
11. Print the weekly schedule and export `Schedule.ics`

The web version follows the same idea, but all settings are entered through the browser interface instead of the console.

## 7. Source code structure

Main files in the project folder:

- `main.cpp`: console program, user interaction, constraint processing, and the main DP logic
- `Course.h`: definition of the `Course` structure and sorting rule by day and end time
- `Activity.h`: definitions of `PersonalActivity` and `ScheduleBlock`
- `DataLoader.h`: functions for reading `data.csv` and converting time strings to minutes
- `SchedulePlanner.h`: logic for placing personal activities, generating free time, and printing the weekly schedule
- `CalendarExport.h`: export functions for `.ics`
- `data.csv`: sample input dataset
- `Schedule.ics`: sample output calendar file

Files related to the web interface:

- `index.html`: user interface
- `style.css`: layout and presentation
- `script.js`: client-side logic for course filtering, activity placement, weekly rendering, and `.ics` export
- `run_ui.bat`: quick local launcher using Python HTTP server
- `manifest.webmanifest`, `service-worker.js`, `icons/`: files for the installable web app version

Additional files currently present in the folder:

- `time_test.cpp`: small test file for checking the time conversion function
- `main.exe`, `app_cli.exe`, `app_test.exe`, `time_test.exe`: compiled executables currently available in the project folder

## 8. How to compile and run

### 8.1. Console version

If a C++ compiler such as `g++` is installed, the console version can be compiled with:

```bash
g++ main.cpp -o SmartScheduler
```

Then run:

```bash
./SmartScheduler
```

On Windows, the compiled file may be run as:

```bash
SmartScheduler.exe
```

The repository also already contains compiled `.exe` files, so on Windows it is possible to open the existing executable directly if needed.

### 8.2. Web version

Because the web interface reads `data.csv`, it should be opened through a local server instead of opening `index.html` directly as a file.

The simplest way on Windows is:

```bash
run_ui.bat
```

This starts a local HTTP server on port `8000` and opens:

```text
http://localhost:8000/index.html
```

Another equivalent way is:

```bash
python -m http.server 8000
```

and then open the same address in a browser.

### 8.3. Public web deployment

Besides local testing, the web version is also prepared for deployment so that other users can access it through a shared link in a browser.

The current project already includes deployment-related files such as:

- `netlify.toml` for static hosting configuration
- `manifest.webmanifest`, `service-worker.js`, and `icons/` for the installable PWA version

This means the interface can be published as a public website on services such as Netlify. After deployment, users can:

- open the scheduler from a public URL without installing C++ tools
- use the application directly in the browser
- optionally install it as a lightweight app on supported devices

In short, the web implementation is not only for demonstration in class, but also for sharing the scheduler with general users in a simple and accessible way.

Deployment notes and publishing steps are summarized in `README_DEPLOY.md`.

## 9. Program output

The program produces the following outputs:

- selected course schedule
- personal activity schedule
- automatically generated free-time blocks
- warning messages for activities that cannot be placed
- exported calendar file `Schedule.ics`

In the console version, the weekly schedule is printed day by day in text form. In the web version, the result is shown as a weekly overview and a detailed daily timeline.

## 10. Notes on implementation

Some details observed directly from the current codebase:

- Time values are converted to minutes for easier comparison and scheduling.
- The minimum break condition is integrated into compatibility checking.
- Lunch avoidance is handled as a hard filter.
- Free time is generated only within the user-defined active time range.
- The project currently uses a fixed reference week when exporting `.ics`, because the focus of the assignment is scheduling logic rather than live calendar synchronization.

## 11. Limitations

The current version still has some limitations:

- the system depends on the correctness of the CSV input format
- there is no database; all data is read from local files
- the scheduler chooses from the available rows in the dataset and does not generate new class sections
- the calendar export uses a fixed sample week instead of connecting to a real semester calendar

These limitations are acceptable for the scope of a DSA final project, since the main focus is the design of the scheduling algorithm and supporting data structures.

## 12. Conclusion

This project applies dynamic programming, binary search, greedy filtering, and standard data structures to solve a practical scheduling problem. The result is a system that not only selects a valid study timetable but also extends it into a full weekly life plan with personal activities and free-time management.

From the viewpoint of a Data Structures and Algorithms course, the most important part of the project is that the problem was modeled clearly, implemented in modular form, and solved using appropriate algorithmic techniques rather than ad hoc manual rules.
