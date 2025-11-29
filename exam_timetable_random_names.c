#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 128

int default_room_capacity[] = {30, 40, 50, 60, 80, 100};
int default_exam_students[] = {50, 45, 60, 55, 40, 35, 50, 45, 60, 55, 40, 35, 50, 45, 60, 55, 40, 35, 50, 45};

int totalExams = 20;
int totalRooms = 6;
int totalFaculties = 10;
int totalTimeslots = 0;

char **roomName = NULL;
int *roomCapacity = NULL;

char **facultyName = NULL;
int **facultyAvailability = NULL;

char **subjectName = NULL;
int *examStudents = NULL;
int **examConflict = NULL;
int *examFacultyAssigned = NULL;

char **timeslotLabel = NULL;

int *examAssignedTimeslot = NULL;
int *examAssignedRoom = NULL;
int **timeslotRoomUsed = NULL;
int *examOrder = NULL;
int *examDegree = NULL;

enum
{
    EXAM_W = 5,
    SUBJECT_W = 28,
    STUDENT_W = 8,
    FACULTY_W = 28,
    DATETIME_W = 34,
    ROOM_W = 20
};

static void safe_free(void **p)
{
    if (p && *p)
    {
        free(*p);
        *p = NULL;
    }
}

static void read_line_trim(char *buf, int bufsz)
{
    if (!fgets(buf, bufsz, stdin))
    {
        buf[0] = 0;
        return;
    }
    size_t L = strlen(buf);
    while (L && (buf[L - 1] == '\n' || buf[L - 1] == '\r'))
    {
        buf[L - 1] = 0;
        L--;
    }
}

static void gen_default_names(void)
{
    const char *subjects[] = {"Calculus", "Physics", "Chemistry", "Biology", "Algorithms", "DataStructures", "OS", "DBMS", "Networks", "AI", "ML", "LinearAlgebra", "Probability", "Statistics", "Electronics", "SoftEng", "Compiler", "Thermo", "DigitalLogic", "Economics"};
    const char *first[] = {"Alex", "Sam", "Jordan", "Taylor", "Morgan", "Casey", "Riley", "Jamie", "Avery", "Drew", "Cameron", "Quinn", "Blake", "Dylan", "Rowan", "Skyler", "Reese", "Parker", "Elliot", "Harper"};
    const char *last[] = {"Patel", "Sharma", "Iyer", "Khan", "Singh", "Gupta", "Das", "Nair", "Rao", "Mehta", "Verma", "Chaudhary", "Malhotra", "Kapoor", "Joshi", "Bose", "Kaur", "Saxena", "Kohli", "Trivedi"};
    const char *build[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
    srand((unsigned int)time(NULL));
    for (int i = 0; i < totalRooms; i++)
    {
        int b = rand() % 10;
        int num = 100 + rand() % 900;
        snprintf(roomName[i], MAX_NAME_LEN, "Room_%s_%d", build[b], num);
        if (i < (int)(sizeof(default_room_capacity) / sizeof(default_room_capacity[0])))
            roomCapacity[i] = default_room_capacity[i];
        else
            roomCapacity[i] = 30 + (rand() % 8) * 10;
    }
    for (int i = 0; i < totalFaculties; i++)
    {
        int a = rand() % 20, b = rand() % 20;
        snprintf(facultyName[i], MAX_NAME_LEN, "Prof_%s_%s", first[a], last[b]);
        for (int t = 0; t < totalTimeslots; t++)
            facultyAvailability[i][t] = 1;
    }
    for (int i = 0; i < totalExams; i++)
    {
        int r = rand() % 20;
        int n = rand() % 100;
        snprintf(subjectName[i], MAX_NAME_LEN, "%s_%d", subjects[r], n);
        if (i < (int)(sizeof(default_exam_students) / sizeof(default_exam_students[0])))
            examStudents[i] = default_exam_students[i];
        else
            examStudents[i] = 30 + (rand() % 7) * 5;
        examFacultyAssigned[i] = i % totalFaculties;
    }
}

static void compute_degrees_and_order(void)
{
    for (int i = 0; i < totalExams; i++)
    {
        int cnt = 0;
        for (int j = 0; j < totalExams; j++)
            if (examConflict[i][j])
                cnt++;
        examDegree[i] = cnt;
        examOrder[i] = i;
    }

    int cmp(const void *a, const void *b)
    {
        int ia = *(const int *)a, ib = *(const int *)b;
        if (examDegree[ia] != examDegree[ib])
            return examDegree[ib] - examDegree[ia];
        return examStudents[ib] - examStudents[ia];
    }
    qsort(examOrder, totalExams, sizeof(int), cmp);
}

static int can_place(int exam, int timeslot, int room)
{
    if (roomCapacity[room] < examStudents[exam])
        return 0;
    if (timeslotRoomUsed[timeslot][room])
        return 0;
    int f = examFacultyAssigned[exam];
    if (f < 0 || f >= totalFaculties)
        return 0;
    if (!facultyAvailability[f][timeslot])
        return 0;
    for (int e = 0; e < totalExams; e++)
    {
        if (examAssignedTimeslot[e] == timeslot && examConflict[exam][e])
            return 0;
    }
    return 1;
}

static int backtrack(int idx)
{
    if (idx == totalExams)
        return 1;
    int exam = examOrder[idx];
    for (int t = 0; t < totalTimeslots; t++)
    {
        for (int r = 0; r < totalRooms; r++)
        {
            if (can_place(exam, t, r))
            {
                examAssignedTimeslot[exam] = t;
                examAssignedRoom[exam] = r;
                timeslotRoomUsed[t][r] = 1;
                if (backtrack(idx + 1))
                    return 1;
                timeslotRoomUsed[t][r] = 0;
                examAssignedTimeslot[exam] = -1;
                examAssignedRoom[exam] = -1;
            }
        }
    }
    return 0;
}

static int build_timeslots_from_dates(int sy, int sm, int sd, int ey, int em, int ed)
{
    struct tm st = {0}, en = {0};
    st.tm_year = sy - 1900;
    st.tm_mon = sm - 1;
    st.tm_mday = sd;
    st.tm_hour = 0;
    en.tm_year = ey - 1900;
    en.tm_mon = em - 1;
    en.tm_mday = ed;
    en.tm_hour = 0;
    time_t start = mktime(&st), end = mktime(&en);
    if (start == (time_t)-1 || end == (time_t)-1 || end < start)
        return 0;

    char **dates = NULL;
    int datesCount = 0;
    for (time_t cur = start; cur <= end; cur += 24 * 3600)
    {
        struct tm *ct = localtime(&cur);
        if (ct->tm_wday == 0 || ct->tm_wday == 6)
            continue;
        dates = realloc(dates, (datesCount + 1) * sizeof(char *));
        dates[datesCount] = malloc(64);
        strftime(dates[datesCount], 64, "%Y-%m-%d %A", ct);
        datesCount++;
    }
    if (datesCount == 0)
    {
        for (int i = 0; i < datesCount; i++)
            free(dates[i]);
        free(dates);
        return 0;
    }

    totalTimeslots = datesCount * 2;
    timeslotLabel = malloc(totalTimeslots * sizeof(char *));
    for (int i = 0; i < totalTimeslots; i++)
    {
        timeslotLabel[i] = malloc(128);
        int d = i / 2, slot = i % 2;
        snprintf(timeslotLabel[i], 128, "%s %s", dates[d], slot == 0 ? "09:00-12:00" : "14:00-17:00");
    }
    for (int i = 0; i < datesCount; i++)
        free(dates[i]);
    free(dates);
    return 1;
}

static char **alloc_names(int n)
{
    char **arr = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++)
    {
        arr[i] = malloc(MAX_NAME_LEN);
        arr[i][0] = 0;
    }
    return arr;
}

