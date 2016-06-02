////////////////////////////////////////////////////
// Usage: Configure one Edison as an access point //
// and connect. Run sigTest to obtain RSSI        //
// Used primarily for data collection.            //
////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  // close and ret urn
  pclose(fp);
  return signal;
}

int main() {
  while (1) {
    printf("%d\n", getSignalLevel());
  }
}
