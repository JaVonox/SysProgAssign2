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
    moveto(0,0,0); //Reset to default position
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
    // x,y
    // draws a line to x1 y1
    //4. -m
    // x,y
    // draws a line to x and y without changing the cursor position

    //add checking to ensure values are integer

    //atoi uses arguments and converts them to their integer values (not using ascii)

    if(strcmp(argv[1],"-e") == 0 && argc >= 0)
    {
        setvideomode(0x13);
        ExampleSquare();
        getch(); //Expect input
        setvideomode(0x03);
    }
    else if(strcmp(argv[1],"-p") == 0 && argc >= 4)
    {
        setvideomode(0x13);
        setpixel(0,atoi(argv[2]),atoi(argv[3])); 
        getch(); //Expect input
        setvideomode(0x03);
    }
    else if(strcmp(argv[1],"-l") == 0 && argc >= 4)
    {
        setvideomode(0x13);
        lineto(0,atoi(argv[2]),atoi(argv[3]));
        getch(); //Expect input
        setvideomode(0x03);
    }
    else if(strcmp(argv[1],"-m") == 0 && argc >= 4)
    {
        int newX = atoi(argv[2]);
        int newY = atoi(argv[3]);
        moveto(0,newX,newY);
        printf(1,"Cursor position moved\n");
    }
    else
    {
        printf(1,"Invalid Arguments. Painter accepts the following arguments:\n -e (Show example image) \n -p [X] [Y] (Draw pixel) \n -l [X] [Y] (Draw line to location) \n -m [X] [Y] (Set cursor position)\n");
    }

    exit();
}