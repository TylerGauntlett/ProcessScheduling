#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Constants
#define FIRST_COME_FIRST_SERVED 0
#define PREEMPTIVE_SHORTEST_JOB_FIRST 1
#define ROUND_ROBIN 2
#define ERROR -1
#define SUCCESS 1
#define MAX_INPUT_LINE_SIZE 101
#define MAX_PROCESS_DIGIT_NUMBER 10 //A signed integer's max value is ten digits long

//Struct Definition
typedef struct{
    char* procName;
    int* procArrival;
    int* procBurst;
} proc;

//Functions
int getProcessCount(FILE* fpIn);
int getRunTime(FILE* fpIn);
int getProcessAlgorithm(FILE *fpIn);
int getRoundRobinQuantum(FILE *fpIn);
int getProcessesInformation(FILE *fpIn, proc* processes, int procCount);

//Main
int main()
{
    //Variables
    int procCount = ERROR;
    int runTime = ERROR;
    int procAlgo = ERROR;
    int timeQuantum = ERROR;
    int i;
    proc *processes;
    FILE* fpIn = fopen("./processes.in","r");

    if(fpIn == NULL){
        printf("File \"processes.in\" not found in current directory.\n");
        fclose(fpIn);
        return EXIT_FAILURE;
    }
    //Process Count
    procCount = getProcessCount(fpIn);
    if(procCount == ERROR){
        printf("Could not determine number of processes.\n");
        fclose(fpIn);
        return EXIT_FAILURE;
    }
    //Run Time
    runTime = getRunTime(fpIn);
    if(runTime == ERROR){
        printf("Could not determine runtime length.\n");
        fclose(fpIn);
        return EXIT_FAILURE;
    }
    //Assign CPU Algorithm
    procAlgo = getProcessAlgorithm(fpIn);
    if(procAlgo == ERROR){
        printf("Could not determine algorithm (\"fcfs\",\"rr\", or \"sjf\")\n");
        fclose(fpIn);
        return EXIT_FAILURE;
    }
    //Gets quantum (only present for ROUND_ROBIN algorithm)
    if(procAlgo == ROUND_ROBIN){
        timeQuantum = getRoundRobinQuantum(fpIn);
        //Quantum not found
            if(timeQuantum == ERROR){
                printf("Could not determine time quantum for round-robin algorithm.\n");
            fclose(fpIn);
            return EXIT_FAILURE;
            }
    }
    //Assign Processes
    processes = malloc(procCount * sizeof(proc)); //Creates array of of struct values
    //Error Check
    if(processes == NULL){
        printf("Not enough memory is available in heap to store processes.\n");
        fclose(fpIn);
        return EXIT_FAILURE;
    }
    getProcessesInformation(fpIn, processes, procCount);

    //All that follows is for Debugging/Testing
    printf("Process Count: %i\n", procCount);
    printf("Run Time: %i\n", runTime);
    printf("Algorithm  (0 is fcfs)  (1 is sjf)  (2 is rr):  %i\n", procAlgo);

    if(procAlgo == ROUND_ROBIN){
        printf("Time Quantum: %i\n", timeQuantum);
    }

    free(processes);
    fclose(fpIn);
    return 0;
}

/**
 *Returns: Number of processes following the words "processorcount" or ERROR
 */
