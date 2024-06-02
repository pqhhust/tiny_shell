#include <windows.h>
#include <stdio.h>
#include <shellapi.h>
#include <time.h>

#define BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"

/*
  Khai báo các lệnh
 */
int hthsh_cd(char **args);
int hthsh_help(char **args);
int hthsh_exit(char **args);
int hthsh_runsh(char **args);
int hthsh_lsdir(char **args);
int hthsh_showtime(char **args);
int hthsh_runapp(char **args);

/*
  Danh sách các lệnh được xây dựng
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "runbat",
  "lsdir",
  "time",
  "runapp"
};

int (*builtin_func[]) (char **) = {
  &hthsh_cd,
  &hthsh_help,
  &hthsh_exit,
  &hthsh_runsh,
  &hthsh_lsdir,
  &hthsh_showtime,
  &hthsh_runapp,
};

int hthsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Cài đặt cho các lệnh
*/
int hthsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "hthsh: Cần tham số cho lệnh \"cd\"\n");
  } else {
    if (!SetCurrentDirectory(args[1])) {
      fprintf(stderr, "hthsh: Error changing directory\n");
    }
  }
  return 1;
}

int hthsh_help(char **args)
{
  int i;
  printf("HTHSH\n");
  printf("Gõ lệnh và các tham số, sau đó nhấn enter.\n");
  printf("Các lệnh được xây dựng: \n");

  for (i = 0; i < hthsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Sử dụng lệnh 'man' để biết cách sử dụng các câu lệnh\n");
  return 1;
}

int hthsh_exit(char **args)
{
  return 0;
}

int hthsh_runsh(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "hthsh: Cần tham số cho lệnh \"runsh\"\n");
    return 1;
  }

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Start the child process. 
  if (!CreateProcess(NULL,   // No module name (use command line)
    args[1],        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    0,              // No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi)           // Pointer to PROCESS_INFORMATION structure
    ) 
  {
    printf("CreateProcess failed (%d).\n", GetLastError());
    return 1;
  }

  // Wait until child process exits.
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles. 
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return 1;
}

int hthsh_lsdir(char **args)
{
  WIN32_FIND_DATA findFileData;
  HANDLE hFind;

  hFind = FindFirstFile("*", &findFileData);
  if (hFind == INVALID_HANDLE_VALUE) {
    printf("FindFirstFile failed (%d)\n", GetLastError());
    return 1;
  } 
  else {
    do {
      if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
        continue;
      printf("%s\n", findFileData.cFileName);
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
  }
  return 1;
}

int hthsh_showtime(char **args)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    printf("Thời gian hiện tại là: %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);
    return 1;
}

int hthsh_runapp(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "hthsh: Cần tham số cho lệnh \"runapp\"\n");
        return 1;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process. 
    if (!CreateProcess(NULL,   // No module name (use command line)
        args[1],        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        ) 
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    printf("\nỨng dụng %s đã kết thúc\n", args[1]);

    return 1;
}

int hthsh_launch(char **args)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Prepare command line argument
  char cmdLine[MAX_PATH] = "";
  for (int i = 0; args[i] != NULL; i++) {
    strcat(cmdLine, args[i]);
    strcat(cmdLine, " ");
  }

  // Start the child process. 
  if (!CreateProcess(NULL,   // No module name (use command line)
    cmdLine,        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    0,              // No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory 
    &si,            // Pointer to STARTUPINFO structure
    &pi)           // Pointer to PROCESS_INFORMATION structure
    ) 
  {
    printf("CreateProcess failed (%d).\n", GetLastError());
    return 1;
  }

  // Wait until child process exits.
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles. 
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return 1;
}

char* hthsh_read_line() {
  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD nRead;
  char* buffer = malloc(BUFSIZE);

  if (!buffer) {
    fprintf(stderr, "hthsh: lỗi cấp phát\n");
    exit(EXIT_FAILURE);
  }

  if (!ReadConsole(hStdin, buffer, BUFSIZE, &nRead, NULL)) {
    fprintf(stderr, "hthsh: lỗi đọc dòng\n");
    exit(EXIT_FAILURE);
  }

  buffer[nRead - 2] = '\0'; // Replace \r\n with \0
  return buffer;
}

char **hthsh_split_line(char *line)
{
    int bufsize = TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "hthsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "hthsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int hthsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < hthsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  // If no built-in command was found, try to run it as a Windows command
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  char cmdLine[MAX_PATH];
  sprintf(cmdLine, "cmd.exe /C %s", args[0]);

  if (!CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
    printf("CreateProcess failed (%d).\n", GetLastError());
    return 1;
  }

  // Wait until child process exits.
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles. 
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return 1;
}

void hthsh_loop() {
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = hthsh_read_line();
    args = hthsh_split_line(line);
    status = hthsh_execute(args);

    free(line);
    free(args);
  } while (status);
}


int main(int argc, char** argv) {
    SetConsoleOutputCP(CP_UTF8);
    hthsh_loop();
    return EXIT_SUCCESS;
}