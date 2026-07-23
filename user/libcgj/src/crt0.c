/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal CRT for freestanding apps linked with libcgj.
 * Expects Linux x86_64 ABI stack: [argc][argv...][NULL][envp...]
 */
#include <stdint.h>
#include <stdlib.h>

extern int main(int argc, char **argv, char **envp);

void
_start(void)
{
    long *pSp;
    int argc;
    char **argv;
    char **envp;
    int code;

    __asm__ volatile("mov %%rsp, %0" : "=r"(pSp));
    /* Account for whatever alignment; rsp points at argc after kernel entry */
    argc = (int)pSp[0];
    argv = (char **)(pSp + 1);
    envp = argv + argc + 1;
    code = main(argc, argv, envp);
    exit(code);
}
