#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
#include <map>
#include "Course.h"
#include "DataLoader.h"
#include "CalendarExport.h"
using namespace std;
int globalMinBreak = 0;

bool isCompatible(const Course &prev, const Course &curr){
    if (prev.day < curr.day) return true;
    if (prev.day > curr.day) return false;
    return prev.end_min + globalMinBreak <= curr.start_min;
}

int findPreviousNonConflict(const vector<Course> &courses, int index){
    int low = 0, high = index - 1;
    while (low <= high){
        int mid = (low + high) / 2;
        if (isCompatible(courses[mid], courses[index])){
            if (mid + 1 < index && isCompatible(courses[mid + 1], courses[index])){
                low = mid + 1;
            }
            else{
                return mid;
            }
        }
        else{
            high = mid - 1;
        }
    }
    return -1;
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
        cout << "No courses found for semester " << userSemester << endl;
        return 0;
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
        cout << "Enter banned day (0=Mon ... 6=Sun): ";
        cin >> d;
        bannedDays.push_back(d);
    }

    int numPreferredDays;
    cout << "\nHow many days you PREFER to study? ";
    cin >> numPreferredDays;

    vector<int> preferredDays;
    for (int i = 0; i < numPreferredDays; i++){
        int d;
        cout << "Enter preferred day (0=Mon ... 8=Sun): ";
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
        cout << "All courses removed by your filters!" << endl;
        return 0;
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
        cout << "No courses remain after advanced filters!" << endl;
        return 0;
    }


    sort(courses.begin(), courses.end());

    int n = courses.size();

    vector<int> dp(n);
    dp[0] = courses[0].weight;
    for (int i = 1; i < n; i++){
        int weight_i = courses[i].weight;
        int p = findPreviousNonConflict(courses, i);
        if (p != -1)
            weight_i += dp[p];
        dp[i] = max(dp[i - 1], weight_i);
    }

    vector<Course> selectedCourses;
    int i = n - 1;
    while (i >= 0){
        if (i == 0){
            selectedCourses.push_back(courses[0]);
            break;
        }
        int p = findPreviousNonConflict(courses, i);
        int val_p = 0;
        if (p != -1){
            val_p = dp[p];
        }
        if (courses[i].weight + val_p >= dp[i - 1]){
            selectedCourses.push_back(courses[i]);
            i = p;
        }
        else{
            i--;
        }
    }

    reverse(selectedCourses.begin(), selectedCourses.end());

    exportToICS(selectedCourses, "Schedule.ics");
    cout << "------------------------------------------" << endl;
    cout << "SUCCESS! Schedule exported to Schedule.ics" << endl;
    return 0;
}