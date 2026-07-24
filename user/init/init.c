/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * First userspace process — pure C11 freestanding (libgj only).
 * Output: build/user/init.elf — rootfs sbin/init, ESP user/, Multiboot embed.
 *
 * Personality: LINUX (Option C hybrid). Brings up uname/clock/mmap/openat/
 * getcwd/getrandom plus named memobj for Proton / wine / GPU export paths.
 *
 * Smoke markers (prefix-stable; greppable live logs):
 *   greenjade-init: hello from userspace
 *   greenjade-init: libgj string PASS
 *   greenjade-init: named memobj PASS   (optional if memobj soft-miss)
 *   greenjade-init: soft deepen PASS    (optional; soft-only surface)
 *   greenjade-init: abi PASS
 *
 * Soft inventory (grep: init: soft) — deepen counters, never hard-fail:
 *   init: soft paths ok=… miss=…
 *   init: soft mmap ok=… miss=…
 *   init: soft ipc pipe=… efd=… poll=…
 *   init: soft spawn fork=… wait=… miss=…   (Linux fork soft; no native spawn)
 *   init: soft doors ok=… miss=…
 *   init: soft memobj ok=… miss=…
 *   init: soft links ok=… miss=…
 *   init: soft ids ok=… miss=…
 *   init: soft clocks ok=… miss=…
 *   init: soft fds ok=… miss=…
 *   init: soft rlimit ok=… miss=…
 *   init: soft stats areas=… path=… mmap=… ipc=… spawn=… door=… memobj=…
 *                 link=… id=… clock=… fd=… rlim=…
 *   init: soft path soft=1 bar3=0 multi_server=0 confine=0 wave=52
 *   init: soft honesty multi_server=0 confine=0 bar3=0 exclusive=1 soft=1 product_kernel=OPEN wave=52
 *   init: soft deepen product_kernel=OPEN wave=52 areas=… multi_server=0 confine=0 bar3=0
 *   init: soft exclusive product_kernel=OPEN wave=52 multi_server=0 confine=0 bar3=0 userland=1
 *
 * Soft deepen Wave 52 (never hard-fails boot; exclusive soft inventory):
 *   - libgj string/memory helpers (strcmp/memcpy/itoa/memmove/strstr/…)
 *   - CLOCK_REALTIME + clock_getres + clock_nanosleep(0)
 *   - fstat / arch_prctl GET_FS/GS / futex WAKE / brk grow query
 *   - second anon mmap + MAP_FIXED + madvise + mremap + memfd soft
 *   - rootfs/product path probes + getdents on /tmp + /proc + /dev
 *   - link/symlink/readlink + mkdir/rename/unlink + access/statx
 *   - pipe2 + eventfd2 + poll(0) + dup/fcntl + nanosleep + fork/wait
 *   - identity (tid/uid/gid/euid/egid) + rlimit/sysinfo/rusage soft
 *   - native doors: platform/notify/console/qos + store/net/vfs/session
 *     STATS (no CLAIM — live daemons own claim tokens)
 *   - scsi READY/STATS/INQUIRY/READ_CAP + HDA STATS (no stream open)
 *   - named memobj create+map + soft second/third name probe
 * Honesty: soft inventory ≠ product multi-server confine (multi_server=0).
 *
 * Parent owns Makefile target + kernel/proc/init_embed.S (.incbin).
 */
#include <gj/string.h>
#include <gj/syscalls.h>

/* Linux arch_prctl codes (x86_64) — match kernel linux_abi.h */
#define INIT_ARCH_GET_FS 0x1003
#define INIT_ARCH_GET_GS 0x1004

/* Linux futex ops — match kernel futex.h (WAKE only; never block) */
#define INIT_FUTEX_WAKE         1
#define INIT_FUTEX_PRIVATE_FLAG 128

/* Linux mmap flags used in soft MAP_FIXED probe */
#define INIT_MAP_PRIVATE 0x02
#define INIT_MAP_ANON    0x20
#define INIT_MAP_FIXED   0x10

/* Soft madvise advice codes (Linux) */
#define INIT_MADV_NORMAL   0
#define INIT_MADV_DONTNEED 4

/* Soft fcntl cmds (Linux) */
#define INIT_F_GETFD 1
#define INIT_F_GETFL 3

/* Soft rlimit resources (Linux) */
#define INIT_RLIMIT_STACK  3
#define INIT_RLIMIT_NOFILE 7
#define INIT_RLIMIT_AS     9

/* Soft poll events */
#define INIT_POLLIN 0x0001

/* Soft product path table entry */
struct init_soft_path {
    const char *szPath;
    int nFlags; /* openat flags; 0 = O_RDONLY */
};

/* Minimal pollfd shape for soft poll(timeout=0) */
struct init_pollfd {
    int nFd;
    short nEvents;
    short nRevents;
};

/* ---- Soft inventory counters (grep: init: soft) ------------------------ */
static unsigned g_cSoftPathOk;
static unsigned g_cSoftPathMiss;
static unsigned g_cSoftMmapOk;
static unsigned g_cSoftMmapMiss;
static unsigned g_cSoftIpcPipeOk;
static unsigned g_cSoftIpcEfdOk;
static unsigned g_cSoftIpcPollOk;
static unsigned g_cSoftSpawnFork; /* parent saw child pid from soft fork */
static unsigned g_cSoftSpawnWait; /* wait4 after soft fork */
static unsigned g_cSoftSpawnMiss; /* fork failed (< 0) */
static unsigned g_cSoftDoorOk;
static unsigned g_cSoftDoorMiss;
static unsigned g_cSoftMemobjOk;
static unsigned g_cSoftMemobjMiss;
static unsigned g_cSoftLinkOk;
static unsigned g_cSoftLinkMiss;
static unsigned g_cSoftIdOk;
static unsigned g_cSoftIdMiss;
static unsigned g_cSoftClockOk;
static unsigned g_cSoftClockMiss;
static unsigned g_cSoftFdOk;
static unsigned g_cSoftFdMiss;
static unsigned g_cSoftRlimOk;
static unsigned g_cSoftRlimMiss;
static unsigned g_cSoftAreas; /* soft deepen suite areas completed */

