/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * exec* path search, system, popen/pclose, wait3/wait4.
 * Soft deepen: real wait4 rusage, POSIX-ish system signals, EINTR waits.
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

#define NR_wait4 61

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile("syscall"
                     : "=a"(ret)
                     : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8),
                       "r"(r9)
                     : "rcx", "r11", "memory");
    return ret;
}

static long
sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

pid_t
wait4(pid_t pid, int *pStatus, int nOptions, struct rusage *pRusage)
{
    /* Real wait4 so rusage is for the waited child, not RUSAGE_CHILDREN. */
    return (pid_t)sys_ret(sys6(NR_wait4, (long)pid, (long)(uintptr_t)pStatus,
                               (long)nOptions, (long)(uintptr_t)pRusage, 0, 0));
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
    int fSawEacces = 0;

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
        /* Soft PATH search: keep scanning on common "not here" errors. */
        if (errno == EACCES) {
            fSawEacces = 1;
            nSaved = EACCES;
        } else if (errno != ENOENT && errno != ENOTDIR && errno != ELOOP &&
                   errno != ESTALE && errno != ENODEV && errno != ETIMEDOUT) {
            return -1;
        } else {
            nSaved = errno;
        }
        if (pColon == NULL) {
            break;
        }
        p = pColon + 1;
    }
    /* Prefer EACCES if any path was denied over final ENOENT. */
    errno = fSawEacces ? EACCES : nSaved;
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
    char *aArgv[128];
    int n = 0;

    aArgv[n++] = (char *)(uintptr_t)szArg0;
    va_start(ap, szArg0);
    while (n < 127) {
        p = va_arg(ap, const char *);
        aArgv[n++] = (char *)(uintptr_t)p;
        if (p == NULL) {
            break;
        }
    }
    va_end(ap);
    if (n == 128 || aArgv[n - 1] != NULL) {
        aArgv[127] = NULL;
    }
    return execv(szPath, aArgv);
}

int
execlp(const char *szFile, const char *szArg0, ...)
{
    va_list ap;
    const char *p;
    char *aArgv[128];
    int n = 0;

    aArgv[n++] = (char *)(uintptr_t)szArg0;
    va_start(ap, szArg0);
    while (n < 127) {
        p = va_arg(ap, const char *);
        aArgv[n++] = (char *)(uintptr_t)p;
        if (p == NULL) {
            break;
        }
    }
    va_end(ap);
    if (n == 128 || aArgv[n - 1] != NULL) {
        aArgv[127] = NULL;
    }
    return execvp(szFile, aArgv);
}

int
execle(const char *szPath, const char *szArg0, ...)
{
    va_list ap;
    const char *p;
    char *aArgv[128];
    char *const *pEnv;
    int n = 0;

    aArgv[n++] = (char *)(uintptr_t)szArg0;
    va_start(ap, szArg0);
    while (n < 127) {
        p = va_arg(ap, const char *);
        aArgv[n++] = (char *)(uintptr_t)p;
        if (p == NULL) {
            break;
        }
    }
    pEnv = va_arg(ap, char *const *);
    va_end(ap);
    if (n == 128 || aArgv[n - 1] != NULL) {
        aArgv[127] = NULL;
    }
    return execve(szPath, aArgv, pEnv);
}

