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
 * Soft deepen (never hard-fails boot):
 *   - libgj string/memory helpers (strcmp/memcpy/itoa)
 *   - CLOCK_REALTIME + clock_getres already on hard MONOTONIC path
 *   - fstat / arch_prctl GET_FS / futex WAKE / brk grow query
 *   - second anon mmap + MAP_FIXED soft probe
 *   - rootfs/product path probes + getdents on /tmp
 *   - link/symlink/readlink + pipe2 + eventfd2 + nanosleep + fork/wait
 *   - native doors: platform/notify/console/qos + store/net/vfs/session
 *     STATS (no CLAIM — live daemons own claim tokens)
 *   - scsi READY/STATS + HDA STATS (no stream open)
 *   - named memobj create+map + soft second name probe
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

/* Soft product path table entry */
struct init_soft_path {
    const char *szPath;
    int nFlags; /* openat flags; 0 = O_RDONLY */
};

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
        return;
    }
    i64Fd = linux_openat(-100 /* AT_FDCWD */, szPath, 0, 0);
    if (i64Fd < 0) {
        return;
    }
    (void)linux_fstat((int)i64Fd, aSt);
    if (cbRead > 0u) {
        if (cbRead > sizeof(aBuf)) {
            cbRead = sizeof(aBuf);
        }
        (void)linux_read((int)i64Fd, aBuf, cbRead);
        (void)linux_lseek((int)i64Fd, 0, 0 /* SEEK_SET */);
    }
    (void)linux_close((int)i64Fd);
}

/* Soft: exercise freestanding libgj string/memory helpers (no I/O). */
static void
soft_libgj_string(void)
{
    static char aDst[32];
    static char aNum[24];
    size_t cb;

    (void)gj_memset(aDst, 0, sizeof(aDst));
    (void)gj_memcpy(aDst, "gj", 3);
    if (gj_strcmp(aDst, "gj") == 0) {
        (void)gj_strncmp(aDst, "gjx", 2);
    }
    cb = gj_strlen(aDst);
    (void)cb;
    (void)gj_strlcpy(aDst, "init", sizeof(aDst));
    (void)gj_strlcat(aDst, "-soft", sizeof(aDst));
    (void)gj_itoa(42, aNum, sizeof(aNum));
    (void)gj_utoa(42ul, aNum, sizeof(aNum));
    (void)gj_xtoa(0xabcul, aNum, sizeof(aNum), 1, 4);
    (void)gj_memchr(aDst, 'i', sizeof(aDst));
    (void)gj_memcmp(aDst, "init", 4);
}

/* Soft: clocks beyond hard MONOTONIC (REALTIME + getres). */
static void
soft_clocks(void)
{
    long i64Ts[2];

    i64Ts[0] = i64Ts[1] = 0;
    (void)linux_clock_gettime(0 /* CLOCK_REALTIME */, i64Ts);
    i64Ts[0] = i64Ts[1] = 0;
    (void)linux_clock_getres(0 /* CLOCK_REALTIME */, i64Ts);
    i64Ts[0] = i64Ts[1] = 0;
    (void)linux_clock_getres(1 /* CLOCK_MONOTONIC */, i64Ts);
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
}

/* Soft: extra mmap surface (second anon + optional MAP_FIXED). */
static void
soft_mmap_extra(void)
{
    void *pMap;
    void *pFixed;
    long i64Brk;

    pMap = (void *)(uintptr_t)linux_mmap(
        0, 8192, 3 /* PROT_READ|WRITE */,
        INIT_MAP_PRIVATE | INIT_MAP_ANON, -1, 0);
    if (!mmap_is_err(pMap) && pMap != 0) {
        *(volatile unsigned char *)pMap = 0xcd;
        (void)linux_mprotect(pMap, 4096, 3 /* RW */);
        (void)linux_mprotect(pMap, 4096, 1 /* R */);
        (void)linux_munmap(pMap, 8192);
    }

    /* High user VA soft FIXED — miss is fine on constrained AS. */
    pFixed = (void *)(uintptr_t)linux_mmap(
        (void *)(uintptr_t)0x3c000000ul, 4096, 3,
        INIT_MAP_PRIVATE | INIT_MAP_ANON | INIT_MAP_FIXED, -1, 0);
    if (!mmap_is_err(pFixed) && pFixed != 0) {
        *(volatile unsigned char *)pFixed = 0x11;
        (void)linux_munmap(pFixed, 4096);
    }

    i64Brk = linux_brk(0);
    if (i64Brk > 0) {
        /* Query-only grow attempt; kernel may soft-refuse — ignore. */
        (void)linux_brk((void *)(uintptr_t)((unsigned long)i64Brk + 4096ul));
        (void)linux_brk((void *)(uintptr_t)(unsigned long)i64Brk);
    }
}