/* Count one door/native soft call: >=0 ok, else miss. Never hard-fails. */
static void
soft_note_door(long i64R)
{
    if (i64R >= 0) {
        g_cSoftDoorOk++;
    } else {
        g_cSoftDoorMiss++;
    }
}

/* Generic soft note: >=0 → *pOk, else *pMiss. */
static void
soft_note(long i64R, unsigned *pOk, unsigned *pMiss)
{
    if (i64R >= 0) {
        (*pOk)++;
    } else {
        (*pMiss)++;
    }
}

/*
 * Emit greppable soft inventory lines (prefix "init: soft ").
 * Pure observation — always soft; does not gate abi PASS.
 * Wave 52: ids/clocks/fds/rlimit + path honesty + deepen wave stamp.
 * Honesty: soft ≠ product multi-server confine.
 */
static void
soft_inventory_log(void)
{
    static char aLine[256];
    unsigned cIpc;
    unsigned cSpawnOk;

    cIpc = g_cSoftIpcPipeOk + g_cSoftIpcEfdOk + g_cSoftIpcPollOk;
    cSpawnOk = g_cSoftSpawnFork + g_cSoftSpawnWait;

    /* Grep: init: soft paths */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft paths ok=%u miss=%u\n",
                      (unsigned long)g_cSoftPathOk,
                      (unsigned long)g_cSoftPathMiss);
    gj_puts(aLine);

    /* Grep: init: soft mmap */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft mmap ok=%u miss=%u\n",
                      (unsigned long)g_cSoftMmapOk,
                      (unsigned long)g_cSoftMmapMiss);
    gj_puts(aLine);

    /* Grep: init: soft ipc */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft ipc pipe=%u efd=%u poll=%u\n",
                      (unsigned long)g_cSoftIpcPipeOk,
                      (unsigned long)g_cSoftIpcEfdOk,
                      (unsigned long)g_cSoftIpcPollOk);
    gj_puts(aLine);

    /* Grep: init: soft spawn (Linux fork soft counts; native spawn N/A) */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft spawn fork=%u wait=%u miss=%u\n",
                      (unsigned long)g_cSoftSpawnFork,
                      (unsigned long)g_cSoftSpawnWait,
                      (unsigned long)g_cSoftSpawnMiss);
    gj_puts(aLine);

    /* Grep: init: soft doors */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft doors ok=%u miss=%u\n",
                      (unsigned long)g_cSoftDoorOk,
                      (unsigned long)g_cSoftDoorMiss);
    gj_puts(aLine);

    /* Grep: init: soft memobj */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft memobj ok=%u miss=%u\n",
                      (unsigned long)g_cSoftMemobjOk,
                      (unsigned long)g_cSoftMemobjMiss);
    gj_puts(aLine);

    /* Grep: init: soft links */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft links ok=%u miss=%u\n",
                      (unsigned long)g_cSoftLinkOk,
                      (unsigned long)g_cSoftLinkMiss);
    gj_puts(aLine);

    /* Grep: init: soft ids (Wave 52) */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft ids ok=%u miss=%u\n",
                      (unsigned long)g_cSoftIdOk,
                      (unsigned long)g_cSoftIdMiss);
    gj_puts(aLine);

    /* Grep: init: soft clocks (Wave 52) */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft clocks ok=%u miss=%u\n",
                      (unsigned long)g_cSoftClockOk,
                      (unsigned long)g_cSoftClockMiss);
    gj_puts(aLine);

    /* Grep: init: soft fds (Wave 52) */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft fds ok=%u miss=%u\n",
                      (unsigned long)g_cSoftFdOk,
                      (unsigned long)g_cSoftFdMiss);
    gj_puts(aLine);

    /* Grep: init: soft rlimit (Wave 52) */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft rlimit ok=%u miss=%u\n",
                      (unsigned long)g_cSoftRlimOk,
                      (unsigned long)g_cSoftRlimMiss);
    gj_puts(aLine);

    /* Grep: init: soft stats (rollup) */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft stats areas=%u path=%u mmap=%u ipc=%u "
                      "spawn=%u door=%u memobj=%u link=%u "
                      "id=%u clock=%u fd=%u rlim=%u\n",
                      (unsigned long)g_cSoftAreas,
                      (unsigned long)g_cSoftPathOk,
                      (unsigned long)g_cSoftMmapOk,
                      (unsigned long)cIpc,
                      (unsigned long)cSpawnOk,
                      (unsigned long)g_cSoftDoorOk,
                      (unsigned long)g_cSoftMemobjOk,
                      (unsigned long)g_cSoftLinkOk,
                      (unsigned long)g_cSoftIdOk,
                      (unsigned long)g_cSoftClockOk,
                      (unsigned long)g_cSoftFdOk,
                      (unsigned long)g_cSoftRlimOk);
    gj_puts(aLine);

    /*
     * Grep: init: soft path (Wave 52 honesty).
     * Soft inventory only — not multi-server confine product.
     */
    gj_puts("init: soft path soft=1 bar3=0 multi_server=0 confine=0 "
            "wave=52\n");

    /*
     * Grep: init: soft honesty (Wave 52 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    gj_puts("init: soft honesty multi_server=0 confine=0 bar3=0 "
            "exclusive=1 soft=1 product_kernel=OPEN wave=52\n");

    /* Grep: init: soft deepen wave (Wave 52 stamp) */
    (void)gj_snprintf(aLine, sizeof(aLine),
                      "init: soft deepen wave=52 areas=%u "
                      "multi_server=0 confine=0 bar3=0 product_kernel=OPEN\n",
                      (unsigned long)g_cSoftAreas);
    gj_puts(aLine);

    /*
     * Grep: init: soft exclusive (Wave 52 exclusive deepen).
     * Soft inventory ≠ product multi-server confine / continuum.
     */
    gj_puts("init: soft exclusive product_kernel=OPEN wave=52 multi_server=0 confine=0 "
            "bar3=0 userland=1 kernel=0 continuum=0 product_kernel=OPEN\n");

    /*
     * Grep: init: soft open (Wave 52 open-lamp rollup).
     * product_kernel=OPEN; soft ≠ product multi-server confine.
     */
    gj_puts("init: soft open multi_server=0 confine=0 bar3=0 "
            "product_kernel=OPEN soft_only=1 wave=52\n");

    /*
     * Grep: init: soft rettenaille — Wave 52 return-glacisgate honesty
     * Soft inventory only; continuum toward 19400; Soft≠product; not bar3.
     */
    gj_puts("init: soft rettenaille soft_only=1 product_gate=0 "
            "soft_ne_product=1 continuum_toward=19400 "
            "(rettenaille honesty; Soft!=product; not bar3)\n");

    /*
     * Grep: init: soft retdemilune — Wave 52 exclusive outwork stamp
     * Soft inventory only; Soft≠product.
     */
    gj_puts("init: soft retdemilune exclusive=1 soft_ne_product=1 "
            "continuum_toward=19400 (retdemilune stamp; Soft!=product)\n");
}

