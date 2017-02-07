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
#define MAX_INPUT_LINE_SIZE 256
#define MAX_PROCESS_DIGIT_NUMBER 10 //A signed integer's max value is ten digits long
#define MAX_PROCESS_COUNT 128

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
void printProcessesArray(proc* processes, int procCount);

//Main
int main()
{
    //Variables
    int procCount = ERROR;
    int runTime = ERROR;
    int procAlgo = ERROR;
    int timeQuantum = ERROR;
    int i;
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
    proc* processes = (proc*)malloc(procCount * sizeof(proc));
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
    printProcessesArray(processes,procCount);

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
            memset(inputLine,0,sizeof(inputLine));
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
    //Note: If number has 10 digits but is greater than MAX_INTEGER the MAX_INTEGER's value is returned and no error is thrown
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
int getProcessesInformation(FILE *fpIn, proc* processes,  int procCount){
    //Variables
    int storedProcNum = 0;
    int i = 0;
    char** arrayOfProcNames;
    char* currentProcName;
    int currentProcNameSize;
    int currentProcArrival;
    int currentProcBurst;
    int lineLength = 0;
    int processStrFound = 0;
    int nameStrFound = 0;

    //Initialize Rows in 2D Name Array
    arrayOfProcNames = (char**) malloc(sizeof(char*) * procCount);

    //Error Checking
    char* inputLine = malloc(sizeof(char)*(int)MAX_INPUT_LINE_SIZE);
    if(inputLine == NULL || arrayOfProcNames == NULL){
        printf("Not enough memory in heap to store process list.\n");
        free(arrayOfProcNames);
        return ERROR;
    }

    //Function Logic
    inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
    lineLength = strlen(inputLine);

    /* Stores all process information (Name, Arrival Time, and Burst Length)
     * in processes (a proc struct array)
     */
    for(storedProcNum = 0; storedProcNum < procCount; storedProcNum++){
        /*
         *Searches for the keywords 'process' and 'name' appearing
         *consecutively (with any number of spaces/comments between them
         */
        while(nameStrFound == 0 && processStrFound == 0 && i < lineLength){
        //printf("Line: %s\n", inputLine);
        //Comment Line or empty line found
            if(inputLine[0] == '#' || (inputLine[0] == '\n' && inputLine[1] == '\0')){
            //Clear input line array
            memset(inputLine,0,sizeof(inputLine));
            //Get next line
            inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
            lineLength = strlen(inputLine);
            i = 0;
            //Next line is empty/EOF
                if(inputLine == NULL){
                    //Free individual processes' data
                    int j;
                    for(j = 0; j < storedProcNum; j++){
                        free(&processes[j]);
                    }
                    free(arrayOfProcNames);
                    free(inputLine);
                    return ERROR;
                }
            }
            //Letter found
            else if(isalpha(inputLine[i])){
                //'process' Check
                char* targetString = "process";
                if( inputLine[i] == targetString[0] &&
                    inputLine[i+1] == targetString[1] &&
                    inputLine[i+2] == targetString[2] &&
                    inputLine[i+3] == targetString[3] &&
                    inputLine[i+4] == targetString[4] &&
                    inputLine[i+5] == targetString[5] &&
                    inputLine[i+6] == targetString[6]){
                        //'name' search begins
                        i += strlen(targetString); //update index
                        processStrFound = 1; //update flag
                        while(nameStrFound == 0){
                            //Comment check
                            if(inputLine[i] == '#' || (inputLine[0] == '\n' && inputLine[1] == '\0')){
                            //Clear input line array
                            memset(inputLine,0,sizeof(inputLine));
                            //Get next line
                            inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
                            //Next line is empty/EOF
                            if(inputLine == NULL){
                                //Free individual processes' data
                                int j;
                                for(j = 0; j < storedProcNum; j++){
                                    free(&processes[j]);
                                }
                                free(arrayOfProcNames);
                                free(inputLine);
                                return ERROR;
                            }
                        i = 0;
                        lineLength = strlen(inputLine);
                        }
                        //'name' Check
                        else if(isalpha(inputLine[i])){
                            char* nameStr = "name";
                                if( inputLine[i]   == nameStr[0] &&
                                    inputLine[i+1] == nameStr[1] &&
                                    inputLine[i+2] == nameStr[2] &&
                                    inputLine[i+3] == nameStr[3]){
                                        nameStrFound = 1; //Updates flag
                                        i += strlen(nameStr);
                                }
                        }
                        //Space found after word 'process'
                        else{
                            i++;
                        }
                    }
                }
            }
            //Space in current array index
            else{
            i++;
            }
        }
    //Error Check: End of line that contained (uncommented) strings but no process declaration was reached
    if(i == lineLength){
        int j;
        for(j = 0; j < storedProcNum; j++){
            free(&processes[j]);
        }
        free(arrayOfProcNames);
        free(inputLine);
        return ERROR;
    }
    //Move line index past spaces
    while(isspace(inputLine[i])){
        i++;
    }
    //Store Process Size
    currentProcNameSize = 0;
    int procNameStartIndex = i;
    while(isalnum(inputLine[i])){
        currentProcNameSize++;
        i++;
    }
    //Store Process Name (O(n + n) = O(2n) = O(n), right?)
    currentProcName = malloc(sizeof(char)* currentProcNameSize + 1);
    arrayOfProcNames[storedProcNum] = malloc(sizeof(char)* currentProcNameSize + 1);
    int j;
    for(j = procNameStartIndex; j < i; j++){
        currentProcName[j - procNameStartIndex] = inputLine[j];
    }
    //Create permanent copy of name for proc struct
    memcpy(arrayOfProcNames[storedProcNum], currentProcName, strlen(currentProcName)+1);
    //Set process name
    processes[storedProcNum].procName = arrayOfProcNames[storedProcNum];
    //Move past spaces
    while(isspace(inputLine[i]) && i < lineLength){
        i++;
    }
    //Check for keyword 'arrival' (Assumes there is no comments between process lines)
    int procArrivalFound = 0;
    if(isalpha(inputLine[i])){
        char* nameStr = "arrival";
        if( inputLine[i]   == nameStr[0] &&
            inputLine[i+1] == nameStr[1] &&
            inputLine[i+2] == nameStr[2] &&
            inputLine[i+3] == nameStr[3] &&
            inputLine[i+4] == nameStr[4] &&
            inputLine[i+5] == nameStr[5] &&
            inputLine[i+6] == nameStr[6])
            {
                procArrivalFound = 1; //Updates flag
                i += strlen(nameStr);
            }
    }
    //Error: 'arrival' keyword not found
    if(procArrivalFound == 0){
        printf("\"arrival\" keyword not found\n");
        int j;
        for(j = 0; j < storedProcNum; j++){
            free(&processes[j]);
        }
        free(currentProcName);
        free(arrayOfProcNames);
        free(inputLine);
        return ERROR;
    }
    //Move past spaces
    while(isspace(inputLine[i]) && i < lineLength){
        i++;
    }
    //Error: Arrival time (digit) not found
    if( !(isdigit(inputLine[i])) ){
        printf("'arrival' keyword found but arrival time not found on same line\n");
        int j;
        for(j = 0; j < storedProcNum; j++){
            free(&processes[j]);
        }
        free(currentProcName);
        free(arrayOfProcNames);
        free(inputLine);
        return ERROR;
    }
    char arrivalTimeAsString[MAX_PROCESS_DIGIT_NUMBER]; //Temporary arrival time storage
    int arrivalStringTempIndex = 0;
    //Temporarily store Arrival Time as string
    while(isdigit(inputLine[i]) ){
        arrivalTimeAsString[arrivalStringTempIndex] = inputLine[i];
        i++;
        arrivalStringTempIndex++;
    }
    arrivalTimeAsString[arrivalStringTempIndex] = (int)NULL;
    //Convert arrivalTimeAsString to Integer and store its value in currentProcArrival
    if((sscanf(arrivalTimeAsString, "%i", &currentProcArrival) != 1) && (currentProcArrival >= 0)){
        //Error converting occurred
        printf("Arrival time could not be properly converted.");
        int j;
        for(j = 0; j < storedProcNum; j++){
            free(&processes[j]);
        }
        free(currentProcName);
        free(arrayOfProcNames);
        free(inputLine);
        return ERROR;
    }
    //Create storage space in struct
    processes[storedProcNum].procArrival = malloc(sizeof(int));
    //Copy Integer value to struct
    memcpy(&processes[storedProcNum].procArrival, &currentProcArrival, sizeof(int));

    //Begin Processing Burst Length

    //Move past space
    while(isspace(inputLine[i]) && i < lineLength){
        i++;
    }
    //Check for keyword 'burst' (Assumes there is no comments between process lines)
    int procBurstFound = 0;
    if(isalpha(inputLine[i])){
        char* nameStr = "burst";
        if( inputLine[i]   == nameStr[0] &&
            inputLine[i+1] == nameStr[1] &&
            inputLine[i+2] == nameStr[2] &&
            inputLine[i+3] == nameStr[3] &&
            inputLine[i+4] == nameStr[4])
            {
                procBurstFound = 1; //Updates flag
                i += strlen(nameStr);
            }
    }
    //Error: 'burst' keyword not found
    if(procBurstFound == 0){
        printf("\"burst\" keyword not found\n");
        int j;
        for(j = 0; j < storedProcNum; j++){
            free(&processes[j]);
        }
        free(currentProcName);
        free(arrayOfProcNames);
        free(inputLine);
        return ERROR;
    }
    //Move past spaces
    while(isspace(inputLine[i]) && i < lineLength){
        i++;
    }
    //Error: Burst time (digit) not found
    if( !(isdigit(inputLine[i])) ){
        printf("'burst' keyword found but burst time not found on same line\n");
        int j;
        for(j = 0; j < storedProcNum; j++){
            free(&processes[j]);
        }
        free(currentProcName);
        free(arrayOfProcNames);
        free(inputLine);
        return ERROR;
    }
    char burstLengthAsString[MAX_PROCESS_DIGIT_NUMBER]; //Temporary arrival time storage
    int burstLengtTempIndex = 0;
    //Temporarily store Arrival Time as string
    while(isdigit(inputLine[i]) ){
        burstLengthAsString[burstLengtTempIndex] = inputLine[i];
        i++;
        burstLengtTempIndex++;
    }
    burstLengthAsString[burstLengtTempIndex] = (int)NULL;
    //Convert arrivalTimeAsString to Integer and store its value in currentProcArrival
    if((sscanf(burstLengthAsString, "%i", &currentProcBurst) != 1) && (currentProcBurst >= 0)){
        //Error converting occurred
        printf("Burst time could not be properly converted.");
        int j;
        for(j = 0; j < storedProcNum; j++){
            free(&processes[j]);
        }
        free(currentProcName);
        free(arrayOfProcNames);
        free(inputLine);
        return ERROR;
    }
    //Create storage space in struct
    processes[storedProcNum].procBurst = malloc(sizeof(int));
    //Copy Integer value to struct
    memcpy(&processes[storedProcNum].procBurst, &currentProcBurst, sizeof(int));

    //Get next line
    memset(inputLine,0,sizeof(inputLine));
    inputLine = fgets(inputLine,(int)MAX_INPUT_LINE_SIZE,fpIn);
    lineLength = strlen(inputLine);
    i = 0;

    //Check if next line is empty/EOF
        if(inputLine == NULL){
        //Free individual processes' data
            int j;
            for(j = 0; j < storedProcNum; j++){
                free(&processes[j]);
            }
                free(inputLine);
                return ERROR;
        }
    //Reset Flags, Name, A
    nameStrFound = 0;
    processStrFound = 0;
    currentProcArrival = 0;
    currentProcBurst = 0;

    }
    //proc fakeStruct = {fakeName,fakeArrival,fakeBurst};

    //@TODO get values in loop from

return SUCCESS;
}

//For Debugging
void printProcessesArray(proc* processes, int procCount){
    int idx;

    for(idx = 0; idx < procCount; idx++){
        printf("\n");
        printf("Process #%i Name: %s \n", (idx+1),processes[idx].procName);
        printf("Process #%i Arrival: %i\n", (idx+1),processes[idx].procArrival);
        printf("Process #%i Burst: %i\n", (idx+1),processes[idx].procBurst);
    }


}




