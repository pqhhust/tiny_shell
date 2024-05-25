/***************************************************************************//**

  @file         main.c

  @author       Pham Quang Hung, Nguyen Duc Trieu, Tran Vuong Hoang

  @date         March 2024

  @brief        HTHSH (HoangTrieuHung SHell)

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>

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
int hthsh_list(char **args);

/*
  Danh sách các lệnh được xây dựng
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "runsh",
  "lsdir",
  "list"
};

int (*builtin_func[]) (char **) = {
  &hthsh_cd,
  &hthsh_help,
  &hthsh_exit,
  &hthsh_runsh,
  &hthsh_lsdir,
  &hthsh_list
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
    printf("%s", args[1]);
    if (chdir(args[1]) != 0) {
      perror("hthsh");
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

  printf("Sử dụng lênh 'man' để biết cách sử dụng các câu lệnh\n");
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

  pid_t pid = fork();
  if (pid == 0) {
    if (chmod(args[1], 0755) < 0) {
      perror("hthsh: chmod error");
      exit(EXIT_FAILURE);
    }
    if (execl("/bin/sh", "/bin/sh", args[1], (char *)NULL) == -1) {
      perror("hthsh");
      exit(EXIT_FAILURE);
    }
  } else if (pid < 0) {
    perror("hthsh");
  } else {
    int status;
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

int hthsh_lsdir(char **args)
{
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        continue;
      printf("%s\n", dir->d_name);
    }
    closedir(d);
  }
  return 1;
}

int hthsh_list(char **args) {
    // Gọi trực tiếp ps
    system("ps");

    return 1;
}

int hthsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Tiến trình con
    if (execvp(args[0], args) == -1) {
      perror("hthsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Lỗi fork
    perror("hthsh");
  } else {
    // Tiến trình cha
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

char* hthsh_read_line() {
  int bufsize = BUFSIZE;
  int position = 0;
  char* buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "hthsh: lỗi cấp phát\n");
    exit(EXIT_FAILURE);
  }

  while(1) {
    c = getchar();
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;
    if (position >= bufsize) {
      bufsize += BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "hthsh: lỗi cấp phát\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

char** hthsh_split_line(char* line) {
  int bufsize = TOK_BUFSIZE, position = 0;
  char** tokens = malloc(bufsize * sizeof(char*));
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
    // Trường hợp không nhập lệnh nào.
    return 1;
  }

  for (i = 0; i < hthsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return hthsh_launch(args);
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
  hthsh_loop();
  return EXIT_SUCCESS;
}

