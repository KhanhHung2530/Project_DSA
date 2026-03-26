#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
#include <map>
#include <limits>
#include "Course.h"
#include "Activity.h"
#include "DataLoader.h"
#include "SchedulePlanner.h"
#include "CalendarExport.h"
using namespace std;
int globalMinBreak = 0;

bool isCompatible(const Course &prev, const Course &curr){
    return prev.end_min + globalMinBreak <= curr.start_min;
}

int findPreviousNonConflict(const vector<Course> &courses, int index){
    int low = 0, high = index - 1;
    int best=-1;
    while (low <= high){
        int mid = (low + high) / 2;
        if (isCompatible(courses[mid], courses[index])){
            best=mid;
            low=mid+1;
        }
        else{
            high = mid - 1;
        }
    }
    return best;
}

string readLineInput(const string &prompt){
    cout << prompt;
    string value;
    getline(cin >> ws, value);
    return value;
}

int readPeriodChoice(){
    int choice;
    cout << "Preferred time period:\n";
    cout << "0 - Anywhere in the day\n";
    cout << "1 - Morning\n";
    cout << "2 - Afternoon\n";
    cout << "3 - Evening\n";
    cout << "Your choice: ";
    cin >> choice;
    return choice;
}

string periodChoiceToString(int choice){
    if (choice == 1) return "morning";
    if (choice == 2) return "afternoon";
    if (choice == 3) return "evening";
    return "any";
}