int getProcessCount(FILE* fpIn){
    char* targetStr = "processcount";
    char* inputLine = malloc(sizeof(char)*MAX_INPUT_LINE_SIZE);
    char result[MAX_PROCESS_DIGIT_NUMBER];
    int procCount = ERROR;
    int i = 0;
    int procCountFound = 0;

    inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
    if(inputLine == NULL){
        free(inputLine);
        return ERROR;
    }
    while( (inputLine != NULL) && (i < MAX_INPUT_LINE_SIZE) && (procCountFound == 0) ){
        //Check if line is a comment
        if(inputLine[i] == '#'){
            //Clear line's contents
            memset(&inputLine[0],0,sizeof(inputLine));
            //Get next line
            inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
            //Next line is empty/EOF
            if(inputLine == NULL){
                free(inputLine);
                return ERROR;
            }
        }
        //Letter found, look for "processcount"
        if(isalpha(inputLine[i])){
            //Compare string (character by character)
            if(inputLine[i]    == targetStr[0]  &&
               inputLine[i+1]  == targetStr[1]  &&
               inputLine[i+2]  == targetStr[2]  &&
               inputLine[i+3]  == targetStr[3]  &&
               inputLine[i+4]  == targetStr[4]  &&
               inputLine[i+5]  == targetStr[5]  &&
               inputLine[i+6]  == targetStr[6]  &&
               inputLine[i+7]  == targetStr[7]  &&
               inputLine[i+8]  == targetStr[8]  &&
               inputLine[i+9]  == targetStr[9]  &&
               inputLine[i+10] == targetStr[10] &&
               inputLine[i+11] == targetStr[11]){
               procCountFound = 1;//Update flag
            }
        }
    i++;
    }
    //Keyword "processcount" found, finds number of processes
    if(procCountFound == 1){
        int resultIdx = 0;
        int maxNumberLength = strlen(inputLine);

        while(i < maxNumberLength){
            //'processcount' string found, but a comment proceeds the value
            if(inputLine[i] == '#'){
                printf("\"%s\" found but actual number of processes not provided on same line!\n", targetStr);
                return ERROR;
            }

            //Read in number
            else if(isdigit(inputLine[i])){
                result[resultIdx] = inputLine[i];
                i++;
                resultIdx++;
                //Continue storing all digits
                while(isdigit(inputLine[i])){
                    result[resultIdx] = inputLine[i];
                    i++;
                    resultIdx++;
                }
                result[resultIdx] = (int)NULL; //Add string terminator
                break;
            }
            else{
            i++;
            }
        }
    }
    //"processcount" not found in line
    else{
    free(inputLine);
    return ERROR;}

    //Convert String 'result' into an integer and return value
    if((sscanf(result, "%i", &procCount) == 1) && (procCount > 0)){
    free(inputLine);
    return procCount;}

    //Error converting occurred
    else{
    printf("Number conversion error.\n");
    free(inputLine);
    return ERROR;
    }
}

/**
 *Returns how long the simulations runs for or ERROR (-1)
 */
int getRunTime(FILE* fpIn){
    char* targetStr = "runfor";
    char* inputLine = malloc(sizeof(char)*MAX_INPUT_LINE_SIZE);
    char result[MAX_PROCESS_DIGIT_NUMBER];
    int runTime = ERROR;
    int i = 0;
    int runTimeFound = 0;

    inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);

    if(inputLine == NULL){
        free(inputLine);
        return ERROR;
    }
    while( (inputLine != NULL) && (i < MAX_INPUT_LINE_SIZE) && (runTimeFound == 0) ){
        //Check if line is a comment
        if(inputLine[i] == '#'){
            //Clear string array
            memset(&inputLine[0],0,sizeof(inputLine));
            //Get next line
            inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
            //Next line is empty/EOF
            if(inputLine == NULL){
                free(inputLine);
                return ERROR;
            }
        }
     //Letter found, look for "runfor"
        if(isalpha(inputLine[i])){
            //Compare string (character by character)
            if(inputLine[i]    == targetStr[0]  &&
               inputLine[i+1]  == targetStr[1]  &&
               inputLine[i+2]  == targetStr[2]  &&
               inputLine[i+3]  == targetStr[3]  &&
               inputLine[i+4]  == targetStr[4]  &&
               inputLine[i+5]  == targetStr[5]){
               runTimeFound = 1;//Update flag
            }
        }
    i++;
    }

    //'runfor' found, determine run time
    if(runTimeFound == 1){
        int resultIdx = 0;
        int maxNumberLength = strlen(inputLine);

        while(i < maxNumberLength){
            //'runfor' string found, but a comment proceeds the value
            if(inputLine[i] == '#'){
                printf("\"%s\" found but its actual value is not provided on same line!\n", targetStr);
                return ERROR;
            }
            //Read in number
            else if(isdigit(inputLine[i])){
                result[resultIdx] = inputLine[i];
                i++;
                resultIdx++;
                //Continue storing all digits
                while(isdigit(inputLine[i])){
                    result[resultIdx] = inputLine[i];
                    i++;
                    resultIdx++;
                }
                result[resultIdx] = (int)NULL; //Add string terminator
                break;
            }
            else{
            i++;
            }
        }
    }
    //"runfor" not found in line
    else{
    free(inputLine);
    return ERROR;}

    //Convert String 'result' into an integer and return value
    if((sscanf(result, "%i", &runTime) == 1) && (runTime > 0)){
    free(inputLine);
    return runTime;}

    //Error converting occurred
    else{
    printf("Number conversion error.\n");
    free(inputLine);
    return ERROR;
    }
}

