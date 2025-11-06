/* project.c
   NeuroStay - AI-Based Hotel Management System (Windows Compatible)
   Works fully on Windows (MinGW, Code::Blocks, VS Code).
   No external libraries or windows.h needed.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* For Windows clear screen */
#ifdef _WIN32
  #define CLEAR_CMD "cls"
#else
  #define CLEAR_CMD "clear"
#endif

/* ------------------- CUSTOM FIX ------------------- */
/* strcasestr() replacement for Windows */
char *strcasestr(const char *haystack, const char *needle) {
    if (!haystack || !needle) return NULL;

    size_t needle_len = strlen(needle);
    if (needle_len == 0) return (char *)haystack;

    for (; *haystack; haystack++) {
        if (tolower((unsigned char)*haystack) == tolower((unsigned char)*needle)) {
            if (strncasecmp(haystack, needle, needle_len) == 0)
                return (char *)haystack;
        }
    }
    return NULL;
}

/* strncasecmp() replacement for Windows */
int strncasecmp(const char *s1, const char *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        unsigned char c1 = tolower((unsigned char)s1[i]);
        unsigned char c2 = tolower((unsigned char)s2[i]);
        if (c1 != c2 || c1 == '\0' || c2 == '\0')
            return c1 - c2;
    }
    return 0;
}

/* -------------------------------------------------- */

struct Booking {
    char name[64];
    char roomType[32];
    int days;
    float price;
};

/* Pause for user */
void pause_console() {
    printf("\nPress ENTER to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {} /* flush buffer */
    getchar();
}

/* Display main menu */
void mainMenu() {
    printf("===========================================\n");
    printf("        🧠 NEUROSTAY AI HOTEL SYSTEM       \n");
    printf("===========================================\n");
    printf("1. Book Room\n");
    printf("2. Give Feedback\n");
    printf("3. AI Suggestion\n");
    printf("4. Show Report\n");
    printf("5. Exit\n");
    printf("===========================================\n");
    printf("Choose an option: ");
}

/* Save booking to file */
void write_booking(const struct Booking *b) {
    FILE *f = fopen("bookings.txt", "a");
    if (!f) {
        printf("Error: cannot open bookings.txt for writing.\n");
        return;
    }
    fprintf(f, "%s|%s|%d|%.2f\n", b->name, b->roomType, b->days, b->price);
    fclose(f);
}

/* Save feedback to file */
void write_feedback(const char *name, const char *text, int rating) {
    FILE *f = fopen("feedback.txt", "a");
    if (!f) {
        printf("Error: cannot open feedback.txt for writing.\n");
        return;
    }
    fprintf(f, "%s|%s|%d\n", name, text, rating);
    fclose(f);
}

/* Booking function */
void bookRoom() {
    struct Booking b;
    char buf[256];

    printf("\nEnter customer full name: ");
    if (!fgets(b.name, sizeof(b.name), stdin)) return;
    b.name[strcspn(b.name, "\n")] = 0;

    printf("Room Type (Deluxe / Standard / Budget): ");
    if (!fgets(b.roomType, sizeof(b.roomType), stdin)) return;
    b.roomType[strcspn(b.roomType, "\n")] = 0;

    printf("Number of days: ");
    if (!fgets(buf, sizeof(buf), stdin)) return;
    b.days = atoi(buf);
    if (b.days <= 0) b.days = 1;

    if (strncasecmp(b.roomType, "Deluxe", 6) == 0) b.price = 2000.0f * b.days;
    else if (strncasecmp(b.roomType, "Standard", 8) == 0) b.price = 1500.0f * b.days;
    else b.price = 1000.0f * b.days;

    write_booking(&b);
    printf("\nBooking saved for %s. Total = ₹%.2f\n", b.name, b.price);
}

/* Feedback system */
void feedbackSystem() {
    char name[64], text[256];
    printf("\nEnter your name: ");
    if (!fgets(name, sizeof(name), stdin)) return;
    name[strcspn(name, "\n")] = 0;

    printf("Write feedback (good/bad/average): ");
    if (!fgets(text, sizeof(text), stdin)) return;
    text[strcspn(text, "\n")] = 0;

    int rating = 0;
    if (strcasestr(text, "good") || strcasestr(text, "excellent") || strcasestr(text, "nice"))
        rating = 1;
    else if (strcasestr(text, "bad") || strcasestr(text, "poor") || strcasestr(text, "terrible"))
        rating = -1;
    else
        rating = 0;

    write_feedback(name, text, rating);
    printf("\nThanks %s — your feedback is recorded.\n", name);
}

/* AI Suggestion (reads feedback.txt) */
void aiSuggestion() {
    FILE *f = fopen("feedback.txt", "r");
    if (!f) {
        printf("\nNo feedback data found.\n");
        return;
    }

    int pos = 0, neg = 0;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char *last = strrchr(line, '|');
        if (!last) continue;
        int rating = atoi(last + 1);
        if (rating > 0) pos++;
        else if (rating < 0) neg++;
    }
    fclose(f);

    printf("\n===== AI SUGGESTION =====\n");
    if (pos > neg)
        printf("Recommend promoting: DELUXE rooms (positive reviews higher)\n");
    else if (neg > pos)
        printf("Recommend improving: BUDGET rooms (negative reviews higher)\n");
    else
        printf("Neutral feedback — focus on STANDARD rooms.\n");
}

/* Show report */
void showReport() {
    FILE *f = fopen("bookings.txt", "r");
    if (!f) {
        printf("\nNo booking data found.\n");
        return;
    }

    char line[256];
    int total = 0;
    double revenue = 0.0;
    printf("\nName\t\tRoom\tDays\tPrice\n");
    printf("-----------------------------------------------\n");
    while (fgets(line, sizeof(line), f)) {
        char name[64], room[32];
        int days;
        double price;
        if (sscanf(line, "%63[^|]|%31[^|]|%d|%lf", name, room, &days, &price) == 4) {
            printf("%-12s %-10s %-4d ₹%.2f\n", name, room, days, price);
            total++;
            revenue += price;
        }
    }
    fclose(f);

    printf("-----------------------------------------------\n");
    printf("Total bookings: %d\n", total);
    printf("Total revenue : ₹%.2f\n", revenue);
}

/* MAIN */
int main() {
    char buf[32];
    while (1) {
        system(CLEAR_CMD);
        mainMenu();

        if (!fgets(buf, sizeof(buf), stdin)) break;
        int choice = atoi(buf);

        if (choice == 1) bookRoom();
        else if (choice == 2) feedbackSystem();
        else if (choice == 3) aiSuggestion();
        else if (choice == 4) showReport();
        else if (choice == 5) {
            printf("\nGoodbye!\n"); 
            break;
        } else
            printf("\nInvalid option. Try again.\n");

        pause_console();
    }
    return 0;
}