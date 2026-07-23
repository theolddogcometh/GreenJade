/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Userspace syscall wrappers (clean-room; matches kernel GJ_SYS_* / Linux NR).
 * Pure C11. Dual-licensed MIT OR Apache-2.0. Header-only convenience inlines
 * plus gj_syscall6 from libgj (syscall.S).
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Native GJ numbers — must match kernel/include/gj/syscall.h */
#define GJ_SYS_DEBUG_LOG  0
#define GJ_SYS_YIELD      1
#define GJ_SYS_EXIT       2
#define GJ_SYS_IPC_CALL  10
#define GJ_SYS_IPC_RECV  11
#define GJ_SYS_IPC_REPLY 12
#define GJ_SYS_THREAD_SET_QOS 72
#define GJ_SYS_GPU_PRESENT       90
#define GJ_SYS_GPU_DISPLAY_INFO  91
#define GJ_SYS_MEMOBJ_CREATE_NAMED 92
#define GJ_SYS_MEMOBJ_MAP_NAMED  93
/**
 * HDA software stream (nr 94) — match kernel/include/gj/syscall.h:
 *   arg0 = GJ_HDA_OP_* (open/write/start/tick/close/stats)
 *   open:  arg1=channels arg2=rate_hz arg3=bits → 0 or -errno
 *   write: arg1=user ptr arg2=nbytes → ret bytes written
 *   start: — → 0 or -errno
 *   tick:  arg1=frames → ret bytes consumed
 *   close: — → 0
 *   stats: arg1=user ptr to u32[3] {queued, played, underruns} → 0
 */
#define GJ_SYS_HDA_STREAM        94
#define GJ_SYS_SESSION           95
#define GJ_SYS_NET               96
#define GJ_SYS_STORE             97
/**
 * Notification wait (nr 99) — native MSI-X / IRQ badge delivery:
 *   arg0 = which (GJ_NOTIFY_WHICH_MSIX_GLOBAL=0 only today)
 *   arg1 = mask (pending & mask; 0 = any non-zero pending)
 *   arg2 = block (0=poll, 1=block until signal)
 *   → pending badges cleared and returned (unsigned in rax)
 */
#define GJ_SYS_NOTIFY_WAIT       99
#define GJ_SYS_VFS               100
#define GJ_SYS_CONSOLE           101
/**
 * SCSI door (nr 102) — userspace scsi_mid → kernel virtio-scsi / HBA:
 *   arg0 = GJ_SCSI_OP_* (inquiry/read_cap/read10/write10/ready/stats)
 *   inquiry:   arg1=user buf arg2=cb → ret cb or -errno
 *   read_cap:  arg1=user u32[2] {last_lba, block_len} → 0
 *   read10/write10: arg1=lba arg2=user buf arg3=blocks (512B) → ret bytes
 *   ready:     — → 1 if transport ready, else 0
 *   stats:     arg1=user u32[2] {door_io, ready} → 0
 */
#define GJ_SYS_SCSI              102
#define GJ_SYS_PLATFORM_INFO     98

/* HDA stream ops (arg0 of GJ_SYS_HDA_STREAM) — match kernel native path */
#define GJ_HDA_OP_OPEN   0u
#define GJ_HDA_OP_WRITE  1u
#define GJ_HDA_OP_START  2u
#define GJ_HDA_OP_TICK   3u
#define GJ_HDA_OP_CLOSE  4u
#define GJ_HDA_OP_STATS  5u

/* Notify wait which (arg0 of GJ_SYS_NOTIFY_WAIT) */
#define GJ_NOTIFY_WHICH_MSIX_GLOBAL 0u

/* SCSI door ops (arg0 of GJ_SYS_SCSI) */
#define GJ_SCSI_OP_INQUIRY   0u /* arg1=buf arg2=cb → ret cb */
#define GJ_SCSI_OP_READ_CAP  1u /* arg1=u32[2] last_lba, block_len */
#define GJ_SCSI_OP_READ10    2u /* arg1=lba arg2=buf arg3=blocks */
#define GJ_SCSI_OP_WRITE10   3u /* arg1=lba arg2=buf arg3=blocks */
#define GJ_SCSI_OP_READY     5u /* → 1 if transport ready */
#define GJ_SCSI_OP_STATS     6u /* arg1=u32[2] door_io, ready */