int main(){
    vector<Course> allCourses = loadCoursesFromCSV("data.csv");
    if (allCourses.empty()){
        cout << "Error: Cannot read data from data.csv!" << endl;
        return 0;
    }
    int userSemester;
    cout << "Enter semester: ";
    cin >> userSemester;
    vector<Course> courses;
    for (const Course &c : allCourses){
        if (c.semester == userSemester)
        {
            courses.push_back(c);
        }
    }

    if (courses.empty()){
        cout << "No courses found for semester " << userSemester << ". You can still build a personal-life schedule.\n";
    }
    // USER MODE SELECTION

    cout << "\n===== STUDY MODE SETTINGS =====\n";

    int timeMode;
    cout << "Choose time preference:\n";
    cout << "0 - No preference\n";
    cout << "1 - Prefer EARLY classes\n";
    cout << "2 - Prefer LATE classes\n";
    cout << "Your choice: ";
    cin >> timeMode;

    int numNoStudyDays;
    cout << "\nHow many days you DO NOT want to study? ";
    cin >> numNoStudyDays;

    vector<int> bannedDays;
    for (int i = 0; i < numNoStudyDays; i++){
        int d;
        cout << "Enter banned day (2=Mon ... 8=Sun): ";
        cin >> d;
        bannedDays.push_back(d);
    }

    int numPreferredDays;
    cout << "\nHow many days you PREFER to study? ";
    cin >> numPreferredDays;

    vector<int> preferredDays;
    for (int i = 0; i < numPreferredDays; i++){
        int d;
        cout << "Enter preferred day (2=Mon ... 8=Sun): ";
        cin >> d;
        preferredDays.push_back(d);
    }

    vector<Course> filteredCourses;
    for (Course c : courses){
        bool banned = false;
        for (int d : bannedDays){
            if (c.day == d){
                banned = true;
                break;
            }
        }
        if (!banned) filteredCourses.push_back(c);
    }
    courses = filteredCourses;
    for (Course &c : courses){
        if (timeMode == 1){
            if (c.start_min < 720) c.weight += 100;
        }

        if (timeMode == 2){
            if (c.start_min >= 720) c.weight += 100;
        }
        for (int d : preferredDays){
            if (c.day == d){
                c.weight += 50;
                break;
            }
        }
    }
    if (courses.empty()){
        cout << "All courses removed by your filters. Continuing with personal activities only.\n";
    }
    // ADVANCED MODE SETTINGS 
    cout << "\n===== ADVANCED MODES =====\n";
    // Max classes per day 
    int maxPerDay;
    cout << "Max classes per day (0 = no limit): ";
    cin >> maxPerDay;

    // Minimum break between classes 
    cout << "Minimum break between classes in minutes (0 = ignore): ";
    cin >> globalMinBreak;

    // Avoid lunch time 
    int avoidLunch;
    cout << "Avoid lunch time block? (1=yes 0=no): ";
    cin >> avoidLunch;

    // Max study days 
    int maxStudyDays;
    cout << "Max study days per week (0 = no limit): ";
    cin >> maxStudyDays;

    // Avoid lunch time (11:30 -> 13:30) 
    if (avoidLunch){
        vector<Course> tmp;
        for (const Course &c : courses){
            if (c.end_min <= 690 || c.start_min >= 810){
                tmp.push_back(c);
            }
        }
        courses = tmp;
    }

    // Limit classes per day 
    if (maxPerDay > 0){
        map<int, vector<Course>> byDay;
        for (const Course &c : courses){
            byDay[c.day].push_back(c);
        }

        vector<Course> tmp;
        for (auto &p : byDay){
            auto &vec = p.second;
            sort(vec.begin(), vec.end(), [](const Course &a, const Course &b){
                return a.weight > b.weight;
            });
            for (int i = 0; i < vec.size() && i < maxPerDay; i++){
                tmp.push_back(vec[i]);
            }
        }
        courses = tmp;
    }
    // Limit number of study days 
    if (maxStudyDays > 0){
        map<int,int> dayWeight;
        for (const Course &c : courses){
            dayWeight[c.day] += c.weight;
        }

        vector<pair<int,int>> days;
        for (auto &p : dayWeight){
            days.push_back({p.second, p.first});
        }

        sort(days.rbegin(), days.rend());

        set<int> allowed;
        for (int i = 0; i < days.size() && i < maxStudyDays; i++){
            allowed.insert(days[i].second);
        }

        vector<Course> tmp;
        for (const Course &c : courses){
            if (allowed.count(c.day)) tmp.push_back(c);
        }
        courses = tmp;
    }

    if (courses.empty()){
        cout << "No courses remain after advanced filters. Continuing with personal activities only.\n";
    }

// THE CORE ENGINE: DYNAMIC PROGRAMMING (PER DAY) 
    map<int, vector<Course>> coursesByDay;
    for (const Course &c : courses) {
        coursesByDay[c.day].push_back(c);
    }

    vector<Course> finalSelectedCourses;
    for (auto &pair : coursesByDay) {
        vector<Course> &dayCourses = pair.second;
        
        sort(dayCourses.begin(), dayCourses.end());

        int n = dayCourses.size();
        vector<int> dp(n);
        dp[0] = dayCourses[0].weight;

        for (int i = 1; i < n; i++){
            int weight_i = dayCourses[i].weight;
            int p = findPreviousNonConflict(dayCourses, i);
            if (p != -1)
                weight_i += dp[p];
            dp[i] = max(dp[i - 1], weight_i);
        }

        vector<Course> daySelected;
        int i = n - 1;
        while (i >= 0){
            if (i == 0){
                daySelected.push_back(dayCourses[0]);
                break;
            }
            int p = findPreviousNonConflict(dayCourses, i);
            int val_p = (p != -1) ? dp[p] : 0;
            
            if (dayCourses[i].weight + val_p >= dp[i - 1]){
                daySelected.push_back(dayCourses[i]);
                i = p;
            }
            else{
                i--;
            }
        }

        finalSelectedCourses.insert(finalSelectedCourses.end(), daySelected.begin(), daySelected.end());
    }

    sort(finalSelectedCourses.begin(), finalSelectedCourses.end());

    cout << "\n===== LIFE SCHEDULE SETTINGS =====\n";
    string dailyStartTime;
    string dailyEndTime;
    cout << "Daily start time (HH:MM): ";
    cin >> dailyStartTime;
    cout << "Daily end time (HH:MM): ";
    cin >> dailyEndTime;

    int activeStartMin = TimeToMinutes(dailyStartTime);
    int activeEndMin = TimeToMinutes(dailyEndTime);
    while (activeEndMin <= activeStartMin){
        cout << "End time must be later than start time. Please enter again.\n";
        cout << "Daily start time (HH:MM): ";
        cin >> dailyStartTime;
        cout << "Daily end time (HH:MM): ";
        cin >> dailyEndTime;
        activeStartMin = TimeToMinutes(dailyStartTime);
        activeEndMin = TimeToMinutes(dailyEndTime);
    }

    int numActivities;
    cout << "\nHow many optional activities do you want to add? ";
    cin >> numActivities;

    vector<PersonalActivity> personalActivities;
    for (int i = 0; i < numActivities; i++){
        cout << "\n----- Activity " << i + 1 << " -----\n";
        PersonalActivity activity;
        activity.name = readLineInput("Activity name: ");
        activity.category = readLineInput("Category (English/Gym/Reading/etc): ");
        activity.location = readLineInput("Location or note: ");
        cout << "Duration per day (minutes): ";
        cin >> activity.duration_min;

        int periodChoice = readPeriodChoice();
        activity.preferred_period = periodChoiceToString(periodChoice);

        int numDays;
        cout << "How many days in the week for this activity? ";
        cin >> numDays;
        for (int j = 0; j < numDays; j++){
            int d;
            cout << "Enter day (2=Mon ... 8=Sun): ";
            cin >> d;
            activity.days.push_back(d);
        }
        personalActivities.push_back(activity);
    }

    vector<string> warnings;
    vector<ScheduleBlock> finalSchedule = buildWeeklySchedule(
        finalSelectedCourses,
        personalActivities,
        activeStartMin,
        activeEndMin,
        warnings
    );

    printWeeklySchedule(finalSchedule);
    if (!warnings.empty()){
        cout << "\n============= WARNINGS =============\n";
        for (const string &warning : warnings){
            cout << "- " << warning << '\n';
        }
    }

    exportToICS(finalSchedule, "Schedule.ics");
    cout << "------------------------------------------" << endl;
    cout << "SUCCESS! Full life schedule exported to Schedule.ics" << endl;
    return 0;
}
