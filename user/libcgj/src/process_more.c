/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * exec* path search, system, popen/pclose, wait3/wait4.
 */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

pid_t
wait4(pid_t pid, int *pStatus, int nOptions, struct rusage *pRusage)
{
    /* Linux wait4 via existing waitpid path; rusage optional best-effort */
    pid_t r;

    (void)pRusage;
    r = waitpid(pid, pStatus, nOptions);
    if (r >= 0 && pRusage != NULL) {
        memset(pRusage, 0, sizeof(*pRusage));
        (void)getrusage(RUSAGE_CHILDREN, pRusage);
    }
    return r;
}

pid_t
wait3(int *pStatus, int nOptions, struct rusage *pRusage)
{
    return wait4(-1, pStatus, nOptions, pRusage);
}

static int
execvpe_path(const char *szFile, char *const aArgv[], char *const aEnvp[])
{
    const char *szPath;
    char aBuf[1024];
    size_t nFile;
    const char *p;
    const char *pColon;
    size_t nDir;
    int nSaved = errno;

    if (szFile == NULL || szFile[0] == '\0') {
        errno = ENOENT;
        return -1;
    }
    if (strchr(szFile, '/') != NULL) {
        return execve(szFile, aArgv, aEnvp);
    }
    szPath = getenv("PATH");
    if (szPath == NULL || szPath[0] == '\0') {
        szPath = "/bin:/usr/bin";
    }
    nFile = strlen(szFile);
    p = szPath;
    for (;;) {
        pColon = strchr(p, ':');
        nDir = pColon ? (size_t)(pColon - p) : strlen(p);
        if (nDir == 0) {
            /* empty PATH component = cwd */
            if (1 + nFile + 1 > sizeof(aBuf)) {
                errno = ENAMETOOLONG;
                return -1;
            }
            aBuf[0] = '.';
            aBuf[1] = '/';
            memcpy(aBuf + 2, szFile, nFile + 1);
        } else {
            if (nDir + 1 + nFile + 1 > sizeof(aBuf)) {
                errno = ENAMETOOLONG;
                return -1;
            }
            memcpy(aBuf, p, nDir);
            aBuf[nDir] = '/';
            memcpy(aBuf + nDir + 1, szFile, nFile + 1);
        }
        execve(aBuf, aArgv, aEnvp);
        if (errno != ENOENT && errno != ENOTDIR && errno != EACCES) {
            return -1;
        }
        nSaved = errno;
        if (pColon == NULL) {
            break;
        }
        p = pColon + 1;
    }
    errno = nSaved;
    return -1;
}

int
execvpe(const char *szFile, char *const aArgv[], char *const aEnvp[])
{
    return execvpe_path(szFile, aArgv, aEnvp != NULL ? aEnvp : environ);
}

int
execvp(const char *szFile, char *const aArgv[])
{
    return execvpe(szFile, aArgv, environ);
}

int
execl(const char *szPath, const char *szArg0, ...)
{
    va_list ap;
    const char *p;
    char *aArgv[64];
    int n = 0;

    aArgv[n++] = (char *)(uintptr_t)szArg0;
    va_start(ap, szArg0);
    while (n < 63) {
        p = va_arg(ap, const char *);
        aArgv[n++] = (char *)(uintptr_t)p;
        if (p == NULL) {
            break;
        }
    }
    va_end(ap);
    if (n == 64 || aArgv[n - 1] != NULL) {
        aArgv[63] = NULL;
    }
    return execv(szPath, aArgv);
}

int
execlp(const char *szFile, const char *szArg0, ...)
{
    va_list ap;
    const char *p;
    char *aArgv[64];
    int n = 0;

    aArgv[n++] = (char *)(uintptr_t)szArg0;
    va_start(ap, szArg0);
    while (n < 63) {
        p = va_arg(ap, const char *);
        aArgv[n++] = (char *)(uintptr_t)p;
        if (p == NULL) {
            break;
        }
    }
    va_end(ap);
    if (n == 64 || aArgv[n - 1] != NULL) {
        aArgv[63] = NULL;
    }
    return execvp(szFile, aArgv);
}