/* Net door ops (arg0 of GJ_SYS_NET) — match kernel/include/gj/net_door.h */
#define GJ_NET_OP_POLL   1u
#define GJ_NET_OP_STATS  2u /* arg1=user u32[4] arp,udp,icmp,calls */
#define GJ_NET_OP_SOCKET 3u
#define GJ_NET_OP_BIND   4u
#define GJ_NET_OP_SEND   5u
#define GJ_NET_OP_RECV   6u
#define GJ_NET_OP_CONNECT 7u
#define GJ_NET_OP_CLOSE  8u
#define GJ_NET_OP_LISTEN 9u
#define GJ_NET_OP_CLAIM  10u
#define GJ_NET_OP_RELEASE 11u
#define GJ_NET_OP_ACCEPT 25u
#define GJ_NET_OP_TCP_STATS 26u

/* Store door ops (arg0 of GJ_SYS_STORE) — match kernel/include/gj/store_door.h */
#define GJ_STORE_OP_STATS  1u /* arg1 = u32[3]: blk_io, scsi_io, calls */
#define GJ_STORE_OP_CAP    2u /* arg1 = u64* capacity sectors */
#define GJ_STORE_OP_READ   3u
#define GJ_STORE_OP_WRITE  4u
#define GJ_STORE_OP_CLAIM  5u
#define GJ_STORE_OP_RELEASE 6u
#define GJ_STORE_OP_QUEUE_INFO 7u
#define GJ_STORE_OP_FLUSH  8u

/* VFS door ops (arg0 of GJ_SYS_VFS) — match kernel/include/gj/vfs_door.h */
#define GJ_VFS_OP_CLAIM    1u
#define GJ_VFS_OP_RELEASE  2u
#define GJ_VFS_OP_FORMAT   3u
#define GJ_VFS_OP_MOUNT    4u
#define GJ_VFS_OP_CREATE   5u
#define GJ_VFS_OP_READ     6u
#define GJ_VFS_OP_WRITE    7u
#define GJ_VFS_OP_UNLINK   8u
#define GJ_VFS_OP_STAT     9u
#define GJ_VFS_OP_LIST     10u
#define GJ_VFS_OP_STATS    11u
#define GJ_VFS_OP_OPEN     12u
#define GJ_VFS_OP_CLOSE    13u
#define GJ_VFS_OP_READFD   14u
#define GJ_VFS_OP_WRITEFD  15u
#define GJ_VFS_O_CREAT     1u
#define GJ_VFS_O_RDWR      2u

/* Session door ops (arg0 of GJ_SYS_SESSION) */
#define GJ_SESS_OP_PRESENT      1
#define GJ_SESS_OP_DISPLAY_INFO 2
#define GJ_SESS_OP_INPUT_POLL   3
#define GJ_SESS_OP_INPUT_POP    4
#define GJ_SESS_OP_STATS        5
#define GJ_SESS_OP_PRESENT_FB   6
#define GJ_SESS_OP_CLAIM        7
#define GJ_SESS_OP_RELEASE      8
#define GJ_SESS_OP_MAP_SCANOUT  9

/* Linux x86_64 numbers (subset) */
#define LINUX_NR_read           0
#define LINUX_NR_write          1
#define LINUX_NR_close          3
#define LINUX_NR_mmap           9
#define LINUX_NR_mprotect      10
#define LINUX_NR_munmap        11
#define LINUX_NR_brk           12
#define LINUX_NR_sched_yield   24
#define LINUX_NR_nanosleep     35
#define LINUX_NR_getpid        39
#define LINUX_NR_getppid      110
#define LINUX_NR_exit          60
#define LINUX_NR_uname         63
#define LINUX_NR_getcwd        79
#define LINUX_NR_fsync         74
#define LINUX_NR_getuid       102
#define LINUX_NR_gettimeofday  96
#define LINUX_NR_getrandom    318
#define LINUX_NR_futex        202
#define LINUX_NR_clock_gettime 228
#define LINUX_NR_clock_getres  229
#define LINUX_NR_openat       257
#define LINUX_NR_pipe2        293
#define LINUX_NR_eventfd2     290
#define LINUX_NR_getdents64   217
#define LINUX_NR_link          86
#define LINUX_NR_readlink      89
#define LINUX_NR_symlink       88
#define LINUX_NR_fork          57
#define LINUX_NR_wait4         61
#define LINUX_NR_execve        59
#define LINUX_NR_lseek          8
#define LINUX_NR_fstat          5
#define LINUX_NR_rt_sigprocmask 14
#define LINUX_NR_arch_prctl    158

