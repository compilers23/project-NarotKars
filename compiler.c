#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

//lexer
const int lexem_words_length = 8;
const int operations_count = 3;
const char lexem_words[8][5] = { "swap", "nip", "tuck", "dup", "drop", "over", ".s" };
const char operations[3][2] = { "+", "-", "*" };

#define MAX_TOKENS 1024
#define MAX_TOKEN_LENGTH 8

bool IsDigit(char input)
{
    return input >= '0' && input <= '9';
}

bool IsNumeric(char input[])
{
    int j = 0;
    if (input[0] == '-')
    {
        j = 1;
    }
    for (int i = j; input[i] != '\0'; i++)
    {
        if (!IsDigit(input[i]))
            return false;
    }
    return true;
}

bool IsFromLexemWord(char input[])
{
    for (int i = 0; i < lexem_words_length; i++)
    {
        if (strcmp(input, lexem_words[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

bool IsOperation(char input[])
{
    if (input[0] != '\0' && input[1] == '\0') {
        for (int i = 0; i < operations_count; i++) {
            if (input[0] == operations[i][0]) {
                return true;
            }
        }
    }
    return false;
}

void ReplaceNewlinesAndTabsWithSpaces(char* str) {
    for (; *str != '\0'; str++) {
        if (*str == '\n' || *str == '\t') {
            *str = ' ';
        }
    }
}

void TokenizeRow(char input[], char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH], int* tokenCount)
{
    char* token;
    ReplaceNewlinesAndTabsWithSpaces(input);

    token = strtok(input, " ");
    while (token != NULL && *tokenCount < MAX_TOKENS) {

        if (IsNumeric(token) || IsFromLexemWord(token) || IsOperation(token))
        {
            strncpy(tokens[*tokenCount], token, MAX_TOKEN_LENGTH - 1);
            tokens[*tokenCount][MAX_TOKEN_LENGTH - 1] = '\0';
            (*tokenCount)++;
        }
        else
        {
            fprintf(stderr, "Token fault: %s", token);
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, " ");
    }
}

//Assembly generator
char axRegister[] = "rax";
char bxRegister[] = "rbx";
char diRegister[] = "rdi";
char spRegister[] = "rsp";
char siRegister[] = "rsi";
char ipRegister[] = "rip";
const char add[] = "addq";
const char push[] = "pushq";
const char pop[] = "popq";
const char sub[] = "subq";
const char mov[] = "movq";
const char mul[] = "imulq";
const char lea[] = "lea";
const char call[] = "call";

bool IsRegister(char input[])
{
    if (strcmp(input, axRegister) == 0)
    {
        return true;
    }

    if (strcmp(input, bxRegister) == 0)
    {
        return true;
    }

    return false;
}

void GeneratePush(FILE* file, char input[])
{
    fprintf(file, push);
    fprintf(file, " ");
    if (IsNumeric(input))
    {
        fprintf(file, "$");
    }
    else if (IsRegister(input))
    {
        fprintf(file, "%%");
    }
    fprintf(file, input);
    fprintf(file, "\n");
}

void GeneratePop(FILE* file, char registerName[])
{
    fprintf(file, pop);
    fprintf(file, " ");
    fprintf(file, "%%");
    if (IsRegister(registerName))
    {
        fprintf(file, registerName);
    }
    fprintf(file, "\n");
}

void GeneratePrintTop(FILE* file)
{
     fprintf(file, mov);
fprintf(file, " (");
fprintf(file, "%%");
fprintf(file, spRegister);
fprintf(file, "), ");
fprintf(file, "%%");
fprintf(file, siRegister);
fprintf(file, "\n");

fprintf(file, mov);
fprintf(file, " ");
fprintf(file, "$0");
fprintf(file, ", ");
fprintf(file, "%%");
fprintf(file, axRegister);
fprintf(file, "\n");

GeneratePush(file, axRegister);

fprintf(file, lea);
fprintf(file, " fmt(");
fprintf(file, "%%");
fprintf(file, ipRegister);
fprintf(file, "), ");
fprintf(file, "%%");
fprintf(file, diRegister);
fprintf(file, "\n");

fprintf(file, "call printf\n");

fprintf(file, mov);
fprintf(file, " ");
fprintf(file, "$0");
fprintf(file, ", ");
fprintf(file, "%%");
fprintf(file, diRegister);
fprintf(file, "\n");

fprintf(file, "call fflush\n");

}

void GenerateAddition(FILE* file)
{
    GeneratePop(file, axRegister);
    GeneratePop(file, bxRegister);
    fprintf(file, add);
    fprintf(file, " ");
    fprintf(file, "%%");
    fprintf(file, bxRegister);
    fprintf(file, ",");
    fprintf(file, " ");
    fprintf(file, "%%");
    fprintf(file, axRegister);
    fprintf(file, "\n");
    GeneratePush(file, axRegister);
}

void GenerateSubtraction(FILE* file)
{
    GeneratePop(file, axRegister);
    GeneratePop(file, bxRegister);
    fprintf(file, sub);
    fprintf(file, " ");
    fprintf(file, "%%");
    fprintf(file, axRegister);
    fprintf(file, ",");
    fprintf(file, " ");
    fprintf(file, "%%");
    fprintf(file, bxRegister);
    fprintf(file, "\n");
    GeneratePush(file, bxRegister);
}

void GenerateMultiplication(FILE* file)
{
    GeneratePop(file, bxRegister);
    GeneratePop(file, axRegister);
    fprintf(file, mul);
    fprintf(file, " ");
    fprintf(file, "%%");
    fprintf(file, axRegister);
    fprintf(file, " ");
    fprintf(file, ", ");
    fprintf(file, "%%");
    fprintf(file, bxRegister);
    fprintf(file, "\n");
    GeneratePush(file, axRegister);
}

void GenerateSwap(FILE* file, char registerName[]) {
    char tempRegister[] = "rcx";

    GeneratePop(file, axRegister);
    GeneratePop(file, bxRegister);

    fprintf(file, "mov %%%s, %%%s\n", axRegister, tempRegister);
    fprintf(file, "mov %%%s, %%%s\n", bxRegister, axRegister);
    fprintf(file, "mov %%%s, %%%s\n", tempRegister, bxRegister);

    GeneratePush(file, bxRegister);
    GeneratePush(file, axRegister);
}

void GenerateNip(FILE* file, char registerName[]) {

    GeneratePop(file, axRegister);
    GeneratePop(file, bxRegister);
    GeneratePush(file, axRegister);
}

void GenerateTuck(FILE* file, char registerName[]) {

    GeneratePop(file, axRegister);
    GeneratePop(file, bxRegister);

    GeneratePush(file, axRegister);
    GeneratePush(file, bxRegister);
    GeneratePush(file, axRegister);
}

void GenerateOver(FILE* file, char registerName[]) {

    GeneratePop(file, axRegister);
    GeneratePop(file, bxRegister);

    GeneratePush(file, bxRegister);
    GeneratePush(file, axRegister);
    GeneratePush(file, bxRegister);
}

// Starting stack: ... X Y
//   After over: ... X Y X
//   After tuck: ... Y X Y


void GenerateDup(FILE* file, char registerName[]) {

    GeneratePop(file, axRegister);

    GeneratePush(file, axRegister);
    GeneratePush(file, axRegister);
}

void GenerateDrop(FILE* file, char registerName[]) {

    GeneratePop(file, axRegister);
}

bool Validate(char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH], int tokenCount)
{
    if (!IsNumeric(tokens[0]))
    {
        return false;
    }

    if (!IsNumeric(tokens[1]) && !strcmp(tokens[1], "dup"))
    {
        return false;
    }

    return true;
}

//Compile
void Compile(FILE* file)
{
    char line[1024];
    char tokens[MAX_TOKENS][MAX_TOKEN_LENGTH];
    int tokenCount = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        TokenizeRow(line, tokens, &tokenCount);
    }

    if (tokenCount == 0)
    {
        perror("There are no tokens");
    }

    if (!Validate(tokens, tokenCount))
    {
        perror("Validation error");
    }

    FILE* outputFile;
    const char* outputFileName = "test.s";
    outputFile = fopen(outputFileName, "w");

    fprintf(outputFile, ".data\n");
    fprintf(outputFile, "fmt: .asciz \"%%d\\n\"\n");
    fprintf(outputFile, "newline: .ascii \"\\n\"\n");
    fprintf(outputFile, ".text\n");
    fprintf(outputFile, ".globl _main\n");
    fprintf(outputFile, "_main:\n");

    for (int i = 0; i < tokenCount; i++)
    {
        if (IsNumeric(tokens[i]))
        {
            GeneratePush(outputFile, tokens[i]);
        }
        if (IsOperation(tokens[i]))
        {
            if (strcmp(tokens[i], "+") == 0)
            {
                GenerateAddition(outputFile);
            }
            else if (strcmp(tokens[i], "-") == 0)
            {
                GenerateSubtraction(outputFile);
            }
            else if (strcmp(tokens[i], "*") == 0)
            {
                GenerateMultiplication(outputFile);
            }
        }
        if (IsFromLexemWord(tokens[i]))
        {
            if (strcmp(tokens[i], "swap") == 0)
            {
                GenerateSwap(outputFile, tokens[i]);
            }
            else if (strcmp(tokens[i], "nip") == 0)
            {
                GenerateNip(outputFile, tokens[i]);
            }
            else if (strcmp(tokens[i], "tuck") == 0)
            {
                GenerateTuck(outputFile, tokens[i]);
            }
            else if (strcmp(tokens[i], "dup") == 0)
            {
                GenerateDup(outputFile, tokens[i]);
            }
            else if (strcmp(tokens[i], "drop") == 0)
            {
                GenerateDrop(outputFile, tokens[i]);
            }
            else if (strcmp(tokens[i], "over") == 0)
            {
                GenerateOver(outputFile, tokens[i]);
            }
            else if (strcmp(tokens[i], ".s") == 0)
            {
                GeneratePrintTop(outputFile);
            }
        }
    }

    fprintf(outputFile, mov);
    fprintf(outputFile, " ");
    fprintf(outputFile, "%%");
    fprintf(outputFile, axRegister);
    fprintf(outputFile, ", ");
    fprintf(outputFile, "%%");
    fprintf(outputFile, diRegister);
    fprintf(outputFile, "\n");

    fprintf(outputFile, mov);
    fprintf(outputFile, " ");
    fprintf(outputFile, "$60");
    fprintf(outputFile, ", ");
    fprintf(outputFile, "%%");
    fprintf(outputFile, axRegister);
    fprintf(outputFile, "\n");

    fprintf(outputFile, "syscall\n");
    fclose(outputFile);
}

int main(int argc, char* argv[])
{
     if (argc < 2) {
         fprintf(stderr, "No file is given");
         return 1;
     }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    Compile(file);
    fclose(file);

    system("as -o test.o test.s");
    system("ld -o test test.o main.o print.o -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2\n");

    printf("\n");
    return 0;
}
