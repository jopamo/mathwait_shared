/*
#
#
# Paul Moses
# CMP_SCI-2750-002
# Project 4: Fork/Wait
#
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void showOptions() {
  printf("\nThis executable mathwait takes the following optional command line options:\n");
  printf("\t-h : Output a help message indicating what types of inputs it expects.\n\n");
  printf("Usage example: ./mathwait INPUTFILE 32 9 10 -13\n");
}

int main(int argc, char *argv[]) {
  FILE *filePointer = NULL;

  // 19 is the spec
  int sum = 19;

  int pairFlag = 0;

  // it will start with 2 when run with only a filename and no numbers
  int numQuant = argc - 2;

  // pid_t data type represents process IDs
  pid_t pid;

  // make sure to catch this first before we go looking for args
  if (argc < 2) {
    fprintf(stderr, "No file specified. Try again.\n");
    showOptions();
    exit(1);
  }

  // getopt causes problems with negatives anyways
  if (strcmp(argv[1], "-h") == 0) {
    showOptions();
    exit(0);
  }

  // overwrite file if exists
  filePointer = fopen(argv[1], "w");

  if (filePointer == NULL) {
      printf("No file specified.\n");
      showOptions();
      exit(1);
  }

  // fork the child process
  pid = fork();

  if (pid < 0) {
    fprintf(stderr, "Error forking");
    exit(1);
  }
  // child process
  else if (pid == 0) {
    // dynamic array using malloc
    int *numArray = malloc(numQuant * sizeof *numArray);

    // keep track of original pointer
    int* freeCopy = numArray;

    // store the numbers in the array
    // args are all strings, strtol converts to int and detects errors
    for (int i = 0; i < numQuant; i++) {
      numArray[i] = strtol(argv[i + 2], NULL, 10);
    }

    if (filePointer == NULL) {
      fprintf(stderr, "Error on input. Try again.\n");
      exit(1);
    }
    else {
      // Child: 171891: 32 9 10 -13
      fprintf(filePointer, "Child: %d:", getpid());

      for (int i = 0; i < numQuant; i++) {
        fprintf(filePointer, " %d", numArray[i]);
      }
      fprintf(filePointer, "\n");

      // display the pairs
      fprintf(filePointer, "Child: %d:", getpid());

      // i is the first position and j is the one after
      // check all the j's for each i if they add up to 'sum'
      for (int i = 0; i < numQuant; i++) {
        for (int j = i + 1; j < numQuant; j++) {
          int z = numArray[i] + numArray[j];

          if (z == sum) {
            fprintf(filePointer, " Pair: %d %d", numArray[i], numArray[j]);
            pairFlag = 1;
          }
        }
      }
      fprintf(filePointer, "\n");
    }

    // free the heap
    free(freeCopy);

    // check if any pairs were found
    if (pairFlag == 1) {
      exit(0);
    }
    else {
      exit(1);
    }
  }
  // parent
  else if (pid > 0) {
    int status;
    char* charStatus = NULL;

    // Waits for child to end
    waitpid(pid, &status, 0);

    // convert int to expected string
    if (WEXITSTATUS(status) == 0) {
      charStatus = "EXIT_SUCCESS";
    }
    else if (WEXITSTATUS(status) == 1) {
      charStatus = "EXIT_FAILURE";
    }

    if (filePointer == NULL) {
      fprintf(stderr, "Error on input. Try again.\n");
      exit(1);
    }
    else {
      // Parent: 172699: EXIT_SUCCESS
      fprintf(filePointer, "Parent: %d: %s\n", getpid(), charStatus);

      // close the file
      fclose(filePointer);
    }
  }
}
