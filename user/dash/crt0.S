/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Naked SysV x86_64 _start for vendored dash. Do not use libcgj crt0.c:
 * a C _start pushes callee-saves and then reads (%rsp) as argc — that is
 * leftover kernel rbx after sysretq, not the kernel-built argv block.
 *
 * On entry: rsp -> [argc][argv…][NULL][envp…][NULL][auxv…]
 */
    .text
    .globl _start
    .type  _start, @function
_start:
    xorl    %ebp, %ebp
    movq    (%rsp), %rdi            /* argc */
    leaq    8(%rsp), %rsi           /* argv */
    movq    %rdi, %rax
    leaq    16(%rsp,%rax,8), %rdx   /* envp = argv + argc + 1 */
    movq    %rdx, environ(%rip)
    pushq   %rdi
    pushq   %rsi
    pushq   %rdx
    call    _libcgj_stdio_init
    movq    (%rsp), %rdi            /* envp */
    call    _libcgj_env_init
    movq    8(%rsp), %rsi           /* argv */
    movq    (%rsi), %rdi            /* argv[0] */
    testq   %rdi, %rdi
    jz      1f
    call    _libcgj_set_progname
1:
    popq    %rdx                    /* envp */
    popq    %rsi                    /* argv */
    popq    %rdi                    /* argc */
    call    main
    movl    %eax, %edi
    call    exit
    hlt
    .size _start, .-_start

    .section .note.GNU-stack,"",@progbits
