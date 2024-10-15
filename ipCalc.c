#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <regex.h>
#include <string.h>

// Global variables
char octets[4][4]; // Stores all the ip octets as char
int octetsInt[4]; // Stores all the ip octets as int
char octetsBin[4][9]; // Stores octets value in binary
char cidr[3]; // Stores cidr value as char
int cidrInt; // Stores cidr value as int
char cidrBin[33]; // Stores cidr value in binary
int maskInt[4]; // Stores mask as int
char maskBin[4][9]; // Stores binary value of mask
int networkInt[4]; // Stores network as int
char networkBin[4][9]; // Stores binary value of network
int firstInt[4]; // Stores first ip as int
char firstBin[4][9]; // Stores binary value of first ip
int broadcastInt[4]; // Stores broadcast as int
char broadcastBin[4][9]; // Stores binary value of broadcast
int lastInt[4]; // Stores last ip as int
char lastBin[4][9]; // Stores binary value of last ip



int global_argc;
char **global_argv;

// Colors
const char *RED = "\033[0;31m";
const char *GREEN = "\033[0;32m";
const char *BLUE = "\033[0;34m";
const char *YELLOW = "\033[0;33m";
const char *RESET = "\033[0m";

const char *ERROR;
const char *INFO;

void initializeColors() {
  ERROR = RED;
  INFO = YELLOW;
}

// Simple help pannel
void helpPannel() {
  printf("\n%s[!]%s Invalid input!\n", ERROR, RESET);
  printf("\nUsage:\n\n");
  printf("\t%s%s %s192.168.10.2/24%s\n\n", GREEN, global_argv[0], YELLOW, RESET);
  exit(EXIT_FAILURE);
}

// Convert octets to binary
char *convertToBin(int num) {

  char oct[9];
  char *output = malloc(50);

  for (int i=7; i >= 0; i--) {

    int bit = (num >> i) & 1;
    oct[7-i] = bit ? '1' : '0';

  }
  oct[8] = '\0';

  strcpy(output, oct);

  return output;

}

// Convert octets to decimal
int convertToDec(const char octet[9]) {

  int bit;
  int pos;
  int out;
  int result=0;

  // I = position of octet, reading inverted
  for (int i=7; i >= 0; i--) {
    out=0;
    // This outputs the bit
    bit = octet[i] == '1' ? 1 : 0;
    // if i = 7 -> pos = 0
    pos = 7-i;
    if (pos == 0 && bit == 1) {
      out = bit * 2;
      for (int e=pos; e > 0; e--) {
        out = e == pos ? bit * 2 : out * 2;
      }
      out--;
    } else {
      for (int e=pos; e > 0; e--) {
        out = e == pos ? bit * 2 : out * 2;
      }
    }

    result += out;

  }
  return result;

}

//Get binary and decimal network direction and first ip
void getNetwork() {

  for(int i=0; i < 4; i++) {
    for (int e=0; e < 8; e++) {

      networkBin[i][e] = octetsBin[i][e] == maskBin[i][e] && octetsBin[i][e] == '1' ? '1' : '0';

    }

    networkInt[i] = convertToDec(networkBin[i]);

    if (i != 3) {
      firstInt[i] = networkInt[i];
    } else {
      firstInt[i] = networkInt[i]+1;
    }

    strcpy(firstBin[i], convertToBin(firstInt[i]));

  }

}

//Get binary and decimal network direction and first ip
void getBroadcast() {

  for(int i=0; i < 4; i++) {
    for (int e=0; e < 8; e++) {

      if (octetsBin[i][e] == '1' && maskBin[i][e] == '1') {

        broadcastBin[i][e] = '1';

      } else if (maskBin[i][e] == '0') {

        broadcastBin[i][e] = '1';

      } else if (octetsBin[i][e] == '0' && maskBin[i][e] == '1') {

        broadcastBin[i][e] = '0';

      }

    }

    broadcastInt[i] = convertToDec(broadcastBin[i]);

    if (i != 3) {
      lastInt[i] = broadcastInt[i];
    } else {
      lastInt[i] = broadcastInt[i]-1;
    }

    strcpy(lastBin[i], convertToBin(lastInt[i]));

  }

}

// Returns the network class
char getClass() {

  if (networkBin[0][0] == '0') {
    return 'A';
  } else if (networkBin[0][0] == '1' && networkBin[0][1] == '0') {
    return 'B';
  } else if (networkBin[0][0] == '1' && networkBin[0][1] == '1' && networkBin[0][2] == '0') {
    return 'C';
  } else if (networkBin[0][0] == '1' && networkBin[0][1] == '1' && networkBin[0][2] == '1' && networkBin[0][3] == '0') {
    return 'D';
  } else if (networkBin[0][0] == '1' && networkBin[0][1] == '1' && networkBin[0][2] == '1' && networkBin[0][3] == '1') {
    return 'E';
  }

}

