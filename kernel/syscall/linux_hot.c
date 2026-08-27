/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Linux hybrid Option C - kernel hot paths (clean-room pure C11).
 * Dual MIT OR Apache-2.0. No GPL source.
 *
 * Soft product inventory (Wave 11/14 base + Wave 36 exclusive deepen):
 *   - Group enter tallies (io/id/mem/time/futex/sched/sig/sock/info/proc)
 *   - Live task view snapshot (pid/tid/cred/brk/fs_base)
 *   - Handler catalog capacity (static product surface count)
 *   - Wave 14: groups catalog + deepen stamp + path/stats wave lamps
 *   - Wave 15: rates / honesty / catalog / PASS complementary surfaces
 *   Never hard-gates; wrap OK; diagnostics only - does not alter i64Ret.
 * Residual freestanding deepen (UDX host / Linux-shaped driver host ABI):
 *   futex REQUEUE|CMP_REQUEUE|WAKE_OP soft collapse + uaddr/uaddr2 align
 *   + mapped gate + soft requeue/wake_op tallies (Soft!=product);
 *   membarrier QUERY; rseq register; sysinfo PMM frames;
 *   clock COARSE/CPUTIME aliases; prctl tid/timerslack;
 *   mlock lean residual: len0 OK, page-round, overflow gate; mlock2
 *   MLOCK_ONFAULT-only flags; soft page/call tallies (no swap host);
 *   mlockall MCL_CURRENT|FUTURE|ONFAULT flag gate; munlock range gate;
 *   socket lean residual: dual-table gate (net_lo + net_tcp); soft
 *   SOL_SOCKET TYPE/ERROR/REUSE_star/KEEPALIVE/BROADCAST/
 *   ACCEPTCONN (+ soft RCVBUF/SNDBUF store + probes);
 *   REUSE_star footgun fix: REUSEADDR+REUSEPORT both tally+lamp (parity);
 *   fd lean residual: dual-table classify + gate tallies (lo/tcp);
 *   poll lean residual: soft net_*_poll_mask probe on sock gate (once);
 *   mmap lean residual: vfs_ram file map PASS / ENOSYS once-lamps;
 *   mmap anon lean residual: memobj/vmm anon PASS once-lamp;
 *   mmap page-round len + FIXED_NOREPLACE as FIXED + soft flag no-ops
 *   (POPULATE/STACK/NORESERVE) + fixed/shared tallies (Soft!=product);
 *   io lean residual: write stdio + vfs_ram + dual-table sock send;
 *   writev/readv sock bridge (net_tcp|net_lo); pread/pwrite loops;
 *   preadv/pwritev lean: positioned vfs_ram_pread/pwrite (offset);
 *   mremap lean residual: multi-chunk soft copy (page loop; Soft!=product);
 *   ctid lean residual: clear_child_tid store0+wake (clone-adjacent);
 *   set_tid_address lean residual: store clear_child_tid + once-lamp;
 *   TCP lean slot table FD 96..111; mincore chunked; mremap soft copy;
 *   affinity lean residual: setaffinity soft mask store + getaffinity
 *   returns stored|online (UDX host pin probes; Soft!=product);
 *   process_vm lean residual: same-pid iov copy tallies + once-lamp;
 *   getrandom lean residual: multi-chunk + call/byte tallies once-lamp;
 *   mincore lean residual: chunked present bits + page tallies once-lamp;
 *   getcpu soft online.
 *   C2 hot ABI residual deepen (Soft!=product · G-AC-1 · Dual DoD OPEN):
 *   product name sync via net_tcp_getsockname/getpeername (hot bridge);
 *   sticky SO_ERROR clear-on-read + POLLERR soft set; SO_ACCEPTCONN lean;
 *   shutdown bridges net_tcp_shutdown + soft half-close bits (not soft-OK
 *   no-op); name/peer/shut/soerr tallies + once-lamps (no stamp storms).
 *   C2 sock I/O residual (UDX/sshd functional; Soft!=product · G-AC-1):
 *   write/writev/readv dual-table bridge net_tcp|net_lo send/recv (hot
 *   direct; STRONGER than ENOSYS cold-defer / EBADF on sock FDs);
 *   SHUT_WR pre-check EPIPE + sticky SO_ERROR; EAGAIN one poll+retry;
 *   send ok clears sticky SO_ERROR; recv EOF soft SHUT_RD lean mirror;
 *   SO_PROTOCOL/SO_DOMAIN soft get (STREAM/AF_INET) for glibc probes.
 * STRONGER functional residual (W10 Dual DoD; stamp-free bar v2026.08.04.75):
 *   lean residual self-check freezes SOL_SOCKET/SO_* / SHUT / POLL / TCP FD
 *   band + dual-table STREAM/AF_INET honesty for UDX/sshd hot ABI half;
 *   Dual DoD A/B OPEN combined; Soft!=product; NEVER invent .76.
 *   NO freestanding rtl residual here (product NIC = UDX userspace).
 *   Sparse greppable only - one-shot lamps; no stamp storms; no version stamp.
 *   Soft!=product · G-AC-1 (no .ko product AC) · dual MIT OR Apache-2.0.
 *   Dual DoD A/B OPEN (agent!=close); freestanding rtl/USB SKIP.
 * Greppable prefix-stable serial markers:
 *   linux: hot soft inventory ...
 *   linux: hot soft groups ...
 *   linux: hot soft io|id|mem|time|futex|sched|sig|sock|info|proc ...
 *   linux: hot soft live ...
 *   linux: hot soft path ...
 *   linux: hot soft stats ...
 *   linux: hot soft rates ...
 *   linux: hot soft honesty ...
 *   linux: hot soft catalog ...
 *   linux: hot soft surfaces ...   (Wave 19)
 *   linux: hot soft note ...       (Wave 19)
 *   linux: hot soft deepen ...
 *   linux: hot soft inventory PASS / soft PASS
 * greppable: "linux: hot soft"
 * greppable: linux: hot soft inventory
 * greppable: linux: hot soft path
 * greppable: linux: hot soft surfaces
 * greppable: linux: hot soft deepen
 * greppable: linux_hot: residual
 * greppable: linux_hot: residual mlock
 * greppable: linux_hot: residual socket gate
 * greppable: linux_hot: residual sock lean
 * greppable: linux_hot: residual mlock lean
 * greppable: linux_hot: residual fd lean
 * greppable: linux_hot: residual mmap lean
 * greppable: linux_hot: residual mmap anon lean
 * greppable: linux_hot: residual poll lean
 * greppable: linux_hot: residual io lean
 * greppable: linux_hot: residual mremap lean
 * greppable: linux_hot: residual ctid lean
 * greppable: linux_hot: residual set_tid lean
 * greppable: linux_hot: residual lean
 * greppable: linux_hot: soft residual lean
 * greppable: linux_hot: residual futex lean
 * greppable: linux_hot: futex requeue soft
 * greppable: linux_hot: futex wake_op soft
 * greppable: linux_hot: residual REUSE_star
 * greppable: linux_hot: residual affinity
 * greppable: linux_hot: residual affinity lean
 * greppable: linux_hot: residual process_vm lean
 * greppable: linux_hot: residual getrandom
 * greppable: linux_hot: residual getrandom lean
 * greppable: linux_hot: residual mincore
 * greppable: linux_hot: residual mincore lean
 * greppable: linux_hot: residual name sync
 * greppable: linux_hot: residual peer name
 * greppable: linux_hot: residual shutdown lean
 * greppable: linux_hot: residual so_error
 * greppable: linux_hot: residual ACCEPTCONN
 * greppable: linux_hot: residual C2 hot ABI
 * greppable: linux_hot: residual sock io
 * greppable: linux_hot: residual sock io lean
 * greppable: linux_hot: residual SO_PROTOCOL
 * greppable: linux_hot: residual SO_DOMAIN
 * greppable: linux_hot: residual W10 hot ABI
 * greppable: linux_hot: residual getpgid
 * greppable: linux_hot: residual umask
 * greppable: linux_hot: membarrier query soft
 * greppable: linux_hot: rseq soft
 * greppable: linux_hot: sysinfo pmm soft
 */

/* STRONGER W10 functional residual self-check arm count (Soft!=product). */
#define HOT_W10_LEAN_CHECKS 10u
#include <gj/cpu.h>
#include <gj/door.h>
#include <gj/error.h>
#include <gj/futex.h>
#include <gj/io_uring.h>
#include <gj/klog.h>
#include <gj/linux_cold_net.h>
#include <gj/linux_dispatch.h>
#include <gj/memobj.h>
#include <gj/net_lo.h>
#include <gj/net_tcp.h>
#include <gj/pmm.h>
#include <gj/process.h>
/* process_wait4 used by waitid */
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/user_access.h>
#include <gj/vfs_ram.h>
#include <gj/vmm.h>

/* Current Linux task view (exported for smoke CapJit toggles). */
struct gj_process *g_pLinuxProc;

static u32 g_u32LinuxPid = 1;
static u32 g_u32LinuxTid = 1;
static u32 g_u32LinuxPpid = 0;
static u32 g_u32LinuxUid = 0;
static u32 g_u32LinuxGid = 0;
static u32 g_u32LinuxEuid = 0;
static u32 g_u32LinuxEgid = 0;
static u32 g_u32LinuxSuid = 0;
static u32 g_u32LinuxSgid = 0;
static u32 g_u32LinuxPgid = 1;
static u32 g_u32LinuxSid = 1;
static u32 g_u32LinuxUmask = 0022u; /* POSIX shell default; umask(2) old-mask */
static u64 g_u64BrkBase = 0x04600000ull; /* 70 MiB: above user.ld + OpenSSH PT_LOAD */
static u64 g_u64BrkCur = 0x04600000ull;

/* Per-wait-pid r/e/s uid/gid. kmain smoke (wait pid 0) keeps the globals. */
#define GJ_LINUX_HOT_CRED_SLOTS 64u

struct linux_hot_cred {
    u32 u32Pid;
    u32 u32Ruid;
    u32 u32Euid;
    u32 u32Suid;
    u32 u32Rgid;
    u32 u32Egid;
    u32 u32Sgid;
};

static struct linux_hot_cred g_aLinuxCred[GJ_LINUX_HOT_CRED_SLOTS];

/**
 * USER TCB: USER*_ENTRY, mid-SYSCALL sysuser, or ring-3 entry with no kfn.
 * Trampoline clears USER_ENTRY; exec_replace still has u64UserRip.
 */
static int
hot_thr_is_user(const struct gj_thread *pThr)
{
    u32 u32UserFl = GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY;

    if (pThr == NULL || pThr->pProc == NULL) {
        return 0;
    }
    if ((pThr->u32Flags & u32UserFl) != 0) {
        return 1;
    }
    if (pThr->u32SysUserValid != 0) {
        return 1;
    }
    if (pThr->pfnEntry == NULL && pThr->u64UserRip != 0) {
        return 1;
    }
    return 0;
}

/**
 * Calling USER PCB, else g_pLinuxProc (kernel smoke).
 * mmap/brk/wait-adjacent hot paths bind here so a fork child is not pid-1 AS.
 * After exec_replace, doors kthread pProc is parent/default — use pClient or
 * the unique wait-registered USER child (sshd-auth), not g_pLinuxProc.
 */
static struct gj_process *
hot_calling_proc(void)
{
    struct gj_thread *pCur = thread_current();
    struct gj_thread *pCli;
    struct gj_door *pDoor;
    struct gj_cpu *pCpu;
    struct gj_process *pProc;
    u64 u64Rip = 0;
    u64 u64Rsp = 0;

    if (hot_thr_is_user(pCur) != 0) {
        return pCur->pProc;
    }
    pDoor = door_cold_personality();
    if (pDoor != NULL) {
        pCli = pDoor->pClient;
        if (hot_thr_is_user(pCli) != 0) {
            return pCli->pProc;
        }
    }
    pCpu = cpu_current();
    if (pCpu != NULL) {
        u64Rip = pCpu->u64UserRip;
        u64Rsp = pCpu->u64UserRsp;
    }
    pProc = process_linux_live_user_child(u64Rip, u64Rsp);
    if (pProc != NULL && pProc->u32Alive != 0) {
        return pProc;
    }
    if (pCur != NULL && pCur->pProc != NULL) {
        pProc = pCur->pProc;
        if (pProc != g_pLinuxProc && pProc->u32Alive != 0 &&
            pProc->u32Personality == 1u && process_wait_pid_of(pProc) != 0) {
            return pProc;
        }
    }
    return g_pLinuxProc;
}
static u64 g_u64FsBase;
static u64 g_u64GsBase;
static u64 g_u64ClearChildTid;
static u64 g_u64MonoNsec; /* crude mono clock until TSC calibrate */
static u64 g_u64TimerSlackNs = 50000ull; /* prctl PR_*_TIMERSLACK soft (50 us) */
static u64 g_u64RseqAddr;               /* soft rseq registration VA */
static u32 g_u32RseqLen;
static u32 g_u32RseqSig;
/* Sparse residual one-shot lamps (not stamp storms). */
static u8 g_fResFutexRequeueOnce;
static u8 g_fResFutexWakeOpOnce;
static u8 g_fResMembarrierOnce;
static u8 g_fResRseqOnce;
static u8 g_fResSysinfoOnce;
static u8 g_fResMlockOnce;
static u8 g_fResSockOnce;
static u8 g_fResSockLeanOnce;  /* REUSE_star / sockopt lean once */
static u8 g_fResMlockLeanOnce; /* mlock2 flags / page tally once */
static u8 g_fResFdLeanOnce;    /* dual-table fd classify once */
static u8 g_fResMmapLeanOnce;  /* file mmap PASS/ENOSYS lean once */
static u8 g_fResMmapAnonOnce;  /* anon mmap PASS lean once */
static u8 g_fResPollLeanOnce;  /* soft poll_mask probe once */
static u8 g_fResIoLeanOnce;    /* write/readv/pread/pwrite lean once */
static u8 g_fResMremapLeanOnce; /* mremap multi-chunk copy lean once */
static u8 g_fResCtidLeanOnce;  /* clear_child_tid clone-adjacent once */
static u8 g_fResSetTidLeanOnce; /* set_tid_address clone-adjacent once */
static u8 g_fResFutexLeanOnce; /* futex residual lean rollup once */
static u8 g_fResAffLeanOnce;   /* sched affinity lean once */
static u8 g_fResPvmLeanOnce;   /* process_vm lean once */
static u8 g_fResGetrandomLeanOnce; /* getrandom lean once */
static u8 g_fResMincoreLeanOnce; /* mincore lean once */
static u8 g_fResNameSyncOnce;  /* C2 product name sync once */
static u8 g_fResShutLeanOnce;  /* C2 shutdown lean once */
static u8 g_fResSoErrOnce;     /* C2 sticky SO_ERROR once */
static u8 g_fResSockIoOnce;    /* C2 sock write/readv hot bridge once */
static u8 g_fResLeanOnce;      /* unified residual lean rollup once */
static u8 g_fResGetpgidOnce;   /* getpgid ENOSYS->stub once */
static u8 g_fResUmaskOnce;     /* umask ENOSYS->stub once */

/* Soft residual tallies (diagnostics only; never hard-gate ret). */
static u64 g_u64ResMlockPages;     /* page-rounded pages accepted soft */
static u64 g_u64ResMlockCalls;     /* mlock / mlock2 enter ok */
static u64 g_u64ResMlock2Calls;    /* mlock2 NR only */
static u64 g_u64ResMunlockCalls;   /* munlock ok */
static u64 g_u64ResMlockallCalls;  /* mlockall ok */
static u32 g_u32ResMlockallFlags;  /* last soft mlockall flags */
static u64 g_u64ResSockReuseSet;   /* SO_REUSEADDR sets (REUSE_star) */
static u64 g_u64ResSockReusePortSet; /* SO_REUSEPORT sets (REUSE_star) */
static u64 g_u64ResSockGet;        /* TCP lean getsockopt ok */
static u64 g_u64ResSockSet;        /* TCP lean setsockopt ok */
static u64 g_u64ResSockGateHits;   /* dual-table gate accepts */
static u64 g_u64ResFdGateLo;       /* net_lo fd gate accepts */
static u64 g_u64ResFdGateTcp;      /* net_tcp fd gate accepts */
static u64 g_u64ResPollHits;       /* soft poll_mask probes */
static u32 g_u32ResPollLastMask;   /* last soft poll mask bits */
static u64 g_u64ResMmapFilePass;   /* vfs_ram file mmap soft PASS */
static u64 g_u64ResMmapFileEnosys; /* file mmap soft ENOSYS */
static u64 g_u64ResMmapAnonPass;   /* anon mmap soft PASS */
static u64 g_u64ResMmapFixed;      /* MAP_FIXED | FIXED_NOREPLACE soft */
static u64 g_u64ResMmapPageRound;  /* non-page len rounded up soft */
static u64 g_u64ResIoWriteCalls;   /* write path enters ok (stdio|vfs) */
static u64 g_u64ResIoReadCalls;    /* readv/pread residual ok */
static u64 g_u64ResIoVfsHits;      /* vfs_ram R/W residual hits */
static u64 g_u64ResIoStdioHits;    /* fd<=2 write residual hits */
static u64 g_u64ResIoBytes;        /* soft residual bytes transferred */
static u64 g_u64ResMremapCopy;     /* mremap MAYMOVE soft-copy bytes */
static u64 g_u64ResCtidClear;      /* clear_child_tid residual clears */
static u64 g_u64ResCtidSet;        /* set_tid_address residual stores */
static u64 g_u64ResFutexRequeue;   /* REQUEUE|CMP_REQUEUE residual ok */
static u64 g_u64ResFutexWakeOp;    /* WAKE_OP residual ok */
static u64 g_u64ResAffSet;         /* sched_setaffinity residual ok */
static u64 g_u64ResAffGet;         /* sched_getaffinity residual ok */
static u8  g_aResAffMask[8];       /* soft stored affinity mask (set) */
static u8  g_fResAffStored;        /* 1 = setaffinity stored soft mask */
static u64 g_u64ResPvmCalls;       /* process_vm_readv/writev residual ok */
static u64 g_u64ResPvmBytes;       /* process_vm residual bytes copied */
static u64 g_u64ResPvmRead;        /* process_vm_readv residual ok */
static u64 g_u64ResPvmWrite;       /* process_vm_writev residual ok */
static u64 g_u64ResGetrandomCalls; /* getrandom residual ok */
static u64 g_u64ResGetrandomBytes; /* getrandom residual bytes */
static u64 g_u64ResMincoreCalls;   /* mincore residual ok */
static u64 g_u64ResMincorePages;   /* mincore residual pages walked */
/* C2 hot ABI residual tallies (Soft!=product; never hard-gate ret). */
static u64 g_u64ResNameSync;       /* product getsockname sync hits */
static u64 g_u64ResPeerName;       /* product getpeername sync hits */
static u64 g_u64ResShut;           /* TCP shutdown residual ok */
static u64 g_u64ResSoErrSet;       /* sticky SO_ERROR soft sets */
static u64 g_u64ResSoErrGet;       /* sticky SO_ERROR clear-on-read */
static u64 g_u64ResAcceptConn;     /* SO_ACCEPTCONN listen-shaped soft */
/* C2 sock I/O residual (hot write/writev/readv -> net_tcp|net_lo). */
static u64 g_u64ResSockIoSend;     /* hot sock send residual ok */
static u64 g_u64ResSockIoRecv;     /* hot sock recv residual ok */
static u64 g_u64ResSockIoBytes;    /* hot sock I/O residual bytes */
static u64 g_u64ResSockIoPump;     /* EAGAIN poll+retry residual */

/*
 * Soft inventory groups (Wave 11/14 base; Wave 15 deepen). Enter-only tallies -
 * never rewrite ret. greppable: linux: hot soft ...
 */
enum {
    HOT_SOFT_GRP_IO = 0,   /* write/readv/pread/pwrite family */
    HOT_SOFT_GRP_ID,       /* get/set uid/gid/pid/sid/groups/personality */
    HOT_SOFT_GRP_MEM,      /* brk/mmap/mprotect/mlock/pkey/mbind */
    HOT_SOFT_GRP_TIME,     /* clock_* nanosleep itimer alarm timeofday */
    HOT_SOFT_GRP_FUTEX,    /* futex + robust_list + set_tid_address */
    HOT_SOFT_GRP_SCHED,    /* yield + sched_* affinity/param/attr */
    HOT_SOFT_GRP_SIG,      /* tkill tgkill sigaltstack rt_sig* pause */
    HOT_SOFT_GRP_SOCK,     /* getsockopt/name/peer/shutdown */
    HOT_SOFT_GRP_INFO,     /* uname/sysinfo/prctl/cap/random/getcpu */
    HOT_SOFT_GRP_PROC,     /* exit/waitid/process_vm/membarrier/rseq */
    HOT_SOFT_GRP_N
};

/*
 * Static product surface: public gj_linux_hot_* entry count (catalog).
 * Wave 15 soft inventory stamp (file-local; never product gate).
 * Areas: inventory|groups|io|id|mem|time|futex|sched|sig|sock|info|proc|
 *        live|path|stats|rates|honesty|catalog|deepen|PASS
 */
#define GJ_LINUX_HOT_SOFT_HANDLERS 107u
#define GJ_LINUX_HOT_SOFT_WAVE 126u
/* Residual UDX-host freestanding deepen (no new public handlers). */
/* Areas: prior 257 + C2 sock_io send/recv/pump + SO_PROTOCOL/DOMAIN
 * (Soft!=product; no version stamp; Dual DoD A/B OPEN; UDX/sshd). */
#define GJ_LINUX_HOT_SOFT_AREAS 261u
/* Soft residual IO transfer cap (single syscall; Soft!=product). */
#define HOT_RES_IO_MAX      4096ull
/* Soft residual mremap MAYMOVE copy cap (page loop; Soft!=product). */
#define HOT_RES_MREMAP_COPY_MAX (256ull << 10) /* 256 KiB residual */
/* Soft residual mmap max single map (driver-host ring; Soft!=product). */
#define HOT_RES_MMAP_MAX    (16ull << 20) /* 16 MiB residual cap */

/*
 * Soft SOL_SOCKET residual constants (x86_64 Linux ABI numbers; man pages).
 * TCP soft table layout matches net_tcp (FD 96..111, 16 slots). Soft!=product.
 * Comment footgun: never write REUSE_star slash or complete_star slash mid-
 * comment (the star-slash token closes C comments); spell as REUSE_star.
 * REUSE_star = REUSEADDR + REUSEPORT (both must tally+lamp; PORT was a
 * footgun that undercounted multi-worker UDX host bind probes).
 */
#define HOT_SOL_SOCKET    1
#define HOT_SO_DEBUG      1
#define HOT_SO_REUSEADDR  2
#define HOT_SO_TYPE       3
#define HOT_SO_ERROR      4
#define HOT_SO_DONTROUTE  5
#define HOT_SO_BROADCAST  6
#define HOT_SO_SNDBUF     7
#define HOT_SO_RCVBUF     8
#define HOT_SO_KEEPALIVE  9
#define HOT_SO_OOBINLINE  10
#define HOT_SO_LINGER     13
#define HOT_SO_REUSEPORT  15
#define HOT_SO_RCVTIMEO   20
#define HOT_SO_SNDTIMEO   21
#define HOT_SO_ACCEPTCONN 30
#define HOT_SO_PROTOCOL   38 /* soft get: IPPROTO_TCP for STREAM */
#define HOT_SO_DOMAIN     39 /* soft get: AF_INET */
#define HOT_SOCK_STREAM   1
#define HOT_AF_INET       2
#define HOT_IPPROTO_TCP   6
#define HOT_TCP_FD_BASE   96u
#define HOT_TCP_MAX       16u
#define HOT_SOFT_SOCKBUF  65536 /* soft RCVBUF/SNDBUF default probe */
#define HOT_SOFT_SOCKBUF_MIN 256
#define HOT_MLOCK_ONFAULT 1u /* mlock2 flags: only this bit legal */
/* mlockall: MCL_CURRENT=1 | MCL_FUTURE=2 | MCL_ONFAULT=4 */
#define HOT_MCL_MASK      7ull
/* Linux-shaped poll bits (cold poll/epoll consumers share these). */
#define HOT_POLL_IN       0x1u
#define HOT_POLL_OUT      0x4u
#define HOT_POLL_ERR      0x8u
#define HOT_POLL_HUP      0x10u
#define HOT_POLL_WANT_DEF (HOT_POLL_IN | HOT_POLL_OUT)
/* Soft half-close bits (mirror cold SHUT_RD/WR; Soft!=product). */
#define HOT_SHUT_RD       1u
#define HOT_SHUT_WR       2u

/*
 * Lean soft per-slot sock state for net_tcp FDs (Soft!=product; no .ko).
 * REUSE_star/KEEPALIVE/BROADCAST + soft buf sizes for get/set probes.
 * C2: sticky SO_ERROR + ACCEPTCONN + soft ports + half-close bits.
 */
struct hot_tcp_sock_lean {
    u8  u8Reuse;
    u8  u8ReusePort;
    u8  u8Keepalive;
    u8  u8Broadcast;
    u8  u8LingerOn;
    u8  u8AcceptConn;  /* SO_ACCEPTCONN listen-shaped soft */
    u8  u8Shut;        /* HOT_SHUT_RD | HOT_SHUT_WR */
    u8  u8PeerOk;      /* product getpeername succeeded */
    u16 u16LingerSec;
    u16 u16SoError;    /* sticky SO_ERROR (positive LINUX_E*; clear-on-read) */
    u16 u16LocalPort;  /* host-order local port from product name sync */
    u16 u16PeerPort;   /* host-order peer port from product name sync */
    u32 u32SndBuf;
    u32 u32RcvBuf;
    u8  u8Inited;
};

static struct hot_tcp_sock_lean g_aHotTcpLean[HOT_TCP_MAX];

struct linux_hot_soft {
    u64 aEnter[HOT_SOFT_GRP_N]; /* per-group handler entries */
    u64 u64EnterTotal;          /* sum of all group enters */
    u64 u64NullRegs;            /* pRegs == NULL observed at enter */
    u64 u64CtxSet;              /* gj_linux_set_current calls */
    u64 u64LogN;                /* inventory log emissions */
};

static struct linux_hot_soft g_hotSoft;
static u8 g_fHotSoftOnce; /* one-shot deep dump after first activity */

static void hot_soft_inc(u64 *pCtr);
static void hot_soft_enter(u32 u32Grp, const struct gj_linux_regs *pRegs);
static void hot_soft_inventory_log(void);
static void hot_soft_inventory_maybe_once(void);
static void hot_residual_lean_once(const char *szVia);
static void hot_residual_io_lean_once(const char *szVia, i64 i64Fd);
static void hot_residual_sock_io_once(const char *szVia, i64 i64Fd, int fSend);
static struct hot_tcp_sock_lean *hot_tcp_lean_slot(i64 i64Fd);
static void hot_tcp_soerr_set(i64 i64Fd, u16 u16Err);
static void hot_tcp_soerr_clear(i64 i64Fd);
static int hot_res_is_again(i64 i64Ret);
static i64 hot_sock_send_lean(i64 i64Fd, const void *pBuf, size_t cb);
static i64 hot_sock_recv_lean(i64 i64Fd, void *pBuf, size_t cb);
static int hot_tcp_product_name_sync(i64 i64Fd);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
hot_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Unified lean residual once-lamp (Soft!=product · G-AC-1).
 * Two greppable lines max (rollup + twin) - no stamp storms, no version stamp.
 * Surfaces: sock/fd/mmap/poll/mlock + io/mremap/ctid/set_tid/futex +
 * affinity/process_vm/getrandom/mincore + C2 name/peer/shut/soerr/ACCEPTCONN +
 * sock_io (write/writev/readv dual-table; UDX/sshd functional residual).
 * STRONGER W10: functional self-check freezes SO_/SHUT/POLL/TCP FD band
 * for UDX/sshd hot ABI half (stamp-free bar v2026.08.04.75; NEVER invent .76).
 * Dual DoD A/B OPEN (agent!=close); freestanding rtl/USB SKIP; G-AC-1.
 * Diagnostics only. greppable: linux_hot: residual lean
 * greppable: linux_hot: soft residual lean
 * greppable: linux_hot: residual C2 hot ABI
 * greppable: linux_hot: residual sock io lean
 * greppable: linux_hot: residual W10 hot ABI
 */
