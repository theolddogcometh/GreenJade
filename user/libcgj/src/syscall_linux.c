/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Linux x86_64 SYSCALL wrappers for libcgj (no host libc).
 */
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sched.h>
#include <signal.h>
#include <sys/file.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <sys/inotify.h>
#include <sys/sendfile.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Linux x86_64 NRs (clean-room public ABI) */
#define NR_read            0
#define NR_write           1
#define NR_open            2
#define NR_close           3
#define NR_pread64        17
#define NR_pwrite64       18
#define NR_stat            4
#define NR_fstat           5
#define NR_lstat           6
#define NR_poll            7
#define NR_ppoll         271
#define NR_sendfile       40
#define NR_timerfd_create 283
#define NR_timerfd_settime 286
#define NR_timerfd_gettime 287
#define NR_signalfd4      282
#define NR_epoll_create    213
#define NR_epoll_create1   291
#define NR_epoll_ctl       233
#define NR_epoll_wait      232
#define NR_epoll_pwait     281
#define NR_lseek           8
#define NR_mmap            9
#define NR_mprotect       10
#define NR_munmap         11
#define NR_brk            12
#define NR_msync          26
#define NR_madvise        28
#define NR_rt_sigaction   13
#define NR_rt_sigprocmask 14
#define NR_ioctl          16
#define NR_access         21
#define NR_pipe           22
#define NR_dup            32
#define NR_dup2           33
#define NR_dup3          292
#define NR_mknod         133
#define NR_mknodat       259
#define NR_pselect6      270
#define NR_pause          34
#define NR_nanosleep      35
#define NR_alarm          37
#define NR_getpid         39
#define NR_fork           57
#define NR_execve         59
#define NR_exit           60
#define NR_wait4          61
#define NR_kill           62
#define NR_uname          63
#define NR_fcntl          72
#define NR_flock          73
#define NR_fsync          74
#define NR_fdatasync      75
#define NR_sync           162
#define NR_syncfs        306
#define NR_faccessat     269
#define NR_faccessat2    439
#define NR_readv          19
#define NR_writev         20
#define NR_getcwd         79
#define NR_chdir          80
#define NR_fchdir         81
#define NR_getitimer      36
#define NR_setitimer      38
#define NR_inotify_init1  294
#define NR_inotify_add_watch 254
#define NR_inotify_rm_watch  255
#define NR_preadv        295
#define NR_pwritev       296
#define NR_mkdir          83
#define NR_rmdir          84
#define NR_creat          85
#define NR_link           86
#define NR_unlink         87
#define NR_symlink        88
#define NR_readlink       89
#define NR_rename         82
#define NR_getuid        102
#define NR_getgid        104
#define NR_setuid        105
#define NR_setgid        106
#define NR_geteuid       107
#define NR_getegid       108
#define NR_setreuid      113
#define NR_setregid      114
#define NR_getgroups     115
#define NR_setgroups     116
#define NR_setresuid     117
#define NR_getresuid     118
#define NR_setresgid     119
#define NR_getresgid     120
#define NR_utimensat     280
#define NR_chmod          90
#define NR_fchmod         91
#define NR_chown          92
#define NR_fchown         93
#define NR_lchown         94
#define NR_umask          95
#define NR_statfs        137
#define NR_fstatfs       138
#define NR_fchownat      260
#define NR_fchmodat      268
#define NR_getppid       110
#define NR_arch_prctl    158
#define NR_gettid        186
#define NR_tkill         200
#define NR_futex         202
#define NR_clock_gettime 228
#define NR_clock_getres  229
#define NR_clock_nanosleep 230
#define NR_timer_create  222
#define NR_timer_settime 223
#define NR_timer_gettime 224
#define NR_timer_getoverrun 225
#define NR_timer_delete  226
#define NR_mq_open       240
#define NR_mq_unlink     241
#define NR_mq_timedsend  242
#define NR_mq_timedreceive 243
#define NR_mq_notify     244
#define NR_mq_getsetattr 245
#define NR_exit_group    231
#define NR_sysinfo        99
#define NR_getrlimit      97
#define NR_getrusage      98
#define NR_setrlimit     160
#define NR_getpriority   140
#define NR_setpriority   141
#define NR_prlimit64     302
#define NR_times         100
#define NR_waitid        247
#define NR_setsid        112
#define NR_openat        257
#define NR_mkdirat       258
#define NR_newfstatat    262
#define NR_unlinkat      263
#define NR_renameat      264
#define NR_linkat        265
#define NR_symlinkat     266
#define NR_readlinkat    267
#define NR_renameat2     316
#define NR_copy_file_range 326
#define NR_pipe2         293
#define NR_memfd_create  319
#define NR_getrandom     318
#define NR_eventfd2      290
#define NR_ftruncate     77
#define NR_fallocate    285
#define NR_getdents64    217
#define NR_socket        41
#define NR_connect       42
#define NR_accept        43
#define NR_sendto        44
#define NR_recvfrom      45
#define NR_sendmsg       46
#define NR_recvmsg       47
#define NR_shutdown      48
#define NR_bind          49
#define NR_listen        50
#define NR_getsockname   51
#define NR_getpeername   52
#define NR_socketpair    53
#define NR_setsockopt    54
#define NR_getsockopt    55
#define NR_accept4      288
#define NR_recvmmsg     299
#define NR_sendmmsg     307
#define NR_splice       275
#define NR_tee          276
#define NR_vmsplice     278
#define NR_shmget        29
#define NR_shmat         30
#define NR_shmctl        31

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8), "r"(r9)
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

ssize_t
read(int nFd, void *pBuf, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_read, nFd, (long)(uintptr_t)pBuf, (long)cb,
                                 0, 0, 0));
}

ssize_t
write(int nFd, const void *pBuf, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_write, nFd, (long)(uintptr_t)pBuf, (long)cb,
                                 0, 0, 0));
}

ssize_t
pread(int nFd, void *pBuf, size_t cb, off_t off)
{
    return (ssize_t)sys_ret(sys6(NR_pread64, nFd, (long)(uintptr_t)pBuf,
                                 (long)cb, (long)off, 0, 0));
}

ssize_t
pwrite(int nFd, const void *pBuf, size_t cb, off_t off)
{
    return (ssize_t)sys_ret(sys6(NR_pwrite64, nFd, (long)(uintptr_t)pBuf,
                                 (long)cb, (long)off, 0, 0));
}

int
close(int nFd)
{
    return (int)sys_ret(sys6(NR_close, nFd, 0, 0, 0, 0, 0));
}

int
fsync(int nFd)
{
    return (int)sys_ret(sys6(NR_fsync, nFd, 0, 0, 0, 0, 0));
}

int
fdatasync(int nFd)
{
    return (int)sys_ret(sys6(NR_fdatasync, nFd, 0, 0, 0, 0, 0));
}

int
sync(void)
{
    return (int)sys_ret(sys6(NR_sync, 0, 0, 0, 0, 0, 0));
}

int
syncfs(int nFd)
{
    return (int)sys_ret(sys6(NR_syncfs, nFd, 0, 0, 0, 0, 0));
}

static long
pathconf_val(int nName)
{
    switch (nName) {
    case _PC_LINK_MAX:
        return 127L;
    case _PC_MAX_CANON:
    case _PC_MAX_INPUT:
        return 255L;
    case _PC_NAME_MAX:
        return 255L;
    case _PC_PATH_MAX:
        return 4096L;
    case _PC_PIPE_BUF:
        return 4096L;
    case _PC_CHOWN_RESTRICTED:
        return 1L;
    case _PC_NO_TRUNC:
        return 1L;
    case _PC_VDISABLE:
        return 0L;
    case _PC_FILESIZEBITS:
        return 64L;
    case _PC_SYMLINK_MAX:
        return 255L;
    case _PC_2_SYMLINKS:
        return 1L;
    case _PC_ALLOC_SIZE_MIN:
    case _PC_REC_MIN_XFER_SIZE:
    case _PC_REC_XFER_ALIGN:
        return 4096L;
    case _PC_REC_INCR_XFER_SIZE:
    case _PC_REC_MAX_XFER_SIZE:
        return 65536L;
    case _PC_SYNC_IO:
    case _PC_ASYNC_IO:
    case _PC_PRIO_IO:
        return -1L; /* not supported → -1 without errno per POSIX optional */
    default:
        errno = EINVAL;
        return -1L;
    }
}

long
pathconf(const char *szPath, int nName)
{
    if (szPath == NULL) {
        errno = EINVAL;
        return -1L;
    }
    /* Bring-up: constants independent of path; still require non-NULL */
    (void)szPath;
    return pathconf_val(nName);
}

long
fpathconf(int nFd, int nName)
{
    if (nFd < 0) {
        errno = EBADF;
        return -1L;
    }
    (void)nFd;
    return pathconf_val(nName);
}

int
faccessat(int nDfd, const char *szPath, int nMode, int nFlags)
{
    long r;

    r = sys6(NR_faccessat2, nDfd, (long)(uintptr_t)szPath, nMode, nFlags, 0, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        /* Fall back: faccessat without flags, or access for AT_FDCWD */
        if (nFlags == 0) {
            r = sys6(NR_faccessat, nDfd, (long)(uintptr_t)szPath, nMode, 0, 0,
                     0);
        } else if (nDfd == AT_FDCWD && (nFlags & AT_EACCESS) != 0) {
            return access(szPath, nMode);
        } else {
            r = sys6(NR_faccessat, nDfd, (long)(uintptr_t)szPath, nMode, 0, 0,
                     0);
        }
    }
    return (int)sys_ret(r);
}

int
euidaccess(const char *szPath, int nMode)
{
    return faccessat(AT_FDCWD, szPath, nMode, AT_EACCESS);
}

int
eaccess(const char *szPath, int nMode)
{
    return euidaccess(szPath, nMode);
}

char *
canonicalize_file_name(const char *szPath)
{
    return realpath(szPath, NULL);
}

#include <sys/uio.h>

