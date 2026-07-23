/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Userspace syscall wrappers (clean-room; matches kernel GJ_SYS_* / Linux NR).
 * Pure C11. Dual-licensed MIT OR Apache-2.0. Header-only convenience inlines
 * plus gj_syscall6 from libgj (syscall.S).
 *
 * Soft deepen: numbers and door ops track kernel headers; wrappers are thin
 * SYSCALL shims (no policy). Freestanding product path only — not a libc.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Native GJ syscall numbers — must match kernel/include/gj/syscall.h
 * (sparse blocks by subsystem; do not renumber).
 * ======================================================================== */

/* Diagnostics / scheduling */
#define GJ_SYS_DEBUG_LOG         0
#define GJ_SYS_YIELD             1
#define GJ_SYS_EXIT              2

/* Door IPC (cold personality / server side) */
#define GJ_SYS_IPC_CALL         10
#define GJ_SYS_IPC_RECV         11
#define GJ_SYS_IPC_REPLY        12

/* Capability table (numbers frozen) */
#define GJ_SYS_CAP_MINT         20
#define GJ_SYS_CAP_MOVE         21
#define GJ_SYS_CAP_COPY         22
#define GJ_SYS_CAP_REVOKE       23
#define GJ_SYS_CAP_IDENT        24

/* Untyped / VM (reserved / partial) */
#define GJ_SYS_UNTYPED_RETYPE   30
#define GJ_SYS_VM_MAP           40
#define GJ_SYS_VM_UNMAP         41
#define GJ_SYS_VM_PROTECT       42
#define GJ_SYS_VM_MAP_OBJ       43

/* Process control */
#define GJ_SYS_PROCESS_SET_PAGER 50
#define GJ_SYS_PROCESS_SPAWN      51  /* arg0=entry arg1=arg arg2=flags → slot|gen */
#define GJ_SYS_PROCESS_KILL       52

/* Wait / futex / thread (native path) */
#define GJ_SYS_WAIT_TIMEOUT      60
#define GJ_SYS_FUTEX_WAIT        70
#define GJ_SYS_FUTEX_WAKE        71
#define GJ_SYS_THREAD_SET_QOS    72  /* arg0=tid(0=cur) arg1=qos class */
#define GJ_SYS_THREAD_SET_CPU    73  /* arg0=tid(0=cur) arg1=cpu slot */

/* Cold IPC scaffolds (G-COLD-2: deprecate after userspace door server) */
#define GJ_SYS_COLD_DEQUEUE      80
#define GJ_SYS_COLD_REPLY        81
#define GJ_SYS_PERSONALITY_SERVE 82  /* arg0=user ptr to gj_linux_regs */

/* GPU / compositor present (virtio-gpu) */
#define GJ_SYS_GPU_PRESENT       90
#define GJ_SYS_GPU_DISPLAY_INFO  91

/* Named memory objects (shared scanout / IPC buffers) */
#define GJ_SYS_MEMOBJ_CREATE_NAMED 92
#define GJ_SYS_MEMOBJ_MAP_NAMED     93

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
 * Platform info (nr 98):
 *   arg0=0 → IOMMU: arg1=user ptr to info; ret=present
 *   arg0=1 → MSI-X inventory: ret=count, optional arg1=info array
 *   arg0=2 → WoW64: arg1=0 query, 1 enable, 2 disable → calls|enabled
 *   arg0=3 → MSI-X IRQ notify soft inject badge=arg1; ret=irq count
 *   arg0=4 → IOMMU enforce: arg1=0/1 set; ret=enforce
 *   arg0=5 → IOMMU window grant: arg1=BDF arg2=pa arg3=cb
 */
#define GJ_SYS_PLATFORM_INFO     98

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

/* ---- HDA stream ops (arg0 of GJ_SYS_HDA_STREAM) ---- */
#define GJ_HDA_OP_OPEN   0u
#define GJ_HDA_OP_WRITE  1u
#define GJ_HDA_OP_START  2u
#define GJ_HDA_OP_TICK   3u
#define GJ_HDA_OP_CLOSE  4u
#define GJ_HDA_OP_STATS  5u

/* Notify wait which (arg0 of GJ_SYS_NOTIFY_WAIT) */
#define GJ_NOTIFY_WHICH_MSIX_GLOBAL 0u

/* ---- SCSI door ops (arg0 of GJ_SYS_SCSI) ---- */
#define GJ_SCSI_OP_INQUIRY   0u /* arg1=buf arg2=cb → ret cb */
#define GJ_SCSI_OP_READ_CAP  1u /* arg1=u32[2] last_lba, block_len */
#define GJ_SCSI_OP_READ10    2u /* arg1=lba arg2=buf arg3=blocks */
#define GJ_SCSI_OP_WRITE10   3u /* arg1=lba arg2=buf arg3=blocks */
/* 4 reserved */
#define GJ_SCSI_OP_READY     5u /* → 1 if transport ready */
#define GJ_SCSI_OP_STATS     6u /* arg1=u32[2] door_io, ready */

/* ---- Net door ops (arg0 of GJ_SYS_NET) — match kernel net_door.h ---- */
#define GJ_NET_OP_POLL        1u
#define GJ_NET_OP_STATS       2u /* arg1=user u32[4] arp,udp,icmp,calls */
#define GJ_NET_OP_SOCKET      3u
#define GJ_NET_OP_BIND        4u
#define GJ_NET_OP_SEND        5u
#define GJ_NET_OP_RECV        6u
#define GJ_NET_OP_CONNECT     7u
#define GJ_NET_OP_CLOSE       8u
#define GJ_NET_OP_LISTEN      9u
#define GJ_NET_OP_CLAIM      10u
#define GJ_NET_OP_RELEASE    11u
/*
 * Virtio / UDX ring ops (kernel net_door.h) — numeric ABI (soft deepen):
 *   12 VIRTIO_TX  13 VIRTIO_RX  14 QUEUE_INFO  15 EXPORT_RING  16 MAP_RING
 *   17 KICK  18 AVAIL_PUSH  19 USED_REAP  20 RING_STATE  21 MAP_DMA
 *   22 DESC_ALLOC  23 USER_AVAIL  24 BOUNCE_FILL
 * Macros for 12..24 are provided only when the translation unit has not
 * already defined them (netstackd_gj still carries local fallbacks without
 * #ifndef; defining them here would -Werror redefine). Prefer the
 * gj_net_*_ring convenience wrappers below once local fallbacks drop.
 */
