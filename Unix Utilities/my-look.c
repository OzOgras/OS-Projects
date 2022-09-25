// "Copyright 2022 <Copyright Oz Ogras>"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX 256

void isAlnum(char string[]) {
    int i;
    int len = strlen(string);        // take the length of token
    for (i = 0; i < len; i++) {
        if (string[i] == 10)
            continue;                // skip the newline
        // check if character is not alpanumeric using ASCII values
        if ((string[i] < 65 && string[i] > 57) || (string[i] < 48) ||
                    (string[i] > 90 && string[i] < 97) || (string[i] > 122)) {
        // printf("The char to be removed is: %c", string[i]);
            for (int j = i; j < len; j++) {
                string[j] = string[j+1];
            }
            len--;
            i--;
        }
    }
}

int main(int argc, char** argv) {
    char filename[MAX];
    char searchString[MAX];
    char original[MAX];
    // char inputString[MAX];
    int option;
    // use getopt() to read the command line
    // accepts V, h, f. They each accept an argument because of the :
    while ((option = getopt(argc, argv, "Vhf::")) != -1) {
        switch (option) {
            case 'V':
                printf("my-look from CS537 Summer 2022\n");
                exit(0);
            case 'h':
                printf("-V : prints information about this utility\n");
                printf("-h: prints help information about this utility\n");
                printf("-f <filename>: uses filename as the input dictionary");
                printf(" If this option is not specified, then my-look\n");
                printf("will read from standard input\n");
                exit(0);
            case 'f':
            // if f then it should copy that file into filename.
                snprintf(filename, MAX, "%s", argv[2]);
                // strcpy(filename, argv[2]);
                snprintf(searchString, MAX, "%s", argv[3]);
                // strcpy(searchString, argv[3]);
                break;
            case '?':
            // encountered incorrect argument
                printf("my-look: invalid command line\n");
                exit(1);
        }
    }

    // Parsed the inputs. Next read the input file: $filename)
    // printf("The filename is: %s\n", filename);
    // printf("searchString is: %s\n", searchString);

    // read file. If -f was written then filename should not be NULL.
    FILE *fp;
    // put in if statement to see if -f was provided.
    // printf("Argc =: %d\n", argc);
    if (argc == 4) {
        fp = fopen(filename, "r");
    } else if (argc == 2) {
        fp = stdin;        // set fp to stdin if -f wasn't provided
        snprintf(searchString, MAX, "%s", argv[1]);
    } else {
        printf("my-look: invalid command line\n");
        exit(1);
    }

    if (fp == NULL) {
    // print error message and exit with 1
        printf("my-look: cannot open file\n");
        exit(1);
    }
    char buffer[MAX];
    while (fgets(buffer, MAX, fp) != NULL) {
        snprintf(original, MAX, "%s", buffer);
        isAlnum(buffer);
        int match = strncasecmp(searchString, buffer, strlen(searchString));
        // if strncasecmp returns 0 then it is a match
        // printf("%s, The match is: %d", buffer, match);
        if (match == 0) {
            printf("%s", original);
        }
    }
    // close the file we opened
    fclose(fp);
    // return 0 if there were no error cases
    return 0;
}


