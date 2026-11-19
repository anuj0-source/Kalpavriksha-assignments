#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

typedef struct PNode {
    int id;
    char name[51];
    char team[51];
    int role;
    int runs;
    float avg;
    float sr;
    int wkts;
    float er;
    float perf;
    struct PNode *next;
} PNode;

typedef struct {
    int id;
    char name[51];
    int total;
    float srSum;
    int batCount;
    float avgBatSr;
    PNode *lists[3];
} Team;

Team teamArr[20];
int totalTeams = 0;

void clearBuf() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getRoleCode(const char *r) {
    if (strcmp(r, "Batsman") == 0) return 0;
    if (strcmp(r, "Bowler") == 0) return 1;
    return 2;
}

const char *roleStr(int r) {
    if (r == 0) return "Batsman";
    if (r == 1) return "Bowler";
    return "All-rounder";
}

const char *rolePlural(int r) {
    if (r == 0) return "Batsmen";
    if (r == 1) return "Bowlers";
    return "All-rounders";
}

float calcPerf(int r, float avg, float sr, int wkts, float er) {
    if (r == 0) {
        return (avg * sr) / 100.0f;
    } else if (r == 1) {
        return (wkts * 2.0f) + (100.0f - er);
    } else {
        return ((avg * sr) / 100.0f) + (wkts * 2.0f);
    }
}