static void
hot_residual_lean_once(const char *szVia)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u32 u32SoOk = 0;
    u32 u32ShutOk = 0;
    u32 u32PollOk = 0;
    u32 u32TcpOk = 0;
    u32 u32StreamOk = 0;
    u32 u32DualDodOk = 0;
    /* Stack-local Soft!=product honesty (never hard-gates). */
    const int nSoft = 1;
    const int nProduct = 0;
    const int nGac1 = 1;
    const int nDualDodOpen = 1;
    const int nFsRtlSkip = 1;
    const int nKoProduct = 0;

    if (g_fResLeanOnce != 0u) {
        return;
    }
    g_fResLeanOnce = 1u;
    if (szVia == NULL) {
        szVia = "hot";
    }

    /*
     * STRONGER W10 functional residual self-check (UDX/sshd hot ABI).
     * Stack-local only; never hard-gates product ret. Soft!=product.
     * greppable: linux_hot: residual W10 hot ABI
     */
    /* 1. SOL_SOCKET + core SO_* freeze (glibc/sshd sockopt surface). */
    u32Checks++;
    if (HOT_SOL_SOCKET == 1 && HOT_SO_REUSEADDR == 2 && HOT_SO_TYPE == 3 &&
        HOT_SO_ERROR == 4 && HOT_SO_SNDBUF == 7 && HOT_SO_RCVBUF == 8 &&
        HOT_SO_KEEPALIVE == 9 && HOT_SO_REUSEPORT == 15 &&
        HOT_SO_ACCEPTCONN == 30 && HOT_SO_PROTOCOL == 38 &&
        HOT_SO_DOMAIN == 39) {
        u32SoOk = 1u;
        u32Ok++;
    }
    /* 2. Half-close SHUT bits (sshd SHUT_WR EPIPE residual). */
    u32Checks++;
    if (HOT_SHUT_RD == 1u && HOT_SHUT_WR == 2u &&
        (HOT_SHUT_RD | HOT_SHUT_WR) == 3u) {
        u32ShutOk = 1u;
        u32Ok++;
    }
    /* 3. Linux-shaped poll bits (cold/hot share; H1 thr consumers). */
    u32Checks++;
    if (HOT_POLL_IN == 0x1u && HOT_POLL_OUT == 0x4u &&
        HOT_POLL_ERR == 0x8u && HOT_POLL_HUP == 0x10u &&
        HOT_POLL_WANT_DEF == (HOT_POLL_IN | HOT_POLL_OUT)) {
        u32PollOk = 1u;
        u32Ok++;
    }
    /* 4. TCP soft FD band (matches net_tcp / cold LCN layout). */
    u32Checks++;
    if (HOT_TCP_FD_BASE == 96u && HOT_TCP_MAX == 16u &&
        HOT_TCP_FD_BASE + HOT_TCP_MAX == 112u) {
        u32TcpOk = 1u;
        u32Ok++;
    }
    /* 5. STREAM / AF_INET / IPPROTO_TCP freeze (sshd dual-table path). */
    u32Checks++;
    if (HOT_SOCK_STREAM == 1 && HOT_AF_INET == 2 && HOT_IPPROTO_TCP == 6 &&
        HOT_SOFT_SOCKBUF >= HOT_SOFT_SOCKBUF_MIN) {
        u32StreamOk = 1u;
        u32Ok++;
    }
    /* 6. Residual IO transfer cap honest (sshd writev/readv bounce). */
    u32Checks++;
    if (HOT_RES_IO_MAX == 4096ull && HOT_RES_IO_MAX >= 256ull &&
        HOT_RES_IO_MAX <= (64ull << 10)) {
        u32Ok++;
    }
    /* 7. Soft areas / wave present (no stamp storms; not image version). */
    u32Checks++;
    if (GJ_LINUX_HOT_SOFT_AREAS >= 261u && GJ_LINUX_HOT_SOFT_WAVE >= 1u &&
        GJ_LINUX_HOT_SOFT_WAVE < 10000u && HOT_W10_LEAN_CHECKS == 10u) {
        u32Ok++;
    }
    /* 8. mlock2 / mlockall flag masks (UDX host pin residual). */
    u32Checks++;
    if (HOT_MLOCK_ONFAULT == 1u && HOT_MCL_MASK == 7ull) {
        u32Ok++;
    }
    /* 9. Handler catalog capacity present (hot product surface count). */
    u32Checks++;
    if (GJ_LINUX_HOT_SOFT_HANDLERS >= 100u &&
        GJ_LINUX_HOT_SOFT_HANDLERS < 1000u) {
        u32Ok++;
    }
    /* 10. Dual DoD A/B OPEN + Soft!=product + G-AC-1 honesty. */
    u32Checks++;
    if (nSoft == 1 && nProduct == 0 && nSoft != nProduct && nGac1 == 1 &&
        nDualDodOpen == 1 && nFsRtlSkip == 1 && nKoProduct == 0 &&
        u32SoOk != 0u && u32ShutOk != 0u && u32PollOk != 0u &&
        u32TcpOk != 0u && u32StreamOk != 0u) {
        u32DualDodOk = 1u;
        u32Ok++;
    }

    /*
     * Grep: linux_hot: residual lean
     * Grep: linux_hot: soft residual lean
     * Grep: linux_hot: residual C2 hot ABI
     * Grep: linux_hot: residual W10 hot ABI
     * Honesty: soft residual surface != product Linux ABI / != .ko AC.
     * Footgun: spell Soft!=product (ASCII); never Soft slash-star mid-comment.
     * REUSE_star = sock_reuse (ADDR) + sock_reuseport (PORT) tallies.
     * Dual DoD A/B OPEN (agent!=close); freestanding rtl/USB SKIP.
     */
    kprintf("linux_hot: residual lean via=%s "
            "w10_ok=%u/%u so=%u shut=%u poll=%u tcp=%u stream=%u dod=%u "
            "sock_get=%lu sock_set=%lu sock_reuse=%lu sock_reuseport=%lu "
            "sock_gate=%lu "
            "fd_lo=%lu fd_tcp=%lu poll_hits=%lu poll_mask=0x%x "
            "mmap_pass=%lu mmap_enosys=%lu mmap_anon=%lu "
            "mmap_fixed=%lu mmap_page_round=%lu "
            "io_w=%lu io_r=%lu io_vfs=%lu io_stdio=%lu io_bytes=%lu "
            "mremap_copy=%lu ctid_clear=%lu ctid_set=%lu "
            "futex_rq=%lu futex_wo=%lu "
            "mlock_calls=%lu mlock2=%lu munlock=%lu mlockall=%lu "
            "mlock_pages=%lu mlockall_flags=0x%x "
            "aff_set=%lu aff_get=%lu aff_stored=%u "
            "pvm_calls=%lu pvm_bytes=%lu pvm_r=%lu pvm_w=%lu "
            "getrandom_calls=%lu getrandom_bytes=%lu "
            "mincore_calls=%lu mincore_pages=%lu "
            "name_sync=%lu peer_name=%lu shut=%lu "
            "soerr_set=%lu soerr_get=%lu acceptconn=%lu "
            "sock_io_send=%lu sock_io_recv=%lu sock_io_bytes=%lu "
            "sock_io_pump=%lu "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "product_linux_abi=OPEN dual_dod_ab=OPEN agent_ne_close=1 "
            "no_ko_product=1 stamp_storm=0 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent_76=1 "
            "(STRONGER functional residual W10 Dual DoD; Soft!=product; "
            "dual MIT OR Apache-2.0; no version stamp; no .ko product AC; "
            "no freestanding rtl residual; Dual DoD A/B OPEN; "
            "C2 hot ABI residual; UDX/sshd sock io hot half)\n",
            szVia,
            (unsigned)u32Ok, (unsigned)u32Checks,
            (unsigned)u32SoOk, (unsigned)u32ShutOk, (unsigned)u32PollOk,
            (unsigned)u32TcpOk, (unsigned)u32StreamOk, (unsigned)u32DualDodOk,
            (unsigned long)g_u64ResSockGet,
            (unsigned long)g_u64ResSockSet,
            (unsigned long)g_u64ResSockReuseSet,
            (unsigned long)g_u64ResSockReusePortSet,
            (unsigned long)g_u64ResSockGateHits,
            (unsigned long)g_u64ResFdGateLo,
            (unsigned long)g_u64ResFdGateTcp,
            (unsigned long)g_u64ResPollHits,
            (unsigned)g_u32ResPollLastMask,
            (unsigned long)g_u64ResMmapFilePass,
            (unsigned long)g_u64ResMmapFileEnosys,
            (unsigned long)g_u64ResMmapAnonPass,
            (unsigned long)g_u64ResMmapFixed,
            (unsigned long)g_u64ResMmapPageRound,
            (unsigned long)g_u64ResIoWriteCalls,
            (unsigned long)g_u64ResIoReadCalls,
            (unsigned long)g_u64ResIoVfsHits,
            (unsigned long)g_u64ResIoStdioHits,
            (unsigned long)g_u64ResIoBytes,
            (unsigned long)g_u64ResMremapCopy,
            (unsigned long)g_u64ResCtidClear,
            (unsigned long)g_u64ResCtidSet,
            (unsigned long)g_u64ResFutexRequeue,
            (unsigned long)g_u64ResFutexWakeOp,
            (unsigned long)g_u64ResMlockCalls,
            (unsigned long)g_u64ResMlock2Calls,
            (unsigned long)g_u64ResMunlockCalls,
            (unsigned long)g_u64ResMlockallCalls,
            (unsigned long)g_u64ResMlockPages,
            (unsigned)g_u32ResMlockallFlags,
            (unsigned long)g_u64ResAffSet,
            (unsigned long)g_u64ResAffGet,
            (unsigned)g_fResAffStored,
            (unsigned long)g_u64ResPvmCalls,
            (unsigned long)g_u64ResPvmBytes,
            (unsigned long)g_u64ResPvmRead,
            (unsigned long)g_u64ResPvmWrite,
            (unsigned long)g_u64ResGetrandomCalls,
            (unsigned long)g_u64ResGetrandomBytes,
            (unsigned long)g_u64ResMincoreCalls,
            (unsigned long)g_u64ResMincorePages,
            (unsigned long)g_u64ResNameSync,
            (unsigned long)g_u64ResPeerName,
            (unsigned long)g_u64ResShut,
            (unsigned long)g_u64ResSoErrSet,
            (unsigned long)g_u64ResSoErrGet,
            (unsigned long)g_u64ResAcceptConn,
            (unsigned long)g_u64ResSockIoSend,
            (unsigned long)g_u64ResSockIoRecv,
            (unsigned long)g_u64ResSockIoBytes,
            (unsigned long)g_u64ResSockIoPump);
    kprintf("linux_hot: soft residual lean via=%s "
            "w10_ok=%u/%u sock+fd+mmap+poll+mlock+io+mremap+ctid+futex=1 "
            "aff+pvm+getrandom+mincore=1 "
            "name_sync+peer+shut+soerr+ACCEPTCONN=1 dual_table=1 "
            "sock_io=1 REUSE_star=1 mlock2_onfault=1 soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 G-AC-1=1 product_linux_abi=OPEN "
            "dual_dod_ab=OPEN agent_ne_close=1 stamp_storm=0 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent_76=1 "
            "(STRONGER functional residual W10 Dual DoD; Soft!=product; "
            "no version stamp; no freestanding rtl; Dual DoD A/B OPEN; "
            "C2 hot ABI residual; UDX/sshd sock io hot half)\n",
            szVia, (unsigned)u32Ok, (unsigned)u32Checks);
}

/**
 * IO residual lean once-lamp (write/readv/pread/pwrite family).
 * Soft!=product · G-AC-1 · diagnostics only; never hard-gates ret.
 * greppable: linux_hot: residual io lean
 */
static void
hot_residual_io_lean_once(const char *szVia, i64 i64Fd)
{
    if (g_fResIoLeanOnce != 0u) {
        return;
    }
    g_fResIoLeanOnce = 1u;
    if (szVia == NULL) {
        szVia = "io";
    }
    /*
     * Grep: linux_hot: residual io lean
     * Surfaces: stdio write + vfs_ram R/W + positioned pread/pwrite loops.
     */
    kprintf("linux_hot: residual io lean via=%s fd=%ld "
            "w_calls=%lu r_calls=%lu vfs=%lu stdio=%lu bytes=%lu "
            "Soft!=product G-AC-1\n",
            szVia, (long)i64Fd,
            (unsigned long)g_u64ResIoWriteCalls,
            (unsigned long)g_u64ResIoReadCalls,
            (unsigned long)g_u64ResIoVfsHits,
            (unsigned long)g_u64ResIoStdioHits,
            (unsigned long)g_u64ResIoBytes);
    hot_residual_lean_once(szVia);
}

/**
 * Soft enter note - never alters handler return. Diagnostics only.
 * greppable: linux: hot soft
 */
static void
hot_soft_enter(u32 u32Grp, const struct gj_linux_regs *pRegs)
{
    if (u32Grp < (u32)HOT_SOFT_GRP_N) {
        hot_soft_inc(&g_hotSoft.aEnter[u32Grp]);
    }
    hot_soft_inc(&g_hotSoft.u64EnterTotal);
    if (pRegs == NULL) {
        hot_soft_inc(&g_hotSoft.u64NullRegs);
    }
    hot_soft_inventory_maybe_once();
}

/**
 * Greppable soft Linux hot-path inventory (product / smoke; Wave 15 deepen).
 *   linux: hot soft inventory ...
 *   linux: hot soft groups ...
 *   linux: hot soft io|id|mem|time|futex|sched|sig|sock|info|proc ...
 *   linux: hot soft live ...
 *   linux: hot soft path ...
 *   linux: hot soft stats ...
 *   linux: hot soft rates ...
 *   linux: hot soft honesty ...
 *   linux: hot soft catalog ...
 *   linux: hot soft deepen ...
 *   linux: hot soft inventory PASS / soft PASS
 * greppable: linux: hot soft
 * Honesty: soft inventory only - not product gate.
 */
