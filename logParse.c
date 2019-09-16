#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>


//Global option values
char inputFile[50] = "input.dat";
char outputFile[50] = "output.dat";
int duration = 10;

//Global file pointers
FILE * outfilePtr;
FILE * infilePtr;

//Global Child PID for access in durationHandler
pid_t childPID;

void durationHandler(int sig){
	//kill children processes;
	//since parent calls this kill the only child process
	kill(childPID,SIGKILL);
	
	//NOTE this may cause a seg fault do we need this?
	//close and reopen to clean the file of partial data.	
	//fclose(outfilePtr);
	//??????????
	
	outfilePtr = fopen(outputFile, "w");
	fprintf(outfilePtr, "The process timed out after %d second(s)\n",duration);
	fclose(outfilePtr);
	fclose(infilePtr);
	exit(0);
}

void alarmHandler(int sig){
	outfilePtr = fopen(outputFile, "a");
        fprintf(outfilePtr, "%ld: No valid subset found after 1 second",getpid());
	fclose(outfilePtr);
	fclose(infilePtr);
	exit(0);
}

//I got this code from https://lowlevelbits.org/handling-timeouts-in-child-processes/
pid_t pidTimer(int *status){
	pid_t pid = 0;
	while ( (pid == waitpid(WAIT_ANY, status, 0)) == -1) {
		if(errno == EINTR) {
			continue;
		}
		else {
			perror("waitpid");
			exit(1);
		}
	}
	return pid;
}

bool subSetSum(int arr[], int n, int sum, bool print,bool recursionTop){
	//Ending case
	if(sum == 0){
		return true;
	}
	if (n == 0 && sum != 0){
		return false;
	}
	// If last element is greater than sum, then ignore it 
	if (arr[n-1] > sum){
		
		return subSetSum(arr, n-1, sum, print, recursionTop); 
        }
        /* else, check if sum can be obtained by any of the following 
          (a) excluding the last element 
          (b) including the last element   */
	if(subSetSum(arr, n-1, sum, print, recursionTop)){
		
		return true;
	}
	if(subSetSum(arr, n-1, sum-arr[n-1], print, false)){
		if(print){
			//print arr[n-1]
			outfilePtr = fopen(outputFile, "a");
	//		signal(SIGABRT, signalHandler);
			if(recursionTop){
				fprintf(outfilePtr,"%d = %d",arr[n-1],sum);
			}
			else{
				fprintf(outfilePtr,"%d + ",arr[n-1]);
			}
			fclose(outfilePtr);

		}
		return true;
	} 
}


