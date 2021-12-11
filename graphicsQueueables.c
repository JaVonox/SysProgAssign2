
#include "rect.h"
#include "hdc.h"
//Graphics.c owns these includes by extension

static struct hdcArray hdcVals = {}; //stores all the hdcs. this is in queuables because it is used by this function and graphics.c

int GetIntLength(int x)
{
    int iter=0;
    int xCh=x;
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
    buffer->lastArgSet++;
    cprintf("%d\n",buffer->lastArgSet);
}

void AppendPixel(int hdc, char* x, char* y) //Adds pixel action to the queue
{
    struct commandBuffer commandItemTemp;
    commandItemTemp.queuedAction = &PixelSetterFuncQueue;

    char str[100];
    ParseChar(hdc,str); //set hdc into str

    //Append these arguments
    AppendArg(str,&commandItemTemp);

    AppendArg(x,&commandItemTemp);

    AppendArg(y,&commandItemTemp);

    cprintf("%s-",x);
    cprintf("%s\n",y);
    
    cprintf("%s:",commandItemTemp.args[1].argumentVal);
    cprintf("%s\n",commandItemTemp.args[2].argumentVal);

    int lastIndex = hdcVals.hdcObjects[hdc].queueEnd;
    
    hdcVals.hdcObjects[hdc].commandQueue[lastIndex] = commandItemTemp;
    
    hdcVals.hdcObjects[hdc].queueEnd++;

}