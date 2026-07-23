/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Additional thin Linux x86_64 SYSCALL wrappers for libcgj (desktop path).
 * Clean-room public ABI only — no GPL source.
 * (sched/process_vm live in sched_more.c — do not duplicate here.)
 * Soft deepen: faccessat2 fallback; null/arg guards; close_range order check.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/capability.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <unistd.h>

/* Linux x86_64 NRs (public ABI) */
#define NR_capget              125
#define NR_capset              126
#define NR_pivot_root          155
#define NR_mount               165
#define NR_umount2             166
#define NR_setxattr            188
#define NR_lsetxattr           189
#define NR_fsetxattr           190
#define NR_getxattr            191
#define NR_lgetxattr           192
#define NR_fgetxattr           193
#define NR_listxattr           194
#define NR_llistxattr          195
#define NR_flistxattr          196
#define NR_removexattr         197
#define NR_lremovexattr        198
#define NR_fremovexattr        199
#define NR_ioprio_set          251
#define NR_ioprio_get          252
#define NR_name_to_handle_at   303
#define NR_open_by_handle_at   304
#define NR_execveat            322
#define NR_membarrier          324
#define NR_statx               332
#define NR_close_range         436
#define NR_faccessat2          439
#define NR_fchmodat2           452

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

/* ---- capabilities ------------------------------------------------------- */

int
capget(cap_user_header_t pHdr, cap_user_data_t pData)
{
    if (pHdr == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_capget, (long)(uintptr_t)pHdr,
                             (long)(uintptr_t)pData, 0, 0, 0, 0));
}

int
capset(cap_user_header_t pHdr, const cap_user_data_t pData)
{
    if (pHdr == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_capset, (long)(uintptr_t)pHdr,
                             (long)(uintptr_t)pData, 0, 0, 0, 0));
}

/* ---- *at / path helpers ------------------------------------------------- */

int
execveat(int nDfd, const char *szPath, char *const aArgv[],
         char *const aEnvp[], int nFlags)
{
    if (szPath == NULL || aArgv == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_execveat, nDfd, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)aArgv, (long)(uintptr_t)aEnvp,
                             nFlags, 0));
}

int
close_range(unsigned uFirst, unsigned uLast, int nFlags)
{
    if (uFirst > uLast) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_close_range, (long)uFirst, (long)uLast, nFlags,
                             0, 0, 0));
}

int
fchmodat2(int nDfd, const char *szPath, mode_t mode, int nFlags)
{
    long r;

    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    r = sys6(NR_fchmodat2, nDfd, (long)(uintptr_t)szPath, (long)mode, nFlags,
             0, 0);
    if (r == -ENOSYS) {
        /* Soft: fall back to fchmodat (flags may be partially honored). */
        return fchmodat(nDfd, szPath, mode, nFlags);
    }
    return (int)sys_ret(r);
}

int
faccessat2(int nDfd, const char *szPath, int nMode, int nFlags)
{
    long r;

    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    r = sys6(NR_faccessat2, nDfd, (long)(uintptr_t)szPath, nMode, nFlags, 0,
             0);
    if (r == -ENOSYS) {
        /*
         * Soft fallback: plain faccessat. AT_EACCESS / AT_SYMLINK_NOFOLLOW
         * may not be fully honored on older kernels — best-effort.
         */
        return faccessat(nDfd, szPath, nMode, nFlags);
    }
    return (int)sys_ret(r);
}

int
statx(int nDfd, const char *szPath, int nFlags, unsigned uMask,
      struct statx *pStx)
{
    if (szPath == NULL || pStx == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_statx, nDfd, (long)(uintptr_t)szPath, nFlags,
                             (long)uMask, (long)(uintptr_t)pStx, 0));
}

int
name_to_handle_at(int nDfd, const char *szPath, struct file_handle *pHandle,
                  int *pMountId, int nFlags)
{
    if (szPath == NULL || pHandle == NULL || pMountId == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_name_to_handle_at, nDfd,
                             (long)(uintptr_t)szPath, (long)(uintptr_t)pHandle,
                             (long)(uintptr_t)pMountId, nFlags, 0));
}

