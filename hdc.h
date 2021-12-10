
typedef   int (*Function)(char *args[]); //Stores a function with an unlimited parameter set

struct commandBuffer{
    //System calls may not call other system calls so this implemention will depreciate graphics system calls aside from endpaint
    //TODO may be worth implementing a single operation function so that the other system calls are used still

    Function queuedAction; //Stores the function to execute next
    char** args; //Stores array of args
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