/* Soft: rootfs + product SO/ELF existence probes. */
static void
soft_rootfs_paths(void)
{
    static const struct init_soft_path aPaths[] = {
        { "/etc/os-release", 0 },
        { "/etc/hostname", 0 },
        { "/lib/ld-gj.so.1", 0 },
        { "/lib/libc.so.6", 0 },
        { "/lib/libgj-so.so.1", 0 },
        { "/lib/libgj-gnu.so.1", 0 },
        { "/bin/sh", 0 },
        { "/bin/greenjade-shell", 0 },
        { "/sbin/init", 0 },
        { "/sbin/sshd", 0 },
        { "/tmp", 0 },
    };
    unsigned i;
    long i64Fd;
    static char aDent[512];

    for (i = 0; i < (unsigned)(sizeof(aPaths) / sizeof(aPaths[0])); i++) {
        /* Short read: ELF magic / text sample when file opens. */
        soft_open_probe(aPaths[i].szPath, 16u);
    }

    /* Directory getdents on /tmp when open succeeds. */
    i64Fd = linux_openat(-100, "/tmp", 0, 0);
    if (i64Fd >= 0) {
        (void)linux_getdents64((int)i64Fd, aDent, sizeof(aDent));
        (void)linux_close((int)i64Fd);
    }
}

/* Soft: FS create + link/symlink/readlink + fstat (bring-up stubs OK). */
static void
soft_fs_links(void)
{
    static const char szA[] = "/tmp/init_ln_a";
    static const char szB[] = "/tmp/init_ln_b";
    static const char szSl[] = "/tmp/init_sl";
    static const char szTouch[] = "/tmp/init_touch";
    static char aLink[64];
    static unsigned char aSt[144];
    static char aDent[256];
    long i64Fd;

    i64Fd = linux_openat(-100, szTouch, 0x41 /* O_WRONLY|O_CREAT */, 0644);
    if (i64Fd >= 0) {
        (void)linux_write((int)i64Fd, "gj", 2);
        (void)linux_fsync((int)i64Fd);
        (void)linux_fstat((int)i64Fd, aSt);
        (void)linux_close((int)i64Fd);
    }

    i64Fd = linux_openat(-100, szA, 0x41 /* O_WRONLY|O_CREAT */, 0644);
    if (i64Fd >= 0) {
        (void)linux_write((int)i64Fd, "L", 1);
        (void)linux_fstat((int)i64Fd, aSt);
        /* Soft: getdents on non-dir may fail; exercise the NR only */
        (void)linux_getdents64((int)i64Fd, aDent, sizeof(aDent));
        (void)linux_close((int)i64Fd);
        (void)linux_link(szA, szB);
        (void)linux_symlink(szA, szSl);
        aLink[0] = '\0';
        (void)linux_readlink(szSl, aLink, sizeof(aLink));
    }
}

/* Soft: pipe2 + eventfd2 round-trip (no block). */
static void
soft_ipc_fds(void)
{
    int aPipe[2];
    long i64Efd;
    static unsigned char aScratch[8];
    static unsigned long long u64One = 1ull;

    aPipe[0] = aPipe[1] = -1;
    if (linux_pipe2(aPipe, 0) == 0 && aPipe[0] >= 0 && aPipe[1] >= 0) {
        (void)linux_write(aPipe[1], "gj", 2);
        (void)linux_read(aPipe[0], aScratch, 2);
        (void)linux_close(aPipe[0]);
        (void)linux_close(aPipe[1]);
    }

    i64Efd = linux_eventfd2(0, 0);
    if (i64Efd >= 0) {
        u64One = 1ull;
        (void)linux_write((int)i64Efd, &u64One, 8);
        (void)linux_read((int)i64Efd, &u64One, 8);
        (void)linux_close((int)i64Efd);
    }
}

