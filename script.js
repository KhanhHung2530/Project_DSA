const DAY_ORDER = [2, 3, 4, 5, 6, 7, 8];
const DAY_LABELS = {
  2: "Monday",
  3: "Tuesday",
  4: "Wednesday",
  5: "Thursday",
  6: "Friday",
  7: "Saturday",
  8: "Sunday"
};

const PERIOD_WINDOWS = {
  any: null,
  morning: { start: 300, end: 720 },
  afternoon: { start: 720, end: 1020 },
  evening: { start: 1020, end: 1440 }
};

const state = {
  csvText: "",
  courses: [],
  schedule: null,
  selectedDay: 2,
  installPrompt: null
};

const elements = {
  semesterInput: document.getElementById("semesterInput"),
  timeModeSelect: document.getElementById("timeModeSelect"),
  activeStartInput: document.getElementById("activeStartInput"),
  activeEndInput: document.getElementById("activeEndInput"),
  maxPerDayInput: document.getElementById("maxPerDayInput"),
  minBreakInput: document.getElementById("minBreakInput"),
  maxStudyDaysInput: document.getElementById("maxStudyDaysInput"),
  avoidLunchInput: document.getElementById("avoidLunchInput"),
  bannedDaysGroup: document.getElementById("bannedDaysGroup"),
  preferredDaysGroup: document.getElementById("preferredDaysGroup"),
  activitiesList: document.getElementById("activitiesList"),
  activityTemplate: document.getElementById("activityTemplate"),
  generateBtn: document.getElementById("generateBtn"),
  exportBtn: document.getElementById("exportBtn"),
  loadSampleBtn: document.getElementById("loadSampleBtn"),
  csvFileInput: document.getElementById("csvFileInput"),
  messageBox: document.getElementById("messageBox"),
  weeklyOverview: document.getElementById("weeklyOverview"),
  dayDetail: document.getElementById("dayDetail"),
  detailTitle: document.getElementById("detailTitle"),
  detailSubtitle: document.getElementById("detailSubtitle"),
  summaryStats: document.getElementById("summaryStats"),
  addActivityBtn: document.getElementById("addActivityBtn"),
  installAppBtn: document.getElementById("installAppBtn"),
  installHint: document.getElementById("installHint")
};

initialize();

function initialize() {
  registerServiceWorker();
  setupInstallPrompt();
  renderDayChipGroups();
  addActivityCard({
    name: "English Study",
    duration: 60,
    category: "English",
    period: "evening",
    location: "Home",
    days: [2, 4, 6, 7]
  });
  addActivityCard({
    name: "Gym",
    duration: 90,
    category: "Fitness",
    period: "afternoon",
    location: "Gym",
    days: [2, 3, 5, 6, 7]
  });

  elements.addActivityBtn.addEventListener("click", () => addActivityCard());
  elements.generateBtn.addEventListener("click", handleGenerate);
  elements.exportBtn.addEventListener("click", exportCurrentSchedule);
  elements.loadSampleBtn.addEventListener("click", loadSampleCsv);
  elements.csvFileInput.addEventListener("change", handleCsvUpload);
  elements.installAppBtn.addEventListener("click", installApplication);

  setMessage("Click \"Load data.csv\" or upload a CSV file to start.", "info");
  loadSampleCsv();
}

function registerServiceWorker() {
  if (!("serviceWorker" in navigator)) {
    return;
  }

  window.addEventListener("load", async () => {
    try {
      await navigator.serviceWorker.register("service-worker.js");
    } catch (error) {
      console.warn("Service worker registration failed.", error);
    }
  });
}

function setupInstallPrompt() {
  updateInstallUi();

  window.addEventListener("beforeinstallprompt", (event) => {
    event.preventDefault();
    state.installPrompt = event;
    updateInstallUi();
  });

  window.addEventListener("appinstalled", () => {
    state.installPrompt = null;
    updateInstallUi("App installed. You can now open it from your desktop or app launcher.");
  });
}