/**
 * Raw SYSCALL: nr in rax; a0..a5 → rdi,rsi,rdx,r10,r8,r9.
 * Returns rax (signed error as negative Linux errno when personality=LINUX).
 */
long gj_syscall6(long nr, long a0, long a1, long a2, long a3, long a4, long a5);

static inline long gj_syscall0(long nr)
{
    return gj_syscall6(nr, 0, 0, 0, 0, 0, 0);
}
static inline long gj_syscall1(long nr, long a0)
{
    return gj_syscall6(nr, a0, 0, 0, 0, 0, 0);
}
static inline long gj_syscall2(long nr, long a0, long a1)
{
    return gj_syscall6(nr, a0, a1, 0, 0, 0, 0);
}
static inline long gj_syscall3(long nr, long a0, long a1, long a2)
{
    return gj_syscall6(nr, a0, a1, a2, 0, 0, 0);
}
static inline long gj_syscall4(long nr, long a0, long a1, long a2, long a3)
{
    return gj_syscall6(nr, a0, a1, a2, a3, 0, 0);
}
static inline long gj_syscall5(long nr, long a0, long a1, long a2, long a3,
                               long a4)
{
    return gj_syscall6(nr, a0, a1, a2, a3, a4, 0);
}

/* ---- Native convenience ---- */
static inline void gj_yield(void)
{
    (void)gj_syscall0(GJ_SYS_YIELD);
}
static inline void gj_exit(int code) __attribute__((noreturn));
static inline void gj_exit(int code)
{
    (void)gj_syscall1(GJ_SYS_EXIT, (long)code);
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
static inline long gj_debug_log(const char *sz, long cb)
{
    return gj_syscall2(GJ_SYS_DEBUG_LOG, (long)(uintptr_t)sz, cb);
}
static inline long gj_gpu_present(unsigned w, unsigned h, void *pFb,
                                  unsigned stride)
{
    return gj_syscall6(GJ_SYS_GPU_PRESENT, (long)w, (long)h,
                       (long)(uintptr_t)pFb, (long)stride, 0, 0);
}
static inline long gj_gpu_display_info(unsigned *pWH2)
{
    return gj_syscall1(GJ_SYS_GPU_DISPLAY_INFO, (long)(uintptr_t)pWH2);
}
static inline long gj_set_qos(unsigned thr_id, unsigned qos)
{
    return gj_syscall2(GJ_SYS_THREAD_SET_QOS, (long)thr_id, (long)qos);
}
static inline long gj_session(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_SESSION, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_session_claim(unsigned token)
{
    return gj_session(GJ_SESS_OP_CLAIM, (long)token, 0, 0);
}
static inline long gj_session_release(unsigned token)
{
    return gj_session(GJ_SESS_OP_RELEASE, (long)token, 0, 0);
}
static inline long gj_session_present_fb(unsigned w, unsigned h, void *pFb)
{
    return gj_session(GJ_SESS_OP_PRESENT_FB, (long)w, (long)h,
                      (long)(uintptr_t)pFb);
}
static inline long gj_net(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_NET, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_store(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_STORE, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_vfs(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_VFS, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_console_poll(void)
{
    return gj_syscall1(GJ_SYS_CONSOLE, 0);
}
static inline long gj_console_getc(void)
{
    return gj_syscall1(GJ_SYS_CONSOLE, 1);
}
static inline long gj_console_read(void *pBuf, unsigned cb)
{
    return gj_syscall3(GJ_SYS_CONSOLE, 2, (long)(uintptr_t)pBuf, (long)cb);
}
/**
 * HDA stream door: op = GJ_HDA_OP_*; see GJ_SYS_HDA_STREAM.
 * open(ch,rate,bits); write(buf,nbytes,0); start/close; tick(frames);
 * stats(u32[3]*).
 */
static inline long gj_hda_stream(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_HDA_STREAM, (long)op, a1, a2, a3, 0, 0);
}

/**
 * SCSI door: op = GJ_SCSI_OP_*; see GJ_SYS_SCSI.
 * Userspace scsi_mid product path over kernel virtio-scsi interim.
 */
static inline long gj_scsi(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_SCSI, (long)op, a1, a2, a3, 0, 0);
}

/**
 * Wait/poll notification badges. which=GJ_NOTIFY_WHICH_MSIX_GLOBAL (0);
 * mask selects badges; block=0 poll / 1 sleep. Returns cleared pending.
 */
static inline long gj_notify_wait(unsigned which, unsigned long mask, int block)
{
    return gj_syscall3(GJ_SYS_NOTIFY_WAIT, (long)which, (long)mask, (long)block);
}

static inline long gj_platform_info(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_PLATFORM_INFO, (long)op, a1, a2, a3, 0, 0);
}
/** Create named shareable memobj (arg1 = page count). 0 = ok. */
static inline long gj_memobj_create_named(const char *szName, unsigned cPages)
{
    return gj_syscall2(GJ_SYS_MEMOBJ_CREATE_NAMED, (long)(uintptr_t)szName,
                       (long)cPages);
}
/** Map named memobj; returns VA or 0 / negative. prot: 1=R 2=W 3=RW */
static inline long gj_memobj_map_named(const char *szName, unsigned long uHint,
                                      unsigned uProt)
{
    return gj_syscall3(GJ_SYS_MEMOBJ_MAP_NAMED, (long)(uintptr_t)szName,
                       (long)uHint, (long)uProt);
}
static inline long linux_futex(volatile void *pUaddr, int nOp, unsigned uVal,
                               const void *pTimeout, void *pUaddr2,
                               unsigned uVal3)
{
    return gj_syscall6(LINUX_NR_futex, (long)(uintptr_t)pUaddr, (long)nOp,
                       (long)uVal, (long)(uintptr_t)pTimeout,
                       (long)(uintptr_t)pUaddr2, (long)uVal3);
}

/* ---- Linux convenience (personality LINUX) ---- */
static inline long linux_write(int fd, const void *p, size_t cb)
{
    return gj_syscall3(LINUX_NR_write, (long)fd, (long)(uintptr_t)p, (long)cb);
}
static inline long linux_read(int fd, void *p, size_t cb)
{
    return gj_syscall3(LINUX_NR_read, (long)fd, (long)(uintptr_t)p, (long)cb);
}
static inline long linux_getpid(void)
{
    return gj_syscall0(LINUX_NR_getpid);
}
static inline void linux_exit(int code) __attribute__((noreturn));
static inline void linux_exit(int code)
{
    (void)gj_syscall1(LINUX_NR_exit, (long)code);
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
static inline long linux_sched_yield(void)
{
    return gj_syscall0(LINUX_NR_sched_yield);
}
static inline long linux_uname(void *pUts)
{
    return gj_syscall1(LINUX_NR_uname, (long)(uintptr_t)pUts);
}
static inline long linux_clock_gettime(int clockid, void *pTs)
{
    return gj_syscall2(LINUX_NR_clock_gettime, (long)clockid,
                       (long)(uintptr_t)pTs);
}
static inline long linux_mmap(void *addr, size_t len, int prot, int flags,
                              int fd, long off)
{
    return gj_syscall6(LINUX_NR_mmap, (long)(uintptr_t)addr, (long)len,
                       (long)prot, (long)flags, (long)fd, off);
}
static inline long linux_munmap(void *addr, size_t len)
{
    return gj_syscall2(LINUX_NR_munmap, (long)(uintptr_t)addr, (long)len);
}
static inline long linux_mprotect(void *addr, size_t len, int prot)
{
    return gj_syscall3(LINUX_NR_mprotect, (long)(uintptr_t)addr, (long)len,
                       (long)prot);
}
static inline long linux_close(int fd)
{
    return gj_syscall1(LINUX_NR_close, (long)fd);
}
static inline long linux_openat(int dfd, const char *path, int flags, int mode)
{
    return gj_syscall6(LINUX_NR_openat, (long)dfd, (long)(uintptr_t)path,
                       (long)flags, (long)mode, 0, 0);
}
static inline long linux_brk(void *addr)
{
    return gj_syscall1(LINUX_NR_brk, (long)(uintptr_t)addr);
}
static inline long linux_getppid(void)
{
    return gj_syscall0(LINUX_NR_getppid);
}
static inline long linux_getuid(void)
{
    return gj_syscall0(LINUX_NR_getuid);
}
static inline long linux_getcwd(char *buf, size_t cb)
{
    return gj_syscall2(LINUX_NR_getcwd, (long)(uintptr_t)buf, (long)cb);
}
static inline long linux_fsync(int fd)
{
    return gj_syscall1(LINUX_NR_fsync, (long)fd);
}
static inline long linux_gettimeofday(void *tv, void *tz)
{
    return gj_syscall2(LINUX_NR_gettimeofday, (long)(uintptr_t)tv,
                       (long)(uintptr_t)tz);
}
static inline long linux_getrandom(void *buf, size_t cb, unsigned flags)
{
    return gj_syscall3(LINUX_NR_getrandom, (long)(uintptr_t)buf, (long)cb,
                       (long)flags);
}
static inline long linux_clock_getres(int clockid, void *pTs)
{
    return gj_syscall2(LINUX_NR_clock_getres, (long)clockid,
                       (long)(uintptr_t)pTs);
}
static inline long linux_pipe2(int *fds, int flags)
{
    return gj_syscall2(LINUX_NR_pipe2, (long)(uintptr_t)fds, (long)flags);
}
static inline long linux_eventfd2(unsigned initval, int flags)
{
    return gj_syscall2(LINUX_NR_eventfd2, (long)initval, (long)flags);
}
static inline long linux_getdents64(int fd, void *dirp, size_t count)
{
    return gj_syscall3(LINUX_NR_getdents64, (long)fd, (long)(uintptr_t)dirp,
                       (long)count);
}
static inline long linux_link(const char *oldp, const char *newp)
{
    return gj_syscall2(LINUX_NR_link, (long)(uintptr_t)oldp,
                       (long)(uintptr_t)newp);
}
static inline long linux_readlink(const char *path, char *buf, size_t cb)
{
    return gj_syscall3(LINUX_NR_readlink, (long)(uintptr_t)path,
                       (long)(uintptr_t)buf, (long)cb);
}
static inline long linux_symlink(const char *target, const char *linkpath)
{
    return gj_syscall2(LINUX_NR_symlink, (long)(uintptr_t)target,
                       (long)(uintptr_t)linkpath);
}
static inline long linux_pipe(int *fds)
{
    return linux_pipe2(fds, 0);
}
static inline long linux_nanosleep(const void *req, void *rem)
{
    return gj_syscall2(LINUX_NR_nanosleep, (long)(uintptr_t)req,
                       (long)(uintptr_t)rem);
}
static inline long linux_fork(void)
{
    return gj_syscall0(LINUX_NR_fork);
}
static inline long linux_wait4(int pid, int *status, int options, void *rusage)
{
    return gj_syscall6(LINUX_NR_wait4, (long)pid, (long)(uintptr_t)status,
                       (long)options, (long)(uintptr_t)rusage, 0, 0);
}
static inline long linux_execve(const char *path, char *const *argv,
                                char *const *envp)
{
    return gj_syscall3(LINUX_NR_execve, (long)(uintptr_t)path,
                       (long)(uintptr_t)argv, (long)(uintptr_t)envp);
}
static inline long linux_lseek(int fd, long off, int whence)
{
    return gj_syscall3(LINUX_NR_lseek, (long)fd, off, (long)whence);
}
static inline long linux_fstat(int fd, void *pStat)
{
    return gj_syscall2(LINUX_NR_fstat, (long)fd, (long)(uintptr_t)pStat);
}
static inline long linux_arch_prctl(int code, unsigned long addr)
{
    return gj_syscall2(LINUX_NR_arch_prctl, (long)code, (long)addr);
}
static inline long linux_munmap_alias(void *addr, size_t len)
{
    return linux_munmap(addr, len);
}

#ifdef __cplusplus
}
#endif