ssize_t
readv(int nFd, const struct iovec *pIov, int nIovcnt)
{
    ssize_t nTotal = 0;
    int i;

    if (pIov == NULL || nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    /* Prefer single syscall when host/kernel supports it */
    {
        long r = sys6(NR_readv, nFd, (long)(uintptr_t)pIov, nIovcnt, 0, 0, 0);

        if (!(r < 0 && r > -4096 && (int)(-r) == ENOSYS)) {
            return (ssize_t)sys_ret(r);
        }
    }
    for (i = 0; i < nIovcnt; i++) {
        ssize_t n;

        if (pIov[i].iov_len == 0) {
            continue;
        }
        n = read(nFd, pIov[i].iov_base, pIov[i].iov_len);
        if (n < 0) {
            return (nTotal > 0) ? nTotal : -1;
        }
        nTotal += n;
        if ((size_t)n < pIov[i].iov_len) {
            break;
        }
    }
    return nTotal;
}

ssize_t
writev(int nFd, const struct iovec *pIov, int nIovcnt)
{
    ssize_t nTotal = 0;
    int i;

    if (pIov == NULL || nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    {
        long r = sys6(NR_writev, nFd, (long)(uintptr_t)pIov, nIovcnt, 0, 0, 0);

        if (!(r < 0 && r > -4096 && (int)(-r) == ENOSYS)) {
            return (ssize_t)sys_ret(r);
        }
    }
    for (i = 0; i < nIovcnt; i++) {
        ssize_t n;

        if (pIov[i].iov_len == 0) {
            continue;
        }
        n = write(nFd, pIov[i].iov_base, pIov[i].iov_len);
        if (n < 0) {
            return (nTotal > 0) ? nTotal : -1;
        }
        nTotal += n;
        if ((size_t)n < pIov[i].iov_len) {
            break;
        }
    }
    return nTotal;
}

ssize_t
preadv(int nFd, const struct iovec *pIov, int nIovcnt, off_t off)
{
    long r;
    ssize_t nTotal = 0;
    int i;
    off_t offCur = off;

    if (pIov == NULL || nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    /* preadv: offset low in arg3, high in arg4 (x86_64 often just low) */
    r = sys6(NR_preadv, nFd, (long)(uintptr_t)pIov, nIovcnt, (long)off, 0, 0);
    if (!(r < 0 && r > -4096 && (int)(-r) == ENOSYS)) {
        return (ssize_t)sys_ret(r);
    }
    for (i = 0; i < nIovcnt; i++) {
        ssize_t n;

        if (pIov[i].iov_len == 0) {
            continue;
        }
        n = pread(nFd, pIov[i].iov_base, pIov[i].iov_len, offCur);
        if (n < 0) {
            return (nTotal > 0) ? nTotal : -1;
        }
        nTotal += n;
        offCur += (off_t)n;
        if ((size_t)n < pIov[i].iov_len) {
            break;
        }
    }
    return nTotal;
}

ssize_t
pwritev(int nFd, const struct iovec *pIov, int nIovcnt, off_t off)
{
    long r;
    ssize_t nTotal = 0;
    int i;
    off_t offCur = off;

    if (pIov == NULL || nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_pwritev, nFd, (long)(uintptr_t)pIov, nIovcnt, (long)off, 0, 0);
    if (!(r < 0 && r > -4096 && (int)(-r) == ENOSYS)) {
        return (ssize_t)sys_ret(r);
    }
    for (i = 0; i < nIovcnt; i++) {
        ssize_t n;

        if (pIov[i].iov_len == 0) {
            continue;
        }
        n = pwrite(nFd, pIov[i].iov_base, pIov[i].iov_len, offCur);
        if (n < 0) {
            return (nTotal > 0) ? nTotal : -1;
        }
        nTotal += n;
        offCur += (off_t)n;
        if ((size_t)n < pIov[i].iov_len) {
            break;
        }
    }
    return nTotal;
}

off_t
lseek(int nFd, off_t off, int nWhence)
{
    return (off_t)sys_ret(sys6(NR_lseek, nFd, (long)off, nWhence, 0, 0, 0));
}

pid_t
getpid(void)
{
    return (pid_t)sys6(NR_getpid, 0, 0, 0, 0, 0, 0);
}

pid_t
getppid(void)
{
    return (pid_t)sys6(NR_getppid, 0, 0, 0, 0, 0, 0);
}

uid_t
getuid(void)
{
    return (uid_t)sys6(NR_getuid, 0, 0, 0, 0, 0, 0);
}

gid_t
getgid(void)
{
    return (gid_t)sys6(NR_getgid, 0, 0, 0, 0, 0, 0);
}

uid_t
geteuid(void)
{
    return (uid_t)sys6(NR_geteuid, 0, 0, 0, 0, 0, 0);
}

gid_t
getegid(void)
{
    return (gid_t)sys6(NR_getegid, 0, 0, 0, 0, 0, 0);
}

int
setuid(uid_t uid)
{
    return (int)sys_ret(sys6(NR_setuid, (long)uid, 0, 0, 0, 0, 0));
}

int
setgid(gid_t gid)
{
    return (int)sys_ret(sys6(NR_setgid, (long)gid, 0, 0, 0, 0, 0));
}

int
seteuid(uid_t euid)
{
    return setresuid((uid_t)-1, euid, (uid_t)-1);
}

int
setegid(gid_t egid)
{
    return setresgid((gid_t)-1, egid, (gid_t)-1);
}

int
setreuid(uid_t ruid, uid_t euid)
{
    return (int)sys_ret(sys6(NR_setreuid, (long)ruid, (long)euid, 0, 0, 0, 0));
}

int
setregid(gid_t rgid, gid_t egid)
{
    return (int)sys_ret(sys6(NR_setregid, (long)rgid, (long)egid, 0, 0, 0, 0));
}

int
getresuid(uid_t *pRuid, uid_t *pEuid, uid_t *pSuid)
{
    return (int)sys_ret(sys6(NR_getresuid, (long)(uintptr_t)pRuid,
                             (long)(uintptr_t)pEuid, (long)(uintptr_t)pSuid, 0,
                             0, 0));
}

int
getresgid(gid_t *pRgid, gid_t *pEgid, gid_t *pSgid)
{
    return (int)sys_ret(sys6(NR_getresgid, (long)(uintptr_t)pRgid,
                             (long)(uintptr_t)pEgid, (long)(uintptr_t)pSgid, 0,
                             0, 0));
}

int
setresuid(uid_t ruid, uid_t euid, uid_t suid)
{
    return (int)sys_ret(sys6(NR_setresuid, (long)ruid, (long)euid, (long)suid,
                             0, 0, 0));
}

int
setresgid(gid_t rgid, gid_t egid, gid_t sgid)
{
    return (int)sys_ret(sys6(NR_setresgid, (long)rgid, (long)egid, (long)sgid,
                             0, 0, 0));
}

int
getgroups(int nSize, gid_t *pList)
{
    return (int)sys_ret(sys6(NR_getgroups, nSize, (long)(uintptr_t)pList, 0, 0,
                             0, 0));
}

int
setgroups(size_t nSize, const gid_t *pList)
{
    return (int)sys_ret(sys6(NR_setgroups, (long)nSize, (long)(uintptr_t)pList,
                             0, 0, 0, 0));
}

int
initgroups(const char *szUser, gid_t gid)
{
    gid_t aG[1];

    (void)szUser; /* bring-up: no /etc/group */
    aG[0] = gid;
    return setgroups(1, aG);
}

int
utimensat(int nDfd, const char *szPath, const struct timespec aTs[2],
          int nFlags)
{
    return (int)sys_ret(sys6(NR_utimensat, nDfd, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)aTs, nFlags, 0, 0));
}

int
utimes(const char *szPath, const struct timeval aTv[2])
{
    struct timespec aTs[2];

    if (aTv != NULL) {
        aTs[0].tv_sec = aTv[0].tv_sec;
        aTs[0].tv_nsec = aTv[0].tv_usec * 1000L;
        aTs[1].tv_sec = aTv[1].tv_sec;
        aTs[1].tv_nsec = aTv[1].tv_usec * 1000L;
        return utimensat(AT_FDCWD, szPath, aTs, 0);
    }
    return utimensat(AT_FDCWD, szPath, NULL, 0);
}

int
lutimes(const char *szPath, const struct timeval aTv[2])
{
    struct timespec aTs[2];

    if (aTv != NULL) {
        aTs[0].tv_sec = aTv[0].tv_sec;
        aTs[0].tv_nsec = aTv[0].tv_usec * 1000L;
        aTs[1].tv_sec = aTv[1].tv_sec;
        aTs[1].tv_nsec = aTv[1].tv_usec * 1000L;
        return utimensat(AT_FDCWD, szPath, aTs, AT_SYMLINK_NOFOLLOW);
    }
    return utimensat(AT_FDCWD, szPath, NULL, AT_SYMLINK_NOFOLLOW);
}

int
futimens(int nFd, const struct timespec aTs[2])
{
    return utimensat(nFd, NULL, aTs, 0);
}

int
futimes(int nFd, const struct timeval aTv[2])
{
    struct timespec aTs[2];

    if (aTv != NULL) {
        aTs[0].tv_sec = aTv[0].tv_sec;
        aTs[0].tv_nsec = aTv[0].tv_usec * 1000L;
        aTs[1].tv_sec = aTv[1].tv_sec;
        aTs[1].tv_nsec = aTv[1].tv_usec * 1000L;
        return futimens(nFd, aTs);
    }
    return futimens(nFd, NULL);
}

int
brk(void *p)
{
    long r = sys6(NR_brk, (long)(uintptr_t)p, 0, 0, 0, 0, 0);

    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return 0;
}

void *
sbrk(intptr_t i64Delta)
{
    static uintptr_t uCur;
    uintptr_t uOld;
    long r;

    if (uCur == 0) {
        r = sys6(NR_brk, 0, 0, 0, 0, 0, 0);
        if (r < 0 && r > -4096) {
            errno = (int)(-r);
            return (void *)(intptr_t)-1;
        }
        uCur = (uintptr_t)r;
    }
    uOld = uCur;
    if (i64Delta == 0) {
        return (void *)uOld;
    }
    r = sys6(NR_brk, (long)(uCur + (uintptr_t)i64Delta), 0, 0, 0, 0, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return (void *)(intptr_t)-1;
    }
    uCur = (uintptr_t)r;
    return (void *)uOld;
}

void *
mmap(void *pAddr, size_t cb, int nProt, int nFlags, int nFd, off_t off)
{
    long r;

    r = sys6(NR_mmap, (long)(uintptr_t)pAddr, (long)cb, nProt, nFlags, nFd,
             (long)off);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return MAP_FAILED;
    }
    return (void *)(uintptr_t)r;
}

int
munmap(void *pAddr, size_t cb)
{
    return (int)sys_ret(sys6(NR_munmap, (long)(uintptr_t)pAddr, (long)cb, 0, 0,
                             0, 0));
}

int
mprotect(void *pAddr, size_t cb, int nProt)
{
    return (int)sys_ret(sys6(NR_mprotect, (long)(uintptr_t)pAddr, (long)cb,
                             nProt, 0, 0, 0));
}

int
msync(void *pAddr, size_t cb, int nFlags)
{
    return (int)sys_ret(sys6(NR_msync, (long)(uintptr_t)pAddr, (long)cb, nFlags,
                             0, 0, 0));
}

int
madvise(void *pAddr, size_t cb, int nAdvice)
{
    return (int)sys_ret(sys6(NR_madvise, (long)(uintptr_t)pAddr, (long)cb,
                             nAdvice, 0, 0, 0));
}

int
open(const char *szPath, int nFlags, ...)
{
    int nMode = 0;
    long r;

    if (nFlags & O_CREAT) {
        va_list ap;

        va_start(ap, nFlags);
        nMode = va_arg(ap, int);
        va_end(ap);
    }
    r = sys6(NR_openat, AT_FDCWD, (long)(uintptr_t)szPath, nFlags, nMode, 0, 0);
    return (int)sys_ret(r);
}

int
openat(int nDfd, const char *szPath, int nFlags, ...)
{
    int nMode = 0;
    long r;

    if (nFlags & O_CREAT) {
        va_list ap;

        va_start(ap, nFlags);
        nMode = va_arg(ap, int);
        va_end(ap);
    }
    r = sys6(NR_openat, nDfd, (long)(uintptr_t)szPath, nFlags, nMode, 0, 0);
    return (int)sys_ret(r);
}

int
creat(const char *szPath, mode_t mode)
{
    return open(szPath, O_CREAT | O_WRONLY | O_TRUNC, (int)mode);
}

int
fcntl(int nFd, int nCmd, ...)
{
    long arg = 0;
    va_list ap;

    va_start(ap, nCmd);
    if (nCmd == F_DUPFD || nCmd == F_SETFD || nCmd == F_SETFL ||
        nCmd == F_GETLK || nCmd == F_SETLK || nCmd == F_SETLKW) {
        arg = va_arg(ap, long);
    }
    va_end(ap);
    return (int)sys_ret(sys6(NR_fcntl, nFd, nCmd, arg, 0, 0, 0));
}

int
flock(int nFd, int nOp)
{
    return (int)sys_ret(sys6(NR_flock, nFd, nOp, 0, 0, 0, 0));
}

int
lockf(int nFd, int nCmd, off_t cbLen)
{
    struct flock fl;

    memset(&fl, 0, sizeof(fl));
    fl.l_whence = SEEK_CUR;
    fl.l_start = 0;
    fl.l_len = cbLen;
    switch (nCmd) {
    case F_ULOCK:
        fl.l_type = F_UNLCK;
        return fcntl(nFd, F_SETLK, (long)(uintptr_t)&fl);
    case F_LOCK:
        fl.l_type = F_WRLCK;
        return fcntl(nFd, F_SETLKW, (long)(uintptr_t)&fl);
    case F_TLOCK:
        fl.l_type = F_WRLCK;
        return fcntl(nFd, F_SETLK, (long)(uintptr_t)&fl);
    case F_TEST:
        fl.l_type = F_WRLCK;
        if (fcntl(nFd, F_GETLK, (long)(uintptr_t)&fl) != 0) {
            return -1;
        }
        if (fl.l_type == F_UNLCK) {
            return 0;
        }
        errno = EACCES;
        return -1;
    default:
        errno = EINVAL;
        return -1;
    }
}

int
fstat(int nFd, struct stat *pSt)
{
    return (int)sys_ret(sys6(NR_fstat, nFd, (long)(uintptr_t)pSt, 0, 0, 0, 0));
}

int
fstatat(int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
    if (szPath == NULL || pSt == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_newfstatat, nDfd, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)pSt, nFlags, 0, 0));
}

