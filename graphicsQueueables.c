
#include "rect.h"
#include "hdc.h"
//Graphics.c owns these includes by extension

static struct hdcArray hdcVals = {}; //stores all the hdcs. this is in queuables because it is used by this function and graphics.c

int SimpleAbs(int a) //Simple implementation of Abs
{
    if(a < 0)
    {
        return a*-1;
    }
    else
    {
        return a;
    }
}


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


    //IDE has issues with this but it all works so ignore any issues here
	uchar* pixMem = (uchar*)P2V(0xA0000);
	ushort offset = (320 * y) + x; //320 due to the screen width
	pixMem[offset] = hdcVals.hdcObjects[hdc].penIndex; //Sets value at offset
    return 0;
}

void PixelSetterFunc(int hdc,int x,int y)
{
	uchar* pixMem = (uchar*)P2V(0xA0000);
	ushort offset = (320 * y) + x; //320 due to the screen width
	pixMem[offset] = hdcVals.hdcObjects[hdc].penIndex; //Sets value at offset
}

void MovePos(int hdc, int x,int y) //Changes the base position of the graphics cursor. Called by moveto and lineto.
{
    hdcVals.hdcObjects[hdc].x = x;
    hdcVals.hdcObjects[hdc].y = y;
}

void XIncPlotter(int x0, int y0, int x1, int y1, int hdc) //Bresenham but from x0 to x1
{
    int xInc = SimpleAbs(x1 - x0);
    int yInc = SimpleAbs(y1 - y0);
    int direc = (y0 > y1) ? -1 : 1;

    int D = (2*yInc) - xInc;
    int curY = y0;

    for (int curX = x0; curX <= x1; curX++)
    {
        PixelSetterFunc(hdc,curX, curY);
        if(D >= 0)
        {
            curY = curY + direc;
            D = D - 2*xInc;
        }
        D = D + 2*yInc;
    }
}

void YIncPlotter(int x0, int y0, int x1, int y1, int hdc) //Bresenham but from y0 to y1
{
    int xInc = SimpleAbs(x1 - x0);
    int yInc = SimpleAbs(y1 - y0);
    int direc = (x0 > x1) ? -1 : 1;

    int D = (2*xInc) - yInc;
    int curX = x0;

    for (int curY = y0; curY <= y1; curY++)
    {
        PixelSetterFunc(hdc,curX, curY);
        if(D >= 0)
        {
            curX = curX + direc;
            D = D - 2*yInc;
        }
        D = D + 2*xInc;
    }
}

int LineDrawFunc(struct argVal *cmdArgs)
{
    //Bresenham's line algorithm

    int hdc = ParseInt(cmdArgs[0].argumentVal);
    int x0 = hdcVals.hdcObjects[hdc].x;
    int y0 = hdcVals.hdcObjects[hdc].y;
    int x1 = ParseInt(cmdArgs[1].argumentVal);
    int y1 = ParseInt(cmdArgs[2].argumentVal);
    int xDif = (x1 - x0); //Difference in x - can be positive or negative
    int yDif = (y1 - y0); //Difference in y - can be positive or negative

    if(SimpleAbs(yDif) < SimpleAbs(xDif)) //Check if the positive yDif is less than the positive xDif
    {

        if(xDif <= 0) //If x goes from low to high
        {
            XIncPlotter(x1,y1,x0,y0,hdc);
        }
        else //high to low
        {
            XIncPlotter(x0,y0,x1,y1,hdc);
        }
    }
    else
    {
        if(yDif <= 0) //If y goes from low to high
        {
            YIncPlotter(x1,y1,x0,y0,hdc);
        }   
        else //high to low
        {
            YIncPlotter(x0,y0,x1,y1,hdc);
        }
    }

    MovePos(hdc,x1,y1); //set new position
    return 0;
}

