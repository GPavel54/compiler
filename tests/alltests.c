#include "ctest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CTEST(alltests, counting)
{
    CTEST_LOG("Test of counting...");
    CTEST_LOG("Compiling...");
    system("../bin/compiler ../progs/counting.cs ../assembler/counting.asm");
    CTEST_LOG("Making object file...");
    system("nasm -f elf64 ../assembler/counting.asm -o ../assembler/counting.o");
    CTEST_LOG("Linking...");
    system("gcc -no-pie ../assembler/counting.o -o counting");
    CTEST_LOG("Executing...");
    system("./counting > out.txt");
    
    FILE *fp;
    fp = fopen("out.txt", "r");
    if (fp != NULL)
    {
        CTEST_LOG("out file doesn't exist. Test failed");
        ASSERT_EQUAL(1, 2);
        return;
    }
    char ch;
    char str[10] = "";
    int index = 0;
    while ((ch = fgetc(fp)) != EOF)
    {
        str[index++] = ch;
    }
    int u = 0;
    if (strcmp(str, "3"))
    {
        u = 1;
    }
    ASSERT_EQUAL(1, u);
}

