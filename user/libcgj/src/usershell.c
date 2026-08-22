/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * getusershell — /etc/shells when present, else static soft list.
 */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ttyent.h>
#include <unistd.h>

#define US_MAX   32
#define US_LINE  128

static const char *const g_aFallback[] = {
    "/bin/sh",
    "/bin/bash",
    "/usr/bin/bash",
    "/bin/dash",
    NULL
};

static char g_aShells[US_MAX][US_LINE];
static int g_cShells;
static int g_iShell;
static int g_fLoaded;
static int g_fUseFallback;

static void
load_shells(void)
{
    FILE *pF;
    char aLine[US_LINE];

    if (g_fLoaded) {
        return;
    }
    g_fLoaded = 1;
    g_cShells = 0;
    g_fUseFallback = 0;
    pF = fopen("/etc/shells", "r");
    if (pF == NULL) {
        g_fUseFallback = 1;
        return;
    }
    while (g_cShells < US_MAX &&
           fgets(aLine, (int)sizeof(aLine), pF) != NULL) {
        size_t n;
        char *p = aLine;

        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0' || *p == '\n' || *p == '#') {
            continue;
        }
        n = strlen(p);
        while (n > 0 && (p[n - 1] == '\n' || p[n - 1] == '\r' ||
                         p[n - 1] == ' ' || p[n - 1] == '\t')) {
            p[--n] = '\0';
        }
        if (n == 0 || n >= US_LINE) {
            continue;
        }
        /* Only absolute paths are valid shells. */
        if (p[0] != '/') {
            continue;
        }
        memcpy(g_aShells[g_cShells], p, n + 1);
        g_cShells++;
    }
    (void)fclose(pF);
    if (g_cShells == 0) {
        g_fUseFallback = 1;
    }
}

char *
getusershell(void)
{
    load_shells();
    if (g_fUseFallback) {
        const char *p = g_aFallback[g_iShell];

        if (p == NULL) {
            return NULL;
        }
        g_iShell++;
        return (char *)(uintptr_t)p;
    }
    if (g_iShell >= g_cShells) {
        return NULL;
    }
    return g_aShells[g_iShell++];
}

void
setusershell(void)
{
    g_iShell = 0;
    /* Re-read /etc/shells on next getusershell. */
    g_fLoaded = 0;
}

void
endusershell(void)
{
    g_iShell = 0;
    g_fLoaded = 0;
    g_cShells = 0;
    g_fUseFallback = 0;
}

static struct ttyent g_tty;
static char g_aTyGetty[] = "/sbin/getty";
static char g_aTyType[] = "linux";
static int g_fTtyOpen;
static int g_iTty;

/* Loginrec ut_line names (no /dev prefix). Soft table, not /etc/ttys. */
static const char *const g_aTyNames[] = {
    "console",
    "tty1", "tty2", "tty3", "tty4", "tty5", "tty6",
    "ttyS0", "ttyS1",
    "pts/0", "pts/1", "pts/2", "pts/3",
    "pts/4", "pts/5", "pts/6", "pts/7",
    "pts/8", "pts/9", "pts/10", "pts/11",
    "pts/12", "pts/13", "pts/14", "pts/15",
    NULL
};

int
setttyent(void)
{
    g_fTtyOpen = 1;
    g_iTty = 0;
    return 1;
}

int
endttyent(void)
{
    g_fTtyOpen = 0;
    g_iTty = 0;
    return 1;
}

struct ttyent *
getttyent(void)
{
    if (!g_fTtyOpen) {
        (void)setttyent();
    }
    if (g_aTyNames[g_iTty] == NULL) {
        return NULL;
    }
    g_tty.ty_name = (char *)(uintptr_t)g_aTyNames[g_iTty];
    g_tty.ty_getty = g_aTyGetty;
    g_tty.ty_type = g_aTyType;
    g_tty.ty_status = TTY_ON | TTY_SECURE;
    g_tty.ty_window = NULL;
    g_tty.ty_comment = NULL;
    g_iTty++;
    return &g_tty;
}

struct ttyent *
getttynam(const char *szName)
{
    struct ttyent *p;

    if (szName == NULL) {
        return NULL;
    }
    (void)setttyent();
    while ((p = getttyent()) != NULL) {
        if (strcmp(p->ty_name, szName) == 0) {
            (void)endttyent();
            return p;
        }
    }
    (void)endttyent();
    return NULL;
}

int
ttyslot(void)
{
    return 1;
}
