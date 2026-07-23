/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * glibc-shaped __libc_start_main + __errno_location (clean-room).
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
    int code;

    (void)pfnRtldFini;
    (void)pStackEnd;

    _libcgj_stdio_init();
    envp = (argv != NULL) ? (argv + argc + 1) : NULL;
    _libcgj_env_init(envp);
    if (argv != NULL && argc > 0 && argv[0] != NULL) {
        _libcgj_set_progname(argv[0]);
    }
    if (pfnInit != NULL) {
        pfnInit();
    }
    code = pfnMain(argc, argv, envp);
    if (pfnFini != NULL) {
        pfnFini();
    }
    exit(code);
    return code;
}