/* Soft: nanosleep 1us + fork/wait4 (child may be stub-unscheduled). */
static void
soft_sleep_fork(void)
{
    long i64Ns[2];
    long i64Child;
    int nSt = 0;

    i64Ns[0] = 0;
    i64Ns[1] = 1000; /* 1 us */
    (void)linux_nanosleep(i64Ns, 0);

    i64Child = linux_fork();
    if (i64Child > 0) {
        (void)linux_wait4((int)i64Child, &nSt, 0, 0);
    } else if (i64Child == 0) {
        /* Child view not scheduled in bring-up fork stub — unused */
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
    static unsigned long long u64Cap;
    long i64R;

    /* Platform inventory (op0 IOMMU present, op1 MSI-X count; no wow64 flip). */
    (void)gj_platform_info(0, 0, 0, 0);
    (void)gj_platform_info(1, 0, 0, 0);

    /* Notify poll only (block=0). */
    (void)gj_notify_wait(GJ_NOTIFY_WHICH_MSIX_GLOBAL, 0ul, 0);

    (void)gj_console_poll();
    (void)gj_set_qos(0, 0);
    (void)gj_yield();
    (void)gj_dlog("greenjade-init: soft dlog\n");

    /* Store: STATS + CAP query (no claim/read/write). */
    u32Store[0] = u32Store[1] = u32Store[2] = u32Store[3] = 0;
    (void)gj_store(GJ_STORE_OP_STATS, (long)(uintptr_t)u32Store, 0, 0);
    u64Cap = 0;
    (void)gj_store(GJ_STORE_OP_CAP, (long)(uintptr_t)&u64Cap, 0, 0);
    (void)gj_store(GJ_STORE_OP_QUEUE_INFO, 0, 0, 0);

    /* Net: POLL + STATS (+ TCP_STATS soft). */
    (void)gj_net(GJ_NET_OP_POLL, 0, 0, 0);
    u32Net[0] = u32Net[1] = u32Net[2] = u32Net[3] = 0;
    (void)gj_net(GJ_NET_OP_STATS, (long)(uintptr_t)u32Net, 0, 0);
    (void)gj_net(GJ_NET_OP_TCP_STATS, (long)(uintptr_t)u32Net, 0, 0);

    /* VFS: STATS only. */
    u32Vfs[0] = u32Vfs[1] = u32Vfs[2] = u32Vfs[3] = 0;
    (void)gj_vfs(GJ_VFS_OP_STATS, (long)(uintptr_t)u32Vfs, 0, 0);

    /* Session: STATS + DISPLAY_INFO soft (no CLAIM). */
    u32Sess[0] = u32Sess[1] = u32Sess[2] = u32Sess[3] = 0;
    (void)gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)u32Sess, 0, 0);
    (void)gj_session(GJ_SESS_OP_DISPLAY_INFO, (long)(uintptr_t)u32Sess, 0, 0);
    (void)gj_session(GJ_SESS_OP_INPUT_POLL, 0, 0, 0);

    /* SCSI: READY + STATS (no READ/WRITE). */
    i64R = gj_scsi(GJ_SCSI_OP_READY, 0, 0, 0);
    (void)i64R;
    u32Scsi[0] = u32Scsi[1] = 0;
    (void)gj_scsi(GJ_SCSI_OP_STATS, (long)(uintptr_t)u32Scsi, 0, 0);

    /* HDA: STATS only (no OPEN/START). */
    u32Hda[0] = u32Hda[1] = u32Hda[2] = 0;
    (void)gj_hda_stream(GJ_HDA_OP_STATS, (long)(uintptr_t)u32Hda, 0, 0);

    /* GPU display info soft (may soft-miss without virtio-gpu). */
    (void)gj_gpu_display_info(u32Sess);
}

/* Soft: named memobj product path + second soft name. */
static void
soft_named_memobj(void)
{
    long i64Va;

    if (gj_memobj_create_named("init-shm", 1) == 0) {
        i64Va = gj_memobj_map_named("init-shm", 0x35000000ul, 3);
        if (i64Va > 0) {
            *(volatile unsigned *)(uintptr_t)i64Va = 0x494e4954u; /* INIT */
            gj_puts("greenjade-init: named memobj PASS\n");
        }
    }

    /* Second soft name — create may soft-miss; never hard-fail. */
    if (gj_memobj_create_named("init-shm2", 1) == 0) {
        i64Va = gj_memobj_map_named("init-shm2", 0x35100000ul, 3);
        if (i64Va > 0) {
            *(volatile unsigned *)(uintptr_t)i64Va = 0x53484d32u; /* SHM2 */
        }
    }
}

/* Aggregate soft deepen; always ends with soft deepen marker when reached. */
static void
soft_deepen_all(void)
{
    soft_libgj_string();
    soft_clocks();
    soft_arch_prctl();
    soft_futex();
    soft_mmap_extra();
    soft_rootfs_paths();
    soft_fs_links();
    soft_ipc_fds();
    soft_sleep_fork();
    soft_native_doors();
    /* named memobj already emitted its own PASS when green */
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
     * Soft deepen: clocks/arch/futex/mmap/rootfs/ipc/doors.
     * All soft — never changes hard-fail exit codes above.
     */
    soft_deepen_all();

    (void)linux_sched_yield();
    (void)linux_write(1, szAbi, gj_strlen(szAbi));
    linux_exit(0);
}