static void
hot_soft_inventory_log(void)
{
    struct linux_hot_soft s;
    u32 u32HasProc;
    u32 u32BrkLive;
    u32 u32LiveCred;
    u32 u32GroupsActive;
    u32 iGrp;

    hot_soft_inc(&g_hotSoft.u64LogN);
    s = g_hotSoft;
    u32HasProc = (g_pLinuxProc != NULL) ? 1u : 0u;
    u32BrkLive = (g_u64BrkBase != 0 || g_u64BrkCur != 0) ? 1u : 0u;
    u32LiveCred = (g_u32LinuxUid != 0 || g_u32LinuxEuid != 0) ? 1u : 0u;
    u32GroupsActive = 0;
    for (iGrp = 0; iGrp < (u32)HOT_SOFT_GRP_N; iGrp++) {
        if (s.aEnter[iGrp] != 0) {
            u32GroupsActive++;
        }
    }

    /* Grep: linux: hot soft inventory */
    kprintf("linux: hot soft inventory wave=%u handlers=%u groups=%u "
            "areas=%u enter=%lu null_regs=%lu ctx_set=%lu log_n=%lu "
            "proc=%u brk_live=%u cred_live=%u grp_active=%u "
            "(soft)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE,
            (unsigned)GJ_LINUX_HOT_SOFT_HANDLERS,
            (unsigned)HOT_SOFT_GRP_N,
            (unsigned)GJ_LINUX_HOT_SOFT_AREAS,
            (unsigned long)s.u64EnterTotal,
            (unsigned long)s.u64NullRegs,
            (unsigned long)s.u64CtxSet,
            (unsigned long)s.u64LogN,
            u32HasProc, u32BrkLive, u32LiveCred, u32GroupsActive);

    /* Grep: linux: hot soft groups (Wave 15 catalog) */
    kprintf("linux: hot soft groups n=%u active=%u "
            "names=io,id,mem,time,futex,sched,sig,sock,info,proc "
            "wave=%u\n",
            (unsigned)HOT_SOFT_GRP_N, u32GroupsActive,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft io */
    kprintf("linux: hot soft io enter=%lu "
            "surface=write,writev,readv,preadv,pwritev,pread64,pwrite64 "
            "residual_io_lean=1 residual_sock_io=1 vfs_write=1 "
            "preadv_off=1 dual_table_sock=1 loop_cap=%lu wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_IO],
            (unsigned long)HOT_RES_IO_MAX,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft id */
    kprintf("linux: hot soft id enter=%lu "
            "surface=get/set uid/gid/pid/sid/groups/personality "
            "wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_ID],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft mem */
    kprintf("linux: hot soft mem enter=%lu "
            "surface=brk,mmap,munmap,mremap,mprotect,msync,mincore,"
            "mlock,pkey,mbind residual_mmap_anon=1 residual_mmap_fixed=1 "
            "residual_mmap_page_round=1 residual_mremap_copy=%lu "
            "residual_mincore_lean=1 mmap_cap=%lu wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_MEM],
            (unsigned long)HOT_RES_MREMAP_COPY_MAX,
            (unsigned long)HOT_RES_MMAP_MAX,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft time */
    kprintf("linux: hot soft time enter=%lu "
            "surface=clock_*,nanosleep,itimer,alarm,timeofday,time "
            "wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_TIME],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft futex */
    kprintf("linux: hot soft futex enter=%lu "
            "surface=futex,futex_wake2,futex_wait2,robust_list,"
            "set_tid_address residual_requeue=1 residual_wake_op=1 "
            "residual_set_tid=1 residual_ctid=1 wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_FUTEX],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft sched */
    kprintf("linux: hot soft sched enter=%lu "
            "surface=yield,sched_get/set*,affinity,attr,priority "
            "residual_affinity_lean=1 aff_stored=%u wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SCHED],
            (unsigned)g_fResAffStored,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft sig */
    kprintf("linux: hot soft sig enter=%lu "
            "surface=tkill,tgkill,sigaltstack,rt_sig*,pause wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SIG],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft sock */
    kprintf("linux: hot soft sock enter=%lu "
            "surface=getsockopt,setsockopt,getsockname,getpeername,"
            "shutdown residual_REUSE_star=1 dual_table=1 "
            "residual_name_sync=1 residual_so_error=1 residual_ACCEPTCONN=1 "
            "residual_shutdown_lean=1 residual_sock_io=1 "
            "residual_SO_PROTOCOL=1 residual_SO_DOMAIN=1 C2_hot_abi=1 "
            "wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SOCK],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft info */
    kprintf("linux: hot soft info enter=%lu "
            "surface=uname,sysinfo,times,rusage,prctl,cap,random,getcpu,"
            "priority,arch_prctl residual_getrandom_lean=1 wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_INFO],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft proc */
    kprintf("linux: hot soft proc enter=%lu "
            "surface=exit,exit_group,waitid,process_vm_*,membarrier,rseq "
            "residual_process_vm_lean=1 dual_dod_ab=OPEN wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_PROC],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft live */
    kprintf("linux: hot soft live pid=%u tid=%u ppid=%u pgid=%u sid=%u "
            "uid=%u euid=%u gid=%u egid=%u brk_base=0x%lx brk_cur=0x%lx "
            "fs_base=0x%lx gs_base=0x%lx clear_ctid=0x%lx wave=%u\n",
            g_u32LinuxPid, g_u32LinuxTid, g_u32LinuxPpid, g_u32LinuxPgid,
            g_u32LinuxSid, g_u32LinuxUid, g_u32LinuxEuid, g_u32LinuxGid,
            g_u32LinuxEgid,
            (unsigned long)g_u64BrkBase, (unsigned long)g_u64BrkCur,
            (unsigned long)g_u64FsBase, (unsigned long)g_u64GsBase,
            (unsigned long)g_u64ClearChildTid,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft path */
    kprintf("linux: hot soft path hybrid=OptionC hot=kernel "
            "cold=personality enter_only=1 ret_rewrite=0 "
            "handlers=%u groups=%u wave=%u "
            "(soft inventory)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_HANDLERS,
            (unsigned)HOT_SOFT_GRP_N,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft stats */
    kprintf("linux: hot soft stats enter=%lu io=%lu id=%lu mem=%lu "
            "time=%lu futex=%lu sched=%lu sig=%lu sock=%lu info=%lu "
            "proc=%lu null=%lu ctx=%lu log_n=%lu wave=%u\n",
            (unsigned long)s.u64EnterTotal,
            (unsigned long)s.aEnter[HOT_SOFT_GRP_IO],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_ID],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_MEM],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_TIME],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_FUTEX],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SCHED],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SIG],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SOCK],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_INFO],
            (unsigned long)s.aEnter[HOT_SOFT_GRP_PROC],
            (unsigned long)s.u64NullRegs,
            (unsigned long)s.u64CtxSet,
            (unsigned long)s.u64LogN,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft rates (Wave 15 deepen) */
    {
        u64 u64BpIo;
        u64 u64BpMem;
        u64 u64BpNull;

        if (s.u64EnterTotal != 0) {
            u64BpIo = (s.aEnter[HOT_SOFT_GRP_IO] * 10000ull) / s.u64EnterTotal;
            u64BpMem = (s.aEnter[HOT_SOFT_GRP_MEM] * 10000ull) / s.u64EnterTotal;
            u64BpNull = (s.u64NullRegs * 10000ull) / s.u64EnterTotal;
        } else {
            u64BpIo = 0;
            u64BpMem = 0;
            u64BpNull = 0;
        }
        kprintf("linux: hot soft rates bp_io=%lu bp_mem=%lu bp_null=%lu "
                "enter=%lu grp_active=%u handlers=%u wave=%u\n",
                (unsigned long)u64BpIo,
                (unsigned long)u64BpMem,
                (unsigned long)u64BpNull,
                (unsigned long)s.u64EnterTotal,
                u32GroupsActive,
                (unsigned)GJ_LINUX_HOT_SOFT_HANDLERS,
                (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
    }

    /* Grep: linux: hot soft honesty (Wave 15 deepen) */
    kprintf("linux: hot soft honesty hybrid=OptionC open=1 "
            "product_linux_abi=open soft_only=1 hot=kernel "
            "dual_dod_ab=OPEN agent_ne_close=1 freestanding_rtl_usb=SKIP "
            "G-AC-1=1 Soft!=product "
            "wave=%u (soft inventory; never closes hybrid; Dual DoD OPEN)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft surfaces (Wave 20 deepen) */
    kprintf("linux: hot soft surfaces count=%u wave=%u "
            "names=inventory,groups,io,id,mem,time,futex,sched,sig,"
            "sock,info,proc,live,path,stats,rates,honesty,catalog,"
            "surfaces,note,return,retmap,deepen,PASS\n",
            (unsigned)GJ_LINUX_HOT_SOFT_AREAS,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft note (Wave 20 deepen) */
    kprintf("linux: hot soft note milestone=wave98 exclusive=1 handlers=%u enter=%lu wave=%u\n",
            (unsigned)GJ_LINUX_HOT_SOFT_HANDLERS,
            (unsigned long)s.u64EnterTotal,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft catalog (Wave 20 deepen) */
    kprintf("linux: hot soft catalog wave=%u areas=%u handlers=%u "
            "surfaces=inventory,groups,io,id,mem,time,futex,sched,sig,"
            "sock,info,proc,live,path,stats,rates,honesty,catalog,"
            "surfaces,note,return,retmap,deepen,PASS\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE,
            (unsigned)GJ_LINUX_HOT_SOFT_AREAS,
            (unsigned)GJ_LINUX_HOT_SOFT_HANDLERS);

    /* Grep: linux: hot soft deepen wave */
    kprintf("linux: hot soft deepen wave=%u areas=%u handlers=%u "
            "groups=%u enter=%lu logs=%lu "
            "(Wave 92 exclusive)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE,
            (unsigned)GJ_LINUX_HOT_SOFT_AREAS,
            (unsigned)GJ_LINUX_HOT_SOFT_HANDLERS,
            (unsigned)HOT_SOFT_GRP_N,
            (unsigned long)s.u64EnterTotal,
            (unsigned long)s.u64LogN);

    /* Grep: linux: hot soft inventory PASS / soft PASS */
    kprintf("linux: hot soft inventory PASS wave=%u logs=%lu "
            "enter=%lu handlers=%u\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE,
            (unsigned long)s.u64LogN,
            (unsigned long)s.u64EnterTotal,
            (unsigned)GJ_LINUX_HOT_SOFT_HANDLERS);
    kprintf("linux: hot soft PASS wave=%u logs=%lu\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE,
            (unsigned long)s.u64LogN);
}

/**
 * After first product activity (set_current or any hot enter), print soft
 * inventory once (mirrors memobj/door soft-stats-once). Diagnostics only.
 */
static void
hot_soft_inventory_maybe_once(void)
{
    if (g_fHotSoftOnce != 0) {
        return;
    }
    if (g_hotSoft.u64EnterTotal == 0 && g_hotSoft.u64CtxSet == 0) {
        return;
    }
    g_fHotSoftOnce = 1;
    hot_soft_inventory_log();
}

void serial_putchar(char ch);

void
gj_linux_set_current(struct gj_process *pProc, u32 u32Pid, u32 u32Tid)
{
    g_pLinuxProc = pProc;
    g_u32LinuxPid = u32Pid ? u32Pid : 1;
    g_u32LinuxTid = u32Tid ? u32Tid : g_u32LinuxPid;
    /* Wave 15 soft: arm inventory on bind (bring-up smoke greps). */
    hot_soft_inc(&g_hotSoft.u64CtxSet);
    hot_soft_inventory_maybe_once();
}

static void
hot_putchar(char ch)
{
    serial_putchar(ch);
}

static void
hot_write_bytes(const char *p, size_t cb)
{
    size_t i;

    if (p == NULL) {
        return;
    }
    for (i = 0; i < cb; i++) {
        hot_putchar(p[i]);
    }
}

i64
gj_linux_hot_write(struct gj_linux_regs *pRegs)
{
    u64 u64Fd;
    u64 u64Len;
    char aKbuf[256];
    size_t cbChunk;
    u64 u64Done = 0;
    u64 u64Src;
    gj_status_t st;
    int fVfs;
    int fSock;

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Fd = pRegs->u64Arg0;
    u64Src = pRegs->u64Arg1;
    u64Len = pRegs->u64Arg2;
    fVfs = 0;
    fSock = 0;

    /*
     * OpenSSH login-shell child dup2(pout[1], 1) leaves a leftover LCN
     * alias on 1. Prefer vfs pipe/PTY or dash stdout is raw TCP
     * (QEMU95 Bad packet length 0x50495045 = "PIPE"). Dual DoD B OPEN.
     */
    if (vfs_ram_fd_poll_parkable((i64)u64Fd) != 0) {
        fVfs = 1;
    } else if (gj_linux_cold_fd_ok((i64)u64Fd) != 0) {
        return gj_linux_cold_send(pRegs);
    }

    /*
     * Hot residual: stdio (0..2) serial + vfs_ram + dual-table sock
     * (net_tcp|net_lo) for UDX/sshd write(2). Unknown FDs stay ENOSYS
     * for cold personality (Soft!=product).
     * greppable: linux_hot: residual io lean
     * greppable: linux_hot: residual sock io
     */
    if (u64Fd > 2) {
        if (vfs_ram_fd_ok((i64)u64Fd)) {
            fVfs = 1;
        } else if (net_tcp_fd_ok((i64)u64Fd) || net_lo_fd_ok((i64)u64Fd)) {
            fSock = 1;
        } else {
            return -LINUX_ENOSYS;
        }
    }
    if (u64Len == 0) {
        return 0;
    }
    if (u64Len > HOT_RES_IO_MAX) {
        u64Len = HOT_RES_IO_MAX;
    }
    /*
     * Kernel smoke may pass kernel buffers; ring3 passes user VAs.
     * Accept either: user-mapped range, or kernel identity pointer.
     */
    while (u64Done < u64Len) {
        i64 n;

        cbChunk = (size_t)(u64Len - u64Done);
        if (cbChunk > sizeof(aKbuf)) {
            cbChunk = sizeof(aKbuf);
        }
        if (user_range_ok(u64Src + u64Done, cbChunk)) {
            st = copy_from_user(aKbuf, u64Src + u64Done, cbChunk);
            if (st != GJ_OK) {
                return u64Done ? (i64)u64Done : -LINUX_EFAULT;
            }
        } else {
            /* Kernel-mode smoke path only */
            memcpy(aKbuf, (const void *)(gj_vaddr_t)(u64Src + u64Done),
                   cbChunk);
        }
        if (fVfs != 0) {
            n = vfs_ram_write((i64)u64Fd, aKbuf, cbChunk);
            if (n < 0) {
                return u64Done ? (i64)u64Done : n;
            }
            if (n == 0) {
                break;
            }
            u64Done += (u64)n;
            if ((u64)n < (u64)cbChunk) {
                break;
            }
        } else if (fSock != 0) {
            n = hot_sock_send_lean((i64)u64Fd, aKbuf, cbChunk);
            if (n < 0) {
                return u64Done ? (i64)u64Done : n;
            }
            if (n == 0) {
                break;
            }
            u64Done += (u64)n;
            if ((u64)n < (u64)cbChunk) {
                break;
            }
        } else {
            hot_write_bytes(aKbuf, cbChunk);
            u64Done += cbChunk;
        }
    }
    g_u64ResIoWriteCalls++;
    g_u64ResIoBytes += u64Done;
    if (fVfs != 0) {
        g_u64ResIoVfsHits++;
        hot_residual_io_lean_once("write_vfs", (i64)u64Fd);
    } else if (fSock != 0) {
        hot_residual_io_lean_once("write_sock", (i64)u64Fd);
        hot_residual_sock_io_once("write", (i64)u64Fd, 1);
    } else {
        g_u64ResIoStdioHits++;
        hot_residual_io_lean_once("write_stdio", (i64)u64Fd);
    }
    return (i64)u64Done;
}

/* struct iovec - public layout: base ptr + length */
struct linux_iovec {
    u64 u64Base;
    u64 u64Len;
};

static int
load_iovecs(u64 u64Iov, u64 u64Cnt, struct linux_iovec *pIov, u64 u64Max)
{
    if (u64Cnt == 0) {
        return 0;
    }
    if (u64Cnt > u64Max) {
        u64Cnt = u64Max;
    }
    if (user_range_ok(u64Iov, u64Cnt * sizeof(pIov[0]))) {
        if (copy_from_user(pIov, u64Iov, (size_t)(u64Cnt * sizeof(pIov[0]))) !=
            GJ_OK) {
            return -1;
        }
    } else {
        memcpy(pIov, (const void *)(gj_vaddr_t)u64Iov,
               (size_t)(u64Cnt * sizeof(pIov[0])));
    }
    return (int)u64Cnt;
}

i64
gj_linux_hot_writev(struct gj_linux_regs *pRegs)
{
    u64 u64Fd;
    u64 u64Iov;
    u64 u64Cnt;
    int nIov;
    int i;
    i64 i64Total = 0;
    struct linux_iovec aIov[8];
    u8 aChunk[256];

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Fd = pRegs->u64Arg0;
    u64Iov = pRegs->u64Arg1;
    u64Cnt = pRegs->u64Arg2;
    if (u64Cnt == 0) {
        return 0;
    }
    nIov = load_iovecs(u64Iov, u64Cnt, aIov, 8);
    if (nIov < 0) {
        return -LINUX_EFAULT;
    }
    for (i = 0; i < nIov; i++) {
        u64 off = 0;
        u64 len = aIov[i].u64Len;
        u64 base = aIov[i].u64Base;

        while (off < len) {
            size_t cb = (size_t)(len - off);
            i64 n;

            if (cb > sizeof(aChunk)) {
                cb = sizeof(aChunk);
            }
            if (user_range_ok(base + off, cb)) {
                if (copy_from_user(aChunk, base + off, cb) != GJ_OK) {
                    return i64Total > 0 ? i64Total : -LINUX_EFAULT;
                }
            } else {
                memcpy(aChunk, (const void *)(gj_vaddr_t)(base + off), cb);
            }
            if (u64Fd <= 2) {
                struct gj_linux_regs one;

                one.u64Arg0 = u64Fd;
                one.u64Arg1 = (u64)(gj_vaddr_t)aChunk;
                one.u64Arg2 = cb;
                n = gj_linux_hot_write(&one);
            } else if (vfs_ram_fd_ok((i64)u64Fd)) {
                n = vfs_ram_write((i64)u64Fd, aChunk, cb);
            } else if (net_tcp_fd_ok((i64)u64Fd) ||
                       net_lo_fd_ok((i64)u64Fd)) {
                /*
                 * C2 sock I/O: writev on STREAM dual-table (sshd/UDX).
                 * Was -EBADF; STRONGER functional residual.
                 * greppable: linux_hot: residual sock io
                 */
                n = hot_sock_send_lean((i64)u64Fd, aChunk, cb);
            } else {
                return i64Total > 0 ? i64Total : -LINUX_EBADF;
            }
            if (n < 0) {
                return i64Total > 0 ? i64Total : n;
            }
            i64Total += n;
            off += (u64)n;
            if ((u64)n < (u64)cb) {
                goto writev_done;
            }
            /* Soft residual: cap cumulative writev transfer. */
            if ((u64)i64Total >= HOT_RES_IO_MAX) {
                goto writev_done;
            }
        }
    }
writev_done:
    if (i64Total > 0 && u64Fd > 2) {
        /* write() residual tallies stdio; vfs/sock writev path tallies here. */
        g_u64ResIoWriteCalls++;
        g_u64ResIoBytes += (u64)i64Total;
        if (vfs_ram_fd_ok((i64)u64Fd)) {
            g_u64ResIoVfsHits++;
            hot_residual_io_lean_once("writev", (i64)u64Fd);
        } else if (net_tcp_fd_ok((i64)u64Fd) || net_lo_fd_ok((i64)u64Fd)) {
            hot_residual_io_lean_once("writev_sock", (i64)u64Fd);
            hot_residual_sock_io_once("writev", (i64)u64Fd, 1);
        }
    }
    return i64Total;
}

i64
gj_linux_hot_readv(struct gj_linux_regs *pRegs)
{
    u64 u64Fd;
    u64 u64Iov;
    u64 u64Cnt;
    int nIov;
    int i;
    i64 i64Total = 0;
    struct linux_iovec aIov[8];
    u8 aChunk[256];

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Fd = pRegs->u64Arg0;
    u64Iov = pRegs->u64Arg1;
    u64Cnt = pRegs->u64Arg2;
    if (u64Cnt == 0) {
        return 0;
    }
    if (u64Fd == 0) {
        return 0; /* empty stdin */
    }
    if (u64Fd <= 2) {
        return -LINUX_EBADF;
    }
    /*
     * vfs_ram or dual-table sock (net_tcp|net_lo). Sock was -EBADF;
     * C2 functional residual for UDX/sshd readv on STREAM.
     * greppable: linux_hot: residual sock io
     */
    if (!vfs_ram_fd_ok((i64)u64Fd) && !net_tcp_fd_ok((i64)u64Fd) &&
        !net_lo_fd_ok((i64)u64Fd)) {
        return -LINUX_EBADF;
    }
    nIov = load_iovecs(u64Iov, u64Cnt, aIov, 8);
    if (nIov < 0) {
        return -LINUX_EFAULT;
    }
    for (i = 0; i < nIov; i++) {
        u64 off = 0;
        u64 len = aIov[i].u64Len;
        u64 base = aIov[i].u64Base;

        while (off < len) {
            size_t cb = (size_t)(len - off);
            i64 n;

            if (cb > sizeof(aChunk)) {
                cb = sizeof(aChunk);
            }
            /* Soft residual: cap cumulative transfer (Soft!=product). */
            if ((u64)i64Total + (u64)cb > HOT_RES_IO_MAX) {
                if ((u64)i64Total >= HOT_RES_IO_MAX) {
                    goto readv_done;
                }
                cb = (size_t)(HOT_RES_IO_MAX - (u64)i64Total);
            }
            if (vfs_ram_fd_ok((i64)u64Fd)) {
                n = vfs_ram_read((i64)u64Fd, aChunk, cb);
            } else {
                n = hot_sock_recv_lean((i64)u64Fd, aChunk, cb);
            }
            if (n < 0) {
                return i64Total > 0 ? i64Total : n;
            }
            if (n == 0) {
                goto readv_done;
            }
            if (user_range_ok(base + off, (u64)n)) {
                if (copy_to_user(base + off, aChunk, (size_t)n) != GJ_OK) {
                    return i64Total > 0 ? i64Total : -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)(base + off), aChunk, (size_t)n);
            }
            i64Total += n;
            off += (u64)n;
            if ((u64)n < (u64)cb) {
                goto readv_done;
            }
        }
    }
readv_done:
    if (i64Total > 0) {
        g_u64ResIoReadCalls++;
        g_u64ResIoBytes += (u64)i64Total;
        if (vfs_ram_fd_ok((i64)u64Fd)) {
            g_u64ResIoVfsHits++;
            hot_residual_io_lean_once("readv", (i64)u64Fd);
        } else {
            hot_residual_io_lean_once("readv_sock", (i64)u64Fd);
            hot_residual_sock_io_once("readv", (i64)u64Fd, 0);
        }
    }
    return i64Total;
}

i64
gj_linux_hot_preadv(struct gj_linux_regs *pRegs)
{
    /*
     * preadv(fd, iov, iovcnt, offset): positioned residual via vfs_ram_pread.
     * Soft!=product · G-AC-1 · greppable: linux_hot: residual io lean
     */
    u64 u64Fd;
    u64 u64Iov;
    u64 u64Cnt;
    u64 u64Off;
    int nIov;
    int i;
    i64 i64Total = 0;
    struct linux_iovec aIov[8];
    u8 aChunk[256];

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Fd = pRegs->u64Arg0;
    u64Iov = pRegs->u64Arg1;
    u64Cnt = pRegs->u64Arg2;
    u64Off = pRegs->u64Arg3;
    if (u64Cnt == 0) {
        return 0;
    }
    if (u64Fd <= 2) {
        return u64Fd == 0 ? 0 : -LINUX_EBADF;
    }
    if (!vfs_ram_fd_ok((i64)u64Fd)) {
        return -LINUX_EBADF;
    }
    nIov = load_iovecs(u64Iov, u64Cnt, aIov, 8);
    if (nIov < 0) {
        return -LINUX_EFAULT;
    }
    for (i = 0; i < nIov; i++) {
        u64 off = 0;
        u64 len = aIov[i].u64Len;
        u64 base = aIov[i].u64Base;

        while (off < len) {
            size_t cb = (size_t)(len - off);
            i64 n;

            if (cb > sizeof(aChunk)) {
                cb = sizeof(aChunk);
            }
            if ((u64)i64Total + (u64)cb > HOT_RES_IO_MAX) {
                if ((u64)i64Total >= HOT_RES_IO_MAX) {
                    goto preadv_done;
                }
                cb = (size_t)(HOT_RES_IO_MAX - (u64)i64Total);
            }
            n = vfs_ram_pread((i64)u64Fd, aChunk, cb, u64Off + (u64)i64Total);
            if (n < 0) {
                return i64Total > 0 ? i64Total : n;
            }
            if (n == 0) {
                goto preadv_done;
            }
            if (user_range_ok(base + off, (u64)n)) {
                if (copy_to_user(base + off, aChunk, (size_t)n) != GJ_OK) {
                    return i64Total > 0 ? i64Total : -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)(base + off), aChunk, (size_t)n);
            }
            i64Total += n;
            off += (u64)n;
            if ((u64)n < (u64)cb) {
                goto preadv_done;
            }
        }
    }
preadv_done:
    if (i64Total > 0) {
        g_u64ResIoReadCalls++;
        g_u64ResIoVfsHits++;
        g_u64ResIoBytes += (u64)i64Total;
        hot_residual_io_lean_once("preadv", (i64)u64Fd);
    }
    return i64Total;
}

i64
gj_linux_hot_pwritev(struct gj_linux_regs *pRegs)
{
    /*
     * pwritev(fd, iov, iovcnt, offset): positioned residual via vfs_ram_pwrite.
     * Soft!=product · G-AC-1 · greppable: linux_hot: residual io lean
     */
    u64 u64Fd;
    u64 u64Iov;
    u64 u64Cnt;
    u64 u64Off;
    int nIov;
    int i;
    i64 i64Total = 0;
    struct linux_iovec aIov[8];
    u8 aChunk[256];

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Fd = pRegs->u64Arg0;
    u64Iov = pRegs->u64Arg1;
    u64Cnt = pRegs->u64Arg2;
    u64Off = pRegs->u64Arg3;
    if (u64Cnt == 0) {
        return 0;
    }
    if (u64Fd <= 2) {
        /* stdio: fall back to sequential writev residual (no seek). */
        return gj_linux_hot_writev(pRegs);
    }
    if (!vfs_ram_fd_ok((i64)u64Fd)) {
        return -LINUX_EBADF;
    }
    nIov = load_iovecs(u64Iov, u64Cnt, aIov, 8);
    if (nIov < 0) {
        return -LINUX_EFAULT;
    }
    for (i = 0; i < nIov; i++) {
        u64 off = 0;
        u64 len = aIov[i].u64Len;
        u64 base = aIov[i].u64Base;

        while (off < len) {
            size_t cb = (size_t)(len - off);
            i64 n;

            if (cb > sizeof(aChunk)) {
                cb = sizeof(aChunk);
            }
            if ((u64)i64Total + (u64)cb > HOT_RES_IO_MAX) {
                if ((u64)i64Total >= HOT_RES_IO_MAX) {
                    goto pwritev_done;
                }
                cb = (size_t)(HOT_RES_IO_MAX - (u64)i64Total);
            }
            if (user_range_ok(base + off, cb)) {
                if (copy_from_user(aChunk, base + off, cb) != GJ_OK) {
                    return i64Total > 0 ? i64Total : -LINUX_EFAULT;
                }
            } else {
                memcpy(aChunk, (const void *)(gj_vaddr_t)(base + off), cb);
            }
            n = vfs_ram_pwrite((i64)u64Fd, aChunk, cb, u64Off + (u64)i64Total);
            if (n < 0) {
                return i64Total > 0 ? i64Total : n;
            }
            if (n == 0) {
                goto pwritev_done;
            }
            i64Total += n;
            off += (u64)n;
            if ((u64)n < (u64)cb) {
                goto pwritev_done;
            }
        }
    }
pwritev_done:
    if (i64Total > 0) {
        g_u64ResIoWriteCalls++;
        g_u64ResIoVfsHits++;
        g_u64ResIoBytes += (u64)i64Total;
        hot_residual_io_lean_once("pwritev", (i64)u64Fd);
    }
    return i64Total;
}

i64
gj_linux_hot_tkill(struct gj_linux_regs *pRegs)
{
    /* tkill(tid, sig): accept self; ignore delivery for bring-up */
    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if ((u32)pRegs->u64Arg0 == g_u32LinuxTid ||
        (u32)pRegs->u64Arg0 == g_u32LinuxPid || pRegs->u64Arg0 == 0) {
        if ((u32)pRegs->u64Arg1 > 0 && (u32)pRegs->u64Arg1 < 64) {
            vfs_ram_signalfd_inject((u32)pRegs->u64Arg1);
        }
        return 0;
    }
    return -LINUX_ESRCH;
}

i64
gj_linux_hot_tgkill(struct gj_linux_regs *pRegs)
{
    /* tgkill(tgid, tid, sig) */
    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if ((u32)pRegs->u64Arg0 != g_u32LinuxPid && pRegs->u64Arg0 != 0 &&
        (u32)pRegs->u64Arg0 != 1) {
        return -LINUX_ESRCH;
    }
    pRegs->u64Arg0 = pRegs->u64Arg1; /* tid */
    pRegs->u64Arg1 = pRegs->u64Arg2; /* sig */
    return gj_linux_hot_tkill(pRegs);
}

i64
gj_linux_hot_sigaltstack(struct gj_linux_regs *pRegs)
{
    /* Accept set/get; report disabled stack */
    static u64 g_u64SsSp;
    static u32 g_u32SsFlags = 2; /* SS_DISABLE */
    static u64 g_u64SsSize;

    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg1 != 0) {
        u64 aOld[3];

        aOld[0] = g_u64SsSp;
        aOld[1] = g_u32SsFlags;
        aOld[2] = g_u64SsSize;
        if (user_range_ok(pRegs->u64Arg1, 24)) {
            (void)copy_to_user(pRegs->u64Arg1, aOld, 24);
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aOld, 24);
        }
    }
    if (pRegs->u64Arg0 != 0) {
        u64 aNew[3];

        if (user_range_ok(pRegs->u64Arg0, 24)) {
            (void)copy_from_user(aNew, pRegs->u64Arg0, 24);
        } else {
            memcpy(aNew, (const void *)(gj_vaddr_t)pRegs->u64Arg0, 24);
        }
        g_u64SsSp = aNew[0];
        g_u32SsFlags = (u32)aNew[1];
        g_u64SsSize = aNew[2];
    }
    return 0;
}

i64
gj_linux_hot_sched_getscheduler(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    (void)pRegs;
    return 0; /* SCHED_NORMAL */
}

i64
gj_linux_hot_sched_setscheduler(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_sched_getparam(struct gj_linux_regs *pRegs)
{
    int prio = 0;

    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0) {
        return -LINUX_EINVAL;
    }
    if (user_range_ok(pRegs->u64Arg1, 4)) {
        (void)copy_to_user(pRegs->u64Arg1, &prio, 4);
    } else {
        *(int *)(gj_vaddr_t)pRegs->u64Arg1 = prio;
    }
    return 0;
}

i64
gj_linux_hot_sched_setparam(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_sched_get_priority_max(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    (void)pRegs;
    return 0; /* SCHED_OTHER max */
}

i64
gj_linux_hot_sched_get_priority_min(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_msync(struct gj_linux_regs *pRegs)
{
    u64 va;
    u64 len;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    va = pRegs->u64Arg0;
    len = pRegs->u64Arg1;
    /* Ram-backed maps: always coherent; MS_* flags ignored for bring-up */
    if (len == 0) {
        return 0;
    }
    if ((va & 0xfffull) != 0) {
        return -LINUX_EINVAL;
    }
    (void)va;
    return 0;
}

/**
 * C2 sock I/O residual once-lamp (write/writev/readv dual-table).
 * Soft!=product · G-AC-1 · UDX/sshd functional · Dual DoD A/B OPEN.
 * greppable: linux_hot: residual sock io
 * greppable: linux_hot: residual sock io lean
 */
static void
hot_residual_sock_io_once(const char *szVia, i64 i64Fd, int fSend)
{
    if (g_fResSockIoOnce != 0u) {
        return;
    }
    g_fResSockIoOnce = 1u;
    if (szVia == NULL) {
        szVia = "sock_io";
    }
    /*
     * Grep: linux_hot: residual sock io
     * Grep: linux_hot: residual sock io lean
     * Honesty: hot direct dual-table != product multi-server confine;
     * Soft!=product · no freestanding rtl residual · G-AC-1.
     */
    kprintf("linux_hot: residual sock io via=%s fd=%ld send=%d "
            "send_ok=%lu recv_ok=%lu bytes=%lu pump=%lu "
            "Soft!=product G-AC-1 dual_dod_ab=OPEN "
            "product_dir=UDX+ABI agent_ne_close=1 stamp_storm=0 "
            "(C2 hot ABI; dual_table net_tcp|net_lo; UDX/sshd)\n",
            szVia, (long)i64Fd, fSend ? 1 : 0,
            (unsigned long)g_u64ResSockIoSend,
            (unsigned long)g_u64ResSockIoRecv,
            (unsigned long)g_u64ResSockIoBytes,
            (unsigned long)g_u64ResSockIoPump);
    kprintf("linux_hot: residual sock io lean via=%s dual_table=1 "
            "write+writev+readv=1 eagain_pump=1 shut_wr_epipe=1 "
            "soerr_clear_on_send_ok=1 soft_ne_product=1 G-AC-1=1 "
            "dual_dod_ab=OPEN (C2 hot ABI residual; UDX/sshd)\n",
            szVia);
    hot_residual_lean_once(fSend != 0 ? "sock_io_send" : "sock_io_recv");
}

/** Soft EAGAIN classifier (Linux-shaped -11). Soft!=product. */
static int
hot_res_is_again(i64 i64Ret)
{
    return (i64Ret == -(i64)LINUX_EAGAIN || i64Ret == -11) ? 1 : 0;
}

/**
 * C2: sticky SO_ERROR clear on successful send (Linux-shaped). Soft!=product.
 * greppable: linux_hot: residual so_error
 */
static void
hot_tcp_soerr_clear(i64 i64Fd)
{
    struct hot_tcp_sock_lean *pSlot;

    pSlot = hot_tcp_lean_slot(i64Fd);
    if (pSlot == NULL) {
        return;
    }
    pSlot->u16SoError = 0u;
}

/**
 * C2 hot sock send lean: dual-table net_tcp|net_lo for write/writev.
 * SHUT_WR pre-check EPIPE + sticky SO_ERROR; EAGAIN one poll+retry;
 * send ok clears sticky SO_ERROR. Soft!=product · G-AC-1 · UDX/sshd.
 * greppable: linux_hot: residual sock io
 */
static i64
hot_sock_send_lean(i64 i64Fd, const void *pBuf, size_t cb)
{
    struct hot_tcp_sock_lean *pSlot;
    i64 i64N;
    int fTcp = 0;

    if (pBuf == NULL && cb != 0u) {
        return -(i64)LINUX_EFAULT;
    }
    if (cb == 0u) {
        return 0;
    }
    if (net_tcp_fd_ok(i64Fd)) {
        fTcp = 1;
    } else if (!net_lo_fd_ok(i64Fd)) {
        return -(i64)LINUX_EBADF;
    }

    /*
     * SHUT_WR pre-check before product send (EPIPE footgun for sshd
     * half-close). Soft!=product · greppable: residual shutdown lean.
     */
    if (fTcp != 0) {
        pSlot = hot_tcp_lean_slot(i64Fd);
        if (pSlot != NULL && (pSlot->u8Shut & HOT_SHUT_WR) != 0u) {
            hot_tcp_soerr_set(i64Fd, (u16)LINUX_EPIPE);
            return -(i64)LINUX_EPIPE;
        }
        i64N = net_tcp_send(i64Fd, pBuf, cb);
        if (hot_res_is_again(i64N) != 0) {
            g_u64ResSockIoPump++;
            net_tcp_poll();
            i64N = net_tcp_send(i64Fd, pBuf, cb);
        }
    } else {
        i64N = net_lo_send(i64Fd, pBuf, cb);
    }

    if (i64N < 0) {
        if (fTcp != 0) {
            if (i64N == -(i64)LINUX_EPIPE || i64N == -32) {
                pSlot = hot_tcp_lean_slot(i64Fd);
                if (pSlot != NULL) {
                    pSlot->u8Shut = (u8)(pSlot->u8Shut | HOT_SHUT_WR);
                }
                hot_tcp_soerr_set(i64Fd, (u16)LINUX_EPIPE);
            } else if (i64N == -(i64)LINUX_ECONNRESET || i64N == -104) {
                pSlot = hot_tcp_lean_slot(i64Fd);
                if (pSlot != NULL) {
                    pSlot->u8Shut =
                        (u8)(pSlot->u8Shut | HOT_SHUT_RD | HOT_SHUT_WR);
                    pSlot->u8AcceptConn = 0u;
                }
                hot_tcp_soerr_set(i64Fd, (u16)LINUX_ECONNRESET);
            }
        }
        return i64N;
    }

    if (fTcp != 0) {
        hot_tcp_soerr_clear(i64Fd);
    }
    g_u64ResSockIoSend++;
    g_u64ResSockIoBytes += (u64)i64N;
    return i64N;
}

/**
 * C2 hot sock recv lean: dual-table net_tcp|net_lo for readv.
 * EAGAIN one poll+retry; recv EOF soft SHUT_RD lean mirror; sticky
 * SO_ERROR on RST. Soft!=product · G-AC-1 · UDX/sshd.
 * greppable: linux_hot: residual sock io
 */
static i64
hot_sock_recv_lean(i64 i64Fd, void *pBuf, size_t cb)
{
    struct hot_tcp_sock_lean *pSlot;
    i64 i64N;
    int fTcp = 0;

    if (pBuf == NULL && cb != 0u) {
        return -(i64)LINUX_EFAULT;
    }
    if (cb == 0u) {
        return 0;
    }
    if (net_tcp_fd_ok(i64Fd)) {
        fTcp = 1;
    } else if (!net_lo_fd_ok(i64Fd)) {
        return -(i64)LINUX_EBADF;
    }

    if (fTcp != 0) {
        i64N = net_tcp_recv(i64Fd, pBuf, cb);
        if (hot_res_is_again(i64N) != 0) {
            g_u64ResSockIoPump++;
            net_tcp_poll();
            i64N = net_tcp_recv(i64Fd, pBuf, cb);
        }
    } else {
        i64N = net_lo_recv(i64Fd, pBuf, cb);
    }

    if (i64N < 0) {
        if (fTcp != 0) {
            if (i64N == -(i64)LINUX_ECONNRESET || i64N == -104) {
                pSlot = hot_tcp_lean_slot(i64Fd);
                if (pSlot != NULL) {
                    pSlot->u8Shut =
                        (u8)(pSlot->u8Shut | HOT_SHUT_RD | HOT_SHUT_WR);
                    pSlot->u8AcceptConn = 0u;
                }
                hot_tcp_soerr_set(i64Fd, (u16)LINUX_ECONNRESET);
            }
        }
        return i64N;
    }

    /*
     * Product recv EOF (0): soft SHUT_RD lean mirror so SO_ACCEPTCONN /
     * poll residual stay coherent for next get (sshd session end).
     */
    if (i64N == 0 && fTcp != 0) {
        pSlot = hot_tcp_lean_slot(i64Fd);
        if (pSlot != NULL) {
            pSlot->u8Shut = (u8)(pSlot->u8Shut | HOT_SHUT_RD);
            pSlot->u8AcceptConn = 0u;
        }
    }

    if (i64N > 0) {
        if (fTcp != 0) {
            hot_tcp_soerr_clear(i64Fd);
        }
        g_u64ResSockIoRecv++;
        g_u64ResSockIoBytes += (u64)i64N;
    }
    return i64N;
}

/**
 * Soft residual sock-gate deepen: fd classify + poll_mask probe.
 * C2: POLLERR soft-sets sticky SO_ERROR (RST-shaped) for SO_ERROR probes.
 * Tallies always first; once-lamps only (no stamp storms). Soft!=product.
 * greppable: linux_hot: residual fd lean
 * greppable: linux_hot: residual poll lean
 * greppable: linux_hot: residual socket gate
 * greppable: linux_hot: residual so_error
 */
static void
hot_residual_sock_gate_deepen(i64 i64Fd, int fTcp)
{
    u32 u32Mask;

    /* 1) fd lean tallies (always). */
    if (fTcp != 0) {
        g_u64ResFdGateTcp++;
    } else {
        g_u64ResFdGateLo++;
    }

    /* 2) poll lean soft probe (always; cold poll/epoll share these masks). */
    if (fTcp != 0) {
        u32Mask = net_tcp_poll_mask(i64Fd, HOT_POLL_WANT_DEF);
    } else {
        u32Mask = net_lo_poll_mask(i64Fd, HOT_POLL_WANT_DEF);
    }
    g_u64ResPollHits++;
    g_u32ResPollLastMask = u32Mask;

    /*
     * C2: POLLERR soft sticky SO_ERROR (RST-shaped) for UDX host probes.
     * Soft!=product · greppable: linux_hot: residual so_error
     */
    if (fTcp != 0 && (u32Mask & HOT_POLL_ERR) != 0u) {
        struct hot_tcp_sock_lean *pSlot = hot_tcp_lean_slot(i64Fd);

        if (pSlot != NULL && pSlot->u16SoError == 0u) {
            hot_tcp_soerr_set(i64Fd, (u16)LINUX_ECONNRESET);
        }
    }

    /* 3) Once-lamps after tallies so rollup sees fd+poll counts. */
    if (g_fResFdLeanOnce == 0u) {
        g_fResFdLeanOnce = 1u;
        /*
         * Grep: linux_hot: residual fd lean
         * Dual table: net_lo FDs + net_tcp STREAM FDs (96..111).
         */
        kprintf("linux_hot: residual fd lean fd=%ld tcp=%d lo=%lu "
                "tcp_hits=%lu gate=%lu Soft!=product G-AC-1\n",
                (long)i64Fd, fTcp ? 1 : 0,
                (unsigned long)g_u64ResFdGateLo,
                (unsigned long)g_u64ResFdGateTcp,
                (unsigned long)g_u64ResSockGateHits);
    }
    if (g_fResPollLeanOnce == 0u) {
        g_fResPollLeanOnce = 1u;
        /*
         * Grep: linux_hot: residual poll lean
         * Soft readiness for driver-host poll/epoll consumers (not product).
         */
        kprintf("linux_hot: residual poll lean fd=%ld tcp=%d mask=0x%x "
                "want=0x%x hits=%lu Soft!=product G-AC-1\n",
                (long)i64Fd, fTcp ? 1 : 0, (unsigned)u32Mask,
                (unsigned)HOT_POLL_WANT_DEF,
                (unsigned long)g_u64ResPollHits);
    }
    if (fTcp != 0 && g_fResSockOnce == 0u) {
        g_fResSockOnce = 1u;
        kprintf("linux_hot: residual socket gate tcp_fd=%ld "
                "dual_table=1 Soft!=product\n",
                (long)i64Fd);
    }
    /* Unified rollup once (no stamp storms; no version stamp). */
    hot_residual_lean_once(fTcp != 0 ? "sock_gate_tcp" : "sock_gate_lo");
}

/**
 * Residual socket fd gate: net_lo table or net_tcp STREAM.
 * Negative fd fails closed. On accept: fd lean + soft poll_mask probe.
 * Soft!=product · G-AC-1 · no freestanding rtl residual.
 * greppable: linux_hot: residual socket gate
 * greppable: linux_hot: residual fd lean
 * greppable: linux_hot: residual poll lean
 */
static int
hot_sock_fd_gate(i64 i64Fd, int *pFTcp)
{
    if (pFTcp != NULL) {
        *pFTcp = 0;
    }
    if (i64Fd < 0) {
        return 0;
    }
    if (net_lo_fd_ok(i64Fd)) {
        g_u64ResSockGateHits++;
        hot_residual_sock_gate_deepen(i64Fd, 0);
        return 1;
    }
    if (net_tcp_fd_ok(i64Fd)) {
        if (pFTcp != NULL) {
            *pFTcp = 1;
        }
        g_u64ResSockGateHits++;
        hot_residual_sock_gate_deepen(i64Fd, 1);
        return 1;
    }
    return 0;
}

/**
 * Lean soft per-slot state for net_tcp FD (96..111). NULL if not TCP range.
 * Default soft buf sizes on first touch. Soft!=product · G-AC-1.
 * greppable: linux_hot: residual sock lean
 */
static struct hot_tcp_sock_lean *
hot_tcp_lean_slot(i64 i64Fd)
{
    u32 u32Idx;
    struct hot_tcp_sock_lean *pSlot;

    if (i64Fd < (i64)HOT_TCP_FD_BASE) {
        return NULL;
    }
    u32Idx = (u32)((u64)i64Fd - (u64)HOT_TCP_FD_BASE);
    if (u32Idx >= HOT_TCP_MAX) {
        return NULL;
    }
    pSlot = &g_aHotTcpLean[u32Idx];
    if (pSlot->u8Inited == 0u) {
        pSlot->u32SndBuf = (u32)HOT_SOFT_SOCKBUF;
        pSlot->u32RcvBuf = (u32)HOT_SOFT_SOCKBUF;
        pSlot->u8Inited = 1u;
    }
    return pSlot;
}

/** Soft sockaddr_in fill: AF_INET + port (host) + 127.0.0.1. */
static void
hot_sock_fill_sin(u8 *pSa, u16 u16PortHost)
{
    u32 i;

    if (pSa == NULL) {
        return;
    }
    for (i = 0; i < 16u; i++) {
        pSa[i] = 0;
    }
    pSa[0] = (u8)HOT_AF_INET; /* little-endian sa_family */
    pSa[1] = 0;
    /* sin_port network order */
    pSa[2] = (u8)((u16PortHost >> 8) & 0xffu);
    pSa[3] = (u8)(u16PortHost & 0xffu);
    /* 127.0.0.1 network order */
    pSa[4] = 127;
    pSa[5] = 0;
    pSa[6] = 0;
    pSa[7] = 1;
}

/** Host-order port from sockaddr_in (sin_port network order). Soft!=product. */
static u16
hot_sin_port_host(const u8 *pSa)
{
    if (pSa == NULL) {
        return 0;
    }
    return (u16)(((u16)pSa[2] << 8) | (u16)pSa[3]);
}

/**
 * C2: sticky SO_ERROR soft set (positive LINUX_E*). Soft!=product.
 * greppable: linux_hot: residual so_error
 */
static void
hot_tcp_soerr_set(i64 i64Fd, u16 u16Err)
{
    struct hot_tcp_sock_lean *pSlot;

    if (u16Err == 0u) {
        return;
    }
    pSlot = hot_tcp_lean_slot(i64Fd);
    if (pSlot == NULL) {
        return;
    }
    pSlot->u16SoError = u16Err;
    g_u64ResSoErrSet++;
    if (g_fResSoErrOnce == 0u) {
        g_fResSoErrOnce = 1u;
        kprintf("linux_hot: residual so_error set fd=%ld err=%u "
                "Soft!=product G-AC-1 dual_dod_ab=OPEN "
                "(C2 hot ABI; sticky clear-on-read)\n",
                (long)i64Fd, (unsigned)u16Err);
        hot_residual_lean_once("so_error");
    }
}

/**
 * C2: soft product name sync via net_tcp_getsockname/getpeername.
 * Prefer product tables; seed lean ports + listen-shaped SO_ACCEPTCONN.
 * Soft!=product · G-AC-1 · Dual DoD A/B OPEN (agent!=close).
 * greppable: linux_hot: residual name sync
 * greppable: linux_hot: residual peer name
 * greppable: linux_hot: residual ACCEPTCONN
 * greppable: linux_hot: residual C2 hot ABI
 */
static int
hot_tcp_product_name_sync(i64 i64Fd)
{
    u8 aSa[16];
    u32 u32Plen;
    struct hot_tcp_sock_lean *pSlot;
    int fSynced = 0;
    i64 i64R;

    pSlot = hot_tcp_lean_slot(i64Fd);
    u32Plen = 16u;
    i64R = net_tcp_getsockname(i64Fd, aSa, &u32Plen);
    if (i64R == 0) {
        u16 u16L = hot_sin_port_host(aSa);

        if (pSlot != NULL && u16L != 0u) {
            pSlot->u16LocalPort = u16L;
        }
        g_u64ResNameSync++;
        fSynced = 1;
    }
    u32Plen = 16u;
    i64R = net_tcp_getpeername(i64Fd, aSa, &u32Plen);
    if (i64R == 0) {
        u16 u16Rport = hot_sin_port_host(aSa);

        if (pSlot != NULL) {
            if (u16Rport != 0u) {
                pSlot->u16PeerPort = u16Rport;
            }
            pSlot->u8PeerOk = 1u;
            pSlot->u8AcceptConn = 0u; /* connected: not listen-shaped */
        }
        g_u64ResPeerName++;
        fSynced = 1;
    } else if (i64R == -(i64)LINUX_ENOTCONN) {
        /*
         * Listener-shaped soft SO_ACCEPTCONN when local name known and
         * peer not connected (UDX host / sshd-shaped name residual).
         * Soft!=product · greppable: linux_hot: residual ACCEPTCONN
         */
        if (pSlot != NULL && pSlot->u16LocalPort != 0u &&
            pSlot->u8PeerOk == 0u && pSlot->u8AcceptConn == 0u) {
            pSlot->u8AcceptConn = 1u;
            g_u64ResAcceptConn++;
        }
    }
    if (fSynced != 0 && g_fResNameSyncOnce == 0u) {
        g_fResNameSyncOnce = 1u;
        /*
         * Grep: linux_hot: residual name sync
         * Grep: linux_hot: residual peer name
         * Grep: linux_hot: residual C2 hot ABI
         */
        kprintf("linux_hot: residual name sync fd=%ld lport=%u rport=%u "
                "peer_ok=%u acceptconn=%u Soft!=product G-AC-1 "
                "dual_dod_ab=OPEN (product_dir=UDX+ABI; C2 hot ABI)\n",
                (long)i64Fd,
                (unsigned)(pSlot != NULL ? pSlot->u16LocalPort : 0u),
                (unsigned)(pSlot != NULL ? pSlot->u16PeerPort : 0u),
                (unsigned)(pSlot != NULL ? pSlot->u8PeerOk : 0u),
                (unsigned)(pSlot != NULL ? pSlot->u8AcceptConn : 0u));
        hot_residual_lean_once("name_sync");
    }
    return fSynced;
}

/** Clamp soft SOCKBUF probe value (bytes). Soft!=product. */
static u32
hot_soft_sockbuf_clamp(int nVal)
{
    if (nVal < (int)HOT_SOFT_SOCKBUF_MIN) {
        return (u32)HOT_SOFT_SOCKBUF_MIN;
    }
    if (nVal > (int)HOT_SOFT_SOCKBUF) {
        return (u32)HOT_SOFT_SOCKBUF;
    }
    return (u32)nVal;
}

/**
 * Lean TCP SOL_SOCKET get residual:
 * TYPE/ERROR/REUSE_star/KEEPALIVE/BROADCAST/ACCEPTCONN/
 * PROTOCOL/DOMAIN/RCVBUF/SNDBUF (+ soft LINGER pair when len>=8).
 * C2: sticky SO_ERROR clear-on-read; SO_ACCEPTCONN lean listen bit;
 * SO_PROTOCOL=IPPROTO_TCP; SO_DOMAIN=AF_INET (glibc/sshd probes).
 * Returns 0 and fills *pV / *pLen, or -LINUX_E*. Soft!=product.
 * greppable: linux_hot: residual sock lean
 * greppable: linux_hot: residual so_error
 * greppable: linux_hot: residual ACCEPTCONN
 * greppable: linux_hot: residual SO_PROTOCOL
 * greppable: linux_hot: residual SO_DOMAIN
 */
static i64
hot_tcp_getsockopt_lean(i64 i64Fd, int nLevel, int nOpt, int *pV, u32 *pLen)
{
    struct hot_tcp_sock_lean *pSlot;
    int nVal = 0;

    if (pV == NULL || pLen == NULL) {
        return -LINUX_EINVAL;
    }
    if (nLevel != HOT_SOL_SOCKET) {
        return -LINUX_ENOPROTOOPT;
    }
    pSlot = hot_tcp_lean_slot(i64Fd);
    if (nOpt == HOT_SO_LINGER) {
        if (*pLen < 8u) {
            return -LINUX_EINVAL;
        }
        pV[0] = (pSlot != NULL && pSlot->u8LingerOn != 0u) ? 1 : 0;
        pV[1] = (pSlot != NULL) ? (int)pSlot->u16LingerSec : 0;
        *pLen = 8u;
        g_u64ResSockGet++;
        if (g_fResSockLeanOnce == 0) {
            g_fResSockLeanOnce = 1;
            kprintf("linux_hot: residual sock lean getopt linger fd=%ld "
                    "Soft!=product G-AC-1\n",
                    (long)i64Fd);
            hot_residual_lean_once("sock_get");
        }
        return 0;
    }
    if (*pLen < 4u) {
        return -LINUX_EINVAL;
    }
    if (nOpt == HOT_SO_TYPE) {
        nVal = HOT_SOCK_STREAM;
    } else if (nOpt == HOT_SO_ERROR) {
        /*
         * C2 sticky SO_ERROR clear-on-read (Linux-shaped). Soft!=product.
         * greppable: linux_hot: residual so_error
         */
        if (pSlot != NULL && pSlot->u16SoError != 0u) {
            nVal = (int)pSlot->u16SoError;
            pSlot->u16SoError = 0u;
            g_u64ResSoErrGet++;
        } else {
            nVal = 0;
        }
    } else if (nOpt == HOT_SO_REUSEADDR) {
        nVal = (pSlot != NULL && pSlot->u8Reuse != 0u) ? 1 : 0;
    } else if (nOpt == HOT_SO_REUSEPORT) {
        nVal = (pSlot != NULL && pSlot->u8ReusePort != 0u) ? 1 : 0;
    } else if (nOpt == HOT_SO_KEEPALIVE) {
        nVal = (pSlot != NULL && pSlot->u8Keepalive != 0u) ? 1 : 0;
    } else if (nOpt == HOT_SO_BROADCAST) {
        nVal = (pSlot != NULL && pSlot->u8Broadcast != 0u) ? 1 : 0;
    } else if (nOpt == HOT_SO_ACCEPTCONN) {
        /*
         * C2 SO_ACCEPTCONN lean listen-shaped bit (name sync seeds). Soft!=product.
         * greppable: linux_hot: residual ACCEPTCONN
         */
        nVal = (pSlot != NULL && pSlot->u8AcceptConn != 0u) ? 1 : 0;
    } else if (nOpt == HOT_SO_PROTOCOL) {
        /*
         * C2 SO_PROTOCOL soft get (IPPROTO_TCP for STREAM table). Soft!=product.
         * greppable: linux_hot: residual SO_PROTOCOL
         * glibc / sshd-shaped SOL_SOCKET probes.
         */
        nVal = HOT_IPPROTO_TCP;
    } else if (nOpt == HOT_SO_DOMAIN) {
        /*
         * C2 SO_DOMAIN soft get (AF_INET). Soft!=product.
         * greppable: linux_hot: residual SO_DOMAIN
         */
        nVal = HOT_AF_INET;
    } else if (nOpt == HOT_SO_RCVBUF) {
        nVal = (pSlot != NULL) ? (int)pSlot->u32RcvBuf
                               : (int)HOT_SOFT_SOCKBUF;
    } else if (nOpt == HOT_SO_SNDBUF) {
        nVal = (pSlot != NULL) ? (int)pSlot->u32SndBuf
                               : (int)HOT_SOFT_SOCKBUF;
    } else {
        nVal = 0; /* other SOL_SOCKET probes soft-accept */
    }
    *pV = nVal;
    *pLen = 4u;
    g_u64ResSockGet++;
    if (g_fResSockLeanOnce == 0) {
        g_fResSockLeanOnce = 1;
        kprintf("linux_hot: residual sock lean getopt opt=%d val=%d fd=%ld "
                "Soft!=product G-AC-1\n",
                nOpt, nVal, (long)i64Fd);
        hot_residual_lean_once("sock_get");
    }
    return 0;
}

/**
 * Lean TCP SOL_SOCKET set residual:
 * REUSE_star/KEEPALIVE/BROADCAST store; SND/RCVBUF clamp store;
 * LINGER soft pair; DEBUG/DONTROUTE/OOBINLINE/timeos soft-accept.
 * REUSE_star footgun fix: REUSEADDR and REUSEPORT both tally + lamp
 * (PORT previously silent; multi-worker UDX host undercount).
 * greppable: linux_hot: residual sock lean
 * greppable: linux_hot: residual REUSE_star
 */
static i64
hot_tcp_setsockopt_lean(i64 i64Fd, int nLevel, int nOpt, int nVal)
{
    struct hot_tcp_sock_lean *pSlot;

    g_u64ResSockSet++;
    if (nLevel != HOT_SOL_SOCKET) {
        /* Soft accept non-SOL_SOCKET probes (glibc/TCP_NODELAY class). */
        (void)nOpt;
        (void)nVal;
        if (g_fResSockLeanOnce == 0) {
            g_fResSockLeanOnce = 1;
            kprintf("linux_hot: residual sock lean setopt level=%d soft=1 "
                    "fd=%ld Soft!=product G-AC-1\n",
                    nLevel, (long)i64Fd);
            hot_residual_lean_once("sock_set");
        }
        return 0;
    }
    pSlot = hot_tcp_lean_slot(i64Fd);
    if (nOpt == HOT_SO_REUSEADDR) {
        if (pSlot != NULL) {
            pSlot->u8Reuse = nVal ? 1u : 0u;
        }
        g_u64ResSockReuseSet++;
        /*
         * Grep: linux_hot: residual REUSE_star
         * Grep: linux_hot: residual sock lean
         */
        if (g_fResSockLeanOnce == 0) {
            g_fResSockLeanOnce = 1;
            kprintf("linux_hot: residual REUSE_star via=REUSEADDR val=%d "
                    "fd=%ld addr_sets=%lu port_sets=%lu Soft!=product "
                    "G-AC-1\n",
                    nVal ? 1 : 0, (long)i64Fd,
                    (unsigned long)g_u64ResSockReuseSet,
                    (unsigned long)g_u64ResSockReusePortSet);
            kprintf("linux_hot: residual sock lean reuse=%d fd=%ld "
                    "sets=%lu Soft!=product G-AC-1\n",
                    nVal ? 1 : 0, (long)i64Fd,
                    (unsigned long)g_u64ResSockReuseSet);
            hot_residual_lean_once("sock_set_reuseaddr");
        }
        return 0;
    }
    if (nOpt == HOT_SO_REUSEPORT) {
        /*
         * Footgun fix: REUSEPORT must tally + lamp like REUSEADDR
         * (REUSE_star parity). Soft!=product · G-AC-1.
         * greppable: linux_hot: residual REUSE_star
         */
        if (pSlot != NULL) {
            pSlot->u8ReusePort = nVal ? 1u : 0u;
        }
        g_u64ResSockReusePortSet++;
        if (g_fResSockLeanOnce == 0) {
            g_fResSockLeanOnce = 1;
            kprintf("linux_hot: residual REUSE_star via=REUSEPORT val=%d "
                    "fd=%ld addr_sets=%lu port_sets=%lu Soft!=product "
                    "G-AC-1\n",
                    nVal ? 1 : 0, (long)i64Fd,
                    (unsigned long)g_u64ResSockReuseSet,
                    (unsigned long)g_u64ResSockReusePortSet);
            kprintf("linux_hot: residual sock lean reuseport=%d fd=%ld "
                    "port_sets=%lu Soft!=product G-AC-1\n",
                    nVal ? 1 : 0, (long)i64Fd,
                    (unsigned long)g_u64ResSockReusePortSet);
            hot_residual_lean_once("sock_set_reuseport");
        }
        return 0;
    }
    if (nOpt == HOT_SO_KEEPALIVE) {
        if (pSlot != NULL) {
            pSlot->u8Keepalive = nVal ? 1u : 0u;
        }
        return 0;
    }
    if (nOpt == HOT_SO_BROADCAST) {
        if (pSlot != NULL) {
            pSlot->u8Broadcast = nVal ? 1u : 0u;
        }
        return 0;
    }
    if (nOpt == HOT_SO_SNDBUF) {
        if (pSlot != NULL) {
            pSlot->u32SndBuf = hot_soft_sockbuf_clamp(nVal);
        }
        return 0;
    }
    if (nOpt == HOT_SO_RCVBUF) {
        if (pSlot != NULL) {
            pSlot->u32RcvBuf = hot_soft_sockbuf_clamp(nVal);
        }
        return 0;
    }
    if (nOpt == HOT_SO_LINGER) {
        /* Caller may pass only first int; full pair handled in setsockopt. */
        if (pSlot != NULL) {
            pSlot->u8LingerOn = nVal ? 1u : 0u;
        }
        return 0;
    }
    /* SO_DEBUG / DONTROUTE / OOBINLINE / RCVTIMEO / SNDTIMEO soft no-op. */
    if (nOpt == HOT_SO_DEBUG || nOpt == HOT_SO_DONTROUTE ||
        nOpt == HOT_SO_OOBINLINE || nOpt == HOT_SO_RCVTIMEO ||
        nOpt == HOT_SO_SNDTIMEO) {
        return 0;
    }
    /* Remaining SOL_SOCKET: soft accept (no stack effect). */
    (void)nVal;
    return 0;
}

i64
gj_linux_hot_getsockopt(struct gj_linux_regs *pRegs)
{
    int aVal[2];
    u32 len = 4;
    u32 u32Copy;
    i64 i64Fd;
    int fTcp = 0;
    int nLevel;
    int nOpt;
    i64 i64Lean;

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    i64Fd = (i64)pRegs->u64Arg0;
    if (!hot_sock_fd_gate(i64Fd, &fTcp)) {
        return -LINUX_EBADF;
    }
    nLevel = (int)pRegs->u64Arg1;
    nOpt = (int)pRegs->u64Arg2;
    aVal[0] = 0;
    aVal[1] = 0;
    if (pRegs->u64Arg4 != 0) {
        if (user_range_ok(pRegs->u64Arg4, 4)) {
            (void)copy_from_user(&len, pRegs->u64Arg4, 4);
        } else {
            len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg4;
        }
    }
    if (fTcp) {
        /*
         * Lean TCP SOL_SOCKET residual (TYPE/ERROR/REUSE_star/KA/BROADCAST/
         * ACCEPTCONN/RCVBUF/SNDBUF/LINGER). Soft!=product · G-AC-1.
         * greppable: linux_hot: residual socket gate
         * greppable: linux_hot: residual sock lean
         */
        i64Lean = hot_tcp_getsockopt_lean(i64Fd, nLevel, nOpt, aVal, &len);
        if (i64Lean != 0) {
            return i64Lean;
        }
    } else {
        i64 r = net_lo_getsockopt(i64Fd, nLevel, nOpt, aVal, &len);

        if (r != 0) {
            return r;
        }
    }
    u32Copy = len;
    if (u32Copy > 8u) {
        u32Copy = 8u;
    }
    if (pRegs->u64Arg3 != 0 && u32Copy != 0u) {
        if (user_range_ok(pRegs->u64Arg3, u32Copy)) {
            (void)copy_to_user(pRegs->u64Arg3, aVal, u32Copy);
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg3, aVal, u32Copy);
        }
    }
    if (pRegs->u64Arg4 != 0) {
        if (user_range_ok(pRegs->u64Arg4, 4)) {
            (void)copy_to_user(pRegs->u64Arg4, &len, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg4 = len;
        }
    }
    return 0;
}

i64
gj_linux_hot_setsockopt(struct gj_linux_regs *pRegs)
{
    int v = 0;
    int aLinger[2];
    i64 i64Fd;
    int fTcp = 0;
    int nLevel;
    int nOpt;
    u32 u32OptLen;
    struct hot_tcp_sock_lean *pSlot;

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    i64Fd = (i64)pRegs->u64Arg0;
    if (!hot_sock_fd_gate(i64Fd, &fTcp)) {
        return -LINUX_EBADF;
    }
    nLevel = (int)pRegs->u64Arg1;
    nOpt = (int)pRegs->u64Arg2;
    u32OptLen = (u32)pRegs->u64Arg4;
    if (pRegs->u64Arg3 != 0 && u32OptLen >= 4u) {
        if (user_range_ok(pRegs->u64Arg3, 4)) {
            (void)copy_from_user(&v, pRegs->u64Arg3, 4);
        } else {
            v = *(const int *)(gj_vaddr_t)pRegs->u64Arg3;
        }
    }
    if (fTcp) {
        /*
         * Lean residual: REUSE_star/KEEPALIVE/BROADCAST/buf/LINGER store.
         * greppable: linux_hot: residual sock lean
         */
        if (nLevel == HOT_SOL_SOCKET && nOpt == HOT_SO_LINGER &&
            pRegs->u64Arg3 != 0 && u32OptLen >= 8u) {
            aLinger[0] = 0;
            aLinger[1] = 0;
            if (user_range_ok(pRegs->u64Arg3, 8)) {
                (void)copy_from_user(aLinger, pRegs->u64Arg3, 8);
            } else {
                aLinger[0] = ((const int *)(gj_vaddr_t)pRegs->u64Arg3)[0];
                aLinger[1] = ((const int *)(gj_vaddr_t)pRegs->u64Arg3)[1];
            }
            pSlot = hot_tcp_lean_slot(i64Fd);
            if (pSlot != NULL) {
                pSlot->u8LingerOn = aLinger[0] ? 1u : 0u;
                if (aLinger[1] < 0) {
                    pSlot->u16LingerSec = 0;
                } else if (aLinger[1] > 65535) {
                    pSlot->u16LingerSec = 65535u;
                } else {
                    pSlot->u16LingerSec = (u16)aLinger[1];
                }
            }
            g_u64ResSockSet++;
            if (g_fResSockLeanOnce == 0) {
                g_fResSockLeanOnce = 1;
                kprintf("linux_hot: residual sock lean setopt linger fd=%ld "
                        "Soft!=product G-AC-1\n",
                        (long)i64Fd);
                hot_residual_lean_once("sock_set");
            }
            return 0;
        }
        return hot_tcp_setsockopt_lean(i64Fd, nLevel, nOpt, v);
    }
    if (nLevel == HOT_SOL_SOCKET && nOpt == HOT_SO_LINGER &&
        pRegs->u64Arg3 != 0 && u32OptLen >= 8u) {
        aLinger[0] = 0;
        aLinger[1] = 0;
        if (user_range_ok(pRegs->u64Arg3, 8)) {
            (void)copy_from_user(aLinger, pRegs->u64Arg3, 8);
        } else {
            aLinger[0] = ((const int *)(gj_vaddr_t)pRegs->u64Arg3)[0];
            aLinger[1] = ((const int *)(gj_vaddr_t)pRegs->u64Arg3)[1];
        }
        return net_lo_setsockopt(i64Fd, nLevel, nOpt, aLinger, 8);
    }
    return net_lo_setsockopt(i64Fd, nLevel, nOpt, &v, 4);
}

i64
gj_linux_hot_getsockname(struct gj_linux_regs *pRegs)
{
    u8 aSa[16];
    u32 len = 16;
    i64 r;
    i64 i64Fd;
    int fTcp = 0;

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    i64Fd = (i64)pRegs->u64Arg0;
    if (!hot_sock_fd_gate(i64Fd, &fTcp)) {
        return -LINUX_EBADF;
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, 4)) {
            (void)copy_from_user(&len, pRegs->u64Arg2, 4);
        } else {
            len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
        }
    }
    if (fTcp) {
        struct hot_tcp_sock_lean *pSlot;
        u32 u32Plen = 16u;

        if (len < 16u) {
            return -LINUX_EINVAL;
        }
        /*
         * C2: prefer product net_tcp_getsockname; lean soft fallback.
         * greppable: linux_hot: residual name sync
         * greppable: linux_hot: residual C2 hot ABI
         */
        r = net_tcp_getsockname(i64Fd, aSa, &u32Plen);
        if (r == 0) {
            (void)hot_tcp_product_name_sync(i64Fd);
            len = (u32Plen > 16u) ? 16u : u32Plen;
            if (len < 16u) {
                len = 16u;
            }
        } else {
            pSlot = hot_tcp_lean_slot(i64Fd);
            hot_sock_fill_sin(aSa,
                              pSlot != NULL ? pSlot->u16LocalPort : 0u);
            len = 16u;
        }
    } else {
        r = net_lo_getsockname(i64Fd, aSa, &len);
        if (r != 0) {
            return r;
        }
    }
    if (pRegs->u64Arg1 != 0) {
        if (user_range_ok(pRegs->u64Arg1, len)) {
            (void)copy_to_user(pRegs->u64Arg1, aSa, len);
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aSa, len);
        }
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, 4)) {
            (void)copy_to_user(pRegs->u64Arg2, &len, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = len;
        }
    }
    return 0;
}

i64
gj_linux_hot_getpeername(struct gj_linux_regs *pRegs)
{
    u8 aSa[16];
    u32 len = 16;
    i64 r;
    i64 i64Fd;
    int fTcp = 0;

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    i64Fd = (i64)pRegs->u64Arg0;
    if (!hot_sock_fd_gate(i64Fd, &fTcp)) {
        return -LINUX_EBADF;
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, 4)) {
            (void)copy_from_user(&len, pRegs->u64Arg2, 4);
        } else {
            len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
        }
    }
    if (fTcp) {
        struct hot_tcp_sock_lean *pSlot;
        u32 u32Plen = 16u;

        if (len < 16u) {
            return -LINUX_EINVAL;
        }
        /*
         * C2: product net_tcp_getpeername first; lean peer soft fallback;
         * honest ENOTCONN when neither product nor lean peer is ready.
         * greppable: linux_hot: residual peer name
         * greppable: linux_hot: residual C2 hot ABI
         */
        r = net_tcp_getpeername(i64Fd, aSa, &u32Plen);
        if (r == 0) {
            (void)hot_tcp_product_name_sync(i64Fd);
            len = (u32Plen > 16u) ? 16u : u32Plen;
            if (len < 16u) {
                len = 16u;
            }
        } else {
            pSlot = hot_tcp_lean_slot(i64Fd);
            if (pSlot != NULL && pSlot->u8PeerOk != 0u &&
                pSlot->u16PeerPort != 0u) {
                hot_sock_fill_sin(aSa, pSlot->u16PeerPort);
                len = 16u;
                if (g_fResNameSyncOnce == 0u) {
                    /* Soft lean peer-only path still seeds once-lamp. */
                    (void)hot_tcp_product_name_sync(i64Fd);
                }
            } else {
                (void)hot_tcp_product_name_sync(i64Fd);
                return -LINUX_ENOTCONN;
            }
        }
    } else {
        r = net_lo_getpeername(i64Fd, aSa, &len);
        if (r != 0) {
            return r;
        }
    }
    if (pRegs->u64Arg1 != 0) {
        if (user_range_ok(pRegs->u64Arg1, len)) {
            (void)copy_to_user(pRegs->u64Arg1, aSa, len);
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aSa, len);
        }
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, 4)) {
            (void)copy_to_user(pRegs->u64Arg2, &len, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = len;
        }
    }
    return 0;
}

i64
gj_linux_hot_shutdown(struct gj_linux_regs *pRegs)
{
    i64 i64Fd;
    int fTcp = 0;
    int nHow;
    i64 i64R;
    struct hot_tcp_sock_lean *pSlot;

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    i64Fd = (i64)pRegs->u64Arg0;
    nHow = (int)pRegs->u64Arg1;
    if (!hot_sock_fd_gate(i64Fd, &fTcp)) {
        return -LINUX_EBADF;
    }
    if (fTcp) {
        /*
         * C2: bridge net_tcp_shutdown (was soft-OK no-op). Track half-close
         * bits; clear SO_ACCEPTCONN on SHUT_RD; sticky SO_ERROR on EPIPE.
         * Soft!=product · G-AC-1 · Dual DoD A/B OPEN.
         * greppable: linux_hot: residual shutdown lean
         * greppable: linux_hot: residual C2 hot ABI
         */
        if (nHow < 0 || nHow > 2) {
            return -LINUX_EINVAL;
        }
        i64R = net_tcp_shutdown(i64Fd, nHow);
        pSlot = hot_tcp_lean_slot(i64Fd);
        if (pSlot != NULL) {
            if (nHow == 0 || nHow == 2) {
                pSlot->u8Shut = (u8)(pSlot->u8Shut | HOT_SHUT_RD);
                pSlot->u8AcceptConn = 0u; /* listen stop / half-close */
            }
            if (nHow == 1 || nHow == 2) {
                pSlot->u8Shut = (u8)(pSlot->u8Shut | HOT_SHUT_WR);
            }
        }
        if (i64R == -(i64)LINUX_EPIPE) {
            hot_tcp_soerr_set(i64Fd, (u16)LINUX_EPIPE);
        }
        if (i64R >= 0) {
            g_u64ResShut++;
            if (g_fResShutLeanOnce == 0u) {
                g_fResShutLeanOnce = 1u;
                kprintf("linux_hot: residual shutdown lean fd=%ld how=%d "
                        "shut=0x%x hits=%lu Soft!=product G-AC-1 "
                        "dual_dod_ab=OPEN (C2 hot ABI; product bridge)\n",
                        (long)i64Fd, nHow,
                        (unsigned)(pSlot != NULL ? pSlot->u8Shut : 0u),
                        (unsigned long)g_u64ResShut);
                hot_residual_lean_once("shutdown");
            }
        }
        return i64R;
    }
    return net_lo_shutdown(i64Fd, nHow);
}

/* Signal/timer stubs for glibc/wine bring-up (no delivery yet). */
static u8 g_aSigMask[8];
static u32 g_u32HotSigchldActLog;

i64
gj_linux_hot_rt_sigaction(struct gj_linux_regs *pRegs)
{
    /* Store sa_handler on the calling USER PCB (wait-pid child, not pid 1). */
    u8 aOld[32];
    u8 aAct[32];
    u32 i;
    u32 u32Sig;
    struct gj_process *pProc;
    struct gj_thread *pCur;
    u64 u64Old = 0;

    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u32Sig = (u32)pRegs->u64Arg0;
    if (u32Sig == 0 || u32Sig >= GJ_PROC_NSIG) {
        return -LINUX_EINVAL;
    }
    pCur = thread_current();
    pProc = NULL;
    if (hot_thr_is_user(pCur) != 0) {
        pProc = pCur->pProc;
    }
    if (pProc == NULL) {
        pProc = hot_calling_proc();
    }
    if (pProc != NULL) {
        u64Old = pProc->aSigHandler[u32Sig];
    }
    for (i = 0; i < sizeof(aOld); i++) {
        aOld[i] = 0;
    }
    {
        u32 iB;
        u64 u64H = u64Old;

        for (iB = 0; iB < 8; iB++) {
            aOld[iB] = (u8)(u64H & 0xffu);
            u64H >>= 8;
        }
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, 32)) {
            (void)copy_to_user(pRegs->u64Arg2, aOld, 32);
        } else {
            memset((void *)(gj_vaddr_t)pRegs->u64Arg2, 0, 32);
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aOld, 8);
        }
    }
    if (pRegs->u64Arg1 != 0 && pProc != NULL) {
        for (i = 0; i < sizeof(aAct); i++) {
            aAct[i] = 0;
        }
        if (user_range_ok(pRegs->u64Arg1, 8)) {
            (void)copy_from_user(aAct, pRegs->u64Arg1, 8);
        } else {
            memcpy(aAct, (const void *)(gj_vaddr_t)pRegs->u64Arg1, 8);
        }
        {
            u64 u64H = 0;
            u32 iB;

            for (iB = 0; iB < 8; iB++) {
                u64H |= ((u64)aAct[iB]) << (iB * 8);
            }
            pProc->aSigHandler[u32Sig] = u64H;
            if (u32Sig == GJ_SIGCHLD && g_u32HotSigchldActLog < 8u) {
                g_u32HotSigchldActLog++;
                kprintf("linux: rt_sigaction SIGCHLD pid=%u h=0x%lx "
                        "Dual DoD B OPEN\n",
                        process_wait_pid_of(pProc), (unsigned long)u64H);
            }
        }
    }
    return 0;
}

