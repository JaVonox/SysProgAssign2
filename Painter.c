#include "types.h"
#include "user.h"
#include "rect.h"

//Used for the painting functions from stage 1
int ExampleSquare()
{
    int errors=0; //Since a valid response always equals 0, anything less than 0 will mean an error has occured.
    errors += moveto(0, 100, 50);
    errors += lineto(0, 200, 50);
    errors += lineto(0, 200, 150);
    errors += lineto(0, 100, 150);
    errors += lineto(0, 100, 50);
    errors += moveto(0,0,0); //Reset to default position
    return errors;
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

    int errorCode=0; //This should always be 0. If the value is -1 this means something failed.
    if(strcmp(argv[1],"-e") == 0 && argc >= 0) //draw example square
    {
        setvideomode(0x13);
        errorCode = ExampleSquare();
        getch(); //Expect input
        setvideomode(0x03);
    }
    else if(strcmp(argv[1],"-p") == 0 && argc >= 4) //draw pixel
    {
        setvideomode(0x13);
        errorCode = setpixel(0,atoi(argv[2]),atoi(argv[3])); 
        getch(); //Expect input
        setvideomode(0x03);
    }
    else if(strcmp(argv[1],"-l") == 0 && argc >= 4) //draw line from cursor
    {
        int newX = atoi(argv[2]);
        int newY = atoi(argv[3]);
        setvideomode(0x13);
        errorCode = lineto(0,newX,newY);
        getch(); //Expect input
        if(errorCode != -1){printf(1,"Cursor position moved to (%d,%d)\n",newX,newY);}
        setvideomode(0x03);
    }
    else if(strcmp(argv[1],"-m") == 0 && argc >= 4) //move cursor
    {
        int newX = atoi(argv[2]);
        int newY = atoi(argv[3]);
        errorCode = moveto(0,newX,newY);
        if(errorCode != -1){printf(1,"Cursor position moved to (%d,%d)\n",newX,newY);}
    }
    else if(strcmp(argv[1],"-dp") == 0 && argc >= 4) //declare pen
    {
        int index = atoi(argv[2]);
        int r = atoi(argv[3]);
        int g = atoi(argv[4]);
        int b = atoi(argv[5]);
        errorCode = setpencolour(index,r,g,b);
        if(errorCode != -1){printf(1,"Set new pen colour at index %d\n",index);}
    }
    else if(strcmp(argv[1],"-gp") == 0 && argc >= 3) //get pointer
    {
        int pen = atoi(argv[2]);
        errorCode =selectpen(0,pen);
        if(errorCode != -1){printf(1,"Got Pen %d\n",pen);}
    }
    else if(strcmp(argv[1],"-r") == 0 && argc >= 5) //draw rect
    {

        int x0 = atoi(argv[2]);
        int x1 = atoi(argv[3]);

        int y0 = atoi(argv[4]);
        int y1 = atoi(argv[5]);

        struct rect newRect; //send in vertices as int array

        //Sort X values and Y values into top/bottom left/right
        int topY,botX,botY,topX;

        topY = (y0 > y1) ? y0 : y1;
        botY = (y0 > y1) ? y1 : y0;

        topX = (x0 > x1) ? x0 : x1;
        botX = (x0 > x1) ? x1 : x0;

        newRect.top = topY;
        newRect.left = botX;
        newRect.bottom = botY;
        newRect.right = topX;
        
        setvideomode(0x13);
        errorCode = fillrect(0,&newRect);

        getch(); 
        setvideomode(0x03);

    }
    else
    {
        printf(1,"Invalid Arguments. Painter accepts the following arguments:\n -e (Show example image) \n -p [X] [Y] (Draw pixel) \n -l [X] [Y] (Draw line to location) \n -m [X] [Y] (Set cursor position)\n -dp [Index] [R/63] [G/63] [B/63] (declare a new pen at the index with the specified RGB)\n -gp [Index] (Get a pen from the specified index)\n -r [X0] [X1] [Y0] [Y1] (Draws a rectangle)\n");
    }

    exit();
}