#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// Color codes
#define R "\033[0m"     // Reset
#define B "\033[1m"     // Bold
#define RD "\033[31m"   // Red
#define G "\033[32m"    // Green
#define Y "\033[33m"    // Yellow
#define C "\033[36m"    // Cyan
#define M "\033[35m"    // Magenta

// Structs
struct Player {
    char name[50];
    int wins, losses, draws;
};

struct Match {
    char player1[50], player2[50], date[11];
    int score1, score2;
};

// Global variables
struct Player players[100];
struct Match matches[100];
int playerCount = 0, matchCount = 0;

// Function prototypes
void clearScreen() { printf("\033[H\033[J"); }
void flushInput() { int c; while ((c = getchar()) != '\n' && c != EOF); }

// Validation functions
int validateName(char *name) {
    if (strlen(name) == 0) {
        printf("%sError: Name cannot be empty.%s\n", RD, R);
        return 0;
    }
    return 1;
}

int validateScore(int score) {
    if (score < 0) {
        printf("%sError: Score cannot be negative.%s\n", RD, R);
        return 0;
    }
    return 1;
}

int validateDate(char *date) {
    if (strlen(date) != 10 || date[2] != '/' || date[5] != '/') {
        printf("%sError: Date must be in format DD/MM/YYYY.%s\n", RD, R);
        return 0;
    }

    for (int i = 0; i < 10; i++)
        if (i != 2 && i != 5 && !isdigit(date[i])) {
            printf("%sError: Date format DD/MM/YYYY needed.%s\n", RD, R);
            return 0;
        }

    int day = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
               (date[8] - '0') * 10 + (date[9] - '0');

    if (month < 1 || month > 12) {
        printf("%sError: Month must be between 1 and 12.%s\n", RD, R);
        return 0;
    }

    int maxDays = 31;
    if (month == 4 || month == 6 || month == 9 || month == 11)
        maxDays = 30;
    else if (month == 2)
        maxDays = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;

    if (day < 1 || day > maxDays) {
        printf("%sError: Invalid day for month.%s\n", RD, R);
        return 0;
    }

    return 1;
}

// Find or create player
int findOrCreatePlayer(char *name) {
    for (int i = 0; i < playerCount; i++)
        if (strcmp(players[i].name, name) == 0)
            return i;

    strcpy(players[playerCount].name, name);
    players[playerCount].wins = players[playerCount].losses = players[playerCount].draws = 0;
    return playerCount++;
}

// Save data functions
void saveData() {
    FILE *pFile = fopen("players.txt", "w");
    if (pFile) {
        for (int i = 0; i < playerCount; i++)
            fprintf(pFile, "%s|%d|%d|%d\n", players[i].name, players[i].wins,
                   players[i].losses, players[i].draws);
        fclose(pFile);
    }

    FILE *mFile = fopen("matches.txt", "w");
    if (mFile) {
        for (int i = 0; i < matchCount; i++)
            fprintf(mFile, "%s|%d|%s|%d|%s\n", matches[i].player1, matches[i].score1,
                   matches[i].player2, matches[i].score2, matches[i].date);
        fclose(mFile);
    }
}

// Load data from files
void loadData() {
    FILE *pFile = fopen("players.txt", "r");
    if (pFile) {
        char line[200];
        while (fgets(line, sizeof(line), pFile)) {
            char *name = strtok(line, "|");
            char *wins = strtok(NULL, "|");
            char *losses = strtok(NULL, "|");
            char *draws = strtok(NULL, "|");

            if (name && wins && losses && draws) {
                strcpy(players[playerCount].name, name);
                players[playerCount].wins = atoi(wins);
                players[playerCount].losses = atoi(losses);
                players[playerCount].draws = atoi(draws);
                playerCount++;
            }
        }
        fclose(pFile);
        printf("Loaded %s%d players%s\n", G, playerCount, R);
    }

    FILE *mFile = fopen("matches.txt", "r");
    if (mFile) {
        char line[200];
        while (fgets(line, sizeof(line), mFile)) {
            char *player1 = strtok(line, "|");
            char *score1 = strtok(NULL, "|");
            char *player2 = strtok(NULL, "|");
            char *score2 = strtok(NULL, "|");
            char *date = strtok(NULL, "|");

            if (player1 && score1 && player2 && score2) {
                strcpy(matches[matchCount].player1, player1);
                matches[matchCount].score1 = atoi(score1);
                strcpy(matches[matchCount].player2, player2);
                matches[matchCount].score2 = atoi(score2);

                if (date) {
                    date[strcspn(date, "\n")] = 0;
                    strcpy(matches[matchCount].date, date);
                } else {
                    strcpy(matches[matchCount].date, "01/01/2025");
                }
                matchCount++;
            }
        }
        fclose(mFile);
        printf("Loaded %s%d matches%s\n", G, matchCount, R);
    }
}

