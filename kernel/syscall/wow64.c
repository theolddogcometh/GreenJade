/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * WoW64 scaffold: i386 Linux syscall NR map + personality flag.
 * Pure C dual-license; clean-room public man-page mapping — not Linux/Wine.
 * Product PE32 loading is userspace; CS32 int 0x80 is trap_dispatch.
 * Hybrid SYSCALL uses this table; args are zero-extended at the entry edge.
 */
#include <gj/klog.h>
#include <gj/wow64.h>

static int g_fWow64;
static u32 g_u32Calls;

int
wow64_enabled(void)
{
    return g_fWow64;
}

void
wow64_set(int fOn)
{
    g_fWow64 = fOn ? 1 : 0;
    kprintf("wow64: personality %s\n", g_fWow64 ? "on" : "off");
}

/*
 * i386 Linux NR subset → x86_64 NR (Deck Top-50 / glibc / PE32 int80 needs).
 * Groups: process, path/vfs, mmap2/mprotect, IPC, timers, *at family.
 * mmap2 (192): NR only → mmap (9). i386 pgoff is pages; byte convert is
 * call-site / trap_dispatch responsibility, not this table.
 */
int
wow64_translate_nr(u32 u32Nr32, u32 *pOutNr)
{
    u32 nr64;

    /* Defensive: refuse NULL out; still count the attempt for smoke visibility */
    g_u32Calls++;
    if (pOutNr == NULL) {
        return -1;
    }
    switch (u32Nr32) {
    /* ---- process / exit / ids ----------------------------------------- */
    case 1:  nr64 = 60; break;  /* exit → exit */
    case 2:   nr64 = 57; break;  /* fork */
    case 11:  nr64 = 59; break;  /* execve */
    case 13:  nr64 = 201; break; /* time */
    case 20: nr64 = 39; break;  /* getpid */
    case 24:  nr64 = 102; break; /* getuid */
    case 27:  nr64 = 37; break;  /* alarm */
    case 37: nr64 = 62; break;  /* kill */
    case 47:  nr64 = 104; break; /* getgid */
    case 49:  nr64 = 107; break; /* geteuid */
    case 50:  nr64 = 108; break; /* getegid */
    case 64:  nr64 = 110; break; /* getppid */
    case 65:  nr64 = 111; break; /* getpgrp */
    case 66:  nr64 = 112; break; /* setsid */
    case 114: nr64 = 61; break; /* wait4 */
    case 190: nr64 = 58; break;  /* vfork */
    case 199: nr64 = 102; break; /* getuid32 → getuid */
    case 200: nr64 = 104; break; /* getgid32 → getgid */
    case 201: nr64 = 107; break; /* geteuid32 */
    case 202: nr64 = 108; break; /* getegid32 */
    case 224: nr64 = 186; break; /* gettid */
    case 252: nr64 = 231; break; /* exit_group */
    case 258: nr64 = 218; break; /* set_tid_address */
    case 284: nr64 = 247; break; /* waitid */
    /* ---- path / vfs (open, stat, rename, *at) -------------------------- */
    case 5:  nr64 = 2; break;   /* open (legacy path; openat preferred) */
    case 6:  nr64 = 3; break;   /* close */
    case 9:   nr64 = 86; break;  /* link */
    case 10:  nr64 = 87; break;  /* unlink */
    case 12:  nr64 = 80; break;  /* chdir */
    case 15:  nr64 = 90; break;  /* chmod */
    case 30:  nr64 = 132; break; /* utime → utime-shaped */
    case 33: nr64 = 21; break;  /* access(path, mode) */
    case 38: nr64 = 82; break;  /* rename(oldpath, newpath) */
    case 39: nr64 = 83; break;  /* mkdir(path) */
    case 40: nr64 = 84; break;  /* rmdir(path) */
    case 83: nr64 = 88; break;  /* symlink */
    case 85: nr64 = 89; break;  /* readlink(path) */
    case 93:  nr64 = 77; break;  /* ftruncate */
    case 94:  nr64 = 91; break;  /* fchmod */
    case 106: nr64 = 4; break;  /* stat(path) */
    case 107: nr64 = 6; break;  /* lstat(path) */
    case 108: nr64 = 5; break;  /* fstat */
    case 118: nr64 = 74; break;  /* fsync */
    case 133: nr64 = 81; break;  /* fchdir */
    case 148: nr64 = 75; break;  /* fdatasync */
    case 183: nr64 = 79; break;  /* getcwd */
    case 195: nr64 = 4; break;  /* stat64(path) → stat */
    case 197: nr64 = 5; break;  /* fstat64 → fstat */
    case 271: nr64 = 280; break; /* utimensat */
    case 295: nr64 = 257; break; /* openat(path) */
    case 296: nr64 = 258; break; /* mkdirat */
    case 300: nr64 = 262; break; /* fstatat64 → newfstatat */
    case 301: nr64 = 263; break; /* unlinkat */
    case 302: nr64 = 264; break; /* renameat */
    case 303: nr64 = 265; break; /* linkat */
    case 304: nr64 = 266; break; /* symlinkat */
    case 305: nr64 = 267; break; /* readlinkat */
    case 307: nr64 = 269; break; /* faccessat */
    /* ---- mmap2 / memory ----------------------------------------------- */
    case 45: nr64 = 12; break;  /* brk */
    case 90: nr64 = 9; break;   /* old mmap → mmap */
    case 91: nr64 = 11; break;  /* munmap */
    case 125: nr64 = 10; break; /* mprotect */
    case 192: nr64 = 9; break;  /* mmap2 → mmap (pgoff pages; convert later) */
    case 219: nr64 = 28; break;  /* madvise */
    case 356: nr64 = 319; break; /* memfd_create */
    /* ---- fd I/O / pipe / socket --------------------------------------- */
    case 3:  nr64 = 0; break;   /* read */
    case 4:  nr64 = 1; break;   /* write */
    case 19: nr64 = 8; break;   /* lseek */
    case 41: nr64 = 32; break;  /* dup */
    case 42: nr64 = 22; break;  /* pipe (i386) → pipe */
    case 54: nr64 = 16; break;  /* ioctl */
    case 63: nr64 = 33; break;  /* dup2 (i386) */
    case 102: nr64 = 41; break;  /* socketcall → socket (coarse) */
    case 140: nr64 = 17; break;  /* _llseek → pread64-shaped */
    case 141: nr64 = 78; break;  /* getdents → getdents */
    case 145: nr64 = 19; break;  /* readv */
    case 146: nr64 = 20; break;  /* writev */
    case 180: nr64 = 17; break;  /* pread64 */
    case 181: nr64 = 18; break;  /* pwrite64 */
    case 220: nr64 = 217; break; /* getdents64 (i386) */
    case 221: nr64 = 72; break;  /* fcntl64 → fcntl */
    case 239: nr64 = 40; break;  /* sendfile64 → sendfile */
    case 324: nr64 = 285; break; /* fallocate */
    case 330: nr64 = 292; break; /* dup3 */
    case 331: nr64 = 293; break; /* pipe2 */
    case 377: nr64 = 326; break; /* copy_file_range */
    case 436: nr64 = 436; break; /* close_range identity */
    /* ---- poll / epoll / inotify / eventfd ----------------------------- */
    case 142: nr64 = 23; break;  /* select */
    case 168: nr64 = 7; break;   /* poll */
    case 254: nr64 = 213; break; /* epoll_create */
    case 255: nr64 = 233; break; /* epoll_ctl */
    case 256: nr64 = 232; break; /* epoll_wait */
    case 291: nr64 = 253; break; /* inotify_init */
    case 292: nr64 = 254; break; /* inotify_add_watch */
    case 293: nr64 = 255; break; /* inotify_rm_watch */
    case 308: nr64 = 270; break; /* pselect6 */
    case 309: nr64 = 271; break; /* ppoll */
    case 323: nr64 = 284; break; /* eventfd */
    case 328: nr64 = 290; break; /* eventfd2 */
    case 329: nr64 = 291; break; /* epoll_create1 */
    case 332: nr64 = 294; break; /* inotify_init1 */
    /* ---- time / futex / tls / misc ------------------------------------ */
    case 36:  nr64 = 162; break; /* sync */
    case 75:  nr64 = 160; break; /* setrlimit */
    case 76:  nr64 = 97; break;  /* getrlimit */
    case 78:  nr64 = 96; break;  /* gettimeofday */
    case 96:  nr64 = 140; break; /* getpriority */
    case 97:  nr64 = 141; break; /* setpriority */
    case 104: nr64 = 38; break;  /* setitimer */
    case 105: nr64 = 36; break;  /* getitimer */
    case 116: nr64 = 99; break;  /* sysinfo */
    case 122: nr64 = 63; break; /* uname */
    case 143: nr64 = 73; break; /* flock */
    case 158: nr64 = 24; break;  /* sched_yield */
    case 162: nr64 = 35; break;  /* nanosleep */
    case 172: nr64 = 158; break; /* prctl (i386) → arch_prctl-shaped later */
    case 174: nr64 = 13; break;  /* rt_sigaction */
    case 175: nr64 = 14; break;  /* rt_sigprocmask */
    case 191: nr64 = 97; break;  /* ugetrlimit → getrlimit */
    case 222: nr64 = 222; break; /* timer_create identity */
    case 240: nr64 = 202; break; /* futex */
    case 243: nr64 = 158; break; /* set_thread_area → arch_prctl-shaped */
    case 265: nr64 = 228; break; /* clock_gettime */
    case 266: nr64 = 229; break; /* clock_getres */
    case 267: nr64 = 230; break; /* clock_nanosleep */
    case 311: nr64 = 273; break; /* set_robust_list */
    case 321: nr64 = 282; break; /* signalfd */
    case 322: nr64 = 283; break; /* timerfd_create */
    case 325: nr64 = 286; break; /* timerfd_settime */
    case 326: nr64 = 287; break; /* timerfd_gettime */
    case 327: nr64 = 289; break; /* signalfd4 */
    case 355: nr64 = 318; break; /* getrandom */
    default:
        /* Unmapped: identity; hybrid cold path may ENOSYS */
        nr64 = u32Nr32;
        break;
    }
    *pOutNr = nr64;
    return 0;
}

u32
wow64_calls(void)
{
    return g_u32Calls;
}
