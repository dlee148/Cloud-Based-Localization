#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <mysql.h>

#include <stdlib.h>

int kbhit();
int getSignalLevel();
void finish_with_error(MYSQL *con);

int main() {
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "mysql_init() failed\n");
    exit(1);
  }

  if (mysql_real_connect(con, "192.168.42.1", "root", "password", "CBL", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  while (1) {
    char query[100];
    char sigStr[100];

    sprintf(query, "%s", "UPDATE RSSI SET SigStrength=");
    sprintf(sigStr, "%d", getSignalLevel());
    strcat(query, sigStr);
    strcat(query, " WHERE NodeID='A'");
    /* Change to 'B' if appropriate */

    if (mysql_query(con, query)) {
      finish_with_error(con);
    }

    printf("Successful update: %s.\n", sigStr);

    if (kbhit()) break;
    sleep(1);
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

int getSignalLevel() {
  // get info from iwconfig
  FILE* fp;
  char path[1035];
  int signal = 0;
  fp = popen("iwconfig 2>/dev/null | grep \"Signal level\"", "r");
  fgets(path, sizeof(path) - 1, fp);

  // extract signal level
  if (path[strlen(path) - 9] <= 57 && path[strlen(path) - 9] >= 48) {
    signal += 10 * (path[strlen(path) - 9] - '0');
  }
  if (path[strlen(path) - 8] <= 57 && path[strlen(path) - 8] >= 48) {
    signal += path[strlen(path) - 8] - '0';
  }

  // close and return
  pclose(fp);
  return signal;
}

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}