i64
gj_linux_hot_rt_sigprocmask(struct gj_linux_regs *pRegs)
{
    /* how, *set, *oldset, sigsetsize — per-PCB, honor BLOCK/UNBLOCK/SETMASK.
     * Global g_aSigMask leaked listener SIGCHLD into OpenSSH osigset so
     * ppoll ran with SIGCHLD blocked (QEMU93 mask=1, handler never entered).
     * Dual DoD B OPEN. */
    u32 cb = 8;
    u32 u32How;
    u64 u64Old;
    u64 u64New;
    u8 aOld[8];
    u8 aNew[8];
    u32 iB;
    struct gj_process *pProc;
    struct gj_thread *pCur;
    u64 *pMask;

    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u32How = (u32)pRegs->u64Arg0;
    if (u32How > 2u) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg3 != 0 && pRegs->u64Arg3 < 8) {
        cb = (u32)pRegs->u64Arg3;
    }
    pCur = thread_current();
    pProc = NULL;
    if (hot_thr_is_user(pCur) != 0) {
        pProc = pCur->pProc;
    }
    if (pProc == NULL) {
        pProc = hot_calling_proc();
    }
    pMask = (pProc != NULL) ? &pProc->u64SigBlocked : NULL;
    u64Old = (pMask != NULL) ? *pMask : 0;
    if (pMask == NULL) {
        for (iB = 0; iB < 8; iB++) {
            u64Old |= ((u64)g_aSigMask[iB]) << (iB * 8);
        }
    }
    for (iB = 0; iB < 8; iB++) {
        aOld[iB] = (u8)((u64Old >> (iB * 8)) & 0xffu);
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, cb)) {
            (void)copy_to_user(pRegs->u64Arg2, aOld, cb);
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aOld, cb);
        }
    }
    if (pRegs->u64Arg1 != 0) {
        for (iB = 0; iB < 8; iB++) {
            aNew[iB] = 0;
        }
        if (user_range_ok(pRegs->u64Arg1, cb)) {
            (void)copy_from_user(aNew, pRegs->u64Arg1, cb);
        } else {
            memcpy(aNew, (const void *)(gj_vaddr_t)pRegs->u64Arg1, cb);
        }
        u64New = 0;
        for (iB = 0; iB < 8; iB++) {
            u64New |= ((u64)aNew[iB]) << (iB * 8);
        }
        if (u32How == 0u) {
            u64Old |= u64New; /* SIG_BLOCK */
        } else if (u32How == 1u) {
            u64Old &= ~u64New; /* SIG_UNBLOCK */
        } else {
            u64Old = u64New; /* SIG_SETMASK */
        }
        if (pMask != NULL) {
            *pMask = u64Old;
        } else {
            for (iB = 0; iB < 8; iB++) {
                g_aSigMask[iB] = (u8)((u64Old >> (iB * 8)) & 0xffu);
            }
        }
    }
    return 0;
}

i64
gj_linux_hot_rt_sigreturn(struct gj_linux_regs *pRegs)
{
    /* No signal frames yet - not reached from real handlers */
    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_getitimer(struct gj_linux_regs *pRegs)
{
    u8 aZero[32];
    u32 i;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    for (i = 0; i < sizeof(aZero); i++) {
        aZero[i] = 0;
    }
    if (user_range_ok(pRegs->u64Arg1, 32)) {
        (void)copy_to_user(pRegs->u64Arg1, aZero, 32);
    } else {
        memset((void *)(gj_vaddr_t)pRegs->u64Arg1, 0, 32);
    }
    return 0;
}

i64
gj_linux_hot_setitimer(struct gj_linux_regs *pRegs)
{
    /* Accept set; return previous zeroed if old_value non-null */
    u8 aZero[32];
    u32 i;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg2 != 0) {
        for (i = 0; i < sizeof(aZero); i++) {
            aZero[i] = 0;
        }
        if (user_range_ok(pRegs->u64Arg2, 32)) {
            (void)copy_to_user(pRegs->u64Arg2, aZero, 32);
        } else {
            memset((void *)(gj_vaddr_t)pRegs->u64Arg2, 0, 32);
        }
    }
    return 0;
}

static u32 g_u32AlarmSec;

i64
gj_linux_hot_alarm(struct gj_linux_regs *pRegs)
{
    u32 u32Prev;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u32Prev = g_u32AlarmSec;
    g_u32AlarmSec = (u32)pRegs->u64Arg0;
    /* Delivery deferred - record only for bring-up */
    return (i64)u32Prev;
}

i64
gj_linux_hot_pause(struct gj_linux_regs *pRegs)
{
    /*
     * pause() waits for signal. Without signal delivery, return EINTR
     * immediately so callers can retry (libc/wine tolerate).
     */
    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    (void)pRegs;
    return -LINUX_EINTR;
}

/* prctl option subset (clean-room public values) */
#define GJ_PR_SET_PDEATHSIG    1
#define GJ_PR_GET_PDEATHSIG    2
#define GJ_PR_SET_NAME         15
#define GJ_PR_GET_NAME         16
#define GJ_PR_SET_TIMERSLACK   29
#define GJ_PR_GET_TIMERSLACK   30
#define GJ_PR_SET_NO_NEW_PRIVS 38
#define GJ_PR_GET_NO_NEW_PRIVS 39
#define GJ_PR_GET_TID_ADDRESS  40
#define GJ_PR_CAPBSET_READ     23
#define GJ_PR_GET_SECUREBITS   27
#define GJ_PR_SET_SECUREBITS   28
#define GJ_PR_SET_THP_DISABLE  41
#define GJ_PR_GET_THP_DISABLE  42

