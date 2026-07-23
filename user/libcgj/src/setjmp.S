/* SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * x86_64 SysV setjmp / longjmp (no signal mask; freestanding).
 */
	.text
	.globl setjmp
	.type setjmp, @function
setjmp:
	/* rdi = jmp_buf */
	movq %rbx, 0(%rdi)
	movq %rbp, 8(%rdi)
	movq %r12, 16(%rdi)
	movq %r13, 24(%rdi)
	movq %r14, 32(%rdi)
	movq %r15, 40(%rdi)
	/* save return address and caller rsp */
	leaq 8(%rsp), %rax
	movq %rax, 48(%rdi)
	movq (%rsp), %rax
	movq %rax, 56(%rdi)
	xorl %eax, %eax
	ret
	.size setjmp, .-setjmp

	.globl longjmp
	.type longjmp, @function
longjmp:
	/* rdi = jmp_buf, esi = val */
	movl %esi, %eax
	testl %eax, %eax
	jne 1f
	movl $1, %eax
1:
	movq 0(%rdi), %rbx
	movq 8(%rdi), %rbp
	movq 16(%rdi), %r12
	movq 24(%rdi), %r13
	movq 32(%rdi), %r14
	movq 40(%rdi), %r15
	movq 48(%rdi), %rsp
	jmpq *56(%rdi)
	.size longjmp, .-longjmp

	.globl sigsetjmp
	.type sigsetjmp, @function
sigsetjmp:
	/* ignore savemask; same as setjmp for bring-up */
	jmp setjmp
	.size sigsetjmp, .-sigsetjmp

	.globl siglongjmp
	.type siglongjmp, @function
siglongjmp:
	jmp longjmp
	.size siglongjmp, .-siglongjmp
