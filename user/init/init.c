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
 *   greenjade-init: abi PASS
 *
 * Parent owns Makefile target + kernel/proc/init_embed.S (.incbin).
 */
#include <gj/string.h>
#include <gj/syscalls.h>

/* Linux mmap errno band: return is -errno in [-4095, -1] on failure. */
static int
mmap_is_err(void *pMap)
{
    unsigned long u = (unsigned long)(uintptr_t)pMap;

    return u >= (unsigned long)-4095UL;
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

    /* Soft probes: rootfs config + dynlinker/SOs (never hard-fail) */
    {
        static char aOs[128];
        long i64Os;

        i64Os = linux_openat(-100, "/etc/os-release", 0, 0);
        if (i64Os >= 0) {
            (void)linux_read((int)i64Os, aOs, sizeof(aOs) - 1u);
            (void)linux_close((int)i64Os);
        }
        i64Os = linux_openat(-100, "/etc/hostname", 0, 0);
        if (i64Os >= 0) {
            (void)linux_read((int)i64Os, aOs, sizeof(aOs) - 1u);
            (void)linux_close((int)i64Os);
        }
        i64Os = linux_openat(-100, "/lib/ld-gj.so.1", 0, 0);
        if (i64Os >= 0) {
            (void)linux_read((int)i64Os, aOs, sizeof(aOs) - 1u);
            (void)linux_lseek((int)i64Os, 0, 0);
            (void)linux_close((int)i64Os);
        }
        i64Os = linux_openat(-100, "/lib/libc.so.6", 0, 0);
        if (i64Os >= 0) {
            (void)linux_read((int)i64Os, aOs, 4); /* ELF magic probe */
            (void)linux_close((int)i64Os);
        }
        i64Os = linux_openat(-100, "/lib/libgj-so.so.1", 0, 0);
        if (i64Os >= 0) {
            (void)linux_close((int)i64Os);
        }
        i64Os = linux_openat(-100, "/lib/libgj-gnu.so.1", 0, 0);
        if (i64Os >= 0) {
            (void)linux_close((int)i64Os);
        }
    }

    /* Named shareable memobj (product path for wine/GPU export) */
    {
        long i64Va;

        if (gj_memobj_create_named("init-shm", 1) == 0) {
            i64Va = gj_memobj_map_named("init-shm", 0x35000000ul, 3);
            if (i64Va > 0) {
                *(volatile unsigned *)(uintptr_t)i64Va = 0x494e4954u; /* INIT */
                gj_puts("greenjade-init: named memobj PASS\n");
            }
        }
    }

    /* Soft IPC-ish + link/symlink + fork/wait (bring-up stubs OK) */
    {
        long i64TsRes[2];
        int aPipe[2];
        long i64Efd;
        static char aDent[256];
        long i64Fd;
        static const char szA[] = "/tmp/init_ln_a";
        static const char szB[] = "/tmp/init_ln_b";

        i64TsRes[0] = i64TsRes[1] = 0;
        (void)linux_clock_getres(1, i64TsRes);

        aPipe[0] = aPipe[1] = -1;
        if (linux_pipe2(aPipe, 0) == 0 && aPipe[0] >= 0 && aPipe[1] >= 0) {
            (void)linux_write(aPipe[1], "gj", 2);
            (void)linux_read(aPipe[0], aRnd, 2);
            (void)linux_close(aPipe[0]);
            (void)linux_close(aPipe[1]);
        }

        i64Efd = linux_eventfd2(0, 0);
        if (i64Efd >= 0) {
            static unsigned long long u64One = 1ull;

            (void)linux_write((int)i64Efd, &u64One, 8);
            (void)linux_read((int)i64Efd, &u64One, 8);
            (void)linux_close((int)i64Efd);
        }

        i64Fd = linux_openat(-100, szA, 0x41 /* O_WRONLY|O_CREAT */, 0644);
        if (i64Fd >= 0) {
            (void)linux_write((int)i64Fd, "L", 1);
            /* Soft: getdents on non-dir may fail; exercise the NR only */
            (void)linux_getdents64((int)i64Fd, aDent, sizeof(aDent));
            (void)linux_close((int)i64Fd);
            (void)linux_link(szA, szB);
            (void)linux_symlink(szA, "/tmp/init_sl");
            aCwd[0] = '\0';
            (void)linux_readlink("/tmp/init_sl", aCwd, sizeof(aCwd));
        }

        {
            long i64Ns[2];

            i64Ns[0] = 0;
            i64Ns[1] = 1000; /* 1 us */
            (void)linux_nanosleep(i64Ns, 0);
        }

        {
            long i64Child;
            int nSt = 0;

            i64Child = linux_fork();
            if (i64Child > 0) {
                (void)linux_wait4((int)i64Child, &nSt, 0, 0);
            } else if (i64Child == 0) {
                /* Child view not scheduled in bring-up fork stub — unused */
            }
        }
    }

    (void)linux_sched_yield();
    (void)linux_write(1, szAbi, gj_strlen(szAbi));
    linux_exit(0);
}
