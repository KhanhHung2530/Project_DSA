#ifndef CALENDAREXPORT_H
#define CALENDAREXPORT_H

#include <iostream>
#include "Course.h"
#include <vector>
#include <fstream>
using namespace std;

string getStartDate(int day)
{
    int startDay = 23;
    int actualDay = startDay + (day - 2);
    string dayStr = to_string(actualDay);
    if (actualDay < 10)
        dayStr = "0" + dayStr;
    return "202602" + dayStr;
}

string formatTime(string t)
{
    string hh = t.substr(0, 2);
    string mm = t.substr(3, 2);
    return hh + mm + "00";
}

void exportToICS(const vector<Course> &selectedCourses, string filename)
{
    ofstream outfile(filename);
    if (!outfile)
    {
        cout << "Error: Could not create file " << filename << endl;
        return;
    }

    outfile << "BEGIN:VCALENDAR" << endl;
    outfile << "VERSION:2.0" << endl;

    for (const Course &c : selectedCourses)
    {
        outfile << "BEGIN:VEVENT" << endl;
        outfile << "SUMMARY:" << c.course_id << endl;
        outfile << "LOCATION:" << c.room << endl;

        string dateStr = getStartDate(c.day);
        string startTime = formatTime(c.start_time);
        string endTime = formatTime(c.end_time);

        outfile << "DTSTART:" << dateStr << "T" << startTime << endl;
        outfile << "DTEND:" << dateStr << "T" << endTime << endl;
        outfile << "RRULE:FREQ=WEEKLY;COUNT=15" << endl;

        outfile << "END:VEVENT" << endl;
    }

    outfile << "END:VCALENDAR" << endl;
    outfile.close();
}

#endif