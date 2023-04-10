/***************************************************************************
 * Dr. Evil's Insidious Bomb, Version 1.1
 * Copyright 2011, Dr. Evil Incorporated. All rights reserved.
 *
 * LICENSE:
 *
 * Dr. Evil Incorporated (the PERPETRATOR) hereby grants you (the
 * VICTIM) explicit permission to use this bomb (the BOMB).  This is a
 * time limited license, which expires on the death of the VICTIM.
 * The PERPETRATOR takes no responsibility for damage, frustration,
 * insanity, bug-eyes, carpal-tunnel syndrome, loss of sleep, or other
 * harm to the VICTIM.  Unless the PERPETRATOR wants to take credit,
 * that is.  The VICTIM may not distribute this bomb source code to
 * any enemies of the PERPETRATOR.  No VICTIM may debug,
 * reverse-engineer, run "strings" on, decompile, decrypt, or use any
 * other technique to gain knowledge of and defuse the BOMB.  BOMB
 * proof clothing may not be worn when handling this program.  The
 * PERPETRATOR will not apologize for the PERPETRATOR's poor sense of
 * humor.  This license is null and void where the BOMB is prohibited
 * by law.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#include "phases.h"

/*
 * Note to self: Remember to erase this file so my victims will have no
 * idea what is going on, and so they will all blow up in a
 * spectaculary fiendish explosion. -- Dr. Evil
 */

FILE* infile;
char (*input_strings)[50];
char num_input_strings;

void sig_handler(int) {
    (void)puts("So you think you can stop the bomb with ctrl-c, do you?");
    (void)sleep(3);
    (void)printf("Well..."); // 寄存器有两个参数？
    (void)fflush(stdout);
    (void)sleep(1);
    (void)puts("OK. :-)");
    exit(0x10);
}

void initialize_bomb() {
    signal(SIGINT, sig_handler);
}

int blank_line(char* rdi) {
    char* rbx = rdi;

    while (*rbx != '\0') {
        // __ctype_b_loc@plt
        rbx++;
        //?
        if (*rbx == ' ') {
            return 0;
        }
    }
    return 1;
}

char* skip() {
    // num = 0x50 * num_input_strings;
    long num = num_input_strings;
    num += 4 * num;
    num <<= 0x4;
    char* rax = fgets(input_strings + num, 0x50, infile);
    if (rax == NULL) {
        return rax;
    }
    rax = blank_line(rax);
    if (rax != 0) {
        skip();
    }
    return rax;
}

char* read_line() {
    char* rax = skip();
    if (rax == NULL) {
        rax = stdin;
        if (rax == infile) { // TODO:
            (void)puts("Error: Premature EOF on stdin");
            exit(8);
        }
        rax = getenv("GRADE_BOMB");
        if (rax != NULL) {
            exit(0);
        }
        infile = stdin;
        rax = skip();
        if (rax == NULL) { // TODO:
            (void)puts("Error: Premature EOF on stdin");
            exit(0);
        }
    }

    char* tmp = input_strings + ((4 * num_input_strings + num_input_strings) << 4);
    int len = strlen(tmp);
    if (len > 0x4e) {
        (void)puts("Error: Input line too long");
        rax = num_input_strings;
        int rdx = rax + 1;
        num_input_strings = rdx;
        rax = rax - 0x50;
        long long rdi = 0x636e7572742a2a2a;
        input_strings = (char*)rdi;
        rdi = 0x2a2a2a64657461;
        // input_strings + 1 = (char*)rdi;
        explode_bomb();
    }
    num_input_strings++;
}

int string_length(char* rdi) {
    if (rdi == NULL) {
        return 0;
    }
    char* rdx = rdi;
    int rax;
    do {
        rdx++;
        rax = (int)rdx;
        rax = (int)rax - (int)rdi;
    } while (*rdx != "\0");
    return rax;
}