int
system(const char *szCmd)
{
    pid_t pid;
    int nStatus = 0;
    int nRet;
    struct sigaction saIgn, saInt, saQuit;
    sigset_t setBlk, setOld;

    if (szCmd == NULL) {
        /* Inquire whether a shell is available (soft: path presence). */
        if (access("/bin/sh", X_OK) == 0 || access("/usr/bin/sh", X_OK) == 0) {
            return 1;
        }
        return 0;
    }

    /* POSIX-shaped: ignore INT/QUIT in parent, block CHLD around wait. */
    memset(&saIgn, 0, sizeof(saIgn));
    saIgn.sa_handler = SIG_IGN;
    (void)sigemptyset(&saIgn.sa_mask);
    saIgn.sa_flags = 0;
    (void)sigaction(SIGINT, &saIgn, &saInt);
    (void)sigaction(SIGQUIT, &saIgn, &saQuit);
    (void)sigemptyset(&setBlk);
    (void)sigaddset(&setBlk, SIGCHLD);
    (void)sigprocmask(SIG_BLOCK, &setBlk, &setOld);

    pid = fork();
    if (pid < 0) {
        nRet = -1;
        goto restore;
    }
    if (pid == 0) {
        char *const aArgv[] = { (char *)"sh", (char *)"-c",
                                (char *)(uintptr_t)szCmd, NULL };

        (void)sigaction(SIGINT, &saInt, NULL);
        (void)sigaction(SIGQUIT, &saQuit, NULL);
        (void)sigprocmask(SIG_SETMASK, &setOld, NULL);
        (void)execve("/bin/sh", aArgv, environ);
        (void)execve("/usr/bin/sh", aArgv, environ);
        _exit(127);
    }
    for (;;) {
        if (waitpid(pid, &nStatus, 0) >= 0) {
            nRet = nStatus;
            break;
        }
        if (errno != EINTR) {
            nRet = -1;
            break;
        }
    }

restore:
    (void)sigaction(SIGINT, &saInt, NULL);
    (void)sigaction(SIGQUIT, &saQuit, NULL);
    (void)sigprocmask(SIG_SETMASK, &setOld, NULL);
    return nRet;
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
    int fCloexec = 0;
    const char *pM;
    int nPipeFlags = 0;

    if (szCmd == NULL || szMode == NULL || szMode[0] == '\0') {
        errno = EINVAL;
        return NULL;
    }
    fRead = (szMode[0] == 'r');
    if (!fRead && szMode[0] != 'w') {
        errno = EINVAL;
        return NULL;
    }
    for (pM = szMode + 1; *pM != '\0'; pM++) {
        if (*pM == 'e') {
            fCloexec = 1;
        }
        /* soft: ignore unknown mode letters (glibc accepts extras loosely) */
    }
#ifdef O_CLOEXEC
    if (fCloexec) {
        nPipeFlags = O_CLOEXEC;
    }
#endif
    if (nPipeFlags != 0) {
        if (pipe2(aPipe, nPipeFlags) != 0) {
            /* soft fallback if pipe2 unavailable */
            if (pipe(aPipe) != 0) {
                return NULL;
            }
            if (fCloexec) {
                (void)fcntl(aPipe[0], F_SETFD, FD_CLOEXEC);
                (void)fcntl(aPipe[1], F_SETFD, FD_CLOEXEC);
            }
        }
    } else if (pipe(aPipe) != 0) {
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
        /* Drop CLOEXEC on the end we keep so the shell inherits it. */
        if (fRead) {
            (void)close(aPipe[0]);
            if (aPipe[1] != 1) {
                (void)dup2(aPipe[1], 1);
                (void)close(aPipe[1]);
            } else {
                (void)fcntl(aPipe[1], F_SETFD, 0);
            }
        } else {
            (void)close(aPipe[1]);
            if (aPipe[0] != 0) {
                (void)dup2(aPipe[0], 0);
                (void)close(aPipe[0]);
            } else {
                (void)fcntl(aPipe[0], F_SETFD, 0);
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
        while (waitpid(pid, NULL, 0) < 0 && errno == EINTR) {
        }
        return NULL;
    }
    pE = (struct popen_ent *)malloc(sizeof(*pE));
    if (pE == NULL) {
        (void)fclose(pF);
        (void)kill(pid, 9);
        while (waitpid(pid, NULL, 0) < 0 && errno == EINTR) {
        }
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
    do {
        r = waitpid(pE->pid, &nStatus, 0);
    } while (r < 0 && errno == EINTR);
    free(pE);
    if (r < 0) {
        return -1;
    }
    return nStatus;
}
