#ifndef CALENDAREXPORT_H
#define CALENDAREXPORT_H

#include <iostream>
#include "Course.h"
#include "Activity.h"
#include <vector>
#include <fstream>
#include <ctime>
using namespace std;

inline string getStartDate(int day)
{
    int startDay = 12;
    int actualDay = startDay + (day - 2);    // Convert weekday to actual calendar date starting from Jan 12
    string dayStr = to_string(actualDay);
    if (actualDay < 10)
        dayStr = "0" + dayStr;
    return "202601" + dayStr;
}

inline string formatTime(const string &t)
{
    if (t.substr(0, 2) == "24") return "235900";
    string hh = t.substr(0, 2);
    string mm = t.substr(3, 2);
    return hh + mm + "00";
}

inline string escapeICS(const string &value)
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

inline void exportToICS(const vector<Course> &selectedCourses, const string &filename)
{
    ofstream outfile(filename, ios::binary);
    if (!outfile)
    {
        cout << "Error: Could not create file " << filename << '\n';
        return;
    }
    string CRLF = "\r\n";
    outfile << "BEGIN:VCALENDAR" << CRLF;
    outfile << "VERSION:2.0" << CRLF;
    outfile << "PRODID:-//DSA Scheduler//EN" << CRLF;

    for (size_t i = 0; i < selectedCourses.size(); ++i)
    {
        const Course &c = selectedCourses[i];
        outfile << "BEGIN:VEVENT" << CRLF; 
        outfile << "UID:course-" << i << "-" << time(0) << "@dsaplanner.com" << CRLF;
        outfile << "DTSTAMP:20260327T220000Z" << CRLF;
        outfile << "SUMMARY:" << escapeICS(c.course_id) << CRLF;
        outfile << "LOCATION:" << escapeICS(c.room) << CRLF;
        outfile << "DESCRIPTION:" << escapeICS(c.type.empty() ? "Class session" : c.type) << CRLF;

        const string dateStr = getStartDate(c.day);
        const string startTime = formatTime(c.start_time);
        const string endTime = formatTime(c.end_time);

        outfile << "DTSTART:" << dateStr << "T" << startTime << CRLF;
        outfile << "DTEND:" << dateStr << "T" << endTime << CRLF;
        outfile << "RRULE:FREQ=WEEKLY;COUNT=15" << CRLF;

        outfile << "END:VEVENT" << CRLF;
    }

    outfile << "END:VCALENDAR" << CRLF;
    outfile.close();
}

inline void exportToICS(const vector<ScheduleBlock> &scheduleBlocks, const string &filename)
{
    ofstream outfile(filename, ios::binary);
    if (!outfile)
    {
        cout << "Error: Could not create file " << filename << '\n';
        return;
    }
    string CRLF = "\r\n";
    outfile << "BEGIN:VCALENDAR" << CRLF;
    outfile << "VERSION:2.0" << CRLF;
    outfile << "PRODID:-//DSA Life Planner//EN" << CRLF;

    for (size_t i = 0; i < scheduleBlocks.size(); ++i)
    {
        const ScheduleBlock &block = scheduleBlocks[i];
        outfile << "BEGIN:VEVENT" << CRLF;
        outfile << "UID:block-" << i << "-" << time(0) << "@dsaplanner.com" << CRLF;
        outfile << "DTSTAMP:20260327T220000Z" << CRLF;
        outfile << "SUMMARY:" << escapeICS(block.title) << CRLF;
        outfile << "LOCATION:" << escapeICS(block.location) << CRLF;
        outfile << "DESCRIPTION:" << escapeICS(block.category + " - " + block.description) << CRLF;

        const string dateStr = getStartDate(block.day);
        const string startTime = formatTime(block.start_time);
        const string endTime = formatTime(block.end_time);

        outfile << "DTSTART:" << dateStr << "T" << startTime << CRLF;
        outfile << "DTEND:" << dateStr << "T" << endTime << CRLF;
        if (block.category != "Free") {
            outfile << "RRULE:FREQ=WEEKLY;COUNT=15" << CRLF;
        }

        outfile << "END:VEVENT" << CRLF;
    }

    outfile << "END:VCALENDAR" << CRLF;
    outfile.close();
}

#endif
