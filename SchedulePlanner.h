#ifndef SCHEDULEPLANNER_H
#define SCHEDULEPLANNER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "Course.h"
#include "Activity.h"
using namespace std;

inline string minutesToClock(int totalMinutes)
{
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    string hh = (hours < 10 ? "0" : "") + to_string(hours);
    string mm = (minutes < 10 ? "0" : "") + to_string(minutes);
    return hh + ":" + mm;
}

inline string dayToLabel(int day)
{
    switch (day)
    {
    case 2: return "Monday";
    case 3: return "Tuesday";
    case 4: return "Wednesday";
    case 5: return "Thursday";
    case 6: return "Friday";
    case 7: return "Saturday";
    case 8: return "Sunday";
    default: return "Unknown";
    }
}

inline ScheduleBlock makeCourseBlock(const Course &course)
{
    ScheduleBlock block;
    block.title = course.course_id;
    block.category = "Course";
    block.location = course.room;
    block.description = course.type.empty() ? "Class session" : course.type;
    block.day = course.day;
    block.start_min = course.start_min;
    block.end_min = course.end_min;
    block.start_time = course.start_time;
    block.end_time = course.end_time;
    return block;
}

inline vector<ScheduleBlock> getFreeSlots(const vector<ScheduleBlock> &occupiedBlocks, int activeStart, int activeEnd)
{
    vector<ScheduleBlock> sortedBlocks = occupiedBlocks;
    sort(sortedBlocks.begin(), sortedBlocks.end());

    vector<ScheduleBlock> freeSlots;
    int cursor = activeStart;

    for (const ScheduleBlock &block : sortedBlocks)
    {
        int blockStart = max(block.start_min, activeStart);
        int blockEnd = min(block.end_min, activeEnd);

        if (blockEnd <= activeStart || blockStart >= activeEnd)
            continue;

        if (blockStart > cursor)
        {
            ScheduleBlock slot;
            slot.title = "Free Slot";
            slot.category = "Free";
            slot.location = "Flexible time";
            slot.description = "Available slot";
            slot.day = block.day;
            slot.start_min = cursor;
            slot.end_min = blockStart;
            slot.start_time = minutesToClock(cursor);
            slot.end_time = minutesToClock(blockStart);
            freeSlots.push_back(slot);
        }
        cursor = max(cursor, blockEnd);
    }

    if (cursor < activeEnd)
    {
        ScheduleBlock slot;
        slot.title = "Free Slot";
        slot.category = "Free";
        slot.location = "Flexible time";
        slot.description = "Available slot";
        slot.day = sortedBlocks.empty() ? 2 : sortedBlocks[0].day;
        slot.start_min = cursor;
        slot.end_min = activeEnd;
        slot.start_time = minutesToClock(cursor);
        slot.end_time = minutesToClock(activeEnd);
        freeSlots.push_back(slot);
    }

    return freeSlots;
}

inline pair<int, int> preferredWindow(const string &period, int activeStart, int activeEnd)
{
    int start = activeStart;
    int end = activeEnd;

    if (period == "morning")
    {
        start = max(activeStart, 5 * 60);
        end = min(activeEnd, 12 * 60);
    }
    else if (period == "afternoon")
    {
        start = max(activeStart, 12 * 60);
        end = min(activeEnd, 17 * 60);
    }
    else if (period == "evening")
    {
        start = max(activeStart, 17 * 60);
        end = min(activeEnd, 24 * 60);
    }

    return {start, end};
}

inline int slotScore(const ScheduleBlock &slot, const pair<int, int> &window)
{
    int overlapStart = max(slot.start_min, window.first);
    int overlapEnd = min(slot.end_min, window.second);
    return overlapEnd > overlapStart ? 0 : 1;
}

inline bool tryPlaceActivity(const PersonalActivity &activity, int day, vector<ScheduleBlock> &occupiedBlocks, int activeStart, int activeEnd, ScheduleBlock &placedBlock)
{
    vector<ScheduleBlock> freeSlots = getFreeSlots(occupiedBlocks, activeStart, activeEnd);
    pair<int, int> window = preferredWindow(activity.preferred_period, activeStart, activeEnd);

    sort(freeSlots.begin(), freeSlots.end(), [&](const ScheduleBlock &a, const ScheduleBlock &b)
    {
        int scoreA = slotScore(a, window);
        int scoreB = slotScore(b, window);
        if (scoreA != scoreB)
            return scoreA < scoreB;
        return a.start_min < b.start_min;
    });

    for (const ScheduleBlock &slot : freeSlots)
    {
        int start = slot.start_min;
        int preferredStart = max(slot.start_min, window.first);
        int preferredEnd = min(slot.end_min, window.second);

        if (preferredEnd - preferredStart >= activity.duration_min)
            start = preferredStart;

        if (start + activity.duration_min <= slot.end_min)
        {
            placedBlock.title = activity.name;
            placedBlock.category = activity.category;
            placedBlock.location = activity.location.empty() ? "Personal time" : activity.location;
            if (activity.preferred_period == "any")
                placedBlock.description = "Flexible personal activity";
            else
                placedBlock.description = "Preferred in the " + activity.preferred_period;
            placedBlock.day = day;
            placedBlock.start_min = start;
            placedBlock.end_min = start + activity.duration_min;
            placedBlock.start_time = minutesToClock(placedBlock.start_min);
            placedBlock.end_time = minutesToClock(placedBlock.end_min);
            occupiedBlocks.push_back(placedBlock);
            sort(occupiedBlocks.begin(), occupiedBlocks.end());
            return true;
        }
    }

    return false;
}