// Returns number of hosts
int getHosts() {

  int restCidr = 32 - cidrInt;
  int hosts = 0; // hosts = 2^cidr
  int out = 2;

  if (cidrInt != 32) {

    for (int i=1; i < restCidr; i++) {
      out = out * 2;
    }

  } else if (cidrInt == 32) {

    out = 1;

  }

  return out;

}

// Calculates the mask
void calcMask() {

  int a=0;
  for (int e=0; e < 4; e++) {

    for (int i=0; i < 8; i++) {

      maskBin[e][i] = a < cidrInt ? '1' : '0';
      a++;

    }

    maskInt[e] = convertToDec(maskBin[e]);

  }

}

//Print results
void printResults() {
  printf("\n%s[+]%s Printing results for %s%s%s\n\n", YELLOW, RESET, YELLOW, global_argv[1], RESET);
  printf("%sDecimal%s\n\n", INFO, RESET);
  printf("IP ->\t\t%i.%i.%i.%i\n", octetsInt[0], octetsInt[1], octetsInt[2], octetsInt[3]);
  printf("Mask ->\t\t%i.%i.%i.%i\n", maskInt[0], maskInt[1], maskInt[2], maskInt[3]);
  printf("CIDR ->\t\t%i\n", cidrInt);
  printf("Class ->\t%c\n", getClass());
  printf("Hosts ->\t%i\n", getHosts());
  printf("Usable hosts ->\t%i\n", getHosts()-2);
  printf("Network ->\t%i.%i.%i.%i\n", networkInt[0], networkInt[1], networkInt[2], networkInt[3]);
  printf("Broadcast ->\t%i.%i.%i.%i\n", broadcastInt[0], broadcastInt[1], broadcastInt[2], broadcastInt[3]);
  printf("First IP ->\t%i.%i.%i.%i\n", firstInt[0], firstInt[1], firstInt[2], firstInt[3]);
  printf("Last IP ->\t%i.%i.%i.%i\n", lastInt[0], lastInt[1], lastInt[2], lastInt[3]);
  printf("\n%sBinary%s\n\n", INFO, RESET);
  printf("IP ->\t\t%s %s %s %s\n", octetsBin[0], octetsBin[1], octetsBin[2], octetsBin[3]);
  printf("Mask ->\t\t%s %s %s %s\n", maskBin[0], maskBin[1], maskBin[2], maskBin[3]);
  printf("Network ->\t%s %s %s %s\n", networkBin[0], networkBin[1], networkBin[2], networkBin[3]);
  printf("Broadcast ->\t%s %s %s %s\n", broadcastBin[0], broadcastBin[1], broadcastBin[2], broadcastBin[3]);
  printf("First IP ->\t%s %s %s %s\n", firstBin[0], firstBin[1], firstBin[2], firstBin[3]);
  printf("Last IP ->\t%s %s %s %s\n", lastBin[0], lastBin[1], lastBin[2], lastBin[3]);
}

// Validates user input
void checkInput(const char *input) {

  regex_t regex;
  int reti;
  regmatch_t matches[6];
  reti = regcomp(&regex, "^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})/([0-9]{1,2})$", REG_EXTENDED);

  // Check if can compile regex
  if (reti) {
    printf("Could not compile regex\n");
    exit(EXIT_FAILURE);
  }

  reti = regexec(&regex, input, 6, matches, 0);

  if (!reti) {
    ;
  }
  else if (reti == REG_NOMATCH) {
    helpPannel();
  }
  else {
    printf("Regex match error!\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < 4; i++) {
    int start = matches[i+1].rm_so;
    int end = matches[i+1].rm_eo;

    snprintf(octets[i], sizeof(octets[i]), "%.*s", end - start, input + start);
    octetsInt[i] = atoi(octets[i]);
    strcpy(octetsBin[i], convertToBin(octetsInt[i]));
  }

  int cidrStart = matches[5].rm_so;
  int cidrEnd = matches[5].rm_eo;

  snprintf(cidr, sizeof(cidr), "%.*s", cidrEnd - cidrStart, input + cidrStart);

  cidrInt = atoi(cidr);

  if (cidrInt > 32 || cidrInt < 0) {

    printf("\n%s[!]%s Invalid cidr: %i\n\n", ERROR, RESET, cidrInt);
    exit(EXIT_FAILURE);

  } else {

    for (int i=0; i < 32; i++) {

      if (i <= cidrInt) {
        cidrBin[i] = '1';
      } else {
        cidrBin[i] = '0';
      }

    }

  }

  regfree(&regex);

}

int main(int argc, char *argv[]) {

  global_argc = argc;
  global_argv = argv;

  initializeColors();

// Check if the user is providing 1 argument
  if (global_argc == 2) {
    const char *input = global_argv[1];
    checkInput(input);
    calcMask();
    getClass();
    getNetwork();
    getBroadcast();
    printResults();
  } else {
    helpPannel();
  }

  return 0;
}