int
stat(const char *szPath, struct stat *pSt)
{
    return (int)sys_ret(sys6(NR_newfstatat, AT_FDCWD, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)pSt, 0, 0, 0));
}

int
lstat(const char *szPath, struct stat *pSt)
{
    return (int)sys_ret(sys6(NR_newfstatat, AT_FDCWD, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)pSt, AT_SYMLINK_NOFOLLOW, 0, 0));
}

int
mkdir(const char *szPath, mode_t mode)
{
    return (int)sys_ret(sys6(NR_mkdir, (long)(uintptr_t)szPath, (long)mode, 0,
                             0, 0, 0));
}

int
rmdir(const char *szPath)
{
    return (int)sys_ret(sys6(NR_rmdir, (long)(uintptr_t)szPath, 0, 0, 0, 0, 0));
}

int
unlink(const char *szPath)
{
    return (int)sys_ret(sys6(NR_unlink, (long)(uintptr_t)szPath, 0, 0, 0, 0, 0));
}

int
rename(const char *szOld, const char *szNew)
{
    return (int)sys_ret(sys6(NR_rename, (long)(uintptr_t)szOld,
                             (long)(uintptr_t)szNew, 0, 0, 0, 0));
}

int
link(const char *szOld, const char *szNew)
{
    return (int)sys_ret(sys6(NR_link, (long)(uintptr_t)szOld,
                             (long)(uintptr_t)szNew, 0, 0, 0, 0));
}

int
symlink(const char *szTarget, const char *szLink)
{
    return (int)sys_ret(sys6(NR_symlink, (long)(uintptr_t)szTarget,
                             (long)(uintptr_t)szLink, 0, 0, 0, 0));
}

ssize_t
readlink(const char *szPath, char *szBuf, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_readlink, (long)(uintptr_t)szPath,
                                 (long)(uintptr_t)szBuf, (long)cb, 0, 0, 0));
}

int
mkdirat(int nDfd, const char *szPath, mode_t mode)
{
    return (int)sys_ret(sys6(NR_mkdirat, nDfd, (long)(uintptr_t)szPath,
                             (long)mode, 0, 0, 0));
}

int
mknod(const char *szPath, mode_t mode, dev_t dev)
{
    return (int)sys_ret(sys6(NR_mknod, (long)(uintptr_t)szPath, (long)mode,
                             (long)dev, 0, 0, 0));
}

int
mknodat(int nDfd, const char *szPath, mode_t mode, dev_t dev)
{
    long r;

    r = sys6(NR_mknodat, nDfd, (long)(uintptr_t)szPath, (long)mode, (long)dev,
             0, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        if (nDfd != AT_FDCWD) {
            errno = ENOSYS;
            return -1;
        }
        return mknod(szPath, mode, dev);
    }
    return (int)sys_ret(r);
}

int
mkfifo(const char *szPath, mode_t mode)
{
    return mknod(szPath, (mode & 07777) | S_IFIFO, (dev_t)0);
}

int
mkfifoat(int nDfd, const char *szPath, mode_t mode)
{
    return mknodat(nDfd, szPath, (mode & 07777) | S_IFIFO, (dev_t)0);
}

int
unlinkat(int nDfd, const char *szPath, int nFlags)
{
    return (int)sys_ret(sys6(NR_unlinkat, nDfd, (long)(uintptr_t)szPath, nFlags,
                             0, 0, 0));
}

int
renameat(int nOlddfd, const char *szOld, int nNewdfd, const char *szNew)
{
    return (int)sys_ret(sys6(NR_renameat, nOlddfd, (long)(uintptr_t)szOld,
                             nNewdfd, (long)(uintptr_t)szNew, 0, 0));
}

int
renameat2(int nOlddfd, const char *szOld, int nNewdfd, const char *szNew,
          unsigned uFlags)
{
    return (int)sys_ret(sys6(NR_renameat2, nOlddfd, (long)(uintptr_t)szOld,
                             nNewdfd, (long)(uintptr_t)szNew, (long)uFlags, 0));
}

int
linkat(int nOlddfd, const char *szOld, int nNewdfd, const char *szNew,
       int nFlags)
{
    return (int)sys_ret(sys6(NR_linkat, nOlddfd, (long)(uintptr_t)szOld, nNewdfd,
                             (long)(uintptr_t)szNew, nFlags, 0));
}

int
symlinkat(const char *szTarget, int nNewdfd, const char *szLink)
{
    return (int)sys_ret(sys6(NR_symlinkat, (long)(uintptr_t)szTarget, nNewdfd,
                             (long)(uintptr_t)szLink, 0, 0, 0));
}

ssize_t
readlinkat(int nDfd, const char *szPath, char *szBuf, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_readlinkat, nDfd, (long)(uintptr_t)szPath,
                                 (long)(uintptr_t)szBuf, (long)cb, 0, 0));
}

ssize_t
copy_file_range(int nFdin, off_t *pOffIn, int nFdout, off_t *pOffOut,
                size_t cbLen, unsigned uFlags)
{
    long r;

    r = sys6(NR_copy_file_range, nFdin, (long)(uintptr_t)pOffIn, nFdout,
             (long)(uintptr_t)pOffOut, (long)cbLen, (long)uFlags);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        char aBuf[4096];
        size_t nLeft = cbLen;
        ssize_t nTotal = 0;
        off_t offIn = pOffIn ? *pOffIn : 0;
        off_t offOut = pOffOut ? *pOffOut : 0;

        (void)uFlags;
        while (nLeft > 0) {
            size_t nChunk = (nLeft > sizeof(aBuf)) ? sizeof(aBuf) : nLeft;
            ssize_t nR;
            ssize_t nW;

            nR = pOffIn ? pread(nFdin, aBuf, nChunk, offIn)
                        : read(nFdin, aBuf, nChunk);
            if (nR <= 0) {
                return (nTotal > 0) ? nTotal : nR;
            }
            nW = pOffOut ? pwrite(nFdout, aBuf, (size_t)nR, offOut)
                         : write(nFdout, aBuf, (size_t)nR);
            if (nW != nR) {
                return (nTotal > 0) ? nTotal : -1;
            }
            offIn += nW;
            offOut += nW;
            nTotal += nW;
            nLeft -= (size_t)nW;
        }
        if (pOffIn) {
            *pOffIn = offIn;
        }
        if (pOffOut) {
            *pOffOut = offOut;
        }
        return nTotal;
    }
    return (ssize_t)sys_ret(r);
}

int
access(const char *szPath, int nMode)
{
    return (int)sys_ret(sys6(NR_access, (long)(uintptr_t)szPath, nMode, 0, 0, 0,
                             0));
}

mode_t
umask(mode_t mask)
{
    return (mode_t)sys6(NR_umask, (long)mask, 0, 0, 0, 0, 0);
}

int
chmod(const char *szPath, mode_t mode)
{
    return (int)sys_ret(sys6(NR_chmod, (long)(uintptr_t)szPath, (long)mode, 0, 0,
                             0, 0));
}

int
fchmod(int nFd, mode_t mode)
{
    return (int)sys_ret(sys6(NR_fchmod, nFd, (long)mode, 0, 0, 0, 0));
}

int
fchmodat(int nDfd, const char *szPath, mode_t mode, int nFlags)
{
    return (int)sys_ret(sys6(NR_fchmodat, nDfd, (long)(uintptr_t)szPath,
                             (long)mode, nFlags, 0, 0));
}

int
chown(const char *szPath, uid_t uid, gid_t gid)
{
    return (int)sys_ret(sys6(NR_chown, (long)(uintptr_t)szPath, (long)uid,
                             (long)gid, 0, 0, 0));
}

int
fchown(int nFd, uid_t uid, gid_t gid)
{
    return (int)sys_ret(sys6(NR_fchown, nFd, (long)uid, (long)gid, 0, 0, 0));
}

int
lchown(const char *szPath, uid_t uid, gid_t gid)
{
    return (int)sys_ret(sys6(NR_lchown, (long)(uintptr_t)szPath, (long)uid,
                             (long)gid, 0, 0, 0));
}

int
fchownat(int nDfd, const char *szPath, uid_t uid, gid_t gid, int nFlags)
{
    return (int)sys_ret(sys6(NR_fchownat, nDfd, (long)(uintptr_t)szPath,
                             (long)uid, (long)gid, nFlags, 0));
}

#include <sys/statvfs.h>

int
statfs(const char *szPath, struct statfs *pBuf)
{
    return (int)sys_ret(sys6(NR_statfs, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)pBuf, 0, 0, 0, 0));
}

int
fstatfs(int nFd, struct statfs *pBuf)
{
    return (int)sys_ret(sys6(NR_fstatfs, nFd, (long)(uintptr_t)pBuf, 0, 0, 0,
                             0));
}

static void
statfs_to_statvfs(const struct statfs *pSf, struct statvfs *pSv)
{
    if (pSf == NULL || pSv == NULL) {
        return;
    }
    memset(pSv, 0, sizeof(*pSv));
    pSv->f_bsize = (unsigned long)pSf->f_bsize;
    pSv->f_frsize = (unsigned long)(pSf->f_frsize ? pSf->f_frsize : pSf->f_bsize);
    pSv->f_blocks = (fsblkcnt_t)pSf->f_blocks;
    pSv->f_bfree = (fsblkcnt_t)pSf->f_bfree;
    pSv->f_bavail = (fsblkcnt_t)pSf->f_bavail;
    pSv->f_files = (fsfilcnt_t)pSf->f_files;
    pSv->f_ffree = (fsfilcnt_t)pSf->f_ffree;
    pSv->f_favail = (fsfilcnt_t)pSf->f_ffree;
    pSv->f_fsid = (unsigned long)pSf->f_fsid[0];
    pSv->f_flag = (unsigned long)pSf->f_flags;
    pSv->f_namemax = (unsigned long)pSf->f_namelen;
}

int
statvfs(const char *szPath, struct statvfs *pBuf)
{
    struct statfs sf;

    if (statfs(szPath, &sf) != 0) {
        return -1;
    }
    statfs_to_statvfs(&sf, pBuf);
    return 0;
}

int
fstatvfs(int nFd, struct statvfs *pBuf)
{
    struct statfs sf;

    if (fstatfs(nFd, &sf) != 0) {
        return -1;
    }
    statfs_to_statvfs(&sf, pBuf);
    return 0;
}

int
chdir(const char *szPath)
{
    return (int)sys_ret(sys6(NR_chdir, (long)(uintptr_t)szPath, 0, 0, 0, 0, 0));
}

int
fchdir(int nFd)
{
    return (int)sys_ret(sys6(NR_fchdir, nFd, 0, 0, 0, 0, 0));
}