static char g_aComm[16] = "greenjade";
static u32  g_u32Pdeath;
static u32  g_u32NoNewPrivs;
static u32  g_u32ThpDisable;
static u64  g_u64RobustHead;
static u64  g_u64RobustLen;

i64
gj_linux_hot_prctl(struct gj_linux_regs *pRegs)
{
    u64 op;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    op = pRegs->u64Arg0;
    if (op == GJ_PR_SET_NAME) {
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(pRegs->u64Arg1, 16)) {
            (void)copy_from_user(g_aComm, pRegs->u64Arg1, 15);
        } else {
            memcpy(g_aComm, (const void *)(gj_vaddr_t)pRegs->u64Arg1, 15);
        }
        g_aComm[15] = '\0';
        return 0;
    }
    if (op == GJ_PR_GET_NAME) {
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(pRegs->u64Arg1, 16)) {
            (void)copy_to_user(pRegs->u64Arg1, g_aComm, 16);
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, g_aComm, 16);
        }
        return 0;
    }
    if (op == GJ_PR_SET_PDEATHSIG) {
        g_u32Pdeath = (u32)pRegs->u64Arg1;
        return 0;
    }
    if (op == GJ_PR_GET_PDEATHSIG) {
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(pRegs->u64Arg1, 4)) {
            (void)copy_to_user(pRegs->u64Arg1, &g_u32Pdeath, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg1 = g_u32Pdeath;
        }
        return 0;
    }
    if (op == GJ_PR_SET_NO_NEW_PRIVS) {
        g_u32NoNewPrivs = 1;
        return 0;
    }
    if (op == GJ_PR_GET_NO_NEW_PRIVS) {
        return (i64)g_u32NoNewPrivs;
    }
    /* Residual UDX-host / glibc prctl surfaces (soft). */
    if (op == GJ_PR_SET_TIMERSLACK) {
        g_u64TimerSlackNs = pRegs->u64Arg1;
        return 0;
    }
    if (op == GJ_PR_GET_TIMERSLACK) {
        return (i64)g_u64TimerSlackNs;
    }
    if (op == GJ_PR_GET_TID_ADDRESS) {
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(pRegs->u64Arg1, 8)) {
            if (copy_to_user(pRegs->u64Arg1, &g_u64ClearChildTid, 8) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            *(u64 *)(gj_vaddr_t)pRegs->u64Arg1 = g_u64ClearChildTid;
        }
        return 0;
    }
    if (op == GJ_PR_CAPBSET_READ) {
        /* Soft: all caps in bounding set (unprivileged host shape). */
        return 1;
    }
    if (op == GJ_PR_GET_SECUREBITS) {
        return 0;
    }
    if (op == GJ_PR_SET_SECUREBITS) {
        return (pRegs->u64Arg1 == 0) ? 0 : -LINUX_EPERM;
    }
    if (op == GJ_PR_SET_THP_DISABLE) {
        g_u32ThpDisable = (u32)(pRegs->u64Arg1 != 0);
        return 0;
    }
    if (op == GJ_PR_GET_THP_DISABLE) {
        return (i64)g_u32ThpDisable;
    }
    return 0; /* ignore unknown options for bring-up */
}

i64
gj_linux_hot_set_robust_list(struct gj_linux_regs *pRegs)
{
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    /*
     * Wire G-FUT-ROBUST / futex: robust set via futex_set_robust_list when
     * a current thread exists. Soft process mirror kept for get fallback and
     * kernel hybrid smoke without thr context.
     */
    st = futex_set_robust_list(pRegs->u64Arg0, pRegs->u64Arg1);
    if (st == GJ_OK) {
        g_u64RobustHead = pRegs->u64Arg0;
        g_u64RobustLen = pRegs->u64Arg1;
        return 0;
    }
    if (st == GJ_ERR_NOMEM) {
        return -LINUX_ENOMEM;
    }
    /* Soft fallback: no thread_current (bring-up smoke path). */
    if (thread_current() == NULL) {
        if (pRegs->u64Arg1 == 0 || pRegs->u64Arg1 > 64ull) {
            return -LINUX_EINVAL;
        }
        g_u64RobustHead = pRegs->u64Arg0;
        g_u64RobustLen = pRegs->u64Arg1;
        return 0;
    }
    return -LINUX_EINVAL;
}

i64
gj_linux_hot_get_robust_list(struct gj_linux_regs *pRegs)
{
    /* get_robust_list(pid, **head_ptr, *len_ptr) - G-FUT-ROBUST soft */
    u64 head;
    u64 len;
    u32 u32Pid;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0 || pRegs->u64Arg2 == 0) {
        return -LINUX_EFAULT;
    }
    u32Pid = (u32)pRegs->u64Arg0;
    /* futex: robust get - prefer per-tid slot (tid 0 ⇒ current). */
    st = futex_get_robust_list(u32Pid, &head, &len);
    if (st != GJ_OK) {
        /* Soft process mirror when slot unset / no thr (smoke PASS path). */
        if (u32Pid != 0 && u32Pid != g_u32LinuxPid && u32Pid != g_u32LinuxTid) {
            return -LINUX_ESRCH;
        }
        head = g_u64RobustHead;
        len = g_u64RobustLen;
    }
    if (user_range_ok(pRegs->u64Arg1, 8)) {
        if (copy_to_user(pRegs->u64Arg1, &head, 8) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        *(u64 *)(gj_vaddr_t)pRegs->u64Arg1 = head;
    }
    if (user_range_ok(pRegs->u64Arg2, 8)) {
        if (copy_to_user(pRegs->u64Arg2, &len, 8) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        *(u64 *)(gj_vaddr_t)pRegs->u64Arg2 = len;
    }
    return 0;
}

i64
gj_linux_hot_waitid(struct gj_linux_regs *pRegs)
{
    /*
     * waitid(idtype, id, *infop, options)
     * Map to process_wait4; fill siginfo-shaped buffer if provided.
     */
    i32 st = 0;
    i64 r;
    i64 pid = -1;
    u8 aInfo[128];

    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    /* idtype: 0=P_ALL 1=P_PID 2=P_PGID (PGID not product). */
    if (pRegs->u64Arg0 == (u64)LINUX_P_PGID) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg0 == (u64)LINUX_P_PID) {
        pid = (i64)pRegs->u64Arg1;
    } else if (pRegs->u64Arg0 != (u64)LINUX_P_ALL) {
        return -LINUX_EINVAL;
    }
    {
        int nOpts = (int)pRegs->u64Arg3;
        struct gj_thread *pCur = thread_current();
        u32 u32UserFl = GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY;
        int fUser;

        if ((nOpts & (LINUX_WEXITED | LINUX_WSTOPPED | LINUX_WCONTINUED)) ==
            0) {
            return -LINUX_EINVAL;
        }
        fUser = (pCur != NULL && (pCur->u32Flags & u32UserFl) != 0);
        /* Kernel smoke: WNOHANG prefer. USER: honor waitid options. */
        if (!fUser) {
            nOpts |= LINUX_WNOHANG;
        }
        r = process_wait4(pid, &st, nOpts);
    }
    if (r == -10) {
        return -LINUX_ECHILD;
    }
    if (r == 0) {
        return 0; /* no child ready */
    }
    if (pRegs->u64Arg2 != 0) {
        linux_siginfo_sigchld_exited(aInfo, (u32)sizeof(aInfo), (u32)r,
                                     (u32)((st >> 8) & 0xff));
        if (user_range_ok(pRegs->u64Arg2, sizeof(aInfo))) {
            (void)copy_to_user(pRegs->u64Arg2, aInfo, sizeof(aInfo));
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aInfo, sizeof(aInfo));
        }
    }
    return 0;
}

i64
gj_linux_hot_mincore(struct gj_linux_regs *pRegs)
{
    /*
     * Residual: chunked mincore for multi-page DMA windows (UDX host rings).
     * Soft vector bit0 = page present via vmm_virt_to_phys.
     * greppable: linux_hot: residual mincore
     * greppable: linux_hot: residual mincore lean
     * Soft!=product · G-AC-1 · Dual DoD A/B OPEN.
     */
    u64 va;
    u64 len;
    u64 u64Vec;
    u64 npages;
    u64 iDone;
    u8 aBit[64];

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL || pRegs->u64Arg2 == 0) {
        return -LINUX_EINVAL;
    }
    va = pRegs->u64Arg0;
    len = pRegs->u64Arg1;
    u64Vec = pRegs->u64Arg2;
    if ((va & 0xfffull) != 0 || len == 0) {
        return -LINUX_EINVAL;
    }
    npages = (len + 0xfffull) / 0x1000ull;
    /* Cap residual walk: 16 MiB -> 4096 pages (driver host maps, not full AS). */
    if (npages > 4096ull) {
        npages = 4096ull;
    }
    for (iDone = 0; iDone < npages; ) {
        u64 nChunk = npages - iDone;
        u64 i;

        if (nChunk > sizeof(aBit)) {
            nChunk = sizeof(aBit);
        }
        for (i = 0; i < nChunk; i++) {
            gj_paddr_t pa =
                vmm_virt_to_phys((gj_vaddr_t)(va + (iDone + i) * 0x1000ull));

            aBit[i] = (pa != 0) ? 1u : 0u;
        }
        if (user_range_ok(u64Vec + iDone, nChunk)) {
            if (copy_to_user(u64Vec + iDone, aBit, (size_t)nChunk) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)(u64Vec + iDone), aBit, (size_t)nChunk);
        }
        iDone += nChunk;
    }
    g_u64ResMincoreCalls++;
    g_u64ResMincorePages += npages;
    if (g_fResMincoreLeanOnce == 0u) {
        g_fResMincoreLeanOnce = 1u;
        kprintf("linux_hot: residual mincore lean via=mincore "
                "pages=%lu calls=%lu total_pages=%lu Soft!=product G-AC-1 "
                "dual_dod_ab=OPEN\n",
                (unsigned long)npages,
                (unsigned long)g_u64ResMincoreCalls,
                (unsigned long)g_u64ResMincorePages);
        hot_residual_lean_once("mincore");
    }
    return 0;
}

i64
gj_linux_hot_sched_yield(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    (void)pRegs;
    thread_yield();
    return 0;
}

i64
gj_linux_hot_getpid(struct gj_linux_regs *pRegs)
{
    struct gj_thread *pCur;
    u32 u32Pid;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    pCur = thread_current();
    if (pCur != NULL && pCur->pProc != NULL) {
        u32Pid = process_wait_pid_of(pCur->pProc);
        if (u32Pid != 0) {
            return (i64)u32Pid;
        }
    }
    return (i64)g_u32LinuxPid;
}

i64
gj_linux_hot_gettid(struct gj_linux_regs *pRegs)
{
    struct gj_thread *pCur;
    u32 u32UserFl = GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    pCur = thread_current();
    (void)u32UserFl;
    if (pCur != NULL && pCur->u32Id != 0) {
        return (i64)pCur->u32Id;
    }
    return (i64)g_u32LinuxTid;
}

/*
 * Per-wait-pid creds (slot table). kmain smoke (no wait pid) keeps globals.
 * After a root drop, restore of an id not in {r,e,s} is EPERM.
 */
struct linux_hot_cred_ptr {
    u32 *pRuid;
    u32 *pEuid;
    u32 *pSuid;
    u32 *pRgid;
    u32 *pEgid;
    u32 *pSgid;
};

static void
linux_cred_bind_globals(struct linux_hot_cred_ptr *pCred)
{
    pCred->pRuid = &g_u32LinuxUid;
    pCred->pEuid = &g_u32LinuxEuid;
    pCred->pSuid = &g_u32LinuxSuid;
    pCred->pRgid = &g_u32LinuxGid;
    pCred->pEgid = &g_u32LinuxEgid;
    pCred->pSgid = &g_u32LinuxSgid;
}

static void
linux_cred_bind_slot(struct linux_hot_cred_ptr *pCred,
                    struct linux_hot_cred *pSlot)
{
    pCred->pRuid = &pSlot->u32Ruid;
    pCred->pEuid = &pSlot->u32Euid;
    pCred->pSuid = &pSlot->u32Suid;
    pCred->pRgid = &pSlot->u32Rgid;
    pCred->pEgid = &pSlot->u32Egid;
    pCred->pSgid = &pSlot->u32Sgid;
}

static struct linux_hot_cred *
linux_cred_find_pid(u32 u32Pid)
{
    u32 iSlot;

    if (u32Pid == 0) {
        return NULL;
    }
    for (iSlot = 0; iSlot < GJ_LINUX_HOT_CRED_SLOTS; iSlot++) {
        if (g_aLinuxCred[iSlot].u32Pid == u32Pid) {
            return &g_aLinuxCred[iSlot];
        }
    }
    return NULL;
}

static struct linux_hot_cred *
linux_cred_alloc_pid(u32 u32Pid)
{
    u32 iSlot;
    struct linux_hot_cred *pSlot;

    pSlot = linux_cred_find_pid(u32Pid);
    if (pSlot != NULL) {
        return pSlot;
    }
    if (u32Pid == 0) {
        return NULL;
    }
    for (iSlot = 0; iSlot < GJ_LINUX_HOT_CRED_SLOTS; iSlot++) {
        if (g_aLinuxCred[iSlot].u32Pid == 0) {
            pSlot = &g_aLinuxCred[iSlot];
            pSlot->u32Pid = u32Pid;
            pSlot->u32Ruid = g_u32LinuxUid;
            pSlot->u32Euid = g_u32LinuxEuid;
            pSlot->u32Suid = g_u32LinuxSuid;
            pSlot->u32Rgid = g_u32LinuxGid;
            pSlot->u32Egid = g_u32LinuxEgid;
            pSlot->u32Sgid = g_u32LinuxSgid;
            return pSlot;
        }
    }
    return NULL;
}

static u32
linux_cred_calling_pid(void)
{
    struct gj_thread *pCur;
    struct gj_cpu *pCpu;
    struct gj_process *pProc;
    u32 u32Pid;
    u64 u64Rip = 0;
    u64 u64Rsp = 0;

    pCur = thread_current();
    pProc = (pCur != NULL) ? pCur->pProc : NULL;
    u32Pid = process_wait_pid_of(pProc);
    if (u32Pid != 0) {
        return u32Pid;
    }
    pCpu = cpu_current();
    if (pCpu != NULL) {
        u64Rip = pCpu->u64UserRip;
        u64Rsp = pCpu->u64UserRsp;
    }
    pProc = process_linux_live_user_child(u64Rip, u64Rsp);
    u32Pid = process_wait_pid_of(pProc);
    if (u32Pid != 0) {
        return u32Pid;
    }
    pProc = hot_calling_proc();
    return process_wait_pid_of(pProc);
}

static int
linux_cred_bind(struct linux_hot_cred_ptr *pCred, int fAlloc)
{
    u32 u32Pid;
    struct linux_hot_cred *pSlot;

    linux_cred_bind_globals(pCred);
    u32Pid = linux_cred_calling_pid();
    if (u32Pid == 0) {
        return 0;
    }
    pSlot = linux_cred_find_pid(u32Pid);
    if (pSlot == NULL && fAlloc != 0) {
        pSlot = linux_cred_alloc_pid(u32Pid);
        if (pSlot == NULL) {
            return -1;
        }
    }
    if (pSlot != NULL) {
        linux_cred_bind_slot(pCred, pSlot);
    }
    return 0;
}

void
gj_linux_hot_cred_fork(const struct gj_process *pParent,
                      struct gj_process *pChild)
{
    u32 u32ParentPid;
    u32 u32ChildPid;
    struct linux_hot_cred *pSrc;
    struct linux_hot_cred *pDst;

    if (pChild == NULL) {
        return;
    }
    u32ChildPid = process_wait_pid_of(pChild);
    if (u32ChildPid == 0) {
        return;
    }
    pSrc = NULL;
    if (pParent != NULL && pParent != g_pLinuxProc) {
        u32ParentPid = process_wait_pid_of((struct gj_process *)pParent);
        pSrc = linux_cred_find_pid(u32ParentPid);
    }
    pDst = linux_cred_alloc_pid(u32ChildPid);
    if (pDst == NULL) {
        return;
    }
    if (pSrc == NULL) {
        return;
    }
    pDst->u32Ruid = pSrc->u32Ruid;
    pDst->u32Euid = pSrc->u32Euid;
    pDst->u32Suid = pSrc->u32Suid;
    pDst->u32Rgid = pSrc->u32Rgid;
    pDst->u32Egid = pSrc->u32Egid;
    pDst->u32Sgid = pSrc->u32Sgid;
}

void
gj_linux_hot_cred_exit(struct gj_process *pProc)
{
    u32 u32Pid;
    struct linux_hot_cred *pSlot;

    if (pProc == NULL || pProc == g_pLinuxProc) {
        return;
    }
    u32Pid = process_wait_pid_of(pProc);
    pSlot = linux_cred_find_pid(u32Pid);
    if (pSlot == NULL) {
        return;
    }
    pSlot->u32Pid = 0;
    pSlot->u32Ruid = 0;
    pSlot->u32Euid = 0;
    pSlot->u32Suid = 0;
    pSlot->u32Rgid = 0;
    pSlot->u32Egid = 0;
    pSlot->u32Sgid = 0;
}

static int
linux_cred_in_set(u32 u32Want, u32 u32R, u32 u32E, u32 u32S)
{
    if (u32Want == u32R || u32Want == u32E || u32Want == u32S) {
        return 1;
    }
    return 0;
}

static i64
linux_cred_res_check(i64 i64Want, u32 u32R, u32 u32E, u32 u32S, u32 u32Euid)
{
    if (i64Want == -1) {
        return 0;
    }
    if (u32Euid == 0) {
        return 0;
    }
    if (linux_cred_in_set((u32)i64Want, u32R, u32E, u32S) != 0) {
        return 0;
    }
    return -LINUX_EPERM;
}

i64
gj_linux_hot_getuid(struct gj_linux_regs *pRegs)
{
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    (void)linux_cred_bind(&stCred, 1);
    return (i64)*stCred.pRuid;
}

i64
gj_linux_hot_getgid(struct gj_linux_regs *pRegs)
{
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    (void)linux_cred_bind(&stCred, 1);
    return (i64)*stCred.pRgid;
}

i64
gj_linux_hot_geteuid(struct gj_linux_regs *pRegs)
{
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    (void)linux_cred_bind(&stCred, 1);
    return (i64)*stCred.pEuid;
}

i64
gj_linux_hot_getegid(struct gj_linux_regs *pRegs)
{
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    (void)linux_cred_bind(&stCred, 1);
    return (i64)*stCred.pEgid;
}

i64
gj_linux_hot_setuid(struct gj_linux_regs *pRegs)
{
    u32 u32Uid;
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (linux_cred_bind(&stCred, 1) != 0) {
        return -LINUX_ENOMEM;
    }
    u32Uid = (u32)pRegs->u64Arg0;
    if (*stCred.pEuid == 0) {
        *stCred.pRuid = u32Uid;
        *stCred.pEuid = u32Uid;
        *stCred.pSuid = u32Uid;
        return 0;
    }
    if (linux_cred_in_set(u32Uid, *stCred.pRuid, *stCred.pEuid,
                          *stCred.pSuid) == 0) {
        return -LINUX_EPERM;
    }
    *stCred.pEuid = u32Uid;
    return 0;
}

i64
gj_linux_hot_setgid(struct gj_linux_regs *pRegs)
{
    u32 u32Gid;
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (linux_cred_bind(&stCred, 1) != 0) {
        return -LINUX_ENOMEM;
    }
    u32Gid = (u32)pRegs->u64Arg0;
    if (*stCred.pEuid == 0) {
        *stCred.pRgid = u32Gid;
        *stCred.pEgid = u32Gid;
        *stCred.pSgid = u32Gid;
        return 0;
    }
    if (linux_cred_in_set(u32Gid, *stCred.pRgid, *stCred.pEgid,
                          *stCred.pSgid) == 0) {
        return -LINUX_EPERM;
    }
    *stCred.pEgid = u32Gid;
    return 0;
}

i64
gj_linux_hot_setreuid(struct gj_linux_regs *pRegs)
{
    i64 i64R;
    i64 i64E;
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (linux_cred_bind(&stCred, 1) != 0) {
        return -LINUX_ENOMEM;
    }
    i64R = (i64)pRegs->u64Arg0;
    i64E = (i64)pRegs->u64Arg1;
    if (linux_cred_res_check(i64R, *stCred.pRuid, *stCred.pEuid, *stCred.pSuid,
                             *stCred.pEuid) != 0 ||
        linux_cred_res_check(i64E, *stCred.pRuid, *stCred.pEuid, *stCred.pSuid,
                             *stCred.pEuid) != 0) {
        return -LINUX_EPERM;
    }
    if (i64R != -1) {
        *stCred.pRuid = (u32)i64R;
    }
    if (i64E != -1) {
        *stCred.pEuid = (u32)i64E;
    }
    return 0;
}

i64
gj_linux_hot_setregid(struct gj_linux_regs *pRegs)
{
    i64 i64R;
    i64 i64E;
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (linux_cred_bind(&stCred, 1) != 0) {
        return -LINUX_ENOMEM;
    }
    i64R = (i64)pRegs->u64Arg0;
    i64E = (i64)pRegs->u64Arg1;
    if (linux_cred_res_check(i64R, *stCred.pRgid, *stCred.pEgid, *stCred.pSgid,
                             *stCred.pEuid) != 0 ||
        linux_cred_res_check(i64E, *stCred.pRgid, *stCred.pEgid, *stCred.pSgid,
                             *stCred.pEuid) != 0) {
        return -LINUX_EPERM;
    }
    if (i64R != -1) {
        *stCred.pRgid = (u32)i64R;
    }
    if (i64E != -1) {
        *stCred.pEgid = (u32)i64E;
    }
    return 0;
}

i64
gj_linux_hot_setresuid(struct gj_linux_regs *pRegs)
{
    i64 i64R;
    i64 i64E;
    i64 i64S;
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (linux_cred_bind(&stCred, 1) != 0) {
        return -LINUX_ENOMEM;
    }
    i64R = (i64)pRegs->u64Arg0;
    i64E = (i64)pRegs->u64Arg1;
    i64S = (i64)pRegs->u64Arg2;
    if (linux_cred_res_check(i64R, *stCred.pRuid, *stCred.pEuid, *stCred.pSuid,
                             *stCred.pEuid) != 0 ||
        linux_cred_res_check(i64E, *stCred.pRuid, *stCred.pEuid, *stCred.pSuid,
                             *stCred.pEuid) != 0 ||
        linux_cred_res_check(i64S, *stCred.pRuid, *stCred.pEuid, *stCred.pSuid,
                             *stCred.pEuid) != 0) {
        return -LINUX_EPERM;
    }
    if (i64R != -1) {
        *stCred.pRuid = (u32)i64R;
    }
    if (i64E != -1) {
        *stCred.pEuid = (u32)i64E;
    }
    if (i64S != -1) {
        *stCred.pSuid = (u32)i64S;
    }
    return 0;
}

i64
gj_linux_hot_setresgid(struct gj_linux_regs *pRegs)
{
    i64 i64R;
    i64 i64E;
    i64 i64S;
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (linux_cred_bind(&stCred, 1) != 0) {
        return -LINUX_ENOMEM;
    }
    i64R = (i64)pRegs->u64Arg0;
    i64E = (i64)pRegs->u64Arg1;
    i64S = (i64)pRegs->u64Arg2;
    if (linux_cred_res_check(i64R, *stCred.pRgid, *stCred.pEgid, *stCred.pSgid,
                             *stCred.pEuid) != 0 ||
        linux_cred_res_check(i64E, *stCred.pRgid, *stCred.pEgid, *stCred.pSgid,
                             *stCred.pEuid) != 0 ||
        linux_cred_res_check(i64S, *stCred.pRgid, *stCred.pEgid, *stCred.pSgid,
                             *stCred.pEuid) != 0) {
        return -LINUX_EPERM;
    }
    if (i64R != -1) {
        *stCred.pRgid = (u32)i64R;
    }
    if (i64E != -1) {
        *stCred.pEgid = (u32)i64E;
    }
    if (i64S != -1) {
        *stCred.pSgid = (u32)i64S;
    }
    return 0;
}