int
open_by_handle_at(int nMountFd, struct file_handle *pHandle, int nFlags)
{
    if (pHandle == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (nMountFd < 0) {
        errno = EBADF;
        return -1;
    }
    return (int)sys_ret(sys6(NR_open_by_handle_at, nMountFd,
                             (long)(uintptr_t)pHandle, nFlags, 0, 0, 0));
}

/* ---- mount / pivot ------------------------------------------------------ */

int
mount(const char *szSource, const char *szTarget, const char *szFstype,
      unsigned long uFlags, const void *pData)
{
    if (szTarget == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_mount, (long)(uintptr_t)szSource,
                             (long)(uintptr_t)szTarget,
                             (long)(uintptr_t)szFstype, (long)uFlags,
                             (long)(uintptr_t)pData, 0));
}

int
umount(const char *szTarget)
{
    return umount2(szTarget, 0);
}

int
umount2(const char *szTarget, int nFlags)
{
    if (szTarget == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_umount2, (long)(uintptr_t)szTarget, nFlags, 0,
                             0, 0, 0));
}

int
pivot_root(const char *szNew, const char *szOld)
{
    if (szNew == NULL || szOld == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_pivot_root, (long)(uintptr_t)szNew,
                             (long)(uintptr_t)szOld, 0, 0, 0, 0));
}

/* ---- xattr -------------------------------------------------------------- */

int
setxattr(const char *szPath, const char *szName, const void *pValue, size_t cb,
         int nFlags)
{
    return (int)sys_ret(sys6(NR_setxattr, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)szName, (long)(uintptr_t)pValue,
                             (long)cb, nFlags, 0));
}

int
lsetxattr(const char *szPath, const char *szName, const void *pValue, size_t cb,
          int nFlags)
{
    return (int)sys_ret(sys6(NR_lsetxattr, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)szName, (long)(uintptr_t)pValue,
                             (long)cb, nFlags, 0));
}

int
fsetxattr(int nFd, const char *szName, const void *pValue, size_t cb,
          int nFlags)
{
    return (int)sys_ret(sys6(NR_fsetxattr, nFd, (long)(uintptr_t)szName,
                             (long)(uintptr_t)pValue, (long)cb, nFlags, 0));
}

ssize_t
getxattr(const char *szPath, const char *szName, void *pValue, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_getxattr, (long)(uintptr_t)szPath,
                                 (long)(uintptr_t)szName,
                                 (long)(uintptr_t)pValue, (long)cb, 0, 0));
}

ssize_t
lgetxattr(const char *szPath, const char *szName, void *pValue, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_lgetxattr, (long)(uintptr_t)szPath,
                                 (long)(uintptr_t)szName,
                                 (long)(uintptr_t)pValue, (long)cb, 0, 0));
}

ssize_t
fgetxattr(int nFd, const char *szName, void *pValue, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_fgetxattr, nFd, (long)(uintptr_t)szName,
                                 (long)(uintptr_t)pValue, (long)cb, 0, 0));
}

ssize_t
listxattr(const char *szPath, char *szList, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_listxattr, (long)(uintptr_t)szPath,
                                 (long)(uintptr_t)szList, (long)cb, 0, 0, 0));
}

ssize_t
llistxattr(const char *szPath, char *szList, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_llistxattr, (long)(uintptr_t)szPath,
                                 (long)(uintptr_t)szList, (long)cb, 0, 0, 0));
}

ssize_t
flistxattr(int nFd, char *szList, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_flistxattr, nFd, (long)(uintptr_t)szList,
                                 (long)cb, 0, 0, 0));
}

int
removexattr(const char *szPath, const char *szName)
{
    return (int)sys_ret(sys6(NR_removexattr, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)szName, 0, 0, 0, 0));
}

int
lremovexattr(const char *szPath, const char *szName)
{
    return (int)sys_ret(sys6(NR_lremovexattr, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)szName, 0, 0, 0, 0));
}

int
fremovexattr(int nFd, const char *szName)
{
    return (int)sys_ret(sys6(NR_fremovexattr, nFd, (long)(uintptr_t)szName, 0,
                             0, 0, 0));
}

/* ---- membarrier / ioprio ------------------------------------------------ */

int
membarrier(int nCmd, unsigned uFlags, int nCpuId)
{
    return (int)sys_ret(sys6(NR_membarrier, nCmd, (long)uFlags, nCpuId, 0, 0,
                             0));
}

int
ioprio_set(int nWhich, int nWho, int nIoprio)
{
    return (int)sys_ret(sys6(NR_ioprio_set, nWhich, nWho, nIoprio, 0, 0, 0));
}

int
ioprio_get(int nWhich, int nWho)
{
    return (int)sys_ret(sys6(NR_ioprio_get, nWhich, nWho, 0, 0, 0, 0));
}
