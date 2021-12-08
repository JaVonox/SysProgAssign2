#include "types.h"
#include "defs.h"
#include "memlayout.h"
#include "x86.h"
#include "rect.h"
#include "hdc.h"
#include "spinlock.h"

//These values are static between all instances of 
static struct hdcArray hdcVals = {}; //stores all the hdcs
static struct spinlock lock; //single static lock to prevent thread/cpu conflicts. 
//Only one lock is needed as both begin/endpaint access the hdcVals array
static int lockinit;

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

void ValueCapper(int *a,int capacity)
{
    if((*a) > capacity || (*a) < 0)
    {
        cprintf("A supplied value exceeded its capacity and was limited\n");
    }
    *a = ((*a) > capacity) ? capacity : *a;
    *a = ((*a) < 0) ? 0 : *a;
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

void clear320x200x256() {
	// This function is called from videosetmode.

    //Fast set all data to 0 colour (black)
    uchar* pixMem = (uchar*)P2V(0xA0000);
    memset(pixMem, 0, (320 * 200));

}

//Graphics calls must be preceeded by setvideomode(int); //0x13 for video mode, 0x03 for text mode

//System calls which use functions in this file

int sys_setpixel(void) //int hdc, int x, int y
{
    int hdc,x,y;

    if(argint(0, &hdc) < 0 || argint(1, &x) < 0 || argint(2, &y) < 0) 
    {
        cprintf("An error occured in the pixel setter function, cancelling its operation. Ensure all arguments were valid.\n");
        return -1;
    }

    if(hdc < 0)
    {
        cprintf("Invalid HDC value: %d \n", hdc);
        return -1;
    }
	
    ValueCapper(&x,319);
    ValueCapper(&y,199);

	PixelSetterFunc(hdc,x,y);
	
    return 0;
}

int sys_moveto(void) //int hdc, int x, int y
{
    int hdc,x,y;

    if(argint(0, &hdc) < 0 || argint(1, &x) < 0 || argint(2, &y) < 0)
    {
        cprintf("An error occured in the movement function, cancelling its operation. Ensure all arguments were valid.\n");
        return -1;
    }

    if(hdc < 0)
    {
        cprintf("Invalid HDC value: %d \n", hdc);
        return -1;
    }

    ValueCapper(&x,319);
    ValueCapper(&y,199);

    MovePos(hdc,x,y);

    return 0;
}

void XIncPlotter(int x0, int y0, int x1, int y1, int hdc) //Bresenham but from x0 to x1
{
    int xInc = (x1 - x0);
    int yInc = (y1 - y0);

    int D = 2*yInc - xInc;
    int curY = y0;
    for (int curX = x0; curX <= x1; curX++)
    {
        PixelSetterFunc(hdc,curX, curY);
        if(D > 0)
        {
            curY = curY + 1;
            D = D - 2*xInc;
        }
        D = D + 2*yInc;
    }
}

void YIncPlotter(int x0, int y0, int x1, int y1, int hdc) //Bresenham but from y0 to y1
{
    int xInc = (x1 - x0);
    int yInc = (y1 - y0);

    int D = 2*xInc - yInc;
    int curX = x0;
    for (int curY = y0; curY <= y1; curY++)
    {
        PixelSetterFunc(hdc,curX, curY);
        if(D > 0)
        {
            curX = curX + 1;
            D = D - 2*yInc;
        }
        D = D + 2*xInc;
    }
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
int sys_lineto(void) //int hdc, int nx, int ny
{
    int hdc,nx,ny;

    if(argint(0, &hdc) < 0 || argint(1, &nx) < 0 || argint(2, &ny) < 0)
    {
        cprintf("An error occured in the line function, cancelling its operation. Ensure all arguments were valid.\n");
        return -1;
    }

    if(hdc < 0)
    {
        cprintf("Invalid HDC value: %d \n", hdc);
        return -1;
    }

    ValueCapper(&nx,319);
    ValueCapper(&ny,199);

    LineDraw(hdcVals.hdcObjects[hdc].x,hdcVals.hdcObjects[hdc].y,nx,ny,hdc);

    MovePos(hdc,nx,ny); //set new position
    
    return 0;
}


int sys_setpencolour(void) //Operates outside of HDC
{
    //index cant be less than 15 - these values are already written to.
    int index,r,g,b;

    if(argint(0, &index) < 0 || argint(1, &r) < 0 || argint(2, &g) < 0 || argint(3, &b) < 0)
    {
        cprintf("An error occured in the pen creator function, cancelling its operation. Ensure all arguments were valid.\n");
        return -1;
    }

    ValueCapper(&r,63);
    ValueCapper(&g,63);
    ValueCapper(&b,63);

    if(index < 16) //check if index was valid
    {
        cprintf("This operation would overwrite a system pen (Pens 0-15) and therefore is not permitted\n");
        return -1;
    }
    else if(index > 255)
    {
        cprintf("This operation would write to a pen outside of the allocated memory (16-255) and therefore is not permitted\n");
        return -1;
    }

    outb(0x3C8,(uchar)index); //output index to 0x3C8 port (essentially a stack of all pens)

    //output the colours to the port
    outb(0x3C9,(uchar)r);
    outb(0x3C9,(uchar)g);
    outb(0x3C9,(uchar)b);

    return 0;
}

int sys_selectpen(void) 
{
    int hdc,index;

    if(argint(0, &hdc) < 0 || argint(1, &index) < 0)
    {
        cprintf("An error occured in the pen selector function, cancelling its operation. Ensure all arguments were valid.\n");
        return -1;
    }

    if(hdc < 0)
    {
        cprintf("Invalid HDC value: %d \n", hdc);
        return -1;
    }

    if(index > 255)
    {
        cprintf("This operation would get a pen outside of the allocated memory (16-255) and therefore is not permitted\n");
        return -1;
    }

    hdcVals.hdcObjects[hdc].penIndex = index;

    return 0;
}

int sys_fillrect(void) //hdc, pointer to rect
{
    int hdc;
    struct rect* pointedRect; //pointer to a rectangle passed in arguments

    if(argint(0,&hdc) < 0 || argptr(1,(void*)&pointedRect, sizeof(*pointedRect) < 0)) //(void*) stops issues with argptr
    {
        return -1;
    }

    if(hdc < 0)
    {
        cprintf("Invalid HDC value: %d \n", hdc);
        return -1;
    }

    //Limits values
    ValueCapper(&pointedRect->top,199);
    ValueCapper(&pointedRect->bottom,199);
    ValueCapper(&pointedRect->left,319);
    ValueCapper(&pointedRect->right,319);

    for(int y = pointedRect->bottom; y <= pointedRect->top;y++) //fills the rectangle by drawing each line
    {
        LineDraw(pointedRect->left,y,pointedRect->right,y,hdc);
    }

    return 0;

}

int sys_beginpaint(void)
{
    int hwnd;

    if(lockinit != 1) //simple check to see if lock exists.
    {
        initlock(&lock, "grapicslock"); //make lock first to avoid any other threads passing this before the lock exists
        lockinit = 1; //prevents future calls to initlock
        //beginpaint is expected to always occur before endpaint in any case, therefore a lock must exist in all cases
    }


    if(argint(0,&hwnd) != 0) //hwnd must be 0 
    {
        return -1;
    }

    acquire(&lock); //spinlock until access can be made to the hdc static

    for (int index = 0; index < 5; index++) {
        if (hdcVals.hdcObjects[index].init == 0) { //1 means an initialised value
            struct hdc item;
            item.init = 1;
            item.x = 0;
            item.y = 0;
            item.penIndex = 15;
            hdcVals.hdcObjects[index] = item;
            release(&lock); //release lock if successful
            return index;
        }
    }

    release(&lock); //release lock if failed

    return -1;
}

int sys_endpaint(void)
{
    int hdc;

    if(argint(0,&hdc) < 0) 
    {
        return -1;
    }

    if(hdc < 0 )
    {
        return -1;
    }

    //Need to acquire the lock while the data is being drawn and the hdc is potentially being dropped
    acquire(&lock);

    //IMPORTANT TODO
    //making a new pen and selecting a pen must be queued so that stage 4 can write to data during the lock
    //Only locking during making a new pen and selecting a pen can potentially cause pen data to be overwritten by other HDCs
    //during processing

    //Drawing script here

    if (hdcVals.hdcObjects[hdc].init == 1) { //Resets all the values in the array at the hdc index, including setting the init to 0 - meaning the space is free
        struct hdc item;
        item.init = 0;
        item.x = 0;
        item.y = 0;
        item.penIndex = 15;
        hdcVals.hdcObjects[hdc] = item;
        release(&lock);
        return 0;
    }

    release(&lock);

    return -1;
}

int sys_getHDC(void)
{
    //this command allows a user to switch HDC objects at will
    //It is meant to serve as a demonstration of how HDCs do not interact with one another
    //This can be done by using the command in an instance of "Painter" to access an existing HDC index
    //This feature would not be present in any release of the system, but exists to permit demonstration of this feature for stage 3

    int hdc;

    if(argint(0,&hdc) < 0) 
    {
        return -1;
    }

    if(hdc > 5 || hdc < 0) //checks the input is within the acceptable range
    {
        return -1;
    }

    if (hdcVals.hdcObjects[hdc].init == 1) 
    { 
        return 0; //returns 0 to permit the system to assign the HDC value if the value is initialised
    }
    else
    {
        return -1; //returns -1 if the index is not intialised
    }
}