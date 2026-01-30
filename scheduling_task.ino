#include <time.h>

// These live in your main file already:
extern const int MAX_TASKS;
extern String taskList[];
extern int taskCount;
extern bool taskActive;
extern String currentTask;
extern UniversalTelegramBot bot;

// NEW arrays for scheduling (defined here)
long taskDue[10];       // adjust size to match MAX_TASKS (see note below)
int  taskLeadSec[10];

// ---------- TIME ----------
void setupTime() {
  // Zurich / CET-CEST timezone rule
  configTzTime("CET-1CEST,M3.5.0/02,M10.5.0/03", "pool.ntp.org", "time.nist.gov");
}

long nowEpoch() {
  time_t now;
  time(&now);
  return (long)now;
}

// dateStr = "2026-01-25", timeStr = "17:00"
long parseDateTimeToEpoch(String dateStr, String timeStr) {
  int Y = dateStr.substring(0, 4).toInt();
  int M = dateStr.substring(5, 7).toInt();
  int D = dateStr.substring(8, 10).toInt();
  int h = timeStr.substring(0, 2).toInt();
  int m = timeStr.substring(3, 5).toInt();

  struct tm t;
  t.tm_year = Y - 1900;
  t.tm_mon  = M - 1;
  t.tm_mday = D;
  t.tm_hour = h;
  t.tm_min  = m;
  t.tm_sec  = 0;
  t.tm_isdst = -1;

  return (long)mktime(&t);
}

// ---------- READY LOGIC ----------
bool isTaskReady(int i, long now) {
  if (taskDue[i] == 0) return true; // normal task always ready
  return (now >= (taskDue[i] - taskLeadSec[i]));
}

// Keep your current behavior: show the LAST ready task received
void syncCurrentTask() {
  long now = nowEpoch();

  // debug print (optional)
  time_t tnow = (time_t)now;
  Serial.println(ctime(&tnow));

  int lastReady = -1;
  for (int i = 0; i < taskCount; i++) {
    if (isTaskReady(i, now)) lastReady = i;
  }

  taskActive = (lastReady != -1);

  if (!taskActive) currentTask = "";
  else currentTask = taskList[lastReady];
}


// ---------- ADD TASKS ----------
void addTaskImmediate(String t) {
  if (taskCount >= 10) return;   // adjust to MAX_TASKS if you want (see note)
  taskList[taskCount] = t;
  taskDue[taskCount] = 0;
  taskLeadSec[taskCount] = 0;
  taskCount++;
  syncCurrentTask();
}

// leadHours: how many hours before due to start showing
void addTaskScheduled(String t, String dateStr, String timeStr, int leadHours) {
  if (taskCount >= 10) return;

  taskList[taskCount] = t;
  taskDue[taskCount] = parseDateTimeToEpoch(dateStr, timeStr);
  taskLeadSec[taskCount] = leadHours * 3600;
  taskCount++;
  syncCurrentTask();
}

// ---------- REMOVE TASK ----------
void removeTaskAt(int index) {
  if (index < 0 || index >= taskCount) return;

  for (int i = index; i < taskCount - 1; i++) {
    taskList[i] = taskList[i + 1];
    taskDue[i] = taskDue[i + 1];
    taskLeadSec[i] = taskLeadSec[i + 1];
  }
  taskCount--;
  syncCurrentTask();
}

// ---------- TELEGRAM COMMAND PARSER ----------
bool handleMaybeScheduledCommand(String text, String chat_id) {
  if (!text.startsWith("/remind ")) return false;

  // Find spaces
  int p1 = text.indexOf(' ');           // after /remind
  int p2 = text.indexOf(' ', p1 + 1);   // after date
  int p3 = text.indexOf(' ', p2 + 1);   // after time
  if (p2 == -1 || p3 == -1) {
    bot.sendMessage(chat_id, "Format: /remind DD-MM HH:MM (optional 5m/3h) Task", "");
    return true;
  }

  String dateStr = text.substring(p1 + 1, p2);
  String timeStr = text.substring(p2 + 1, p3);

  // Now we don't know if next token is lead or task text
  int p4 = text.indexOf(' ', p3 + 1);   // after possible lead
  String token4 = (p4 == -1) ? text.substring(p3 + 1) : text.substring(p3 + 1, p4);

  int leadSeconds = 0;
  String taskText;

  // If token4 ends with m or h AND starts with a digit, treat it as lead
  if (token4.length() > 1 && isDigit(token4[0]) && (token4.endsWith("m") || token4.endsWith("h"))) {
    if (token4.endsWith("m")) leadSeconds = token4.toInt() * 60;
    else leadSeconds = token4.toInt() * 3600;

    if (p4 == -1) taskText = "";
    else taskText = text.substring(p4 + 1);
  } else {
    // No lead provided: show exactly at due time
    leadSeconds = 0;
    taskText = text.substring(p3 + 1);
  }

  // parse dateStr (DD-MM or DD-MM-YYYY)
  int d1 = dateStr.indexOf('-');
  int d2 = dateStr.indexOf('-', d1 + 1);

  int DD = dateStr.substring(0, d1).toInt();
  int MM = (d2 == -1) ? dateStr.substring(d1 + 1).toInt()
                      : dateStr.substring(d1 + 1, d2).toInt();

  int YYYY;
  if (d2 == -1) {
    time_t n; time(&n);
    struct tm *ti = localtime(&n);
    YYYY = ti->tm_year + 1900;   // current year
  } else {
    YYYY = dateStr.substring(d2 + 1).toInt();
  }

  long due = makeEpochFromDMY(DD, MM, YYYY, timeStr);
  addTaskScheduledEpoch(taskText, due, leadSeconds);

  // Feedback
  String msg = "✅ Scheduled: " + taskText + "\nDue: " + dateStr + " " + timeStr;
  if (leadSeconds == 0) msg += "\nShows: at that time";
  else msg += "\nShows: " + String(leadSeconds / 60) + " min before";
  bot.sendMessage(chat_id, msg, "");
  return true;
}


void addTaskScheduledEpoch(String t, long dueEpoch, int leadSeconds) {
  if (taskCount >= MAX_TASKS) return;
  taskList[taskCount] = t;
  taskDue[taskCount] = dueEpoch;
  taskLeadSec[taskCount] = leadSeconds;
  taskCount++;
  syncCurrentTask();
}

long makeEpochFromDMY(int DD, int MM, int YYYY, String timeStr) {
  int h = timeStr.substring(0, 2).toInt();
  int m = timeStr.substring(3, 5).toInt();

  struct tm t;
  t.tm_year = YYYY - 1900;
  t.tm_mon  = MM - 1;
  t.tm_mday = DD;
  t.tm_hour = h;
  t.tm_min  = m;
  t.tm_sec  = 0;
  t.tm_isdst = -1;

  return (long)mktime(&t);
}



