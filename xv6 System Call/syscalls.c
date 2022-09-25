#include "types.h"
#include "user.h"

int main(int argc, char * argv[]){
   
    int pid = getpid();    

    if (argc != 3){
    	printf(1, "incorrect number of arguments! Must be 2\n");
	exit();
    }

    int N = atoi(argv[1]);    // required num of system calls
    int g = atoi(argv[2]);    // required num of good system calls

    
    if ((N < 1) || (g < 1)) {
    	printf(1, "incorrect number of arguments! Must be 2\n");
	exit();
    }

    for (int i = 0; i < g - 1; i++) {
        pid = getpid();
    }

    for (int i = 0; i < N - g; i++) {
        kill(-1);
    }
    
    int goodCall = getnumsyscallsgood(pid);
    if (goodCall != g) {
        printf(1, "Wrong number of good calls. Returned %d, but it should be %d\n",goodCall, g);
    }

    int totalCall = getnumsyscalls(pid);
    if (totalCall != N) {
        printf(1, "Wrong number of total calls. Returned %d, but it should be %d\n", totalCall, N);
    }

    printf(1, "%d %d\n", totalCall, goodCall);
   // printf(1, "Num of completed syscalls:  %d, \t Succesful syscalls:  %d\n", getnumsyscalls(pid) - completedCount, getnumsyscallsgood(pid) - successfulCount);


    //printf(1, "Current num of completed %d and succesful %d system calls\n", completedCount, successfulCount);
    exit();
}
