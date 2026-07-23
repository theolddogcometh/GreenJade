/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * getauxval — walk AT_ vector after envp on process stack (Linux x86_64).
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/auxv.h>
#include <unistd.h>

/* Captured at _libcgj_env_init / first call via stack walk from environ */
static int g_fAuxInit;
static unsigned long g_aAux[64][2];
static int g_nAux;

static void
auxv_init(void)
{
    char **env;
    unsigned long *p;

    if (g_fAuxInit) {
        return;
    }
    g_fAuxInit = 1;
    env = environ;
    if (env == NULL) {
        return;
    }
    /* walk past envp NULL */
    while (*env != NULL) {
        env++;
    }
    env++;
    p = (unsigned long *)env;
    g_nAux = 0;
    while (g_nAux < 64) {
        unsigned long t = p[0];
        unsigned long v = p[1];

        g_aAux[g_nAux][0] = t;
        g_aAux[g_nAux][1] = v;
        g_nAux++;
        if (t == AT_NULL) {
            break;
        }
        p += 2;
    }
}

unsigned long
getauxval(unsigned long uType)
{
    int i;

    auxv_init();
    for (i = 0; i < g_nAux; i++) {
        if (g_aAux[i][0] == uType) {
            return g_aAux[i][1];
        }
    }
    errno = ENOENT;
    return 0;
}