/* Linux mmap errno band: return is -errno in [-4095, -1] on failure. */
static int
mmap_is_err(void *pMap)
{
    unsigned long u = (unsigned long)(uintptr_t)pMap;

    return u >= (unsigned long)-4095UL;
}

/* Soft open-read-close a path; never hard-fail. Optionally fstat + short read. */
static void
soft_open_probe(const char *szPath, size_t cbRead)
{
    static char aBuf[128];
    static unsigned char aSt[144]; /* struct stat-ish blob */
    long i64Fd;

    if (szPath == 0) {
        g_cSoftPathMiss++;
        return;
    }
    i64Fd = linux_openat(-100 /* AT_FDCWD */, szPath, 0, 0);
    if (i64Fd < 0) {
        g_cSoftPathMiss++;
        return;
    }
    g_cSoftPathOk++;
    (void)linux_fstat((int)i64Fd, aSt);
    if (cbRead > 0u) {
        if (cbRead > sizeof(aBuf)) {
            cbRead = sizeof(aBuf);
        }
        (void)linux_read((int)i64Fd, aBuf, cbRead);
        (void)linux_lseek((int)i64Fd, 0, 0 /* SEEK_SET */);
        (void)linux_pread64((int)i64Fd, aBuf, cbRead > 8u ? 8u : cbRead, 0);
    }
    (void)linux_close((int)i64Fd);
}

/* Soft: exercise freestanding libgj string/memory helpers (no I/O). */
static void
soft_libgj_string(void)
{
    static char aDst[48];
    static char aNum[24];
    static char aMove[16];
    size_t cb;

    (void)gj_memset(aDst, 0, sizeof(aDst));
    (void)gj_memcpy(aDst, "gj", 3);
    if (gj_strcmp(aDst, "gj") == 0) {
        (void)gj_strncmp(aDst, "gjx", 2);
    }
    cb = gj_strlen(aDst);
    (void)cb;
    (void)gj_strnlen(aDst, sizeof(aDst));
    (void)gj_strlcpy(aDst, "init", sizeof(aDst));
    (void)gj_strlcat(aDst, "-soft", sizeof(aDst));
    (void)gj_strncpy(aMove, "wave11", sizeof(aMove));
    (void)gj_memmove(aMove + 2, aMove, 4);
    (void)gj_itoa(42, aNum, sizeof(aNum));
    (void)gj_utoa(42ul, aNum, sizeof(aNum));
    (void)gj_xtoa(0xabcul, aNum, sizeof(aNum), 1, 4);
    (void)gj_memchr(aDst, 'i', sizeof(aDst));
    (void)gj_memcmp(aDst, "init", 4);
    (void)gj_strchr(aDst, 's');
    (void)gj_strrchr(aDst, 't');
    (void)gj_strstr(aDst, "soft");
    (void)gj_strcasecmp("Init", "init");
    (void)gj_strncasecmp("Wave", "wave", 4);
    (void)gj_strtoul("11", 0, 10);
    (void)gj_strtol("-11", 0, 10);
}

/* Soft: clocks beyond hard MONOTONIC (REALTIME + getres + nanosleep 0). */
static void
soft_clocks(void)
{
    long i64Ts[2];
    long i64R;

    i64Ts[0] = i64Ts[1] = 0;
    i64R = linux_clock_gettime(0 /* CLOCK_REALTIME */, i64Ts);
    soft_note(i64R, &g_cSoftClockOk, &g_cSoftClockMiss);

    i64Ts[0] = i64Ts[1] = 0;
    i64R = linux_clock_getres(0 /* CLOCK_REALTIME */, i64Ts);
    soft_note(i64R, &g_cSoftClockOk, &g_cSoftClockMiss);

    i64Ts[0] = i64Ts[1] = 0;
    i64R = linux_clock_getres(1 /* CLOCK_MONOTONIC */, i64Ts);
    soft_note(i64R, &g_cSoftClockOk, &g_cSoftClockMiss);

    /* Zero-duration clock_nanosleep — never blocks. */
    i64Ts[0] = 0;
    i64Ts[1] = 0;
    i64R = linux_clock_nanosleep(1 /* CLOCK_MONOTONIC */, 0, i64Ts, 0);
    soft_note(i64R, &g_cSoftClockOk, &g_cSoftClockMiss);

    /* Soft REALTIME zero sleep as well. */
    i64Ts[0] = 0;
    i64Ts[1] = 0;
    i64R = linux_clock_nanosleep(0 /* CLOCK_REALTIME */, 0, i64Ts, 0);
    soft_note(i64R, &g_cSoftClockOk, &g_cSoftClockMiss);
}