#if !defined(GJ_NET_OP_VIRTIO_TX) && defined(GJ_LIBGJ_NET_RING_OPS)
#define GJ_NET_OP_VIRTIO_TX   12u
#define GJ_NET_OP_VIRTIO_RX   13u
#define GJ_NET_OP_QUEUE_INFO  14u
#define GJ_NET_OP_EXPORT_RING 15u
#define GJ_NET_OP_MAP_RING    16u
#define GJ_NET_OP_KICK        17u
#define GJ_NET_OP_AVAIL_PUSH  18u
#define GJ_NET_OP_USED_REAP   19u
#define GJ_NET_OP_RING_STATE  20u
#define GJ_NET_OP_MAP_DMA     21u
#define GJ_NET_OP_DESC_ALLOC  22u
#define GJ_NET_OP_USER_AVAIL  23u
#define GJ_NET_OP_BOUNCE_FILL 24u
#endif
#define GJ_NET_OP_ACCEPT      25u
#define GJ_NET_OP_TCP_STATS   26u

/* Soft net socket domain/type + bounce — opt-in (see GJ_LIBGJ_NET_RING_OPS). */
#if defined(GJ_LIBGJ_NET_RING_OPS)
#ifndef GJ_AF_INET
#define GJ_AF_INET       2
#endif
#ifndef GJ_SOCK_STREAM
#define GJ_SOCK_STREAM   1
#endif
#ifndef GJ_SOCK_DGRAM
#define GJ_SOCK_DGRAM    2
#endif
#ifndef GJ_NET_XFER_MAX
/** SEND/RECV bounce ceiling (bytes); match kernel NET_XFER_MAX. */
#define GJ_NET_XFER_MAX  4096u
#endif
#endif

/* ---- Store door ops (arg0 of GJ_SYS_STORE) — match store_door.h ---- */
#define GJ_STORE_OP_STATS       1u /* arg1 = u32[3]: blk_io, scsi_io, calls */
#define GJ_STORE_OP_CAP         2u /* arg1 = u64* capacity sectors */
#define GJ_STORE_OP_READ        3u
#define GJ_STORE_OP_WRITE       4u
#define GJ_STORE_OP_CLAIM       5u
#define GJ_STORE_OP_RELEASE     6u
#define GJ_STORE_OP_QUEUE_INFO  7u
#define GJ_STORE_OP_FLUSH       8u
#define GJ_STORE_OP_EXPORT_RING 9u
#define GJ_STORE_OP_KICK       10u
#define GJ_STORE_OP_RING_STATE 11u
#define GJ_STORE_OP_MAP_RING   12u
/** Bounce / single-call transfer ceiling; match kernel GJ_STORE_XFER_MAX. */
#define GJ_STORE_XFER_MAX 4096u

/* ---- VFS door ops (arg0 of GJ_SYS_VFS) — match vfs_door.h ---- */
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
#define GJ_VFS_OP_SEEKFD   16u
#define GJ_VFS_O_CREAT     1u
#define GJ_VFS_O_RDWR      2u
#define GJ_VFS_SEEK_SET    0u
#define GJ_VFS_SEEK_CUR    1u
#define GJ_VFS_SEEK_END    2u

/* ---- Session door ops (arg0 of GJ_SYS_SESSION) — match session_door.h ---- */
#define GJ_SESS_OP_PRESENT      1u
#define GJ_SESS_OP_DISPLAY_INFO 2u
#define GJ_SESS_OP_INPUT_POLL   3u
#define GJ_SESS_OP_INPUT_POP    4u
#define GJ_SESS_OP_STATS        5u
#define GJ_SESS_OP_PRESENT_FB   6u
#define GJ_SESS_OP_CLAIM        7u
#define GJ_SESS_OP_RELEASE      8u
#define GJ_SESS_OP_MAP_SCANOUT  9u

/* ---- Platform info ops (arg0 of GJ_SYS_PLATFORM_INFO) ---- */
#define GJ_PLAT_OP_IOMMU_INFO      0u
#define GJ_PLAT_OP_MSIX_INVENTORY  1u
#define GJ_PLAT_OP_WOW64           2u
#define GJ_PLAT_OP_MSIX_INJECT     3u
#define GJ_PLAT_OP_IOMMU_ENFORCE   4u
#define GJ_PLAT_OP_IOMMU_GRANT     5u

/* ---- Console ops (arg0 of GJ_SYS_CONSOLE) ---- */
#define GJ_CONSOLE_OP_POLL  0u
#define GJ_CONSOLE_OP_GETC  1u
#define GJ_CONSOLE_OP_READ  2u

/* ========================================================================
 * Linux x86_64 numbers (subset used by freestanding / hybrid path)
 * Align with kernel/include/gj/linux_abi.h where present.
 * ======================================================================== */