async function installApplication() {
  if (!state.installPrompt) {
    updateInstallUi("Installation is available in Chrome or Edge after opening the hosted site.");
    return;
  }

  state.installPrompt.prompt();
  const outcome = await state.installPrompt.userChoice;
  state.installPrompt = null;

  if (outcome.outcome === "accepted") {
    updateInstallUi("Installation started. Your browser will finish adding the app.");
  } else {
    updateInstallUi("Installation was dismissed. You can install it again later.");
  }
}

function updateInstallUi(message) {
  const isInstallable = Boolean(state.installPrompt);
  elements.installAppBtn.hidden = !isInstallable;
  elements.installHint.textContent =
    message ||
    (isInstallable
      ? "This site can be installed like an app on supported browsers."
      : "Open this site in Chrome or Edge to install it like an app.");
}

function renderDayChipGroups() {
  elements.bannedDaysGroup.innerHTML = "";
  elements.preferredDaysGroup.innerHTML = "";
  DAY_ORDER.forEach((day) => {
    elements.bannedDaysGroup.appendChild(createDayChip(day, `banned-${day}`));
    elements.preferredDaysGroup.appendChild(createDayChip(day, `preferred-${day}`));
  });
}

function createDayChip(day, idPrefix, checked = false) {
  const label = document.createElement("label");
  label.className = "day-chip";

  const input = document.createElement("input");
  input.type = "checkbox";
  input.value = String(day);
  input.id = idPrefix;
  input.checked = checked;

  const span = document.createElement("span");
  span.textContent = DAY_LABELS[day].slice(0, 3);

  label.appendChild(input);
  label.appendChild(span);
  return label;
}

function addActivityCard(activity = {}) {
  const fragment = elements.activityTemplate.content.cloneNode(true);
  const card = fragment.querySelector(".activity-card");
  const title = card.querySelector("h3");
  const nameInput = card.querySelector(".activity-name");
  const durationInput = card.querySelector(".activity-duration");
  const categorySelect = card.querySelector(".activity-category");
  const periodSelect = card.querySelector(".activity-period");
  const locationInput = card.querySelector(".activity-location");
  const daysGroup = card.querySelector(".activity-days");
  const removeBtn = card.querySelector(".remove-activity-btn");

  nameInput.value = activity.name || "";
  durationInput.value = activity.duration || 60;
  categorySelect.value = activity.category || "Personal";
  periodSelect.value = activity.period || "any";
  locationInput.value = activity.location || "";
  title.textContent = activity.name || "New Activity";

  DAY_ORDER.forEach((day) => {
    daysGroup.appendChild(
      createDayChip(day, `activity-${Date.now()}-${day}`, (activity.days || []).includes(day))
    );
  });

  nameInput.addEventListener("input", () => {
    title.textContent = nameInput.value.trim() || "New Activity";
  });

  removeBtn.addEventListener("click", () => {
    card.remove();
  });

  elements.activitiesList.appendChild(fragment);
}

async function loadSampleCsv() {
  try {
    const response = await fetch("data.csv");
    if (!response.ok) {
      throw new Error("Cannot open data.csv.");
    }
    state.csvText = await response.text();
    state.courses = parseCsv(state.csvText);
    const semesters = [...new Set(state.courses.map((course) => course.semester))].sort((a, b) => a - b);
    if (semesters.length > 0) {
      elements.semesterInput.value = semesters[0];
    }
    setMessage(`Loaded data.csv successfully. Found ${state.courses.length} course rows.`, "success");
  } catch (error) {
    setMessage("Could not auto-load data.csv. Please use a hosted link or upload the CSV manually.", "warning");
  }
}

function handleCsvUpload(event) {
  const file = event.target.files[0];
  if (!file) {
    return;
  }
  const reader = new FileReader();
  reader.onload = () => {
    state.csvText = String(reader.result || "");
    state.courses = parseCsv(state.csvText);
    setMessage(`Loaded ${file.name}. Found ${state.courses.length} course rows.`, "success");
  };
  reader.readAsText(file);
}

