#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

//Global option values
char inputFile[50] = "input.dat";
char outputFile[50] = "output.dat";
int duration = 10;



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
			printf("%d ", arr[n-1]);
			FILE * out;
			out = fopen(outputFile, "a");
			if(recursionTop){
				fprintf(out,"%d = %d",arr[n-1],sum);
			}
			else{
				fprintf(out,"%d + ",arr[n-1]);
			}
			fclose(out);

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
		
	FILE *infilePtr;
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
	//fork the number of subtasks times
	subtasks = 3;
	
	//open outputfile for writing to clear it.
	FILE *outfilePtr;
	outfilePtr = fopen(outputFile,"w");
	fclose(outfilePtr);

	int i = 0;
	int firstNumInTask = -1;
	for(i = 0; i < subtasks; i++){
		if(getline(&task, &len, infilePtr) != -1){
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
				FILE * outfilePtr;
				outfilePtr = fopen(outputFile, "a");	
				fprintf(outfilePtr, "%ld: ",getpid());
				fclose(outfilePtr);
	                       // printf("%ld: ",getpid());
				subSetSum(numArray,size,firstNumInTask,true,true);
				                       	
        		        //return selectedNums;
        		        //find the sum by sending combinations into isZeroSum once its true continue. else break
        /*		        int k = 0;
				int m = 0;
				for(k = 0; k < i; k++){
					int sum = 0;
					//k is starting point
					for(m = k; m < i; m++){
						if(k == m){
							continue;
						}
						sum += numArray[m];
						
					}
				} 
				*/
       			}
        		else {
				FILE * outfilePtr;
                                outfilePtr = fopen(outputFile, "a");
				
                		fprintf(outfilePtr, "%ld: No subset of numbers summed to %d",getpid(),firstNumInTask);
				fclose(outfilePtr);
				
        		}
			
			//fprintf(outfilePtr, "%s",line);
			//int *numArray = getSumSubtasks(line);
			/*int j = 0;
			for(j = 0; j < size; j++){
				fprintf(outfilePtr, "%d ",numArray[j]);	
			}*/
			free(numArray);
		}
		FILE * outfilePtr;
                outfilePtr = fopen(outputFile, "a");
		fprintf(outfilePtr, "\n");
		printf("\n");
		fclose(outfilePtr);
	}
	fclose(infilePtr);
	
	return 0;
}
