#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"

void PixelSetterFunc(int hdc,int x,int y)
{
	uchar* pixMem = (uchar*)P2V(0xA0000);
	ushort offset = (320 * y) + x;
	pixMem[offset] = hdc;
}

void clear320x200x256() {
	// You need to put code to clear the video buffer here.  Initially, 
	// you might just set each pixel to black in a nested loop, but think
	// about faster ways to do it. 
	//
	// This function is called from videosetmode.

	//For some reason the junk values stay in temporarily?

	for (int x = 0; x <= 320; x++)
	{
		for (int y = 0; y <= 200; y++)
		{
			PixelSetterFunc(0,x,y);
		}
	}
}

//Graphics calls must be preceeded by setvideomode(int); //0x13 for video mode, 0x03 for text mode

//System calls which use functions in this file

int sys_setpixel(void) //int hdc, int x, int y
{
    int hdc,x,y;

    argint(0, &hdc); //take arguments
    argint(1, &x);
    argint(2, &y);

    if(hdc < 0 || x < 0 || y < 0) //TODO this may not fufill the requirements for parameter checking?
    {
        return -1;
    }
	
	PixelSetterFunc(hdc,x,y);
	//outb(pixelMem,hdc); //set the value at the pixel memory location to the specified colour
	
    return 0;
}

int sys_moveto(void) //int hdc, int x, int y
{
    int hdc,x,y;

    argint(0, &hdc); //take arguments
    argint(1, &x);
    argint(2, &y);
    return 0;
}

int sys_lineto(void) //int hdc, int x, int y
{
    int hdc,x,y;

    argint(0, &hdc); //take arguments
    argint(1, &x);
    argint(2, &y);

    return 0;
}