int
ttyname_r(int nFd, char *szBuf, size_t cb)
{
    char aLink[64];
    char aPath[256];
    ssize_t n;

    if (szBuf == NULL || cb == 0) {
        return EINVAL;
    }
    if (!isatty(nFd)) {
        return ENOTTY;
    }
    /* /proc/self/fd/N → real path when proc is present (host Linux) */
    {
        char aNum[16];
        int i = 0;
        int nV = nFd;
        char aTmp[16];
        int j = 0;

        aNum[0] = '\0';
        if (nV < 0) {
            return EBADF;
        }
        if (nV == 0) {
            aTmp[j++] = '0';
        } else {
            while (nV > 0 && j < (int)sizeof(aTmp)) {
                aTmp[j++] = (char)('0' + (nV % 10));
                nV /= 10;
            }
        }
        while (j > 0) {
            aNum[i++] = aTmp[--j];
        }
        aNum[i] = '\0';
        /* build "/proc/self/fd/<n>" */
        {
            const char *szP = "/proc/self/fd/";
            size_t k = 0;

            while (szP[k] != '\0' && k + 1 < sizeof(aLink)) {
                aLink[k] = szP[k];
                k++;
            }
            i = 0;
            while (aNum[i] != '\0' && k + 1 < sizeof(aLink)) {
                aLink[k++] = aNum[i++];
            }
            aLink[k] = '\0';
        }
    }
    n = readlink(aLink, aPath, sizeof(aPath) - 1);
    if (n < 0) {
        /* bring-up fallback name */
        const char *szDef = "/dev/tty";
        size_t cbDef = 8;

        if (cbDef + 1 > cb) {
            return ERANGE;
        }
        {
            size_t k;

            for (k = 0; k < cbDef; k++) {
                szBuf[k] = szDef[k];
            }
            szBuf[cbDef] = '\0';
        }
        return 0;
    }
    aPath[n] = '\0';
    if ((size_t)n + 1 > cb) {
        return ERANGE;
    }
    {
        size_t k;

        for (k = 0; k < (size_t)n; k++) {
            szBuf[k] = aPath[k];
        }
        szBuf[n] = '\0';
    }
    return 0;
}

char *
ttyname(int nFd)
{
    static char aBuf[256];

    if (ttyname_r(nFd, aBuf, sizeof(aBuf)) != 0) {
        return NULL;
    }
    return aBuf;
}

char *
getcwd(char *szBuf, size_t cb)
{
    long r;

    if (szBuf == NULL || cb == 0) {
        errno = EINVAL;
        return NULL;
    }
    r = sys6(NR_getcwd, (long)(uintptr_t)szBuf, (long)cb, 0, 0, 0, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return NULL;
    }
    return szBuf;
}

int
pipe(int aFd[2])
{
    return (int)sys_ret(sys6(NR_pipe2, (long)(uintptr_t)aFd, 0, 0, 0, 0, 0));
}

int
pipe2(int aFd[2], int nFlags)
{
    return (int)sys_ret(sys6(NR_pipe2, (long)(uintptr_t)aFd, nFlags, 0, 0, 0, 0));
}

int
dup(int nFd)
{
    return (int)sys_ret(sys6(NR_dup, nFd, 0, 0, 0, 0, 0));
}

int
dup2(int nOld, int nNew)
{
    return (int)sys_ret(sys6(NR_dup2, nOld, nNew, 0, 0, 0, 0));
}

int
dup3(int nOld, int nNew, int nFlags)
{
    long r;

    r = sys6(NR_dup3, nOld, nNew, nFlags, 0, 0, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        int nFd;

        if (nOld == nNew) {
            errno = EINVAL;
            return -1;
        }
        nFd = dup2(nOld, nNew);
        if (nFd < 0) {
            return -1;
        }
        if ((nFlags & O_CLOEXEC) != 0) {
            (void)fcntl(nFd, F_SETFD, FD_CLOEXEC);
        }
        return nFd;
    }
    return (int)sys_ret(r);
}

/* atfork hooks (atfork.c) */
void _libcgj_atfork_prepare(void);
void _libcgj_atfork_parent(void);
void _libcgj_atfork_child(void);

pid_t
fork(void)
{
    long r;
    pid_t pid;

    _libcgj_atfork_prepare();
    r = sys6(NR_fork, 0, 0, 0, 0, 0, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return (pid_t)-1;
    }
    pid = (pid_t)r;
    if (pid == 0) {
        _libcgj_atfork_child();
    } else {
        _libcgj_atfork_parent();
    }
    return pid;
}

int
execve(const char *szPath, char *const aArgv[], char *const aEnvp[])
{
    return (int)sys_ret(sys6(NR_execve, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)aArgv, (long)(uintptr_t)aEnvp, 0,
                             0, 0));
}

int
execv(const char *szPath, char *const aArgv[])
{
    return execve(szPath, aArgv, (char *const *)0);
}

pid_t
waitpid(pid_t pid, int *pStatus, int nOptions)
{
    return (pid_t)sys_ret(sys6(NR_wait4, pid, (long)(uintptr_t)pStatus, nOptions,
                               0, 0, 0));
}

pid_t
wait(int *pStatus)
{
    return waitpid(-1, pStatus, 0);
}

int
uname(struct utsname *pBuf)
{
    return (int)sys_ret(sys6(NR_uname, (long)(uintptr_t)pBuf, 0, 0, 0, 0, 0));
}

#include <sys/resource.h>
#include <sys/sysinfo.h>

int
getrusage(int nWho, struct rusage *pUsage)
{
    return (int)sys_ret(sys6(NR_getrusage, nWho, (long)(uintptr_t)pUsage, 0, 0,
                             0, 0));
}

int
getrlimit(int nResource, struct rlimit *pRlim)
{
    return (int)sys_ret(sys6(NR_getrlimit, nResource, (long)(uintptr_t)pRlim, 0,
                             0, 0, 0));
}

int
setrlimit(int nResource, const struct rlimit *pRlim)
{
    return (int)sys_ret(sys6(NR_setrlimit, nResource, (long)(uintptr_t)pRlim, 0,
                             0, 0, 0));
}

int
getpriority(int nWhich, id_t nWho)
{
    long r;

    errno = 0;
    r = sys6(NR_getpriority, nWhich, (long)nWho, 0, 0, 0, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    /* Linux syscall returns 20 - nice; glibc converts to nice value */
    return 20 - (int)r;
}

int
setpriority(int nWhich, id_t nWho, int nPrio)
{
    return (int)sys_ret(sys6(NR_setpriority, nWhich, (long)nWho, nPrio, 0, 0,
                             0));
}

int
nice(int nInc)
{
    int nCur;
    int nNew;

    errno = 0;
    nCur = getpriority(PRIO_PROCESS, 0);
    if (nCur == -1 && errno != 0) {
        return -1;
    }
    nNew = nCur + nInc;
    if (nNew > 19) {
        nNew = 19;
    }
    if (nNew < -20) {
        nNew = -20;
    }
    if (setpriority(PRIO_PROCESS, 0, nNew) != 0) {
        return -1;
    }
    errno = 0;
    return getpriority(PRIO_PROCESS, 0);
}

int
prlimit(pid_t nPid, int nResource, const struct rlimit *pNew,
        struct rlimit *pOld)
{
    return (int)sys_ret(sys6(NR_prlimit64, nPid, nResource,
                             (long)(uintptr_t)pNew, (long)(uintptr_t)pOld, 0,
                             0));
}

#include <sys/times.h>
#include <sys/wait.h>

clock_t
times(struct tms *pBuf)
{
    return (clock_t)sys_ret(sys6(NR_times, (long)(uintptr_t)pBuf, 0, 0, 0, 0,
                                 0));
}

int
waitid(idtype_t idtype, id_t id, siginfo_t *pInfop, int nOptions)
{
    return (int)sys_ret(sys6(NR_waitid, (long)idtype, (long)id,
                             (long)(uintptr_t)pInfop, nOptions, 0, 0));
}

int
sysinfo(struct sysinfo *pInfo)
{
    return (int)sys_ret(sys6(NR_sysinfo, (long)(uintptr_t)pInfo, 0, 0, 0, 0, 0));
}

int
getloadavg(double aLoad[], int nElem)
{
    struct sysinfo si;
    int n;
    int i;

    if (aLoad == NULL || nElem < 0) {
        errno = EINVAL;
        return -1;
    }
    if (nElem == 0) {
        return 0;
    }
    n = (nElem > 3) ? 3 : nElem;
    if (sysinfo(&si) != 0) {
        /* Fall back: IEEE 0.0 via integer zero (no SSE/FP ops) */
        for (i = 0; i < n; i++) {
            unsigned long long z = 0;

            memcpy(&aLoad[i], &z, sizeof(double));
        }
        return n;
    }
    for (i = 0; i < n; i++) {
        /*
         * Linux loads[] are scaled by 65536. Convert to double without FP
         * instructions: store IEEE754 for (loads/65536) only if we can
         * avoid SSE — use zero for bring-up when conversion needs FPU.
         * Product apps often only need success + non-negative values.
         */
        unsigned long u = si.loads[i];
        unsigned long long bits = 0;

        if (u == 0) {
            bits = 0;
        } else {
            /* Approximate: leave as 0; full float conversion needs SSE. */
            bits = 0;
            (void)u;
        }
        memcpy(&aLoad[i], &bits, sizeof(double));
    }
    return n;
}

pid_t
setsid(void)
{
    return (pid_t)sys_ret(sys6(NR_setsid, 0, 0, 0, 0, 0, 0));
}

int
daemon(int nNochdir, int nNoclose)
{
    pid_t pid;
    int nFd;

    pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid > 0) {
        _exit(0);
    }
    if (setsid() < 0) {
        return -1;
    }
    pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid > 0) {
        _exit(0);
    }
    if (!nNochdir) {
        (void)chdir("/");
    }
    if (!nNoclose) {
        nFd = open("/dev/null", O_RDWR);
        if (nFd >= 0) {
            (void)dup2(nFd, 0);
            (void)dup2(nFd, 1);
            (void)dup2(nFd, 2);
            if (nFd > 2) {
                (void)close(nFd);
            }
        }
    }
    return 0;
}

int
clock_gettime(clockid_t clk, struct timespec *pTs)
{
    return (int)sys_ret(sys6(NR_clock_gettime, clk, (long)(uintptr_t)pTs, 0, 0,
                             0, 0));
}

int
clock_getres(clockid_t clk, struct timespec *pTs)
{
    return (int)sys_ret(sys6(NR_clock_getres, clk, (long)(uintptr_t)pTs, 0, 0, 0,
                             0));
}

int
clock_nanosleep(clockid_t clk, int nFlags, const struct timespec *pReq,
                struct timespec *pRem)
{
    long r;

    r = sys6(NR_clock_nanosleep, clk, nFlags, (long)(uintptr_t)pReq,
             (long)(uintptr_t)pRem, 0, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        if (nFlags != 0) {
            /* TIMER_ABSTIME bring-up: convert to relative via clock_gettime */
            struct timespec now;
            struct timespec rel;

            if (pReq == NULL) {
                errno = EINVAL;
                return -1;
            }
            if (clock_gettime(clk, &now) != 0) {
                return -1;
            }
            rel.tv_sec = pReq->tv_sec - now.tv_sec;
            rel.tv_nsec = pReq->tv_nsec - now.tv_nsec;
            if (rel.tv_nsec < 0) {
                rel.tv_sec--;
                rel.tv_nsec += 1000000000L;
            }
            if (rel.tv_sec < 0 || (rel.tv_sec == 0 && rel.tv_nsec <= 0)) {
                return 0;
            }
            return nanosleep(&rel, pRem);
        }
        return nanosleep(pReq, pRem);
    }
    return (int)sys_ret(r);
}

int
timer_create(clockid_t clk, struct sigevent *pEv, timer_t *pTimerid)
{
    return (int)sys_ret(sys6(NR_timer_create, clk, (long)(uintptr_t)pEv,
                             (long)(uintptr_t)pTimerid, 0, 0, 0));
}

int
timer_delete(timer_t timerid)
{
    return (int)sys_ret(sys6(NR_timer_delete, (long)(uintptr_t)timerid, 0, 0, 0,
                             0, 0));
}

int
timer_settime(timer_t timerid, int nFlags, const struct itimerspec *pNew,
              struct itimerspec *pOld)
{
    return (int)sys_ret(sys6(NR_timer_settime, (long)(uintptr_t)timerid, nFlags,
                             (long)(uintptr_t)pNew, (long)(uintptr_t)pOld, 0,
                             0));
}