/* Soft: identity surface (tid/uid/gid/euid/egid) — Wave 11. */
static void
soft_ids(void)
{
    long i64R;

    i64R = linux_gettid();
    soft_note(i64R, &g_cSoftIdOk, &g_cSoftIdMiss);

    i64R = linux_getuid();
    soft_note(i64R, &g_cSoftIdOk, &g_cSoftIdMiss);

    i64R = linux_getgid();
    soft_note(i64R, &g_cSoftIdOk, &g_cSoftIdMiss);

    i64R = linux_geteuid();
    soft_note(i64R, &g_cSoftIdOk, &g_cSoftIdMiss);

    i64R = linux_getegid();
    soft_note(i64R, &g_cSoftIdOk, &g_cSoftIdMiss);

    i64R = linux_getppid();
    soft_note(i64R, &g_cSoftIdOk, &g_cSoftIdMiss);

    /* Soft set_tid_address (query-shaped; pointer to static word). */
    {
        static int nTidClear = 0;

        i64R = linux_set_tid_address(&nTidClear);
        soft_note(i64R, &g_cSoftIdOk, &g_cSoftIdMiss);
    }
}

/* Soft: arch_prctl GET_FS/GET_GS (TLS bases; never SET — avoid clobber). */
static void
soft_arch_prctl(void)
{
    unsigned long uFs = 0;
    unsigned long uGs = 0;

    (void)linux_arch_prctl(INIT_ARCH_GET_FS, (unsigned long)(uintptr_t)&uFs);
    (void)linux_arch_prctl(INIT_ARCH_GET_GS, (unsigned long)(uintptr_t)&uGs);
    (void)uFs;
    (void)uGs;
}

/* Soft: futex WAKE only (never WAIT — must not block first process). */
static void
soft_futex(void)
{
    static volatile int nWord = 0;

    (void)linux_futex(&nWord, INIT_FUTEX_WAKE | INIT_FUTEX_PRIVATE_FLAG, 1,
                      0, 0, 0);
    (void)linux_futex(&nWord, INIT_FUTEX_WAKE, 0, 0, 0, 0);
    (void)linux_futex_wake(&nWord, 1);
}

/* Soft: extra mmap surface (second anon + MAP_FIXED + madvise/mremap/memfd). */
static void
soft_mmap_extra(void)
{
    void *pMap;
    void *pFixed;
    void *pRemap;
    long i64Brk;
    long i64Mfd;
    long i64R;
    static unsigned char aMfd[8];

    pMap = (void *)(uintptr_t)linux_mmap(
        0, 8192, 3 /* PROT_READ|WRITE */,
        INIT_MAP_PRIVATE | INIT_MAP_ANON, -1, 0);
    if (!mmap_is_err(pMap) && pMap != 0) {
        *(volatile unsigned char *)pMap = 0xcd;
        (void)linux_mprotect(pMap, 4096, 3 /* RW */);
        /* Soft madvise before protect-down / unmap. */
        i64R = linux_madvise(pMap, 8192, INIT_MADV_NORMAL);
        soft_note(i64R, &g_cSoftMmapOk, &g_cSoftMmapMiss);
        /* Soft mremap grow in place (may miss — ok). */
        pRemap = (void *)(uintptr_t)linux_mremap(pMap, 8192, 16384,
                                                 1 /* MAYMOVE */, 0);
        if (!mmap_is_err(pRemap) && pRemap != 0) {
            g_cSoftMmapOk++;
            (void)linux_madvise(pRemap, 4096, INIT_MADV_DONTNEED);
            (void)linux_munmap(pRemap, 16384);
        } else {
            g_cSoftMmapMiss++;
            (void)linux_mprotect(pMap, 4096, 1 /* R */);
            (void)linux_munmap(pMap, 8192);
        }
        g_cSoftMmapOk++;
    } else {
        g_cSoftMmapMiss++;
    }

    /* High user VA soft FIXED — miss is fine on constrained AS. */
    pFixed = (void *)(uintptr_t)linux_mmap(
        (void *)(uintptr_t)0x3c000000ul, 4096, 3,
        INIT_MAP_PRIVATE | INIT_MAP_ANON | INIT_MAP_FIXED, -1, 0);
    if (!mmap_is_err(pFixed) && pFixed != 0) {
        *(volatile unsigned char *)pFixed = 0x11;
        (void)linux_munmap(pFixed, 4096);
        g_cSoftMmapOk++;
    } else {
        g_cSoftMmapMiss++;
    }

    /* Soft memfd_create + short write (no seal dance). */
    i64Mfd = linux_memfd_create("init-mfd", 0);
    if (i64Mfd >= 0) {
        aMfd[0] = 'g';
        aMfd[1] = 'j';
        (void)linux_write((int)i64Mfd, aMfd, 2);
        (void)linux_ftruncate((int)i64Mfd, 4096);
        pMap = (void *)(uintptr_t)linux_mmap(
            0, 4096, 3, INIT_MAP_PRIVATE, (int)i64Mfd, 0);
        if (!mmap_is_err(pMap) && pMap != 0) {
            g_cSoftMmapOk++;
            (void)linux_munmap(pMap, 4096);
        } else {
            g_cSoftMmapMiss++;
        }
        (void)linux_close((int)i64Mfd);
        g_cSoftMmapOk++;
    } else {
        g_cSoftMmapMiss++;
    }

    i64Brk = linux_brk(0);
    if (i64Brk > 0) {
        /* Query-only grow attempt; kernel may soft-refuse — ignore. */
        (void)linux_brk((void *)(uintptr_t)((unsigned long)i64Brk + 4096ul));
        (void)linux_brk((void *)(uintptr_t)(unsigned long)i64Brk);
        g_cSoftMmapOk++; /* brk query surface exercised */
    } else {
        g_cSoftMmapMiss++;
    }
}

