#include "types.h"
#include "user.h"
#include "rect.h"
#include "hdc.h"


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

//char* argsBuffer[30]; //stores the partitioned args in the input
//char inputbuffer[30];

int AcceptInput()
{
    //gets() for input
    //Split args into different char* and store in global space
    //return arg count
}

int main(int argc, char *argv[]) 
{

    //add checking to ensure values are integer

    //atoi uses arguments and converts them to their integer values (not using ascii)
    int currentHDC = -1; //stores current HDC value
    printf(1,"Entered painter\n");
    while(1)
    {
        printf(1,">");

        int argsCount = AcceptInput();

        for(int x =0;x<argsCount;x++)
        {
            printf(1,"%s|\n",argsBuffer[x]);
        }
        
        int errorCode=0; //This should always be 0. If the value is -1 this means something failed.
        printf(1,"HDC %d\n", currentHDC);

        if(strcmp(argsBuffer[1],"-e") == 0 && argsCount >= 0) //draw example square
        {
            setvideomode(0x13);
            errorCode = ExampleSquare();
            if(errorCode != -1){getch();} //Expect input
            setvideomode(0x03);
        }
        else if(strcmp(argsBuffer[1],"-p") == 0 && argsCount >= 4) //draw pixel
        {
            setvideomode(0x13);
            errorCode = setpixel(currentHDC,atoi(argsBuffer[2]),atoi(argsBuffer[3])); 
            if(errorCode != -1){getch();} //Expect input
            setvideomode(0x03);
        }
        else if(strcmp(argsBuffer[1],"-l") == 0 && argsCount >= 4) //draw line from cursor
        {
            int newX = atoi(argsBuffer[2]);
            int newY = atoi(argsBuffer[3]);
            setvideomode(0x13);
            errorCode = lineto(currentHDC,newX,newY);
            if(errorCode != -1){getch();}; //Expect input
            if(errorCode != -1){printf(1,"Cursor position moved to (%d,%d)\n",newX,newY);}
            setvideomode(0x03);
        }
        else if(strcmp(argsBuffer[1],"-m") == 0 && argsCount >= 4) //move cursor
        {
            int newX = atoi(argsBuffer[2]);
            int newY = atoi(argsBuffer[3]);
            errorCode = moveto(currentHDC,newX,newY);
            if(errorCode != -1){printf(1,"Cursor position moved to (%d,%d)\n",newX,newY);}
        }
        else if(strcmp(argsBuffer[1],"-dp") == 0 && argsCount >= 4) //declare pen
        {
            int index = atoi(argsBuffer[2]);
            int r = atoi(argsBuffer[3]);
            int g = atoi(argsBuffer[4]);
            int b = atoi(argsBuffer[5]);
            errorCode = setpencolour(index,r,g,b);
            if(errorCode != -1){printf(1,"Set new pen colour at index %d\n",index);}
        }
        else if(strcmp(argsBuffer[1],"-gp") == 0 && argsCount >= 3) //get pointer
        {
            int pen = atoi(argsBuffer[2]);
            errorCode =selectpen(currentHDC,pen);
            if(errorCode != -1){printf(1,"Got Pen %d\n",pen);}
        }
        else if(strcmp(argsBuffer[1],"-r") == 0 && argsCount >= 5) //draw rect
        {

            int x0 = atoi(argsBuffer[2]);
            int x1 = atoi(argsBuffer[3]);

            int y0 = atoi(argsBuffer[4]);
            int y1 = atoi(argsBuffer[5]);

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

            if(errorCode != -1){getch();}; 
            setvideomode(0x03);

        }
        else if(strcmp(argsBuffer[1],"-nHDC") == 0 && argsCount >= 0) //-nHDC declares a new HDC item
        {
            errorCode = beginpaint(0); //create a new HDC object
            if(errorCode != -1){
                printf(1,"Created new HDC at index %d\n",errorCode);
                currentHDC = errorCode;
            }
        }
        else if(strcmp(argsBuffer[1],"-exit") == 0)
        {
            break;
        }
        else
        {
            printf(1,"Invalid Arguments. Painter accepts the following arguments:\n -nHDC (Create new HDC) \n -exit (exit the function)) \n -e (Show example image) \n -p [X] [Y] (Draw pixel) \n -l [X] [Y] (Draw line to location) \n -m [X] [Y] (Set cursor position)\n -dp [Index] [R/63] [G/63] [B/63] (declare a new pen at the index with the specified RGB)\n -gp [Index] (Get a pen from the specified index)\n -r [X0] [X1] [Y0] [Y1] (Draws a rectangle)\n");
        }
    }
    exit();
}