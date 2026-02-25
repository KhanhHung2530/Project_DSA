#ifndef COURSE_H
#define COURSE_H

#include <string>
using namespace std;

struct Course
{
    string course_id;
    int day;           // Day of the week (2 for Monday, 7 for Saturday)
    string start_time; // Time format HH:MM (Example: 07:30)
    string end_time;   // Time format HH:MM (Example: 09:10)
    string room;

    // Converted values for calculation
    int start_min; // Start time converted to minutes
    int end_min;   // End time converted to minutes
    int weight;    // Priority weight for DP
    int semester;

    // Overload the less-than operator to sort courses by their end time
    // This is a prerequisite for both Greedy and DP algorithms
    bool operator<(const Course &other) const
    {
        if (day != other.day)
            return day < other.day;
        return end_min < other.end_min;
    }
};

#endif