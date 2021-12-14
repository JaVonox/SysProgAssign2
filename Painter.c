#include "types.h"
#include "user.h"
#include "rect.h"
#include "hdc.h"


//Used for the painting functions from stage 1
int ExampleSquare(int hdc)
{
    int errors=0; //Since a valid response always equals 0, anything less than 0 will mean an error has occured.
    errors += moveto(hdc, 100, 50);
    errors += lineto(hdc, 200, 50);
    errors += lineto(hdc, 200, 150);
    errors += lineto(hdc, 100, 150);
    errors += lineto(hdc, 100, 50);
    errors += moveto(0,0,0); //Reset to 0,0 position
    return errors;
}

//Duplicate functions for various purposes

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

int AcceptInput(char argsBuffer[30][50])
{
    //gets() for input
    //Split args into different char* and store in global space
    //return arg count

    char inputbuffer[50];

    gets(inputbuffer,49); //get input
    int inputLen= strlen(inputbuffer) - 1; //length -1 (to skip the end character)
    
    int charIter=0;
    int wordIter=0;

    char preBuffArgs[30][50]= {}; //before any fixing can be done to the arg order
    for(int i=0;i<inputLen;i++)
    {
        if( inputbuffer[i] == ' ' || inputbuffer[i] == '\0' ) //check for space or newline char
        {
            preBuffArgs[wordIter][charIter] = '\0';
            wordIter++; //add an extra word to the list
            charIter=0; //reset the character value to 0
        }
        else
        {
            preBuffArgs[wordIter][charIter] = inputbuffer[i];
            charIter++; //increment the character value
        }
    }

    if(inputLen >= 48) //if the input len reaches the maximum input value
    {
        return -1;
    }

    int lastIndex =0; //count the index of the argument
    for(int x=0;x<wordIter+1;x++)
    {
        if(strcmp(preBuffArgs[x],"") != 0)
        {
            strcpy(argsBuffer[lastIndex],preBuffArgs[x]); //copy the contents of preBuff into argsBuffer 
            lastIndex++; //add to next index
        }
    }

    return lastIndex; //amount of words generated
}

