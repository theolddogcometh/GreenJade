/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal freestanding CRT for apps linked with libcgj.
 * Linux x86_64 process stack: [argc][argv…][NULL][envp…][NULL][auxv…]
 * Soft deepen: seed environ, run libcgj bring-up hooks, then main/exit.
 */
#include <stdint.h>
#include <stdlib.h>

extern int main(int argc, char **argv, char **envp);
extern char **environ;

/* Optional product hooks (weak-friendly: always linked in full libcgj). */
extern void _libcgj_stdio_init(void);
extern void _libcgj_env_init(char **envp);
extern void _libcgj_set_progname(char *szArg0);

void
_start(void)
{
	long *pSp;
	int nArgc;
	char **ppArgv;
	char **ppEnvp;
	int nCode;

	/*
	 * On kernel→user entry, rsp points at argc. Inline asm keeps this
	 * freestanding (no CRT prologue from a hosted main).
	 */
	__asm__ volatile("mov %%rsp, %0" : "=r"(pSp));

	nArgc = (int)pSp[0];
	if (nArgc < 0) {
		nArgc = 0;
	}
	ppArgv = (char **)(void *)(pSp + 1);
	/* argv[argc] is NULL; envp follows. */
	ppEnvp = ppArgv + nArgc + 1;

	/* Seed process environment before any libc call that reads it. */
	environ = ppEnvp;

	_libcgj_stdio_init();
	_libcgj_env_init(ppEnvp);
	if (nArgc > 0 && ppArgv != NULL && ppArgv[0] != NULL) {
		_libcgj_set_progname(ppArgv[0]);
	}

	nCode = main(nArgc, ppArgv, ppEnvp);
	exit(nCode);
}
