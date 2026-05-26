#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_Q 200
#define QUIZ_COUNT 10

struct Question
{
    int number;
    char question[200];
    char options[4][100];
};

typedef struct {
    char username[30];
    int score;
    float percentage;
} Result;

// Functions
void userLogin();
void adminLogin();
void test();
int checkCredentials(char Username[], char Password[]);
int checkAdmincredentials(char Username[], char Password[]);
void adminMenu();
void addMcq();
void addUser();
void saveResult(Result r);
void searchResult();
int loadQuestions(struct Question q[], int max);
int loadAnswers(int answers[], int max);
int getQuestion(struct Question q[], int index);
float applyNegativeMarking(int correct, int total);
float calculatePercentage(float score, int total);
int countAnswers();

// ============================================================
int main()
{
    int choice = 0;
    do {
        printf("\n========================================\n");
        printf("        ONLINE MCQ TEST SYSTEM\n");
        printf("========================================\n");
        printf("1. User Log In\n2. Admin Log In\n3. Exit\nEnter Choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1:
                userLogin();
                break;
            case 2:
                adminLogin();
                break;
            case 3:
                printf("\n========================================\n");
                printf("     THANK YOU FOR USING THE SYSTEM\n");
                printf("========================================\n");
                printf("             SEE YOU SOON \n");
                printf("========================================\n");
                break;
            default:
                printf("Invalid choice, Try again\n");
                break;
        }
    } while(choice != 3);

    printf("\n----------------------------------------\n");
    printf("     PROGRAM TERMINATED SUCCESSFULLY\n");
    printf("----------------------------------------\n");
    return 0;
}

// ============================================================
void userLogin()
{
    printf("\n----------------------------------------\n");
    printf("              USER LOGIN\n");
    printf("----------------------------------------\n");
    int attempts = 0;
    while(attempts < 3) {
        int status;
        char username[50];
        char password[50];
        printf("Enter Username: ");
        scanf("%s", username);
        printf("Enter Password: ");
        scanf("%s", password);
        status = checkCredentials(username, password);
        if (status == 1) {
            printf("\n========================================\n");
            printf("            LOGIN SUCCESSFUL\n");
            printf("========================================\n");
            printf("\n========================================\n");
            printf("            TEST STARTED\n");
            printf("========================================\n");
            test();
            return;
        } else if (status == 0) {
            printf("Wrong Username or Password, Try again\n");
        }
        attempts++;
    }
    printf("Too many tries, Access Denied\n");
    return;
}

// ============================================================
int checkCredentials(char Username[], char Password[])
{
    char user[50];
    char pass[50];
    FILE *p;
    p = fopen("users.txt", "r");
    if (p == NULL) {
        printf("ERROR, FILE NOT FOUND\n");
        return -1;
    }
    while(fscanf(p, "%s %s", user, pass) != EOF) {
        if (strcmp(user, Username) == 0 && strcmp(pass, Password) == 0) {
            fclose(p);
            return 1;
        }
    }
    fclose(p);
    return 0;
}

// ============================================================
void adminLogin()
{
    printf("\n----------------------------------------\n");
    printf("             ADMIN LOGIN\n");
    printf("----------------------------------------\n");
    int attempts = 0;
    while(attempts < 3) {
        int status;
        char username[50];
        char password[50];
        printf("Enter Username: ");
        scanf("%s", username);
        printf("Enter Password: ");
        scanf("%s", password);
        status = checkAdmincredentials(username, password);
        if (status == 1) {
            printf("\n========================================\n");
            printf("             LOGIN SUCCESSFUL\n");
            printf("========================================\n");
            adminMenu();
            return;
        } else if (status == 0) {
            printf("Wrong Username or Password, Try again\n");
        }
        attempts++;
    }
    printf("Too many tries, Access Denied\n");
    return;
}

// ============================================================
int checkAdmincredentials(char Username[], char Password[])
{
    char user[50];
    char pass[50];
    FILE *p;
    p = fopen("admin.txt", "r");
    if (p == NULL) {
        printf("ERROR, FILE NOT FOUND\n");
        return -1;
    }
    while(fscanf(p, "%s %s", user, pass) != EOF) {
        if (strcmp(user, Username) == 0 && strcmp(pass, Password) == 0) {
            fclose(p);
            return 1;
        }
    }
    fclose(p);
    return 0;
}

// ============================================================
// Reads answers.txt — 1 answer per line, 1-indexed (line 1 = Q1)
int loadAnswers(int answers[], int max)
{
    FILE *fp = fopen("answers.txt", "r");
    if (fp == NULL) {
        printf("Error: answers.txt not found.\n");
        return 0;
    }
    int count = 1; // 1-indexed to match question numbers
    while (count <= max && fscanf(fp, "%d", &answers[count]) == 1) {
        count++;
    }
    fclose(fp);
    return count - 1; // returns total answers loaded
}