int main(int argc, char *argv[]) 
{

    //add checking to ensure values are integer

    //atoi uses arguments and converts them to their integer values (not using ascii)
    int currentHDC = -1; //stores current HDC value
    int mode = 0; //0 == queuemode, 1 == system call mode
    printf(1,"Entered Painter\n");
    printf(1,"Enter your command starting with the keyword 'Painter' - type 'Painter -help' for a list of commands\n");
    while(1)
    {
        if(currentHDC != -1) //User legibility features
        {
            printf(1,"HDC%d>",currentHDC);
        }
        else
        {
            printf(1,"NoHDC>");
        }

        char argsBuffer[30][50] = {}; //stores the partitioned args in the input
        int argsCount = AcceptInput(argsBuffer); //gets count of arguments

        int errorCode=0; //This should always be 0. If the value is -1 this means something failed.
        
        if(argsCount == -1)
        {
            //There is an issue where inputs that are too long may be interpreted as seperate commands but this issue is uncommon
            printf(1,"Command exceeded maximum input length. Please reenter your query.\n");
        }
        else if(strcmp(argsBuffer[1],"-exit") == 0)
        {
            if(currentHDC != -1)
            {
                endpaint(currentHDC,0); //dumps the HDC without displaying queued data
                printf(1,"Quit painter and dumped HDC\n",currentHDC);
                break;
            }
            else
            {
                printf(1,"Quitting Painter\n");
                break;
            }
        }
        else if(strcmp(argsBuffer[1],"-nHDC") == 0 && argsCount >= 2) //-nHDC declares a new HDC item.
        {
            if(currentHDC != -1)
            {
                printf(1,"There is currently an active HDC\n");
            }
            else
            {
                errorCode = beginpaint(0); //create a new HDC object
                if(errorCode != -1){
                    printf(1,"Created new HDC at index %d\n",errorCode);
                    currentHDC = errorCode;
                }
            }
        }
        else if(strcmp(argsBuffer[1],"-toggleMode") == 0 && argsCount >= 2) //switches to sys if queue and vice versa
        {
            mode = (mode == 0) ? 1 : 0; //switches mode between 1 and 0
            
            if(mode == 0)
            {
                printf(1,"Entered queueing mode\n");
            }
            else if(mode == 1)
            {
                printf(1,"Entered system call mode (Previous queue will remain active)\n");
            }
        }
        else if(strcmp(argsBuffer[1],"-help") == 0)
        {
            if(mode == 0)
            {
                printf(1,"You are currently in queue mode. The following commands are available in this mode:\n");
                printf(1,"-nHDC declares a new HDC at the next available hdc index\n");
                printf(1,"-exec executes all the queued commands and disposes of the HDC\n");
            }
            else if(mode == 1)
            {
                printf(1,"You are currently in system call mode. The following commands are available in this mode:\n");
                printf(1,"-nHDC declares a new HDC at the next available hdc index\n");
                printf(1,"-e draws an example square\n");
            }

            printf(1,"-toggleMode switches the system between queue and system call mode and vice versa\n");
            printf(1,"-p [X] [Y] sets a pixel at a specified X and Y coordinate\n");
            printf(1,"-l [X] [Y] draws a line towards the specified location from the current graphics cursor position\n");
            printf(1,"-m [X] [Y] move the graphics cursor to the pecified position\n");
            printf(1,"-dp [index] [R/63] [G/63] [B/63] declares a new pen at the pen index with the colours specified");
            printf(1,"-gp [index] gets the pen from the specified index (values 0-15 are preset)\n");
            printf(1,"-r [X0] [X1] [Y0] [Y1] draws a filled rectangle between the 2 points specified\n");
            printf(1,"-exit quits the program and disposes of the HDC\n");
        }
        else if(currentHDC < 0 || currentHDC > 5)
        {
            printf(1,"There is currently no active HDC. enter Painter -nHDC to create a new device context\n");
        }
        else if(strcmp(argsBuffer[1],"-e") == 0 && argsCount >= 2 && mode == 1) //draw example square
        {
            setvideomode(0x13);
            errorCode = ExampleSquare(currentHDC);
            if(errorCode != -1){getch();} //Expect input
            setvideomode(0x03);
        }
        else if(strcmp(argsBuffer[1],"-p") == 0 && argsCount >= 4 && mode == 1) //draw pixel using system call
        {
            setvideomode(0x13);
            errorCode = setpixel(currentHDC,atoi(argsBuffer[2]),atoi(argsBuffer[3])); 
            if(errorCode != -1){getch();} //Expect input
            setvideomode(0x03);
        }
        else if(strcmp(argsBuffer[1],"-l") == 0 && argsCount >= 4 && mode == 1) //draw line from cursor using system call
        {
            int newX = atoi(argsBuffer[2]);
            int newY = atoi(argsBuffer[3]);
            setvideomode(0x13);
            errorCode = lineto(currentHDC,newX,newY);
            if(errorCode != -1){getch();}; //Expect input
            if(errorCode != -1){printf(1,"Cursor position moved to (%d,%d)\n",newX,newY);}
            setvideomode(0x03);
        }
        else if(strcmp(argsBuffer[1],"-m") == 0 && argsCount >= 4 && mode == 1) //move cursor using system call
        {
            int newX = atoi(argsBuffer[2]);
            int newY = atoi(argsBuffer[3]);
            errorCode = moveto(currentHDC,newX,newY);
            if(errorCode != -1){printf(1,"Cursor position moved to (%d,%d)\n",newX,newY);}
        }
        else if(strcmp(argsBuffer[1],"-dp") == 0 && argsCount >= 6 && mode == 1) //declare pen
        {
            int index = atoi(argsBuffer[2]);
            int r = atoi(argsBuffer[3]);
            int g = atoi(argsBuffer[4]);
            int b = atoi(argsBuffer[5]);
            errorCode = setpencolour(index,r,g,b);
            if(errorCode != -1){printf(1,"Set new pen colour at index %d\n",index);}
        }
        else if(strcmp(argsBuffer[1],"-gp") == 0 && argsCount >= 3 && mode == 1) //get pen
        {
            int pen = atoi(argsBuffer[2]);
            errorCode =selectpen(currentHDC,pen);
            if(errorCode != -1){printf(1,"Got Pen %d\n",pen);}
        }
        else if(strcmp(argsBuffer[1],"-r") == 0 && argsCount >= 6 && mode == 1) //draw rect
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
            errorCode = fillrect(currentHDC,&newRect);

            if(errorCode != -1){getch();}; 
            setvideomode(0x03);

        }
        else if(strcmp(argsBuffer[1],"-p") == 0 && argsCount >= 4 && mode == 0) //queue draw pixel
        {
            struct argsSet args;
            args.arg0 = argsBuffer[2];
            args.arg1 = argsBuffer[3];
            writeQueue(currentHDC,0,&args); 
            printf(1,"Queued pixel setting action\n");
        }
        else if(strcmp(argsBuffer[1],"-l") == 0 && argsCount >= 4 && mode == 0) //draw line from cursor
        {
            struct argsSet args;
            args.arg0 = argsBuffer[2];
            args.arg1 = argsBuffer[3];
            writeQueue(currentHDC,1,&args);
            printf(1,"Queued line drawing and cursor movement\n");

        }
        else if(strcmp(argsBuffer[1],"-m") == 0 && argsCount >= 4 && mode == 0) //move cursor
        {
            struct argsSet args;
            args.arg0 = argsBuffer[2];
            args.arg1 = argsBuffer[3];

            writeQueue(currentHDC,2,&args);
            printf(1,"Queued cursor movement\n");
        }
        else if(strcmp(argsBuffer[1],"-dp") == 0 && argsCount >= 6 && mode == 0) //declare pen
        {
            struct argsSet args;
            args.arg0 = argsBuffer[2]; //index
            args.arg1 = argsBuffer[3]; //R
            args.arg2 = argsBuffer[4]; //G
            args.arg3 = argsBuffer[5]; //B

            writeQueue(currentHDC,3,&args);
            printf(1,"Queued pen declaration\n");
        }
        else if(strcmp(argsBuffer[1],"-gp") == 0 && argsCount >= 3 && mode == 0) //get pen
        {
            struct argsSet args;
            args.arg0 = argsBuffer[2]; //index

            writeQueue(currentHDC,4,&args);
            printf(1,"Queued pen get\n");
        }
        else if(strcmp(argsBuffer[1],"-r") == 0 && argsCount >= 6 && mode == 0) //draw rect
        {
            int x0 = atoi(argsBuffer[2]);
            int x1 = atoi(argsBuffer[3]);

            int y0 = atoi(argsBuffer[4]);
            int y1 = atoi(argsBuffer[5]);

            //Sort X values and Y values into top/bottom left/right
            int topY,botX,botY,topX;

            topY = (y0 > y1) ? y0 : y1;
            botY = (y0 > y1) ? y1 : y0;

            topX = (x0 > x1) ? x0 : x1;
            botX = (x0 > x1) ? x1 : x0;

            struct argsSet args;
            char arg0tmp[50],arg1tmp[50],arg2tmp[50],arg3tmp[50];
            ParseChar(topY,arg0tmp);
            ParseChar(botY,arg1tmp);
            ParseChar(topX,arg2tmp);
            ParseChar(botX,arg3tmp);

            args.arg0 = arg0tmp;
            args.arg1 = arg1tmp;
            args.arg2 = arg2tmp;
            args.arg3 = arg3tmp;

            writeQueue(currentHDC,5,&args);
            printf(1,"Queued rectangle drawing\n");

        }
        else if(strcmp(argsBuffer[1],"-exec") == 0 && argsCount >= 2 && mode == 0) //draw the values from the queue
        {
            if(currentHDC == -1)
            {
                printf(1,"There is currently no HDC selected. Please use -nHDC to create a new device context\n");
            }
            else
            {
                setvideomode(0x13);
                if(endpaint(currentHDC,1) == 0) //if the HDC was successfully killed
                {
                    currentHDC = -1;
                    errorCode = 0;
                    printf(1,"Queued objects processed. HDC dumped.\n");
                }
                else
                {
                    errorCode = -1;
                    printf(1,"An error occured during execution\n"); //This should not occur ideally
                }

                if(errorCode != -1){getch();}; 
                setvideomode(0x03);
            }
        }
        else //any invalid arguments fall to here
        {
            printf(1,"Command not recognised for this mode or invalid arguments were supplied. please enter 'Painter -help' for a list of available commands\n");
        }
    }
    exit();
}