#ifndef DATALOADER_H
#define DATALOADER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Course.h"
using namespace std;

// Converts a time string HH:MM into total minutes since the start of the day.
inline int TimeToMinutes(const string &Time)
{
    int hours = stoi(Time.substr(0, 2));
    int minutes = stoi(Time.substr(3, 2));
    return hours * 60 + minutes;
}

inline vector<Course> loadCoursesFromCSV(const string &filename)
{
    vector<Course> courses;
    ifstream infile(filename);
    string line;
    if (!infile)
    {
        cout << "Error: Could not open file " << filename << '\n';
        return courses;
    }
    string header;
    getline(infile, header);
    Course c;
    string dayStr, weightStr, semStr, typeStr;
    while (getline(infile, c.course_id, ','))
    {
        getline(infile, dayStr, ',');
        getline(infile, c.start_time, ',');
        getline(infile, c.end_time, ',');
        getline(infile, c.room, ',');
        getline(infile, weightStr, ',');
        getline(infile, semStr, ',');
        getline(infile, typeStr);
        if (!typeStr.empty() && typeStr.back() == '\r') {
            typeStr.pop_back();
        }
        c.day = stoi(dayStr);
        c.start_min = TimeToMinutes(c.start_time);
        c.end_min = TimeToMinutes(c.end_time);
        c.weight = stoi(weightStr);
        c.semester = stoi(semStr);
        courses.push_back(c);
    }
    infile.close();
    return courses;
}
#endif