static int **alloc_int_matrix(int rows, int cols)
{
    int **m = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        m[i] = calloc(cols, sizeof(int));
    }
    return m;
}

static void print_timetable_header(void)
{
    printf("\n=== EXAM TIMETABLE GENERATED SUCCESSFULLY ===\n\n");
    printf("%-*s | %-*s | %-*s | %-*s | %-*s | %-*s\n",
           EXAM_W, "Exam", SUBJECT_W, "Subject", STUDENT_W, "Students",
           FACULTY_W, "Faculty", DATETIME_W, "Date & Time", ROOM_W, "Room");
    int sep = EXAM_W + SUBJECT_W + STUDENT_W + FACULTY_W + DATETIME_W + ROOM_W + 5 * 3;
    for (int i = 0; i < sep; i++)
        putchar('-');
    putchar('\n');
}

static void print_timetable_row(int i)
{
    printf("%-*d | ", EXAM_W, i);
    printf("%-*.*s | ", SUBJECT_W, SUBJECT_W, subjectName[i]);
    printf("%*d | ", STUDENT_W, examStudents[i]);
    printf("%-*.*s | ", FACULTY_W, FACULTY_W, facultyName[examFacultyAssigned[i]]);
    printf("%-*.*s | ", DATETIME_W, DATETIME_W, timeslotLabel[examAssignedTimeslot[i]]);
    printf("%-*.*s\n", ROOM_W, ROOM_W, roomName[examAssignedRoom[i]]);
}

static int prompt_int(const char *msg, int def)
{
    char buf[128];
    printf("%s", msg);
    if (!fgets(buf, sizeof(buf), stdin))
        return def;
    if (buf[0] == '\n')
        return def;
    return atoi(buf);
}

