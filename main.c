/////////////////////////////////////////////////////////////////
// IMPORTANT: CHANGE IP AND DISTANCE_C BEFORE USE IF NECESSARY //
/////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mysql.h>
#include <math.h>

//pre-measured value, c, in meters
#define distance_C 50
//dimensions of displayed map (currently square)
#define dim 50

int kbhit();
double sigToDist(int sig);
void displayMap(double a, double b, double c);
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
    if (mysql_query(con, "SELECT * FROM RSSI")) {
      finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    int sig_A, sig_B;
    MYSQL_ROW row = mysql_fetch_row(result);
    sig_A = atoi(row[1]);
    row = mysql_fetch_row(result);
    sig_B = atoi(row[1]);

    system("clear");
    displayMap(sigToDist(sig_A), sigToDist(sig_B), distance_C);

    // press any key to break
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

double sigToDist(int sig) {
  // Distance as a function of signal (see Excel sheet)
  // Distance(Signal)=.0308e^(.082*Signal)
  return .0308*exp(.082*(double)sig);
}

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void displayMap(double a, double b, double c) {
  double theta;
  int x, y, i, j;

  //printf("A: %f, B: %f, C: %f\n", a, b, c);

  //Law of Cosines
  theta = acos((pow(b,2)-pow(a,2)-pow(c,2))/(-2*a*c));
  x = round(a*cos(theta));
  y = round(a*sin(theta));

  for (i=0; i<dim; i++) {
    for (j=0; j<dim; j++) {
      if (j == x && i == abs(y - dim - 1)) printf("* ");
      else printf("- ");
    }
    printf("\n");
  }
  printf("Current position: x = %d, y = %d",x,y);

  return;
}