int main(int argc, char *argv[]) {
        //variable for option switch
        int opt;
        //default indentation is set to 4 spaces
        int indentation = 4;
        //to keep track of options and their optargs
        int numOfOptargs = 0;
        //use getopt to iterate through each option.
        while((opt = getopt(argc, argv, "hi:o:t:")) != -1){
        	switch(opt){
        	//print usage and exit
        		case 'h':
				printf("Options:"
					"\n-h --Help option."
					"\n-i --Input file option takes in an argument for the input file name."
					"\n-o --Output file option takes in an argument for the output file name."
					"\n-t --Timeout set option, takes in a number for the timeout duration in seconds.\n");
				exit(1);
			case 'i':
				if(strlen(optarg) > 45 || strlen(optarg) < 1){
					printf("The input file name was too large please use a shorter name.\n");
					return 0;
				}
				strncpy(inputFile,optarg,sizeof(inputFile));
				break;
			case 'o':
				if(strlen(optarg) > 45 || strlen(optarg) < 1){
					printf("The output file name was too long please use a shorter name.\n");
					return 0;
				}
				strncpy(outputFile,optarg,sizeof(outputFile)); 
				break;
			case 't':
				if(atoi(optarg) > 1000){
					printf("The number for duration was too big use a smaller number\n");
					return 0;	
				}
				else if(atoi(optarg) < 0){
					printf("The number for duration was negative and that makes no sense. please use a positive number for duration.\n");
					return 0;
				}
				duration = atoi(optarg);
				break;
			default:
				printf("You gave an option that was not usable. Use -h for help on how to use this program\n");
				exit(1);
		}
	}
	//first set the timer for the whole process.
	alarm(duration);
	signal(SIGALRM,durationHandler);
	infilePtr = fopen(inputFile,"r");
	char * task = NULL;
	size_t len = 0;
	int subtasks = 0;
	if(getline(&task, &len, infilePtr) != -1){
		printf("from file: %s",task);
		subtasks = atoi(task);
		if(subtasks > 10 || subtasks < 0){
			printf("The number of subtasks was not between 1 and 10 inclusive ending program");
			exit(1);
		}
	}
	else{
		perror("ERROR: logParse: Reading first number from file failed: ");
		exit(2);
	}
	
	//open outputfile for writing to clear it.
	outfilePtr = fopen(outputFile,"w");
	fclose(outfilePtr);

	int i = 0;
	int firstNumInTask = -1;
	
	//array to store the pid's of children
	int *pidArray = (int*)malloc(sizeof(int) *subtasks);
	
	for(i = 0; i < subtasks; i++){
		if(getline(&task, &len, infilePtr) != -1){
			//fork timer
		/*	pid_t timerPID = fork();
			if(timerPID == 0){
				sleep(1);
				exit(0);
			}
		*/	//fork here the number of subtask times.
			childPID = fork();
			if(childPID == 0){
				
				alarm(1);
				signal(SIGALRM,alarmHandler);
				//child process
			
				//split the task up and count the elements into i
				int size = 0;
	        		char line[100] = "";
	        		strncpy(line,task,sizeof(line)-1);
	        		line[99] = "\0";
	        		char *ptr = strtok(line, " ");
	        		while(ptr != NULL){
	        		        ptr = strtok(NULL, " ");
	        		        size++;
	        		}
	
				//now that we know the number of elements, do the actual split.
        			char newLine[100] = "";
        			strncpy(newLine,task,sizeof(newLine)-1);
        			newLine[99] = "\0";
				//make the buffer 1 less than the numbers
				//to ignore the first number which is the sum
        			int *numArray = (int*)malloc(sizeof(int) * size - 1);
        			char *newPtr = strtok(newLine, " ");
				//get the sum
				firstNumInTask = atoi(newPtr);
				newPtr = strtok(NULL, " ");
				
        			size = 0;
        			while (newPtr != NULL){
        			        numArray[size] = atoi(newPtr);
                			size++;
               				newPtr = strtok(NULL, " ");
        			}
				
        			if(subSetSum(numArray,size,firstNumInTask,false,false)){
				//	printf("\nThere is a sum!\n");
					outfilePtr = fopen(outputFile, "a");	
					fprintf(outfilePtr, "%ld: ",getpid());
					fclose(outfilePtr);
	        	               // printf("%ld: ",getpid());
					subSetSum(numArray,size,firstNumInTask,true,true);
					                       	
        			        //return selectedNums;
        			        //find the sum by sending combinations into isZeroSum once its true continue. else break
       				}
        			else {
                	                outfilePtr = fopen(outputFile, "a");	
                			fprintf(outfilePtr, "%ld: No subset of numbers summed to %d",getpid(),firstNumInTask);
					fclose(outfilePtr);
					
        			}
				
				free(numArray);
				exit(0);
			}
			else{ 
			//parent process
			//this function will either kill the process after timeout or will just wait on it.
			int status = 0;
			status = wait(NULL);
		/*	pid_t firstCompleted = pidTimer(&status);
			//printf("%s",firstCompleted);
			if(firstCompleted == timerPID){
				//kill child
				kill(childPID, SIGABRT);
				//print message to file
				printf("killing child\n");
				outfilePtr = fopen(outputFile, "a");
				fprintf(outfilePtr, "%ld: No valid subset found after 1 second",childPID);
				fclose(outfilePtr);
			}
			else if( firstCompleted == childPID){
				printf("killed timer\n");
				kill(timerPID, SIGKILL);
			}
		*/
			pidArray[i] = childPID;
			
			}
		
		}
		else{
		perror("ERROR: logParse: Expected a line but did not find one in file. ");
		exit(0);
		}
		//end the line in the file
                outfilePtr = fopen(outputFile, "a");
                fprintf(outfilePtr, "\n");
                fclose(outfilePtr);

	}
	
        outfilePtr = fopen(outputFile, "a");
	int k = 0;
	for(k = 0; k < subtasks; k++){
		fprintf(outfilePtr, "Child PID: %ld\n",pidArray[k]);
	}
        fprintf(outfilePtr, "Parent PID: %ld\n",getpid());
        fclose(outfilePtr);
	fclose(infilePtr);
	
	return 0;
}