int strings_not_equal(char* rdi, char* rsi) {
    char* rbx = rdi;
    char* rbp = rsi;
    int r12d = string_length(rdi);
    int rax = r12d;
    int edx = 1;
    r12d = string_length(rsi);
    if (r12d != rax) {
        edx = 0;
        goto END;
    }
    rax = *rbx;
    if (rax == 0) {
        edx = 0;
        goto END;
    }
LOOP:
    if (rax == *(rbp + 0)) {
        rbx++;
        rbp++;
        rax = *rbx;
        if (rax != 0) {
            goto LOOP;
        }
        edx = 0;
        goto END;
    }

END:
    rax = edx;
    return rax;
}

void explode_bomb() {
    (void)puts("\nBOOM!!!");
    (void)puts("The bomb has blown up.");
    exit(0x8);
}

// rdi里面数据是个二叉树，fun7实际上是二分查找
int fun7(char* rdi, int rsi) {
    int eax;
    if (rdi == 0x0) {
        eax = 0xffffffff; // -1
        return;
    }
    char edx = *rdi;
    if (rsi > edx) {
        rdi = rdi + 0x8;
        eax = fuc7(rdi, rsi);
        eax += eax;
        return eax;
    }
    eax = 0;
    if (rsi == edx) {
        return eax;
    }
    rdi = rdi + 0x10;
    eax = func(rdi, rsi);
    eax = eax + eax * 1 + 0x1;
    return eax;
}

void secret_phase(void) {
    int edx = 0xa;
    int esi = 0x0;
    long rax = read_line();
    int rdi = rax;
    int rbx = rax;
    rax = strtol(rdi, esi, edx);
    rax--;
    if (0x3e8 <= rax) {
        explode_bomb();
    }
    int rsi = rbx;
    rax = fun7("$", rsi);
    if (rax != 2) {
        explode_bomb();
    }
    (void)puts("Wow! You've defused the secret stage!");
    phase_defused();
}

void phase_defused(void) {
    if (num_input_strings != 6) {
        return;
    }
    int a;        // 0x8(%rsp)
    int b;        // 0xc(%rsp)
    char c[0x50]; // 0x10(%rsp)
    int rax = sscanf(input_strings, "%d %d %s", &a, &b, c);
    if (rax == 3) {
        rax = strings_not_equal(c, "DrEvil");
        if (rax == 0) {
            (void)puts("Curses, you've found the secret phase!");
            (void)puts("But finding it and solving it are quite different...");
            rax = 0;
            secret_phase();
        }
    }
    (void)puts("Congratulations! You've defused tha bomb!");
    return;
}

// 输入字符串，看是否和预设的相同
void phase_1(char* rdi) {
    char* rsi = "Border relations with Canada have never been better.";
    int rax = strings_not_equal(rdi, rsi);
    if (rax != 0) {
        explode_bomb();
    }
    return;
}

void read_six_numbers(char* rdi, int* rdx, int* rcx, int* r8, int* r9, int* rsp1, int* rsp2) {
    int rax = sscanf(rdi, "%d %d %d %d %d %d", rdx, rcx, r8, r9, rsp1, rsp2);
    if (rax <= 5) {
        explode_bomb();
    }
}

// 输入6个数字，第一个是1，后续都是前面的数值的两倍
void phase_2(char* rdi) {
    // int rdx;
    // int rcx;
    // int r8;
    // int r9;
    // int rsp1; // 0x4(%rsp)
    // int rsp2; // (%rsp)
    int rsp[6];
    read_six_numbers(rdi, &rsp[0], &rsp[1], &rsp[2], &rsp[3], &rsp[4], &rsp[5]);
    int rbx = rsp[0];
    if (rbx != 1) {
        explode_bomb();
    }
    for (int i = 1; i < 6; i++) {
        if (rsp[i] != 2 * rbx) {
            explode_bomb();
        }
    }
    return;
}