inline string freeTimeDescription(int startMin, int endMin)
{
    if (startMin < 9 * 60)
        return "Breakfast and morning routine";
    if (startMin < 13 * 60 && endMin > 12 * 60)
        return "Lunch break";
    if (startMin >= 18 * 60)
        return "Rest and recharge";
    return "Open space for your own plans";
}

inline vector<ScheduleBlock> buildWeeklySchedule(
    const vector<Course> &selectedCourses,
    const vector<PersonalActivity> &activities,
    int activeStart,
    int activeEnd,
    vector<string> &warnings)
{
    map<int, vector<ScheduleBlock>> byDay;

    for (const Course &course : selectedCourses)
    {
        byDay[course.day].push_back(makeCourseBlock(course));
    }

    for (int day = 2; day <= 8; ++day)
    {
        sort(byDay[day].begin(), byDay[day].end());
    }

    for (const PersonalActivity &activity : activities)
    {
        for (int day : activity.days)
        {
            ScheduleBlock placedBlock;
            if (!tryPlaceActivity(activity, day, byDay[day], activeStart, activeEnd, placedBlock))
            {
                warnings.push_back("Could not fit activity \"" + activity.name + "\" on " + dayToLabel(day) + ".");
            }
        }
    }

    vector<ScheduleBlock> finalSchedule;
    for (int day = 2; day <= 8; ++day)
    {
        vector<ScheduleBlock> dayBlocks = byDay[day];
        sort(dayBlocks.begin(), dayBlocks.end());

        int cursor = activeStart;
        for (const ScheduleBlock &block : dayBlocks)
        {
            if (block.end_min <= activeStart || block.start_min >= activeEnd)
                continue;

            if (block.start_min > cursor)
            {
                ScheduleBlock freeBlock;
                freeBlock.title = "Free Time";
                freeBlock.category = "Free";
                freeBlock.location = "Flexible time";
                freeBlock.description = freeTimeDescription(cursor, block.start_min);
                freeBlock.day = day;
                freeBlock.start_min = cursor;
                freeBlock.end_min = block.start_min;
                freeBlock.start_time = minutesToClock(cursor);
                freeBlock.end_time = minutesToClock(block.start_min);
                finalSchedule.push_back(freeBlock);
            }

            ScheduleBlock clipped = block;
            clipped.start_min = max(block.start_min, activeStart);
            clipped.end_min = min(block.end_min, activeEnd);
            clipped.start_time = minutesToClock(clipped.start_min);
            clipped.end_time = minutesToClock(clipped.end_min);
            if (clipped.end_min > clipped.start_min)
                finalSchedule.push_back(clipped);

            cursor = max(cursor, block.end_min);
        }

        if (cursor < activeEnd)
        {
            ScheduleBlock freeBlock;
            freeBlock.title = "Free Time";
            freeBlock.category = "Free";
            freeBlock.location = "Flexible time";
            freeBlock.description = freeTimeDescription(cursor, activeEnd);
            freeBlock.day = day;
            freeBlock.start_min = cursor;
            freeBlock.end_min = activeEnd;
            freeBlock.start_time = minutesToClock(cursor);
            freeBlock.end_time = minutesToClock(activeEnd);
            finalSchedule.push_back(freeBlock);
        }
    }

    sort(finalSchedule.begin(), finalSchedule.end());
    return finalSchedule;
}

inline void printWeeklySchedule(const vector<ScheduleBlock> &schedule)
{
    cout << "\n============= WEEKLY SCHEDULE =============\n";
    int currentDay = -1;
    for (const ScheduleBlock &block : schedule)
    {
        if (block.day != currentDay)
        {
            currentDay = block.day;
            cout << "\n[" << dayToLabel(currentDay) << "]\n";
        }

        cout << "- " << block.start_time << " - " << block.end_time
             << " | " << block.title
             << " | " << block.category
             << " | " << block.location
             << '\n';
    }
}

#endif
