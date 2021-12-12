
#include "rect.h"
#include "hdc.h"
//Graphics.c owns these includes by extension

static struct hdcArray hdcVals = {}; //stores all the hdcs. this is in queuables because it is used by this function and graphics.c

int GetIntLength(int x)
{
    int iter=0;
    int xCh=x;

    if(xCh == 0) //in case of HDC 0
    {
        iter++;
    }

    while(xCh > 0)
    {
        iter++;
        xCh/=10;
    }
    return iter;
}


int ParseInt(char* charToInt) //char to int
{
    int holder=0;
    int offsetMP=1;
    for(int x=strlen(charToInt) -1;x>=0;x--)
    {
        holder += ((int)charToInt[x]-48) * offsetMP;
        offsetMP *=10;
    }
    return holder;
}

void ParseChar(int intToChar, char* outChar) //int to char
{
    int len = GetIntLength(intToChar);
    for(int x=0;x<len;x++)
    {
        int power=1;
        for(int y=1;y<(len-x);y++)
        {
            power *= 10;
        }
        outChar[x] = ((intToChar / power) %10) + 48;
    }
    outChar[len] = '\0';
}

int TempFunc(char *argv[]) //This is a demo function. Should not be usable in the final build
{
    cprintf("Test!");
    return 0;
}

int PixelSetterFuncQueue(struct argVal *cmdArgs)
{
    int hdc = ParseInt(cmdArgs[0].argumentVal);
    int x = ParseInt(cmdArgs[1].argumentVal);
    int y = ParseInt(cmdArgs[2].argumentVal);

    cprintf("%d,",x);
    cprintf("%d\n",y);

    //IDE has issues with this but it all works so ignore any issues here
	uchar* pixMem = (uchar*)P2V(0xA0000);
	ushort offset = (320 * y) + x; //320 due to the screen width
	pixMem[offset] = hdcVals.hdcObjects[hdc].penIndex; //Sets value at offset
    return 0;
}

//Queuing functions

void AppendArg(char* argument, struct commandBuffer *buffer) //appends all the args in the set to the 
{
    if(buffer->buffset != 1)
    {
        buffer->lastArgSet = 0;
        buffer->buffset = 1;
    }


    struct argVal tmpArg;
    strncpy(tmpArg.argumentVal,argument,sizeof(argument));

    buffer->args[buffer->lastArgSet] = tmpArg;
    buffer->lastArgSet = buffer->lastArgSet +1;
}

void AppendPixel(int hdc, char* x, char* y) //Adds pixel action to the queue
{
    struct commandBuffer commandItemTemp;
    commandItemTemp.queuedAction = &PixelSetterFuncQueue;

    char str[100];
    ParseChar(hdc,str); //set hdc into str

    //Append relevant arguments
    AppendArg(str,&commandItemTemp);

    AppendArg(x,&commandItemTemp);
    
    AppendArg(y,&commandItemTemp);

    commandItemTemp.buffset = 0; //causes a buffer to reset - this fixes a strange issue where the commandItemTemp would append data
    //On top of its past instances

    int lastIndex = hdcVals.hdcObjects[hdc].queueEnd;
    
    hdcVals.hdcObjects[hdc].commandQueue[lastIndex] = commandItemTemp;

    hdcVals.hdcObjects[hdc].queueEnd++;

}

int sys_writeQueue(void) //takes queue request to be processed at the endpaint
{
    int opType;
    int argsSize;
    char* argSet[30];
    argint(0,&opType);
    argint(1,&argsSize);
    argptr(2,(char**)&argSet,argsSize);

    cprintf("%s",argSet[0]);
    cprintf("/%s",argSet[1]);

    return 1;
}