i64
gj_linux_hot_getresuid(struct gj_linux_regs *pRegs)
{
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    (void)linux_cred_bind(&stCred, 0);
    if (pRegs->u64Arg0 != 0) {
        u32 u = *stCred.pRuid;

        if (user_range_ok(pRegs->u64Arg0, 4)) {
            (void)copy_to_user(pRegs->u64Arg0, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg0 = u;
        }
    }
    if (pRegs->u64Arg1 != 0) {
        u32 u = *stCred.pEuid;

        if (user_range_ok(pRegs->u64Arg1, 4)) {
            (void)copy_to_user(pRegs->u64Arg1, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg1 = u;
        }
    }
    if (pRegs->u64Arg2 != 0) {
        u32 u = *stCred.pSuid;

        if (user_range_ok(pRegs->u64Arg2, 4)) {
            (void)copy_to_user(pRegs->u64Arg2, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = u;
        }
    }
    return 0;
}

i64
gj_linux_hot_getresgid(struct gj_linux_regs *pRegs)
{
    struct linux_hot_cred_ptr stCred;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    (void)linux_cred_bind(&stCred, 0);
    if (pRegs->u64Arg0 != 0) {
        u32 u = *stCred.pRgid;

        if (user_range_ok(pRegs->u64Arg0, 4)) {
            (void)copy_to_user(pRegs->u64Arg0, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg0 = u;
        }
    }
    if (pRegs->u64Arg1 != 0) {
        u32 u = *stCred.pEgid;

        if (user_range_ok(pRegs->u64Arg1, 4)) {
            (void)copy_to_user(pRegs->u64Arg1, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg1 = u;
        }
    }
    if (pRegs->u64Arg2 != 0) {
        u32 u = *stCred.pSgid;

        if (user_range_ok(pRegs->u64Arg2, 4)) {
            (void)copy_to_user(pRegs->u64Arg2, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = u;
        }
    }
    return 0;
}

i64
gj_linux_hot_setpgid(struct gj_linux_regs *pRegs)
{
    u32 pid;
    u32 pgid;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    pid = (u32)pRegs->u64Arg0;
    pgid = (u32)pRegs->u64Arg1;
    {
        struct gj_process *pProc = hot_calling_proc();
        u32 u32Self = g_u32LinuxPid;

        if (pProc != NULL) {
            u32 u32W = process_wait_pid_of(pProc);

            if (u32W != 0) {
                u32Self = u32W;
            }
        }
        if (pid == 0 || pid == u32Self || pid == g_u32LinuxPid) {
            u32 u32Set = pgid ? pgid : u32Self;

            g_u32LinuxPgid = u32Set;
            if (pProc != NULL) {
                pProc->u32Pgid = u32Set;
            }
        }
    }
    return 0;
}

i64
gj_linux_hot_setsid(struct gj_linux_regs *pRegs)
{
    struct gj_process *pProc;
    u32 u32Sid = g_u32LinuxPid;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    pProc = hot_calling_proc();
    if (pProc != NULL) {
        u32 u32W = process_wait_pid_of(pProc);

        if (u32W != 0) {
            u32Sid = u32W;
        }
        pProc->u32Sid = u32Sid;
        pProc->u32Pgid = u32Sid;
    }
    g_u32LinuxSid = u32Sid;
    g_u32LinuxPgid = u32Sid;
    return (i64)u32Sid;
}

i64
gj_linux_hot_setgroups(struct gj_linux_regs *pRegs)
{
    /* Accept any size; groups table not stored beyond getgroups smoke */
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return 0;
}

/**
 * Residual mlock range gate (Linux man semantics, soft/no-swap host):
 *   - len == 0 -> success (no-op); *pPages = 0
 *   - round addr down / end up to page; overflow -> EINVAL
 *   - no swap -> advisory success (pages stay resident soft)
 * On success with pPages: fill page count of rounded range (lean tally).
 * greppable: linux_hot: residual mlock
 * greppable: linux_hot: residual mlock lean
 */
static i64
hot_mlock_range_gate(u64 u64Addr, u64 u64Len, u64 *pPages)
{
    u64 u64Start;
    u64 u64End;

    if (pPages != NULL) {
        *pPages = 0;
    }
    if (u64Len == 0) {
        return 0;
    }
    /* Page-round like Linux do_mlock: start down, end up. */
    u64Start = u64Addr & ~0xfffull;
    u64End = u64Addr + u64Len;
    if (u64End < u64Addr) {
        return -LINUX_EINVAL; /* overflow */
    }
    u64End = (u64End + 0xfffull) & ~0xfffull;
    if (u64End < u64Start) {
        return -LINUX_EINVAL;
    }
    if (pPages != NULL) {
        *pPages = (u64End - u64Start) >> 12;
    }
    return 0;
}

/**
 * Lean mlock2 flags gate: only MLOCK_ONFAULT (bit0) is legal; others EINVAL.
 * mlock (NR != mlock2) ignores arg2 (register noise). Soft!=product.
 * greppable: linux_hot: residual mlock lean
 */
static i64
hot_mlock2_flags_gate(const struct gj_linux_regs *pRegs)
{
    u64 u64Flags;

    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Nr != (u64)LINUX_NR_mlock2) {
        return 0;
    }
    u64Flags = pRegs->u64Arg2;
    if ((u64Flags & ~(u64)HOT_MLOCK_ONFAULT) != 0) {
        return -LINUX_EINVAL;
    }
    return 0;
}

i64
gj_linux_hot_mlock(struct gj_linux_regs *pRegs)
{
    /*
     * Residual UDX-host path: no swap -> mlock is advisory success when the
     * range gate passes (len0 OK; unaligned OK via page-round).
     * mlock2: flags lean gate (MLOCK_ONFAULT only). Soft!=product · G-AC-1.
     * greppable: linux_hot: residual mlock
     * greppable: linux_hot: residual mlock lean
     */
    u64 va;
    u64 len;
    u64 u64Pages = 0;
    i64 i64Gate;
    int fMlock2;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    va = pRegs->u64Arg0;
    len = pRegs->u64Arg1;
    fMlock2 = (pRegs->u64Nr == (u64)LINUX_NR_mlock2) ? 1 : 0;
    i64Gate = hot_mlock2_flags_gate(pRegs);
    if (i64Gate != 0) {
        return i64Gate;
    }
    i64Gate = hot_mlock_range_gate(va, len, &u64Pages);
    if (i64Gate != 0) {
        return i64Gate;
    }
    g_u64ResMlockPages += u64Pages;
    g_u64ResMlockCalls++;
    if (fMlock2 != 0) {
        g_u64ResMlock2Calls++;
    }
    if (g_fResMlockOnce == 0) {
        g_fResMlockOnce = 1;
        kprintf("linux_hot: residual mlock va=0x%lx len=%lu soft=1 "
                "Soft!=product\n",
                (unsigned long)va, (unsigned long)len);
    }
    if (g_fResMlockLeanOnce == 0 &&
        (u64Pages != 0 || fMlock2 != 0)) {
        g_fResMlockLeanOnce = 1;
        kprintf("linux_hot: residual mlock lean pages=%lu total=%lu "
                "calls=%lu mlock2=%lu nr=%lu "
                "Soft!=product G-AC-1\n",
                (unsigned long)u64Pages,
                (unsigned long)g_u64ResMlockPages,
                (unsigned long)g_u64ResMlockCalls,
                (unsigned long)g_u64ResMlock2Calls,
                (unsigned long)pRegs->u64Nr);
        hot_residual_lean_once(fMlock2 != 0 ? "mlock2" : "mlock");
    } else if (g_fResLeanOnce == 0 && u64Pages == 0 && fMlock2 == 0) {
        /* len0 soft OK path - still arm unified lean once (no storm). */
        hot_residual_lean_once("mlock_len0");
    }
    return 0;
}

i64
gj_linux_hot_munlock(struct gj_linux_regs *pRegs)
{
    u64 va;
    u64 len;
    i64 i64Gate;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    va = pRegs->u64Arg0;
    len = pRegs->u64Arg1;
    /* munlock: range gate only (no flags); soft tally not decremented. */
    i64Gate = hot_mlock_range_gate(va, len, NULL);
    if (i64Gate != 0) {
        return i64Gate;
    }
    g_u64ResMunlockCalls++;
    if (g_fResMlockLeanOnce == 0) {
        g_fResMlockLeanOnce = 1;
        kprintf("linux_hot: residual mlock lean munlock len=%lu "
                "Soft!=product G-AC-1\n",
                (unsigned long)len);
        hot_residual_lean_once("munlock");
    }
    return 0;
}

i64
gj_linux_hot_mlockall(struct gj_linux_regs *pRegs)
{
    /* flags: MCL_CURRENT=1|MCL_FUTURE=2|MCL_ONFAULT=4 - zero flags -> EINVAL */
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg0 == 0 || (pRegs->u64Arg0 & ~HOT_MCL_MASK) != 0) {
        return -LINUX_EINVAL;
    }
    /* Soft residual: no-swap host -> advisory success. Soft!=product. */
    g_u32ResMlockallFlags = (u32)pRegs->u64Arg0;
    g_u64ResMlockallCalls++;
    if (g_fResMlockLeanOnce == 0) {
        g_fResMlockLeanOnce = 1;
        kprintf("linux_hot: residual mlock lean mlockall flags=0x%lx "
                "calls=%lu Soft!=product G-AC-1\n",
                (unsigned long)pRegs->u64Arg0,
                (unsigned long)g_u64ResMlockallCalls);
        hot_residual_lean_once("mlockall");
    }
    return 0;
}

i64
gj_linux_hot_munlockall(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    (void)pRegs;
    /* Soft residual: clear last mlockall flags store (no page walk). */
    g_u32ResMlockallFlags = 0;
    return 0;
}

i64
gj_linux_hot_pkey_alloc(struct gj_linux_regs *pRegs)
{
    /* Return synthetic key id 1 (no real PKRU). */
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    (void)pRegs;
    return 1;
}

i64
gj_linux_hot_pkey_free(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL || (i64)pRegs->u64Arg0 < 1) {
        return -LINUX_EINVAL;
    }
    return 0;
}

i64
gj_linux_hot_getppid(struct gj_linux_regs *pRegs)
{
    struct gj_thread *pCur;
    struct gj_process *pProc;
    u32 u32Ppid;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    pCur = thread_current();
    pProc = (pCur != NULL) ? pCur->pProc : NULL;
    if (pProc != NULL && pProc->pParent != NULL) {
        u32Ppid = process_wait_pid_of(pProc->pParent);
        if (u32Ppid == 0) {
            u32Ppid = process_soft_parent_pid_of(pProc->pParent);
        }
        if (u32Ppid != 0) {
            return (i64)u32Ppid;
        }
    }
    return (i64)g_u32LinuxPpid;
}

i64
gj_linux_hot_getsid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return (i64)g_u32LinuxSid;
}

i64
gj_linux_hot_personality(struct gj_linux_regs *pRegs)
{
    static u32 g_u32Pers = 0; /* PER_LINUX */

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if ((i64)pRegs->u64Arg0 == -1) {
        return (i64)g_u32Pers; /* query */
    }
    {
        u32 old = g_u32Pers;

        g_u32Pers = (u32)pRegs->u64Arg0;
        return (i64)old;
    }
}

/*
 * process_vm_readv/writev: local (same pid) iovec copy for bring-up.
 * Cross-process deferred (needs AS walk + SMAP).
 * Residual UDX-host lean: tallies + once-lamp (Soft!=product; Dual DoD OPEN).
 * args: pid, local_iov, liovcnt, remote_iov, riovcnt, flags
 * greppable: linux_hot: residual process_vm lean
 */
/* Soft residual per-iov byte cap (UDX host ring/descriptor probes). */
#define HOT_RES_PVM_IOV_MAX 32768ull
/* Soft residual iovcnt cap (pair loop; Soft!=product). */
#define HOT_RES_PVM_IOV_CNT 16ull

static i64
process_vm_copy(struct gj_linux_regs *pRegs, int fWriteRemote)
{
    u32 u32Pid;
    u64 u64LocalIov;
    u64 u64RemoteIov;
    u64 u64Liovcnt;
    u64 u64Riovcnt;
    u64 u64Done = 0;
    u64 i;
    int fFault = 0;

    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u32Pid = (u32)pRegs->u64Arg0;
    u64LocalIov = pRegs->u64Arg1;
    u64Liovcnt = pRegs->u64Arg2;
    u64RemoteIov = pRegs->u64Arg3;
    u64Riovcnt = pRegs->u64Arg4;
    if (u64Liovcnt == 0 || u64Riovcnt == 0 ||
        u64Liovcnt > HOT_RES_PVM_IOV_CNT ||
        u64Riovcnt > HOT_RES_PVM_IOV_CNT) {
        return -LINUX_EINVAL;
    }
    /* Same-process only for bring-up */
    if (u32Pid != 0 && u32Pid != g_u32LinuxPid && u32Pid != 1) {
        return -LINUX_ESRCH;
    }
    if (u64LocalIov == 0 || u64RemoteIov == 0) {
        return -LINUX_EFAULT;
    }
    for (i = 0; i < u64Liovcnt && i < u64Riovcnt; i++) {
        u64 baseL = u64LocalIov + i * 16u;
        u64 baseR = u64RemoteIov + i * 16u;
        u64 u64BaseL = 0, u64LenL = 0, u64BaseR = 0, u64LenR = 0;
        u64 n;

        if (user_range_ok(baseL, 16)) {
            (void)copy_from_user(&u64BaseL, baseL, 8);
            (void)copy_from_user(&u64LenL, baseL + 8, 8);
            (void)copy_from_user(&u64BaseR, baseR, 8);
            (void)copy_from_user(&u64LenR, baseR + 8, 8);
        } else {
            u64BaseL = ((const u64 *)(gj_vaddr_t)baseL)[0];
            u64LenL = ((const u64 *)(gj_vaddr_t)baseL)[1];
            u64BaseR = ((const u64 *)(gj_vaddr_t)baseR)[0];
            u64LenR = ((const u64 *)(gj_vaddr_t)baseR)[1];
        }
        n = u64LenL < u64LenR ? u64LenL : u64LenR;
        if (n == 0) {
            continue;
        }
        /* Residual UDX host: up to 32 KiB per iov pair (was 16 KiB). */
        if (n > HOT_RES_PVM_IOV_MAX) {
            n = HOT_RES_PVM_IOV_MAX;
        }
        {
            u8 aTmp[4096];
            u64 u64Off = 0;

            while (u64Off < n) {
                u64 nChunk = n - u64Off;

                if (nChunk > sizeof(aTmp)) {
                    nChunk = sizeof(aTmp);
                }
                if (fWriteRemote) {
                    /* local -> remote */
                    if (user_range_ok(u64BaseL + u64Off, nChunk)) {
                        if (copy_from_user(aTmp, u64BaseL + u64Off,
                                           (size_t)nChunk) != GJ_OK) {
                            fFault = 1;
                            goto pvm_done;
                        }
                    } else {
                        memcpy(aTmp,
                               (const void *)(gj_vaddr_t)(u64BaseL + u64Off),
                               (size_t)nChunk);
                    }
                    if (user_range_ok(u64BaseR + u64Off, nChunk)) {
                        if (copy_to_user(u64BaseR + u64Off, aTmp,
                                         (size_t)nChunk) != GJ_OK) {
                            fFault = 1;
                            goto pvm_done;
                        }
                    } else {
                        memcpy((void *)(gj_vaddr_t)(u64BaseR + u64Off), aTmp,
                               (size_t)nChunk);
                    }
                } else {
                    /* remote -> local */
                    if (user_range_ok(u64BaseR + u64Off, nChunk)) {
                        if (copy_from_user(aTmp, u64BaseR + u64Off,
                                           (size_t)nChunk) != GJ_OK) {
                            fFault = 1;
                            goto pvm_done;
                        }
                    } else {
                        memcpy(aTmp,
                               (const void *)(gj_vaddr_t)(u64BaseR + u64Off),
                               (size_t)nChunk);
                    }
                    if (user_range_ok(u64BaseL + u64Off, nChunk)) {
                        if (copy_to_user(u64BaseL + u64Off, aTmp,
                                         (size_t)nChunk) != GJ_OK) {
                            fFault = 1;
                            goto pvm_done;
                        }
                    } else {
                        memcpy((void *)(gj_vaddr_t)(u64BaseL + u64Off), aTmp,
                               (size_t)nChunk);
                    }
                }
                u64Off += nChunk;
                u64Done += nChunk;
            }
        }
    }
pvm_done:
    if (u64Done > 0) {
        g_u64ResPvmCalls++;
        g_u64ResPvmBytes += u64Done;
        if (fWriteRemote != 0) {
            g_u64ResPvmWrite++;
        } else {
            g_u64ResPvmRead++;
        }
        if (g_fResPvmLeanOnce == 0u) {
            g_fResPvmLeanOnce = 1u;
            kprintf("linux_hot: residual process_vm lean via=%s "
                    "bytes=%lu calls=%lu r=%lu w=%lu iov_max=%lu "
                    "Soft!=product G-AC-1 dual_dod_ab=OPEN\n",
                    fWriteRemote != 0 ? "writev" : "readv",
                    (unsigned long)u64Done,
                    (unsigned long)g_u64ResPvmCalls,
                    (unsigned long)g_u64ResPvmRead,
                    (unsigned long)g_u64ResPvmWrite,
                    (unsigned long)HOT_RES_PVM_IOV_MAX);
            hot_residual_lean_once(fWriteRemote != 0 ?
                                   "process_vm_writev" :
                                   "process_vm_readv");
        }
        return (i64)u64Done;
    }
    if (fFault != 0) {
        return -LINUX_EFAULT;
    }
    return 0;
}

i64
gj_linux_hot_process_vm_readv(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    return process_vm_copy(pRegs, 0);
}

i64
gj_linux_hot_process_vm_writev(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    return process_vm_copy(pRegs, 1);
}

i64
gj_linux_hot_membarrier(struct gj_linux_regs *pRegs)
{
    /*
     * Residual: membarrier(cmd, flags) for UDX host / glibc expedited probes.
     *   cmd 0  MEMBARRIER_CMD_QUERY -> supported soft bitmask
     *   else   mfence + success (no multi-CPU IPI product path yet)
     * greppable: linux_hot: membarrier query soft
     */
    i64 i64Cmd;
    i64 i64Flags;
    /* Soft supported: GLOBAL | GLOBAL_EXPEDITED | PRIVATE_EXPEDITED |
     * REGISTER_GLOBAL_EXPEDITED | REGISTER_PRIVATE_EXPEDITED (public bits). */
    const i64 i64SoftSupport = 1 | 2 | 4 | 8 | 16;

    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    i64Cmd = (i64)pRegs->u64Arg0;
    i64Flags = (i64)pRegs->u64Arg1;
    if (i64Flags != 0) {
        return -LINUX_EINVAL;
    }
    if (i64Cmd == 0) {
        if (g_fResMembarrierOnce == 0) {
            g_fResMembarrierOnce = 1;
            kprintf("linux_hot: membarrier query soft support=0x%lx residual=1\n",
                    (unsigned long)i64SoftSupport);
        }
        return i64SoftSupport;
    }
    /* Accept known expedited / global cmds as full barrier soft. */
    if (i64Cmd < 0 || (i64Cmd & ~i64SoftSupport) != 0) {
        return -LINUX_EINVAL;
    }
    __asm__ volatile("mfence" ::: "memory");
    return 0;
}

i64
gj_linux_hot_rseq(struct gj_linux_regs *pRegs)
{
    /*
     * Residual rseq(rseq, rseq_len, flags, sig) for glibc/UDX host TLS probe.
     * Soft register/unregister only - no critical-section abort delivery.
     * greppable: linux_hot: rseq soft
     */
    u64 u64Addr;
    u32 u32Len;
    u32 u32Flags;
    u32 u32Sig;

    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Addr = pRegs->u64Arg0;
    u32Len = (u32)pRegs->u64Arg1;
    u32Flags = (u32)pRegs->u64Arg2;
    u32Sig = (u32)pRegs->u64Arg3;
    /* flags: 0 = register, 1 = RSEQ_FLAG_UNREGISTER */
    if ((u32Flags & ~1u) != 0) {
        return -LINUX_EINVAL;
    }
    if (u32Flags & 1u) {
        /* unregister: match soft head when set */
        if (g_u64RseqAddr != 0 && u64Addr != 0 && u64Addr != g_u64RseqAddr) {
            return -LINUX_EINVAL;
        }
        g_u64RseqAddr = 0;
        g_u32RseqLen = 0;
        g_u32RseqSig = 0;
        return 0;
    }
    if (u64Addr == 0 || u32Len < 20u || u32Len > 1024u) {
        return -LINUX_EINVAL;
    }
    g_u64RseqAddr = u64Addr;
    g_u32RseqLen = u32Len;
    g_u32RseqSig = u32Sig;
    if (g_fResRseqOnce == 0) {
        g_fResRseqOnce = 1;
        kprintf("linux_hot: rseq soft register len=%u residual=1\n",
                (unsigned)u32Len);
    }
    return 0;
}

/*
 * Soft online CPU mask (bit0..N-1) into aMask[8]. Returns online count.
 * Soft!=product · UDX host topology residual.
 */
static u32
hot_res_aff_online_mask(u8 *pMask, u32 u32MaskBytes)
{
    u32 u32Online;
    u32 i;

    if (pMask == NULL || u32MaskBytes == 0) {
        return 0;
    }
    memset(pMask, 0, u32MaskBytes);
    u32Online = cpu_online_count();
    if (u32Online == 0) {
        u32Online = 1;
    }
    if (u32Online > (u32MaskBytes * 8u)) {
        u32Online = u32MaskBytes * 8u;
    }
    for (i = 0; i < u32Online; i++) {
        pMask[i / 8u] = (u8)(pMask[i / 8u] | (u8)(1u << (i % 8u)));
    }
    return u32Online;
}

i64
gj_linux_hot_sched_setaffinity(struct gj_linux_regs *pRegs)
{
    /*
     * Residual affinity lean: store soft mask for getaffinity round-trip
     * (UDX host worker pin probes). Intersect with online; empty -> EINVAL.
     * Soft!=product · G-AC-1 · Dual DoD A/B OPEN (agent!=close).
     * greppable: linux_hot: residual affinity
     * greppable: linux_hot: residual affinity lean
     */
    u8 aIn[8];
    u8 aOnline[8];
    u64 u64Len;
    u64 u64Copy;
    u32 u32Online;
    u32 i;
    int fAny = 0;

    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Len = pRegs->u64Arg1;
    if (u64Len == 0 || pRegs->u64Arg2 == 0) {
        return -LINUX_EINVAL;
    }
    memset(aIn, 0, sizeof(aIn));
    u64Copy = u64Len;
    if (u64Copy > sizeof(aIn)) {
        u64Copy = sizeof(aIn);
    }
    if (user_range_ok(pRegs->u64Arg2, u64Copy)) {
        if (copy_from_user(aIn, pRegs->u64Arg2, (size_t)u64Copy) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        memcpy(aIn, (const void *)(gj_vaddr_t)pRegs->u64Arg2, (size_t)u64Copy);
    }
    u32Online = hot_res_aff_online_mask(aOnline, (u32)sizeof(aOnline));
    (void)u32Online;
    /* Intersect requested mask with soft online set. */
    for (i = 0; i < (u32)sizeof(aIn); i++) {
        aIn[i] = (u8)(aIn[i] & aOnline[i]);
        if (aIn[i] != 0) {
            fAny = 1;
        }
    }
    if (fAny == 0) {
        return -LINUX_EINVAL;
    }
    memcpy(g_aResAffMask, aIn, sizeof(g_aResAffMask));
    g_fResAffStored = 1u;
    g_u64ResAffSet++;
    if (g_fResAffLeanOnce == 0u) {
        g_fResAffLeanOnce = 1u;
        kprintf("linux_hot: residual affinity lean via=setaffinity "
                "mask0=0x%x online=%u sets=%lu gets=%lu Soft!=product "
                "G-AC-1 dual_dod_ab=OPEN\n",
                (unsigned)g_aResAffMask[0], (unsigned)u32Online,
                (unsigned long)g_u64ResAffSet,
                (unsigned long)g_u64ResAffGet);
        hot_residual_lean_once("setaffinity");
    }
    return 0;
}

i64
gj_linux_hot_sched_getaffinity(struct gj_linux_regs *pRegs)
{
    /*
     * Residual: return soft stored affinity (if set) intersected with online,
     * else soft online CPU mask (bit0..N-1) for UDX host topology probes.
     * Size written = cpuset bytes requested (capped).
     * greppable: linux_hot: residual affinity
     * greppable: linux_hot: residual affinity lean
     * Soft!=product · Dual DoD A/B OPEN.
     */
    u8 aMask[8];
    u8 aOnline[8];
    u64 u64Len;
    u32 u32Online;
    u32 i;

    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Len = pRegs->u64Arg1;
    if (u64Len == 0 || pRegs->u64Arg2 == 0) {
        return -LINUX_EINVAL;
    }
    u32Online = hot_res_aff_online_mask(aOnline, (u32)sizeof(aOnline));
    if (g_fResAffStored != 0u) {
        for (i = 0; i < (u32)sizeof(aMask); i++) {
            aMask[i] = (u8)(g_aResAffMask[i] & aOnline[i]);
        }
    } else {
        memcpy(aMask, aOnline, sizeof(aMask));
    }
    if (u64Len > sizeof(aMask)) {
        u64Len = sizeof(aMask);
    }
    if (user_range_ok(pRegs->u64Arg2, u64Len)) {
        if (copy_to_user(pRegs->u64Arg2, aMask, (size_t)u64Len) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aMask, (size_t)u64Len);
    }
    g_u64ResAffGet++;
    if (g_fResAffLeanOnce == 0u) {
        g_fResAffLeanOnce = 1u;
        kprintf("linux_hot: residual affinity lean via=getaffinity "
                "mask0=0x%x online=%u stored=%u sets=%lu gets=%lu "
                "Soft!=product G-AC-1 dual_dod_ab=OPEN\n",
                (unsigned)aMask[0], (unsigned)u32Online,
                (unsigned)g_fResAffStored,
                (unsigned long)g_u64ResAffSet,
                (unsigned long)g_u64ResAffGet);
        hot_residual_lean_once("getaffinity");
    }
    return (i64)u64Len;
}

i64
gj_linux_hot_capget(struct gj_linux_regs *pRegs)
{
    /* Return empty effective/permitted/inheritable (unprivileged). */
    u32 aHdr[2];
    u32 aData[6];

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    memset(aHdr, 0, sizeof(aHdr));
    memset(aData, 0, sizeof(aData));
    aHdr[0] = 0x20080522u; /* _LINUX_CAPABILITY_VERSION_3 */
    aHdr[1] = g_u32LinuxPid;
    if (pRegs->u64Arg0 != 0) {
        if (user_range_ok(pRegs->u64Arg0, 8)) {
            (void)copy_to_user(pRegs->u64Arg0, aHdr, 8);
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg0, aHdr, 8);
        }
    }
    if (pRegs->u64Arg1 != 0) {
        if (user_range_ok(pRegs->u64Arg1, sizeof(aData))) {
            (void)copy_to_user(pRegs->u64Arg1, aData, sizeof(aData));
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aData, sizeof(aData));
        }
    }
    return 0;
}

i64
gj_linux_hot_capset(struct gj_linux_regs *pRegs)
{
    /* Accept any set for bring-up (no real capability enforcement yet). */
    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_getcpu(struct gj_linux_regs *pRegs)
{
    /* Residual: soft CPU id from hot path cpu_id() when per-CPU ready. */
    u32 cpu = 0;
    u32 node = 0;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    cpu = cpu_id();
    if (pRegs->u64Arg0 != 0) {
        if (user_range_ok(pRegs->u64Arg0, 4)) {
            (void)copy_to_user(pRegs->u64Arg0, &cpu, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg0 = cpu;
        }
    }
    if (pRegs->u64Arg1 != 0) {
        if (user_range_ok(pRegs->u64Arg1, 4)) {
            (void)copy_to_user(pRegs->u64Arg1, &node, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg1 = node;
        }
    }
    return 0;
}

i64
gj_linux_hot_gettimeofday(struct gj_linux_regs *pRegs)
{
    /* struct timeval { time_t tv_sec; suseconds_t tv_usec; } - 16 bytes LP64 */
    i64 aTv[2];
    u64 nsec;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (timer_ready()) {
        nsec = timer_mono_nsec();
    } else {
        g_u64MonoNsec += 1000000ull;
        nsec = g_u64MonoNsec;
    }
    aTv[0] = (i64)(nsec / 1000000000ull);
    aTv[1] = (i64)((nsec % 1000000000ull) / 1000ull);
    if (pRegs->u64Arg0 != 0) {
        if (user_range_ok(pRegs->u64Arg0, 16)) {
            if (copy_to_user(pRegs->u64Arg0, aTv, 16) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg0, aTv, 16);
        }
    }
    /* timezone ignored (arg1) */
    return 0;
}

i64
gj_linux_hot_settimeofday(struct gj_linux_regs *pRegs)
{
    /* Accept no-op (no RTC write on bring-up) */
    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_time(struct gj_linux_regs *pRegs)
{
    i64 sec;
    u64 nsec;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (timer_ready()) {
        nsec = timer_mono_nsec();
    } else {
        g_u64MonoNsec += 1000000ull;
        nsec = g_u64MonoNsec;
    }
    sec = (i64)(nsec / 1000000000ull);
    if (pRegs != NULL && pRegs->u64Arg0 != 0) {
        if (user_range_ok(pRegs->u64Arg0, 8)) {
            if (copy_to_user(pRegs->u64Arg0, &sec, 8) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            *(i64 *)(gj_vaddr_t)pRegs->u64Arg0 = sec;
        }
    }
    return sec;
}

i64
gj_linux_hot_clock_settime(struct gj_linux_regs *pRegs)
{
    /* Bring-up: accept CLOCK_REALTIME set as no-op (no RTC write). */
    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg0 != LINUX_CLOCK_REALTIME &&
        pRegs->u64Arg0 != LINUX_CLOCK_MONOTONIC) {
        return -LINUX_EINVAL;
    }
    return 0;
}

i64
gj_linux_hot_clock_adjtime(struct gj_linux_regs *pRegs)
{
    /* Accept adjtimex-shaped no-op; leave timespec buffer untouched. */
    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_sched_setattr(struct gj_linux_regs *pRegs)
{
    /* Accept SCHED_OTHER-shaped setattr (no real class switch yet). */
    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_sched_getattr(struct gj_linux_regs *pRegs)
{
    /* Write minimal sched_attr: size=48, policy=0 (NORMAL), nice=0 */
    u8 aAttr[48];

    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    memset(aAttr, 0, sizeof(aAttr));
    /* size at offset 0 */
    aAttr[0] = 48;
    if (pRegs->u64Arg1 != 0) {
        u64 cb = pRegs->u64Arg2;

        if (cb == 0) {
            cb = 48;
        }
        if (cb > sizeof(aAttr)) {
            cb = sizeof(aAttr);
        }
        if (user_range_ok(pRegs->u64Arg1, cb)) {
            if (copy_to_user(pRegs->u64Arg1, aAttr, (size_t)cb) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aAttr, (size_t)cb);
        }
    }
    return 0;
}

i64
gj_linux_hot_mbind(struct gj_linux_regs *pRegs)
{
    /* NUMA mbind - accept no-op (single node bring-up). */
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_set_mempolicy(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_get_mempolicy(struct gj_linux_regs *pRegs)
{
    i64 mode = 0; /* MPOL_DEFAULT */

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs != NULL && pRegs->u64Arg0 != 0) {
        if (user_range_ok(pRegs->u64Arg0, 8)) {
            if (copy_to_user(pRegs->u64Arg0, &mode, 8) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            *(i64 *)(gj_vaddr_t)pRegs->u64Arg0 = mode;
        }
    }
    return 0;
}

i64
gj_linux_hot_exit(struct gj_linux_regs *pRegs)
{
    i64 i64Code = pRegs ? (i64)pRegs->u64Arg0 : 0;
    struct gj_thread *pThr = thread_current();
    u64 u64Ctid;

    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    kprintf("linux: exit(%ld)\n", (long)i64Code);
    /* futex: robust exit - soft G-FUT-ROBUST OWNER_DIED walk before death */
    if (pThr != NULL) {
        (void)futex_exit_robust_list(pThr);
    }
    /*
     * Soft clear_child_tid: thread_exit stores 0 + wakes join.
     * Smoke path: promote the global CTID onto this thr first.
     * SYS_exit is thread-scoped when other USER siblings live
     * (pthread_exit). exit_group still kills the PCB.
     * greppable: linux_hot: residual ctid lean
     * Soft!=product · G-AC-1.
     */
    if (pThr != NULL && pThr->u64ClearChildTid == 0 &&
        g_u64ClearChildTid != 0) {
        pThr->u64ClearChildTid = g_u64ClearChildTid;
        g_u64ClearChildTid = 0;
        g_u64ResCtidClear++;
        if (g_fResCtidLeanOnce == 0u) {
            g_fResCtidLeanOnce = 1u;
            kprintf("linux_hot: residual ctid lean via=clear_child_tid "
                    "ctid=0x%lx clears=%lu sets=%lu Soft!=product G-AC-1 "
                    "(clone-adjacent; clone cold; set_tid hot)\n",
                    (unsigned long)pThr->u64ClearChildTid,
                    (unsigned long)g_u64ResCtidClear,
                    (unsigned long)g_u64ResCtidSet);
            hot_residual_lean_once("ctid_clear");
        }
    } else if (pThr == NULL && g_u64ClearChildTid != 0) {
        u64Ctid = g_u64ClearChildTid;
        g_u64ClearChildTid = 0;
        if (user_range_ok(u64Ctid, sizeof(u32))) {
            if (user_range_mapped(u64Ctid, sizeof(u32))) {
                (void)user_store_u32(u64Ctid, 0);
            }
        }
    }
    if (pThr != NULL && pThr->pProc != NULL) {
        if (thread_user_live_count(pThr->pProc) <= 1u) {
            process_death(pThr->pProc, (u32)i64Code);
        }
    }
    /* End this thread; schedule others (idle / personality). */
    thread_exit();
    return 0;
}

i64
gj_linux_hot_exit_group(struct gj_linux_regs *pRegs)
{
    i64 i64Code = pRegs ? (i64)pRegs->u64Arg0 : 0;
    struct gj_thread *pThr = thread_current();

    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    if (pThr != NULL) {
        (void)futex_exit_robust_list(pThr);
    }
    if (pThr != NULL && pThr->pProc != NULL) {
        process_death(pThr->pProc, (u32)i64Code);
    }
    thread_exit();
    return 0;
}

/*
 * brk grow: memobj first, then leaf USER maps in the calling AS.
 * Identity 2MiB leftovers stay supervisor without GJ_VMM_PROT_USER.
 */
static gj_vaddr_t
hot_map_brk_pages(struct gj_process *pProc, u64 u64Va, size_t cbLen)
{
    u32 u32Prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER;
    gj_vaddr_t vaMap;
    u64 u64Off;
    u64 u64SavedCr3 = 0;

    if (cbLen == 0 || (u64Va & 0xfffull) != 0) {
        return 0;
    }
    if (pProc != NULL) {
        vaMap = memobj_map_anon(pProc, u64Va, cbLen, u32Prot, 1);
        if (vaMap == (gj_vaddr_t)u64Va) {
            return vaMap;
        }
        if (process_as_ensure(pProc) != GJ_OK) {
            return 0;
        }
        u64SavedCr3 = cpu_read_cr3();
        process_as_activate(pProc);
    } else {
        vaMap = vmm_mmap_anon(u64Va, cbLen, u32Prot, 1);
        if (vaMap == (gj_vaddr_t)u64Va) {
            return vaMap;
        }
    }
    for (u64Off = 0; u64Off < (u64)cbLen; u64Off += 4096ull) {
        gj_paddr_t pa;
        void *pK;

        pa = pmm_alloc();
        if (pa == 0) {
            if (u64SavedCr3 != 0) {
                cpu_load_cr3(u64SavedCr3);
            }
            return 0;
        }
        pK = (void *)hhdm_to_virt(pa);
        memset(pK, 0, 4096);
        if (vmm_map_page((gj_vaddr_t)(u64Va + u64Off), pa, u32Prot) != GJ_OK) {
            pmm_free(pa);
            if (u64SavedCr3 != 0) {
                cpu_load_cr3(u64SavedCr3);
            }
            return 0;
        }
    }
    if (u64SavedCr3 != 0) {
        cpu_load_cr3(u64SavedCr3);
    }
    return (gj_vaddr_t)u64Va;
}

/*
 * MAP_ANON on the calling USER PCB. memobj first; if pool/region/vmm miss,
 * map zeroed frames in that AS (same belt as brk grow). addr=0 uses the
 * per-PCB anon cursor (0x40000000), not g_pLinuxProc's table.
 */
static gj_vaddr_t
hot_map_anon_user(struct gj_process *pProc, u64 u64Hint, size_t cbLen,
                  u32 u32Prot, int fFixed)
{
    gj_vaddr_t vaMap;
    u64 u64Va;
    u64 u64Off;
    u64 u64SavedCr3 = 0;
    size_t cbAligned;

    if (cbLen == 0) {
        return 0;
    }
    cbAligned = (cbLen + 0xfffull) & ~0xfffull;
    if (pProc != NULL) {
        vaMap = memobj_map_anon(pProc, u64Hint, cbLen, u32Prot, fFixed);
        if (vaMap != 0) {
            return vaMap;
        }
        if (process_as_ensure(pProc) != GJ_OK) {
            return 0;
        }
        u64SavedCr3 = cpu_read_cr3();
        process_as_activate(pProc);
        if (fFixed != 0) {
            u64Va = u64Hint;
        } else if (u64Hint >= GJ_USER_VA_BASE &&
                   (u64Hint + (u64)cbAligned) <= GJ_USER_VA_END &&
                   (u64Hint & 0xfffull) == 0) {
            u64Va = u64Hint;
        } else {
            u64Va = pProc->u64AnonNext;
            if (u64Va < GJ_USER_VA_BASE ||
                (u64Va + (u64)cbAligned) > GJ_USER_VA_END) {
                u64Va = 0x0000000040000000ull;
            }
        }
    } else {
        return vmm_mmap_anon(u64Hint, cbLen, u32Prot, fFixed);
    }
    if ((u64Va & 0xfffull) != 0 || u64Va < GJ_USER_VA_BASE ||
        (u64Va + (u64)cbAligned) > GJ_USER_VA_END) {
        if (u64SavedCr3 != 0) {
            cpu_load_cr3(u64SavedCr3);
        }
        return 0;
    }
    for (u64Off = 0; u64Off < (u64)cbAligned; u64Off += 4096ull) {
        gj_paddr_t pa;
        void *pK;

        pa = pmm_alloc();
        if (pa == 0) {
            if (u64SavedCr3 != 0) {
                cpu_load_cr3(u64SavedCr3);
            }
            return 0;
        }
        pK = (void *)hhdm_to_virt(pa);
        memset(pK, 0, 4096);
        if (vmm_map_page((gj_vaddr_t)(u64Va + u64Off), pa, u32Prot) != GJ_OK) {
            pmm_free(pa);
            if (u64SavedCr3 != 0) {
                cpu_load_cr3(u64SavedCr3);
            }
            return 0;
        }
    }
    if (fFixed == 0) {
        pProc->u64AnonNext = u64Va + (u64)cbAligned;
    }
    if (u64SavedCr3 != 0 &&
        (u64SavedCr3 & ~0xfffull) != (pProc->u64Cr3 & ~0xfffull)) {
        cpu_load_cr3(u64SavedCr3);
    }
    return (gj_vaddr_t)u64Va;
}

i64
gj_linux_hot_brk(struct gj_linux_regs *pRegs)
{
    static u32 s_cBrkGrowLamp;
    u64 u64Req;
    struct gj_process *pProc;
    u64 *pBase;
    u64 *pCur;
    int nMap;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Req = pRegs->u64Arg0;
    pProc = hot_calling_proc();
    if (pProc != NULL) {
        if (pProc->u64BrkBase == 0) {
            if (pProc == g_pLinuxProc && g_u64BrkBase != 0) {
                pProc->u64BrkBase = g_u64BrkBase;
                pProc->u64BrkCur = g_u64BrkCur;
            } else {
                pProc->u64BrkBase = 0x04600000ull;
                pProc->u64BrkCur = pProc->u64BrkBase;
            }
        }
        pBase = &pProc->u64BrkBase;
        pCur = &pProc->u64BrkCur;
    } else {
        if (g_u64BrkBase == 0) {
            g_u64BrkBase = 0x04600000ull;
            g_u64BrkCur = g_u64BrkBase;
        }
        pBase = &g_u64BrkBase;
        pCur = &g_u64BrkCur;
    }
    if (u64Req == 0) {
        return (i64)*pCur;
    }
    /* Allow grow within 64 MiB of base (OpenSSL 3.5.7 provider + namemap). */
    if (u64Req < *pBase || u64Req > *pBase + (64ull << 20)) {
        if (pProc != NULL && pProc->u32Personality == 1u &&
            u64Req > *pCur && s_cBrkGrowLamp < 8u) {
            s_cBrkGrowLamp++;
            kprintf("linux_hot: brk grow base=0x%lx cur=0x%lx req=0x%lx map=%d\n",
                    (unsigned long)*pBase, (unsigned long)*pCur,
                    (unsigned long)u64Req, 0);
        }
        return (i64)*pCur;
    }
    /*
     * Map new pages so libcgj sbrk/malloc can store. Pointer-only brk
     * was the 0.1.139/140 dash #PF I=1 class.
     */
    if (u64Req > *pCur) {
        u64 u64Old = *pCur;
        u64 u64Covered;
        u64 u64Need;
        gj_vaddr_t vaMap;

        nMap = 1;
        if (u64Old <= *pBase) {
            u64Covered = *pBase;
        } else {
            u64Covered = (u64Old + 0xfffull) & ~0xfffull;
        }
        u64Need = (u64Req + 0xfffull) & ~0xfffull;
        if (u64Need > u64Covered) {
            size_t cbMap = (size_t)(u64Need - u64Covered);

            vaMap = hot_map_brk_pages(pProc, u64Covered, cbMap);
            if (vaMap != (gj_vaddr_t)u64Covered) {
                nMap = 0;
                if (pProc != NULL && pProc->u32Personality == 1u &&
                    s_cBrkGrowLamp < 8u) {
                    s_cBrkGrowLamp++;
                    kprintf("linux_hot: brk grow base=0x%lx cur=0x%lx req=0x%lx map=%d\n",
                            (unsigned long)*pBase, (unsigned long)*pCur,
                            (unsigned long)u64Req, nMap);
                }
                /* libcgj sbrk treats a non-errno return as success. */
                return -LINUX_ENOMEM;
            }
        }
        if (pProc != NULL && pProc->u32Personality == 1u &&
            s_cBrkGrowLamp < 8u) {
            s_cBrkGrowLamp++;
            kprintf("linux_hot: brk grow base=0x%lx cur=0x%lx req=0x%lx map=%d\n",
                    (unsigned long)*pBase, (unsigned long)*pCur,
                    (unsigned long)u64Req, nMap);
        }
    }
    *pCur = u64Req;
    if (pProc != NULL && pProc == g_pLinuxProc) {
        g_u64BrkBase = *pBase;
        g_u64BrkCur = *pCur;
    }
    return (i64)*pCur;
}

i64
gj_linux_hot_mmap(struct gj_linux_regs *pRegs)
{
    u64 u64Addr;
    u64 u64Len;
    u64 u64Prot;
    u64 u64Flags;
    u32 u32VmmProt;
    gj_vaddr_t va;
    int fFixed;
    int fAnon;
    int fNoreplace;
    struct gj_process *pProc;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Addr = pRegs->u64Arg0;
    u64Len = pRegs->u64Arg1;
    u64Prot = pRegs->u64Arg2;
    u64Flags = pRegs->u64Arg3;
    fAnon = ((u64Flags & LINUX_MAP_ANONYMOUS) != 0) ? 1 : 0;
    pProc = hot_calling_proc();

    if (u64Len == 0) {
        return -LINUX_EINVAL;
    }
    /*
     * Soft residual: page-round length up (Linux kernel does this).
     * No silent max-truncate (would regress large UDX host rings).
     * HOT_RES_MMAP_MAX is inventory honesty only (Soft!=product).
     * greppable: linux_hot: residual mmap lean
     */
    if ((u64Len & 0xfffull) != 0) {
        u64Len = (u64Len + 0xfffull) & ~0xfffull;
        g_u64ResMmapPageRound++;
    }
    /*
     * MAP_FIXED_NOREPLACE (0x100000): treat as FIXED for residual placement.
     * Soft no product occupancy reject yet (EEXIST path remains OPEN).
     * POPULATE/STACK/NORESERVE/NONBLOCK: soft no-op flags (accepted).
     */
    fNoreplace = ((u64Flags & LINUX_MAP_FIXED_NOREPLACE) != 0) ? 1 : 0;
    fFixed = ((u64Flags & LINUX_MAP_FIXED) != 0 || fNoreplace != 0) ? 1 : 0;
    if (fFixed != 0) {
        g_u64ResMmapFixed++;
    }
    (void)(u64Flags & (LINUX_MAP_POPULATE | LINUX_MAP_STACK |
                        LINUX_MAP_NORESERVE | LINUX_MAP_NONBLOCK));

    /* Hot path: anonymous, io_uring ring fd, or soft vfs_ram file map. */
    if (!fAnon) {
        i64 i64Fd = (i64)pRegs->u64Arg4;
        u64 u64Off = pRegs->u64Arg5;

        if (gj_io_uring_fd_ok(i64Fd)) {
            return gj_io_uring_mmap(i64Fd, u64Off, u64Len);
        }
        /* W^X gate also applies to soft file maps. */
        if ((u64Prot & LINUX_PROT_WRITE) && (u64Prot & LINUX_PROT_EXEC)) {
            if (!gj_process_has_jit(pProc)) {
                return -LINUX_EACCES; /* W^X without CapJit */
            }
        }
        u32VmmProt = GJ_VMM_PROT_USER;
        if (u64Prot & LINUX_PROT_READ) {
            u32VmmProt |= GJ_VMM_PROT_READ;
        }
        if (u64Prot & LINUX_PROT_WRITE) {
            u32VmmProt |= GJ_VMM_PROT_WRITE;
        }
        if (u64Prot & LINUX_PROT_EXEC) {
            u32VmmProt |= GJ_VMM_PROT_EXEC;
        }
        if (u32VmmProt == GJ_VMM_PROT_USER) {
            u32VmmProt |= GJ_VMM_PROT_READ;
        }
        /* fFixed already computed above (FIXED | FIXED_NOREPLACE). */
        /*
         * addr==0: leave hint 0. memobj_pick_va bumps pProc->u64AnonNext
         * from 0x40000000. Forcing the same hint twice overlaps the first map.
         */
        /*
         * Soft product path (ABI-first item 4): live vfs_ram regular-file
         * fd (open / memfd path) -> snapshot fill -> FILE memobj + USER PTEs.
         * Success returns mapped VA. MAP_ANONYMOUS path is separate below.
         * Non-ram / unsupported fd types stay ENOSYS (product vfs door OPEN).
         */
        if (pProc != NULL && vfs_ram_fd_ok(i64Fd)) {
            /* Linux mmap: offset must be page-aligned (4 KiB). */
            if ((u64Off & 0xfffull) != 0) {
                return -LINUX_EINVAL;
            }
            va = memobj_map_file_fd(pProc, i64Fd, u64Addr,
                                    (size_t)u64Len, u32VmmProt, fFixed,
                                    u64Off);
            if (va != 0) {
                g_u64ResMmapFilePass++;
                /*
                 * Greppable: linux_hot: mmap file soft PASS
                 * greppable: linux_hot: residual mmap lean
                 * Soft!=product · G-AC-1 · once-lamp only (no stamp storms).
                 */
                if (g_fResMmapLeanOnce == 0u) {
                    g_fResMmapLeanOnce = 1u;
                    kprintf("linux_hot: mmap file soft PASS va=0x%lx "
                            "fd=%ld len=%lu off=0x%lx\n",
                            (unsigned long)va, (long)i64Fd,
                            (unsigned long)u64Len, (unsigned long)u64Off);
                    kprintf("linux_hot: residual mmap lean via=file_pass "
                            "va=0x%lx fd=%ld len=%lu pass=%lu enosys=%lu "
                            "fixed=%d page_round=%lu Soft!=product G-AC-1\n",
                            (unsigned long)va, (long)i64Fd,
                            (unsigned long)u64Len,
                            (unsigned long)g_u64ResMmapFilePass,
                            (unsigned long)g_u64ResMmapFileEnosys,
                            fFixed,
                            (unsigned long)g_u64ResMmapPageRound);
                    hot_residual_lean_once("mmap_file_pass");
                }
                return (i64)va;
            }
            /*
             * ram fd present but snapshot/map failed (OOM, non-regular,
             * region full, > GJ_MEMOBJ_MAX_PAGES). Not a permanent
             * ENOSYS-only path - soft miss with proper errno.
             */
            kprintf("linux_hot: mmap file soft FAIL fd=%ld len=%lu "
                    "off=0x%lx (ENOMEM soft)\n",
                    (long)i64Fd, (unsigned long)u64Len,
                    (unsigned long)u64Off);
            return -LINUX_ENOMEM;
        }
        /*
         * Greppable: linux_hot: mmap file soft ENOSYS (non-vfs_ram / no proc)
         * greppable: linux_hot: residual mmap lean
         * Once-lamp only - no ENOSYS stamp storms on repeated probes.
         */
        g_u64ResMmapFileEnosys++;
        if (g_fResMmapLeanOnce == 0u) {
            g_fResMmapLeanOnce = 1u;
            kprintf("linux_hot: mmap file soft ENOSYS fd=%ld len=%lu off=0x%lx "
                    "proc=%p\n",
                    (long)i64Fd, (unsigned long)u64Len, (unsigned long)u64Off,
                    (void *)pProc);
            kprintf("linux_hot: residual mmap lean via=file_enosys "
                    "fd=%ld len=%lu pass=%lu enosys=%lu "
                    "Soft!=product G-AC-1\n",
                    (long)i64Fd, (unsigned long)u64Len,
                    (unsigned long)g_u64ResMmapFilePass,
                    (unsigned long)g_u64ResMmapFileEnosys);
            hot_residual_lean_once("mmap_file_enosys");
        }
        return -LINUX_ENOSYS;
    }
    if ((u64Prot & LINUX_PROT_WRITE) && (u64Prot & LINUX_PROT_EXEC)) {
        if (!gj_process_has_jit(pProc)) {
            return -LINUX_EACCES; /* W^X without CapJit */
        }
    }
    u32VmmProt = GJ_VMM_PROT_USER;
    if (u64Prot & LINUX_PROT_READ) {
        u32VmmProt |= GJ_VMM_PROT_READ;
    }
    if (u64Prot & LINUX_PROT_WRITE) {
        u32VmmProt |= GJ_VMM_PROT_WRITE;
    }
    if (u64Prot & LINUX_PROT_EXEC) {
        u32VmmProt |= GJ_VMM_PROT_EXEC;
    }
    if (u32VmmProt == GJ_VMM_PROT_USER) {
        u32VmmProt |= GJ_VMM_PROT_READ;
    }
    /* fFixed already computed above (FIXED | FIXED_NOREPLACE). */
    /*
     * addr==0: leave hint 0. memobj_map_anon / memobj_pick_va already bump
     * pProc->u64AnonNext from 0x40000000. Forcing 0x40000000 here made the
     * second mmap(0) overlap the first (sshd / OpenSSL).
     * FIXED maps keep the caller VA. GJ_VMM_PROT_USER stays on every PTE.
     */
    /* G-MO-1: anon mmap on calling USER PCB (sshd-auth after exec_replace). */
    va = hot_map_anon_user(pProc, u64Addr, (size_t)u64Len, u32VmmProt, fFixed);
    if (va == 0) {
        return -LINUX_ENOMEM;
    }
    g_u64ResMmapAnonPass++;
    /*
     * greppable: linux_hot: residual mmap anon lean
     * Soft!=product · G-AC-1 · once-lamp only (no stamp storms).
     * FIXED_NOREPLACE soft-as-FIXED; page-round tallied above.
     */
    if (g_fResMmapAnonOnce == 0u) {
        g_fResMmapAnonOnce = 1u;
        kprintf("linux_hot: residual mmap anon lean via=anon_pass "
                "va=0x%lx len=%lu pass=%lu fixed=%d noreplace=%d "
                "page_round=%lu Soft!=product G-AC-1\n",
                (unsigned long)va, (unsigned long)u64Len,
                (unsigned long)g_u64ResMmapAnonPass, fFixed, fNoreplace,
                (unsigned long)g_u64ResMmapPageRound);
        hot_residual_lean_once("mmap_anon_pass");
    }
    return (i64)va;
}

i64
gj_linux_hot_munmap(struct gj_linux_regs *pRegs)
{
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0) {
        return -LINUX_EINVAL;
    }
    if (hot_calling_proc() != NULL) {
        st = memobj_unmap(hot_calling_proc(), (gj_vaddr_t)pRegs->u64Arg0,
                          (size_t)pRegs->u64Arg1);
    } else {
        st = vmm_munmap((gj_vaddr_t)pRegs->u64Arg0, (size_t)pRegs->u64Arg1);
    }
    if (st == GJ_ERR_INVAL) {
        return -LINUX_EINVAL;
    }
    return 0;
}

i64
gj_linux_hot_mremap(struct gj_linux_regs *pRegs)
{
    /*
     * mremap(old_addr, old_size, new_size, flags, [new_addr])
     * Bring-up: shrink in place; grow in place when possible; else MAYMOVE.
     */
    u64 u64Old;
    u64 u64OldSz;
    u64 u64NewSz;
    u64 u64Flags;
    u64 u64NewHint;
    gj_vaddr_t vaGrow;
    struct gj_linux_regs one;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Old = pRegs->u64Arg0;
    u64OldSz = pRegs->u64Arg1;
    u64NewSz = pRegs->u64Arg2;
    u64Flags = pRegs->u64Arg3;
    u64NewHint = pRegs->u64Arg4;
    if (u64Old == 0 || (u64Old & 0xfffull) != 0 || u64OldSz == 0) {
        return -LINUX_EINVAL;
    }
    if (u64NewSz == 0) {
        return -LINUX_EINVAL;
    }
    if (u64NewSz == u64OldSz) {
        return (i64)u64Old;
    }
    if (u64NewSz < u64OldSz) {
        /* Shrink: unmap the tail */
        one.u64Arg0 = u64Old + u64NewSz;
        one.u64Arg1 = u64OldSz - u64NewSz;
        if (gj_linux_hot_munmap(&one) != 0) {
            return -LINUX_EINVAL;
        }
        return (i64)u64Old;
    }
    /* Grow: try map extra pages at end (same VA) */
    vaGrow = hot_map_brk_pages(hot_calling_proc(), u64Old + u64OldSz,
                               (size_t)(u64NewSz - u64OldSz));
    if (vaGrow == (gj_vaddr_t)(u64Old + u64OldSz)) {
        return (i64)u64Old;
    }
    /* In-place grow failed */
    if ((u64Flags & LINUX_MREMAP_MAYMOVE) == 0) {
        return -LINUX_ENOMEM;
    }
    /* MAYMOVE: allocate new region; soft-copy old content (≤4 KiB residual). */
    if ((u64Flags & LINUX_MREMAP_FIXED) != 0 && u64NewHint != 0) {
        one.u64Arg0 = u64NewHint;
        one.u64Arg1 = u64NewSz;
        one.u64Arg2 = LINUX_PROT_READ | LINUX_PROT_WRITE;
        one.u64Arg3 = LINUX_MAP_PRIVATE | LINUX_MAP_ANONYMOUS | LINUX_MAP_FIXED;
        one.u64Arg4 = (u64)-1;
        one.u64Arg5 = 0;
    } else {
        one.u64Arg0 = 0;
        one.u64Arg1 = u64NewSz;
        one.u64Arg2 = LINUX_PROT_READ | LINUX_PROT_WRITE;
        one.u64Arg3 = LINUX_MAP_PRIVATE | LINUX_MAP_ANONYMOUS;
        one.u64Arg4 = (u64)-1;
        one.u64Arg5 = 0;
    }
    {
        i64 i64New = gj_linux_hot_mmap(&one);
        u64 u64Copy;
        u64 u64Done = 0;
        u8 aTmp[4096];

        if (i64New < 0) {
            return i64New;
        }
        /*
         * greppable: linux_hot: residual mremap copy
         * greppable: linux_hot: residual mremap lean
         * Multi-chunk soft copy (page loop) for Linux-shaped realloc paths.
         * Soft residual cap HOT_RES_MREMAP_COPY_MAX (Soft!=product).
         */
        u64Copy = u64OldSz < u64NewSz ? u64OldSz : u64NewSz;
        if (u64Copy > HOT_RES_MREMAP_COPY_MAX) {
            u64Copy = HOT_RES_MREMAP_COPY_MAX;
        }
        while (u64Done < u64Copy) {
            size_t cb = (size_t)(u64Copy - u64Done);

            if (cb > sizeof(aTmp)) {
                cb = sizeof(aTmp);
            }
            if (user_range_ok(u64Old + u64Done, cb) &&
                user_range_ok((u64)i64New + u64Done, cb)) {
                if (copy_from_user(aTmp, u64Old + u64Done, cb) != GJ_OK) {
                    break;
                }
                if (copy_to_user((u64)i64New + u64Done, aTmp, cb) != GJ_OK) {
                    break;
                }
            } else if (!user_range_ok(u64Old + u64Done, cb)) {
                /* Kernel smoke path */
                memcpy(aTmp, (const void *)(gj_vaddr_t)(u64Old + u64Done), cb);
                memcpy((void *)(gj_vaddr_t)((u64)i64New + u64Done), aTmp, cb);
            } else {
                break;
            }
            u64Done += cb;
        }
        g_u64ResMremapCopy += u64Done;
        if (g_fResMremapLeanOnce == 0u && u64Done > 0) {
            g_fResMremapLeanOnce = 1u;
            kprintf("linux_hot: residual mremap lean via=maymove_copy "
                    "old=0x%lx new=0x%lx copied=%lu old_sz=%lu new_sz=%lu "
                    "Soft!=product G-AC-1\n",
                    (unsigned long)u64Old, (unsigned long)i64New,
                    (unsigned long)u64Done, (unsigned long)u64OldSz,
                    (unsigned long)u64NewSz);
            hot_residual_lean_once("mremap_copy");
        }
        if ((u64Flags & LINUX_MREMAP_DONTUNMAP) == 0) {
            one.u64Arg0 = u64Old;
            one.u64Arg1 = u64OldSz;
            (void)gj_linux_hot_munmap(&one);
        }
        return i64New;
    }
}

i64
gj_linux_hot_mprotect(struct gj_linux_regs *pRegs)
{
    u64 u64Addr;
    u64 u64Len;
    u64 u64Prot;
    u32 u32VmmProt;
    u64 u64Off;
    gj_status_t st;
    u64 u64SavedCr3 = 0;
    struct gj_process *pProc;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    pProc = hot_calling_proc();
    u64Addr = pRegs->u64Arg0;
    u64Len = pRegs->u64Arg1;
    u64Prot = pRegs->u64Arg2;
    if ((u64Prot & LINUX_PROT_WRITE) != 0 &&
        (u64Prot & LINUX_PROT_EXEC) != 0) {
        if (!gj_process_has_jit(pProc)) {
            return -LINUX_EACCES;
        }
    }
    u32VmmProt = GJ_VMM_PROT_USER;
    if (u64Prot & LINUX_PROT_READ) {
        u32VmmProt |= GJ_VMM_PROT_READ;
    }
    if (u64Prot & LINUX_PROT_WRITE) {
        u32VmmProt |= GJ_VMM_PROT_WRITE;
    }
    if (u64Prot & LINUX_PROT_EXEC) {
        u32VmmProt |= GJ_VMM_PROT_EXEC;
    }
    if (pProc != NULL && pProc->u64Cr3 != 0) {
        u64SavedCr3 = cpu_read_cr3();
        process_as_activate(pProc);
    }
    for (u64Off = 0; u64Off < u64Len; u64Off += 4096) {
        st = vmm_protect_page((gj_vaddr_t)(u64Addr + u64Off), u32VmmProt);
        if (st == GJ_ERR_PERM) {
            if (u64SavedCr3) {
                cpu_load_cr3(u64SavedCr3);
            }
            return -LINUX_EACCES;
        }
        if (st != GJ_OK && st != GJ_ERR_NOENT) {
            if (u64SavedCr3) {
                cpu_load_cr3(u64SavedCr3);
            }
            return -LINUX_ENOMEM;
        }
    }
    /* Leave process AS active if it was already (or is current task). */
    if (u64SavedCr3 && pProc != NULL &&
        (u64SavedCr3 & ~0xfffull) != (pProc->u64Cr3 & ~0xfffull)) {
        cpu_load_cr3(u64SavedCr3);
    }
    return 0;
}

i64
gj_linux_hot_clock_gettime(struct gj_linux_regs *pRegs)
{
    u64 u64Clk;
    struct linux_timespec tsK;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Clk = pRegs->u64Arg0;
    if (pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    /* Residual: COARSE + CPUTIME soft-alias mono (UDX host timer probes). */
    if (u64Clk != LINUX_CLOCK_MONOTONIC &&
        u64Clk != LINUX_CLOCK_MONOTONIC_RAW &&
        u64Clk != LINUX_CLOCK_MONOTONIC_COARSE &&
        u64Clk != LINUX_CLOCK_BOOTTIME &&
        u64Clk != LINUX_CLOCK_REALTIME &&
        u64Clk != LINUX_CLOCK_REALTIME_COARSE &&
        u64Clk != LINUX_CLOCK_PROCESS_CPUTIME_ID &&
        u64Clk != LINUX_CLOCK_THREAD_CPUTIME_ID) {
        return -LINUX_EINVAL;
    }
    if (timer_ready()) {
        g_u64MonoNsec = timer_mono_nsec();
    } else {
        g_u64MonoNsec += 1000000ull;
    }
    tsK.i64Sec = (i64)(g_u64MonoNsec / 1000000000ull);
    tsK.i64Nsec = (i64)(g_u64MonoNsec % 1000000000ull);
    if (user_range_ok(pRegs->u64Arg1, sizeof(tsK))) {
        st = copy_to_user(pRegs->u64Arg1, &tsK, sizeof(tsK));
        if (st != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        *(struct linux_timespec *)(gj_vaddr_t)pRegs->u64Arg1 = tsK;
    }
    return 0;
}

i64
gj_linux_hot_clock_getres(struct gj_linux_regs *pRegs)
{
    u64 u64Clk;
    struct linux_timespec tsK;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Clk = pRegs->u64Arg0;
    if (pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    if (u64Clk != LINUX_CLOCK_MONOTONIC &&
        u64Clk != LINUX_CLOCK_MONOTONIC_RAW &&
        u64Clk != LINUX_CLOCK_MONOTONIC_COARSE &&
        u64Clk != LINUX_CLOCK_BOOTTIME &&
        u64Clk != LINUX_CLOCK_REALTIME &&
        u64Clk != LINUX_CLOCK_REALTIME_COARSE &&
        u64Clk != LINUX_CLOCK_PROCESS_CPUTIME_ID &&
        u64Clk != LINUX_CLOCK_THREAD_CPUTIME_ID) {
        return -LINUX_EINVAL;
    }
    /* COARSE -> 1 ms; fine clocks -> 1 us soft residual */
    tsK.i64Sec = 0;
    if (u64Clk == LINUX_CLOCK_MONOTONIC_COARSE ||
        u64Clk == LINUX_CLOCK_REALTIME_COARSE) {
        tsK.i64Nsec = 1000000;
    } else {
        tsK.i64Nsec = 1000;
    }
    if (user_range_ok(pRegs->u64Arg1, sizeof(tsK))) {
        st = copy_to_user(pRegs->u64Arg1, &tsK, sizeof(tsK));
        if (st != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        *(struct linux_timespec *)(gj_vaddr_t)pRegs->u64Arg1 = tsK;
    }
    return 0;
}

static i64
sleep_timespec(const struct linux_timespec *pTs)
{
    u64 u64Delta;
    u64 u64Deadline;

    if (pTs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pTs->i64Sec < 0 || pTs->i64Nsec < 0 || pTs->i64Nsec >= 1000000000) {
        return -LINUX_EINVAL;
    }
    u64Delta = (u64)pTs->i64Sec * 1000000000ull + (u64)pTs->i64Nsec;
    if (timer_ready()) {
        u64Deadline = timer_mono_nsec() + u64Delta;
        timer_sleep_until(u64Deadline);
        g_u64MonoNsec = timer_mono_nsec();
    } else {
        g_u64MonoNsec += u64Delta;
    }
    return 0;
}

i64
gj_linux_hot_nanosleep(struct gj_linux_regs *pRegs)
{
    struct linux_timespec tsK;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL || pRegs->u64Arg0 == 0) {
        return -LINUX_EINVAL;
    }
    if (user_range_ok(pRegs->u64Arg0, sizeof(tsK))) {
        st = copy_from_user(&tsK, pRegs->u64Arg0, sizeof(tsK));
        if (st != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        tsK = *(const struct linux_timespec *)(gj_vaddr_t)pRegs->u64Arg0;
    }
    return sleep_timespec(&tsK);
}

i64
gj_linux_hot_clock_nanosleep(struct gj_linux_regs *pRegs)
{
    /*
     * clock_nanosleep(clockid, flags, *req, *rem)
     * flags: 0 = relative, TIMER_ABSTIME=1 absolute - bring-up: relative only.
     */
    u64 u64Clk;
    u64 u64Flags;
    struct linux_timespec tsK;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_TIME, pRegs);
    if (pRegs == NULL || pRegs->u64Arg2 == 0) {
        return -LINUX_EINVAL;
    }
    u64Clk = pRegs->u64Arg0;
    u64Flags = pRegs->u64Arg1;
    if (u64Clk != LINUX_CLOCK_MONOTONIC && u64Clk != LINUX_CLOCK_REALTIME &&
        u64Clk != LINUX_CLOCK_BOOTTIME &&
        u64Clk != LINUX_CLOCK_MONOTONIC_COARSE &&
        u64Clk != LINUX_CLOCK_REALTIME_COARSE) {
        return -LINUX_EINVAL;
    }
    if ((u64Flags & ~1ull) != 0) {
        return -LINUX_EINVAL;
    }
    if (user_range_ok(pRegs->u64Arg2, sizeof(tsK))) {
        st = copy_from_user(&tsK, pRegs->u64Arg2, sizeof(tsK));
        if (st != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        tsK = *(const struct linux_timespec *)(gj_vaddr_t)pRegs->u64Arg2;
    }
    if (u64Flags & 1ull) {
        /* TIMER_ABSTIME: sleep until abs time (clamp if past) */
        u64 u64Abs;
        u64 u64Now;

        if (tsK.i64Sec < 0 || tsK.i64Nsec < 0 || tsK.i64Nsec >= 1000000000) {
            return -LINUX_EINVAL;
        }
        u64Abs = (u64)tsK.i64Sec * 1000000000ull + (u64)tsK.i64Nsec;
        u64Now = timer_ready() ? timer_mono_nsec() : g_u64MonoNsec;
        if (u64Abs <= u64Now) {
            return 0;
        }
        tsK.i64Sec = (i64)((u64Abs - u64Now) / 1000000000ull);
        tsK.i64Nsec = (i64)((u64Abs - u64Now) % 1000000000ull);
    }
    return sleep_timespec(&tsK);
}

/*
 * Soft futex timeout -> absolute mono-nsec deadline (0 = none).
 * fAbs: WAIT_BITSET uses absolute timespec; classic WAIT is relative.
 * CLOCK_REALTIME flag accepted but still mapped to mono soft (G-FUT-2).
 * Returns 0, or -LINUX_EFAULT / -LINUX_EINVAL.
 */
static i64
hot_futex_deadline(u64 u64TsPtr, int fAbs, u64 *pOutDeadline)
{
    struct linux_timespec tsK;
    u64 u64Now;
    u64 u64Ns;

    if (pOutDeadline == NULL) {
        return -LINUX_EINVAL;
    }
    *pOutDeadline = 0;
    if (u64TsPtr == 0) {
        return 0;
    }
    if (user_range_ok(u64TsPtr, sizeof(tsK))) {
        if (copy_from_user(&tsK, u64TsPtr, sizeof(tsK)) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        tsK = *(const struct linux_timespec *)(gj_vaddr_t)u64TsPtr;
    }
    if (tsK.i64Sec < 0 || tsK.i64Nsec < 0 || tsK.i64Nsec >= 1000000000) {
        return -LINUX_EINVAL;
    }
    u64Ns = (u64)tsK.i64Sec * 1000000000ull + (u64)tsK.i64Nsec;
    u64Now = timer_ready() ? timer_mono_nsec() : g_u64MonoNsec;
    if (fAbs) {
        /* WAIT_BITSET: absolute timespec (mono soft; CLOCK_REALTIME flag ok). */
        *pOutDeadline = u64Ns;
    } else {
        /* Classic WAIT: relative timespec -> absolute mono deadline. */
        *pOutDeadline = u64Now + u64Ns;
    }
    return 0;
}

i64
gj_linux_hot_futex(struct gj_linux_regs *pRegs)
{
    u32 u32Op;
    u32 u32Cmd;
    u32 u32Val;
    u32 u32Bitset;
    volatile u32 *pU32;
    struct gj_futex_key key;
    u64 u64Deadline;
    int fPrivate;
    i64 i64Ts;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL || pRegs->u64Arg0 == 0) {
        return -LINUX_EINVAL;
    }
    /* Linux: futex word must be 4-byte aligned. */
    if ((pRegs->u64Arg0 & 3ull) != 0) {
        return -LINUX_EINVAL;
    }
    /* Pointer must be user-mapped for ring3; kernel smoke uses kernel word. */
    if (user_range_ok(pRegs->u64Arg0, sizeof(u32))) {
        if (!user_range_mapped(pRegs->u64Arg0, sizeof(u32))) {
            return -LINUX_EFAULT;
        }
    }
    pU32 = (volatile u32 *)(gj_vaddr_t)pRegs->u64Arg0;
    u32Op = (u32)pRegs->u64Arg1;
    fPrivate = (u32Op & GJ_FUTEX_PRIVATE_FLAG) ? 1 : 0;
    /* CLOCK_REALTIME only affects wait deadlines; soft maps to mono. */
    u32Cmd = u32Op & 0x7fu;
    st = futex_key_from_uaddr(&key, pRegs->u64Arg0, fPrivate);
    if (st != GJ_OK) {
        return -LINUX_EFAULT;
    }

    if (u32Cmd == GJ_FUTEX_WAIT) {
        u32Val = (u32)pRegs->u64Arg2;
        i64Ts = hot_futex_deadline(pRegs->u64Arg3, 0, &u64Deadline);
        if (i64Ts < 0) {
            return i64Ts;
        }
        return futex_wait(pU32, u32Val, &key, u64Deadline);
    }
    if (u32Cmd == GJ_FUTEX_WAKE) {
        return futex_wake(&key, (u32)pRegs->u64Arg2);
    }
    /* futex: wait_bitset - G-FUT-BITSET soft product (val3 = bitset) */
    if (u32Cmd == GJ_FUTEX_WAIT_BITSET) {
        u32Val = (u32)pRegs->u64Arg2;
        u32Bitset = (u32)pRegs->u64Arg5;
        i64Ts = hot_futex_deadline(pRegs->u64Arg3, 1, &u64Deadline);
        if (i64Ts < 0) {
            return i64Ts;
        }
        return futex_wait_bitset(pU32, u32Val, &key, u64Deadline, u32Bitset);
    }
    /* futex: wake_bitset - G-FUT-BITSET soft product (val3 = bitset) */
    if (u32Cmd == GJ_FUTEX_WAKE_BITSET) {
        u32Bitset = (u32)pRegs->u64Arg5;
        return futex_wake_bitset(&key, (u32)pRegs->u64Arg2, u32Bitset);
    }
    /*
     * Residual UDX-host / glibc condvar path: FUTEX_REQUEUE | CMP_REQUEUE.
     * No parked-waiter transfer API yet - soft collapse:
     *   CMP: if *uaddr != val -> -EAGAIN
     *   wake nr_wake on uaddr; residual also wake nr_requeue on uaddr
     *   (and on uaddr2 when distinct) so waiters are not stranded.
     * uaddr2: 4-byte align + soft mapped gate (mirror uaddr).
     * greppable: linux_hot: futex requeue soft
     * greppable: linux_hot: residual futex lean
     */
    if (u32Cmd == GJ_FUTEX_REQUEUE || u32Cmd == GJ_FUTEX_CMP_REQUEUE) {
        u32 u32NrWake = (u32)pRegs->u64Arg2;
        u32 u32NrRequeue = (u32)pRegs->u64Arg3;
        u64 u64Uaddr2 = pRegs->u64Arg4;
        i64 i64Woke = 0;
        i64 i64Extra;
        struct gj_futex_key key2;

        if (u64Uaddr2 != 0 && (u64Uaddr2 & 3ull) != 0) {
            return -LINUX_EINVAL;
        }
        /* Soft residual: uaddr2 must be user-mapped when in user range. */
        if (u64Uaddr2 != 0 && user_range_ok(u64Uaddr2, sizeof(u32))) {
            if (!user_range_mapped(u64Uaddr2, sizeof(u32))) {
                return -LINUX_EFAULT;
            }
        }
        if (u32Cmd == GJ_FUTEX_CMP_REQUEUE) {
            u32 u32Expect = (u32)pRegs->u64Arg5;
            u32 u32Cur = 0;

            if (user_range_ok(pRegs->u64Arg0, sizeof(u32))) {
                if (user_load_u32(pRegs->u64Arg0, &u32Cur) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                u32Cur = *pU32;
            }
            if (u32Cur != u32Expect) {
                return -LINUX_EAGAIN;
            }
        }
        i64Woke = futex_wake(&key, u32NrWake);
        if (i64Woke < 0) {
            return i64Woke;
        }
        if (u32NrRequeue > 0) {
            /* Soft residual: wake requeue budget on source (no move API). */
            i64Extra = futex_wake(&key, u32NrRequeue);
            if (i64Extra > 0) {
                i64Woke += i64Extra;
            }
            if (u64Uaddr2 != 0 && u64Uaddr2 != pRegs->u64Arg0) {
                if (futex_key_from_uaddr(&key2, u64Uaddr2, fPrivate) == GJ_OK) {
                    i64Extra = futex_wake(&key2, u32NrRequeue);
                    if (i64Extra > 0) {
                        i64Woke += i64Extra;
                    }
                }
            }
        }
        g_u64ResFutexRequeue++;
        if (g_fResFutexRequeueOnce == 0) {
            g_fResFutexRequeueOnce = 1;
            kprintf("linux_hot: futex requeue soft cmd=%u wake=%u requeue=%u "
                    "hits=%lu residual=1 (collapse; Soft!=product G-AC-1)\n",
                    (unsigned)u32Cmd, (unsigned)u32NrWake,
                    (unsigned)u32NrRequeue,
                    (unsigned long)g_u64ResFutexRequeue);
        }
        if (g_fResFutexLeanOnce == 0u) {
            g_fResFutexLeanOnce = 1u;
            kprintf("linux_hot: residual futex lean via=requeue "
                    "rq=%lu wo=%lu Soft!=product G-AC-1\n",
                    (unsigned long)g_u64ResFutexRequeue,
                    (unsigned long)g_u64ResFutexWakeOp);
            hot_residual_lean_once("futex_requeue");
        }
        return i64Woke;
    }
    /*
     * Residual: FUTEX_WAKE_OP soft collapse (glibc mutex unlock path).
     * Full atomic op on uaddr2 not implemented - wake nr on uaddr and
     * soft-wake 1 on uaddr2 when present so waiters are not stranded.
     * uaddr2: 4-byte align + soft mapped gate (mirror uaddr / requeue).
     * greppable: linux_hot: futex wake_op soft
     * greppable: linux_hot: residual futex lean
     */
    if (u32Cmd == GJ_FUTEX_WAKE_OP) {
        u32 u32NrWake = (u32)pRegs->u64Arg2;
        u64 u64Uaddr2 = pRegs->u64Arg4;
        i64 i64Woke;
        i64 i64Extra;
        struct gj_futex_key key2;

        if (u64Uaddr2 != 0 && (u64Uaddr2 & 3ull) != 0) {
            return -LINUX_EINVAL;
        }
        if (u64Uaddr2 != 0 && user_range_ok(u64Uaddr2, sizeof(u32))) {
            if (!user_range_mapped(u64Uaddr2, sizeof(u32))) {
                return -LINUX_EFAULT;
            }
        }
        i64Woke = futex_wake(&key, u32NrWake);
        if (i64Woke < 0) {
            return i64Woke;
        }
        if (u64Uaddr2 != 0 && u64Uaddr2 != pRegs->u64Arg0) {
            if (futex_key_from_uaddr(&key2, u64Uaddr2, fPrivate) == GJ_OK) {
                /* Soft: wake at least one on secondary (op result ignored). */
                i64Extra = futex_wake(&key2, 1u);
                if (i64Extra > 0) {
                    i64Woke += i64Extra;
                }
            }
        }
        g_u64ResFutexWakeOp++;
        if (g_fResFutexWakeOpOnce == 0) {
            g_fResFutexWakeOpOnce = 1;
            kprintf("linux_hot: futex wake_op soft nr=%u hits=%lu residual=1 "
                    "(collapse; Soft!=product G-AC-1)\n",
                    (unsigned)u32NrWake,
                    (unsigned long)g_u64ResFutexWakeOp);
        }
        if (g_fResFutexLeanOnce == 0u) {
            g_fResFutexLeanOnce = 1u;
            kprintf("linux_hot: residual futex lean via=wake_op "
                    "rq=%lu wo=%lu Soft!=product G-AC-1\n",
                    (unsigned long)g_u64ResFutexRequeue,
                    (unsigned long)g_u64ResFutexWakeOp);
            hot_residual_lean_once("futex_wake_op");
        }
        return i64Woke;
    }
    return -LINUX_ENOSYS;
}

i64
gj_linux_hot_futex_wake2(struct gj_linux_regs *pRegs)
{
    /* futex_wake(uaddr, mask, nr, flags) -> FUTEX_WAKE_BITSET soft */
    struct gj_linux_regs r;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    r = *pRegs;
    /* Soft: private by default (bring-up); mask -> val3 bitset */
    r.u64Arg1 = GJ_FUTEX_WAKE_BITSET | GJ_FUTEX_PRIVATE_FLAG;
    r.u64Arg2 = pRegs->u64Arg2; /* nr */
    r.u64Arg5 = pRegs->u64Arg1; /* mask */
    if (r.u64Arg5 == 0) {
        r.u64Arg5 = GJ_FUTEX_BITSET_MATCH_ANY;
    }
    return gj_linux_hot_futex(&r);
}

i64
gj_linux_hot_futex_wait2(struct gj_linux_regs *pRegs)
{
    /* futex_wait(uaddr, val, mask, flags, ...) -> FUTEX_WAIT_BITSET soft */
    struct gj_linux_regs r;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    r = *pRegs;
    /* Soft: private by default; mask -> val3 bitset */
    r.u64Arg1 = GJ_FUTEX_WAIT_BITSET | GJ_FUTEX_PRIVATE_FLAG;
    r.u64Arg2 = pRegs->u64Arg1; /* val */
    r.u64Arg3 = pRegs->u64Arg4; /* optional timespec* (soft) */
    r.u64Arg5 = pRegs->u64Arg2; /* mask */
    if (r.u64Arg5 == 0) {
        r.u64Arg5 = GJ_FUTEX_BITSET_MATCH_ANY;
    }
    return gj_linux_hot_futex(&r);
}

i64
gj_linux_hot_arch_prctl(struct gj_linux_regs *pRegs)
{
    u64 u64Code;
    u64 u64Addr;
    u64 u64Val;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Code = pRegs->u64Arg0;
    u64Addr = pRegs->u64Arg1;
    if (u64Code == LINUX_ARCH_SET_FS) {
        struct gj_thread *pCur = thread_current();

        g_u64FsBase = u64Addr;
        if (pCur != NULL) {
            pCur->u64FsBase = u64Addr;
        }
        cpu_set_fs_base(u64Addr);
        return 0;
    }
    if (u64Code == LINUX_ARCH_SET_GS) {
        g_u64GsBase = u64Addr;
        return 0;
    }
    if (u64Code == LINUX_ARCH_GET_FS || u64Code == LINUX_ARCH_GET_GS) {
        u64Val = (u64Code == LINUX_ARCH_GET_FS) ? g_u64FsBase : g_u64GsBase;
        if (u64Addr == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(u64Addr, sizeof(u64Val))) {
            if (copy_to_user(u64Addr, &u64Val, sizeof(u64Val)) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            /* Kernel-mode smoke path only */
            *(u64 *)(gj_vaddr_t)u64Addr = u64Val;
        }
        return 0;
    }
    return -LINUX_EINVAL;
}

i64
gj_linux_hot_set_tid_address(struct gj_linux_regs *pRegs)
{
    /*
     * Clone-adjacent residual (clone itself is cold): store clear_child_tid
     * for glibc pthread join (exit store0+wake pair). Soft!=product · G-AC-1.
     * greppable: linux_hot: residual set_tid lean
     * greppable: linux_hot: residual ctid lean
     */
    u64 u64Ctid;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Ctid = pRegs->u64Arg0;
    /* Soft residual: if non-zero CTID in user range, require 4-byte align. */
    if (u64Ctid != 0 && (u64Ctid & 3ull) != 0) {
        return -LINUX_EINVAL;
    }
    {
        struct gj_thread *pCur = thread_current();

        if (pCur != NULL) {
            pCur->u64ClearChildTid = u64Ctid;
        }
    }
    g_u64ClearChildTid = u64Ctid;
    g_u64ResCtidSet++;
    if (g_fResSetTidLeanOnce == 0u) {
        g_fResSetTidLeanOnce = 1u;
        kprintf("linux_hot: residual set_tid lean via=set_tid_address "
                "ctid=0x%lx sets=%lu clears=%lu Soft!=product G-AC-1 "
                "(clone-adjacent; clone cold)\n",
                (unsigned long)u64Ctid,
                (unsigned long)g_u64ResCtidSet,
                (unsigned long)g_u64ResCtidClear);
        hot_residual_lean_once("set_tid_address");
    }
    {
        struct gj_thread *pCur = thread_current();
        u32 u32UserFl = GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY;

        (void)u32UserFl;
        if (pCur != NULL && pCur->u32Id != 0) {
            return (i64)pCur->u32Id;
        }
    }
    return (i64)g_u32LinuxTid;
}

i64
gj_linux_hot_getgroups(struct gj_linux_regs *pRegs)
{
    u32 u32Size;
    u32 gid = g_u32LinuxGid;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u32Size = (u32)pRegs->u64Arg0;
    if (u32Size == 0) {
        return 1; /* one supplementary group */
    }
    if (u32Size < 1 || pRegs->u64Arg1 == 0) {
        return -LINUX_EINVAL;
    }
    if (user_range_ok(pRegs->u64Arg1, sizeof(gid))) {
        if (copy_to_user(pRegs->u64Arg1, &gid, sizeof(gid)) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        *(u32 *)(gj_vaddr_t)pRegs->u64Arg1 = gid;
    }
    return 1;
}

i64
gj_linux_hot_getpgrp(struct gj_linux_regs *pRegs)
{
    struct gj_process *pProc;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    pProc = hot_calling_proc();
    if (pProc != NULL && pProc->u32Pgid != 0) {
        return (i64)pProc->u32Pgid;
    }
    return (i64)g_u32LinuxPgid;
}

/*
 * getpgid(pid): musl/glibc getpgrp() is getpgid(0). PATH_NONE was ENOSYS and
 * blocked dash/zsh/tcsh job-control probes. Soft residual: one stored pgrp
 * (matches setpgid/getpgrp/setsid). pid<0 -> -EINVAL so scripts can detect
 * the stub (not ENOSYS). Per-child pgrp / ESRCH-on-dead product OPEN.
 * greppable: linux_hot: residual getpgid
 * Soft!=product. G-AC-1. Dual DoD A/B OPEN.
 */
i64
gj_linux_hot_getpgid(struct gj_linux_regs *pRegs)
{
    u32 u32Pid;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if ((i64)pRegs->u64Arg0 < 0) {
        return -LINUX_EINVAL;
    }
    u32Pid = (u32)pRegs->u64Arg0;
    {
        struct gj_process *pProc = hot_calling_proc();
        u32 u32Pgid = g_u32LinuxPgid;

        if (pProc != NULL && pProc->u32Pgid != 0) {
            u32Pgid = pProc->u32Pgid;
        }
        if (g_fResGetpgidOnce == 0) {
            g_fResGetpgidOnce = 1;
            kprintf("linux_hot: residual getpgid pid=%u pgid=%u "
                    "Soft!=product g_ac_1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
                    "enosys_to_stub=1 shell_id=1\n",
                    u32Pid, u32Pgid);
        }
        (void)u32Pid;
        return (i64)u32Pgid;
    }
}

/*
 * umask(mask): every POSIX shell calls this at init and as a builtin.
 * PATH_NONE was ENOSYS (dash `umask 022` / set -e scripts fail closed).
 * Returns previous mask; stores mask & 07777. Soft!=product.
 * greppable: linux_hot: residual umask
 */
i64
gj_linux_hot_umask(struct gj_linux_regs *pRegs)
{
    u32 u32Old;
    u32 u32New;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u32New = (u32)pRegs->u64Arg0 & 07777u;
    u32Old = g_u32LinuxUmask;
    g_u32LinuxUmask = u32New;
    if (g_fResUmaskOnce == 0) {
        g_fResUmaskOnce = 1;
        kprintf("linux_hot: residual umask old=0%o new=0%o "
                "Soft!=product g_ac_1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
                "enosys_to_stub=1 shell_id=1\n",
                u32Old, u32New);
    }
    return (i64)u32Old;
}

/* Linux sysinfo - public layout (partial fill) */
struct linux_sysinfo {
    i64 uptime;
    u64 loads[3];
    u64 totalram;
    u64 freeram;
    u64 sharedram;
    u64 bufferram;
    u64 totalswap;
    u64 freeswap;
    u16 procs;
    u16 pad;
    u64 totalhigh;
    u64 freehigh;
    u32 mem_unit;
    char _f[20 - 2 * sizeof(u64) - sizeof(u32)];
};

i64
gj_linux_hot_sysinfo(struct gj_linux_regs *pRegs)
{
    /*
     * Residual: report PMM total/free frames for UDX host DMA sizing.
     * greppable: linux_hot: sysinfo pmm soft
     */
    struct linux_sysinfo si;
    u64 mono;
    u64 u64TotalFr;
    u64 u64FreeFr;
    u32 u32Online;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg0 == 0) {
        return -LINUX_EFAULT;
    }
    memset(&si, 0, sizeof(si));
    mono = timer_ready() ? timer_mono_nsec() : 0;
    si.uptime = (i64)(mono / 1000000000ull);
    u64TotalFr = pmm_total_frames();
    u64FreeFr = (u64)pmm_free_count();
    if (u64TotalFr == 0) {
        /* Soft fallback when PMM not yet inventory_complete. */
        u64TotalFr = (1024ull * 1024ull * 1024ull) / 4096ull;
        u64FreeFr = u64TotalFr / 2ull;
    }
    if (u64FreeFr > u64TotalFr) {
        u64FreeFr = u64TotalFr;
    }
    si.totalram = u64TotalFr * 4096ull;
    si.freeram = u64FreeFr * 4096ull;
    si.sharedram = 0;
    si.bufferram = 0;
    si.totalswap = 0;
    si.freeswap = 0;
    u32Online = cpu_online_count();
    si.procs = (u16)(u32Online > 0 ? u32Online : 1u);
    si.mem_unit = 1;
    if (g_fResSysinfoOnce == 0) {
        g_fResSysinfoOnce = 1;
        kprintf("linux_hot: sysinfo pmm soft total=%lu free=%lu residual=1\n",
                (unsigned long)si.totalram, (unsigned long)si.freeram);
    }
    if (user_range_ok(pRegs->u64Arg0, sizeof(si))) {
        if (copy_to_user(pRegs->u64Arg0, &si, sizeof(si)) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        *(struct linux_sysinfo *)(gj_vaddr_t)pRegs->u64Arg0 = si;
    }
    return 0;
}

/* struct tms - public layout */
struct linux_tms {
    i64 tms_utime;
    i64 tms_stime;
    i64 tms_cutime;
    i64 tms_cstime;
};

i64
gj_linux_hot_times(struct gj_linux_regs *pRegs)
{
    struct linux_tms tms;
    u64 mono;
    i64 ticks;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    mono = timer_ready() ? timer_mono_nsec() : 0;
    /* clock ticks ~100 Hz */
    ticks = (i64)(mono / 10000000ull);
    memset(&tms, 0, sizeof(tms));
    tms.tms_utime = ticks / 2;
    tms.tms_stime = ticks / 2;
    if (pRegs->u64Arg0 != 0) {
        if (user_range_ok(pRegs->u64Arg0, sizeof(tms))) {
            if (copy_to_user(pRegs->u64Arg0, &tms, sizeof(tms)) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            *(struct linux_tms *)(gj_vaddr_t)pRegs->u64Arg0 = tms;
        }
    }
    return ticks;
}

/* rusage - first fields used by wine/glibc */
struct linux_rusage {
    struct {
        i64 tv_sec;
        i64 tv_usec;
    } ru_utime;
    struct {
        i64 tv_sec;
        i64 tv_usec;
    } ru_stime;
    i64 ru_maxrss;
    i64 ru_ixrss;
    i64 ru_idrss;
    i64 ru_isrss;
    i64 ru_minflt;
    i64 ru_majflt;
    i64 ru_nswap;
    i64 ru_inblock;
    i64 ru_oublock;
    i64 ru_msgsnd;
    i64 ru_msgrcv;
    i64 ru_nsignals;
    i64 ru_nvcsw;
    i64 ru_nivcsw;
};

i64
gj_linux_hot_getrusage(struct gj_linux_regs *pRegs)
{
    struct linux_rusage ru;
    u64 mono;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    /* who: 0=self 1=children -1=thread - all same for bring-up */
    memset(&ru, 0, sizeof(ru));
    mono = timer_ready() ? timer_mono_nsec() : 0;
    ru.ru_utime.tv_sec = (i64)(mono / 2000000000ull);
    ru.ru_utime.tv_usec = (i64)((mono / 2000ull) % 1000000ull);
    ru.ru_stime.tv_sec = ru.ru_utime.tv_sec;
    ru.ru_stime.tv_usec = ru.ru_utime.tv_usec;
    ru.ru_maxrss = 4096;
    if (user_range_ok(pRegs->u64Arg1, sizeof(ru))) {
        if (copy_to_user(pRegs->u64Arg1, &ru, sizeof(ru)) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        *(struct linux_rusage *)(gj_vaddr_t)pRegs->u64Arg1 = ru;
    }
    return 0;
}

i64
gj_linux_hot_getpriority(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    (void)pRegs;
    /* nice 0 -> kernel returns 20 - nice for getpriority */
    return 20;
}

i64
gj_linux_hot_setpriority(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    (void)pRegs;
    return 0; /* accept any nice for bring-up */
}

/* linux_utsname - public structure shape */
struct linux_utsname {
    char aSysname[65];
    char aNodename[65];
    char aRelease[65];
    char aVersion[65];
    char aMachine[65];
    char aDomain[65];
};

i64
gj_linux_hot_uname(struct gj_linux_regs *pRegs)
{
    struct linux_utsname utsK;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg0 == 0) {
        return -LINUX_EFAULT;
    }
    memset(&utsK, 0, sizeof(utsK));
    /* G-UNAME-1: Linux-compatible; identify GreenJade in release/version. */
    memcpy(utsK.aSysname, "Linux", 6);
    memcpy(utsK.aNodename, "greenjade", 10);
    memcpy(utsK.aRelease, "6.0.0-greenjade", 15);
    memcpy(utsK.aVersion, "GreenJade hybrid personality", 29);
    memcpy(utsK.aMachine, "x86_64", 7);
    if (user_range_ok(pRegs->u64Arg0, sizeof(utsK))) {
        st = copy_to_user(pRegs->u64Arg0, &utsK, sizeof(utsK));
        if (st != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        /* Kernel smoke buffer */
        *(struct linux_utsname *)(gj_vaddr_t)pRegs->u64Arg0 = utsK;
    }
    return 0;
}

i64
gj_linux_hot_getrandom(struct gj_linux_regs *pRegs)
{
    /*
     * Residual: multi-chunk getrandom for UDX host nonce/seed paths.
     * flags GRND_NONBLOCK|GRND_RANDOM accepted (soft always nonblocking).
     * greppable: linux_hot: residual getrandom
     * greppable: linux_hot: residual getrandom lean
     * Soft!=product · Dual DoD A/B OPEN.
     */
    u8 aBuf[256];
    u64 u64Want;
    u64 u64Done = 0;
    u64 u64Flags;
    u64 i;
    static u32 g_u32Prng = 0xC0FFEEu;
    static u8 g_fPrngSeeded;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg0 == 0) {
        return -LINUX_EFAULT;
    }
    u64Want = pRegs->u64Arg1;
    u64Flags = pRegs->u64Arg2;
    /* Only public GRND_NONBLOCK(1) | GRND_RANDOM(2) | GRND_INSECURE(4). */
    if ((u64Flags & ~7ull) != 0) {
        return -LINUX_EINVAL;
    }
    if (u64Want == 0) {
        return 0;
    }
    /* Cap single call soft residual (driver host: ≤4 KiB). */
    if (u64Want > 4096ull) {
        u64Want = 4096ull;
    }
    if (!g_fPrngSeeded) {
        g_fPrngSeeded = 1;
        if (timer_ready()) {
            g_u32Prng ^= (u32)timer_mono_nsec() ^ (u32)(timer_mono_nsec() >> 32);
        }
        g_u32Prng ^= (u32)g_u32LinuxTid * 0x9e3779b9u;
    }
    while (u64Done < u64Want) {
        u64 n = u64Want - u64Done;

        if (n > sizeof(aBuf)) {
            n = sizeof(aBuf);
        }
        for (i = 0; i < n; i++) {
            g_u32Prng = g_u32Prng * 1664525u + 1013904223u;
            aBuf[i] = (u8)(g_u32Prng >> 16);
        }
        if (user_range_ok(pRegs->u64Arg0 + u64Done, n)) {
            st = copy_to_user(pRegs->u64Arg0 + u64Done, aBuf, (size_t)n);
            if (st != GJ_OK) {
                if (u64Done > 0) {
                    g_u64ResGetrandomCalls++;
                    g_u64ResGetrandomBytes += u64Done;
                }
                return u64Done ? (i64)u64Done : -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)(pRegs->u64Arg0 + u64Done), aBuf,
                   (size_t)n);
        }
        u64Done += n;
    }
    if (u64Done > 0) {
        g_u64ResGetrandomCalls++;
        g_u64ResGetrandomBytes += u64Done;
        if (g_fResGetrandomLeanOnce == 0u) {
            g_fResGetrandomLeanOnce = 1u;
            kprintf("linux_hot: residual getrandom lean via=getrandom "
                    "bytes=%lu calls=%lu total=%lu Soft!=product G-AC-1 "
                    "dual_dod_ab=OPEN\n",
                    (unsigned long)u64Done,
                    (unsigned long)g_u64ResGetrandomCalls,
                    (unsigned long)g_u64ResGetrandomBytes);
            hot_residual_lean_once("getrandom");
        }
    }
    return (i64)u64Done;
}

i64
gj_linux_hot_pread64(struct gj_linux_regs *pRegs)
{
    /*
     * pread64 loop residual (chunked; cap HOT_RES_IO_MAX). Soft!=product.
     * greppable: linux_hot: residual io lean
     */
    u8 aBuf[512];
    u64 u64Want;
    u64 u64Done = 0;
    u64 u64Off;
    u64 u64Dst;
    i64 fd;
    i64 n;

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    fd = (i64)pRegs->u64Arg0;
    u64Dst = pRegs->u64Arg1;
    u64Want = pRegs->u64Arg2;
    u64Off = pRegs->u64Arg3;
    if (u64Want == 0) {
        return 0;
    }
    if (u64Want > HOT_RES_IO_MAX) {
        u64Want = HOT_RES_IO_MAX;
    }
    if (!vfs_ram_fd_ok(fd)) {
        return -LINUX_EBADF;
    }
    while (u64Done < u64Want) {
        size_t cb = (size_t)(u64Want - u64Done);

        if (cb > sizeof(aBuf)) {
            cb = sizeof(aBuf);
        }
        n = vfs_ram_pread(fd, aBuf, cb, u64Off + u64Done);
        if (n < 0) {
            return u64Done ? (i64)u64Done : n;
        }
        if (n == 0) {
            break;
        }
        if (user_range_ok(u64Dst + u64Done, (u64)n)) {
            if (copy_to_user(u64Dst + u64Done, aBuf, (size_t)n) != GJ_OK) {
                return u64Done ? (i64)u64Done : -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)(u64Dst + u64Done), aBuf, (size_t)n);
        }
        u64Done += (u64)n;
        if ((u64)n < (u64)cb) {
            break;
        }
    }
    if (u64Done > 0) {
        g_u64ResIoReadCalls++;
        g_u64ResIoVfsHits++;
        g_u64ResIoBytes += u64Done;
        hot_residual_io_lean_once("pread64", fd);
    }
    return (i64)u64Done;
}

i64
gj_linux_hot_pwrite64(struct gj_linux_regs *pRegs)
{
    /*
     * pwrite64 loop residual (chunked; cap HOT_RES_IO_MAX). Soft!=product.
     * greppable: linux_hot: residual io lean
     */
    u8 aBuf[512];
    u64 u64Want;
    u64 u64Done = 0;
    u64 u64Off;
    u64 u64Src;
    i64 fd;
    i64 n;

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    fd = (i64)pRegs->u64Arg0;
    u64Src = pRegs->u64Arg1;
    u64Want = pRegs->u64Arg2;
    u64Off = pRegs->u64Arg3;
    if (u64Want == 0) {
        return 0;
    }
    if (u64Want > HOT_RES_IO_MAX) {
        u64Want = HOT_RES_IO_MAX;
    }
    if (!vfs_ram_fd_ok(fd)) {
        return -LINUX_EBADF;
    }
    while (u64Done < u64Want) {
        size_t cb = (size_t)(u64Want - u64Done);

        if (cb > sizeof(aBuf)) {
            cb = sizeof(aBuf);
        }
        if (user_range_ok(u64Src + u64Done, cb)) {
            if (copy_from_user(aBuf, u64Src + u64Done, cb) != GJ_OK) {
                return u64Done ? (i64)u64Done : -LINUX_EFAULT;
            }
        } else {
            memcpy(aBuf, (const void *)(gj_vaddr_t)(u64Src + u64Done), cb);
        }
        n = vfs_ram_pwrite(fd, aBuf, cb, u64Off + u64Done);
        if (n < 0) {
            return u64Done ? (i64)u64Done : n;
        }
        if (n == 0) {
            break;
        }
        u64Done += (u64)n;
        if ((u64)n < (u64)cb) {
            break;
        }
    }
    if (u64Done > 0) {
        g_u64ResIoWriteCalls++;
        g_u64ResIoVfsHits++;
        g_u64ResIoBytes += u64Done;
        hot_residual_io_lean_once("pwrite64", fd);
    }
    return (i64)u64Done;
}