/**
 *Returns process algorithm or ERROR
 */
int getProcessAlgorithm(FILE *fpIn){
    char* targetStr = "use";
    char* inputLine = malloc(sizeof(char)*MAX_INPUT_LINE_SIZE);
    int i = 0;
    int useFound = 0;

    inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);

    if(inputLine == NULL){
        free(inputLine);
        return ERROR;
    }
    while( (inputLine != NULL) && (i < MAX_INPUT_LINE_SIZE) && (useFound == 0) ){
        //Check if line is a comment
        if(inputLine[i] == '#'){
            //Clear string array
            memset(&inputLine[0],0,sizeof(inputLine));
            //Get next line
            inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
            //Next line is empty/EOF
            if(inputLine == NULL){
                free(inputLine);
                return ERROR;
            }
        }
     //Letter found, look for "use"
        if(isalpha(inputLine[i])){
            //Compare string (character by character)
            if(inputLine[i]    == targetStr[0]  &&
               inputLine[i+1]  == targetStr[1]  &&
               inputLine[i+2]  == targetStr[2]){
               useFound = 1;//Update flag
            }
        }
    i++;
    }
    //'use' found, determine algorithm

    if(useFound == 1){
        int maxNumberLength = strlen(inputLine);
        //Check for comment
        while(i < maxNumberLength){
            //'use' string found, but a comment proceeds the value
            if(inputLine[i] == '#'){
                printf("\"%s\" found but its actual value is not provided on same line!\n", targetStr);
                free(inputLine);
                return ERROR;
            }

            if(isalpha(inputLine[i])){
              //Check for "fcfs" (FIRST COME FIRST SERVED)
              if(inputLine[i]   == 'f' &&
                 inputLine[i+1] == 'c' &&
                 inputLine[i+2] == 'f' &&
                 inputLine[i+3] == 's'){
                    free(inputLine);
                    return FIRST_COME_FIRST_SERVED;
                 }
            //Check for "sjf" (SHORTEST JOB FIRST)
            else if(inputLine[i]   == 's' &&
                    inputLine[i+1] == 'j' &&
                    inputLine[i+2] == 'f'){
                        free(inputLine);
                        return PREEMPTIVE_SHORTEST_JOB_FIRST;
                 }
            //Check for "rr" (ROUND ROBIN)
            else if(inputLine[i]   == 'r' &&
                    inputLine[i+1] == 'r'){
                    free(inputLine);
                    return ROUND_ROBIN;
                 }
                i++;
            }
            else{
            i++;}
        }
    }
    //"use" not found in line
    else{
    free(inputLine);
    return ERROR;}
    //There are strings after "use" keyword but none match the allowed types
    return ERROR;
}

/**
 *Returns number of value of time quantum for ROUND_ROBIN or ERROR
 */
