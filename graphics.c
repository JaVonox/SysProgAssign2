#include "types.h"
#include "defs.h"
#include "memlayout.h"
#include "x86.h"
#include "rect.h"

typedef struct 
{ //Stores the current graphics cursor position
    int x; //Due to the nature of ints, x and y are defaulted to 0 each. Hence fufilling the requirement for default values.
    int y;
    int penIndex; //the index of the pen. defaults to white
} moveToXY;

moveToXY cursor = { //default values for the cursor
    .x = 0,
    .y = 0,
    .penIndex = 15
};

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
	pixMem[offset] = cursor.penIndex; //Sets value at offset
}

void MovePos(int x,int y) //Changes the base position of the graphics cursor. Called by moveto and lineto.
{
    cursor.x = x;
    cursor.y = y;
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
	
    ValueCapper(&x,319);
    ValueCapper(&y,199);

	PixelSetterFunc(0,x,y);
	
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

    ValueCapper(&x,319);
    ValueCapper(&y,199);

    MovePos(x,y);

    return 0;
}

void XIncPlotter(int x0, int y0, int x1, int y1) //Bresenham but from x0 to x1
{
    int xInc = (x1 - x0);
    int yInc = (y1 - y0);

    int D = 2*yInc - xInc;
    int curY = y0;
    for (int curX = x0; curX <= x1; curX++)
    {
        PixelSetterFunc(0,curX, curY);
        if(D > 0)
        {
            curY = curY + 1;
            D = D - 2*xInc;
        }
        D = D + 2*yInc;
    }
}

void YIncPlotter(int x0, int y0, int x1, int y1) //Bresenham but from y0 to y1
{
    int xInc = (x1 - x0);
    int yInc = (y1 - y0);

    int D = 2*xInc - yInc;
    int curX = x0;
    for (int curY = y0; curY <= y1; curY++)
    {
        PixelSetterFunc(0,curX, curY);
        if(D > 0)
        {
            curX = curX + 1;
            D = D - 2*yInc;
        }
        D = D + 2*xInc;
    }
}

void LineDraw(int x0,int y0, int x1,int y1)
{
    //Bresenham's line algorithm

    int xDif = (x1 - x0); //Difference in x - can be positive or negative
    int yDif = (y1 - y0); //Difference in y - can be positive or negative

    if(SimpleAbs(yDif) < SimpleAbs(xDif)) //Check if the positive yDif is less than the positive xDif
    {
        if(xDif <= 0) //If x goes from low to high
        {
            XIncPlotter(x1,y1,x0,y0);
        }
        else //high to low
        {
            XIncPlotter(x0,y0,x1,y1);
        }
    }
    else
    {
        if(yDif <= 0) //If y goes from low to high
        {
            YIncPlotter(x1,y1,x0,y0);
        }
        else //high to low
        {
            YIncPlotter(x0,y0,x1,y1);
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

    ValueCapper(&nx,319);
    ValueCapper(&ny,199);

    LineDraw(cursor.x,cursor.y,nx,ny);

    MovePos(nx,ny); //set new position
    
    return 0;
}


int sys_setpencolour(void)
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

    if(index > 255)
    {
        cprintf("This operation would get a pen outside of the allocated memory (16-255) and therefore is not permitted\n");
        return -1;
    }

    cursor.penIndex = index;

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

    for(int y = pointedRect->bottom; y <= pointedRect->top;y++) //fills the rectangle by drawing each line
    {
        LineDraw(pointedRect->left,y,pointedRect->right,y);
    }

    return 0;

}