#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------
 * Vectors
 * ------------------------------------------------------ */

typedef struct Vec
{
    int len;
    int *vdata;
} Vec;

Vec mkVec(int l)
{
    Vec v;
    v.len = l;
    v.vdata = (int *) malloc(l * sizeof(int));
    return v;
}

int vecGet(Vec *v, int idx)
{
    if (idx < 0 || idx >= v->len) return -1;

    return v->vdata[idx];
}

int vecPut(Vec *v, int idx, int val)
{
    if (idx < 0 || idx >= v->len) return -1;

    v->vdata[idx] = val;
    return 0;
}

void vecPrint(Vec *v)
{
    printf("[");
    for (int i = 0; i < v->len; ++i)
    {
        printf("%d", v->vdata[i]);
        if (i < v->len-1) printf(", ");
    }

    printf("]\n");
}

void vecDel(Vec *v)
{
    free(v->vdata);
}


/* ------------------------------------------------------
 * Stack
 * ------------------------------------------------------ */

typedef struct Stack
{
    int sp;
    int sp_max;
    Vec *data;
} Stack;

Stack stack;


void stackInit(int l)
{
    stack.sp = -1;
    stack.sp_max = l - 1;
    stack.data = (Vec *) malloc(l * sizeof(Vec));
}

Vec stackPop()
{
    if (stack.sp < 0)
    {
        fprintf(stderr, "Pop from empty stack!");
        exit(1);
    }

    Vec v = stack.data[stack.sp--];
    return v;
}

int stackPush(Vec v)
{
    if (stack.sp >= stack.sp_max) return -1;

    stack.data[++stack.sp] = v;
    return 0;
}

void stackPrint()
{
    if (stack.sp < 0)
    {
        printf("Empty stack.\n\n");
        return;
    }

    printf("--- Stack ---\n");
    for (int i = stack.sp; i >= 0; --i)
        vecPrint(stack.data + i);

    printf("--- end ---\n\n");
}


void stackClean()
{
    for (int i = stack.sp; i >= 0; ++i)
        vecDel(stack.data+i);
    free(stack.data);
}

/* ------------------------------------------------------
 * Instructions
 * ------------------------------------------------------ */

typedef struct Instr
{
    int opcode, arg1;
} Instr;


int hasArg(int opc)
{
    return opc >= 0 && opc <= 5;
}


Instr parseInstr(const char *ln)
{
    int opc = 0, a1 = 0;
    sscanf(ln, "%d", &opc);
    if (hasArg(opc))
        sscanf(ln, "%d %d", &opc, &a1);

    Instr instr;
    instr.opcode = opc;
    instr.arg1   = a1;

    return instr;
}

void printInstr(Instr instr)
{
    switch(instr.opcode) {
        case 0:
            printf("Load    %d\n", instr.arg1); break;
        case 1:
            printf("Store   %d\n", instr.arg1); break;
        case 2:
            printf("Push    %d\n", instr.arg1); break;
        case 3:
            printf("Label   %d\n", instr.arg1); break;
        case 4:
            printf("JumpIPZ %d\n", instr.arg1); break;
        case 5:
            printf("JumpIP  %d\n", instr.arg1); break;
        case 6:
            printf("Add\n"); break;
        case 7:
            printf("Sub\n"); break;
        case 8:
            printf("Mul\n"); break;
        case 9:
            printf("Div\n"); break;
        case 10:
            printf("Dup\n"); break;
        default:
            printf("Unrecognized opcode");
    }
}


/* ---------------------------------------------------------
 * Processing instructions
 * --------------------------------------------------------- */

typedef struct ProgState
{
    int IP;  // instruction pointer
    int ZF;  // zero flag
    Instr *instrs;
    Vec *ram;
} ProgState;

ProgState progState;

void initProgState(const char *filename)
{
    progState.IP = 0;
    progState.ZF = 0;
    progState.instrs = (Instr *) malloc(256 * sizeof(Instr));
    progState.ram = (Vec *) malloc(512 * sizeof(Vec));

    FILE *fp = NULL;
    if ((fp = fopen(filename, "w")) == NULL)
    {
        perror("Error reading program file");
        exit(2);
    }

    char *line = (char *) malloc(32 * sizeof(char));
    int delim = '\n';
    size_t size = 32;
    Instr tmpInstr;
    int i = 0;
    while (true)
    {
        ssize_t res = getdelim(&line, &size, delim, fp);
        if (res == -1)
        {
            perror("Error reading line from program file.");
            exit(3);
        }
        if (res < 2) break;

        tmpInstr = parseInstr(line);
        progState.instrs[i++] = tmpInstr;
    }

    free(line);
    fclose(fp);
}

void jump(int newIP)
{
    progState.IP = newIP;
}

void jumpZ(int newIP)
{
    if (progState.ZF) jump(newIP);
}


void processLoad(int addr);
void processStore(int addr);
void processPush(Vec val);
void processJump(int tgt);
void processJumpZ(int tgt);
void processAdd();
void processSub();
void processMul();
void processDiv();
void processDup();



/* ---------------------------------------------------------
 * Tests
 * --------------------------------------------------------- */

int main(int argc, char **argv)
{
    printf("%s\n", "Welcome to the Radom VM");

    Vec v = mkVec(3);
    vecPut(&v, 0, 42);
    vecPut(&v, 1, 43);
    vecPut(&v, 2, 44);
    int val = vecGet(&v, 2);
    printf("Vector value[2] = %d\n", val);

    stackInit(128);
    stackPush(v);
    stackPrint();

    Vec v2 = stackPop();
    stackPrint();

    val = vecGet(&v2, 2);
    printf("Vector value[2] = %d\n", val);

    vecPrint(&v2);

    stackClean();
    // Instr instr1 = parseInstr("9");
    // Instr instr2 = parseInstr("3 16");
    // Instr instr3 = parseInstr("4 1623");
    // Instr instr4 = parseInstr("0 12");
    // Instr instr5 = parseInstr("2 232");
    // printInstr(instr1);
    // printInstr(instr2);
    // printInstr(instr3);
    // printInstr(instr4);
    // printInstr(instr5);
    return 0;
}
