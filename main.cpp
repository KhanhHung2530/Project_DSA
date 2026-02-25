#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "Course.h"
#include "DataLoader.h"
#include "CalendarExport.h"
using namespace std;

bool isCompatible(const Course &prev, const Course &curr){
    if (prev.day < curr.day) return true;
    if (prev.day > curr.day) return false;
    return prev.end_min <= curr.start_min;
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