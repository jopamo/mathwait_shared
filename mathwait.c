/*
#
#
# Paul Moses
# CMP_SCI-2750-002
# Project 5: Fork/Wait-Shared memory
#
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

void showOptions() {
  printf("\nThis executable mathwait takes the following optional command line options:\n");
  printf("\t-h : Output a help message indicating what types of inputs it expects.\n\n");
  printf("Usage example: ./mathwait OUTFILE 32 9 10 -13\n");
}

int main(int argc, char *argv[]) {
  int num = 2;

  FILE *filePointer = NULL;

  key_t key = ftok("mathwait.c", 'b');

  // 19 is the spec
  int sum = 19;

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

  //test
  int shmid = shmget(key, num, 0644|IPC_CREAT);
  int *arr = shmat(shmid, NULL, 0);
  arr[0] = -2;
  arr[1] = -2;

  // overwrite file if exists
  filePointer = fopen(argv[1], "w");

  if (filePointer == NULL) {
    printf("No file specified.\n");
    showOptions();

    // shmctl with IPC_RMID marks shared segment for destruction after detachment
    shmctl(shmid, IPC_RMID, NULL);

    // detach
    shmdt(&shmid);

    exit(1);
  }

  // fork the child process
  pid = fork();

  if (pid < 0) {
    fprintf(stderr, "Error forking");

    // shmctl with IPC_RMID marks shared segment for destruction after detachment
    shmctl(shmid, IPC_RMID, NULL);

    // detach
    shmdt(&shmid);

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

      // shmctl with IPC_RMID marks shared segment for destruction after detachment
      shmctl(shmid, IPC_RMID, NULL);

      // detach
      shmdt(&shmid);

      exit(1);
    }
    else {
      // i is the first position and j is the one after
      // check all the j's for each i if they add up to 'sum'
      for (int i = 0; i < numQuant; i++) {
        for (int j = i + 1; j < numQuant; j++) {
          int z = numArray[i] + numArray[j];

          if (z == sum) {
            arr[0] = numArray[i];
  			arr[1] = numArray[j];
            free(freeCopy);

            // detaches as well
            exit(0);
          }
        }
      }
    }

    // free the heap
    free(freeCopy);

    // none found
    arr[0] = -1;
  	arr[1] = -1;

    exit(1);
  }
  // parent
  else if (pid > 0) {
    int status = 0;

    // Waits for child to end
    waitpid(pid, &status, 0);

    if (filePointer == NULL) {
      fprintf(stderr, "Error on input. Try again.\n");

      // shmctl with IPC_RMID marks shared segment for destruction after detachment
      shmctl(shmid, IPC_RMID, NULL);

      // detach
      shmdt(&shmid);

      exit(1);
    }
    else {
      if (arr[0] == -1 && arr[1] == -1) {
        fprintf(filePointer, "No pair was found.\n");
      }
      else if (arr[0] == -2 && arr[1] == -2) {
        fprintf(filePointer, "Child did not do anything to it and so some error occurred.\n");

        // close the file
        fclose(filePointer);

        // shmctl with IPC_RMID marks shared segment for destruction after detachment
        shmctl(shmid, IPC_RMID, NULL);

        // detach
        shmdt(&shmid);

        exit(1);
      }
      else {
        fprintf(filePointer, "Pair found by child: %d %d\n", arr[0], arr[1]);
      }

      // close the file
      fclose(filePointer);

      // shmctl with IPC_RMID marks shared segment for destruction after detachment
      shmctl(shmid, IPC_RMID, NULL);

      // detach
      shmdt(&shmid);

      exit(0);
    }
  }
}