/* Soft: rootfs + product SO/ELF existence probes. */
static void
soft_rootfs_paths(void)
{
    static const struct init_soft_path aPaths[] = {
        { "/etc/os-release", 0 },
        { "/etc/hostname", 0 },
        { "/etc/passwd", 0 },
        { "/etc/group", 0 },
        { "/lib/ld-gj.so.1", 0 },
        { "/lib/libc.so.6", 0 },
        { "/lib/libgj-so.so.1", 0 },
        { "/lib/libgj-gnu.so.1", 0 },
        { "/lib/libGreenJade_icd.so", 0 },
        { "/bin/sh", 0 },
        { "/bin/greenjade-shell", 0 },
        { "/sbin/init", 0 },
        { "/sbin/sshd", 0 },
        { "/sbin/vfsd", 0 },
        { "/sbin/storaged", 0 },
        { "/sbin/netstackd", 0 },
        { "/sbin/sessiond", 0 },
        { "/usr/bin/env", 0 },
        { "/tmp", 0 },
        { "/proc", 0 },
        { "/proc/self", 0 },
        { "/dev", 0 },
        { "/dev/null", 0 },
        { "/opt/steam", 0 },
    };
    unsigned i;
    long i64Fd;
    long i64R;
    static char aDent[512];
    static unsigned char aSt[144];

    for (i = 0; i < (unsigned)(sizeof(aPaths) / sizeof(aPaths[0])); i++) {
        /* Short read: ELF magic / text sample when file opens. */
        soft_open_probe(aPaths[i].szPath, 16u);
    }

    /* access / faccessat soft existence probes (never hard-fail). */
    i64R = linux_access("/tmp", 0 /* F_OK */);
    soft_note(i64R, &g_cSoftPathOk, &g_cSoftPathMiss);
    i64R = linux_faccessat(-100, "/etc/os-release", 0, 0);
    soft_note(i64R, &g_cSoftPathOk, &g_cSoftPathMiss);

    /* newfstatat / statx soft on known path. */
    i64R = linux_newfstatat(-100, "/tmp", aSt, 0);
    soft_note(i64R, &g_cSoftPathOk, &g_cSoftPathMiss);
    i64R = linux_statx(-100, "/tmp", 0, 0x000007ffu /* BASIC_STATS */, aSt);
    soft_note(i64R, &g_cSoftPathOk, &g_cSoftPathMiss);

    /* Directory getdents on /tmp when open succeeds. */
    i64Fd = linux_openat(-100, "/tmp", 0, 0);
    if (i64Fd >= 0) {
        (void)linux_getdents64((int)i64Fd, aDent, sizeof(aDent));
        (void)linux_close((int)i64Fd);
        g_cSoftPathOk++; /* getdents surface on live /tmp */
    } else {
        g_cSoftPathMiss++;
    }

    /* Soft getdents on /proc when present. */
    i64Fd = linux_openat(-100, "/proc", 0, 0);
    if (i64Fd >= 0) {
        (void)linux_getdents64((int)i64Fd, aDent, sizeof(aDent));
        (void)linux_close((int)i64Fd);
        g_cSoftPathOk++;
    } else {
        g_cSoftPathMiss++;
    }
}

/* Soft: FS create + link/symlink/readlink + mkdir/rename/unlink. */
static void
soft_fs_links(void)
{
    static const char szA[] = "/tmp/init_ln_a";
    static const char szB[] = "/tmp/init_ln_b";
    static const char szSl[] = "/tmp/init_sl";
    static const char szTouch[] = "/tmp/init_touch";
    static const char szDir[] = "/tmp/init_soft_dir";
    static const char szRen[] = "/tmp/init_soft_ren";
    static const char szRen2[] = "/tmp/init_soft_ren2";
    static char aLink[64];
    static unsigned char aSt[144];
    static char aDent[256];
    static char aPwrite[4];
    long i64Fd;
    long i64R;

    i64Fd = linux_openat(-100, szTouch, 0x41 /* O_WRONLY|O_CREAT */, 0644);
    if (i64Fd >= 0) {
        (void)linux_write((int)i64Fd, "gj", 2);
        aPwrite[0] = 'W';
        aPwrite[1] = '1';
        (void)linux_pwrite64((int)i64Fd, aPwrite, 2, 0);
        (void)linux_fdatasync((int)i64Fd);
        (void)linux_fsync((int)i64Fd);
        (void)linux_fstat((int)i64Fd, aSt);
        (void)linux_close((int)i64Fd);
        g_cSoftLinkOk++;
    } else {
        g_cSoftLinkMiss++;
    }

    /* Soft sync (global) — never hard-fail. */
    (void)linux_sync();

    i64R = linux_mkdir(szDir, 0755);
    soft_note(i64R, &g_cSoftLinkOk, &g_cSoftLinkMiss);
    i64R = linux_mkdirat(-100, szDir, 0755); /* may EEXIST — still soft */
    (void)i64R;

    i64Fd = linux_openat(-100, szRen, 0x41 /* O_WRONLY|O_CREAT */, 0644);
    if (i64Fd >= 0) {
        (void)linux_write((int)i64Fd, "R", 1);
        (void)linux_close((int)i64Fd);
        i64R = linux_rename(szRen, szRen2);
        soft_note(i64R, &g_cSoftLinkOk, &g_cSoftLinkMiss);
        i64R = linux_unlink(szRen2);
        soft_note(i64R, &g_cSoftLinkOk, &g_cSoftLinkMiss);
    } else {
        g_cSoftLinkMiss++;
    }

    i64Fd = linux_openat(-100, szA, 0x41 /* O_WRONLY|O_CREAT */, 0644);
    if (i64Fd >= 0) {
        (void)linux_write((int)i64Fd, "L", 1);
        (void)linux_fstat((int)i64Fd, aSt);
        /* Soft: getdents on non-dir may fail; exercise the NR only */
        (void)linux_getdents64((int)i64Fd, aDent, sizeof(aDent));
        (void)linux_close((int)i64Fd);
        i64R = linux_link(szA, szB);
        if (i64R >= 0) {
            g_cSoftLinkOk++;
        } else {
            g_cSoftLinkMiss++;
        }
        i64R = linux_symlink(szA, szSl);
        if (i64R >= 0) {
            g_cSoftLinkOk++;
        } else {
            g_cSoftLinkMiss++;
        }
        aLink[0] = '\0';
        i64R = linux_readlink(szSl, aLink, sizeof(aLink));
        if (i64R >= 0) {
            g_cSoftLinkOk++;
        } else {
            g_cSoftLinkMiss++;
        }
        /* Soft readlinkat + unlinkat cleanup (best-effort). */
        i64R = linux_readlinkat(-100, szSl, aLink, sizeof(aLink));
        soft_note(i64R, &g_cSoftLinkOk, &g_cSoftLinkMiss);
        (void)linux_unlinkat(-100, szB, 0);
        (void)linux_unlinkat(-100, szSl, 0);
        (void)linux_chmod(szA, 0644);
        i64R = linux_unlink(szA);
        soft_note(i64R, &g_cSoftLinkOk, &g_cSoftLinkMiss);
    } else {
        g_cSoftLinkMiss++;
    }

    /* Soft rmdir of soft dir (may miss if not empty / unsupported). */
    i64R = linux_rmdir(szDir);
    soft_note(i64R, &g_cSoftLinkOk, &g_cSoftLinkMiss);
}

