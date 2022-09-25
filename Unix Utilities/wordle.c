// "Copyright 2022 <Copyright Oz Ogras>"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX 256
int main(int argc, char** argv) {
    char filename[MAX];
    char searchString[MAX];

    // If the program is passed invalid number of arguments
    if (argc != 3) {
        printf("wordle: invalid number of args\n");
        exit(1);
    }
    // filename now includes whatever was file name was provided
    snprintf(filename, MAX, "%s", argv[1]);
    // strcpy(filename, argv[1]);
    // searchString now includes argument provided
    snprintf(searchString, MAX, "%s", argv[2]);
    // strcpy(searchString, argv[2]);
    // printf("The searchString is: %s\n", searchString);
    // open the file
    FILE *fp;
    fp = fopen(filename, "r");

    // if file is empty or cannot open exit with error
    if (fp == NULL) {
        printf("wordle: cannot open file\n");
        exit(1);
    }

    // char *token = strtok(argv[2]);
    char buffer[MAX];
    char *match;
    while (fgets(buffer, MAX, fp) != NULL) {
        // printf("The contents of buffer are: %s\n", buffer);
        int len = strlen(buffer);
        // printf("The length of: %s is %d", buffer, len);
        // check if buffer is greater than len 5. If so continue to next word
        if (len != 6) {        // strlen counts null termination so > 6
            continue;
        }
        // check if searchString has a match in buffer
        match = strpbrk(buffer, searchString);
        // printf("%s", match);
        // if match == NULL then there was no match
        if (match == NULL) {
            printf("%s", buffer);
        }
    }
    fclose(fp);
    // return 0 if there were no errors
    return 0;
}
