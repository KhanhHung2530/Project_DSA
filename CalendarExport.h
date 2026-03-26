#ifndef CALENDAREXPORT_H
#define CALENDAREXPORT_H

#include <iostream>
#include "Course.h"
#include "Activity.h"
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

string escapeICS(const string &value)
{
    string escaped;
    for (char ch : value)
    {
        if (ch == '\\' || ch == ';' || ch == ',')
            escaped.push_back('\\');
        if (ch == '\n')
            escaped += "\\n";
        else
            escaped.push_back(ch);
    }
    return escaped;
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
        outfile << "SUMMARY:" << escapeICS(c.course_id) << '\n';
        outfile << "LOCATION:" << escapeICS(c.room) << '\n';
        outfile << "DESCRIPTION:" << escapeICS(c.type.empty() ? "Class session" : c.type) << '\n';

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

void exportToICS(const vector<ScheduleBlock> &scheduleBlocks, const string &filename)
{
    ofstream outfile(filename);
    if (!outfile)
    {
        cout << "Error: Could not create file " << filename << '\n';
        return;
    }

    outfile << "BEGIN:VCALENDAR\n";
    outfile << "VERSION:2.0\n";
    outfile << "PRODID:-//DSA Life Planner//EN\n";

    for (const ScheduleBlock &block : scheduleBlocks)
    {
        outfile << "BEGIN:VEVENT\n";
        outfile << "SUMMARY:" << escapeICS(block.title) << '\n';
        outfile << "LOCATION:" << escapeICS(block.location) << '\n';
        outfile << "DESCRIPTION:" << escapeICS(block.category + " - " + block.description) << '\n';

        const string dateStr = getStartDate(block.day);
        const string startTime = formatTime(block.start_time);
        const string endTime = formatTime(block.end_time);

        outfile << "DTSTART:" << dateStr << "T" << startTime << '\n';
        outfile << "DTEND:" << dateStr << "T" << endTime << '\n';
        outfile << "END:VEVENT\n";
    }

    outfile << "END:VCALENDAR\n";
    outfile.close();
}

#endif
