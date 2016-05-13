#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <my_global.h>
#include <mysql.h>

#define distance_C 5;
/* pre-measured value, c */

int kbhit();
void displayMap(int a, int b, int c);
void finish_with_error(MYSQL *con);

int main() {
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "mysql_init() failed\n");
    exit(1);
  }

  if (mysql_real_connect(con, "host", "user", "pw", "db", "port", "socket", "flag") == NULL) {
    finish_with_error(con);
  }

  while (1) {
    if (mysql_query(con, "SELECT * FROM Signals")) {
      finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    int distance_A, distance_B;
    MYSQL_ROW row = mysql_fetch_row();
    distance_A = row[1];
    row = mysql_fetch_row();
    distance_B = row[1];

    displayMap(distance_A, distance_B, distance_C);

    if (kbhit()) break;
  }

  mysql_close(con);
  exit(0);
}

int kbhit() {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void displayMap(int a, int b, int c) {
  printf("%d, %d, %d", a, b, c);
  return;
}
