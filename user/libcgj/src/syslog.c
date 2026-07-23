/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * syslog stubs: optional stderr when LOG_PERROR; no real syslogd.
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

static int g_nLogMask = LOG_UPTO(LOG_DEBUG);
static int g_nOption;
static char g_aIdent[32];

void
openlog(const char *szIdent, int nOption, int nFacility)
{
    (void)nFacility;
    g_nOption = nOption;
    g_aIdent[0] = '\0';
    if (szIdent != NULL) {
        size_t n = 0;

        while (szIdent[n] != '\0' && n + 1 < sizeof(g_aIdent)) {
            g_aIdent[n] = szIdent[n];
            n++;
        }
        g_aIdent[n] = '\0';
    }
}

void
closelog(void)
{
    g_aIdent[0] = '\0';
    g_nOption = 0;
}

int
setlogmask(int nMask)
{
    int nOld = g_nLogMask;

    if (nMask != 0) {
        g_nLogMask = nMask;
    }
    return nOld;
}

void
vsyslog(int nPriority, const char *szFmt, va_list ap)
{
    int nPri = nPriority & 0x07;
    char aMsg[256];
    int n;

    if (szFmt == NULL) {
        return;
    }
    if ((g_nLogMask & LOG_MASK(nPri)) == 0) {
        return;
    }
    /* Bring-up: only surface when LOG_PERROR (apps often set it). */
    if ((g_nOption & LOG_PERROR) == 0) {
        return;
    }
    n = 0;
    if (g_aIdent[0] != '\0') {
        size_t i = 0;

        while (g_aIdent[i] != '\0' && n + 2 < (int)sizeof(aMsg)) {
            aMsg[n++] = g_aIdent[i++];
        }
        if (n + 2 < (int)sizeof(aMsg)) {
            aMsg[n++] = ':';
            aMsg[n++] = ' ';
        }
    }
    {
        int r = vsnprintf(aMsg + n, sizeof(aMsg) - (size_t)n, szFmt, ap);

        if (r > 0) {
            n += r;
            if (n >= (int)sizeof(aMsg)) {
                n = (int)sizeof(aMsg) - 1;
            }
        }
    }
    if (n > 0 && aMsg[n - 1] != '\n' && n + 1 < (int)sizeof(aMsg)) {
        aMsg[n++] = '\n';
    }
    aMsg[n] = '\0';
    (void)write(2, aMsg, (size_t)n);
}

void
syslog(int nPriority, const char *szFmt, ...)
{
    va_list ap;

    va_start(ap, szFmt);
    vsyslog(nPriority, szFmt, ap);
    va_end(ap);
}
