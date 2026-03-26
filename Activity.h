#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <string>
#include <vector>
using namespace std;

struct PersonalActivity
{
    string name;
    string category;
    string location;
    string preferred_period;
    int duration_min;
    vector<int> days;
};

struct ScheduleBlock
{
    string title;
    string category;
    string location;
    string description;
    int day;
    int start_min;
    int end_min;
    string start_time;
    string end_time;

    bool operator<(const ScheduleBlock &other) const
    {
        if (day != other.day)
            return day < other.day;
        if (start_min != other.start_min)
            return start_min < other.start_min;
        return end_min < other.end_min;
    }
};

#endif
