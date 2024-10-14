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
int convertToDec(const char octet[9]){

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
  printf("\n[+] Printing results for %s\n\n", global_argv[1]);
  printf("Decimal\n\n");
  printf("IP ->\t%s.%s.%s.%s\n", octets[0], octets[1], octets[2], octets[3]);
  printf("Mask ->\t%i.%i.%i.%i\n", maskInt[0], maskInt[1], maskInt[2], maskInt[3]);
  printf("CIDR ->\t%s\n", cidr);
  printf("\nBinary\n\n");
  printf("IP ->\t%s %s %s %s\n", octetsBin[0], octetsBin[1], octetsBin[2], octetsBin[3]);
  printf("Mask ->\t%s %s %s %s\n", maskBin[0], maskBin[1], maskBin[2], maskBin[3]);
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
    printResults();
  } else {
    helpPannel();
  }

  return 0;
}