#define LINUX_NR_read             0
#define LINUX_NR_write            1
#define LINUX_NR_open             2
#define LINUX_NR_close            3
#define LINUX_NR_stat             4
#define LINUX_NR_fstat            5
#define LINUX_NR_lstat            6
#define LINUX_NR_poll             7
#define LINUX_NR_lseek            8
#define LINUX_NR_mmap             9
#define LINUX_NR_mprotect        10
#define LINUX_NR_munmap          11
#define LINUX_NR_brk             12
#define LINUX_NR_rt_sigaction    13
#define LINUX_NR_rt_sigprocmask  14
#define LINUX_NR_ioctl           16
#define LINUX_NR_pread64         17
#define LINUX_NR_pwrite64        18
#define LINUX_NR_readv           19
#define LINUX_NR_writev          20
#define LINUX_NR_access          21
#define LINUX_NR_pipe            22
#define LINUX_NR_select          23
#define LINUX_NR_sched_yield     24
#define LINUX_NR_mremap          25
#define LINUX_NR_madvise         28
#define LINUX_NR_dup             32
#define LINUX_NR_dup2            33
#define LINUX_NR_pause           34
#define LINUX_NR_nanosleep       35
#define LINUX_NR_getpid          39
#define LINUX_NR_socket          41
#define LINUX_NR_connect         42
#define LINUX_NR_accept          43
#define LINUX_NR_sendto          44
#define LINUX_NR_recvfrom        45
#define LINUX_NR_sendmsg         46
#define LINUX_NR_recvmsg         47
#define LINUX_NR_shutdown        48
#define LINUX_NR_bind            49
#define LINUX_NR_listen          50
#define LINUX_NR_getsockname     51
#define LINUX_NR_getpeername     52
#define LINUX_NR_socketpair      53
#define LINUX_NR_setsockopt      54
#define LINUX_NR_getsockopt      55
#define LINUX_NR_clone           56
#define LINUX_NR_fork            57
#define LINUX_NR_vfork           58
#define LINUX_NR_execve          59
#define LINUX_NR_exit            60
#define LINUX_NR_wait4           61
#define LINUX_NR_kill            62
#define LINUX_NR_uname           63
#define LINUX_NR_fcntl           72
#define LINUX_NR_flock           73
#define LINUX_NR_fsync           74
#define LINUX_NR_fdatasync       75
#define LINUX_NR_ftruncate       77
#define LINUX_NR_getdents        78
#define LINUX_NR_getcwd          79
#define LINUX_NR_chdir           80
#define LINUX_NR_fchdir          81
#define LINUX_NR_rename          82
#define LINUX_NR_mkdir           83
#define LINUX_NR_rmdir           84
#define LINUX_NR_creat           85
#define LINUX_NR_link            86
#define LINUX_NR_unlink          87
#define LINUX_NR_symlink         88
#define LINUX_NR_readlink        89
#define LINUX_NR_chmod           90
#define LINUX_NR_fchmod          91
#define LINUX_NR_gettimeofday    96
#define LINUX_NR_getrlimit       97
#define LINUX_NR_getrusage       98
#define LINUX_NR_sysinfo         99
#define LINUX_NR_times          100
#define LINUX_NR_getuid         102
#define LINUX_NR_getgid         104
#define LINUX_NR_geteuid        107
#define LINUX_NR_getegid        108
#define LINUX_NR_getppid        110
#define LINUX_NR_getpgrp        111
#define LINUX_NR_setsid         112
#define LINUX_NR_arch_prctl     158
#define LINUX_NR_sync           162
#define LINUX_NR_gettid         186
#define LINUX_NR_tkill          200
#define LINUX_NR_time           201
#define LINUX_NR_futex          202
#define LINUX_NR_sched_setaffinity 203
#define LINUX_NR_sched_getaffinity 204
#define LINUX_NR_getdents64     217
#define LINUX_NR_set_tid_address 218
#define LINUX_NR_clock_gettime  228
#define LINUX_NR_clock_getres   229
#define LINUX_NR_clock_nanosleep 230
#define LINUX_NR_exit_group     231
#define LINUX_NR_epoll_wait     232
#define LINUX_NR_epoll_ctl      233
#define LINUX_NR_tgkill         234
#define LINUX_NR_waitid         247
#define LINUX_NR_openat         257
#define LINUX_NR_mkdirat        258
#define LINUX_NR_newfstatat     262
#define LINUX_NR_unlinkat       263
#define LINUX_NR_renameat       264
#define LINUX_NR_linkat         265
#define LINUX_NR_symlinkat      266
#define LINUX_NR_readlinkat     267
#define LINUX_NR_fchmodat       268
#define LINUX_NR_faccessat      269
#define LINUX_NR_ppoll          271
#define LINUX_NR_accept4        288
#define LINUX_NR_eventfd2       290
#define LINUX_NR_epoll_create1  291
#define LINUX_NR_dup3           292
#define LINUX_NR_pipe2          293
#define LINUX_NR_prlimit64      302
#define LINUX_NR_syncfs         306
#define LINUX_NR_getrandom      318
#define LINUX_NR_memfd_create   319
#define LINUX_NR_execveat       322
#define LINUX_NR_statx          332
#define LINUX_NR_clone3         435
#define LINUX_NR_close_range    436
#define LINUX_NR_openat2        437

/* Soft freestanding flag shapes (Linux ABI; not policy) */
#define GJ_AT_FDCWD            (-100)
#define GJ_PROT_READ           0x1
#define GJ_PROT_WRITE          0x2
#define GJ_PROT_EXEC           0x4
#define GJ_PROT_NONE           0x0
#define GJ_MAP_SHARED          0x01
#define GJ_MAP_PRIVATE         0x02
#define GJ_MAP_FIXED           0x10
#define GJ_MAP_ANONYMOUS       0x20
#define GJ_O_RDONLY            0
#define GJ_O_WRONLY            1
#define GJ_O_RDWR              2
#define GJ_O_CREAT             0x40
#define GJ_O_EXCL              0x80
#define GJ_O_TRUNC             0x200
#define GJ_O_APPEND            0x400
#define GJ_O_NONBLOCK          0x800
#define GJ_O_DIRECTORY         0x10000
#define GJ_O_CLOEXEC           0x80000
#define GJ_CLOCK_REALTIME      0
#define GJ_CLOCK_MONOTONIC     1
#define GJ_SEEK_SET            0
#define GJ_SEEK_CUR            1
#define GJ_SEEK_END            2
#define GJ_FUTEX_WAIT          0
#define GJ_FUTEX_WAKE          1
#define GJ_FUTEX_PRIVATE_FLAG  128
#define GJ_FUTEX_WAIT_PRIVATE  (GJ_FUTEX_WAIT | GJ_FUTEX_PRIVATE_FLAG)
#define GJ_FUTEX_WAKE_PRIVATE  (GJ_FUTEX_WAKE | GJ_FUTEX_PRIVATE_FLAG)
#define GJ_MEMOBJ_PROT_R       1u
#define GJ_MEMOBJ_PROT_W       2u
#define GJ_MEMOBJ_PROT_RW      3u

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

/* Door IPC (cold server / protonrt path) */
static inline long gj_ipc_call(long a0, long a1, long a2, long a3)
{
    return gj_syscall4(GJ_SYS_IPC_CALL, a0, a1, a2, a3);
}
static inline long gj_ipc_recv(void *pRegs)
{
    return gj_syscall1(GJ_SYS_IPC_RECV, (long)(uintptr_t)pRegs);
}
static inline long gj_ipc_reply(long i64Val)
{
    return gj_syscall1(GJ_SYS_IPC_REPLY, i64Val);
}
static inline long gj_personality_serve(void *pRegs)
{
    return gj_syscall1(GJ_SYS_PERSONALITY_SERVE, (long)(uintptr_t)pRegs);
}
static inline long gj_cold_dequeue(void *pReq)
{
    return gj_syscall1(GJ_SYS_COLD_DEQUEUE, (long)(uintptr_t)pReq);
}
static inline long gj_cold_reply(long token, long i64Ret)
{
    return gj_syscall2(GJ_SYS_COLD_REPLY, token, i64Ret);
}

/* Process / thread soft wrappers */
static inline long gj_process_spawn(unsigned long uEntry, unsigned long uArg,
                                    unsigned uFlags)
{
    return gj_syscall3(GJ_SYS_PROCESS_SPAWN, (long)uEntry, (long)uArg,
                       (long)uFlags);
}
static inline long gj_process_kill(unsigned long uSlotGen)
{
    return gj_syscall1(GJ_SYS_PROCESS_KILL, (long)uSlotGen);
}
static inline long gj_set_qos(unsigned thr_id, unsigned qos)
{
    return gj_syscall2(GJ_SYS_THREAD_SET_QOS, (long)thr_id, (long)qos);
}
static inline long gj_set_cpu(unsigned thr_id, unsigned cpu)
{
    return gj_syscall2(GJ_SYS_THREAD_SET_CPU, (long)thr_id, (long)cpu);
}

