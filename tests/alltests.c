#include "ctest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

CTEST(alltests, counting)
{
    CTEST_LOG("Testing counting...");
    CTEST_LOG("Compiling...");
    system("bin/compiler progs/counting.cs assembler/counting.asm");
    CTEST_LOG("Making object file...");
    system("nasm -f elf64 assembler/counting.asm -o assembler/counting.o");
    CTEST_LOG("Linking...");
    system("gcc -no-pie assembler/counting.o -o counting");
    CTEST_LOG("Executing...");
    system("./counting > out.txt");
    sleep(1);
    FILE *fp;
    fp = fopen("out.txt", "r");
    if (fp == NULL)
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
    CTEST_LOG("Program returned: %s, expected 3", str);
    if (strcmp(str, "3 ") == 0)
    {
        u = 1;
    }
    ASSERT_EQUAL(1, u);
    system("rm out.txt");
    fclose(fp);
}

CTEST(alltests, branch_then)
{
    CTEST_LOG("Testing branches_then...");
    CTEST_LOG("Compiling...");
    system("bin/compiler progs/branches_then.cs assembler/branches_then.asm");
    CTEST_LOG("Making object file...");
    system("nasm -f elf64 assembler/branches_then.asm -o assembler/branches_then.o");
    CTEST_LOG("Linking...");
    system("gcc -no-pie assembler/branches_then.o -o branches_then");
    CTEST_LOG("Executing...");
    system("./branches_then > out.txt");
    sleep(1);

    char ch;
    FILE *fp;
    fp = fopen("out.txt", "r");
    if (fp == NULL)
    {
        CTEST_LOG("out file doesn't exist. Test failed");
        ASSERT_EQUAL(1, 2);
        return;
    }
    char str[10] = "";
    int index = 0;
    while ((ch = fgetc(fp)) != EOF)
    {
        str[index++] = ch;
    }
    int u = 0;
    CTEST_LOG("Program returned: %s, expected 24", str);
    if (strcmp(str, "24 ") == 0)
    {
        u = 1;
    }
    ASSERT_EQUAL(1, u);
    system("rm out.txt");
    fclose(fp);
}

CTEST(alltests, branch_else)
{
    CTEST_LOG("Testing branches_else...");
    CTEST_LOG("Compiling...");
    system("bin/compiler progs/branches_else.cs assembler/branches_else.asm");
    CTEST_LOG("Making object file...");
    system("nasm -f elf64 assembler/branches_else.asm -o assembler/branches_else.o");
    CTEST_LOG("Linking...");
    system("gcc -no-pie assembler/branches_else.o -o branches_else");
    CTEST_LOG("Executing...");
    system("./branches_else > out.txt");
    sleep(1);

    char ch;
    FILE *fp;
    fp = fopen("out.txt", "r");
    if (fp == NULL)
    {
        CTEST_LOG("out file doesn't exist. Test failed");
        ASSERT_EQUAL(1, 2);
        return;
    }
    char str[10] = "";
    int index = 0;
    while ((ch = fgetc(fp)) != EOF)
    {
        str[index++] = ch;
    }
    int u = 0;
    CTEST_LOG("Program returned: %s, expected 16", str);
    if (strcmp(str, "16 ") == 0)
    {
        u = 1;
    }
    ASSERT_EQUAL(1, u);
    system("rm out.txt");
    fclose(fp);
}

CTEST(alltests, string)
{
    CTEST_LOG("Testing string...");
    CTEST_LOG("Compiling...");
    system("bin/compiler progs/string.cs assembler/string.asm");
    CTEST_LOG("Making object file...");
    system("nasm -f elf64 assembler/string.asm -o assembler/string.o");
    CTEST_LOG("Linking...");
    system("gcc -no-pie assembler/string.o -o string");
    CTEST_LOG("Executing...");
    system("./string > out.txt");
    sleep(1);

    char ch;
    FILE *fp;
    fp = fopen("out.txt", "r");
    if (fp == NULL)
    {
        CTEST_LOG("out file doesn't exist. Test failed");
        ASSERT_EQUAL(1, 2);
        return;
    }
    char str[20] = "";
    int index = 0;
    while ((ch = fgetc(fp)) != EOF)
    {
        str[index++] = ch;
    }
    char sample[19] = {'A', 5, 's', 5, 's', 5, 'e', 5, 'm', 5, 'b', 5, 'l', 5, 'e', 5, 'r', 5};
    int u = 0;
    CTEST_LOG("Length = %ld", strlen(str));
    CTEST_LOG("Program returned: %s, expected Assembler", str);
    if (strcmp(str, sample) == 0)
    {
        u = 1;
    }
    ASSERT_EQUAL(1, u);
    system("rm out.txt");
    fclose(fp);
}

CTEST(alltests, loop)
{
    CTEST_LOG("Testing whiel loop...");
    CTEST_LOG("Compiling...");
    system("bin/compiler progs/while.cs assembler/while.asm");
    CTEST_LOG("Making object file...");
    system("nasm -f elf64 assembler/while.asm -o assembler/while.o");
    CTEST_LOG("Linking...");
    system("gcc -no-pie assembler/while.o -o while");
    CTEST_LOG("Executing...");
    system("./while > out.txt");
    sleep(1);

    char ch;
    FILE *fp;
    fp = fopen("out.txt", "r");
    if (fp == NULL)
    {
        CTEST_LOG("out file doesn't exist. Test failed");
        ASSERT_EQUAL(1, 2);
        return;
    }
    char str[20] = "";
    int index = 0;
    while ((ch = fgetc(fp)) != EOF)
    {
        str[index++] = ch;
    }
    int u = 0;
    CTEST_LOG("Length = %ld", strlen(str));
    CTEST_LOG("Program returned: %s, expected 53", str);
    if (strcmp(str, "53 ") == 0)
    {
        u = 1;
    }
    ASSERT_EQUAL(1, u);
    system("rm out.txt");
    fclose(fp);
}