// ============================================================
// Returns total number of answers currently in answers.txt
int countAnswers()
{
    FILE *fp = fopen("answers.txt", "r");
    if (fp == NULL) return 0;
    int count = 0, val;
    while (fscanf(fp, "%d", &val) == 1)
        count++;
    fclose(fp);
    return count;
}

// ============================================================
void test()
{
    struct Question q[MAX_Q];
    int userAnswers[QUIZ_COUNT];
    int selected[QUIZ_COUNT];
    int correctAnswers[MAX_Q + 1]; // loaded from answers.txt, 1-indexed

    int totalQuestions, i, j;
    int correctCount;
    float score, percentage;
    Result r;

    printf("Enter your username for result record: ");
    scanf("%s", r.username);

    totalQuestions = loadQuestions(q, MAX_Q);
    int totalLoaded = loadAnswers(correctAnswers, MAX_Q);

    if (totalLoaded == 0) {
        printf("Could not load answers. Test aborted.\n");
        return;
    }
    if (totalQuestions < QUIZ_COUNT) {
        printf("Not enough questions in file.\n");
        return;
    }

    srand(time(NULL));

    // Pick UNIQUE random question indices
    for (i = 0; i < QUIZ_COUNT; i++) {
        int randIndex, duplicate;
        do {
            duplicate = 0;
            randIndex = rand() % totalQuestions;
            for (j = 0; j < i; j++) {
                if (selected[j] == randIndex) {
                    duplicate = 1;
                    break;
                }
            }
        } while (duplicate);
        selected[i] = randIndex;
    }

    // Display questions with proper serial number (1, 2, 3... not file number)
    for (i = 0; i < QUIZ_COUNT; i++) {
        printf("\nQuestion %d of %d:\n", i + 1, QUIZ_COUNT);
        userAnswers[i] = getQuestion(q, selected[i]);
    }

    // Compare answers on the spot using file-loaded correctAnswers[]
    correctCount = 0;
    for (i = 0; i < QUIZ_COUNT; i++) {
        int qNumber = q[selected[i]].number; // actual question number from file
        if (userAnswers[i] == correctAnswers[qNumber])
            correctCount++;
    }

    score = applyNegativeMarking(correctCount, QUIZ_COUNT);
    percentage = calculatePercentage(score, QUIZ_COUNT);

    r.score = correctCount;
    r.percentage = percentage;
    saveResult(r);

    printf("\n========================================\n");
    printf("             RESULT SUMMARY\n");
    printf("========================================\n");
    printf("Total Questions : %d\n", QUIZ_COUNT);
    printf("Correct Answers : %d\n", correctCount);
    printf("Final Score     : %.2f\n", score);
    printf("Percentage      : %.2f%%\n", percentage);
    printf("========================================\n");
}

// ============================================================
void adminMenu()
{
    int choice = 0;
    while(choice != 4) {
        printf("\n----------------------------------------\n");
        printf("             ADMIN CONTROLS\n");
        printf("----------------------------------------\n");
        printf("1. Add MCQ\n2. Add user\n3. See results\n4. Admin Log out\nEnter Choice: ");
        scanf("%d", &choice);
        switch(choice) {
            case 1:
                addMcq();
                break;
            case 2:
                addUser();
                break;
            case 3:
                searchResult();
                break;
            case 4:
                return;
            default:
                printf("Invalid choice, Try again\n");
                break;
        }
    }
    return;
}

// ============================================================
void addMcq()
{
    printf("\n----------------------------------------\n");
    printf("              ADD NEW MCQ\n");
    printf("----------------------------------------\n");

    int currentCount = countAnswers();
    if (currentCount >= MAX_Q) {
        printf("Maximum question limit reached!\n");
        return;
    }

    char ques[200];
    char op1[100], op2[100], op3[100], op4[100];

    // Next question number = current answer count + 1
    // This keeps question numbers in sync with answer line positions
    int nextQ = currentCount + 1;

    printf("Enter the question: ");
    scanf(" %[^\n]", ques);
    printf("Enter the first option: ");
    scanf(" %[^\n]", op1);
    printf("Enter the second option: ");
    scanf(" %[^\n]", op2);
    printf("Enter the third option: ");
    scanf(" %[^\n]", op3);
    printf("Enter the fourth option: ");
    scanf(" %[^\n]", op4);

    char ans = '0';
    while (ans < '1' || ans > '4') {
        printf("Enter the correct option (1-4): ");
        scanf(" %c", &ans);
    }

    // Write question to questions.txt
    FILE *p = fopen("questions.txt", "a");
    if (p == NULL) { printf("Error opening questions.txt\n"); return; }
    fprintf(p, "%d %s\n", nextQ, ques);
    fprintf(p, "1.) %s\n", op1);
    fprintf(p, "2.) %s\n", op2);
    fprintf(p, "3.) %s\n", op3);
    fprintf(p, "4.) %s\n", op4);
    fclose(p);

    // Write answer to answers.txt
    FILE *fa = fopen("answers.txt", "a");
    if (fa == NULL) { printf("Error opening answers.txt\n"); return; }
    fprintf(fa, "%d\n", ans - '0');
    fclose(fa);

    printf("\n========================================\n");
    printf("          MCQ ADDED SUCCESSFULLY\n");
    printf("========================================\n");
}

