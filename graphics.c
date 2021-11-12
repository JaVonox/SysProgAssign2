#include "types.h"
#include "defs.h"
#include "memlayout.h"

static struct { //Stores the current graphics cursor position
    int x;
    int y;
} movetoXY;;

void PixelSetterFunc(int hdc,int x,int y, int col) //col is maybe temporary - if im not mistaken HDC should handle colour data
{
	uchar* pixMem = (uchar*)P2V(0xA0000);
	ushort offset = (320 * y) + x; //320 due to the screen width
	pixMem[offset] = col; //Sets value at offset
}

void MovePos(int x,int y) //Changes the base position of the graphics cursor. Called by moveto and lineto.
{
    movetoXY.x = x;
    movetoXY.y = y;
}

void clear320x200x256() {
	// This function is called from videosetmode.

    //Simple clearing function
	for (int x = 0; x < 320; x++)
	{
		for (int y = 0; y < 200; y++)
		{
			PixelSetterFunc(0,x,y,0);
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

    if(hdc < 0 || x < 0 || y < 0) 
    {
        return -1;
    }
	
	PixelSetterFunc(0,x,y,0xF);
	
    return 0;
}

int sys_moveto(void) //int hdc, int x, int y
{
    int hdc,x,y;

    argint(0, &hdc); //take arguments
    argint(1, &x);
    argint(2, &y);

    if(hdc < 0 || x < 0 || y < 0)
    {
        return -1;
    }

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
        PixelSetterFunc(0,curX, curY,5);
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
        PixelSetterFunc(0,curX, curY,5);
        if(D > 0)
        {
            curX = curX + 1;
            D = D - 2*yInc;
        }
        D = D + 2*xInc;
    }
}

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

int sys_lineto(void) //int hdc, int nx, int ny
{
    int hdc,nx,ny;

    argint(0, &hdc); //take arguments
    argint(1, &nx);
    argint(2, &ny);

    //Bresenham's line algorithm

    int xDif = (nx - movetoXY.x); //Difference in x - can be positive or negative
    int yDif = (ny - movetoXY.y); //Difference in y - can be positive or negative

    if(SimpleAbs(yDif) < SimpleAbs(xDif)) //Check if the positive yDif is less than the positive xDif
    {
        if(xDif <= 0) //If x goes from low to high
        {
            XIncPlotter(nx,ny,movetoXY.x,movetoXY.y);
        }
        else //high to low
        {
            XIncPlotter(movetoXY.x,movetoXY.y,nx,ny);
        }
    }
    else
    {
        if(yDif <= 0) //If y goes from low to high
        {
            YIncPlotter(nx,ny,movetoXY.x,movetoXY.y);
        }
        else //high to low
        {
            YIncPlotter(movetoXY.x,movetoXY.y,nx,ny);
        }
    }

    MovePos(nx,ny); //set new position
    
    return 0;
}