function handleGenerate() {
  if (!state.courses.length) {
    setMessage("Please load data.csv or upload a CSV file before generating.", "warning");
    return;
  }

  const config = getPlannerConfig();
  if (config.activeStart >= config.activeEnd) {
    setMessage("Daily end time must be later than daily start time.", "warning");
    return;
  }

  const semesterCourses = state.courses.filter((course) => course.semester === config.semester);

  const selectedCourses = buildCourseSchedule(semesterCourses, config);
  const scheduledActivities = placeActivities(selectedCourses, config.activities, config);
  const fullWeek = buildFullWeek(selectedCourses, scheduledActivities.blocks, config);

  state.schedule = {
    config,
    selectedCourses,
    personalActivities: scheduledActivities.blocks,
    unscheduledActivities: scheduledActivities.unscheduled,
    fullWeek
  };

  renderSummary(state.schedule);
  renderWeeklyOverview(state.schedule);
  renderDayDetail(state.selectedDay);

  const unscheduledCount = state.schedule.unscheduledActivities.length;
  if (!semesterCourses.length) {
    setMessage(
      `No course rows found for semester ${config.semester}. Generated a personal-life schedule only.`,
      "warning"
    );
  } else if (unscheduledCount > 0) {
    setMessage(
      `Schedule generated with ${unscheduledCount} activity block(s) that could not fit inside the selected time window.`,
      "warning"
    );
  } else {
    setMessage("Schedule generated successfully. You can now review and export Schedule.ics.", "success");
  }

  elements.exportBtn.disabled = false;
}

function getPlannerConfig() {
  return {
    semester: Number(elements.semesterInput.value || 0),
    timeMode: Number(elements.timeModeSelect.value || 0),
    activeStart: timeToMinutes(elements.activeStartInput.value || "07:30"),
    activeEnd: timeToMinutes(elements.activeEndInput.value || "22:00"),
    maxPerDay: Number(elements.maxPerDayInput.value || 0),
    minBreak: Number(elements.minBreakInput.value || 0),
    maxStudyDays: Number(elements.maxStudyDaysInput.value || 0),
    avoidLunch: elements.avoidLunchInput.checked,
    bannedDays: getCheckedValues(elements.bannedDaysGroup),
    preferredDays: getCheckedValues(elements.preferredDaysGroup),
    activities: collectActivities()
  };
}

function getCheckedValues(container) {
  return Array.from(container.querySelectorAll("input:checked")).map((input) => Number(input.value));
}

function collectActivities() {
  return Array.from(elements.activitiesList.querySelectorAll(".activity-card"))
    .map((card, index) => {
      const name = card.querySelector(".activity-name").value.trim();
      const duration = Number(card.querySelector(".activity-duration").value || 0);
      const category = card.querySelector(".activity-category").value;
      const period = card.querySelector(".activity-period").value;
      const location = card.querySelector(".activity-location").value.trim();
      const days = Array.from(card.querySelectorAll(".activity-days input:checked")).map((input) => Number(input.value));

      return {
        id: `activity-${index + 1}`,
        name,
        duration,
        category,
        period,
        location,
        days
      };
    })
    .filter((activity) => activity.name && activity.duration > 0 && activity.days.length > 0);
}

function parseCsv(text) {
  const lines = text
    .split(/\r?\n/)
    .map((line) => line.trim())
    .filter(Boolean);

  if (lines.length <= 1) {
    return [];
  }

  return lines.slice(1).map((line, index) => {
    const parts = line.split(",");
    const courseId = parts[0] || "";
    const day = Number(parts[1] || 0);
    const startTime = parts[2] || "00:00";
    const endTime = parts[3] || "00:00";
    const room = parts[4] || "";
    const weight = Number(parts[5] || 0);
    const semester = Number(parts[6] || 0);
    const type = parts[7] || "CLASS";

    return {
      id: `${courseId}-${day}-${index}`,
      courseId,
      day,
      startTime,
      endTime,
      room,
      weight,
      semester,
      type,
      startMin: timeToMinutes(startTime),
      endMin: timeToMinutes(endTime)
    };
  });
}