/* Cap table soft wrappers (numbers frozen; kernel path as implemented) */
static inline long gj_cap_mint(long a0, long a1, long a2, long a3)
{
    return gj_syscall4(GJ_SYS_CAP_MINT, a0, a1, a2, a3);
}
static inline long gj_cap_move(long a0, long a1, long a2, long a3)
{
    return gj_syscall4(GJ_SYS_CAP_MOVE, a0, a1, a2, a3);
}
static inline long gj_cap_copy(long a0, long a1, long a2, long a3)
{
    return gj_syscall4(GJ_SYS_CAP_COPY, a0, a1, a2, a3);
}
static inline long gj_cap_revoke(long a0, long a1, long a2, long a3)
{
    return gj_syscall4(GJ_SYS_CAP_REVOKE, a0, a1, a2, a3);
}
static inline long gj_cap_ident(long a0, long a1, long a2, long a3)
{
    return gj_syscall4(GJ_SYS_CAP_IDENT, a0, a1, a2, a3);
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

/* Session door */
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
static inline long gj_session_display_info(unsigned *pWH2)
{
    return gj_session(GJ_SESS_OP_DISPLAY_INFO, (long)(uintptr_t)pWH2, 0, 0);
}
static inline long gj_session_input_poll(void)
{
    return gj_session(GJ_SESS_OP_INPUT_POLL, 0, 0, 0);
}
static inline long gj_session_input_pop(void *pEv)
{
    return gj_session(GJ_SESS_OP_INPUT_POP, (long)(uintptr_t)pEv, 0, 0);
}
static inline long gj_session_stats(void *pU32x5)
{
    return gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)pU32x5, 0, 0);
}
static inline long gj_session_map_scanout(unsigned long *pVaHint,
                                          unsigned *pWHStride3)
{
    return gj_session(GJ_SESS_OP_MAP_SCANOUT, (long)(uintptr_t)pVaHint,
                      (long)(uintptr_t)pWHStride3, 0);
}