// Add match function
void addMatch() {
    char player1[50], player2[50], date[11];
    int score1, score2, valid;

    clearScreen();
    printf("%s===== Add New Match =====%s\n\n", C, R);

    // Player 1 input
    do {
        printf("Enter %sPlayer 1%s name: ", Y, R);
        scanf(" %49[^\n]", player1);
        flushInput();
        valid = validateName(player1);
    } while (!valid);

    do {
        printf("Enter %sPlayer 1%s score: ", Y, R);
        if (scanf("%d", &score1) != 1) {
            printf("%sError: Enter a valid number.%s\n", RD, R);
            flushInput();
            valid = 0;
        } else {
            flushInput();
            valid = validateScore(score1);
        }
    } while (!valid);

    // Player 2 input
    do {
        printf("Enter %sPlayer 2%s name: ", G, R);
        scanf(" %49[^\n]", player2);
        flushInput();
        valid = validateName(player2);
    } while (!valid);

    do {
        printf("Enter %sPlayer 2%s score: ", G, R);
        if (scanf("%d", &score2) != 1) {
            printf("%sError: Enter a valid number.%s\n", RD, R);
            flushInput();
            valid = 0;
        } else {
            flushInput();
            valid = validateScore(score2);
        }
    } while (!valid);

    // Date input
    do {
        printf("Enter match date (%sDD/MM/YYYY%s): ", M, R);
        scanf(" %10[^\n]", date);
        flushInput();
        valid = validateDate(date);
    } while (!valid);

    // Process match
    int index1 = findOrCreatePlayer(player1);
    int index2 = findOrCreatePlayer(player2);

    strcpy(matches[matchCount].player1, player1);
    matches[matchCount].score1 = score1;
    strcpy(matches[matchCount].player2, player2);
    matches[matchCount].score2 = score2;
    strcpy(matches[matchCount].date, date);
    matchCount++;

    if (score1 > score2) {
        players[index1].wins++;
        players[index2].losses++;
    } else if (score2 > score1) {
        players[index2].wins++;
        players[index1].losses++;
    } else {
        players[index1].draws++;
        players[index2].draws++;
    }

    saveData();
    printf("\n%sMatch added successfully!%s\n", G, R);
}

// Sort and show rankings
void showRankings() {
    clearScreen();

    // Quick sort by wins
    for (int i = 0; i < playerCount - 1; i++)
        for (int j = 0; j < playerCount - i - 1; j++)
            if (players[j].wins < players[j + 1].wins) {
                struct Player temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }

    printf("%s===== Player Rankings =====%s\n\n", C, R);

    if (playerCount == 0) {
        printf("%sNo players yet.%s\n", Y, R);
        return;
    }

    printf("%-5s %-20s %-10s %-10s %-10s\n", "Rank", "Player", "Wins", "Losses", "Draws");
    printf("------------------------------------------------------\n");

    for (int i = 0; i < playerCount; i++) {
        printf("%-5d %s%-20s%s %-10d %-10d %-10d\n",
               i + 1, Y, players[i].name, R,
               players[i].wins, players[i].losses, players[i].draws);
    }
}

// Show match history
void showMatchHistory() {
    clearScreen();
    printf("%s===== Match History =====%s\n\n", C, R);

    if (matchCount == 0) {
        printf("%sNo matches yet.%s\n", Y, R);
        return;
    }

    for (int i = 0; i < matchCount; i++) {
        printf("%d. %s%s%s %s%d%s - %s%d%s %s%s%s (%s%s%s)\n",
               i + 1,
               Y, matches[i].player1, R,
               (matches[i].score1 > matches[i].score2) ? G : RD, matches[i].score1, R,
               (matches[i].score2 > matches[i].score1) ? G : RD, matches[i].score2, R,
               G, matches[i].player2, R,
               M, matches[i].date, R);
    }
}

// Main menu
void menu() {
    int choice, valid;

    while (1) {
        clearScreen();
        printf("%s===============================%s\n", C, R);
        printf("%s     TABLE TENNIS TOURNAMENT    %s\n", B, R);
        printf("%s===============================%s\n\n", C, R);

        printf("1. %sAdd Match Result%s\n", Y, R);
        printf("2. %sView Rankings%s\n", G, R);
        printf("3. %sView Match History%s\n", C, R);
        printf("4. %sExit%s\n\n", RD, R);

        do {
            printf("Select an option (1-4): ");
            if (scanf("%d", &choice) != 1) {
                printf("%sError: Enter a valid number.%s\n", RD, R);
                flushInput();
                valid = 0;
            } else {
                flushInput();
                valid = (choice >= 1 && choice <= 4);
                if (!valid)
                    printf("%sError: Enter 1-4.%s\n", RD, R);
            }
        } while (!valid);

        if (choice == 1) addMatch();
        else if (choice == 2) showRankings();
        else if (choice == 3) showMatchHistory();
        else if (choice == 4) {
            printf("%sExiting program...%s\n", Y, R);
            return;
        }

        printf("\nPress Enter to continue...");
        getchar();
    }
}

// Main function
int main() {
    clearScreen();
    printf("%s== TABLE TENNIS TOURNAMENT ==%s\n\n", C, R);
    loadData();
    menu();
    return 0;
}