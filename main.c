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
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <limits.h>

#define BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\r\n\a"
#define MAN_DIR "./man/"
#define PATH_MAX 4096

// Biến toàn cục để lưu thư mục chứa chương trình
char home_directory[PATH_MAX];

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
int hthsh_man(char **args); 

/*
  Danh sách các lệnh được xây dựng
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "runsh",
  "lsdir",
  "time",
  "runapp",
  "man"
};

int (*builtin_func[]) (char **) = {
  &hthsh_cd,
  &hthsh_help,
  &hthsh_exit,
  &hthsh_runsh,
  &hthsh_lsdir,
  &hthsh_showtime,
  &hthsh_runapp,
  &hthsh_man,
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
    fprintf(stderr, "🤧 Cần tham số cho lệnh \"cd\"\n");
  } else if (strcmp(args[1], "~") == 0) {
    if (chdir(home_directory) != 0) {
      perror("🙈");
    }
  } else {
    if (chdir(args[1]) != 0) {
      perror("🙈");
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
  printf("Goodbye 🙋 See you soon 💕 \n");
  return 0;
}

int hthsh_runsh(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "🤧 Cần tham số cho lệnh \"runsh\"\n");
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
      printf("%s  ", dir->d_name);
    }
    closedir(d);
    printf("\n");
  }
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

int hthsh_showtime(char **args)
{
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    printf ("⏱️ Thời gian hiện tại là: %s", asctime(timeinfo));

    return 1;
}

int hthsh_runapp(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "🤧 Cần tham số cho lệnh \"runapp\"\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Tiến trình con
        execlp(args[1], args[1], (char *)NULL);
        perror("hthsh");
        return 1;
    } else if (pid < 0) {
        // Lỗi khi tạo tiến trình con
        perror("hthsh");
        return 1;
    } else {
        // Tiến trình cha
        int status;
        waitpid(pid, &status, 0); // Chờ tiến trình con kết thúc
        printf("Ứng dụng %s đã kết thúc 👻\n", args[1]);
    }
    return 1;
}

int hthsh_man(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "🤧 Cần tham số cho lệnh \"man\"\n");
        return 1;
    }

    char filepath[BUFSIZE];
    snprintf(filepath, sizeof(filepath), "%s%s.txt", MAN_DIR, args[1]);

    FILE *file = fopen(filepath, "r");
    if (!file) {
        fprintf(stderr, "🙈 Không tìm thấy trang hướng dẫn cho lệnh \"%s\"\n", args[1]);
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1) {
        printf("%s", line);
    }

    free(line);
    fclose(file);
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
    fprintf(stderr, "hthsh: lỗi cấp phát\n");
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
        fprintf(stderr, "hthsh:  lỗi cấp phát\n");
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
    printf("(👉ﾟヮﾟ)👉 ");
    line = hthsh_read_line();
    args = hthsh_split_line(line);
    status = hthsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

void print_welcome_message() {
    printf("*******************************************************************************\n");
    printf("  @file         main.c\n");
    printf("  @author       Pham Quang Hung, Nguyen Duc Trieu, Tran Vuong Hoang\n");
    printf("  @date         March 2024\n");
    printf("  @brief        HTHSH (HoangTrieuHung SHell)\n");
    printf("*******************************************************************************\n");
    printf("\n");
    printf("🚀 Welcome to HTHSH (HoangTrieuHung SHell)! 🚀\n\n");
}

int main(int argc, char** argv) {
  if (getcwd(home_directory, sizeof(home_directory)) == NULL) {
    perror("getcwd() error");
    return 1;
  }
  print_welcome_message();
  hthsh_loop();
  return EXIT_SUCCESS;
}