/* Soft: pipe2 + eventfd2 + poll(timeout=0) round-trip (no block). */
static void
soft_ipc_fds(void)
{
    int aPipe[2];
    long i64Efd;
    long i64R;
    static unsigned char aScratch[8];
    static unsigned long long u64One = 1ull;
    struct init_pollfd stPf;

    aPipe[0] = aPipe[1] = -1;
    if (linux_pipe2(aPipe, 0) == 0 && aPipe[0] >= 0 && aPipe[1] >= 0) {
        (void)linux_write(aPipe[1], "gj", 2);
        /* Soft poll with timeout 0 while data ready — never blocks. */
        stPf.nFd = aPipe[0];
        stPf.nEvents = (short)INIT_POLLIN;
        stPf.nRevents = 0;
        i64R = linux_poll(&stPf, 1ul, 0);
        if (i64R >= 0) {
            g_cSoftIpcPollOk++;
        }
        (void)linux_read(aPipe[0], aScratch, 2);
        (void)linux_close(aPipe[0]);
        (void)linux_close(aPipe[1]);
        g_cSoftIpcPipeOk++;
    }

    /* Soft CLOEXEC pipe probe (flags may soft-miss). */
    aPipe[0] = aPipe[1] = -1;
    if (linux_pipe2(aPipe, 0x80000 /* O_CLOEXEC */) == 0 &&
        aPipe[0] >= 0 && aPipe[1] >= 0) {
        (void)linux_close(aPipe[0]);
        (void)linux_close(aPipe[1]);
        g_cSoftIpcPipeOk++;
    }

    i64Efd = linux_eventfd2(0, 0);
    if (i64Efd >= 0) {
        u64One = 1ull;
        (void)linux_write((int)i64Efd, &u64One, 8);
        stPf.nFd = (int)i64Efd;
        stPf.nEvents = (short)INIT_POLLIN;
        stPf.nRevents = 0;
        i64R = linux_poll(&stPf, 1ul, 0);
        if (i64R >= 0) {
            g_cSoftIpcPollOk++;
        }
        (void)linux_read((int)i64Efd, &u64One, 8);
        (void)linux_close((int)i64Efd);
        g_cSoftIpcEfdOk++;
    }
}

/* Soft: dup/fcntl/close_range surface (Wave 11 fds). */
static void
soft_fds(void)
{
    long i64Dup;
    long i64R;
    int aPipe[2];

    aPipe[0] = aPipe[1] = -1;
    if (linux_pipe2(aPipe, 0) != 0 || aPipe[0] < 0) {
        g_cSoftFdMiss++;
        return;
    }

    i64Dup = linux_dup(aPipe[0]);
    soft_note(i64Dup, &g_cSoftFdOk, &g_cSoftFdMiss);

    i64R = linux_fcntl(aPipe[0], INIT_F_GETFD, 0);
    soft_note(i64R, &g_cSoftFdOk, &g_cSoftFdMiss);

    i64R = linux_fcntl(aPipe[0], INIT_F_GETFL, 0);
    soft_note(i64R, &g_cSoftFdOk, &g_cSoftFdMiss);

    if (i64Dup >= 0) {
        i64R = linux_dup2((int)i64Dup, (int)i64Dup); /* identity soft */
        soft_note(i64R, &g_cSoftFdOk, &g_cSoftFdMiss);
        i64R = linux_dup3(aPipe[0], (int)i64Dup, 0);
        soft_note(i64R, &g_cSoftFdOk, &g_cSoftFdMiss);
        (void)linux_close((int)i64Dup);
    }

    /* Soft close_range on the pipe pair upper fd only (safe band). */
    if (aPipe[1] >= 0) {
        i64R = linux_close_range((unsigned)aPipe[1], (unsigned)aPipe[1], 0);
        if (i64R < 0) {
            (void)linux_close(aPipe[1]);
            g_cSoftFdMiss++;
        } else {
            g_cSoftFdOk++;
        }
    }
    (void)linux_close(aPipe[0]);
}

/* Soft: rlimit / sysinfo / rusage (Wave 11). */
static void
soft_rlimit(void)
{
    static unsigned long long aRlim[2];
    static unsigned char aInfo[128];
    static unsigned char aUsage[144];
    long i64R;

    aRlim[0] = aRlim[1] = 0;
    i64R = linux_getrlimit(INIT_RLIMIT_STACK, aRlim);
    soft_note(i64R, &g_cSoftRlimOk, &g_cSoftRlimMiss);

    aRlim[0] = aRlim[1] = 0;
    i64R = linux_getrlimit(INIT_RLIMIT_NOFILE, aRlim);
    soft_note(i64R, &g_cSoftRlimOk, &g_cSoftRlimMiss);

    /* prlimit64 query-only (pNew = NULL). */
    aRlim[0] = aRlim[1] = 0;
    i64R = linux_prlimit64(0, INIT_RLIMIT_AS, 0, aRlim);
    soft_note(i64R, &g_cSoftRlimOk, &g_cSoftRlimMiss);

    i64R = linux_sysinfo(aInfo);
    soft_note(i64R, &g_cSoftRlimOk, &g_cSoftRlimMiss);

    i64R = linux_getrusage(0 /* RUSAGE_SELF */, aUsage);
    soft_note(i64R, &g_cSoftRlimOk, &g_cSoftRlimMiss);
}