function buildCourseSchedule(courses, config) {
  let filtered = courses
    .filter((course) => !config.bannedDays.includes(course.day))
    .map((course) => ({ ...course, adjustedWeight: course.weight }));

  filtered.forEach((course) => {
    if (config.timeMode === 1 && course.startMin < 720) {
      course.adjustedWeight += 100;
    }
    if (config.timeMode === 2 && course.startMin >= 720) {
      course.adjustedWeight += 100;
    }
    if (config.preferredDays.includes(course.day)) {
      course.adjustedWeight += 50;
    }
  });

  if (config.avoidLunch) {
    filtered = filtered.filter((course) => course.endMin <= 690 || course.startMin >= 810);
  }

  if (config.maxPerDay > 0) {
    const byDay = groupByDay(filtered);
    filtered = [];
    DAY_ORDER.forEach((day) => {
      const dayCourses = (byDay[day] || []).slice().sort((a, b) => b.adjustedWeight - a.adjustedWeight);
      filtered.push(...dayCourses.slice(0, config.maxPerDay));
    });
  }

  if (config.maxStudyDays > 0) {
    const byDay = groupByDay(filtered);
    const dayWeights = DAY_ORDER.map((day) => ({
      day,
      weight: (byDay[day] || []).reduce((sum, course) => sum + course.adjustedWeight, 0)
    }))
      .filter((item) => item.weight > 0)
      .sort((a, b) => b.weight - a.weight);

    const allowedDays = new Set(dayWeights.slice(0, config.maxStudyDays).map((item) => item.day));
    filtered = filtered.filter((course) => allowedDays.has(course.day));
  }

  const grouped = groupByDay(filtered);
  const selected = [];

  DAY_ORDER.forEach((day) => {
    const dayCourses = (grouped[day] || []).slice().sort(compareByEndTime);
    if (!dayCourses.length) {
      return;
    }

    const dp = new Array(dayCourses.length).fill(0);
    dp[0] = dayCourses[0].adjustedWeight;

    for (let i = 1; i < dayCourses.length; i += 1) {
      let currentWeight = dayCourses[i].adjustedWeight;
      const previous = findPreviousNonConflict(dayCourses, i, config.minBreak);
      if (previous !== -1) {
        currentWeight += dp[previous];
      }
      dp[i] = Math.max(dp[i - 1], currentWeight);
    }

    let i = dayCourses.length - 1;
    const chosen = [];
    while (i >= 0) {
      if (i === 0) {
        chosen.push(dayCourses[0]);
        break;
      }

      const previous = findPreviousNonConflict(dayCourses, i, config.minBreak);
      const prevWeight = previous !== -1 ? dp[previous] : 0;

      if (dayCourses[i].adjustedWeight + prevWeight >= dp[i - 1]) {
        chosen.push(dayCourses[i]);
        i = previous;
      } else {
        i -= 1;
      }
    }

    selected.push(
      ...chosen.reverse().map((course) => ({
        id: course.id,
        day: course.day,
        title: course.courseId,
        startMin: course.startMin,
        endMin: course.endMin,
        startTime: course.startTime,
        endTime: course.endTime,
        location: course.room,
        category: "course",
        detail: course.type,
        duration: course.endMin - course.startMin
      }))
    );
  });

  return selected.sort(compareBlocks);
}

