
#include "rect.h"
#include "hdc.h"
//Graphics.c owns these includes by extension

static struct hdcArray hdcVals = {}; //stores all the hdcs. this is in queuables because it is used by this function and graphics.c

int TempFunc(char *argv[]) //This is a demo function. Should not be usable in the final build
{
    cprintf("Augh!");
    return 0;
}

int PixelSetterFuncQueue(char *argv[])
{
    int hdc = (int)argv[0];
    int x = (int)argv[1];
    int y = (int)argv[2];

	uchar* pixMem = (uchar*)P2V(0xA0000);
	ushort offset = (320 * y) + x; //320 due to the screen width
	pixMem[offset] = hdcVals.hdcObjects[hdc].penIndex; //Sets value at offset
    return 0;
}