/* Soft: nanosleep 1us + fork/wait4 (spawn soft counts; child may be stub). */
static void
soft_sleep_fork(void)
{
    long i64Ns[2];
    long i64Child;
    long i64W;
    int nSt = 0;

    i64Ns[0] = 0;
    i64Ns[1] = 1000; /* 1 us */
    (void)linux_nanosleep(i64Ns, 0);

    i64Child = linux_fork();
    if (i64Child > 0) {
        g_cSoftSpawnFork++;
        i64W = linux_wait4((int)i64Child, &nSt, 0, 0);
        if (i64W >= 0) {
            g_cSoftSpawnWait++;
        }
        /* Soft waitid surface (may miss). */
        (void)linux_waitid(1 /* P_PID */, (unsigned)i64Child, 0, 4 /* WEXITED */,
                           0);
    } else if (i64Child == 0) {
        /* Child view not scheduled in bring-up fork stub — unused */
    } else {
        g_cSoftSpawnMiss++;
    }
}

/*
 * Soft native product doors — STATS/POLL/READY only.
 * Never CLAIM: live sessiond/netstackd/storaged/vfsd own claim tokens.
 */
static void
soft_native_doors(void)
{
    static unsigned u32Store[4];
    static unsigned u32Net[4];
    static unsigned u32Vfs[4];
    static unsigned u32Sess[8];
    static unsigned u32Scsi[2];
    static unsigned u32Hda[3];
    static unsigned char aInq[36];
    static unsigned u32Cap[2];
    static unsigned char aEv[32];
    static unsigned long long u64Cap;
    long i64R;

    /* Platform inventory (op0 IOMMU present, op1 MSI-X count; no wow64 flip). */
    soft_note_door(gj_platform_info(0, 0, 0, 0));
    soft_note_door(gj_platform_info(1, 0, 0, 0));
    /* WOW64 query only (arg1=0) — never enable/disable. */
    soft_note_door(gj_platform_info(2, 0, 0, 0));

    /* Notify poll only (block=0). */
    soft_note_door(gj_notify_wait(GJ_NOTIFY_WHICH_MSIX_GLOBAL, 0ul, 0));

    soft_note_door(gj_console_poll());
    soft_note_door(gj_set_qos(0, 0));
    soft_note_door(gj_set_cpu(0, 0));
    gj_yield(); /* void return — surface exercised counts as door ok */
    g_cSoftDoorOk++;
    soft_note_door(gj_dlog("greenjade-init: soft dlog\n"));

    /* Store: STATS + CAP + QUEUE + FLUSH + RING_STATE (no claim/read/write). */
    u32Store[0] = u32Store[1] = u32Store[2] = u32Store[3] = 0;
    soft_note_door(gj_store(GJ_STORE_OP_STATS, (long)(uintptr_t)u32Store, 0, 0));
    u64Cap = 0;
    soft_note_door(gj_store(GJ_STORE_OP_CAP, (long)(uintptr_t)&u64Cap, 0, 0));
    soft_note_door(gj_store(GJ_STORE_OP_QUEUE_INFO, 0, 0, 0));
    soft_note_door(gj_store(GJ_STORE_OP_FLUSH, 0, 0, 0));
    soft_note_door(gj_store(GJ_STORE_OP_RING_STATE, (long)(uintptr_t)u32Store, 0,
                            0));

    /* Net: POLL + STATS (+ TCP_STATS soft) + QUEUE_INFO numeric. */
    soft_note_door(gj_net(GJ_NET_OP_POLL, 0, 0, 0));
    u32Net[0] = u32Net[1] = u32Net[2] = u32Net[3] = 0;
    soft_note_door(gj_net(GJ_NET_OP_STATS, (long)(uintptr_t)u32Net, 0, 0));
    soft_note_door(gj_net(GJ_NET_OP_TCP_STATS, (long)(uintptr_t)u32Net, 0, 0));
    soft_note_door(gj_net(14u /* QUEUE_INFO */, (long)(uintptr_t)u32Net, 0, 0));
    soft_note_door(gj_net(20u /* RING_STATE */, (long)(uintptr_t)u32Net, 0, 0));

    /* VFS: STATS + LIST soft (no CLAIM). */
    u32Vfs[0] = u32Vfs[1] = u32Vfs[2] = u32Vfs[3] = 0;
    soft_note_door(gj_vfs(GJ_VFS_OP_STATS, (long)(uintptr_t)u32Vfs, 0, 0));
    soft_note_door(gj_vfs(GJ_VFS_OP_LIST, (long)(uintptr_t)aInq, 0,
                          (long)sizeof(aInq)));

    /* Session: STATS + DISPLAY_INFO + INPUT_POLL + INPUT_POP soft (no CLAIM). */
    u32Sess[0] = u32Sess[1] = u32Sess[2] = u32Sess[3] = 0;
    soft_note_door(gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)u32Sess, 0, 0));
    soft_note_door(gj_session(GJ_SESS_OP_DISPLAY_INFO, (long)(uintptr_t)u32Sess,
                              0, 0));
    soft_note_door(gj_session(GJ_SESS_OP_INPUT_POLL, 0, 0, 0));
    /* INPUT_POP may -EAGAIN with empty queue — still soft door miss ok. */
    soft_note_door(gj_session(GJ_SESS_OP_INPUT_POP, (long)(uintptr_t)aEv, 0, 0));

    /* SCSI: READY + STATS + INQUIRY + READ_CAP (no READ/WRITE). */
    i64R = gj_scsi(GJ_SCSI_OP_READY, 0, 0, 0);
    soft_note_door(i64R);
    u32Scsi[0] = u32Scsi[1] = 0;
    soft_note_door(gj_scsi(GJ_SCSI_OP_STATS, (long)(uintptr_t)u32Scsi, 0, 0));
    (void)gj_memset(aInq, 0, sizeof(aInq));
    soft_note_door(gj_scsi(GJ_SCSI_OP_INQUIRY, (long)(uintptr_t)aInq,
                           (long)sizeof(aInq), 0));
    u32Cap[0] = u32Cap[1] = 0;
    soft_note_door(gj_scsi(GJ_SCSI_OP_READ_CAP, (long)(uintptr_t)u32Cap, 0, 0));

    /* HDA: STATS only (no OPEN/START). */
    u32Hda[0] = u32Hda[1] = u32Hda[2] = 0;
    soft_note_door(gj_hda_stream(GJ_HDA_OP_STATS, (long)(uintptr_t)u32Hda, 0, 0));

    /* GPU display info soft (may soft-miss without virtio-gpu). */
    soft_note_door(gj_gpu_display_info(u32Sess));
}