// 输入两个数字，通过第一个数是否在表里匹配第二个数
void phase_3(char* rdi) {
    int rsp[2];
    int rax = sscanf(rdi, "%d %d", &rsp[0], &rsp[1]);
    if (rax <= 1) {
        explode_bomb();
    }
    if (rsp[0] > 0x7) {
        explode_bomb();
    }
    rax = rsp[0];

    switch (rax) {
    case 0:
        rax = 0xcf;
        break;
    case 1:
        rax = 0x137;
        break;
    case 2:
        rax = 0x2c3;
        break;
    case 3:
        rax = 0x100;
        break;
    case 4:
        rax = 0x185;
        break;
    case 5:
        rax = 0xce;
        break;
    case 6:
        rax = 0x2aa;
        break;
    case 7:
        rax = 0x147;
        break;
    default:
        break;
    }
    if (rax != rsp[1]) {
        explode_bomb();
    }
    return;
}

int func4(int rdi, int rsi, int rdx) {
    int rax = rdx - rsi;   // 0xe - 0;
    int ecx = rax >> 0x1f; // 0
    rax += ecx;
    ecx = rax + 1 * rsi;
    rax = 0;
    if (rdi > ecx) {
        rdx = ecx - 1;
        rax = func4(rdi, rsi, rdx);
        rax = 1 + rax + 1 * rax;
        rax += rax;
    } else if (rdi < ecx) {
        rsi = ecx + 1;
        func4(rdi, rsi, rdx);
        rax = 1 + rax + 1 * rax;
    }
    return rax;
}

// 输入两个数字，
void phase_4(char* rdi) {
    int rsp[2];
    int rax = sscanf(rdi, "%d %d", &rsp[0], &rsp[1]);
    if (rax != 2) {
        explode_bomb();
    }
    if (rsp[0] > 0xe) {
        explode_bomb();
    }
    rax = func4(rsp[0], 0, 0xe);
    if (rax == 0) {
        if (rsp[1] == 0) {
            return;
        }
    }
    explode_bomb();
}

// 输入六个字符，按位与&后查找字符表"maduiersnfotvbyl"，映射后字符拼接是否等于"flyers"
void phase_5(char* rdi) {
    char* rsp[5];
    char* rbx = rdi;
    long long rax;
    rsp[0x18] = rax;
    rax = 0;
    rax = string_length(rdi);
    if (rax != 6) {
        explode_bomb();
    }
    rax = 0;
LOOP:
    char* ecx = rbx + rax;
    rsp[0] = ecx;
    char rdx = rsp[0];
    rdx = rdx & 0xf;
    char* tmp = "maduiersnfotvbyl";
    //           0123456789ABCDEF
    rdx = *(tmp + rdx);
    *(rsp[2] + rax) = rdx;
    rax++;
    if (rax != 6) {
        goto LOOP;
    }
    rsp[3] = 0;
    char* esi = "flyers";
    //           9FE567
    rdi = rsp[2];
    rax = strings_not_equal(rdi, esi);
    if (rax != 0) {
        explode_bomb();
    }
    return;
}

