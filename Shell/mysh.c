#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX 512

// int execv(const char *path, char *const argv[]);

// My helper function that executes the arguments provided
void executeCommand(char* arg_cmd[])
{
    // Forking a child
    int rc = fork();
    int status; // status of the child process

    //for (int i = 0; i < 5; i++) {
    	//printf("arg_cmd: %s\n", arg_cmd[i]);
	//fflush(stdout);
    // }
    // printf("Called the fork function with %s, returned with rc = %d\n", arg_cmd[0], rc);
    // fflush(stdout);

    if (arg_cmd[0] == NULL) {
        return;
    }
    if (rc == -1) {
	// printf("Error could not fork\n");
        // fflush(stderr);
        _exit(1);
    } else if (rc == 0) {
	// printf("Calling execv with path:%s %s %s\n",arg_cmd[0],arg_cmd[1],arg_cmd[2]);    
        // fflush(stdout);
	int e_rc = execv(arg_cmd[0], arg_cmd);
	// printf("e_rc is: %d", e_rc);
        // fflush(stdout);
	if (e_rc == -1) {
            char command_message[MAX];
            snprintf(command_message,MAX,"%s: Command not found.\n", arg_cmd[0]);
            write(STDERR_FILENO, command_message, strlen(command_message));
	    // printf("The return code is: %d", e_rc);
	    // fflush(stderr);
	    // If in here that means execv returned error
	    _exit(0);
	}
	// _exit(-1);
	// printf("Fork returned with rc is 0\n");
	// fflush(stdout);
	_exit(0);
    } else {
	// printf("Coming to wait ...\n");
        // fflush(stdout);	
        waitpid(rc,&status,0);
	return;
    }
}

// My helper function that executes the arguments provided when there is redirect
void executeCommand_withRedirect(char* arg_cmd[], char* filename)
{ 
   // printf("Execute command with redirect. Command: '%s', output file:'%s'\n",arg_cmd[0],filename);
   // printf("arg_cmd[1] is: %s\n", arg_cmd[1]);
   // fflush(stdout);
   // forking a child
   int rc = fork();
   // int status;
   
   if (rc == -1) {
       _exit(1);
   }
   if (rc == 0) { // child process
       int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
       dup2(fd, STDOUT_FILENO);
       close(fd);
       int e_rc = execv(arg_cmd[0], arg_cmd);

       // if cannot execute
       if (e_rc) {
           char execute_fault[MAX];
           snprintf(execute_fault,MAX,"Cannot write to file %s.\n", filename);
           write(STDERR_FILENO, execute_fault, strlen(execute_fault));

       }
   } else if (rc > 0) {
       // this is parent process
       (void) wait(NULL);
   }

}


// Removes the trailing white spaces from the input string
// Returns 1 if there are other spaces in the string (in addition to the trailing ones
// Otherwise, returns 0
int Clean_Whitespace(char str[]) {

  // Remover the leading zeros
  int first_nonspace_ind = 0;
  char nextChar = str[first_nonspace_ind];

  // Remove the leading spaces
  // printf("Begin: '%s'\n",str);
  // fflush(stdout);
  while ( (nextChar == ' ') || (nextChar == '\t') || (nextChar == '\n') ) {
    first_nonspace_ind++;
    nextChar = str[first_nonspace_ind];
  }

  // At this point the nextChar is the first non-white space real character
  // Continue until finding another white space
  int next_ind = first_nonspace_ind;
  while ( (nextChar != ' ') && (nextChar != '\t') && (nextChar != '\n') ) {
      next_ind++;
      nextChar = str[next_ind];
  }
  int last_nonspace_ind = next_ind - 1;   // Record the position of end of the input

  // printf("First nonspace ind: %d the last: %d\n",first_nonspace_ind,last_nonspace_ind);
  // fflush(stdout);
  // At this point next_ind is after the non-space char; i.e., the first space after the proper input
  // If there is another non-space character left, this means the input had two words. So, return 1
  while (nextChar != '\0') {
    next_ind++;
    nextChar = str[next_ind];
    // printf("NEXT char '%c'\n",nextChar);
    // fflush(stdout);
    if ( (nextChar != '\0') && (nextChar != ' ') && (nextChar != '\t') && (nextChar != '\n') )
        return 1;
  }

  // If we didn't return 1, then the trailing characters are spaces
  if (first_nonspace_ind > 0) { // Shift the str to left
    nextChar = str[first_nonspace_ind];
    int i = first_nonspace_ind;
    while (i <= last_nonspace_ind) {
      str[i-first_nonspace_ind] = nextChar;
      i++;
      nextChar = str[i];
    }
    str[i-1] = '\0';
  } 
  else
     str[last_nonspace_ind+1] = '\0';

  // printf("After removing '%s'\n",str);
  // fflush(stdout);
  return 0;
}

