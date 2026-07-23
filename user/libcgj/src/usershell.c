/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * getusershell — static shell list bring-up.
 */
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

static const char *const g_aShells[] = {
    "/bin/sh",
    "/bin/bash",
    NULL
};
static int g_iShell = 0;

char *
getusershell(void)
{
    const char *p;

    p = g_aShells[g_iShell];
    if (p == NULL) {
        return NULL;
    }
    g_iShell++;
    return (char *)(uintptr_t)p;
}

void
setusershell(void)
{
    g_iShell = 0;
}

void
endusershell(void)
{
    g_iShell = 0;
}