// node 小大排  2 1 6 5 4 3
// 反转         3 4 5 6 1 2
// 7 - x        4 3 2 1 6 5
void phase_6(char* rdi) {
    char rsp[0x50];
    char* r13 = rsp;
    char* rsi = rsp;
    read_six_numbers(rdi, rsi);
    char* r14 = rsp;
    char r12 = 0;
LOOP: // 将输入复制到栈上
    char* rbp = r13;
    char rax = &r13;
    // 从输入中读取数据
    rax--;
    if (0x5 <= rax) {
        explode_bomb();
    }
    r12++; // 1 2 ...
    if (r12 != 0x6) {
        int ebx = r12;
    LOOP_1: // 判断输入后续是否有等于第一个输入
        rax = ebx;
        rax = rsp[4 * rax];
        if (rax == (0x0 + rbp)) {
            explode_bomb();
        }
        ebx++;
        if (0x5 <= ebx) {
            goto LOOP_1;
        }
        r13 += 4;
        goto LOOP;
    }
    rsi = rsp[0x18];
    rax = r14;
    long rcx = 0x7;
LOOP_2: // 0x7 - x，x上面复制到栈上输入的六个数
    long rdx = rcx;
    rdx -= *rax;
    *rax = rdx;
    rax += 4;
    if (rsi != rax) {
        goto LOOP_2;
    }
    rsi = 0;
    goto LOOP_4;

LOOP_8:
    rdx = rdx[0x8];
    rax++;
    if (rcx != rax) {
        goto LOOP_8;
    }
    goto LOOP_9;

LOOP_3:
    rdx = 0x6032d0;
LOOP_9: // 将输入作为索引将node数据复制8byte到栈上0x20后续内存
    rsp[2 * *rsi + 0x20] = rdx;
    rsi += 4;
    if (rsi == 0x18) {
        goto LOOP_5;
    }

LOOP_4: // 如果输入大于1，将从node复制数据到栈上
    rcx = rsp[*rsi];
    if (0x1 <= rcx) {
        goto LOOP_3;
    }
    rax = 1;
    rdx = 0x6032d0;
    goto LOOP_8;

LOOP_5:
    long rbx = rsp[0x20];
    rax = &rsp[0x28];
    rsi = &rsp[0x50];
    rcx = rbx;

LOOP_6: // 将栈上node地址反转下
    rdx = *rax;
    rcx[0x8] = rdx;
    rax += 0x8;
    if (rsi != rax) {
        rdx = rcx;
        goto LOOP_6;
    }
LOOP_7: // 从栈上node节点取值，如果后面的比前面要大就报错
    rdx[0x8] = 0;
    long ebp = 0x5;
    rax = rbx[0x8];
    rax = *rax;
    if (rax < *rbx) {
        explode_bomb();
    }
    rbx = rbx[0x8];
    rbp--;
    if (rbp != 0) {
        goto LOOP_7;
    }
    return;
}

int main(int argc, char* argv[]) {
    char* input;

    /* Note to self: remember to port this bomb to Windows and put a
     * fantastic GUI on it. */

    /* When run with no arguments, the bomb reads its input lines
     * from standard input. */
    if (argc == 1) {
        infile = stdin;
    }

    /* When run with one argument <file>, the bomb reads from <file>
     * until EOF, and then switches to standard input. Thus, as you
     * defuse each phase, you can add its defusing string to <file> and
     * avoid having to retype it. */
    else if (argc == 2) {
        if (!(infile = fopen(argv[1], "r"))) {
            printf("%s: Error: Couldn't open %s\n", argv[0], argv[1]);
            exit(8);
        }
    }

    /* You can't call the bomb with more than 1 command line argument. */
    else {
        printf("Usage: %s [<input_file>]\n", argv[0]);
        exit(8);
    }

    /* Do all sorts of secret stuff that makes the bomb harder to defuse. */
    initialize_bomb();

    printf("Welcome to my fiendish little bomb. You have 6 phases with\n");
    printf("which to blow yourself up. Have a nice day!\n");

    /* Hmm...  Six phases must be more secure than one phase! */
    input = read_line(); /* Get input                   */
    phase_1(input);      /* Run the phase               */
    phase_defused();     /* Drat!  They figured it out!
                          * Let me know how they did it. */
    printf("Phase 1 defused. How about the next one?\n");

    /* The second phase is harder.  No one will ever figure out
     * how to defuse this... */
    input = read_line();
    phase_2(input);
    phase_defused();
    printf("That's number 2.  Keep going!\n");

    /* I guess this is too easy so far.  Some more complex code will
     * confuse people. */
    input = read_line();
    phase_3(input);
    phase_defused();
    printf("Halfway there!\n");

    /* Oh yeah?  Well, how good is your math?  Try on this saucy problem! */
    input = read_line();
    phase_4(input);
    phase_defused();
    printf("So you got that one.  Try this one.\n");

    /* Round and 'round in memory we go, where we stop, the bomb blows! */
    input = read_line();
    phase_5(input);
    phase_defused();
    printf("Good work!  On to the next...\n");

    /* This phase will never be used, since no one will get past the
     * earlier ones.  But just in case, make this one extra hard. */
    input = read_line();
    phase_6(input);
    phase_defused();

    /* Wow, they got it!  But isn't something... missing?  Perhaps
     * something they overlooked?  Mua ha ha ha ha! */

    return 0;
}
