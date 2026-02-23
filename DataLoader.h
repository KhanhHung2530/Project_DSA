#ifndef DATALOADER_H
#define DATALOADER_H

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Course.h"
using namespace std;

// Converts a time string HH:MM into total minutes since the start of the day.
int TimeToMinutes(string Time)
{
    int hours = stoi(Time.substr(0, 2));
    int minutes = stoi(Time.substr(3, 2));
    return hours * 60 + minutes;
}

vector<Course> loadCoursesFromCSV(string filename)
{
    vector<Course> courses;
    ifstream infile(filename);
    string line;
    if (!infile)
    {
        cout << "Error: Could not open file " << filename << endl;
        return courses;
    }
    string header;
    getline(infile, header);
    Course c;
    string dayStr;
    while (getline(infile, c.course_id, ','))
    {
        getline(infile, dayStr, ',');
        getline(infile, c.start_time, ',');
        getline(infile, c.end_time, ',');
        getline(infile, c.room);
        c.day = stoi(dayStr);
        c.start_min = TimeToMinutes(c.start_time);
        c.end_min = TimeToMinutes(c.end_time);
        c.weight = 10;
        courses.push_back(c);
    }
    infile.close();
    return courses;
}
#endif