int main(void)
{
    char buf[256];

    printf("=== Exam Timetable Generator (Menu-driven) ===\n\n");

    printf("Enter number of exams (default 20): ");
    if (!fgets(buf, sizeof(buf), stdin))
        return 0;
    if (buf[0] != '\n')
        totalExams = atoi(buf);
    else
        totalExams = 20;

    printf("Enter number of rooms (default 6): ");
    if (!fgets(buf, sizeof(buf), stdin))
        return 0;
    if (buf[0] != '\n')
        totalRooms = atoi(buf);
    else
        totalRooms = 6;

    printf("Enter number of faculties (default 10): ");
    if (!fgets(buf, sizeof(buf), stdin))
        return 0;
    if (buf[0] != '\n')
        totalFaculties = atoi(buf);
    else
        totalFaculties = 10;

    roomName = alloc_names(totalRooms);
    facultyName = alloc_names(totalFaculties);
    subjectName = alloc_names(totalExams);

    roomCapacity = malloc(totalRooms * sizeof(int));
    examStudents = malloc(totalExams * sizeof(int));
    examFacultyAssigned = malloc(totalExams * sizeof(int));

    printf("\nEnter start date YYYY MM DD (or press Enter for default today): ");
    int sy = 0, sm = 0, sd = 0, ey = 0, em = 0, ed = 0;
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n')
    {
        if (sscanf(buf, "%d %d %d", &sy, &sm, &sd) != 3)
            sy = 0;
    }
    printf("Enter end date YYYY MM DD (or press Enter for default +13 days): ");
    if (fgets(buf, sizeof(buf), stdin) && buf[0] != '\n')
    {
        if (sscanf(buf, "%d %d %d", &ey, &em, &ed) != 3)
            ey = 0;
    }

    time_t now = time(NULL);
    struct tm tm_now = *localtime(&now);
    if (sy == 0)
    {
        sy = tm_now.tm_year + 1900;
        sm = tm_now.tm_mon + 1;
        sd = tm_now.tm_mday;
    }
    if (ey == 0)
    {
        struct tm tmp = tm_now;
        tmp.tm_mday += 13;
        mktime(&tmp);
        ey = tmp.tm_year + 1900;
        em = tmp.tm_mon + 1;
        ed = tmp.tm_mday;
    }
    if (!build_timeslots_from_dates(sy, sm, sd, ey, em, ed))
    {
        printf("Failed to build timeslots (invalid dates). Exiting.\n");
        return 0;
    }

    facultyAvailability = alloc_int_matrix(totalFaculties, totalTimeslots);

    printf("\nDo you want to manually enter room names & capacities? (y/N): ");
    read_line_trim(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y')
    {
        for (int i = 0; i < totalRooms; i++)
        {
            printf("Room %d name: ", i);
            read_line_trim(roomName[i], MAX_NAME_LEN);
            if (roomName[i][0] == 0)
                snprintf(roomName[i], MAX_NAME_LEN, "Room_%d", i);
            printf("Room %d capacity: ", i);
            int cap = prompt_int("", 0);
            roomCapacity[i] = (cap > 0) ? cap : 30;
        }
    }
    else
    {
        for (int i = 0; i < totalRooms; i++)
        {
            if (i < sizeof(default_room_capacity) / sizeof(default_room_capacity[0]))
                roomCapacity[i] = default_room_capacity[i];
            else
                roomCapacity[i] = 30 + (rand() % 8) * 10;
        }
        gen_default_names();
    }

    printf("\nDo you want to manually enter faculty names and availability? (y/N): ");
    read_line_trim(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y')
    {
        for (int f = 0; f < totalFaculties; f++)
        {
            printf("Faculty %d name: ", f);
            read_line_trim(facultyName[f], MAX_NAME_LEN);
            if (facultyName[f][0] == 0)
                snprintf(facultyName[f], MAX_NAME_LEN, "Prof_%d", f);
            printf("Is %s available for all timeslots? (Y/n): ", facultyName[f]);
            read_line_trim(buf, sizeof(buf));
            if (buf[0] == 'n' || buf[0] == 'N')
            {
                for (int t = 0; t < totalTimeslots; t++)
                {
                    printf("Available at timeslot %d (%s)? (1/0): ", t, timeslotLabel[t]);
                    int a = prompt_int("", 1);
                    facultyAvailability[f][t] = (a == 0) ? 0 : 1;
                }
            }
            else
            {
                for (int t = 0; t < totalTimeslots; t++)
                    facultyAvailability[f][t] = 1;
            }
        }
    }
    else
    {
        for (int f = 0; f < totalFaculties; f++)
        {
            if (facultyName[f][0] == 0)
                snprintf(facultyName[f], MAX_NAME_LEN, "Prof_%d", f);
            for (int t = 0; t < totalTimeslots; t++)
                facultyAvailability[f][t] = 1;
        }
    }

    printf("\nDo you want to manually enter subjects and student counts? (y/N): ");
    read_line_trim(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y')
    {
        for (int e = 0; e < totalExams; e++)
        {
            printf("Subject %d name: ", e);
            read_line_trim(subjectName[e], MAX_NAME_LEN);
            if (subjectName[e][0] == 0)
                snprintf(subjectName[e], MAX_NAME_LEN, "Subject_%d", e);
            printf("Students for %s: ", subjectName[e]);
            examStudents[e] = prompt_int("", 30);
        }
        for (int e = 0; e < totalExams; e++)
        {
            printf("Assign faculty index for exam %d (%s) [0..%d] (default %d): ", e, subjectName[e], totalFaculties - 1, e % totalFaculties);
            int fid = prompt_int("", e % totalFaculties);
            if (fid < 0 || fid >= totalFaculties)
                fid = e % totalFaculties;
            examFacultyAssigned[e] = fid;
        }
    }
    else
    {
        for (int e = 0; e < totalExams; e++)
        {
            if (subjectName[e][0] == 0)
                snprintf(subjectName[e], MAX_NAME_LEN, "Subject_%d", e);
            if (e < sizeof(default_exam_students) / sizeof(default_exam_students[0]))
                examStudents[e] = default_exam_students[e];
            else
                examStudents[e] = 30 + (rand() % 7) * 5;
            examFacultyAssigned[e] = e % totalFaculties;
        }
    }

    examConflict = alloc_int_matrix(totalExams, totalExams);
    printf("\nDo you want to auto-generate conflict groups? (y/N): ");
    read_line_trim(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y')
    {
        int groupSize = prompt_int("Group size (default 4): ", 4);
        if (groupSize < 2)
            groupSize = 2;
        int idx = 0;
        while (idx < totalExams)
        {
            int end = idx + groupSize;
            if (end > totalExams)
                end = totalExams;
            for (int a = idx; a < end; a++)
                for (int b = idx; b < end; b++)
                    if (a != b)
                        examConflict[a][b] = 1;
            idx += groupSize;
        }
    }
    else
    {
        printf("Enter number of conflict pairs you will provide: ");
        int pairs = prompt_int("", 0);
        for (int p = 0; p < pairs; p++)
        {
            printf("Enter pair as: examA examB: ");
            int a = -1, b = -1;
            if (scanf("%d %d", &a, &b) == 2)
            {
                if (a >= 0 && a < totalExams && b >= 0 && b < totalExams && a != b)
                {
                    examConflict[a][b] = examConflict[b][a] = 1;
                }
            }
            while (getchar() != '\n')
                ;
        }
    }

    examAssignedTimeslot = malloc(totalExams * sizeof(int));
    examAssignedRoom = malloc(totalExams * sizeof(int));
    timeslotRoomUsed = malloc(totalTimeslots * sizeof(int *));
    for (int t = 0; t < totalTimeslots; t++)
        timeslotRoomUsed[t] = calloc(totalRooms, sizeof(int));
    for (int e = 0; e < totalExams; e++)
    {
        examAssignedTimeslot[e] = -1;
        examAssignedRoom[e] = -1;
    }

    examOrder = malloc(totalExams * sizeof(int));
    examDegree = malloc(totalExams * sizeof(int));

    compute_degrees_and_order();

    if (totalTimeslots < totalExams)
    {
        printf("Not enough timeslots (%d) for %d exams. Increase date range.\n", totalTimeslots, totalExams);
        return 0;
    }

    int ok = backtrack(0);

    if (!ok)
    {
        printf("No feasible timetable found with current inputs.\n");
        return 0;
    }

    print_timetable_header();
    for (int i = 0; i < totalExams; i++)
        print_timetable_row(i);

    for (int i = 0; i < totalRooms; i++)
        free(roomName[i]);
    free(roomName);
    free(roomCapacity);
    for (int i = 0; i < totalFaculties; i++)
        free(facultyName[i]);
    free(facultyName);
    for (int i = 0; i < totalExams; i++)
        free(subjectName[i]);
    free(subjectName);
    for (int t = 0; t < totalTimeslots; t++)
        free(timeslotLabel[t]);
    free(timeslotLabel);

    for (int i = 0; i < totalTimeslots; i++)
        free(timeslotRoomUsed[i]);
    free(timeslotRoomUsed);

    for (int i = 0; i < totalFaculties; i++)
        free(facultyAvailability[i]);
    free(facultyAvailability);

    for (int i = 0; i < totalExams; i++)
        free(examConflict[i]);
    free(examConflict);
    free(examAssignedTimeslot);
    free(examAssignedRoom);
    free(examOrder);
    free(examDegree);

    return 0;
}