int
timer_gettime(timer_t timerid, struct itimerspec *pCur)
{
    return (int)sys_ret(sys6(NR_timer_gettime, (long)(uintptr_t)timerid,
                             (long)(uintptr_t)pCur, 0, 0, 0, 0));
}

int
timer_getoverrun(timer_t timerid)
{
    return (int)sys_ret(sys6(NR_timer_getoverrun, (long)(uintptr_t)timerid, 0, 0,
                             0, 0, 0));
}

/*
 * POSIX message queues — kernel returns ENOSYS; surface for symbol graphs.
 * mq_open has mode/attr when O_CREAT; ignore varargs and fail cleanly.
 */
mqd_t
mq_open(const char *szName, int nFlags, ...)
{
    long r;

    (void)nFlags;
    r = sys6(NR_mq_open, (long)(uintptr_t)szName, nFlags, 0, 0, 0, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return (mqd_t)-1;
    }
    return (mqd_t)r;
}

int
mq_close(mqd_t mq)
{
    (void)mq;
    errno = ENOSYS;
    return -1;
}

int
mq_unlink(const char *szName)
{
    return (int)sys_ret(sys6(NR_mq_unlink, (long)(uintptr_t)szName, 0, 0, 0, 0,
                             0));
}

int
mq_send(mqd_t mq, const char *pMsg, size_t cb, unsigned uPrio)
{
    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    return mq_timedsend(mq, pMsg, cb, uPrio, &ts);
}

int
mq_timedsend(mqd_t mq, const char *pMsg, size_t cb, unsigned uPrio,
             const struct timespec *pAbs)
{
    return (int)sys_ret(sys6(NR_mq_timedsend, mq, (long)(uintptr_t)pMsg,
                             (long)cb, (long)uPrio, (long)(uintptr_t)pAbs, 0));
}

ssize_t
mq_receive(mqd_t mq, char *pMsg, size_t cb, unsigned *pPrio)
{
    struct timespec ts;

    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    return mq_timedreceive(mq, pMsg, cb, pPrio, &ts);
}

ssize_t
mq_timedreceive(mqd_t mq, char *pMsg, size_t cb, unsigned *pPrio,
                const struct timespec *pAbs)
{
    return (ssize_t)sys_ret(sys6(NR_mq_timedreceive, mq, (long)(uintptr_t)pMsg,
                                 (long)cb, (long)(uintptr_t)pPrio,
                                 (long)(uintptr_t)pAbs, 0));
}

int
mq_notify(mqd_t mq, const struct sigevent *pEv)
{
    return (int)sys_ret(sys6(NR_mq_notify, mq, (long)(uintptr_t)pEv, 0, 0, 0,
                             0));
}

int
mq_getattr(mqd_t mq, struct mq_attr *pAttr)
{
    return (int)sys_ret(sys6(NR_mq_getsetattr, mq, 0, (long)(uintptr_t)pAttr, 0,
                             0, 0));
}

int
mq_setattr(mqd_t mq, const struct mq_attr *pNew, struct mq_attr *pOld)
{
    return (int)sys_ret(sys6(NR_mq_getsetattr, mq, (long)(uintptr_t)pNew,
                             (long)(uintptr_t)pOld, 0, 0, 0));
}

int
nanosleep(const struct timespec *pReq, struct timespec *pRem)
{
    return (int)sys_ret(sys6(NR_nanosleep, (long)(uintptr_t)pReq,
                             (long)(uintptr_t)pRem, 0, 0, 0, 0));
}

time_t
time(time_t *pT)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return (time_t)-1;
    }
    if (pT != NULL) {
        *pT = ts.tv_sec;
    }
    return ts.tv_sec;
}

int
gettimeofday(struct timeval *pTv, void *pTz)
{
    struct timespec ts;

    (void)pTz;
    if (pTv == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return -1;
    }
    pTv->tv_sec = ts.tv_sec;
    pTv->tv_usec = ts.tv_nsec / 1000;
    return 0;
}

int
isatty(int nFd)
{
    /* Bring-up: fd 0/1/2 treated as tty when no TIOCGWINSZ */
    return (nFd >= 0 && nFd <= 2) ? 1 : 0;
}

unsigned int
sleep(unsigned int u32Sec)
{
    struct timespec ts;

    ts.tv_sec = (time_t)u32Sec;
    ts.tv_nsec = 0;
    (void)nanosleep(&ts, NULL);
    return 0;
}

int
usleep(unsigned int u32Usec)
{
    struct timespec ts;

    ts.tv_sec = (time_t)(u32Usec / 1000000u);
    ts.tv_nsec = (long)((u32Usec % 1000000u) * 1000u);
    return nanosleep(&ts, NULL);
}

unsigned int
alarm(unsigned int u32Sec)
{
    return (unsigned int)sys6(NR_alarm, (long)u32Sec, 0, 0, 0, 0, 0);
}

int
pause(void)
{
    return (int)sys_ret(sys6(NR_pause, 0, 0, 0, 0, 0, 0));
}

void
_exit(int nCode)
{
    (void)sys6(NR_exit_group, nCode, 0, 0, 0, 0, 0);
    (void)sys6(NR_exit, nCode, 0, 0, 0, 0, 0);
    for (;;) {
        __asm__ volatile("hlt");
    }
}

int
memfd_create(const char *szName, unsigned uFlags)
{
    return (int)sys_ret(sys6(NR_memfd_create, (long)(uintptr_t)szName,
                             (long)uFlags, 0, 0, 0, 0));
}

#define NR_memfd_secret 447

int
memfd_secret(unsigned uFlags)
{
    return (int)sys_ret(sys6(NR_memfd_secret, (long)uFlags, 0, 0, 0, 0, 0));
}

/*
 * POSIX shm_open shape: open/create a named ram file under /dev/shm/.
 * GreenJade maps this to vfs_ram + optional named memobj via memfd path.
 */
int
shm_open(const char *szName, int nFlags, mode_t mode)
{
    char aPath[96];
    size_t i = 0;
    size_t j;
    const char *p;

    if (szName == NULL || szName[0] == '\0') {
        errno = EINVAL;
        return -1;
    }
    /* skip leading slash */
    p = szName;
    if (p[0] == '/') {
        p++;
    }
    aPath[i++] = '/';
    aPath[i++] = 'd';
    aPath[i++] = 'e';
    aPath[i++] = 'v';
    aPath[i++] = '/';
    aPath[i++] = 's';
    aPath[i++] = 'h';
    aPath[i++] = 'm';
    aPath[i++] = '/';
    for (j = 0; p[j] != '\0' && i + 1 < sizeof(aPath); j++) {
        char c = p[j];

        if (c == '/') {
            c = '_';
        }
        aPath[i++] = c;
    }
    aPath[i] = '\0';
    return open(aPath, nFlags, (int)mode);
}

int
shm_unlink(const char *szName)
{
    char aPath[96];
    size_t i = 0;
    size_t j;
    const char *p;

    if (szName == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = szName;
    if (p[0] == '/') {
        p++;
    }
    aPath[i++] = '/';
    aPath[i++] = 'd';
    aPath[i++] = 'e';
    aPath[i++] = 'v';
    aPath[i++] = '/';
    aPath[i++] = 's';
    aPath[i++] = 'h';
    aPath[i++] = 'm';
    aPath[i++] = '/';
    for (j = 0; p[j] != '\0' && i + 1 < sizeof(aPath); j++) {
        char c = p[j];

        if (c == '/') {
            c = '_';
        }
        aPath[i++] = c;
    }
    aPath[i] = '\0';
    return unlink(aPath);
}

int
poll(struct pollfd *pFds, nfds_t nFds, int nTimeoutMs)
{
    return (int)sys_ret(sys6(NR_poll, (long)(uintptr_t)pFds, (long)nFds,
                             (long)nTimeoutMs, 0, 0, 0));
}

int
ppoll(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTs,
      const void *pSigmask)
{
    long r;
    size_t cbMask = 8; /* kernel often takes 8; glibc uses sizeof(sigset_t) */

    r = sys6(NR_ppoll, (long)(uintptr_t)pFds, (long)nFds, (long)(uintptr_t)pTs,
             (long)(uintptr_t)pSigmask, (long)cbMask, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        int nMs = -1;

        if (pTs != NULL) {
            long long ms = (long long)pTs->tv_sec * 1000LL +
                           (long long)pTs->tv_nsec / 1000000LL;
            if (ms > 2147483647LL) {
                ms = 2147483647LL;
            }
            if (ms < 0) {
                ms = 0;
            }
            nMs = (int)ms;
        }
        (void)pSigmask;
        return poll(pFds, nFds, nMs);
    }
    return (int)sys_ret(r);
}

int
timerfd_create(int nClockid, int nFlags)
{
    return (int)sys_ret(sys6(NR_timerfd_create, nClockid, nFlags, 0, 0, 0, 0));
}

int
timerfd_settime(int nFd, int nFlags, const struct itimerspec *pNew,
                struct itimerspec *pOld)
{
    return (int)sys_ret(sys6(NR_timerfd_settime, nFd, nFlags,
                             (long)(uintptr_t)pNew, (long)(uintptr_t)pOld, 0,
                             0));
}

int
timerfd_gettime(int nFd, struct itimerspec *pCur)
{
    return (int)sys_ret(sys6(NR_timerfd_gettime, nFd, (long)(uintptr_t)pCur, 0,
                             0, 0, 0));
}

int
signalfd(int nFd, const sigset_t *pMask, int nFlags)
{
    /*
     * signalfd4(ufd, mask, sizemask, flags).
     * Kernel x86_64 sigset_t is 8 bytes (_NSIG=64); glibc user sigset_t is
     * larger — pass kernel size so host and GreenJade both accept.
     */
    return (int)sys_ret(sys6(NR_signalfd4, nFd, (long)(uintptr_t)pMask, 8L,
                             nFlags, 0, 0));
}

ssize_t
sendfile(int nOutFd, int nInFd, off_t *pOffset, size_t cbCount)
{
    return (ssize_t)sys_ret(sys6(NR_sendfile, nOutFd, nInFd,
                                 (long)(uintptr_t)pOffset, (long)cbCount, 0,
                                 0));
}

int
getitimer(int nWhich, struct itimerval *pCur)
{
    return (int)sys_ret(sys6(NR_getitimer, nWhich, (long)(uintptr_t)pCur, 0, 0,
                             0, 0));
}

int
setitimer(int nWhich, const struct itimerval *pNew, struct itimerval *pOld)
{
    return (int)sys_ret(sys6(NR_setitimer, nWhich, (long)(uintptr_t)pNew,
                             (long)(uintptr_t)pOld, 0, 0, 0));
}

int
inotify_init1(int nFlags)
{
    return (int)sys_ret(sys6(NR_inotify_init1, nFlags, 0, 0, 0, 0, 0));
}

int
inotify_init(void)
{
    return inotify_init1(0);
}

int
inotify_add_watch(int nFd, const char *szPath, uint32_t uMask)
{
    return (int)sys_ret(sys6(NR_inotify_add_watch, nFd, (long)(uintptr_t)szPath,
                             (long)uMask, 0, 0, 0));
}

int
inotify_rm_watch(int nFd, int nWd)
{
    return (int)sys_ret(sys6(NR_inotify_rm_watch, nFd, nWd, 0, 0, 0, 0));
}

int
epoll_create(int nSize)
{
    if (nSize <= 0) {
        errno = EINVAL;
        return -1;
    }
    return epoll_create1(0);
}

int
epoll_create1(int nFlags)
{
    return (int)sys_ret(sys6(NR_epoll_create1, nFlags, 0, 0, 0, 0, 0));
}

int
epoll_ctl(int nEpfd, int nOp, int nFd, struct epoll_event *pEvent)
{
    return (int)sys_ret(sys6(NR_epoll_ctl, nEpfd, nOp, nFd,
                             (long)(uintptr_t)pEvent, 0, 0));
}

int
epoll_wait(int nEpfd, struct epoll_event *pEvents, int nMaxevents, int nTimeout)
{
    return (int)sys_ret(sys6(NR_epoll_wait, nEpfd, (long)(uintptr_t)pEvents,
                             nMaxevents, nTimeout, 0, 0));
}

int
epoll_pwait(int nEpfd, struct epoll_event *pEvents, int nMaxevents,
            int nTimeout, const void *pSigmask)
{
    long r;

    r = sys6(NR_epoll_pwait, nEpfd, (long)(uintptr_t)pEvents, nMaxevents,
             nTimeout, (long)(uintptr_t)pSigmask, 8);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        (void)pSigmask;
        return epoll_wait(nEpfd, pEvents, nMaxevents, nTimeout);
    }
    return (int)sys_ret(r);
}