int main(int argc, char* argv[]){

    char invalidArg_message[50] = "Usage: mysh [batch-file]\n";
    // char commandPrompt [25] = "mysh> ";
    char redirection_error[50] = "Redirection misformatted.\n";
    char filename[512];
    char buffer[MAX];
    // char redirection[MAX];
    char* inputCommands[MAX];

    // check if there are invalid number of command line arguments
    if (argc > 2) {
        write(STDERR_FILENO, invalidArg_message, strlen(invalidArg_message));
        exit(1);	
    }

    // check if we run in interactive or batch mode
    if (argc == 1) {
        // there was only one argumnet provided
	// should run in interactive  mode
        // printf("In interactive mode!\n");
	// fflush(stdout);

        while (1) {
            // write out to stanard out
            // write(stdout, commandPrompt, strlen(commandPrompt));
            printf("mysh> ");
	    fflush(stdout);
            
	    // read the command from stdin
	    fgets(buffer, MAX, stdin);

	    // check if end of file from stdin
	    if (feof(stdin) != 0) {
	        exit(0);
	    }
            // after reading from buffer, echo's to stdout
	    //write(1,buffer,strlen(buffer));
	   
	    // checks if > is included in buffer
	    char* checkRedirection = strchr(buffer, '>');
	    char *p; 
	    char *inputCommandStr = strtok(buffer,">");
	    
	    // Parse the input commands
            char* token = strtok(inputCommandStr, " ");
            // printf("The token is: %s", token);
            if ((strncmp(token, "exit\n", MAX)) == 0) {
                // printf("Received exit command\n");
                // return -1;
		// write(1,token,strlen(token));
                exit(0);
            }
            // printf("The token is: %s",token);
            // fflush(stdout);
            // printf(".\n");
	    // fflush(stdout);
            int i = 0;
            while (token != NULL) {
                token[strcspn(token, "\n")] = '\0';
                // printf("The updated token is: %s\n", token);
                // fflush(stdout);
                inputCommands[i] = token;
                // printf("InputCommands is: %s\n", inputCommands[i]);
                token = strtok(NULL, " ");
                i++;
            }
            // now call the function
            // add null to end of inputCommands
            inputCommands[i] = NULL;

            if (checkRedirection == NULL) { // There is no redirection
	        executeCommand(inputCommands);
	    }
            else {
                // Need to redirect. First check the syntax and find the filename  
		// printf("Found p: '%s'\n", checkRedirection);
		// fflush(stdout);
		p = checkRedirection+1;
		// printf("Found p + 1: '%s'\n", p);
	    	if ( (p == NULL) || (strncmp(p,"\n",MAX) == 0) ) {
                    write(STDERR_FILENO, redirection_error, strlen(redirection_error));
                    continue;
		    //exit(0);
                } 
		else {
                   // printf("Before clean white space: '%s'\n",p);
                   // fflush(stdout);
                   if ((strchr(p,'>') != NULL) || (Clean_Whitespace(p) == 1) ) {
                       // there is one more redirection sign || there is more than one word
		       write(STDERR_FILENO, redirection_error, strlen(redirection_error));
                       continue;
		       // exit(0);
                   }
		} // else

		// Clean_Whitespace(p);
                // printf("After clean up, the filename: '%s', command:'%s'\n",p,inputCommands[0]);
		// fflush(stdout);
		executeCommand_withRedirect(inputCommands,p);

	    } // else for if redirect

	
	} // while (1)

    } else if (argc == 2) {
        // there was two arguments provided
	// should run in batch mode
	// printf("In batch mode!\n");
	// fflush(stdout);
	FILE *fp;
	char *p;
        // copy file provided into filename
	snprintf(filename, MAX, "%s", argv[1]);
        fp = fopen(filename, "r");
	// printf("The filename is: %s\n", filename);
        // if fp == NULL then file cannot be opened, return error
	if (fp == NULL) {
	    char error_message[MAX];
	    /// use strcat and strcpy 
	    snprintf(error_message,MAX,"Error: Cannot open file %s.\n", argv[1]);
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
	}
        
        char buffer[MAX];
	char* batchCommands[MAX];
	// read each line from file, only reads first 512
	while (fgets(buffer, MAX, fp) != NULL) {
	    // int i = 0;	
            // after reading from buffer, echo's to stdout
	    write(1,buffer,strlen(buffer));
	    // checks if > is included in buffer
            char* checkRedirection = strchr(buffer, '>');
	    char* inputCommandStr = strtok(buffer, ">");

	    batchCommands[0] = strtok(inputCommandStr, " \t\r\n\v\f");
	    int j = 0;
	    while (batchCommands[j] != NULL) {
	        j++;
		batchCommands[j] = strtok(NULL, " \t\r\n\v\f");
	    }

	    if (batchCommands[0] == NULL) {
	        // printf("BatchCommands[0] was null!!");
                continue;
	    }

	    if (strcmp(batchCommands[0], "exit") == 0) {
	        // write(1,"exit",strlen(buffer));
	        exit(0);
	    }
	    batchCommands[j] = NULL;
	    
	    if (checkRedirection == NULL) {
	        executeCommand(batchCommands);
	    } 
	    else {
	        // Need to redirect. First check the syntax and find the filename  
                // printf("Found p: '%s'\n", checkRedirection);
                // fflush(stdout);
                p = checkRedirection+1;
		// printf("Found p + 1: '%s'\n", p);
                // fflush(stdout);
                if ( (p == NULL) || (strncmp(p,"\n",MAX) == 0) ) {
		    write(STDERR_FILENO, redirection_error, strlen(redirection_error));
                    continue;
		    // exit(0);
                }
                else {
                   // printf("Before clean white space: '%s'\n",p);
                   // fflush(stdout);
		   int numWordsAfterRedirect = Clean_Whitespace(p);
                   if ((strchr(p,'>') != NULL) || (numWordsAfterRedirect == 1) ) {
                       // there is one more redirection sign || there is more than one word
                       write(STDERR_FILENO, redirection_error, strlen(redirection_error));
                       continue;
		       // exit(0);
		   }
		}
		// printf("After clean up, the filename: '%s', command:'%s'\n",p,batchCommands[0]);
                // fflush(stdout);
		executeCommand_withRedirect(batchCommands,p);
	    }
	}
	return 0;
    }
}