void LineDraw(int x0,int y0, int x1,int y1, int hdc)
{
    //Bresenham's line algorithm

    int xDif = (x1 - x0); //Difference in x - can be positive or negative
    int yDif = (y1 - y0); //Difference in y - can be positive or negative

    if(SimpleAbs(yDif) < SimpleAbs(xDif)) //Check if the positive yDif is less than the positive xDif
    {
        if(xDif <= 0) //If x goes from low to high
        {
            XIncPlotter(x1,y1,x0,y0,hdc);
        }
        else //high to low
        {
            XIncPlotter(x0,y0,x1,y1,hdc);
        }
    }
    else
    {
        if(yDif <= 0) //If y goes from low to high
        {
            YIncPlotter(x1,y1,x0,y0,hdc);
        }
        else //high to low
        {
            YIncPlotter(x0,y0,x1,y1,hdc);
        }
    }

}

int MovePosFunc(struct argVal *cmdArgs) //Changes the base position of the graphics cursor. Called by moveto and lineto.
{
    int hdc = ParseInt(cmdArgs[0].argumentVal);
    int x = ParseInt(cmdArgs[1].argumentVal);
    int y = ParseInt(cmdArgs[2].argumentVal);

    hdcVals.hdcObjects[hdc].x = x;
    hdcVals.hdcObjects[hdc].y = y;
    return 0;
}

int DeclarePenFunc(struct argVal *cmdArgs) //Changes the base position of the graphics cursor. Called by moveto and lineto.
{
    int index = ParseInt(cmdArgs[1].argumentVal);
    int r = ParseInt(cmdArgs[2].argumentVal);
    int g = ParseInt(cmdArgs[3].argumentVal);
    int b = ParseInt(cmdArgs[4].argumentVal);


    outb(0x3C8,(uchar)index); //output index to 0x3C8 port (essentially a stack of all pens)

    //output the colours to the port
    outb(0x3C9,(uchar)r);
    outb(0x3C9,(uchar)g);
    outb(0x3C9,(uchar)b);
    return 0;
}

int GetPenFunc(struct argVal *cmdArgs)
{
    int hdc = ParseInt(cmdArgs[0].argumentVal);
    int index = ParseInt(cmdArgs[1].argumentVal);

    hdcVals.hdcObjects[hdc].penIndex = index;
    return 0;
}

