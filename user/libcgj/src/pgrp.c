/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Process-group / session helpers + tcgetpgrp / tcsetpgrp / tcgetsid.
 */
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <termios.h>
#include <unistd.h>

#define NR_setpgid 109
#define NR_getpgrp 111
#define NR_getpgid 121
#define NR_getsid  124
#define NR_chroot  161

#ifndef TIOCGPGRP
#define TIOCGPGRP 0x540F
#endif
#ifndef TIOCSPGRP
#define TIOCSPGRP 0x5410
#endif
#ifndef TIOCGSID
#define TIOCGSID 0x5429
#endif

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
getpgid(pid_t pid)
{
    return (pid_t)sys_ret(sys6(NR_getpgid, (long)pid, 0, 0, 0, 0, 0));
}

pid_t
getpgrp(void)
{
    long r;

    r = sys6(NR_getpgrp, 0, 0, 0, 0, 0, 0);
    if (r < 0 && r > -4096) {
        return getpgid(0);
    }
    return (pid_t)r;
}

int
setpgid(pid_t pid, pid_t pgid)
{
    return (int)sys_ret(sys6(NR_setpgid, (long)pid, (long)pgid, 0, 0, 0, 0));
}

int
setpgrp(void)
{
    return setpgid(0, 0);
}

pid_t
getsid(pid_t pid)
{
    return (pid_t)sys_ret(sys6(NR_getsid, (long)pid, 0, 0, 0, 0, 0));
}

int
chroot(const char *szPath)
{
    if (szPath == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_chroot, (long)(uintptr_t)szPath, 0, 0, 0, 0, 0));
}

pid_t
tcgetpgrp(int nFd)
{
    pid_t pgid = 0;
    int n;

    n = ioctl(nFd, TIOCGPGRP, &pgid);
    if (n < 0) {
        return (pid_t)-1;
    }
    return pgid;
}

int
tcsetpgrp(int nFd, pid_t pgrp)
{
    return ioctl(nFd, TIOCSPGRP, &pgrp);
}

pid_t
tcgetsid(int nFd)
{
    pid_t sid = 0;
    int n;

    n = ioctl(nFd, TIOCGSID, &sid);
    if (n < 0) {
        return (pid_t)-1;
    }
    return sid;
}

int
getdomainname(char *szName, size_t cb)
{
    struct utsname un;
    size_t n;

    if (szName == NULL || cb == 0) {
        errno = EINVAL;
        return -1;
    }
    if (uname(&un) != 0) {
        return -1;
    }
    n = strlen(un.domainname);
    if (n + 1 > cb) {
        errno = ENAMETOOLONG;
        return -1;
    }
    memcpy(szName, un.domainname, n + 1);
    return 0;
}

int
setdomainname(const char *szName, size_t cb)
{
    (void)szName;
    (void)cb;
    errno = EPERM;
    return -1;
}