int findTeamById(int tid) {
    int low = 0, high = totalTeams - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (teamArr[mid].id == tid) return mid;
        if (teamArr[mid].id < tid) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

int findTeamByName(const char *tname) {
    int low = 0, high = totalTeams - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = strcmp(teamArr[mid].name, tname);
        if (cmp == 0) return mid;
        if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

void insertSorted(PNode **head, PNode *node) {
    if (*head == NULL || node->perf > (*head)->perf) {
        node->next = *head;
        *head = node;
        return;
    }
    PNode *cur = *head;
    while (cur->next != NULL && cur->next->perf >= node->perf) {
        cur = cur->next;
    }
    node->next = cur->next;
    cur->next = node;
}

void attachPlayer(int idx, PNode *node) {
    insertSorted(&teamArr[idx].lists[node->role], node);
    teamArr[idx].total++;
    if (node->role == 0 || node->role == 2) {
        teamArr[idx].srSum += node->sr;
        teamArr[idx].batCount++;
        teamArr[idx].avgBatSr = teamArr[idx].srSum / teamArr[idx].batCount;
    }
}

int idExists(int pid) {
    for (int i = 0; i < totalTeams; i++) {
        for (int r = 0; r < 3; r++) {
            PNode *cur = teamArr[i].lists[r];
            while (cur != NULL) {
                if (cur->id == pid) return 1;
                cur = cur->next;
            }
        }
    }
    return 0;
}

void initData() {
    totalTeams = teamCount;
    for (int i = 0; i < totalTeams; i++) {
        teamArr[i].id = i + 1;
        strncpy(teamArr[i].name, teams[i], 50);
        teamArr[i].name[50] = '\0';
        teamArr[i].total = 0;
        teamArr[i].srSum = 0.0f;
        teamArr[i].batCount = 0;
        teamArr[i].avgBatSr = 0.0f;
        teamArr[i].lists[0] = NULL;
        teamArr[i].lists[1] = NULL;
        teamArr[i].lists[2] = NULL;
    }

    for (int i = 0; i < playerCount; i++) {
        int idx = findTeamByName(players[i].team);
        if (idx == -1) continue;

        PNode *p = (PNode *)malloc(sizeof(PNode));
        if (p == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }

        p->id = players[i].id;
        strncpy(p->name, players[i].name, 50);
        p->name[50] = '\0';
        strncpy(p->team, players[i].team, 50);
        p->team[50] = '\0';
        p->role = getRoleCode(players[i].role);
        p->runs = players[i].totalRuns;
        p->avg = players[i].battingAverage;
        p->sr = players[i].strikeRate;
        p->wkts = players[i].wickets;
        p->er = players[i].economyRate;
        p->perf = calcPerf(p->role, p->avg, p->sr, p->wkts, p->er);
        p->next = NULL;

        attachPlayer(idx, p);
    }
}

void addPlayer() {
    int tid;
    printf("Enter Team ID to add player: ");
    if (scanf("%d", &tid) != 1) {
        clearBuf();
        printf("Invalid input.\n");
        return;
    }

    int idx = findTeamById(tid);
    if (idx == -1) {
        printf("Team not found.\n");
        return;
    }

    if (teamArr[idx].total >= 50) {
        printf("Team %s already has maximum 50 players.\n", teamArr[idx].name);
        return;
    }

    int pid, r, runs, wkts;
    float avg, sr, er;
    char name[51];

    printf("Enter Player Details:\n");
    printf("Player ID: ");
    if (scanf("%d", &pid) != 1 || pid < 1) {
        clearBuf();
        printf("Invalid Player ID.\n");
        return;
    }
    if (idExists(pid)) {
        printf("Player ID %d already exists.\n", pid);
        return;
    }

    printf("Name: ");
    scanf(" %50[^\n]", name);

    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (scanf("%d", &r) != 1 || r < 1 || r > 3) {
        clearBuf();
        printf("Invalid role.\n");
        return;
    }

    printf("Total Runs: ");
    if (scanf("%d", &runs) != 1 || runs < 0) {
        clearBuf();
        printf("Invalid runs.\n");
        return;
    }

    printf("Batting Average: ");
    if (scanf("%f", &avg) != 1 || avg < 0) {
        clearBuf();
        printf("Invalid batting average.\n");
        return;
    }

    printf("Strike Rate: ");
    if (scanf("%f", &sr) != 1 || sr < 0) {
        clearBuf();
        printf("Invalid strike rate.\n");
        return;
    }

    printf("Wickets: ");
    if (scanf("%d", &wkts) != 1 || wkts < 0) {
        clearBuf();
        printf("Invalid wickets.\n");
        return;
    }

    printf("Economy Rate: ");
    if (scanf("%f", &er) != 1 || er < 0) {
        clearBuf();
        printf("Invalid economy rate.\n");
        return;
    }

    PNode *p = (PNode *)malloc(sizeof(PNode));
    if (p == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    p->id = pid;
    strcpy(p->name, name);
    strcpy(p->team, teamArr[idx].name);
    p->role = r - 1;
    p->runs = runs;
    p->avg = avg;
    p->sr = sr;
    p->wkts = wkts;
    p->er = er;
    p->perf = calcPerf(p->role, p->avg, p->sr, p->wkts, p->er);
    p->next = NULL;

    attachPlayer(idx, p);
    printf("Player added successfully to Team %s!\n", teamArr[idx].name);
}

void printPlayerRow(PNode *p, int showTeam) {
    if (showTeam) {
        printf("%-5d %-25s %-15s %-13s %-7d %-7.1f %-7.1f %-6d %-6.1f %.2f\n",
               p->id, p->name, p->team, roleStr(p->role),
               p->runs, p->avg, p->sr, p->wkts, p->er, p->perf);
    } else {
        printf("%-5d %-25s %-13s %-7d %-7.1f %-7.1f %-6d %-6.1f %.2f\n",
               p->id, p->name, roleStr(p->role),
               p->runs, p->avg, p->sr, p->wkts, p->er, p->perf);
    }
}

void showTeamPlayers() {
    int tid;
    printf("Enter Team ID: ");
    if (scanf("%d", &tid) != 1) {
        clearBuf();
        printf("Invalid input.\n");
        return;
    }

    int idx = findTeamById(tid);
    if (idx == -1) {
        printf("Team not found.\n");
        return;
    }

    printf("\nPlayers of Team %s:\n", teamArr[idx].name);
    printf("=========================================================================================\n");
    printf("%-5s %-25s %-13s %-7s %-7s %-7s %-6s %-6s %s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("=========================================================================================\n");

    for (int r = 0; r < 3; r++) {
        PNode *cur = teamArr[idx].lists[r];
        while (cur != NULL) {
            printPlayerRow(cur, 0);
            cur = cur->next;
        }
    }

    printf("=========================================================================================\n");
    printf("Total Players: %d\n", teamArr[idx].total);
    printf("Average Batting Strike Rate: %.2f\n", teamArr[idx].avgBatSr);
}

void showTeamsBySr() {
    Team *sorted[20];
    for (int i = 0; i < totalTeams; i++) {
        sorted[i] = &teamArr[i];
    }

    for (int i = 1; i < totalTeams; i++) {
        Team *key = sorted[i];
        int j = i - 1;
        while (j >= 0 && sorted[j]->avgBatSr < key->avgBatSr) {
            sorted[j + 1] = sorted[j];
            j--;
        }
        sorted[j + 1] = key;
    }

    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("%-5s %-20s %-15s %s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("=========================================================\n");
    for (int i = 0; i < totalTeams; i++) {
        printf("%-5d %-20s %-15.2f %d\n",
               sorted[i]->id, sorted[i]->name, sorted[i]->avgBatSr, sorted[i]->total);
    }
    printf("=========================================================\n");
}

void showTopK() {
    int tid, r, k;
    printf("Enter Team ID: ");
    if (scanf("%d", &tid) != 1) {
        clearBuf();
        printf("Invalid input.\n");
        return;
    }

    int idx = findTeamById(tid);
    if (idx == -1) {
        printf("Team not found.\n");
        return;
    }

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (scanf("%d", &r) != 1 || r < 1 || r > 3) {
        clearBuf();
        printf("Invalid role.\n");
        return;
    }

    printf("Enter number of players: ");
    if (scanf("%d", &k) != 1 || k <= 0) {
        clearBuf();
        printf("Invalid count.\n");
        return;
    }

    PNode *cur = teamArr[idx].lists[r - 1];
    if (cur == NULL) {
        printf("No %s found in Team %s.\n", rolePlural(r - 1), teamArr[idx].name);
        return;
    }

    printf("\nTop %d %s of Team %s:\n", k, rolePlural(r - 1), teamArr[idx].name);
    printf("=========================================================================================\n");
    printf("%-5s %-25s %-13s %-7s %-7s %-7s %-6s %-6s %s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("=========================================================================================\n");

    int cnt = 0;
    while (cur != NULL && cnt < k) {
        printPlayerRow(cur, 0);
        cur = cur->next;
        cnt++;
    }
    printf("=========================================================================================\n");
}

void heapUp(PNode *h[], int i) {
    while (i > 0) {
        int par = (i - 1) / 2;
        if (h[par]->perf < h[i]->perf) {
            PNode *tmp = h[par];
            h[par] = h[i];
            h[i] = tmp;
            i = par;
        } else {
            break;
        }
    }
}

void heapDown(PNode *h[], int sz, int i) {
    while (1) {
        int best = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        if (l < sz && h[l]->perf > h[best]->perf) best = l;
        if (r < sz && h[r]->perf > h[best]->perf) best = r;
        if (best != i) {
            PNode *tmp = h[i];
            h[i] = h[best];
            h[best] = tmp;
            i = best;
        } else {
            break;
        }
    }
}

void showAllByRole() {
    int r;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (scanf("%d", &r) != 1 || r < 1 || r > 3) {
        clearBuf();
        printf("Invalid role.\n");
        return;
    }

    int rIdx = r - 1;
    PNode *heap[20];
    int sz = 0;

    for (int i = 0; i < totalTeams; i++) {
        if (teamArr[i].lists[rIdx] != NULL) {
            heap[sz] = teamArr[i].lists[rIdx];
            heapUp(heap, sz);
            sz++;
        }
    }

    if (sz == 0) {
        printf("No players found for this role.\n");
        return;
    }

    printf("\n%s of all teams:\n", rolePlural(rIdx));
    printf("=========================================================================================================\n");
    printf("%-5s %-25s %-15s %-13s %-7s %-7s %-7s %-6s %-6s %s\n",
           "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("=========================================================================================================\n");

    while (sz > 0) {
        PNode *top = heap[0];
        printPlayerRow(top, 1);

        if (top->next != NULL) {
            heap[0] = top->next;
            heapDown(heap, sz, 0);
        } else {
            heap[0] = heap[sz - 1];
            sz--;
            if (sz > 0) heapDown(heap, sz, 0);
        }
    }
    printf("=========================================================================================================\n");
}

void freeAll() {
    for (int i = 0; i < totalTeams; i++) {
        for (int r = 0; r < 3; r++) {
            PNode *cur = teamArr[i].lists[r];
            while (cur != NULL) {
                PNode *nxt = cur->next;
                free(cur);
                cur = nxt;
            }
            teamArr[i].lists[r] = NULL;
        }
    }
}

int main() {
    initData();

    int ch = 0;
    while (1) {
        printf("\n==============================================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("==============================================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printf("==============================================================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &ch) != 1) {
            clearBuf();
            printf("Please enter a valid number.\n");
            continue;
        }

        if (ch == 1) {
            addPlayer();
        } else if (ch == 2) {
            showTeamPlayers();
        } else if (ch == 3) {
            showTeamsBySr();
        } else if (ch == 4) {
            showTopK();
        } else if (ch == 5) {
            showAllByRole();
        } else if (ch == 6) {
            freeAll();
            break;
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}