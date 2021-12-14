struct argVal //stores characters representing an argument
{
    char argumentVal[100];
};

typedef   int (*Function)(struct argVal[]); //Stores a function with an unlimited parameter set

struct commandBuffer{
    Function queuedAction; //Stores the function to execute next
    struct argVal args[20]; //Stores array of args
    int lastArgSet; //stores the index of the last argument in the argVal set
    int buffset; //stores 1 if the buffer int is valid
};

struct hdc{
    int init;
    int x;
    int y;
    int penIndex;
    struct commandBuffer commandQueue[50]; //store command queue
    int queueEnd; //stores the end of the command queue
};

struct hdcArray{
    struct hdc hdcObjects[5]; //Holds 5 hdc objects
};

struct argsSet{
    char* arg0;
    char* arg1;
    char* arg2;
    char* arg3;
    char* arg4;
};