int DrawRect(struct argVal *cmdArgs)
{
    int hdc = ParseInt(cmdArgs[0].argumentVal);
    int top = ParseInt(cmdArgs[1].argumentVal);
    int left = ParseInt(cmdArgs[2].argumentVal);
    int bottom = ParseInt(cmdArgs[3].argumentVal);
    int right = ParseInt(cmdArgs[4].argumentVal);

    for(int y = bottom; y <= top;y++) //fills the rectangle by drawing each line
    {
        LineDraw(left,y,right,y,hdc);
    }
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

void AppendLineTo(int hdc,char* x1,char* y1) //draw line added to the queue
{
    struct commandBuffer commandItemTemp;
    commandItemTemp.queuedAction = &LineDrawFunc;

    char str[100];
    ParseChar(hdc,str); //set hdc into str

    //Append relevant arguments
    AppendArg(str,&commandItemTemp);
    AppendArg(x1,&commandItemTemp);
    AppendArg(y1,&commandItemTemp);

    commandItemTemp.buffset = 0; //causes a buffer to reset - this fixes a strange issue where the commandItemTemp would append data
    //On top of its past instances

    int lastIndex = hdcVals.hdcObjects[hdc].queueEnd;
    
    hdcVals.hdcObjects[hdc].commandQueue[lastIndex] = commandItemTemp;

    hdcVals.hdcObjects[hdc].queueEnd++;
}

void AppendCursorMove(int hdc,char* x1,char* y1) //Add cursor movement to the queue
{
    struct commandBuffer commandItemTemp;
    commandItemTemp.queuedAction = &MovePosFunc;

    char str[100];
    ParseChar(hdc,str); //set hdc into str

    //Append relevant arguments
    AppendArg(str,&commandItemTemp);
    AppendArg(x1,&commandItemTemp);
    AppendArg(y1,&commandItemTemp);

    commandItemTemp.buffset = 0; //causes a buffer to reset - this fixes a strange issue where the commandItemTemp would append data
    //On top of its past instances

    int lastIndex = hdcVals.hdcObjects[hdc].queueEnd;
    
    hdcVals.hdcObjects[hdc].commandQueue[lastIndex] = commandItemTemp;

    hdcVals.hdcObjects[hdc].queueEnd++;
}

void AppendDeclarePen(int hdc,char* index, char* r, char* g, char* b) //Adds new pen declaration to the queue
{
    struct commandBuffer commandItemTemp;
    commandItemTemp.queuedAction = &DeclarePenFunc;

    char str[100];
    ParseChar(hdc,str); //set hdc into str

    //Append relevant arguments
    AppendArg(str,&commandItemTemp);
    AppendArg(index,&commandItemTemp);
    AppendArg(r,&commandItemTemp);
    AppendArg(g,&commandItemTemp);
    AppendArg(b,&commandItemTemp);

    commandItemTemp.buffset = 0; //causes a buffer to reset - this fixes a strange issue where the commandItemTemp would append data
    //On top of its past instances

    int lastIndex = hdcVals.hdcObjects[hdc].queueEnd;
    
    hdcVals.hdcObjects[hdc].commandQueue[lastIndex] = commandItemTemp;

    hdcVals.hdcObjects[hdc].queueEnd++;
}

void AppendGetPen(int hdc,char* index) //Adds new pen declaration to the queue
{
    struct commandBuffer commandItemTemp;
    commandItemTemp.queuedAction = &GetPenFunc;

    char str[100];
    ParseChar(hdc,str); //set hdc into str

    //Append relevant arguments
    AppendArg(str,&commandItemTemp);
    AppendArg(index,&commandItemTemp);

    commandItemTemp.buffset = 0; //causes a buffer to reset - this fixes a strange issue where the commandItemTemp would append data
    //On top of its past instances

    int lastIndex = hdcVals.hdcObjects[hdc].queueEnd;
    
    hdcVals.hdcObjects[hdc].commandQueue[lastIndex] = commandItemTemp;

    hdcVals.hdcObjects[hdc].queueEnd++;
}

void AppendDrawRect(int hdc,char* topY,char* botY,char* topX,char* botX) //Adds new pen declaration to the queue
{
    struct commandBuffer commandItemTemp;
    commandItemTemp.queuedAction = &DrawRect;

    char str[100];
    ParseChar(hdc,str); //set hdc into str

    //Append relevant arguments
    AppendArg(str,&commandItemTemp);
    AppendArg(topY,&commandItemTemp);
    AppendArg(botX,&commandItemTemp);
    AppendArg(botY,&commandItemTemp);
    AppendArg(topX,&commandItemTemp);

    commandItemTemp.buffset = 0; //causes a buffer to reset - this fixes a strange issue where the commandItemTemp would append data
    //On top of its past instances

    int lastIndex = hdcVals.hdcObjects[hdc].queueEnd;
    
    hdcVals.hdcObjects[hdc].commandQueue[lastIndex] = commandItemTemp;

    hdcVals.hdcObjects[hdc].queueEnd++;
}

int sys_writeQueue(void) //takes queue request to be processed at the endpaint
{
    int hdc;
    int opType;
    struct argsSet* set; //pointer to the args
    argint(0,&hdc);
    argint(1,&opType);
    argptr(2,(void*)&set, sizeof(*set) < 0);

    if(opType == 0) //pixel drawing OP
    {
        AppendPixel(hdc,(char*)set->arg0,(char*)set->arg1); //adds method to queue
    }
    else if(opType == 1) //line drawing op
    {
        AppendLineTo(hdc,(char*)set->arg0,(char*)set->arg1); //adds method to queue
    }
    else if(opType == 2) //cursor movement op
    {
        AppendCursorMove(hdc,(char*)set->arg0,(char*)set->arg1); //adds method to queue
    }
    else if(opType == 3) //declare pen op
    {
        AppendDeclarePen(hdc,(char*)set->arg0,(char*)set->arg1,(char*)set->arg2,(char*)set->arg3);
    }
    else if(opType == 4) //get pen op
    {
        AppendGetPen(hdc,(char*)set->arg0);
    }
    else if(opType == 5) //draw rect op
    {
        AppendDrawRect(hdc,(char*)set->arg0,(char*)set->arg1,(char*)set->arg2,(char*)set->arg3);
    }


    return 1;
}