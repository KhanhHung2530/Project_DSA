#ifndef CALENDAREXPORT_H
#define CALENDAREXPORT_H

#include <iostream>
#include "Course.h"
#include <vector>
#include <fstream>
using namespace std;

string getStartDate(int day)
{
    int startDay = 12;
    int actualDay = startDay + (day - 2);    // Convert weekday to actual calendar date starting from Jan 12
    string dayStr = to_string(actualDay);
    if (actualDay < 10)
        dayStr = "0" + dayStr;
    return "202601" + dayStr;
}

string formatTime(const string &t)
{
    string hh = t.substr(0, 2);
    string mm = t.substr(3, 2);
    return hh + mm + "00";
}

void exportToICS(const vector<Course> &selectedCourses, const string &filename)
{
    ofstream outfile(filename);
    if (!outfile)
    {
        cout << "Error: Could not create file " << filename << '\n';
        return;
    }

    outfile << "BEGIN:VCALENDAR\n";
    outfile << "VERSION:2.0\n";

    for (const Course &c : selectedCourses)
    {
        outfile << "BEGIN:VEVENT\n";
        outfile << "SUMMARY:" << c.course_id << '\n';
        outfile << "LOCATION:" << c.room << '\n';

        const string dateStr = getStartDate(c.day);
        const string startTime = formatTime(c.start_time);
        const string endTime = formatTime(c.end_time);

        outfile << "DTSTART:" << dateStr << "T" << startTime << '\n';
        outfile << "DTEND:" << dateStr << "T" << endTime << '\n';
        outfile << "RRULE:FREQ=WEEKLY;COUNT=15\n";

        outfile << "END:VEVENT\n";
    }

    outfile << "END:VCALENDAR\n";
    outfile.close();
}

#endif