/* Net door */
static inline long gj_net(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_NET, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_net_claim(unsigned token)
{
    return gj_net(GJ_NET_OP_CLAIM, (long)token, 0, 0);
}
static inline long gj_net_release(unsigned token)
{
    return gj_net(GJ_NET_OP_RELEASE, (long)token, 0, 0);
}
static inline long gj_net_socket(int domain, int type, int proto)
{
    return gj_net(GJ_NET_OP_SOCKET, (long)domain, (long)type, (long)proto);
}
static inline long gj_net_bind(long hFd, unsigned uPort)
{
    return gj_net(GJ_NET_OP_BIND, hFd, (long)uPort, 0);
}
static inline long gj_net_listen(long hFd)
{
    return gj_net(GJ_NET_OP_LISTEN, hFd, 0, 0);
}
static inline long gj_net_connect(long hFd, unsigned uPort)
{
    return gj_net(GJ_NET_OP_CONNECT, hFd, (long)uPort, 0);
}
static inline long gj_net_accept(long hListen)
{
    return gj_net(GJ_NET_OP_ACCEPT, hListen, 0, 0);
}
static inline long gj_net_send(long hFd, const void *p, size_t cb)
{
    return gj_net(GJ_NET_OP_SEND, hFd, (long)(uintptr_t)p, (long)cb);
}
static inline long gj_net_recv(long hFd, void *p, size_t cb)
{
    return gj_net(GJ_NET_OP_RECV, hFd, (long)(uintptr_t)p, (long)cb);
}
static inline long gj_net_close(long hFd)
{
    return gj_net(GJ_NET_OP_CLOSE, hFd, 0, 0);
}
static inline long gj_net_poll(void)
{
    return gj_net(GJ_NET_OP_POLL, 0, 0, 0);
}
static inline long gj_net_stats(void *pU32x4)
{
    return gj_net(GJ_NET_OP_STATS, (long)(uintptr_t)pU32x4, 0, 0);
}
static inline long gj_net_tcp_stats(void *pU32x4)
{
    return gj_net(GJ_NET_OP_TCP_STATS, (long)(uintptr_t)pU32x4, 0, 0);
}
/* Ring / UDX soft wrappers use fixed op numbers (kernel net_door.h 12..24). */
static inline long gj_net_virtio_tx(const void *p, size_t cb)
{
    return gj_net(12u, (long)(uintptr_t)p, (long)cb, 0);
}
static inline long gj_net_virtio_rx(void *p, size_t cbMax)
{
    return gj_net(13u, (long)(uintptr_t)p, (long)cbMax, 0);
}
static inline long gj_net_queue_info(void *pU32x5)
{
    return gj_net(14u, (long)(uintptr_t)pU32x5, 0, 0);
}
static inline long gj_net_export_ring(unsigned which, void *pExport)
{
    return gj_net(15u, (long)which, (long)(uintptr_t)pExport, 0);
}
static inline long gj_net_map_ring(unsigned which, unsigned long uVa,
                                   void *pExportOpt)
{
    return gj_net(16u, (long)which, (long)uVa, (long)(uintptr_t)pExportOpt);
}
static inline long gj_net_kick(unsigned which)
{
    return gj_net(17u, (long)which, 0, 0);
}
static inline long gj_net_avail_push(unsigned which, const void *p, size_t cb)
{
    return gj_net(18u, (long)which, (long)(uintptr_t)p, (long)cb);
}
static inline long gj_net_used_reap(unsigned which, unsigned cMax)
{
    return gj_net(19u, (long)which, (long)cMax, 0);
}
static inline long gj_net_ring_state(void *pU32x4)
{
    return gj_net(20u, (long)(uintptr_t)pU32x4, 0, 0);
}
static inline long gj_net_map_dma(unsigned long uVa, void *pDmaExport)
{
    return gj_net(21u, (long)uVa, (long)(uintptr_t)pDmaExport, 0);
}
static inline long gj_net_desc_alloc(unsigned which)
{
    return gj_net(22u, (long)which, 0, 0);
}
static inline long gj_net_user_avail(unsigned which, long head, long flags)
{
    return gj_net(23u, (long)which, head, flags);
}
static inline long gj_net_bounce_fill(unsigned slot, const void *p, size_t cb)
{
    return gj_net(24u, (long)slot, (long)(uintptr_t)p, (long)cb);
}

/* Store door */
static inline long gj_store(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_STORE, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_store_claim(unsigned token)
{
    return gj_store(GJ_STORE_OP_CLAIM, (long)token, 0, 0);
}
static inline long gj_store_release(unsigned token)
{
    return gj_store(GJ_STORE_OP_RELEASE, (long)token, 0, 0);
}
static inline long gj_store_cap(unsigned long *pSectors)
{
    return gj_store(GJ_STORE_OP_CAP, (long)(uintptr_t)pSectors, 0, 0);
}
static inline long gj_store_read(unsigned long uLba, void *p, size_t cb)
{
    return gj_store(GJ_STORE_OP_READ, (long)uLba, (long)(uintptr_t)p, (long)cb);
}
static inline long gj_store_write(unsigned long uLba, const void *p, size_t cb)
{
    return gj_store(GJ_STORE_OP_WRITE, (long)uLba, (long)(uintptr_t)p,
                    (long)cb);
}
static inline long gj_store_flush(void)
{
    return gj_store(GJ_STORE_OP_FLUSH, 0, 0, 0);
}
static inline long gj_store_stats(void *pU32x3)
{
    return gj_store(GJ_STORE_OP_STATS, (long)(uintptr_t)pU32x3, 0, 0);
}
static inline long gj_store_export_ring(void *pExport)
{
    return gj_store(GJ_STORE_OP_EXPORT_RING, (long)(uintptr_t)pExport, 0, 0);
}
static inline long gj_store_kick(void)
{
    return gj_store(GJ_STORE_OP_KICK, 0, 0, 0);
}
static inline long gj_store_ring_state(void *pU32x2)
{
    return gj_store(GJ_STORE_OP_RING_STATE, (long)(uintptr_t)pU32x2, 0, 0);
}
static inline long gj_store_map_ring(unsigned long uVa, void *pExportOpt)
{
    return gj_store(GJ_STORE_OP_MAP_RING, (long)uVa, (long)(uintptr_t)pExportOpt,
                    0);
}

/* VFS door */
static inline long gj_vfs(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_VFS, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_vfs_claim(unsigned token)
{
    return gj_vfs(GJ_VFS_OP_CLAIM, (long)token, 0, 0);
}
static inline long gj_vfs_release(unsigned token)
{
    return gj_vfs(GJ_VFS_OP_RELEASE, (long)token, 0, 0);
}
static inline long gj_vfs_format(void)
{
    return gj_vfs(GJ_VFS_OP_FORMAT, 0, 0, 0);
}
static inline long gj_vfs_mount(void *pU32x4Opt)
{
    return gj_vfs(GJ_VFS_OP_MOUNT, (long)(uintptr_t)pU32x4Opt, 0, 0);
}
static inline long gj_vfs_open(const char *szName, unsigned uFlags)
{
    return gj_vfs(GJ_VFS_OP_OPEN, (long)(uintptr_t)szName, (long)uFlags, 0);
}
static inline long gj_vfs_close(long hFd)
{
    return gj_vfs(GJ_VFS_OP_CLOSE, hFd, 0, 0);
}
static inline long gj_vfs_readfd(long hFd, void *p, size_t cb)
{
    return gj_vfs(GJ_VFS_OP_READFD, hFd, (long)(uintptr_t)p, (long)cb);
}
static inline long gj_vfs_writefd(long hFd, const void *p, size_t cb)
{
    return gj_vfs(GJ_VFS_OP_WRITEFD, hFd, (long)(uintptr_t)p, (long)cb);
}
static inline long gj_vfs_seekfd(long hFd, long off, unsigned uWhence)
{
    return gj_vfs(GJ_VFS_OP_SEEKFD, hFd, off, (long)uWhence);
}
static inline long gj_vfs_unlink(const char *szName)
{
    return gj_vfs(GJ_VFS_OP_UNLINK, (long)(uintptr_t)szName, 0, 0);
}
static inline long gj_vfs_stat(const char *szName, void *pU32x2)
{
    return gj_vfs(GJ_VFS_OP_STAT, (long)(uintptr_t)szName,
                  (long)(uintptr_t)pU32x2, 0);
}
static inline long gj_vfs_list(void *pBuf, size_t cb)
{
    return gj_vfs(GJ_VFS_OP_LIST, (long)(uintptr_t)pBuf, 0, (long)cb);
}
static inline long gj_vfs_stats(void *pU32x4)
{
    return gj_vfs(GJ_VFS_OP_STATS, (long)(uintptr_t)pU32x4, 0, 0);
}

/* Console */
static inline long gj_console_poll(void)
{
    return gj_syscall1(GJ_SYS_CONSOLE, (long)GJ_CONSOLE_OP_POLL);
}
static inline long gj_console_getc(void)
{
    return gj_syscall1(GJ_SYS_CONSOLE, (long)GJ_CONSOLE_OP_GETC);
}
static inline long gj_console_read(void *pBuf, unsigned cb)
{
    return gj_syscall3(GJ_SYS_CONSOLE, (long)GJ_CONSOLE_OP_READ,
                       (long)(uintptr_t)pBuf, (long)cb);
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
static inline long gj_hda_open(unsigned ch, unsigned rate_hz, unsigned bits)
{
    return gj_hda_stream(GJ_HDA_OP_OPEN, (long)ch, (long)rate_hz, (long)bits);
}
static inline long gj_hda_write(const void *p, size_t cb)
{
    return gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)p, (long)cb, 0);
}
static inline long gj_hda_start(void)
{
    return gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
}
static inline long gj_hda_tick(unsigned frames)
{
    return gj_hda_stream(GJ_HDA_OP_TICK, (long)frames, 0, 0);
}
static inline long gj_hda_close(void)
{
    return gj_hda_stream(GJ_HDA_OP_CLOSE, 0, 0, 0);
}
static inline long gj_hda_stats(void *pU32x3)
{
    return gj_hda_stream(GJ_HDA_OP_STATS, (long)(uintptr_t)pU32x3, 0, 0);
}

/**
 * SCSI door: op = GJ_SCSI_OP_*; see GJ_SYS_SCSI.
 * Userspace scsi_mid product path over kernel virtio-scsi interim.
 */
static inline long gj_scsi(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_SCSI, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_scsi_inquiry(void *pBuf, size_t cb)
{
    return gj_scsi(GJ_SCSI_OP_INQUIRY, (long)(uintptr_t)pBuf, (long)cb, 0);
}
static inline long gj_scsi_read_cap(void *pU32x2)
{
    return gj_scsi(GJ_SCSI_OP_READ_CAP, (long)(uintptr_t)pU32x2, 0, 0);
}
static inline long gj_scsi_read10(unsigned long uLba, void *p, unsigned cBlocks)
{
    return gj_scsi(GJ_SCSI_OP_READ10, (long)uLba, (long)(uintptr_t)p,
                   (long)cBlocks);
}
static inline long gj_scsi_write10(unsigned long uLba, const void *p,
                                   unsigned cBlocks)
{
    return gj_scsi(GJ_SCSI_OP_WRITE10, (long)uLba, (long)(uintptr_t)p,
                   (long)cBlocks);
}
static inline long gj_scsi_ready(void)
{
    return gj_scsi(GJ_SCSI_OP_READY, 0, 0, 0);
}
static inline long gj_scsi_stats(void *pU32x2)
{
    return gj_scsi(GJ_SCSI_OP_STATS, (long)(uintptr_t)pU32x2, 0, 0);
}

/**
 * Wait/poll notification badges. which=GJ_NOTIFY_WHICH_MSIX_GLOBAL (0);
 * mask selects badges; block=0 poll / 1 sleep. Returns cleared pending.
 */
static inline long gj_notify_wait(unsigned which, unsigned long mask, int block)
{
    return gj_syscall3(GJ_SYS_NOTIFY_WAIT, (long)which, (long)mask, (long)block);
}
/** Soft poll: any MSI-X global badge, non-blocking. */
static inline long gj_notify_poll(void)
{
    return gj_notify_wait(GJ_NOTIFY_WHICH_MSIX_GLOBAL, (unsigned long)-1, 0);
}
/** Soft block: any MSI-X global badge. */
static inline long gj_notify_block(void)
{
    return gj_notify_wait(GJ_NOTIFY_WHICH_MSIX_GLOBAL, (unsigned long)-1, 1);
}

static inline long gj_platform_info(unsigned op, long a1, long a2, long a3)
{
    return gj_syscall6(GJ_SYS_PLATFORM_INFO, (long)op, a1, a2, a3, 0, 0);
}
static inline long gj_plat_iommu_info(void *pInfo)
{
    return gj_platform_info(GJ_PLAT_OP_IOMMU_INFO, (long)(uintptr_t)pInfo, 0,
                            0);
}
static inline long gj_plat_msix_inventory(void *pInfoOpt)
{
    return gj_platform_info(GJ_PLAT_OP_MSIX_INVENTORY,
                            (long)(uintptr_t)pInfoOpt, 0, 0);
}
static inline long gj_plat_msix_inject(unsigned long uBadge)
{
    return gj_platform_info(GJ_PLAT_OP_MSIX_INJECT, (long)uBadge, 0, 0);
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
static inline long linux_futex_wait(volatile void *pUaddr, unsigned uVal)
{
    return linux_futex(pUaddr, GJ_FUTEX_WAIT_PRIVATE, uVal, 0, 0, 0);
}
static inline long linux_futex_wake(volatile void *pUaddr, unsigned cWake)
{
    return linux_futex(pUaddr, GJ_FUTEX_WAKE_PRIVATE, cWake, 0, 0, 0);
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
static inline long linux_pread64(int fd, void *p, size_t cb, long off)
{
    return gj_syscall6(LINUX_NR_pread64, (long)fd, (long)(uintptr_t)p, (long)cb,
                       off, 0, 0);
}
static inline long linux_pwrite64(int fd, const void *p, size_t cb, long off)
{
    return gj_syscall6(LINUX_NR_pwrite64, (long)fd, (long)(uintptr_t)p,
                       (long)cb, off, 0, 0);
}
static inline long linux_getpid(void)
{
    return gj_syscall0(LINUX_NR_getpid);
}
static inline long linux_gettid(void)
{
    return gj_syscall0(LINUX_NR_gettid);
}
static inline void linux_exit(int code) __attribute__((noreturn));
static inline void linux_exit(int code)
{
    (void)gj_syscall1(LINUX_NR_exit, (long)code);
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
static inline void linux_exit_group(int code) __attribute__((noreturn));
static inline void linux_exit_group(int code)
{
    (void)gj_syscall1(LINUX_NR_exit_group, (long)code);
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
static inline long linux_clock_getres(int clockid, void *pTs)
{
    return gj_syscall2(LINUX_NR_clock_getres, (long)clockid,
                       (long)(uintptr_t)pTs);
}
static inline long linux_clock_nanosleep(int clockid, int flags,
                                         const void *pReq, void *pRem)
{
    return gj_syscall6(LINUX_NR_clock_nanosleep, (long)clockid, (long)flags,
                       (long)(uintptr_t)pReq, (long)(uintptr_t)pRem, 0, 0);
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
static inline long linux_madvise(void *addr, size_t len, int advice)
{
    return gj_syscall3(LINUX_NR_madvise, (long)(uintptr_t)addr, (long)len,
                       (long)advice);
}
static inline long linux_mremap(void *pOld, size_t cbOld, size_t cbNew,
                                int flags, void *pNew)
{
    return gj_syscall6(LINUX_NR_mremap, (long)(uintptr_t)pOld, (long)cbOld,
                       (long)cbNew, (long)flags, (long)(uintptr_t)pNew, 0);
}
static inline long linux_close(int fd)
{
    return gj_syscall1(LINUX_NR_close, (long)fd);
}
static inline long linux_close_range(unsigned uFirst, unsigned uLast,
                                     unsigned uFlags)
{
    return gj_syscall3(LINUX_NR_close_range, (long)uFirst, (long)uLast,
                       (long)uFlags);
}
static inline long linux_openat(int dfd, const char *path, int flags, int mode)
{
    return gj_syscall6(LINUX_NR_openat, (long)dfd, (long)(uintptr_t)path,
                       (long)flags, (long)mode, 0, 0);
}
/** Soft open(path) via openat(AT_FDCWD, …). */
static inline long linux_open(const char *path, int flags, int mode)
{
    return linux_openat(GJ_AT_FDCWD, path, flags, mode);
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
static inline long linux_getgid(void)
{
    return gj_syscall0(LINUX_NR_getgid);
}
static inline long linux_geteuid(void)
{
    return gj_syscall0(LINUX_NR_geteuid);
}
static inline long linux_getegid(void)
{
    return gj_syscall0(LINUX_NR_getegid);
}
static inline long linux_getcwd(char *buf, size_t cb)
{
    return gj_syscall2(LINUX_NR_getcwd, (long)(uintptr_t)buf, (long)cb);
}
static inline long linux_chdir(const char *path)
{
    return gj_syscall1(LINUX_NR_chdir, (long)(uintptr_t)path);
}
static inline long linux_fchdir(int fd)
{
    return gj_syscall1(LINUX_NR_fchdir, (long)fd);
}
static inline long linux_fsync(int fd)
{
    return gj_syscall1(LINUX_NR_fsync, (long)fd);
}
static inline long linux_fdatasync(int fd)
{
    return gj_syscall1(LINUX_NR_fdatasync, (long)fd);
}
static inline long linux_sync(void)
{
    return gj_syscall0(LINUX_NR_sync);
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
static inline long linux_pipe2(int *fds, int flags)
{
    return gj_syscall2(LINUX_NR_pipe2, (long)(uintptr_t)fds, (long)flags);
}
static inline long linux_pipe(int *fds)
{
    return linux_pipe2(fds, 0);
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
static inline long linux_unlink(const char *path)
{
    return gj_syscall1(LINUX_NR_unlink, (long)(uintptr_t)path);
}
static inline long linux_unlinkat(int dfd, const char *path, int flags)
{
    return gj_syscall3(LINUX_NR_unlinkat, (long)dfd, (long)(uintptr_t)path,
                       (long)flags);
}
static inline long linux_readlink(const char *path, char *buf, size_t cb)
{
    return gj_syscall3(LINUX_NR_readlink, (long)(uintptr_t)path,
                       (long)(uintptr_t)buf, (long)cb);
}
static inline long linux_readlinkat(int dfd, const char *path, char *buf,
                                    size_t cb)
{
    return gj_syscall6(LINUX_NR_readlinkat, (long)dfd, (long)(uintptr_t)path,
                       (long)(uintptr_t)buf, (long)cb, 0, 0);
}
static inline long linux_symlink(const char *target, const char *linkpath)
{
    return gj_syscall2(LINUX_NR_symlink, (long)(uintptr_t)target,
                       (long)(uintptr_t)linkpath);
}
static inline long linux_rename(const char *oldp, const char *newp)
{
    return gj_syscall2(LINUX_NR_rename, (long)(uintptr_t)oldp,
                       (long)(uintptr_t)newp);
}
static inline long linux_renameat(int olddfd, const char *oldp, int newdfd,
                                  const char *newp)
{
    return gj_syscall6(LINUX_NR_renameat, (long)olddfd, (long)(uintptr_t)oldp,
                       (long)newdfd, (long)(uintptr_t)newp, 0, 0);
}
static inline long linux_mkdir(const char *path, int mode)
{
    return gj_syscall2(LINUX_NR_mkdir, (long)(uintptr_t)path, (long)mode);
}
static inline long linux_mkdirat(int dfd, const char *path, int mode)
{
    return gj_syscall3(LINUX_NR_mkdirat, (long)dfd, (long)(uintptr_t)path,
                       (long)mode);
}
static inline long linux_rmdir(const char *path)
{
    return gj_syscall1(LINUX_NR_rmdir, (long)(uintptr_t)path);
}
static inline long linux_access(const char *path, int mode)
{
    return gj_syscall2(LINUX_NR_access, (long)(uintptr_t)path, (long)mode);
}
static inline long linux_faccessat(int dfd, const char *path, int mode,
                                   int flags)
{
    return gj_syscall6(LINUX_NR_faccessat, (long)dfd, (long)(uintptr_t)path,
                       (long)mode, (long)flags, 0, 0);
}
static inline long linux_chmod(const char *path, int mode)
{
    return gj_syscall2(LINUX_NR_chmod, (long)(uintptr_t)path, (long)mode);
}
static inline long linux_fchmod(int fd, int mode)
{
    return gj_syscall2(LINUX_NR_fchmod, (long)fd, (long)mode);
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
static inline long linux_vfork(void)
{
    return gj_syscall0(LINUX_NR_vfork);
}
static inline long linux_clone(unsigned long uFlags, void *pStack,
                              int *pParentTid, int *pChildTid,
                              unsigned long uTls)
{
    /* x86_64: clone(flags, stack, parent_tid, child_tid, tls) — arg order
     * differs from man page; matches kernel linux_abi / SYSCALL ABI. */
    return gj_syscall6(LINUX_NR_clone, (long)uFlags, (long)(uintptr_t)pStack,
                       (long)(uintptr_t)pParentTid, (long)(uintptr_t)pChildTid,
                       (long)uTls, 0);
}
static inline long linux_wait4(int pid, int *status, int options, void *rusage)
{
    return gj_syscall6(LINUX_NR_wait4, (long)pid, (long)(uintptr_t)status,
                       (long)options, (long)(uintptr_t)rusage, 0, 0);
}
static inline long linux_waitid(int idtype, unsigned id, void *pInfo,
                                int options, void *pRusage)
{
    return gj_syscall6(LINUX_NR_waitid, (long)idtype, (long)id,
                       (long)(uintptr_t)pInfo, (long)options,
                       (long)(uintptr_t)pRusage, 0);
}
static inline long linux_kill(int pid, int sig)
{
    return gj_syscall2(LINUX_NR_kill, (long)pid, (long)sig);
}
static inline long linux_tgkill(int tgid, int tid, int sig)
{
    return gj_syscall3(LINUX_NR_tgkill, (long)tgid, (long)tid, (long)sig);
}
static inline long linux_execve(const char *path, char *const *argv,
                                char *const *envp)
{
    return gj_syscall3(LINUX_NR_execve, (long)(uintptr_t)path,
                       (long)(uintptr_t)argv, (long)(uintptr_t)envp);
}
static inline long linux_execveat(int dfd, const char *path, char *const *argv,
                                  char *const *envp, int flags)
{
    return gj_syscall6(LINUX_NR_execveat, (long)dfd, (long)(uintptr_t)path,
                       (long)(uintptr_t)argv, (long)(uintptr_t)envp,
                       (long)flags, 0);
}
static inline long linux_lseek(int fd, long off, int whence)
{
    return gj_syscall3(LINUX_NR_lseek, (long)fd, off, (long)whence);
}
static inline long linux_fstat(int fd, void *pStat)
{
    return gj_syscall2(LINUX_NR_fstat, (long)fd, (long)(uintptr_t)pStat);
}
static inline long linux_newfstatat(int dfd, const char *path, void *pStat,
                                    int flags)
{
    return gj_syscall6(LINUX_NR_newfstatat, (long)dfd, (long)(uintptr_t)path,
                       (long)(uintptr_t)pStat, (long)flags, 0, 0);
}
static inline long linux_statx(int dfd, const char *path, int flags,
                               unsigned uMask, void *pStatx)
{
    return gj_syscall6(LINUX_NR_statx, (long)dfd, (long)(uintptr_t)path,
                       (long)flags, (long)uMask, (long)(uintptr_t)pStatx, 0);
}
static inline long linux_ftruncate(int fd, long cb)
{
    return gj_syscall2(LINUX_NR_ftruncate, (long)fd, cb);
}
static inline long linux_dup(int fd)
{
    return gj_syscall1(LINUX_NR_dup, (long)fd);
}
static inline long linux_dup2(int oldfd, int newfd)
{
    return gj_syscall2(LINUX_NR_dup2, (long)oldfd, (long)newfd);
}
static inline long linux_dup3(int oldfd, int newfd, int flags)
{
    return gj_syscall3(LINUX_NR_dup3, (long)oldfd, (long)newfd, (long)flags);
}
static inline long linux_fcntl(int fd, int cmd, long arg)
{
    return gj_syscall3(LINUX_NR_fcntl, (long)fd, (long)cmd, arg);
}
static inline long linux_ioctl(int fd, unsigned long uReq, long arg)
{
    return gj_syscall3(LINUX_NR_ioctl, (long)fd, (long)uReq, arg);
}
static inline long linux_flock(int fd, int op)
{
    return gj_syscall2(LINUX_NR_flock, (long)fd, (long)op);
}
static inline long linux_poll(void *pFds, unsigned long cFds, int timeout)
{
    return gj_syscall3(LINUX_NR_poll, (long)(uintptr_t)pFds, (long)cFds,
                       (long)timeout);
}
static inline long linux_ppoll(void *pFds, unsigned long cFds,
                               const void *pTs, const void *pSigmask)
{
    return gj_syscall6(LINUX_NR_ppoll, (long)(uintptr_t)pFds, (long)cFds,
                       (long)(uintptr_t)pTs, (long)(uintptr_t)pSigmask, 0, 0);
}
static inline long linux_epoll_create1(int flags)
{
    return gj_syscall1(LINUX_NR_epoll_create1, (long)flags);
}
static inline long linux_epoll_ctl(int epfd, int op, int fd, void *pEvent)
{
    return gj_syscall6(LINUX_NR_epoll_ctl, (long)epfd, (long)op, (long)fd,
                       (long)(uintptr_t)pEvent, 0, 0);
}
static inline long linux_epoll_wait(int epfd, void *pEvents, int cMax,
                                    int timeout)
{
    return gj_syscall6(LINUX_NR_epoll_wait, (long)epfd, (long)(uintptr_t)pEvents,
                       (long)cMax, (long)timeout, 0, 0);
}
static inline long linux_socket(int domain, int type, int protocol)
{
    return gj_syscall3(LINUX_NR_socket, (long)domain, (long)type,
                       (long)protocol);
}
static inline long linux_bind(int fd, const void *pAddr, unsigned cbAddr)
{
    return gj_syscall3(LINUX_NR_bind, (long)fd, (long)(uintptr_t)pAddr,
                       (long)cbAddr);
}
static inline long linux_listen(int fd, int backlog)
{
    return gj_syscall2(LINUX_NR_listen, (long)fd, (long)backlog);
}
static inline long linux_accept(int fd, void *pAddr, void *pAddrLen)
{
    return gj_syscall3(LINUX_NR_accept, (long)fd, (long)(uintptr_t)pAddr,
                       (long)(uintptr_t)pAddrLen);
}
static inline long linux_accept4(int fd, void *pAddr, void *pAddrLen, int flags)
{
    return gj_syscall6(LINUX_NR_accept4, (long)fd, (long)(uintptr_t)pAddr,
                       (long)(uintptr_t)pAddrLen, (long)flags, 0, 0);
}
static inline long linux_connect(int fd, const void *pAddr, unsigned cbAddr)
{
    return gj_syscall3(LINUX_NR_connect, (long)fd, (long)(uintptr_t)pAddr,
                       (long)cbAddr);
}
static inline long linux_shutdown(int fd, int how)
{
    return gj_syscall2(LINUX_NR_shutdown, (long)fd, (long)how);
}
static inline long linux_setsockopt(int fd, int level, int optname,
                                    const void *pOpt, unsigned cbOpt)
{
    return gj_syscall6(LINUX_NR_setsockopt, (long)fd, (long)level, (long)optname,
                       (long)(uintptr_t)pOpt, (long)cbOpt, 0);
}
static inline long linux_getsockopt(int fd, int level, int optname, void *pOpt,
                                    void *pCbOpt)
{
    return gj_syscall6(LINUX_NR_getsockopt, (long)fd, (long)level, (long)optname,
                       (long)(uintptr_t)pOpt, (long)(uintptr_t)pCbOpt, 0);
}
static inline long linux_sendto(int fd, const void *p, size_t cb, int flags,
                                const void *pAddr, unsigned cbAddr)
{
    return gj_syscall6(LINUX_NR_sendto, (long)fd, (long)(uintptr_t)p, (long)cb,
                       (long)flags, (long)(uintptr_t)pAddr, (long)cbAddr);
}
static inline long linux_recvfrom(int fd, void *p, size_t cb, int flags,
                                  void *pAddr, void *pAddrLen)
{
    return gj_syscall6(LINUX_NR_recvfrom, (long)fd, (long)(uintptr_t)p, (long)cb,
                       (long)flags, (long)(uintptr_t)pAddr,
                       (long)(uintptr_t)pAddrLen);
}
static inline long linux_sendmsg(int fd, const void *pMsg, int flags)
{
    return gj_syscall3(LINUX_NR_sendmsg, (long)fd, (long)(uintptr_t)pMsg,
                       (long)flags);
}
static inline long linux_recvmsg(int fd, void *pMsg, int flags)
{
    return gj_syscall3(LINUX_NR_recvmsg, (long)fd, (long)(uintptr_t)pMsg,
                       (long)flags);
}
static inline long linux_arch_prctl(int code, unsigned long addr)
{
    return gj_syscall2(LINUX_NR_arch_prctl, (long)code, (long)addr);
}
static inline long linux_rt_sigprocmask(int how, const void *pSet, void *pOld,
                                        size_t cbSigset)
{
    return gj_syscall6(LINUX_NR_rt_sigprocmask, (long)how,
                       (long)(uintptr_t)pSet, (long)(uintptr_t)pOld,
                       (long)cbSigset, 0, 0);
}
static inline long linux_rt_sigaction(int sig, const void *pAct, void *pOld,
                                      size_t cbSigset)
{
    return gj_syscall6(LINUX_NR_rt_sigaction, (long)sig, (long)(uintptr_t)pAct,
                       (long)(uintptr_t)pOld, (long)cbSigset, 0, 0);
}
static inline long linux_getrlimit(int resource, void *pRlim)
{
    return gj_syscall2(LINUX_NR_getrlimit, (long)resource,
                       (long)(uintptr_t)pRlim);
}
static inline long linux_prlimit64(int pid, int resource, const void *pNew,
                                   void *pOld)
{
    return gj_syscall6(LINUX_NR_prlimit64, (long)pid, (long)resource,
                       (long)(uintptr_t)pNew, (long)(uintptr_t)pOld, 0, 0);
}
static inline long linux_getrusage(int who, void *pUsage)
{
    return gj_syscall2(LINUX_NR_getrusage, (long)who, (long)(uintptr_t)pUsage);
}
static inline long linux_sysinfo(void *pInfo)
{
    return gj_syscall1(LINUX_NR_sysinfo, (long)(uintptr_t)pInfo);
}
static inline long linux_memfd_create(const char *szName, unsigned uFlags)
{
    return gj_syscall2(LINUX_NR_memfd_create, (long)(uintptr_t)szName,
                       (long)uFlags);
}
static inline long linux_set_tid_address(int *pTid)
{
    return gj_syscall1(LINUX_NR_set_tid_address, (long)(uintptr_t)pTid);
}
static inline long linux_munmap_alias(void *addr, size_t len)
{
    return linux_munmap(addr, len);
}

#ifdef __cplusplus
}
#endif