ssize_t
getrandom(void *pBuf, size_t cb, unsigned int u32Flags)
{
    return (ssize_t)sys_ret(sys6(NR_getrandom, (long)(uintptr_t)pBuf, (long)cb,
                                 (long)u32Flags, 0, 0, 0));
}

#include <sys/select.h>

int
select(int nFds, fd_set *pRead, fd_set *pWrite, fd_set *pExcept,
       struct timeval *pTv)
{
    struct pollfd aPf[64];
    int nPf = 0;
    int i;
    int nTimeout = -1;
    int nReady;
    int nOut = 0;

    if (nFds < 0) {
        errno = EINVAL;
        return -1;
    }
    if (nFds > 64) {
        nFds = 64; /* bring-up cap */
    }
    if (pTv != NULL) {
        long ms = (long)pTv->tv_sec * 1000L + pTv->tv_usec / 1000L;
        if (ms < 0) {
            ms = 0;
        }
        if (ms > 0x7fffffffL) {
            ms = 0x7fffffffL;
        }
        nTimeout = (int)ms;
    }
    for (i = 0; i < nFds && nPf < 64; i++) {
        short ev = 0;

        if (pRead != NULL && FD_ISSET(i, pRead)) {
            ev = (short)(ev | POLLIN);
        }
        if (pWrite != NULL && FD_ISSET(i, pWrite)) {
            ev = (short)(ev | POLLOUT);
        }
        if (pExcept != NULL && FD_ISSET(i, pExcept)) {
            ev = (short)(ev | POLLPRI);
        }
        if (ev != 0) {
            aPf[nPf].fd = i;
            aPf[nPf].events = ev;
            aPf[nPf].revents = 0;
            nPf++;
        }
    }
    nReady = poll(aPf, (nfds_t)nPf, nTimeout);
    if (nReady < 0) {
        return -1;
    }
    if (pRead != NULL) {
        FD_ZERO(pRead);
    }
    if (pWrite != NULL) {
        FD_ZERO(pWrite);
    }
    if (pExcept != NULL) {
        FD_ZERO(pExcept);
    }
    for (i = 0; i < nPf; i++) {
        if (aPf[i].revents == 0) {
            continue;
        }
        if (pRead != NULL && (aPf[i].revents & (POLLIN | POLLHUP | POLLERR))) {
            FD_SET(aPf[i].fd, pRead);
            nOut++;
        }
        if (pWrite != NULL && (aPf[i].revents & POLLOUT)) {
            FD_SET(aPf[i].fd, pWrite);
            nOut++;
        }
        if (pExcept != NULL &&
            (aPf[i].revents & (POLLPRI | POLLERR | POLLNVAL))) {
            FD_SET(aPf[i].fd, pExcept);
            nOut++;
        }
    }
    (void)nReady;
    return nOut;
}

int
pselect(int nFds, fd_set *pRead, fd_set *pWrite, fd_set *pExcept,
        const struct timespec *pTs, const void *pSigmask)
{
    long r;
    /*
     * pselect6(nfds, read, write, except, timespec, sigset_with_size).
     * 6th arg is a pointer to { const sigset_t *ss; size_t ss_len }.
     */
    struct {
        const void *pSs;
        size_t      cbSs;
    } sigpack;
    struct timeval tv;
    struct timeval *pTv = NULL;

    sigpack.pSs = pSigmask;
    sigpack.cbSs = 8; /* kernel x86_64 sigset size */
    r = sys6(NR_pselect6, nFds, (long)(uintptr_t)pRead, (long)(uintptr_t)pWrite,
             (long)(uintptr_t)pExcept, (long)(uintptr_t)pTs,
             (long)(uintptr_t)(pSigmask != NULL ? &sigpack : NULL));
    if (!(r < 0 && r > -4096 && (int)(-r) == ENOSYS)) {
        return (int)sys_ret(r);
    }

    /* ENOSYS: drop signal mask, map timeout to select */
    (void)pSigmask;
    if (pTs != NULL) {
        tv.tv_sec = pTs->tv_sec;
        tv.tv_usec = (long)(pTs->tv_nsec / 1000L);
        pTv = &tv;
    }
    return select(nFds, pRead, pWrite, pExcept, pTv);
}

#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

int
ioctl(int nFd, unsigned long u64Req, ...)
{
    va_list ap;
    unsigned long arg = 0;

    va_start(ap, u64Req);
    arg = va_arg(ap, unsigned long);
    va_end(ap);
    return (int)sys_ret(sys6(NR_ioctl, nFd, (long)u64Req, (long)arg, 0, 0, 0));
}

int
socket(int nDomain, int nType, int nProtocol)
{
    return (int)sys_ret(sys6(NR_socket, nDomain, nType, nProtocol, 0, 0, 0));
}

int
socketpair(int nDomain, int nType, int nProtocol, int aSv[2])
{
    return (int)sys_ret(sys6(NR_socketpair, nDomain, nType, nProtocol,
                             (long)(uintptr_t)aSv, 0, 0));
}

int
bind(int nFd, const struct sockaddr *pAddr, socklen_t cbAddr)
{
    return (int)sys_ret(sys6(NR_bind, nFd, (long)(uintptr_t)pAddr, (long)cbAddr,
                             0, 0, 0));
}

int
listen(int nFd, int nBacklog)
{
    return (int)sys_ret(sys6(NR_listen, nFd, nBacklog, 0, 0, 0, 0));
}

int
accept(int nFd, struct sockaddr *pAddr, socklen_t *pCbAddr)
{
    return (int)sys_ret(sys6(NR_accept, nFd, (long)(uintptr_t)pAddr,
                             (long)(uintptr_t)pCbAddr, 0, 0, 0));
}

int
accept4(int nFd, struct sockaddr *pAddr, socklen_t *pCbAddr, int nFlags)
{
    long r;

    r = sys6(NR_accept4, nFd, (long)(uintptr_t)pAddr, (long)(uintptr_t)pCbAddr,
             nFlags, 0, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        int nNew;

        nNew = accept(nFd, pAddr, pCbAddr);
        if (nNew < 0) {
            return -1;
        }
        if ((nFlags & SOCK_CLOEXEC) != 0) {
            (void)fcntl(nNew, F_SETFD, FD_CLOEXEC);
        }
        if ((nFlags & SOCK_NONBLOCK) != 0) {
            int nFl = fcntl(nNew, F_GETFL);

            if (nFl >= 0) {
                (void)fcntl(nNew, F_SETFL, nFl | O_NONBLOCK);
            }
        }
        return nNew;
    }
    return (int)sys_ret(r);
}

int
connect(int nFd, const struct sockaddr *pAddr, socklen_t cbAddr)
{
    return (int)sys_ret(sys6(NR_connect, nFd, (long)(uintptr_t)pAddr,
                             (long)cbAddr, 0, 0, 0));
}

ssize_t
sendto(int nFd, const void *pBuf, size_t cb, int nFlags,
       const struct sockaddr *pAddr, socklen_t cbAddr)
{
    return (ssize_t)sys_ret(sys6(NR_sendto, nFd, (long)(uintptr_t)pBuf, (long)cb,
                                 nFlags, (long)(uintptr_t)pAddr, (long)cbAddr));
}

ssize_t
recvfrom(int nFd, void *pBuf, size_t cb, int nFlags, struct sockaddr *pAddr,
         socklen_t *pCbAddr)
{
    return (ssize_t)sys_ret(sys6(NR_recvfrom, nFd, (long)(uintptr_t)pBuf,
                                 (long)cb, nFlags, (long)(uintptr_t)pAddr,
                                 (long)(uintptr_t)pCbAddr));
}

ssize_t
send(int nFd, const void *pBuf, size_t cb, int nFlags)
{
    return sendto(nFd, pBuf, cb, nFlags, NULL, 0);
}

ssize_t
recv(int nFd, void *pBuf, size_t cb, int nFlags)
{
    return recvfrom(nFd, pBuf, cb, nFlags, NULL, NULL);
}

ssize_t
sendmsg(int nFd, const struct msghdr *pMsg, int nFlags)
{
    return (ssize_t)sys_ret(sys6(NR_sendmsg, nFd, (long)(uintptr_t)pMsg, nFlags,
                                 0, 0, 0));
}

ssize_t
recvmsg(int nFd, struct msghdr *pMsg, int nFlags)
{
    return (ssize_t)sys_ret(sys6(NR_recvmsg, nFd, (long)(uintptr_t)pMsg, nFlags,
                                 0, 0, 0));
}

int
sendmmsg(int nFd, struct mmsghdr *pMsgvec, unsigned int uVlen, int nFlags)
{
    long r;
    unsigned int i;
    ssize_t nSent;

    r = sys6(NR_sendmmsg, nFd, (long)(uintptr_t)pMsgvec, (long)uVlen, nFlags, 0,
             0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        if (pMsgvec == NULL && uVlen != 0) {
            errno = EFAULT;
            return -1;
        }
        for (i = 0; i < uVlen; i++) {
            nSent = sendmsg(nFd, &pMsgvec[i].msg_hdr, nFlags);
            if (nSent < 0) {
                return (i == 0) ? -1 : (int)i;
            }
            pMsgvec[i].msg_len = (unsigned int)nSent;
        }
        return (int)uVlen;
    }
    return (int)sys_ret(r);
}

int
recvmmsg(int nFd, struct mmsghdr *pMsgvec, unsigned int uVlen, int nFlags,
         struct timespec *pTimeout)
{
    long r;
    unsigned int i;
    ssize_t nGot;

    r = sys6(NR_recvmmsg, nFd, (long)(uintptr_t)pMsgvec, (long)uVlen, nFlags,
             (long)(uintptr_t)pTimeout, 0);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        (void)pTimeout;
        if (pMsgvec == NULL && uVlen != 0) {
            errno = EFAULT;
            return -1;
        }
        for (i = 0; i < uVlen; i++) {
            nGot = recvmsg(nFd, &pMsgvec[i].msg_hdr, nFlags);
            if (nGot < 0) {
                return (i == 0) ? -1 : (int)i;
            }
            pMsgvec[i].msg_len = (unsigned int)nGot;
            if (nGot == 0) {
                return (int)(i + 1);
            }
        }
        return (int)uVlen;
    }
    return (int)sys_ret(r);
}