// ============================================================
void addUser()
{
    printf("\n----------------------------------------\n");
    printf("              ADD NEW USER\n");
    printf("----------------------------------------\n");
    char username[50];
    char password[50];
    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    // Check for duplicate username before adding
    char u[50], pw[50];
    FILE *check = fopen("users.txt", "r");
    if (check != NULL) {
        while (fscanf(check, "%s %s", u, pw) != EOF) {
            if (strcmp(u, username) == 0) {
                printf("Username already exists! Choose a different one.\n");
                fclose(check);
                return;
            }
        }
        fclose(check);
    }

    FILE *p = fopen("users.txt", "a");
    if (p == NULL) {
        printf("FILE NOT FOUND");
    } else {
        fprintf(p, "%s %s\n", username, password);
        printf("\n========================================\n");
        printf("        USER CREATED SUCCESSFULLY\n");
        printf("========================================\n");
        fclose(p);
    }
    return;
}

// ============================================================
void saveResult(Result r)
{
    FILE *fp = fopen("results.txt", "a");
    if (fp == NULL) {
        printf("results.txt not found");
        return;
    }
    fprintf(fp, "%s %d %.2f\n", r.username, r.score, r.percentage);
    fclose(fp);
}

// ============================================================
void searchResult()
{
    int found = 0;
    Result r;
    char searchName[30];
    printf("Enter the username to search results for: ");
    scanf("%s", searchName);

    FILE *fp = fopen("results.txt", "r");
    if (fp == NULL) {
        printf("No results file found!\n");
        return;
    }

    printf("\n----------------------------------------\n");
    printf("       RESULTS FOR: %s\n", searchName);
    printf("----------------------------------------\n");

    while (fscanf(fp, "%s %d %f", r.username, &r.score, &r.percentage) == 3) {
        if (strcmp(r.username, searchName) == 0) {
            printf("Score: %d/10 | Percentage: %.2f%%\n", r.score, r.percentage);
            found = 1;
        }
    }
    if (!found) {
        printf("No results found for this username.\n");
    }
    printf("----------------------------------------\n");
    fclose(fp);
}

// ============================================================
int loadQuestions(struct Question q[], int max)
{
    FILE *fp;
    int count = 0;

    fp = fopen("questions.txt", "r");
    if (fp == NULL) {
        printf("Error opening questions file.\n");
        return 0;
    }

    while (count < max && fgets(q[count].question, 200, fp)) {

        // Add this check after fgets() reads the question line:
        if (strlen(q[count].question) < 3) break; // skip blank lines
        // Parse the leading question number from the line
        int i = 0, num = 0;
        while(q[count].question[i] >= '0' && q[count].question[i] <= '9') {
            num = num * 10 + (q[count].question[i] - '0');
            i++;
        }
        q[count].number = num;

        fgets(q[count].options[0], 100, fp);
        fgets(q[count].options[1], 100, fp);
        fgets(q[count].options[2], 100, fp);
        fgets(q[count].options[3], 100, fp);
        count++;
    }

    fclose(fp);
    return count;
}

// ============================================================
int getQuestion(struct Question q[], int index)
{
    int userAnswer;

    // Skip the leading number in the question text so it doesn't print twice
    char *qText = q[index].question;
    while (*qText >= '0' && *qText <= '9') qText++; // skip digits
    if (*qText == ' ') qText++;                       // skip space after number

    printf("%s", qText);
    printf("%s", q[index].options[0]);
    printf("%s", q[index].options[1]);
    printf("%s", q[index].options[2]);
    printf("%s", q[index].options[3]);

    do {
        printf("Enter your answer (1-4): ");
        scanf("%d", &userAnswer);
    } while (userAnswer < 1 || userAnswer > 4);

    return userAnswer;
}

// ============================================================
float applyNegativeMarking(int correct, int total)
{
    int wrong = total - correct;
    return (correct * 1.0f) - (wrong * 0.25f);
}

float calculatePercentage(float score, int total)
{
    return (score / total) * 100;
}
