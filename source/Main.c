#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>

void execute_cd(char *path) {
    if (strcmp(path, "..") == 0) {
        SetCurrentDirectory("..");
    } else if (SetCurrentDirectory(path) == 0) {
        printf("Error: Cannot change directory to %s\n", path);
    }
}

void execute_ls() {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*", &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error: Unable to list directory.\n");
        return;
    } 
    do {
        printf("%s\n", findFileData.cFileName);
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
}

void execute_program(char *program, char **args) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD exitCode;
    clock_t start, end;
    double time_spent;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    memset(&pi, 0, sizeof(pi));

    char cmd[1024] = "";
    strcat(cmd, program);
    for (int i = 1; args[i] != NULL; i++) {
        strcat(cmd, " ");
        strcat(cmd, args[i]);
    }

    start = clock();
    if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        printf("Error: Failed to start program %s\n", program);
        return;
    }
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    end = clock();

    time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution time: %.3f seconds\n", time_spent);

    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void print_prompt() {
    char cwd[1024];
    if (GetCurrentDirectory(sizeof(cwd), cwd)) {
        printf("%s$ ", cwd);
    } else {
        printf("$ ");
    }
}

int main() {
    char input[1024];
    char *args[64];
    char *token;

    while (1) {
        print_prompt();
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        int i = 0;
        token = strtok(input, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (args[0] == NULL) continue;

        if (strcmp(args[0], "exit") == 0) {
            break;
        } else if (strcmp(args[0], "cd") == 0) {
            if (args[1] != NULL) {
                execute_cd(args[1]);
            } else {
                printf("Usage: cd <directory>\n");
            }
        } else if (strcmp(args[0], "ls") == 0) {
            execute_ls();
        } else if (args[0][0] == '.' && args[0][1] == '/') {
            execute_program(args[0], args);
        } else {
            printf("Command not recognized.\n");
        }
    }

    return 0;
}
