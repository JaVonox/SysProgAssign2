#include "types.h"
#include "user.h"

//Used for the painting functions from stage 1
void ExampleSquare()
{
    setpixel(0,10,10);
    moveto(0, 100, 50);
    lineto(0, 200, 50);
    lineto(0, 200, 150);
    lineto(0, 100, 150);
    lineto(0, 100, 50);
}

void PixelSetter(int x,int y)
{
    setpixel(0,x,y);
}

void LineSetter(int x0, int y0, int x1, int y1)
{
    moveto(0,x0,y0);
    lineto(0,x1,y1);
}

int main(int argc, char *argv[]) 
{
    //args
    //1. -e
    //Runs example square coe
    //2. -p
    // x,y
    //allows setting of a pixel
    //3. -l
    // x0,y0,x1,y1
    setvideomode(0x13);

    //add error handling?
    //atoi uses arguments and converts them to their integer values (not using ascii)

    if(strcmp(argv[1],"-e") == 0)
    {
        ExampleSquare();
    }
    else if(strcmp(argv[1],"-p") == 0)
    {
        PixelSetter(atoi(argv[2]),atoi(argv[3])); 
    }
    else if(strcmp(argv[1],"-l") == 0)
    {
        LineSetter(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
    }

    getch(); //Expect input
    setvideomode(0x03);
    exit();
}