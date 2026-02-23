#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "Course.h"
using namespace std;

// Converts a time string HH:MM into total minutes since the start of the day.
int TimeToMinutes(string Time)
{
    int hours = stoi(Time.substr(0, 2));
    int minutes = stoi(Time.substr(3, 2));
    return hours * 60 + minutes;
}

// Find the latest course that does not overlap with the current one
// Uses Binary Search for O(log n) performance
int findPreviourNonConflict(const vector<Course> &courses, int index)
{
    int low = 0, high = index - 1;
    while (low <= high)
    {
        int mid = (low + high) / 2;
        // Check if the middle course finishes before the current course starts
        if (courses[mid].end_min <= courses[index].start_min)
        {
            // If the next course also fits, look further to the right
            if (courses[mid + 1].end_min <= courses[index].start_min)
            {
                low = mid + 1;
            }
            else
            {
                // This is the closest compatible course
                return mid;
            }
        }
        else
        {
            // Conflict found, look to the left
            high = mid - 1;
        }
    }
    // No compatible course found
    return -1;
}

int main()
{
    vector<Course> courses;
    cout << "Smart Course Scheduler Started!" << endl;
    // TODO: Load data from CSV
    // TODO: Sort courses by end_min
    // TODO: Run Dynamic Programming algorithm
    return 0;
}