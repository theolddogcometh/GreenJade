/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * fanotify + keyctl thin Linux wrappers (desktop link surface).
 */
#include <errno.h>
#include <stdint.h>
#include <sys/fanotify.h>
#include <unistd.h>

#define NR_fanotify_init 300
#define NR_fanotify_mark 301
#define NR_add_key       248
#define NR_request_key   249
#define NR_keyctl        250

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    long r;
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;

    __asm__ volatile("syscall"
                     : "=a"(r)
                     : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8),
                       "r"(r9)
                     : "rcx", "r11", "memory");
    return r;
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

int
fanotify_init(unsigned uFlags, unsigned uEventFflags)
{
    return (int)sys_ret(sys6(NR_fanotify_init, (long)uFlags, (long)uEventFflags,
                             0, 0, 0, 0));
}

int
fanotify_mark(int nFanotifyFd, unsigned uFlags, uint64_t u64Mask, int nDfd,
              const char *szPathname)
{
    return (int)sys_ret(sys6(NR_fanotify_mark, nFanotifyFd, (long)uFlags,
                             (long)u64Mask, nDfd, (long)(uintptr_t)szPathname,
                             0));
}

/* keyutils-shaped thin wrappers */
long
add_key(const char *szType, const char *szDesc, const void *pPayload,
        size_t cb, int nKeyring)
{
    return sys_ret(sys6(NR_add_key, (long)(uintptr_t)szType,
                        (long)(uintptr_t)szDesc, (long)(uintptr_t)pPayload,
                        (long)cb, nKeyring, 0));
}

long
request_key(const char *szType, const char *szDesc, const char *szCallout,
            int nDestKeyring)
{
    return sys_ret(sys6(NR_request_key, (long)(uintptr_t)szType,
                        (long)(uintptr_t)szDesc, (long)(uintptr_t)szCallout,
                        nDestKeyring, 0, 0));
}

long
keyctl(int nCmd, unsigned long a1, unsigned long a2, unsigned long a3,
       unsigned long a4)
{
    return sys_ret(sys6(NR_keyctl, nCmd, (long)a1, (long)a2, (long)a3, (long)a4,
                        0));
}