ssize_t
splice(int nFdin, off_t *pOffIn, int nFdout, off_t *pOffOut, size_t cbLen,
       unsigned uFlags)
{
    long r;
    char aBuf[4096];
    size_t cbLeft = cbLen;
    size_t cbDone = 0;

    r = sys6(NR_splice, nFdin, (long)(uintptr_t)pOffIn, nFdout,
             (long)(uintptr_t)pOffOut, (long)cbLen, (long)uFlags);
    if (!(r < 0 && r > -4096 && (int)(-r) == ENOSYS)) {
        return (ssize_t)sys_ret(r);
    }

    /* ENOSYS fallback: userspace copy (pipe/file bring-up) */
    while (cbLeft > 0) {
        size_t cbChunk = cbLeft > sizeof(aBuf) ? sizeof(aBuf) : cbLeft;
        ssize_t nR;
        ssize_t nW;
        size_t cbOff = 0;

        if (pOffIn != NULL) {
            nR = pread(nFdin, aBuf, cbChunk, *pOffIn);
        } else {
            nR = read(nFdin, aBuf, cbChunk);
        }
        if (nR < 0) {
            return (cbDone == 0) ? -1 : (ssize_t)cbDone;
        }
        if (nR == 0) {
            break;
        }
        if (pOffIn != NULL) {
            *pOffIn += (off_t)nR;
        }
        while (cbOff < (size_t)nR) {
            if (pOffOut != NULL) {
                nW = pwrite(nFdout, aBuf + cbOff, (size_t)nR - cbOff, *pOffOut);
            } else {
                nW = write(nFdout, aBuf + cbOff, (size_t)nR - cbOff);
            }
            if (nW < 0) {
                return (cbDone == 0) ? -1 : (ssize_t)cbDone;
            }
            if (nW == 0) {
                break;
            }
            if (pOffOut != NULL) {
                *pOffOut += (off_t)nW;
            }
            cbOff += (size_t)nW;
            cbDone += (size_t)nW;
            cbLeft -= (size_t)nW;
        }
        if (cbOff < (size_t)nR) {
            break;
        }
    }
    return (ssize_t)cbDone;
}

ssize_t
tee(int nFdin, int nFdout, size_t cbLen, unsigned uFlags)
{
    long r;
    char aBuf[4096];
    size_t cbChunk;
    ssize_t nR;
    ssize_t nW;
    size_t cbOff = 0;

    (void)uFlags;
    r = sys6(NR_tee, nFdin, nFdout, (long)cbLen, (long)uFlags, 0, 0);
    if (!(r < 0 && r > -4096 && (int)(-r) == ENOSYS)) {
        return (ssize_t)sys_ret(r);
    }

    /*
     * ENOSYS fallback: peek-read then write. True tee preserves input for
     * a later consumer; without kernel tee we copy once and leave data
     * consumed — better than failing hard on bring-up hosts.
     */
    cbChunk = cbLen > sizeof(aBuf) ? sizeof(aBuf) : cbLen;
    nR = read(nFdin, aBuf, cbChunk);
    if (nR <= 0) {
        return nR;
    }
    while (cbOff < (size_t)nR) {
        nW = write(nFdout, aBuf + cbOff, (size_t)nR - cbOff);
        if (nW < 0) {
            return -1;
        }
        if (nW == 0) {
            break;
        }
        cbOff += (size_t)nW;
    }
    return (ssize_t)cbOff;
}

ssize_t
vmsplice(int nFd, const struct iovec *pIov, unsigned long cSegs, unsigned uFlags)
{
    long r;
    unsigned long i;
    size_t cbDone = 0;

    (void)uFlags;
    r = sys6(NR_vmsplice, nFd, (long)(uintptr_t)pIov, (long)cSegs, (long)uFlags,
             0, 0);
    if (!(r < 0 && r > -4096 && (int)(-r) == ENOSYS)) {
        return (ssize_t)sys_ret(r);
    }

    /* ENOSYS: writev into the pipe/fd */
    if (pIov == NULL && cSegs != 0) {
        errno = EFAULT;
        return -1;
    }
    for (i = 0; i < cSegs; i++) {
        size_t cbOff = 0;
        const char *pBase = (const char *)pIov[i].iov_base;
        size_t cbLen = pIov[i].iov_len;

        while (cbOff < cbLen) {
            ssize_t nW = write(nFd, pBase + cbOff, cbLen - cbOff);

            if (nW < 0) {
                return (cbDone == 0) ? -1 : (ssize_t)cbDone;
            }
            if (nW == 0) {
                return (ssize_t)cbDone;
            }
            cbOff += (size_t)nW;
            cbDone += (size_t)nW;
        }
    }
    return (ssize_t)cbDone;
}

int
shutdown(int nFd, int nHow)
{
    return (int)sys_ret(sys6(NR_shutdown, nFd, nHow, 0, 0, 0, 0));
}

int
setsockopt(int nFd, int nLevel, int nOpt, const void *pVal, socklen_t cbVal)
{
    return (int)sys_ret(sys6(NR_setsockopt, nFd, nLevel, nOpt,
                             (long)(uintptr_t)pVal, (long)cbVal, 0));
}

int
getsockopt(int nFd, int nLevel, int nOpt, void *pVal, socklen_t *pCbVal)
{
    return (int)sys_ret(sys6(NR_getsockopt, nFd, nLevel, nOpt,
                             (long)(uintptr_t)pVal, (long)(uintptr_t)pCbVal, 0));
}

int
getsockname(int nFd, struct sockaddr *pAddr, socklen_t *pCbAddr)
{
    return (int)sys_ret(sys6(NR_getsockname, nFd, (long)(uintptr_t)pAddr,
                             (long)(uintptr_t)pCbAddr, 0, 0, 0));
}

int
getpeername(int nFd, struct sockaddr *pAddr, socklen_t *pCbAddr)
{
    return (int)sys_ret(sys6(NR_getpeername, nFd, (long)(uintptr_t)pAddr,
                             (long)(uintptr_t)pCbAddr, 0, 0, 0));
}

/* Linux getdents64 kernel layout */
struct linux_dirent64 {
    uint64_t       d_ino;
    int64_t        d_off;
    unsigned short d_reclen;
    unsigned char  d_type;
    char           d_name[];
};

static long
getdents64_raw(int nFd, void *pBuf, size_t cb)
{
    return sys_ret(sys6(NR_getdents64, nFd, (long)(uintptr_t)pBuf, (long)cb, 0,
                        0, 0));
}

DIR *
opendir(const char *szPath)
{
    DIR *p;
    int nFd;

    if (szPath == NULL) {
        errno = EINVAL;
        return NULL;
    }
    nFd = open(szPath, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (nFd < 0) {
        return NULL;
    }
    p = (DIR *)malloc(sizeof(DIR));
    if (p == NULL) {
        (void)close(nFd);
        errno = ENOMEM;
        return NULL;
    }
    memset(p, 0, sizeof(*p));
    p->nFd = nFd;
    p->nPos = 0;
    p->nEnd = 0;
    return p;
}

struct dirent *
readdir(DIR *pDir)
{
    struct linux_dirent64 *pK;
    size_t nName;
    long n;

    if (pDir == NULL || pDir->nFd < 0) {
        errno = EBADF;
        return NULL;
    }
    if (pDir->nPos >= pDir->nEnd) {
        n = getdents64_raw(pDir->nFd, pDir->aBuf, sizeof(pDir->aBuf));
        if (n <= 0) {
            return NULL;
        }
        pDir->nEnd = (int)n;
        pDir->nPos = 0;
    }
    pK = (struct linux_dirent64 *)(pDir->aBuf + pDir->nPos);
    if (pK->d_reclen == 0 ||
        pDir->nPos + (int)pK->d_reclen > pDir->nEnd) {
        errno = EIO;
        return NULL;
    }
    pDir->ent.d_ino = (ino_t)pK->d_ino;
    pDir->ent.d_off = (off_t)pK->d_off;
    pDir->ent.d_reclen = pK->d_reclen;
    pDir->ent.d_type = pK->d_type;
    nName = 0;
    while (nName + 1 < sizeof(pDir->ent.d_name) && pK->d_name[nName] != '\0') {
        pDir->ent.d_name[nName] = pK->d_name[nName];
        nName++;
    }
    pDir->ent.d_name[nName] = '\0';
    pDir->nPos += (int)pK->d_reclen;
    return &pDir->ent;
}

int
closedir(DIR *pDir)
{
    int nFd;

    if (pDir == NULL) {
        errno = EBADF;
        return -1;
    }
    nFd = pDir->nFd;
    free(pDir);
    return close(nFd);
}

void
rewinddir(DIR *pDir)
{
    if (pDir == NULL || pDir->nFd < 0) {
        return;
    }
    (void)lseek(pDir->nFd, 0, SEEK_SET);
    pDir->nPos = 0;
    pDir->nEnd = 0;
}

DIR *
fdopendir(int nFd)
{
    DIR *p;

    if (nFd < 0) {
        errno = EBADF;
        return NULL;
    }
    p = (DIR *)malloc(sizeof(DIR));
    if (p == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    memset(p, 0, sizeof(*p));
    p->nFd = nFd;
    p->nPos = 0;
    p->nEnd = 0;
    return p;
}

int
alphasort(const struct dirent **ppA, const struct dirent **ppB)
{
    if (ppA == NULL || ppB == NULL || *ppA == NULL || *ppB == NULL) {
        return 0;
    }
    return strcmp((*ppA)->d_name, (*ppB)->d_name);
}

int
scandir(const char *szPath, struct dirent ***ppList,
        int (*pfnFilter)(const struct dirent *),
        int (*pfnCmp)(const struct dirent **, const struct dirent **))
{
    DIR *pDir;
    struct dirent *pEnt;
    struct dirent **aList = NULL;
    struct dirent *pCopy;
    size_t nCap = 0;
    size_t nCnt = 0;
    size_t i;

    if (szPath == NULL || ppList == NULL) {
        errno = EINVAL;
        return -1;
    }
    pDir = opendir(szPath);
    if (pDir == NULL) {
        return -1;
    }
    while ((pEnt = readdir(pDir)) != NULL) {
        if (pfnFilter != NULL && pfnFilter(pEnt) == 0) {
            continue;
        }
        if (nCnt + 1 > nCap) {
            size_t nNew = (nCap == 0) ? 8 : nCap * 2;
            struct dirent **aNew =
                (struct dirent **)malloc(nNew * sizeof(struct dirent *));

            if (aNew == NULL) {
                goto fail;
            }
            for (i = 0; i < nCnt; i++) {
                aNew[i] = aList[i];
            }
            /* bump free is no-op; drop old pointer without free for bring-up */
            aList = aNew;
            nCap = nNew;
        }
        pCopy = (struct dirent *)malloc(sizeof(struct dirent));
        if (pCopy == NULL) {
            goto fail;
        }
        *pCopy = *pEnt;
        aList[nCnt++] = pCopy;
    }
    (void)closedir(pDir);
    pDir = NULL;
    if (pfnCmp != NULL && nCnt > 1) {
        qsort(aList, nCnt, sizeof(struct dirent *),
              (int (*)(const void *, const void *))pfnCmp);
    }
    *ppList = aList;
    return (int)nCnt;

fail:
    if (pDir != NULL) {
        (void)closedir(pDir);
    }
    if (aList != NULL) {
        for (i = 0; i < nCnt; i++) {
            free(aList[i]);
        }
        free(aList);
    }
    errno = ENOMEM;
    return -1;
}

int
fallocate(int nFd, int nMode, off_t off, off_t cbLen)
{
    return (int)sys_ret(sys6(NR_fallocate, nFd, nMode, (long)off, (long)cbLen,
                             0, 0));
}

int
posix_fallocate(int nFd, off_t off, off_t cbLen)
{
    return fallocate(nFd, 0, off, cbLen);
}

int
getdtablesize(void)
{
    long n = sysconf(_SC_OPEN_MAX);

    if (n < 0) {
        return 1024;
    }
    return (int)n;
}

int
getpagesize(void)
{
    long n = sysconf(_SC_PAGESIZE);

    return (n > 0) ? (int)n : 4096;
}

#include <sys/eventfd.h>

int
eventfd(unsigned int u32Init, int nFlags)
{
    return (int)sys_ret(sys6(NR_eventfd2, (long)u32Init, nFlags, 0, 0, 0, 0));
}

int
eventfd_read(int nFd, uint64_t *pVal)
{
    ssize_t n;

    if (pVal == NULL) {
        errno = EFAULT;
        return -1;
    }
    n = read(nFd, pVal, sizeof(*pVal));
    return (n == (ssize_t)sizeof(*pVal)) ? 0 : -1;
}

int
eventfd_write(int nFd, uint64_t u64Val)
{
    ssize_t n = write(nFd, &u64Val, sizeof(u64Val));

    return (n == (ssize_t)sizeof(u64Val)) ? 0 : -1;
}

/* getentropy: OpenBSD/glibc shape over getrandom */
int
getentropy(void *pBuf, size_t cb)
{
    ssize_t n;
    size_t off = 0;
    unsigned char *p = (unsigned char *)pBuf;

    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (cb > 256) {
        errno = EIO;
        return -1;
    }
    while (off < cb) {
        n = getrandom(p + off, cb - off, 0);
        if (n < 0) {
            return -1;
        }
        if (n == 0) {
            errno = EIO;
            return -1;
        }
        off += (size_t)n;
    }
    return 0;
}

int
ftruncate(int nFd, off_t cbLen)
{
    return (int)sys_ret(sys6(NR_ftruncate, nFd, (long)cbLen, 0, 0, 0, 0));
}

int
truncate(const char *szPath, off_t cbLen)
{
    int nFd;
    int n;

    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    nFd = open(szPath, O_WRONLY);
    if (nFd < 0) {
        return -1;
    }
    n = ftruncate(nFd, cbLen);
    (void)close(nFd);
    return n;
}

long
sysconf(int nName)
{
    switch (nName) {
    case _SC_PAGESIZE:
        return 4096L;
    case _SC_CLK_TCK:
        return 100L;
    case _SC_OPEN_MAX:
        return 1024L;
    case _SC_ARG_MAX:
        return 131072L;
    case _SC_LINE_MAX:
        return 2048L;
    case _SC_HOST_NAME_MAX:
        return 64L;
    case _SC_NPROCESSORS_ONLN:
    case _SC_NPROCESSORS_CONF:
        return 1L; /* bring-up default; product: parse online count */
    default:
        errno = EINVAL;
        return -1L;
    }
}

size_t
confstr(int nName, char *szBuf, size_t cb)
{
    const char *sz;
    size_t n;

    switch (nName) {
    case _CS_PATH:
        sz = "/bin:/usr/bin";
        break;
    case _CS_GNU_LIBC_VERSION:
        sz = "libcgj 0.1 (GLIBC-shaped)";
        break;
    default:
        errno = EINVAL;
        return 0;
    }
    n = strlen(sz) + 1;
    if (szBuf != NULL && cb > 0) {
        size_t c = (n < cb) ? n : cb;
        memcpy(szBuf, sz, c);
        szBuf[c - 1] = '\0';
    }
    return n;
}

int
gethostname(char *szName, size_t cb)
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
    n = strlen(un.nodename);
    if (n + 1 > cb) {
        /* Soft: fill what fits, still report ENAMETOOLONG. */
        if (cb > 0) {
            size_t cCopy = cb - 1;
            if (cCopy > n) {
                cCopy = n;
            }
            memcpy(szName, un.nodename, cCopy);
            szName[cCopy] = '\0';
        }
        errno = ENAMETOOLONG;
        return -1;
    }
    memcpy(szName, un.nodename, n + 1);
    return 0;
}

#define NR_sethostname 170

int
sethostname(const char *szName, size_t cb)
{
    /* Soft deepen: real sethostname SYSCALL (CAP_SYS_ADMIN / privilege). */
    if (szName == NULL && cb != 0) {
        errno = EFAULT;
        return -1;
    }
    /* Linux utsname nodename max is __NEW_UTS_LEN (64). */
    if (cb > 64) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_sethostname, (long)(uintptr_t)szName, (long)cb,
                             0, 0, 0, 0));
}

