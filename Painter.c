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

int AcceptInput(char argsBuffer[30][30]) //TODO set hard character limits
{
    //gets() for input
    //Split args into different char* and store in global space
    //return arg count

    char inputbuffer[30];

    gets(inputbuffer,25); //get input
    int inputLen= strlen(inputbuffer) - 1; //length -1 (to skip the end character)
    
    int charIter=0;
    int wordIter=0;

    char preBuffArgs[30][30]= {}; //before any fixing can be done to the arg order
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

        char argsBuffer[30][30] = {}; //stores the partitioned args in the input
        int argsCount = AcceptInput(argsBuffer); //gets count of arguments

        int errorCode=0; //This should always be 0. If the value is -1 this means something failed.
        if(strcmp(argsBuffer[1],"-exit") == 0)
        {
            if(currentHDC != -1)
            {
                printf(1,"There is currently a HDC device active. This HDC will remain active and can be resumed using the -gHDC command\n");
                printf(1,"If you would rather delete the HDC please use the -kHDC command before using -exit\n");
                printf(1,"Do you still want to proceed with exiting painter, leaving the current HDC active (enter y for yes)?\n");
                printf(1,"exit?>");

                char argumentInput[10];
                gets(argumentInput,2);

                if(argumentInput[0] == 'y')
                {
                    printf(1,"Quitting Painter, leaving HDC %d active.\n",currentHDC);
                    break;
                }             
                printf(1,"\n");
            }
            else
            {
                printf(1,"Quitting Painter\n");
                break;
            }
        }
        else if(strcmp(argsBuffer[1],"-gHDC") == 0 && argsCount >= 2)
        {

                printf(1,"This command is designed for debugging/administrative uses only, are you sure you wish to proceed (enter y for yes)?\n");
                printf(1,"gHDC?>");

                char argumentInput[10];
                gets(argumentInput,2);

                if(argumentInput[0] == 'y')
                {
                    errorCode = getHDC(atoi(argsBuffer[2]));

                    if(errorCode != -1)
                    {
                        printf(1, "Switched to HDC %d\n", atoi(argsBuffer[2]));
                        currentHDC = atoi(argsBuffer[2]); //sets the current HDC to the supplied value
                    }
                    else
                    {
                        printf(1, "Supplied index was either invalid or there is no existing HDC object at the provided index. Please resubmit your query.\n");
                    }
                }
                else
                {
                    printf(1,"\n");
                }
        }
        else if(strcmp(argsBuffer[1],"-nHDC") == 0 && argsCount >= 0) //-nHDC declares a new HDC item.
        {
            if(currentHDC != -1)
            {
                printf(1,"There is currently another HDC active - please enter 'Painter -kHDC' to delete the current HDC object.\n");
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
        else if(currentHDC == -1 && strcmp(argsBuffer[1],"-help") != 0)
        {
            printf(1,"No device context is currently selected, use 'Painter -nHDC' to create a new device context.\n");
        }
        else if(strcmp(argsBuffer[1],"-e") == 0 && argsCount >= 0) //draw example square
        {
            setvideomode(0x13);
            errorCode = ExampleSquare(currentHDC);
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
        else if(strcmp(argsBuffer[1],"-gp") == 0 && argsCount >= 3) //get pen
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
            errorCode = fillrect(currentHDC,&newRect);

            if(errorCode != -1){getch();}; 
            setvideomode(0x03);

        }
        else if(strcmp(argsBuffer[1],"-kHDC") == 0 && argsCount >= 1) //dispose of HDC
        {
            if(currentHDC == -1)
            {
                printf(1,"There is currently no HDC selected. Please use -nHDC to create a new device context\n");
            }
            else
            {
                if(endpaint(currentHDC) == 0) //if the HDC was successfully killed
                {
                    currentHDC = -1;
                    printf(1,"The current HDC object has been released.\n");
                }
                else
                {
                    printf(1,"An error occured while resetting the HDC object\n"); //This should not occur ideally
                }
            }
        }
        else //any invalid arguments or -helps fall to here
        {
            printf(1,"Painter accepts the following arguments:\n -nHDC (Create new HDC) \n -gHDC [Index] (Get a HDC object from a specific address, this feature is intended for debugging use only) \n -kHDC (Dispose of current HDC) \n -exit (exit the painter) \n -e (Show example image) \n -p [X] [Y] (Draw pixel) \n -l [X] [Y] (Draw line to location) \n -m [X] [Y] (Set cursor position)\n -dp [Index] [R/63] [G/63] [B/63] (declare a new pen at the index with the specified RGB)\n -gp [Index] (Get a pen from the specified index)\n -r [X0] [X1] [Y0] [Y1] (Draws a rectangle)\n");
        }
    }
    exit();
}