int
execle(const char *szPath, const char *szArg0, ...)
{
    va_list ap;
    const char *p;
    char *aArgv[64];
    char *const *pEnv;
    int n = 0;

    aArgv[n++] = (char *)(uintptr_t)szArg0;
    va_start(ap, szArg0);
    while (n < 63) {
        p = va_arg(ap, const char *);
        aArgv[n++] = (char *)(uintptr_t)p;
        if (p == NULL) {
            break;
        }
    }
    pEnv = va_arg(ap, char *const *);
    va_end(ap);
    if (n == 64 || aArgv[n - 1] != NULL) {
        aArgv[63] = NULL;
    }
    return execve(szPath, aArgv, pEnv);
}

int
system(const char *szCmd)
{
    pid_t pid;
    int nStatus = 0;

    if (szCmd == NULL) {
        /* inquire whether shell is available */
        return 1;
    }
    pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        char *const aArgv[] = { (char *)"sh", (char *)"-c",
                                (char *)(uintptr_t)szCmd, NULL };
        (void)execve("/bin/sh", aArgv, environ);
        (void)execve("/usr/bin/sh", aArgv, environ);
        _exit(127);
    }
    if (waitpid(pid, &nStatus, 0) < 0) {
        return -1;
    }
    return nStatus;
}

/* popen: track child pid on FILE via unused wide flag packing — use list */
struct popen_ent {
    FILE *pF;
    pid_t pid;
    struct popen_ent *pNext;
};

static struct popen_ent *g_pPopen;

FILE *
popen(const char *szCmd, const char *szMode)
{
    int aPipe[2];
    pid_t pid;
    FILE *pF;
    struct popen_ent *pE;
    int fRead;

    if (szCmd == NULL || szMode == NULL) {
        errno = EINVAL;
        return NULL;
    }
    fRead = (szMode[0] == 'r');
    if (!fRead && szMode[0] != 'w') {
        errno = EINVAL;
        return NULL;
    }
    if (pipe(aPipe) != 0) {
        return NULL;
    }
    pid = fork();
    if (pid < 0) {
        (void)close(aPipe[0]);
        (void)close(aPipe[1]);
        return NULL;
    }
    if (pid == 0) {
        char *const aArgv[] = { (char *)"sh", (char *)"-c",
                                (char *)(uintptr_t)szCmd, NULL };
        if (fRead) {
            (void)close(aPipe[0]);
            if (aPipe[1] != 1) {
                (void)dup2(aPipe[1], 1);
                (void)close(aPipe[1]);
            }
        } else {
            (void)close(aPipe[1]);
            if (aPipe[0] != 0) {
                (void)dup2(aPipe[0], 0);
                (void)close(aPipe[0]);
            }
        }
        (void)execve("/bin/sh", aArgv, environ);
        (void)execve("/usr/bin/sh", aArgv, environ);
        _exit(127);
    }
    if (fRead) {
        (void)close(aPipe[1]);
        pF = fdopen(aPipe[0], "r");
    } else {
        (void)close(aPipe[0]);
        pF = fdopen(aPipe[1], "w");
    }
    if (pF == NULL) {
        (void)close(fRead ? aPipe[0] : aPipe[1]);
        (void)kill(pid, 9);
        (void)waitpid(pid, NULL, 0);
        return NULL;
    }
    pE = (struct popen_ent *)malloc(sizeof(*pE));
    if (pE == NULL) {
        (void)fclose(pF);
        (void)kill(pid, 9);
        (void)waitpid(pid, NULL, 0);
        return NULL;
    }
    pE->pF = pF;
    pE->pid = pid;
    pE->pNext = g_pPopen;
    g_pPopen = pE;
    return pF;
}

int
pclose(FILE *pF)
{
    struct popen_ent **pp;
    struct popen_ent *pE;
    int nStatus = 0;
    pid_t r;

    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (pp = &g_pPopen; *pp != NULL; pp = &(*pp)->pNext) {
        if ((*pp)->pF == pF) {
            break;
        }
    }
    if (*pp == NULL) {
        errno = ECHILD;
        return -1;
    }
    pE = *pp;
    *pp = pE->pNext;
    (void)fclose(pF);
    r = waitpid(pE->pid, &nStatus, 0);
    free(pE);
    if (r < 0) {
        return -1;
    }
    return nStatus;
}