#define NR_sched_yield        24
#define NR_sched_setaffinity 203
#define NR_sched_getaffinity 204

int
sched_yield(void)
{
    return (int)sys_ret(sys6(NR_sched_yield, 0, 0, 0, 0, 0, 0));
}

int
sched_getaffinity(pid_t pid, size_t cbSet, cpu_set_t *pSet)
{
    long r;

    if (pSet == NULL || cbSet == 0) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_sched_getaffinity, (long)pid, (long)cbSet,
             (long)(uintptr_t)pSet, 0, 0, 0);
    r = sys_ret(r);
    if (r < 0) {
        return -1;
    }
    /* Kernel returns bytes written; zero rest of user buffer */
    if ((size_t)r < cbSet) {
        memset((unsigned char *)pSet + (size_t)r, 0, cbSet - (size_t)r);
    }
    return 0;
}

int
sched_setaffinity(pid_t pid, size_t cbSet, const cpu_set_t *pSet)
{
    long r;

    if (pSet == NULL || cbSet == 0) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_sched_setaffinity, (long)pid, (long)cbSet,
             (long)(uintptr_t)pSet, 0, 0, 0);
    return (int)sys_ret(r);
}

#define NR_clock_settime 227
#define NR_prctl         157

int
clock_settime(clockid_t clk, const struct timespec *pTs)
{
    if (pTs == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_clock_settime, clk, (long)(uintptr_t)pTs, 0, 0,
                             0, 0));
}

int
prctl(int nOption, unsigned long a2, unsigned long a3, unsigned long a4,
      unsigned long a5)
{
    return (int)sys_ret(sys6(NR_prctl, nOption, (long)a2, (long)a3, (long)a4,
                             (long)a5, 0));
}

#define NR_getcpu     309
#define NR_fadvise64  221

int
sched_getcpu(void)
{
    unsigned uCpu = 0;
    long r;

    r = sys6(NR_getcpu, (long)(uintptr_t)&uCpu, 0, 0, 0, 0, 0);
    if (r < 0) {
        return (int)sys_ret(r);
    }
    return (int)uCpu;
}

#ifndef NR_adjtimex
#define NR_adjtimex 159
#endif

int
adjtime(const struct timeval *pDelta, struct timeval *pOldDelta)
{
    struct timex tx;
    long r;
    long off;

    memset(&tx, 0, sizeof(tx));
    if (pDelta == NULL) {
        /* Query remaining adjustment via single-shot read mode. */
        tx.modes = ADJ_OFFSET_SS_READ;
        r = sys6(NR_adjtimex, (long)(uintptr_t)&tx, 0, 0, 0, 0, 0);
        if (r < 0 && r > -4096) {
            errno = (int)(-r);
            if (pOldDelta != NULL) {
                pOldDelta->tv_sec = 0;
                pOldDelta->tv_usec = 0;
            }
            return -1;
        }
        if (pOldDelta != NULL) {
            off = tx.offset;
            /* timex.offset is usec (ADJ_OFFSET_SINGLESHOT shape). */
            pOldDelta->tv_sec = off / 1000000L;
            pOldDelta->tv_usec = off % 1000000L;
            if (pOldDelta->tv_usec < 0) {
                pOldDelta->tv_sec -= 1;
                pOldDelta->tv_usec += 1000000L;
            }
        }
        return 0;
    }
    off = (long)pDelta->tv_sec * 1000000L + (long)pDelta->tv_usec;
    tx.modes = ADJ_OFFSET_SINGLESHOT;
    tx.offset = off;
    r = sys6(NR_adjtimex, (long)(uintptr_t)&tx, 0, 0, 0, 0, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    if (pOldDelta != NULL) {
        off = tx.offset;
        pOldDelta->tv_sec = off / 1000000L;
        pOldDelta->tv_usec = off % 1000000L;
        if (pOldDelta->tv_usec < 0) {
            pOldDelta->tv_sec -= 1;
            pOldDelta->tv_usec += 1000000L;
        }
    }
    return 0;
}

int
posix_fadvise(int nFd, off_t off, off_t cbLen, int nAdvice)
{
    return (int)sys_ret(sys6(NR_fadvise64, nFd, (long)off, (long)cbLen,
                             (long)nAdvice, 0, 0));
}

int
posix_madvise(void *pAddr, size_t cb, int nAdvice)
{
    return madvise(pAddr, cb, nAdvice);
}

#define NR_mlock      149
#define NR_munlock    150
#define NR_mlockall   151
#define NR_munlockall 152
#define NR_mincore    27 /* x86_64: was misnumbered as 148 (sched_rr) */
#define NR_setns      308
#define NR_unshare    272

/* glibc getcpu(cpu, node, tcache) — tcache ignored since Linux 2.6.24 */
int
getcpu(unsigned *pCpu, unsigned *pNode, void *pTcache)
{
    unsigned uCpu = 0;
    unsigned uNode = 0;
    long r;

    (void)pTcache;
    r = sys6(NR_getcpu, (long)(uintptr_t)&uCpu, (long)(uintptr_t)&uNode, 0, 0, 0,
             0);
    if (r < 0) {
        return (int)sys_ret(r);
    }
    if (pCpu != NULL) {
        *pCpu = uCpu;
    }
    if (pNode != NULL) {
        *pNode = uNode;
    }
    return 0;
}

pid_t
gettid(void)
{
    return (pid_t)sys6(NR_gettid, 0, 0, 0, 0, 0, 0);
}

int
mlock(const void *pAddr, size_t cb)
{
    return (int)sys_ret(sys6(NR_mlock, (long)(uintptr_t)pAddr, (long)cb, 0, 0,
                             0, 0));
}

int
munlock(const void *pAddr, size_t cb)
{
    return (int)sys_ret(sys6(NR_munlock, (long)(uintptr_t)pAddr, (long)cb, 0, 0,
                             0, 0));
}

int
mlockall(int nFlags)
{
    return (int)sys_ret(sys6(NR_mlockall, nFlags, 0, 0, 0, 0, 0));
}

int
munlockall(void)
{
    return (int)sys_ret(sys6(NR_munlockall, 0, 0, 0, 0, 0, 0));
}

int
mincore(void *pAddr, size_t cb, unsigned char *pVec)
{
    if (pVec == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_mincore, (long)(uintptr_t)pAddr, (long)cb,
                             (long)(uintptr_t)pVec, 0, 0, 0));
}

int
setns(int nFd, int nNstype)
{
    return (int)sys_ret(sys6(NR_setns, nFd, nNstype, 0, 0, 0, 0));
}

int
unshare(int nFlags)
{
    return (int)sys_ret(sys6(NR_unshare, nFlags, 0, 0, 0, 0, 0));
}

#define NR_sync_file_range 277
#define NR_readahead       187
#define NR_personality     135
#define NR_reboot          169
#define NR_syslog          103 /* klogctl */

int
sync_file_range(int nFd, off_t off, off_t cb, unsigned uFlags)
{
    return (int)sys_ret(sys6(NR_sync_file_range, nFd, (long)off, (long)cb,
                             (long)uFlags, 0, 0));
}

ssize_t
readahead(int nFd, off_t off, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_readahead, nFd, (long)off, (long)cb, 0, 0,
                                 0));
}

int
personality(unsigned long uPersona)
{
    return (int)sys_ret(sys6(NR_personality, (long)uPersona, 0, 0, 0, 0, 0));
}

int
reboot(int nMagic, int nMagic2, int nCmd, void *pArg)
{
    return (int)sys_ret(sys6(NR_reboot, nMagic, nMagic2, nCmd,
                             (long)(uintptr_t)pArg, 0, 0));
}

int
klogctl(int nType, char *szBuf, int nLen)
{
    return (int)sys_ret(sys6(NR_syslog, nType, (long)(uintptr_t)szBuf, nLen, 0,
                             0, 0));
}

#ifndef NR_adjtimex
#define NR_adjtimex      159
#endif
#define NR_clock_adjtime 305
#define NR_acct          163

int
adjtimex(struct timex *pBuf)
{
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_adjtimex, (long)(uintptr_t)pBuf, 0, 0, 0, 0, 0));
}

int
clock_adjtime(clockid_t clk, struct timex *pBuf)
{
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_clock_adjtime, clk, (long)(uintptr_t)pBuf, 0, 0,
                             0, 0));
}

int
ntp_adjtime(struct timex *pBuf)
{
    return adjtimex(pBuf);
}

int
acct(const char *szFilename)
{
    return (int)sys_ret(sys6(NR_acct, (long)(uintptr_t)szFilename, 0, 0, 0, 0,
                             0));
}
