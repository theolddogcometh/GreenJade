/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * glibc-shaped __libc_start_main + __errno_location (clean-room).
 *
 * greppable: CGJ_START_SOFT_NULL_MAIN
 * greppable: CGJ_START_SOFT_ENVP
 * greppable: CGJ_START_SOFT_INIT_FINI
 *
 * Soft deepen: reject null main, derive envp past argv NULL, run product
 * stdio/env/progname hooks before optional init, then main → fini → exit.
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

extern void _libcgj_stdio_init(void);
extern void _libcgj_env_init(char **envp);
extern void _libcgj_set_progname(char *szArg0);

int *
__errno_location(void)
{
    return &errno;
}

/*
 * glibc ABI (simplified):
 *   int __libc_start_main(int (*main)(int,char**,char**), int argc,
 *                         char **ubp_av, void (*init)(void),
 *                         void (*fini)(void), void (*rtld_fini)(void),
 *                         void *stack_end);
 */
int
__libc_start_main(int (*pfnMain)(int, char **, char **), int argc, char **argv,
                  void (*pfnInit)(void), void (*pfnFini)(void),
                  void (*pfnRtldFini)(void), void *pStackEnd)
{
    char **envp;
    int nCode;
    int nArgc;

    (void)pfnRtldFini;
    (void)pStackEnd;

    /* greppable: CGJ_START_SOFT_NULL_MAIN */
    if (pfnMain == NULL) {
        exit(127);
        return 127;
    }

    nArgc = argc;
    if (nArgc < 0) {
        nArgc = 0;
    }

    _libcgj_stdio_init();

    /* greppable: CGJ_START_SOFT_ENVP */
    envp = NULL;
    if (argv != NULL) {
        /* Prefer argv[argc] == NULL then envp; else walk to first NULL. */
        if (nArgc >= 0 && argv[nArgc] == NULL) {
            envp = argv + nArgc + 1;
        } else {
            char **pp = argv;

            while (*pp != NULL) {
                pp++;
            }
            envp = pp + 1;
        }
    }

    _libcgj_env_init(envp);
    if (argv != NULL && nArgc > 0 && argv[0] != NULL) {
        _libcgj_set_progname(argv[0]);
    }

    /* greppable: CGJ_START_SOFT_INIT_FINI */
    if (pfnInit != NULL) {
        pfnInit();
    }
    nCode = pfnMain(nArgc, argv, envp);
    if (pfnFini != NULL) {
        pfnFini();
    }
    exit(nCode);
    return nCode;
}
