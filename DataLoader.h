#ifndef DATALOADER_H
#define DATALOADER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Course.h"
using namespace std;

// Converts a time string HH:MM into total minutes since the start of the day.
int TimeToMinutes(const string &Time)
{
    string cleanTime;
    for (char ch : Time)
    {
        if (ch != '\r' && ch != '\n' && ch != ' ' && ch != '\t')
            cleanTime.push_back(ch);
    }
    size_t colonPos = cleanTime.find(':');
    if (colonPos == string::npos || colonPos == 0 || colonPos + 1 >= cleanTime.size())
        return 0;

    int hours = stoi(cleanTime.substr(0, colonPos));
    int minutes = stoi(cleanTime.substr(colonPos + 1));
    return hours * 60 + minutes;
}

vector<Course> loadCoursesFromCSV(const string &filename)
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
    string dayStr, weightStr, semStr, typeStr;
    Course c;
    while (getline(infile, c.course_id, ','))
    {
        getline(infile, dayStr, ',');
        getline(infile, c.start_time, ',');
        getline(infile, c.end_time, ',');
        getline(infile, c.room, ',');
        getline(infile, weightStr, ',');
        getline(infile, semStr, ',');
        getline(infile, typeStr);
        if (!semStr.empty() && semStr.back() == '\r') {
            semStr.pop_back();
        }
        if (!typeStr.empty() && typeStr.back() == '\r') {
            typeStr.pop_back();
        }
        c.day = stoi(dayStr);
        c.start_min = TimeToMinutes(c.start_time);
        c.end_min = TimeToMinutes(c.end_time);
        c.weight = stoi(weightStr);
        c.semester = stoi(semStr);
        c.type = typeStr;
        courses.push_back(c);
    }
    infile.close();
    return courses;
}
#endif
