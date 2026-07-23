/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * syslog soft fill: LOG_PERROR → stderr; optional /dev/log datagram;
 * LOG_PID / facility tags. No real syslogd required.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

static int g_nLogMask = LOG_UPTO(LOG_DEBUG);
static int g_nOption;
static int g_nFacility = LOG_USER;
static int g_nLogFd = -1;
static char g_aIdent[64];

static const char *const s_aPri[] = {
    "emerg", "alert", "crit", "err", "warning", "notice", "info", "debug"
};

void
openlog(const char *szIdent, int nOption, int nFacility)
{
    g_nOption = nOption;
    if (nFacility != 0) {
        g_nFacility = nFacility & ~0x07;
    }
    g_aIdent[0] = '\0';
    if (szIdent != NULL) {
        size_t n = 0;

        while (szIdent[n] != '\0' && n + 1 < sizeof(g_aIdent)) {
            g_aIdent[n] = szIdent[n];
            n++;
        }
        g_aIdent[n] = '\0';
    }
    if (g_nLogFd >= 0) {
        (void)close(g_nLogFd);
        g_nLogFd = -1;
    }
    if ((nOption & LOG_NDELAY) != 0) {
        g_nLogFd = open("/dev/log", O_WRONLY | O_CLOEXEC | O_NONBLOCK);
        if (g_nLogFd < 0) {
            g_nLogFd = open("/dev/log", O_WRONLY | O_CLOEXEC);
        }
    }
}

void
closelog(void)
{
    if (g_nLogFd >= 0) {
        (void)close(g_nLogFd);
        g_nLogFd = -1;
    }
    g_aIdent[0] = '\0';
    g_nOption = 0;
    g_nFacility = LOG_USER;
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

static int
ensure_log_fd(void)
{
    if (g_nLogFd >= 0) {
        return g_nLogFd;
    }
    g_nLogFd = open("/dev/log", O_WRONLY | O_CLOEXEC | O_NONBLOCK);
    if (g_nLogFd < 0) {
        g_nLogFd = open("/dev/log", O_WRONLY | O_CLOEXEC);
    }
    return g_nLogFd;
}

void
vsyslog(int nPriority, const char *szFmt, va_list ap)
{
    int nPri = nPriority & 0x07;
    int nFac = nPriority & ~0x07;
    char aMsg[512];
    char aBody[384];
    int n = 0;
    int nBody;
    int fPerror = (g_nOption & LOG_PERROR) != 0;
    int fTryLog = 1;

    if (szFmt == NULL) {
        return;
    }
    if ((g_nLogMask & LOG_MASK(nPri)) == 0) {
        return;
    }
    if (nFac == 0) {
        nFac = g_nFacility;
    }

    nBody = vsnprintf(aBody, sizeof(aBody), szFmt, ap);
    if (nBody < 0) {
        return;
    }
    if (nBody >= (int)sizeof(aBody)) {
        nBody = (int)sizeof(aBody) - 1;
    }

    /* RFC5424-ish soft prefix: <pri>ident[pid]: msg */
    n = snprintf(aMsg, sizeof(aMsg), "<%d>", nFac | nPri);
    if (n < 0) {
        n = 0;
    }
    if (g_aIdent[0] != '\0') {
        int r = snprintf(aMsg + n, sizeof(aMsg) - (size_t)n, "%s", g_aIdent);

        if (r > 0) {
            n += r;
            if (n >= (int)sizeof(aMsg)) {
                n = (int)sizeof(aMsg) - 1;
            }
        }
    }
    if ((g_nOption & LOG_PID) != 0) {
        int r = snprintf(aMsg + n, sizeof(aMsg) - (size_t)n, "[%d]",
                         (int)getpid());

        if (r > 0) {
            n += r;
            if (n >= (int)sizeof(aMsg)) {
                n = (int)sizeof(aMsg) - 1;
            }
        }
    }
    if (g_aIdent[0] != '\0' || (g_nOption & LOG_PID) != 0) {
        if (n + 2 < (int)sizeof(aMsg)) {
            aMsg[n++] = ':';
            aMsg[n++] = ' ';
        }
    }
    {
        size_t cbLeft = sizeof(aMsg) - (size_t)n;
        size_t cbCopy = (size_t)nBody;

        if (cbCopy >= cbLeft) {
            cbCopy = cbLeft > 0 ? cbLeft - 1 : 0;
        }
        if (cbCopy > 0) {
            memcpy(aMsg + n, aBody, cbCopy);
            n += (int)cbCopy;
        }
    }
    if (n > 0 && aMsg[n - 1] != '\n' && n + 1 < (int)sizeof(aMsg)) {
        aMsg[n++] = '\n';
    }
    aMsg[n] = '\0';

    /* Bring-up default: only surface without LOG_PERROR if /dev/log works. */
    if (!fPerror) {
        int nFd = ensure_log_fd();

        if (nFd < 0) {
            return;
        }
        if (write(nFd, aMsg, (size_t)n) < 0) {
            if ((g_nOption & LOG_CONS) != 0) {
                int nC = open("/dev/console", O_WRONLY | O_CLOEXEC);

                if (nC >= 0) {
                    (void)write(nC, aMsg, (size_t)n);
                    (void)close(nC);
                }
            }
        }
        (void)fTryLog;
        return;
    }

    /* LOG_PERROR path: human-readable stderr line with priority tag. */
    {
        char aErr[512];
        int nE = 0;
        const char *szPri =
            (nPri >= 0 && nPri <= 7) ? s_aPri[nPri] : "info";

        if (g_aIdent[0] != '\0') {
            int r =
                snprintf(aErr + nE, sizeof(aErr) - (size_t)nE, "%s: ", g_aIdent);

            if (r > 0) {
                nE += r;
            }
        }
        {
            int r = snprintf(aErr + nE, sizeof(aErr) - (size_t)nE, "<%s> ",
                             szPri);

            if (r > 0) {
                nE += r;
            }
        }
        {
            size_t cbLeft = sizeof(aErr) - (size_t)nE;
            size_t cbCopy = (size_t)nBody;

            if (cbCopy >= cbLeft) {
                cbCopy = cbLeft > 0 ? cbLeft - 1 : 0;
            }
            if (cbCopy > 0) {
                memcpy(aErr + nE, aBody, cbCopy);
                nE += (int)cbCopy;
            }
        }
        if (nE > 0 && aErr[nE - 1] != '\n' && nE + 1 < (int)sizeof(aErr)) {
            aErr[nE++] = '\n';
        }
        aErr[nE] = '\0';
        (void)write(2, aErr, (size_t)nE);
    }

    /* Also try /dev/log when open. */
    {
        int nFd = ensure_log_fd();

        if (nFd >= 0) {
            (void)write(nFd, aMsg, (size_t)n);
        }
    }
}

void
syslog(int nPriority, const char *szFmt, ...)
{
    va_list ap;

    va_start(ap, szFmt);
    vsyslog(nPriority, szFmt, ap);
    va_end(ap);
}