function findPreviousNonConflict(courses, index, minBreak) {
  let low = 0;
  let high = index - 1;
  let best = -1;

  while (low <= high) {
    const mid = Math.floor((low + high) / 2);
    if (courses[mid].endMin + minBreak <= courses[index].startMin) {
      best = mid;
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  return best;
}

function placeActivities(selectedCourses, activities, config) {
  const dayBlocks = groupByDay(selectedCourses);
  const scheduled = [];
  const unscheduled = [];

  DAY_ORDER.forEach((day) => {
    const occupied = (dayBlocks[day] || []).map((block) => ({ ...block }));
    const activitiesToday = activities.filter((activity) => activity.days.includes(day));
    activitiesToday.forEach((activity) => {
      const block = findSlotForActivity(occupied, activity, day, config);
      if (block) {
        occupied.push(block);
        occupied.sort(compareBlocks);
        scheduled.push(block);
      } else {
        unscheduled.push({ day, name: activity.name, duration: activity.duration });
      }
    });
  });

  return { blocks: scheduled.sort(compareBlocks), unscheduled };
}

function findSlotForActivity(existingBlocks, activity, day, config) {
  const freeSlots = calculateFreeSlots(existingBlocks, config.activeStart, config.activeEnd);
  const preferredWindow = getPreferredWindow(activity.period, config.activeStart, config.activeEnd);
  const sortedSlots = freeSlots.slice().sort((a, b) => {
    const scoreA = slotScore(a, preferredWindow);
    const scoreB = slotScore(b, preferredWindow);
    if (scoreA !== scoreB) {
      return scoreA - scoreB;
    }
    return a.startMin - b.startMin;
  });

  for (const slot of sortedSlots) {
    const actualStart = chooseActivityStart(slot, activity.duration, preferredWindow);
    if (actualStart === null) {
      continue;
    }

    const actualEnd = actualStart + activity.duration;
    if (actualEnd <= slot.endMin) {
      return {
        id: `${activity.id}-${day}`,
        day,
        title: activity.name,
        startMin: actualStart,
        endMin: actualEnd,
        startTime: minutesToTime(actualStart),
        endTime: minutesToTime(actualEnd),
        location: activity.location || "Personal time",
        category: activity.category,
        detail: activity.period === "any" ? "Flexible" : `${capitalize(activity.period)} routine`,
        duration: activity.duration
      };
    }
  }

  return null;
}

function slotScore(slot, preferredWindow) {
  if (!preferredWindow) {
    return 1;
  }
  const overlapStart = Math.max(slot.startMin, preferredWindow.start);
  const overlapEnd = Math.min(slot.endMin, preferredWindow.end);
  return overlapEnd > overlapStart ? 0 : 1;
}

function chooseActivityStart(slot, duration, preferredWindow) {
  if (!preferredWindow) {
    return slot.startMin;
  }

  const candidateStart = Math.max(slot.startMin, preferredWindow.start);
  const candidateEnd = candidateStart + duration;
  if (candidateEnd <= Math.min(slot.endMin, preferredWindow.end)) {
    return candidateStart;
  }

  if (slot.startMin + duration <= slot.endMin) {
    return slot.startMin;
  }

  return null;
}

function getPreferredWindow(period, activeStart, activeEnd) {
  const base = PERIOD_WINDOWS[period];
  if (!base) {
    return null;
  }
  return {
    start: Math.max(base.start, activeStart),
    end: Math.min(base.end, activeEnd)
  };
}

function buildFullWeek(selectedCourses, personalActivities, config) {
  const allBlocks = [...selectedCourses, ...personalActivities].sort(compareBlocks);
  const grouped = groupByDay(allBlocks);
  const result = {};

  DAY_ORDER.forEach((day) => {
    const merged = (grouped[day] || []).slice().sort(compareBlocks);
    result[day] = [];
    let cursor = config.activeStart;

    merged.forEach((block) => {
      if (block.startMin > cursor) {
        result[day].push(createFreeBlock(day, cursor, block.startMin));
      }

      if (block.endMin > config.activeStart && block.startMin < config.activeEnd) {
        result[day].push({
          ...block,
          startMin: Math.max(block.startMin, config.activeStart),
          endMin: Math.min(block.endMin, config.activeEnd),
          startTime: minutesToTime(Math.max(block.startMin, config.activeStart)),
          endTime: minutesToTime(Math.min(block.endMin, config.activeEnd)),
          duration: Math.min(block.endMin, config.activeEnd) - Math.max(block.startMin, config.activeStart)
        });
      }

      cursor = Math.max(cursor, block.endMin);
    });

    if (cursor < config.activeEnd) {
      result[day].push(createFreeBlock(day, cursor, config.activeEnd));
    }
  });

  return result;
}

function createFreeBlock(day, startMin, endMin) {
  return {
    id: `free-${day}-${startMin}`,
    day,
    title: "Free Time",
    startMin,
    endMin,
    startTime: minutesToTime(startMin),
    endTime: minutesToTime(endMin),
    location: "Flexible time",
    category: "free",
    detail: suggestFreeTimeNote(startMin, endMin),
    duration: endMin - startMin
  };
}

function suggestFreeTimeNote(startMin, endMin) {
  if (startMin < 540) {
    return "Breakfast and morning routine";
  }
  if (startMin < 780 && endMin > 720) {
    return "Lunch break";
  }
  if (startMin >= 1080) {
    return "Rest and recharge";
  }
  return "Open space for your own plans";
}

function calculateFreeSlots(blocks, activeStart, activeEnd) {
  const sorted = blocks.slice().sort(compareBlocks);
  const slots = [];
  let cursor = activeStart;

  sorted.forEach((block) => {
    const blockStart = Math.max(block.startMin, activeStart);
    const blockEnd = Math.min(block.endMin, activeEnd);
    if (blockStart > cursor) {
      slots.push({ startMin: cursor, endMin: blockStart });
    }
    cursor = Math.max(cursor, blockEnd);
  });

  if (cursor < activeEnd) {
    slots.push({ startMin: cursor, endMin: activeEnd });
  }

  return slots.filter((slot) => slot.endMin > slot.startMin);
}

function renderSummary(schedule) {
  const allBlocks = DAY_ORDER.flatMap((day) => schedule.fullWeek[day] || []);
  const courseCount = allBlocks.filter((block) => block.category === "course").length;
  const activityCount = allBlocks.filter((block) => block.category !== "course" && block.category !== "free").length;
  const freeMinutes = allBlocks
    .filter((block) => block.category === "free")
    .reduce((sum, block) => sum + block.duration, 0);
  const busyMinutes = allBlocks
    .filter((block) => block.category !== "free")
    .reduce((sum, block) => sum + block.duration, 0);

  elements.summaryStats.innerHTML = `
    <div class="stat-card">
      <strong>${courseCount}</strong>
      <span>Selected classes</span>
    </div>
    <div class="stat-card">
      <strong>${activityCount}</strong>
      <span>Personal activity blocks</span>
    </div>
    <div class="stat-card">
      <strong>${Math.round(freeMinutes / 60)}h</strong>
      <span>Free time in the week</span>
    </div>
    <div class="stat-card">
      <strong>${Math.round(busyMinutes / 60)}h</strong>
      <span>Busy time in the week</span>
    </div>
  `;
}

function renderWeeklyOverview(schedule) {
  elements.weeklyOverview.innerHTML = "";

  DAY_ORDER.forEach((day) => {
    const blocks = schedule.fullWeek[day] || [];
    const column = document.createElement("button");
    column.type = "button";
    column.className = `day-column ${state.selectedDay === day ? "active" : ""}`;
    column.addEventListener("click", () => {
      state.selectedDay = day;
      renderWeeklyOverview(schedule);
      renderDayDetail(day);
    });

    const busyCount = blocks.filter((block) => block.category !== "free").length;
    column.innerHTML = `
      <div class="day-column-header">
        <h3>${DAY_LABELS[day]}</h3>
        <p>${busyCount} planned block(s)</p>
      </div>
      <div class="mini-blocks">
        ${blocks
          .map(
            (block) => `
              <div class="mini-block ${block.category}">
                <h4>${escapeHtml(block.title)}</h4>
                <p class="time-line">${block.startTime} - ${block.endTime}</p>
              </div>
            `
          )
          .join("")}
      </div>
    `;

    elements.weeklyOverview.appendChild(column);
  });
}

function renderDayDetail(day) {
  if (!state.schedule) {
    return;
  }

  const blocks = state.schedule.fullWeek[day] || [];
  elements.detailTitle.textContent = `${DAY_LABELS[day]} Timeline`;
  elements.detailSubtitle.textContent = `Full schedule for ${DAY_LABELS[day]}.`;

  if (!blocks.length) {
    elements.dayDetail.className = "day-detail empty-state";
    elements.dayDetail.textContent = "No blocks available for this day.";
    return;
  }

  elements.dayDetail.className = "day-detail";
  elements.dayDetail.innerHTML = blocks
    .map(
      (block) => `
        <article class="detail-card ${block.category}">
          <div class="detail-top">
            <div>
              <h3>${escapeHtml(block.title)}</h3>
              <p class="time-line">${block.startTime} - ${block.endTime} | ${block.duration} min</p>
            </div>
            <span class="tag-pill">${escapeHtml(formatTag(block.category))}</span>
          </div>
          <p class="meta-line">Location: ${escapeHtml(block.location || "Flexible time")}</p>
          <p class="meta-line">Note: ${escapeHtml(block.detail || "Planned block")}</p>
        </article>
      `
    )
    .join("");
}

function exportCurrentSchedule() {
  if (!state.schedule) {
    return;
  }

  const lines = ["BEGIN:VCALENDAR", "VERSION:2.0", "PRODID:-//Life Schedule Planner//EN"];
  const baseDate = {
    2: "20260323",
    3: "20260324",
    4: "20260325",
    5: "20260326",
    6: "20260327",
    7: "20260328",
    8: "20260329"
  };

  DAY_ORDER.forEach((day) => {
    (state.schedule.fullWeek[day] || [])
      .filter((block) => block.category !== "free")
      .forEach((block, index) => {
        lines.push("BEGIN:VEVENT");
        lines.push(`UID:${day}-${index}-${Date.now()}@life-planner`);
        lines.push(`SUMMARY:${escapeIcs(block.title)}`);
        lines.push(`LOCATION:${escapeIcs(block.location || "Flexible time")}`);
        lines.push(`DESCRIPTION:${escapeIcs(block.detail || "Planned block")}`);
        lines.push(`DTSTART:${baseDate[day]}T${toIcsTime(block.startTime)}`);
        lines.push(`DTEND:${baseDate[day]}T${toIcsTime(block.endTime)}`);
        lines.push("END:VEVENT");
      });
  });

  lines.push("END:VCALENDAR");

  const blob = new Blob([lines.join("\r\n")], { type: "text/calendar;charset=utf-8" });
  const url = URL.createObjectURL(blob);
  const link = document.createElement("a");
  link.href = url;
  link.download = "Schedule.ics";
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
  URL.revokeObjectURL(url);
}

function groupByDay(blocks) {
  return blocks.reduce((accumulator, block) => {
    if (!accumulator[block.day]) {
      accumulator[block.day] = [];
    }
    accumulator[block.day].push(block);
    return accumulator;
  }, {});
}

function compareByEndTime(a, b) {
  if (a.day !== b.day) {
    return a.day - b.day;
  }
  return a.endMin - b.endMin;
}

function compareBlocks(a, b) {
  if (a.day !== b.day) {
    return a.day - b.day;
  }
  if (a.startMin !== b.startMin) {
    return a.startMin - b.startMin;
  }
  return a.endMin - b.endMin;
}

function timeToMinutes(time) {
  const [hour, minute] = time.split(":").map(Number);
  return hour * 60 + minute;
}

function minutesToTime(totalMinutes) {
  const hour = Math.floor(totalMinutes / 60);
  const minute = totalMinutes % 60;
  return `${String(hour).padStart(2, "0")}:${String(minute).padStart(2, "0")}`;
}

function formatTag(category) {
  if (category === "course") {
    return "Course";
  }
  if (category === "free") {
    return "Free";
  }
  return category;
}

function capitalize(text) {
  return text.charAt(0).toUpperCase() + text.slice(1);
}

function toIcsTime(time) {
  return `${time.replace(":", "")}00`;
}

function escapeIcs(text) {
  return String(text || "")
    .replace(/\\/g, "\\\\")
    .replace(/,/g, "\\,")
    .replace(/;/g, "\\;")
    .replace(/\n/g, "\\n");
}

function escapeHtml(text) {
  return String(text || "")
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#39;");
}

function setMessage(text, type) {
  elements.messageBox.textContent = text;
  elements.messageBox.className = `message-box ${type}`;
}
