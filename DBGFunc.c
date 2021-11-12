#include "types.h"
#include "user.h"

//This file is intended to work as a debug space to run system calls independently.
//This should not exist in the final release - it is not intended for usage outside of debugging.
//References:
//Makefile - UPROGS - _DBGFunc

int main(int argc, char *argv[])
{
    printf(1,"This function exists for debugging! Please dont use me!\n");
    exit();
}