int getRoundRobinQuantum(FILE *fpIn){
    char* targetStr = "quantum";
    char* inputLine = malloc(sizeof(char)*MAX_INPUT_LINE_SIZE);
    char result[MAX_PROCESS_DIGIT_NUMBER];
    int timeQuantum = 0;
    int i = 0;
    int timeQuantumFound = 0;

    inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);

    if(inputLine == NULL){
        free(inputLine);
        return ERROR;
    }
    while( (inputLine != NULL) && (i < MAX_INPUT_LINE_SIZE) && (timeQuantumFound == 0) ){
        //Check if line is a comment
        if(inputLine[i] == '#'){
            //Clear string array
            memset(&inputLine[0],0,sizeof(inputLine));
            //Get next line
            inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
            //Next line is empty/EOF
            if(inputLine == NULL){
                free(inputLine);
                return ERROR;
            }
        }
     //Letter found, look for "quantum"
        if(isalpha(inputLine[i])){
            //Compare string (character by character)
            if(inputLine[i]    == targetStr[0]  &&
               inputLine[i+1]  == targetStr[1]  &&
               inputLine[i+2]  == targetStr[2]  &&
               inputLine[i+3]  == targetStr[3]  &&
               inputLine[i+4]  == targetStr[4]  &&
               inputLine[i+5]  == targetStr[5]  &&
               inputLine[i+6]  == targetStr[6]){
               timeQuantumFound = 1;//Update flag
            }
        }
    i++;
    }
    //'quantum' found, determine quantum
    if(timeQuantumFound == 1){
        int resultIdx = 0;
        int maxNumberLength = strlen(inputLine);

        while(i < maxNumberLength){
            //'quantum' string found, but a comment proceeds the value
            if(inputLine[i] == '#'){
                printf("\"%s\" found but its actual value is not provided on same line!\n", targetStr);
                return ERROR;
            }
            //Read in number
            else if(isdigit(inputLine[i])){
                result[resultIdx] = inputLine[i];
                i++;
                resultIdx++;
                //Continue storing all digits
                while(isdigit(inputLine[i])){
                    result[resultIdx] = inputLine[i];
                    i++;
                    resultIdx++;
                }
                result[resultIdx] = (int)NULL; //Add string terminator
                break;
            }
            else{
            i++;
            }
        }
    }
    //"runfor" not found in line
    else{
        free(inputLine);
        return ERROR;}

    //Convert String 'result' into an integer and return value
    if((sscanf(result, "%i", &timeQuantum) == 1) && (timeQuantum > 0)){
    free(inputLine);
    return timeQuantum;}

    //Error converting occurred
    else{
    printf("Number conversion error.\n");
    free(inputLine);
    return ERROR;
    }
}
/**
 *Sets and malloc's processes array
 *Returns ERROR (-1) if parsing error occurs, otherwise returns SUCCESS (1);
 */
int getProcessesInformation(FILE *fpIN, proc* processes,  int procCount){
    int result = ERROR;
    int i;
    int procNameLength = 6;
    char* name = "tacos";
    //@TODO get values in loop from file
    //Proof of concept code
    /*
    for(i = 0; i < procCount;i++){
        processes[i].procName = (char*)malloc(sizeof(char)*procNameLength+1);
            //Error Check
        if(processes[i].procName == NULL){
            return result;
        }
        processes[i].procName = name;
        processes[i].procArrival = malloc(sizeof(int));
              //Error Check
        if(processes[i].procArrival == NULL){
            return result;
        }
        processes[i].procArrival = 0;
        processes[i].procBurst = malloc(sizeof(int));
              //Error Check
        if(processes[i].procBurst == NULL){
            return result;
        }
        processes[i].procBurst = 10;
       }
     //For debugging
       for(i = 0; i<procCount; i++ ){
        printf("Struct test:\n %s\t\t%i\t\t%i\n", *(processes[i]).procName, *(processes[i]).procArrival, *(processes[i]).procBurst);
       }
    */
result = SUCCESS;
return result;
}