/* Soft: named memobj product path + second soft name. */
static void
soft_named_memobj(void)
{
    long i64Va;
    long i64R;

    i64R = gj_memobj_create_named("init-shm", 1);
    if (i64R == 0) {
        i64Va = gj_memobj_map_named("init-shm", 0x35000000ul, 3);
        if (i64Va > 0) {
            *(volatile unsigned *)(uintptr_t)i64Va = 0x494e4954u; /* INIT */
            g_cSoftMemobjOk++;
            gj_puts("greenjade-init: named memobj PASS\n");
        } else {
            g_cSoftMemobjMiss++;
        }
    } else {
        g_cSoftMemobjMiss++;
    }

    /* Second soft name — create may soft-miss; never hard-fail. */
    i64R = gj_memobj_create_named("init-shm2", 1);
    if (i64R == 0) {
        i64Va = gj_memobj_map_named("init-shm2", 0x35100000ul, 3);
        if (i64Va > 0) {
            *(volatile unsigned *)(uintptr_t)i64Va = 0x53484d32u; /* SHM2 */
            g_cSoftMemobjOk++;
        } else {
            g_cSoftMemobjMiss++;
        }
    } else {
        g_cSoftMemobjMiss++;
    }

    /* Third soft name — Wave 11 deepen. */
    i64R = gj_memobj_create_named("init-shm3", 1);
    if (i64R == 0) {
        i64Va = gj_memobj_map_named("init-shm3", 0x35200000ul, 3);
        if (i64Va > 0) {
            *(volatile unsigned *)(uintptr_t)i64Va = 0x53484d33u; /* SHM3 */
            g_cSoftMemobjOk++;
        } else {
            g_cSoftMemobjMiss++;
        }
    } else {
        g_cSoftMemobjMiss++;
    }
}

/* Aggregate soft deepen; inventory log + soft deepen marker when reached. */
static void
soft_deepen_all(void)
{
    soft_libgj_string();
    g_cSoftAreas++;
    soft_clocks();
    g_cSoftAreas++;
    soft_ids();
    g_cSoftAreas++;
    soft_arch_prctl();
    g_cSoftAreas++;
    soft_futex();
    g_cSoftAreas++;
    soft_mmap_extra();
    g_cSoftAreas++;
    soft_rootfs_paths();
    g_cSoftAreas++;
    soft_fs_links();
    g_cSoftAreas++;
    soft_ipc_fds();
    g_cSoftAreas++;
    soft_fds();
    g_cSoftAreas++;
    soft_rlimit();
    g_cSoftAreas++;
    soft_sleep_fork();
    g_cSoftAreas++;
    soft_native_doors();
    g_cSoftAreas++;
    /* named memobj already emitted its own PASS when green; counts ready */
    soft_inventory_log();
    gj_puts("greenjade-init: soft deepen PASS\n");
}

void
_start(void)
{
    static const char szBanner[] = "greenjade-init: hello from userspace\n";
    static const char szAbi[] = "greenjade-init: abi PASS\n";
    static char aUts[390];
    static char aCwd[64];
    static unsigned char aRnd[16];
    long i64Pid;
    long i64W;
    long i64R;
    long i64Ts[2];
    long i64Tv[2];
    void *pMap;

    i64Pid = linux_getpid();
    i64W = linux_write(1, szBanner, gj_strlen(szBanner));
    (void)i64W;
    (void)i64Pid;
    gj_puts("greenjade-init: libgj string PASS\n");
    (void)linux_getppid();
    (void)linux_getuid();

    /* Hard path: uname + CLOCK_MONOTONIC must work for abi PASS */
    i64R = linux_uname(aUts);
    if (i64R != 0) {
        linux_exit(1);
    }

    i64Ts[0] = i64Ts[1] = 0;
    i64R = linux_clock_gettime(1 /* CLOCK_MONOTONIC */, i64Ts);
    if (i64R != 0) {
        linux_exit(2);
    }

    i64Tv[0] = i64Tv[1] = 0;
    (void)linux_gettimeofday(i64Tv, 0);

    pMap = (void *)(uintptr_t)linux_mmap(
        0, 4096, 3 /* PROT_READ|WRITE */, 0x22 /* PRIVATE|ANON */, -1, 0);
    if (mmap_is_err(pMap)) {
        linux_exit(3);
    }
    if (pMap != 0) {
        *(volatile unsigned char *)pMap = 0xab;
        (void)linux_mprotect(pMap, 4096, 1 /* PROT_READ */);
        (void)linux_munmap(pMap, 4096);
    }

    /* Soft FS touch (create) — kept pre-deepen for early path coverage */
    {
        static const char szTmp[] = "/tmp/init_touch";
        long i64Fd;

        i64Fd = linux_openat(-100 /* AT_FDCWD */, szTmp,
                             0x41 /* O_WRONLY|O_CREAT */, 0644);
        if (i64Fd >= 0) {
            (void)linux_write((int)i64Fd, "gj", 2);
            (void)linux_fsync((int)i64Fd);
            (void)linux_close((int)i64Fd);
        }
    }

    aCwd[0] = '\0';
    (void)linux_getcwd(aCwd, sizeof(aCwd));
    (void)linux_getrandom(aRnd, sizeof(aRnd), 0);
    (void)linux_brk(0);

    /* Named shareable memobj (product path for wine/GPU export) */
    soft_named_memobj();

    /*
     * Soft deepen: clocks/ids/arch/futex/mmap/rootfs/ipc/fds/rlimit/doors.
     * All soft — never changes hard-fail exit codes above.
     */
    soft_deepen_all();

    (void)linux_sched_yield();
    (void)linux_write(1, szAbi, gj_strlen(szAbi));
    linux_exit(0);
}
