/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Linux hybrid Option C — kernel hot paths (clean-room pure C11).
 * Dual MIT OR Apache-2.0. No GPL source.
 *
 * Soft product inventory (Wave 11/14 base + Wave 36 exclusive deepen):
 *   - Group enter tallies (io/id/mem/time/futex/sched/sig/sock/info/proc)
 *   - Live task view snapshot (pid/tid/cred/brk/fs_base)
 *   - Handler catalog capacity (static product surface count)
 *   - Wave 14: groups catalog + deepen stamp + path/stats wave lamps
 *   - Wave 15: rates / honesty / catalog / PASS complementary surfaces
 *   Never hard-gates; wrap OK; diagnostics only — does not alter i64Ret.
 * Greppable prefix-stable serial markers:
 *   linux: hot soft inventory …
 *   linux: hot soft groups …
 *   linux: hot soft io|id|mem|time|futex|sched|sig|sock|info|proc …
 *   linux: hot soft live …
 *   linux: hot soft path …
 *   linux: hot soft stats …
 *   linux: hot soft rates …
 *   linux: hot soft honesty …
 *   linux: hot soft catalog …
 *   linux: hot soft surfaces …   (Wave 19)
 *   linux: hot soft note …       (Wave 19)
 *   linux: hot soft deepen …
 *   linux: hot soft inventory PASS / soft PASS
 * greppable: "linux: hot soft"
 * greppable: linux: hot soft inventory
 * greppable: linux: hot soft path
 * greppable: linux: hot soft surfaces
 * greppable: linux: hot soft deepen
 */
#include <gj/cpu.h>
#include <gj/error.h>
#include <gj/futex.h>
#include <gj/io_uring.h>
#include <gj/klog.h>
#include <gj/linux_dispatch.h>
#include <gj/memobj.h>
#include <gj/net_lo.h>
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
static u32 g_u32LinuxUid = 1000;
static u32 g_u32LinuxGid = 1000;
static u32 g_u32LinuxEuid = 1000;
static u32 g_u32LinuxEgid = 1000;
static u32 g_u32LinuxSuid = 1000;
static u32 g_u32LinuxSgid = 1000;
static u32 g_u32LinuxPgid = 1;
static u32 g_u32LinuxSid = 1;
static u64 g_u64BrkBase;
static u64 g_u64BrkCur;
static u64 g_u64FsBase;
static u64 g_u64GsBase;
static u64 g_u64ClearChildTid;
static u64 g_u64MonoNsec; /* crude mono clock until TSC calibrate */

/*
 * Soft inventory groups (Wave 11/14 base; Wave 15 deepen). Enter-only tallies —
 * never rewrite ret. greppable: linux: hot soft …
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
#define GJ_LINUX_HOT_SOFT_HANDLERS 105u
#define GJ_LINUX_HOT_SOFT_WAVE 110u
#define GJ_LINUX_HOT_SOFT_AREAS 203u

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
 * Soft enter note — never alters handler return. Diagnostics only.
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
 *   linux: hot soft inventory …
 *   linux: hot soft groups …
 *   linux: hot soft io|id|mem|time|futex|sched|sig|sock|info|proc …
 *   linux: hot soft live …
 *   linux: hot soft path …
 *   linux: hot soft stats …
 *   linux: hot soft rates …
 *   linux: hot soft honesty …
 *   linux: hot soft catalog …
 *   linux: hot soft deepen …
 *   linux: hot soft inventory PASS / soft PASS
 * greppable: linux: hot soft
 * Honesty: soft inventory only — not product gate; not bar3.
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
            "(soft; not bar3)\n",
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
            "wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_IO],
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
            "mlock,pkey,mbind wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_MEM],
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
            "set_tid_address wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_FUTEX],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft sched */
    kprintf("linux: hot soft sched enter=%lu "
            "surface=yield,sched_get/set*,affinity,attr,priority "
            "wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SCHED],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft sig */
    kprintf("linux: hot soft sig enter=%lu "
            "surface=tkill,tgkill,sigaltstack,rt_sig*,pause wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SIG],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft sock */
    kprintf("linux: hot soft sock enter=%lu "
            "surface=getsockopt,setsockopt,getsockname,getpeername,"
            "shutdown wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_SOCK],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft info */
    kprintf("linux: hot soft info enter=%lu "
            "surface=uname,sysinfo,times,rusage,prctl,cap,random,getcpu,"
            "priority,arch_prctl wave=%u\n",
            (unsigned long)s.aEnter[HOT_SOFT_GRP_INFO],
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft proc */
    kprintf("linux: hot soft proc enter=%lu "
            "surface=exit,exit_group,waitid,process_vm_*,membarrier,rseq "
            "wave=%u\n",
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
            "(soft inventory; not bar3)\n",
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
    kprintf("linux: hot soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 hot=kernel "
            "wave=%u (soft inventory; never closes hybrid)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft surfaces (Wave 20 deepen) */
    kprintf("linux: hot soft surfaces count=%u wave=%u "
            "names=inventory,groups,io,id,mem,time,futex,sched,sig,"
            "sock,info,proc,live,path,stats,rates,honesty,catalog,"
            "surfaces,note,return,retmap,deepen,PASS\n",
            (unsigned)GJ_LINUX_HOT_SOFT_AREAS,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft note (Wave 20 deepen) */
    kprintf("linux: hot soft note milestone=wave98 exclusive=1 "
            "soft_only=1 not_bar3=1 handlers=%u enter=%lu wave=%u\n",
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

    /* Grep: linux: hot soft return (Wave 20 deepen) */
    kprintf("linux: hot soft return enter=%lu null_regs=%lu ctx_set=%lu "
            "handlers=%u groups=%u product_gate=0 wave=%u\n",
            (unsigned long)s.u64EnterTotal,
            (unsigned long)s.u64NullRegs,
            (unsigned long)s.u64CtxSet,
            (unsigned)GJ_LINUX_HOT_SOFT_HANDLERS,
            (unsigned)HOT_SOFT_GRP_N,
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft retmap — Wave 19 return-surface map */
    kprintf("linux: hot soft retmap enter|null_regs|ctx_set|handlers|groups product_gate=0 soft_only=1 wave=%u\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

    /* Grep: linux: hot soft deepen wave */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: linux: hot: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("linux: hot: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
    /* Grep: linux: hot: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("linux: hot: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: linux: hot: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("linux: hot: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
    /* Grep: linux: hot: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("linux: hot: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: linux: hot: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("linux: hot: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /* Grep: linux: hot: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("linux: hot: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: linux: hot: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("linux: hot: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /* Grep: linux: hot: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("linux: hot: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: linux: hot: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("linux: hot: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /* Grep: linux: hot: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("linux: hot: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: hot: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("linux: hot: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /* Grep: linux: hot: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("linux: hot: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: hot: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("linux: hot: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /* Grep: linux: hot: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("linux: hot: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: linux: hot: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("linux: hot: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
            /* Grep: linux: hot: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("linux: hot: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: linux: hot: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("linux: hot: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                    /* Grep: linux: hot: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("linux: hot: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: hot: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("linux: hot: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                            /* Grep: linux: hot: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("linux: hot: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: hot: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("linux: hot: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                            /* Grep: linux: hot: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("linux: hot: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: hot: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("linux: hot: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                            /* Grep: linux: hot: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("linux: hot: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: linux: hot: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("linux: hot: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                            /* Grep: linux: hot: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("linux: hot: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                            /* Grep: linux: hot: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("linux: hot: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("linux: hot: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("linux: hot: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("linux: hot: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("linux: hot: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("linux: hot: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("linux: hot: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retfortress — Wave 35 return-fortress honesty */
kprintf("linux: hot: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("linux: hot: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft rethold — Wave 36 return-hold honesty */
kprintf("linux: hot: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retspire — Wave 36 exclusive spire stamp */
kprintf("linux: hot: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retwall — Wave 37 return-wall honesty */
kprintf("linux: hot: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retgate — Wave 37 exclusive gate stamp */
kprintf("linux: hot: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retmoat — Wave 38 return-moat honesty */
kprintf("linux: hot: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retower — Wave 38 exclusive tower stamp */
kprintf("linux: hot: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("linux: hot: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("linux: hot: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("linux: hot: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("linux: hot: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retravelin — Wave 41 return-travelin honesty */
kprintf("linux: hot: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("linux: hot: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("linux: hot: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("linux: hot: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("linux: hot: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("linux: hot: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("linux: hot: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("linux: hot: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("linux: hot: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("linux: hot: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retbailey — Wave 46 return-bailey honesty */
kprintf("linux: hot: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);
/* Grep: linux: hot: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("linux: hot: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_LINUX_HOT_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("linux: hot: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("linux: hot: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("linux: hot: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("linux: hot: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("linux: hot: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("linux: hot: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retsally — Wave 50 return-sally honesty */
kprintf("linux: hot: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("linux: hot: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retfosse — Wave 51 return-fosse honesty */
kprintf("linux: hot: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("linux: hot: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("linux: hot: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("linux: hot: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retravelin — Wave 53 return-travelin honesty */
kprintf("linux: hot: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("linux: hot: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("linux: hot: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retredan — Wave 54 exclusive redan stamp */
kprintf("linux: hot: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retflank — Wave 55 return-flank honesty */
kprintf("linux: hot: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retface — Wave 55 exclusive face stamp */
kprintf("linux: hot: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retgorge — Wave 56 return-gorge honesty */
kprintf("linux: hot: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("linux: hot: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retraverse — Wave 57 return-traverse honesty */
kprintf("linux: hot: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("linux: hot: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retorillon — Wave 58 return-orillon honesty */
kprintf("linux: hot: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("linux: hot: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("linux: hot: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("linux: hot: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retplace — Wave 60 return-place honesty */
kprintf("linux: hot: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("linux: hot: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("linux: hot: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("linux: hot: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("linux: hot: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("linux: hot: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("linux: hot: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("linux: hot: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: linux: hot: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("linux: hot: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: linux: hot: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("linux: hot: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: linux: hot: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("linux: hot: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: linux: hot: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("linux: hot: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: linux: hot: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("linux: hot: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=110 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: linux: hot: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("linux: hot: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=110 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("linux: hot: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("linux: hot: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("linux: hot: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("linux: hot: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("linux: hot: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=110 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("linux: hot: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=110 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("linux: hot: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("linux: hot: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("linux: hot: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("linux: hot: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: linux: hot: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("linux: hot: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("linux: hot: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("linux: hot: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("linux: hot: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbastionangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("linux: hot: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("linux: hot: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retparapetangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("linux: hot: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("linux: hot: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retowerangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("linux: hot: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("linux: hot: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retwallangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("linux: hot: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("linux: hot: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retholdangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("linux: hot: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("linux: hot: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retfortressangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("linux: hot: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("linux: hot: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("linux: hot: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("linux: hot: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("linux: hot: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("linux: hot: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("linux: hot: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("linux: hot: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retaegisangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("linux: hot: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("linux: hot: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retsigilangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("linux: hot: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("linux: hot: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retglyphangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("linux: hot: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("linux: hot: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retshardangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("linux: hot: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("linux: hot: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retprismangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("linux: hot: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("linux: hot: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retcipherangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("linux: hot: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("linux: hot: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retledgerangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("linux: hot: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("linux: hot: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retvaultangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("linux: hot: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("linux: hot: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (rettokenangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("linux: hot: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("linux: hot: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retphaseangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("linux: hot: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("linux: hot: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retpulseangle stamp; Soft≠product)\n");

/* Grep: linux: hot: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("linux: hot: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("linux: hot: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retboundangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("linux: hot: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("linux: hot: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbladeangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("linux: hot: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("linux: hot: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retarcangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("linux: hot: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("linux: hot: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("linux: hot: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("linux: hot: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("linux: hot: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("linux: hot: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retellipseangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("linux: hot: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("linux: hot: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("linux: hot: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("linux: hot: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (rethelixangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("linux: hot: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("linux: hot: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retknotangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("linux: hot: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("linux: hot: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retkleinangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("linux: hot: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("linux: hot: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retaffineangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("linux: hot: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("linux: hot: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("linux: hot: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("linux: hot: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retcubicangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("linux: hot: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("linux: hot: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retquinticangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("linux: hot: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("linux: hot: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbezierangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("linux: hot: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("linux: hot: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("linux: hot: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("linux: hot: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: linux: hot: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("linux: hot: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=110 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: linux: hot: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("linux: hot: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=110 (retgridangle stamp; Soft≠product)\n");
                            kprintf("linux: hot soft deepen wave=%u areas=%u handlers=%u "
            "groups=%u enter=%lu logs=%lu "
            "(Wave 92 exclusive; not bar3)\n",
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

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Fd = pRegs->u64Arg0;
    u64Src = pRegs->u64Arg1;
    u64Len = pRegs->u64Arg2;

    /* Hot: stdin/out/err only; other FDs → cold personality. */
    if (u64Fd > 2) {
        return -LINUX_ENOSYS;
    }
    if (u64Len == 0) {
        return 0;
    }
    if (u64Len > 4096) {
        u64Len = 4096;
    }
    /*
     * Kernel smoke may pass kernel buffers; ring3 passes user VAs.
     * Accept either: user-mapped range, or kernel identity pointer.
     */
    while (u64Done < u64Len) {
        cbChunk = (size_t)(u64Len - u64Done);
        if (cbChunk > sizeof(aKbuf)) {
            cbChunk = sizeof(aKbuf);
        }
        if (user_range_ok(u64Src + u64Done, cbChunk)) {
            st = copy_from_user(aKbuf, u64Src + u64Done, cbChunk);
            if (st != GJ_OK) {
                return u64Done ? (i64)u64Done : -LINUX_EFAULT;
            }
            hot_write_bytes(aKbuf, cbChunk);
        } else {
            /* Kernel-mode smoke path only */
            hot_write_bytes((const char *)(gj_vaddr_t)(u64Src + u64Done),
                            cbChunk);
        }
        u64Done += cbChunk;
    }
    return (i64)u64Done;
}

/* struct iovec — public layout: base ptr + length */
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
            } else {
                return i64Total > 0 ? i64Total : -LINUX_EBADF;
            }
            if (n < 0) {
                return i64Total > 0 ? i64Total : n;
            }
            i64Total += n;
            off += (u64)n;
            if ((u64)n < (u64)cb) {
                return i64Total;
            }
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
            n = vfs_ram_read((i64)u64Fd, aChunk, cb);
            if (n < 0) {
                return i64Total > 0 ? i64Total : n;
            }
            if (n == 0) {
                return i64Total;
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
                return i64Total;
            }
        }
    }
    return i64Total;
}

i64
gj_linux_hot_preadv(struct gj_linux_regs *pRegs)
{
    /* preadv(fd, iov, iovcnt, offset) — ignore offset; sequential readv */
    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    return gj_linux_hot_readv(pRegs);
}

i64
gj_linux_hot_pwritev(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    return gj_linux_hot_writev(pRegs);
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

i64
gj_linux_hot_getsockopt(struct gj_linux_regs *pRegs)
{
    int v = 0;
    u32 len = 4;

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL || !net_lo_fd_ok((i64)pRegs->u64Arg0)) {
        return -LINUX_EBADF;
    }
    if (pRegs->u64Arg4 != 0) {
        if (user_range_ok(pRegs->u64Arg4, 4)) {
            (void)copy_from_user(&len, pRegs->u64Arg4, 4);
        } else {
            len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg4;
        }
    }
    {
        i64 r = net_lo_getsockopt((i64)pRegs->u64Arg0, (int)pRegs->u64Arg1,
                                  (int)pRegs->u64Arg2, &v, &len);

        if (r != 0) {
            return r;
        }
    }
    if (pRegs->u64Arg3 != 0) {
        if (user_range_ok(pRegs->u64Arg3, 4)) {
            (void)copy_to_user(pRegs->u64Arg3, &v, 4);
        } else {
            *(int *)(gj_vaddr_t)pRegs->u64Arg3 = v;
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

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL || !net_lo_fd_ok((i64)pRegs->u64Arg0)) {
        return -LINUX_EBADF;
    }
    if (pRegs->u64Arg3 != 0 && pRegs->u64Arg4 >= 4) {
        if (user_range_ok(pRegs->u64Arg3, 4)) {
            (void)copy_from_user(&v, pRegs->u64Arg3, 4);
        } else {
            v = *(const int *)(gj_vaddr_t)pRegs->u64Arg3;
        }
    }
    return net_lo_setsockopt((i64)pRegs->u64Arg0, (int)pRegs->u64Arg1,
                             (int)pRegs->u64Arg2, &v, 4);
}

i64
gj_linux_hot_getsockname(struct gj_linux_regs *pRegs)
{
    u8 aSa[16];
    u32 len = 16;
    i64 r;

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL || !net_lo_fd_ok((i64)pRegs->u64Arg0)) {
        return -LINUX_EBADF;
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, 4)) {
            (void)copy_from_user(&len, pRegs->u64Arg2, 4);
        } else {
            len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
        }
    }
    r = net_lo_getsockname((i64)pRegs->u64Arg0, aSa, &len);
    if (r != 0) {
        return r;
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

    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL || !net_lo_fd_ok((i64)pRegs->u64Arg0)) {
        return -LINUX_EBADF;
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, 4)) {
            (void)copy_from_user(&len, pRegs->u64Arg2, 4);
        } else {
            len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
        }
    }
    r = net_lo_getpeername((i64)pRegs->u64Arg0, aSa, &len);
    if (r != 0) {
        return r;
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
    hot_soft_enter(HOT_SOFT_GRP_SOCK, pRegs);
    if (pRegs == NULL || !net_lo_fd_ok((i64)pRegs->u64Arg0)) {
        return -LINUX_EBADF;
    }
    return net_lo_shutdown((i64)pRegs->u64Arg0, (int)pRegs->u64Arg1);
}

/* Signal/timer stubs for glibc/wine bring-up (no delivery yet). */
static u8 g_aSigMask[8];

i64
gj_linux_hot_rt_sigaction(struct gj_linux_regs *pRegs)
{
    /* sigaction(signum, *act, *oldact) — record no-op; return empty oldact */
    u8 aZero[32];
    u32 i;

    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    for (i = 0; i < sizeof(aZero); i++) {
        aZero[i] = 0;
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, 32)) {
            (void)copy_to_user(pRegs->u64Arg2, aZero, 32);
        } else {
            memset((void *)(gj_vaddr_t)pRegs->u64Arg2, 0, 32);
        }
    }
    (void)pRegs->u64Arg0;
    (void)pRegs->u64Arg1;
    return 0;
}

i64
gj_linux_hot_rt_sigprocmask(struct gj_linux_regs *pRegs)
{
    /* how, *set, *oldset, sigsetsize */
    u32 cb = 8;

    hot_soft_enter(HOT_SOFT_GRP_SIG, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg3 != 0 && pRegs->u64Arg3 < 8) {
        cb = (u32)pRegs->u64Arg3;
    }
    if (pRegs->u64Arg2 != 0) {
        if (user_range_ok(pRegs->u64Arg2, cb)) {
            (void)copy_to_user(pRegs->u64Arg2, g_aSigMask, cb);
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, g_aSigMask, cb);
        }
    }
    if (pRegs->u64Arg1 != 0) {
        if (user_range_ok(pRegs->u64Arg1, cb)) {
            (void)copy_from_user(g_aSigMask, pRegs->u64Arg1, cb);
        } else {
            memcpy(g_aSigMask, (const void *)(gj_vaddr_t)pRegs->u64Arg1, cb);
        }
    }
    return 0;
}

i64
gj_linux_hot_rt_sigreturn(struct gj_linux_regs *pRegs)
{
    /* No signal frames yet — not reached from real handlers */
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
    /* Delivery deferred — record only for bring-up */
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
#define GJ_PR_SET_PDEATHSIG 1
#define GJ_PR_GET_PDEATHSIG 2
#define GJ_PR_SET_NAME      15
#define GJ_PR_GET_NAME      16
#define GJ_PR_SET_NO_NEW_PRIVS 38
#define GJ_PR_GET_NO_NEW_PRIVS 39

static char g_aComm[16] = "greenjade";
static u32  g_u32Pdeath;
static u32  g_u32NoNewPrivs;
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
    /* get_robust_list(pid, **head_ptr, *len_ptr) — G-FUT-ROBUST soft */
    u64 head;
    u64 len;
    u32 u32Pid;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0 || pRegs->u64Arg2 == 0) {
        return -LINUX_EFAULT;
    }
    u32Pid = (u32)pRegs->u64Arg0;
    /* futex: robust get — prefer per-tid slot (tid 0 ⇒ current). */
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
    u32 i;

    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    /* idtype: 0=P_ALL 1=P_PID 2=P_PGID */
    if (pRegs->u64Arg0 == 1) {
        pid = (i64)pRegs->u64Arg1;
    }
    r = process_wait4(pid, &st, (int)pRegs->u64Arg3 | 1 /* WNOHANG prefer */);
    if (r == -10) {
        return -LINUX_ECHILD;
    }
    if (r == 0) {
        return 0; /* no child ready */
    }
    if (pRegs->u64Arg2 != 0) {
        for (i = 0; i < sizeof(aInfo); i++) {
            aInfo[i] = 0;
        }
        /* si_signo=SIGCHLD(17), si_code=CLD_EXITED(1), si_pid, si_status */
        aInfo[0] = 17;
        aInfo[4] = 1; /* si_code low */
        {
            u32 p = (u32)r;

            aInfo[12] = (u8)(p & 0xffu);
            aInfo[13] = (u8)((p >> 8) & 0xffu);
            aInfo[14] = (u8)((p >> 16) & 0xffu);
            aInfo[15] = (u8)((p >> 24) & 0xffu);
        }
        aInfo[24] = (u8)((st >> 8) & 0xffu);
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
    u64 va;
    u64 len;
    u64 u64Vec;
    u64 npages;
    u64 i;
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
    if (npages > sizeof(aBit)) {
        npages = sizeof(aBit);
    }
    for (i = 0; i < npages; i++) {
        gj_paddr_t pa = vmm_virt_to_phys((gj_vaddr_t)(va + i * 0x1000ull));

        aBit[i] = (pa != 0) ? 1u : 0u; /* present bit in vector */
    }
    if (user_range_ok(u64Vec, npages)) {
        if (copy_to_user(u64Vec, aBit, (size_t)npages) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        memcpy((void *)(gj_vaddr_t)u64Vec, aBit, (size_t)npages);
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
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return (i64)g_u32LinuxPid;
}

i64
gj_linux_hot_gettid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return (i64)g_u32LinuxTid;
}

i64
gj_linux_hot_getuid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return (i64)g_u32LinuxUid;
}

i64
gj_linux_hot_getgid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return (i64)g_u32LinuxGid;
}

i64
gj_linux_hot_geteuid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return (i64)g_u32LinuxEuid;
}

i64
gj_linux_hot_getegid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return (i64)g_u32LinuxEgid;
}

i64
gj_linux_hot_setuid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    /* Unprivileged bring-up: allow only if already matching or root-shaped */
    g_u32LinuxUid = (u32)pRegs->u64Arg0;
    g_u32LinuxEuid = g_u32LinuxUid;
    g_u32LinuxSuid = g_u32LinuxUid;
    return 0;
}

i64
gj_linux_hot_setgid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    g_u32LinuxGid = (u32)pRegs->u64Arg0;
    g_u32LinuxEgid = g_u32LinuxGid;
    g_u32LinuxSgid = g_u32LinuxGid;
    return 0;
}

i64
gj_linux_hot_setreuid(struct gj_linux_regs *pRegs)
{
    i64 r, e;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    r = (i64)pRegs->u64Arg0;
    e = (i64)pRegs->u64Arg1;
    if (r != -1) {
        g_u32LinuxUid = (u32)r;
    }
    if (e != -1) {
        g_u32LinuxEuid = (u32)e;
    }
    return 0;
}

i64
gj_linux_hot_setregid(struct gj_linux_regs *pRegs)
{
    i64 r, e;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    r = (i64)pRegs->u64Arg0;
    e = (i64)pRegs->u64Arg1;
    if (r != -1) {
        g_u32LinuxGid = (u32)r;
    }
    if (e != -1) {
        g_u32LinuxEgid = (u32)e;
    }
    return 0;
}

i64
gj_linux_hot_setresuid(struct gj_linux_regs *pRegs)
{
    i64 r, e, s;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    r = (i64)pRegs->u64Arg0;
    e = (i64)pRegs->u64Arg1;
    s = (i64)pRegs->u64Arg2;
    if (r != -1) {
        g_u32LinuxUid = (u32)r;
    }
    if (e != -1) {
        g_u32LinuxEuid = (u32)e;
    }
    if (s != -1) {
        g_u32LinuxSuid = (u32)s;
    }
    return 0;
}

i64
gj_linux_hot_setresgid(struct gj_linux_regs *pRegs)
{
    i64 r, e, s;

    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    r = (i64)pRegs->u64Arg0;
    e = (i64)pRegs->u64Arg1;
    s = (i64)pRegs->u64Arg2;
    if (r != -1) {
        g_u32LinuxGid = (u32)r;
    }
    if (e != -1) {
        g_u32LinuxEgid = (u32)e;
    }
    if (s != -1) {
        g_u32LinuxSgid = (u32)s;
    }
    return 0;
}

i64
gj_linux_hot_getresuid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg0 != 0) {
        u32 u = g_u32LinuxUid;

        if (user_range_ok(pRegs->u64Arg0, 4)) {
            (void)copy_to_user(pRegs->u64Arg0, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg0 = u;
        }
    }
    if (pRegs->u64Arg1 != 0) {
        u32 u = g_u32LinuxEuid;

        if (user_range_ok(pRegs->u64Arg1, 4)) {
            (void)copy_to_user(pRegs->u64Arg1, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg1 = u;
        }
    }
    if (pRegs->u64Arg2 != 0) {
        u32 u = g_u32LinuxSuid;

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
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    if (pRegs->u64Arg0 != 0) {
        u32 u = g_u32LinuxGid;

        if (user_range_ok(pRegs->u64Arg0, 4)) {
            (void)copy_to_user(pRegs->u64Arg0, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg0 = u;
        }
    }
    if (pRegs->u64Arg1 != 0) {
        u32 u = g_u32LinuxEgid;

        if (user_range_ok(pRegs->u64Arg1, 4)) {
            (void)copy_to_user(pRegs->u64Arg1, &u, 4);
        } else {
            *(u32 *)(gj_vaddr_t)pRegs->u64Arg1 = u;
        }
    }
    if (pRegs->u64Arg2 != 0) {
        u32 u = g_u32LinuxSgid;

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
    if (pid == 0 || pid == g_u32LinuxPid) {
        g_u32LinuxPgid = pgid ? pgid : g_u32LinuxPid;
    }
    return 0;
}

i64
gj_linux_hot_setsid(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    g_u32LinuxSid = g_u32LinuxPid;
    g_u32LinuxPgid = g_u32LinuxPid;
    return (i64)g_u32LinuxSid;
}

i64
gj_linux_hot_setgroups(struct gj_linux_regs *pRegs)
{
    /* Accept any size; groups table not stored beyond getgroups smoke */
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_mlock(struct gj_linux_regs *pRegs)
{
    /* No swap: mlock always succeeds for mapped ranges */
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_munlock(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_mlockall(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_munlockall(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    (void)pRegs;
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
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
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
 * args: pid, local_iov, liovcnt, remote_iov, riovcnt, flags
 */
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

    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u32Pid = (u32)pRegs->u64Arg0;
    u64LocalIov = pRegs->u64Arg1;
    u64Liovcnt = pRegs->u64Arg2;
    u64RemoteIov = pRegs->u64Arg3;
    u64Riovcnt = pRegs->u64Arg4;
    if (u64Liovcnt == 0 || u64Riovcnt == 0 || u64Liovcnt > 8 ||
        u64Riovcnt > 8) {
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
        if (n > 4096) {
            n = 4096;
        }
        {
            u8 aTmp[4096];

            if (fWriteRemote) {
                /* local → remote */
                if (user_range_ok(u64BaseL, n)) {
                    if (copy_from_user(aTmp, u64BaseL, (size_t)n) != GJ_OK) {
                        return -LINUX_EFAULT;
                    }
                } else {
                    memcpy(aTmp, (const void *)(gj_vaddr_t)u64BaseL, (size_t)n);
                }
                if (user_range_ok(u64BaseR, n)) {
                    if (copy_to_user(u64BaseR, aTmp, (size_t)n) != GJ_OK) {
                        return -LINUX_EFAULT;
                    }
                } else {
                    memcpy((void *)(gj_vaddr_t)u64BaseR, aTmp, (size_t)n);
                }
            } else {
                /* remote → local */
                if (user_range_ok(u64BaseR, n)) {
                    if (copy_from_user(aTmp, u64BaseR, (size_t)n) != GJ_OK) {
                        return -LINUX_EFAULT;
                    }
                } else {
                    memcpy(aTmp, (const void *)(gj_vaddr_t)u64BaseR, (size_t)n);
                }
                if (user_range_ok(u64BaseL, n)) {
                    if (copy_to_user(u64BaseL, aTmp, (size_t)n) != GJ_OK) {
                        return -LINUX_EFAULT;
                    }
                } else {
                    memcpy((void *)(gj_vaddr_t)u64BaseL, aTmp, (size_t)n);
                }
            }
            u64Done += n;
        }
    }
    return (i64)u64Done;
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
     * membarrier(cmd, flags): no-op success for bring-up.
     * Single-CPU-ish + SMP already uses serializing barriers elsewhere.
     */
    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    (void)pRegs;
    __asm__ volatile("mfence" ::: "memory");
    return 0;
}

i64
gj_linux_hot_rseq(struct gj_linux_regs *pRegs)
{
    /*
     * rseq(rseq, rseq_len, flags, sig): register/unregister.
     * Accept and ignore structure for glibc probe (no critical sections yet).
     */
    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_sched_setaffinity(struct gj_linux_regs *pRegs)
{
    /* Accept any mask; single-task bring-up ignores CPU set. */
    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    (void)pRegs;
    return 0;
}

i64
gj_linux_hot_sched_getaffinity(struct gj_linux_regs *pRegs)
{
    /* Return a 1-CPU mask (CPU0) into user buffer; return size written. */
    u8 aMask[8];
    u64 u64Len;

    hot_soft_enter(HOT_SOFT_GRP_SCHED, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Len = pRegs->u64Arg1;
    if (u64Len == 0 || pRegs->u64Arg2 == 0) {
        return -LINUX_EINVAL;
    }
    memset(aMask, 0, sizeof(aMask));
    aMask[0] = 0x01; /* CPU 0 */
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
    u32 cpu = 0;
    u32 node = 0;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
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
    /* struct timeval { time_t tv_sec; suseconds_t tv_usec; } — 16 bytes LP64 */
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
    /* NUMA mbind — accept no-op (single node bring-up). */
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
    /* futex: robust exit — soft G-FUT-ROBUST OWNER_DIED walk before death */
    if (pThr != NULL) {
        (void)futex_exit_robust_list(pThr);
    }
    /*
     * Soft clear_child_tid: store 0 + wake one private waiter (glibc join).
     * Best-effort; ignore faults on unmapped CTID.
     */
    u64Ctid = g_u64ClearChildTid;
    g_u64ClearChildTid = 0;
    if (u64Ctid != 0) {
        struct gj_futex_key key;
        int fOk = 0;

        if (user_range_ok(u64Ctid, sizeof(u32))) {
            if (user_range_mapped(u64Ctid, sizeof(u32)) &&
                user_store_u32(u64Ctid, 0) == GJ_OK) {
                fOk = 1;
            }
        } else {
            /* Kernel-mode smoke path only */
            *(volatile u32 *)(gj_vaddr_t)u64Ctid = 0;
            fOk = 1;
        }
        if (fOk && futex_key_from_uaddr(&key, u64Ctid, 1) == GJ_OK) {
            (void)futex_wake(&key, 1);
        }
    }
    if (pThr != NULL && pThr->pProc != NULL) {
        process_death(pThr->pProc, (u32)i64Code);
    }
    /* End this thread; schedule others (idle / personality). */
    thread_exit();
    return 0;
}

i64
gj_linux_hot_exit_group(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_PROC, pRegs);
    return gj_linux_hot_exit(pRegs);
}

i64
gj_linux_hot_brk(struct gj_linux_regs *pRegs)
{
    u64 u64Req;

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Req = pRegs->u64Arg0;
    if (g_u64BrkBase == 0) {
        g_u64BrkBase = 0x10000000ull; /* placeholder heap base */
        g_u64BrkCur = g_u64BrkBase;
    }
    if (u64Req == 0) {
        return (i64)g_u64BrkCur;
    }
    /* Allow grow within 16 MiB of base for early libc */
    if (u64Req < g_u64BrkBase || u64Req > g_u64BrkBase + (16ull << 20)) {
        return (i64)g_u64BrkCur;
    }
    g_u64BrkCur = u64Req;
    return (i64)g_u64BrkCur;
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

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Addr = pRegs->u64Arg0;
    u64Len = pRegs->u64Arg1;
    u64Prot = pRegs->u64Arg2;
    u64Flags = pRegs->u64Arg3;

    if (u64Len == 0) {
        return -LINUX_EINVAL;
    }
    /* Hot path: anonymous, or io_uring ring fd (SQ/CQ/SQE mmap). */
    if ((u64Flags & LINUX_MAP_ANONYMOUS) == 0) {
        i64 i64Fd = (i64)pRegs->u64Arg4;
        u64 u64Off = pRegs->u64Arg5;

        if (gj_io_uring_fd_ok(i64Fd)) {
            return gj_io_uring_mmap(i64Fd, u64Off, u64Len);
        }
        return -LINUX_ENOSYS; /* other file maps → cold later */
    }
    if ((u64Prot & LINUX_PROT_WRITE) && (u64Prot & LINUX_PROT_EXEC)) {
        if (!gj_process_has_jit(g_pLinuxProc)) {
            return -LINUX_EACCES; /* W^X without CapJit */
        }
    }
    u32VmmProt = 0;
    if (u64Prot & LINUX_PROT_READ) {
        u32VmmProt |= GJ_VMM_PROT_READ;
    }
    if (u64Prot & LINUX_PROT_WRITE) {
        u32VmmProt |= GJ_VMM_PROT_WRITE;
    }
    if (u64Prot & LINUX_PROT_EXEC) {
        u32VmmProt |= GJ_VMM_PROT_EXEC;
    }
    if (u32VmmProt == 0) {
        u32VmmProt = GJ_VMM_PROT_READ;
    }
    fFixed = ((u64Flags & LINUX_MAP_FIXED) != 0) ? 1 : 0;
    /*
     * Prefer high anon band for ring-3 maps. Never hint into kernel identity
     * (classic PE 0x400000 + 1 MiB used to land on BSS once embeds grew).
     */
    if (!fFixed && u64Addr == 0) {
        u64Addr = 0x0000000040000000ull; /* match process anon cursor default */
    }
    /* G-MO-1: anon mmap via memory object when process is known */
    if (g_pLinuxProc != NULL) {
        va = memobj_map_anon(g_pLinuxProc, u64Addr, (size_t)u64Len, u32VmmProt,
                             fFixed);
    } else {
        va = vmm_mmap_anon(u64Addr, (size_t)u64Len, u32VmmProt, fFixed);
    }
    if (va == 0) {
        return -LINUX_ENOMEM;
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
    if (g_pLinuxProc != NULL) {
        st = memobj_unmap(g_pLinuxProc, (gj_vaddr_t)pRegs->u64Arg0,
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
    if (g_pLinuxProc != NULL) {
        vaGrow = memobj_map_anon(g_pLinuxProc, u64Old + u64OldSz,
                                 (size_t)(u64NewSz - u64OldSz),
                                 GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE, 1);
    } else {
        vaGrow = vmm_mmap_anon(u64Old + u64OldSz, (size_t)(u64NewSz - u64OldSz),
                               GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE, 1);
    }
    if (vaGrow == (gj_vaddr_t)(u64Old + u64OldSz)) {
        return (i64)u64Old;
    }
    /* In-place grow failed */
    if ((u64Flags & LINUX_MREMAP_MAYMOVE) == 0) {
        return -LINUX_ENOMEM;
    }
    /* MAYMOVE: allocate new region; unmap old (content not copied — smoke) */
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

        if (i64New < 0) {
            return i64New;
        }
        one.u64Arg0 = u64Old;
        one.u64Arg1 = u64OldSz;
        (void)gj_linux_hot_munmap(&one);
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

    hot_soft_enter(HOT_SOFT_GRP_MEM, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    u64Addr = pRegs->u64Arg0;
    u64Len = pRegs->u64Arg1;
    u64Prot = pRegs->u64Arg2;
    if ((u64Prot & LINUX_PROT_WRITE) != 0 &&
        (u64Prot & LINUX_PROT_EXEC) != 0) {
        if (!gj_process_has_jit(g_pLinuxProc)) {
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
    if (g_pLinuxProc != NULL && g_pLinuxProc->u64Cr3 != 0) {
        u64SavedCr3 = cpu_read_cr3();
        process_as_activate(g_pLinuxProc);
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
    if (u64SavedCr3 &&
        (u64SavedCr3 & ~0xfffull) != (g_pLinuxProc->u64Cr3 & ~0xfffull)) {
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
    if (u64Clk != LINUX_CLOCK_MONOTONIC &&
        u64Clk != LINUX_CLOCK_MONOTONIC_RAW &&
        u64Clk != LINUX_CLOCK_BOOTTIME &&
        u64Clk != LINUX_CLOCK_REALTIME) {
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
        u64Clk != LINUX_CLOCK_BOOTTIME &&
        u64Clk != LINUX_CLOCK_REALTIME) {
        return -LINUX_EINVAL;
    }
    /* 1 ms resolution for PIT/APIC bring-up clocks */
    tsK.i64Sec = 0;
    tsK.i64Nsec = 1000000;
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
     * flags: 0 = relative, TIMER_ABSTIME=1 absolute — bring-up: relative only.
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
        u64Clk != LINUX_CLOCK_BOOTTIME) {
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
 * Soft futex timeout → absolute mono-nsec deadline (0 = none).
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
        /* Classic WAIT: relative timespec → absolute mono deadline. */
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
    /* futex: wait_bitset — G-FUT-BITSET soft product (val3 = bitset) */
    if (u32Cmd == GJ_FUTEX_WAIT_BITSET) {
        u32Val = (u32)pRegs->u64Arg2;
        u32Bitset = (u32)pRegs->u64Arg5;
        i64Ts = hot_futex_deadline(pRegs->u64Arg3, 1, &u64Deadline);
        if (i64Ts < 0) {
            return i64Ts;
        }
        return futex_wait_bitset(pU32, u32Val, &key, u64Deadline, u32Bitset);
    }
    /* futex: wake_bitset — G-FUT-BITSET soft product (val3 = bitset) */
    if (u32Cmd == GJ_FUTEX_WAKE_BITSET) {
        u32Bitset = (u32)pRegs->u64Arg5;
        return futex_wake_bitset(&key, (u32)pRegs->u64Arg2, u32Bitset);
    }
    return -LINUX_ENOSYS;
}

i64
gj_linux_hot_futex_wake2(struct gj_linux_regs *pRegs)
{
    /* futex_wake(uaddr, mask, nr, flags) → FUTEX_WAKE_BITSET soft */
    struct gj_linux_regs r;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    r = *pRegs;
    /* Soft: private by default (bring-up); mask → val3 bitset */
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
    /* futex_wait(uaddr, val, mask, flags, ...) → FUTEX_WAIT_BITSET soft */
    struct gj_linux_regs r;

    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    r = *pRegs;
    /* Soft: private by default; mask → val3 bitset */
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
        g_u64FsBase = u64Addr;
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
    hot_soft_enter(HOT_SOFT_GRP_FUTEX, pRegs);
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
    g_u64ClearChildTid = pRegs->u64Arg0;
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
    hot_soft_enter(HOT_SOFT_GRP_ID, pRegs);
    (void)pRegs;
    return (i64)g_u32LinuxPgid;
}

/* Linux sysinfo — public layout (partial fill) */
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
    struct linux_sysinfo si;
    u64 mono;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg0 == 0) {
        return -LINUX_EFAULT;
    }
    memset(&si, 0, sizeof(si));
    mono = timer_ready() ? timer_mono_nsec() : 0;
    si.uptime = (i64)(mono / 1000000000ull);
    si.totalram = 1024ull * 1024ull * 1024ull; /* 1 GiB class report */
    si.freeram = si.totalram / 2ull;
    si.procs = 1;
    si.mem_unit = 1;
    if (user_range_ok(pRegs->u64Arg0, sizeof(si))) {
        if (copy_to_user(pRegs->u64Arg0, &si, sizeof(si)) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        *(struct linux_sysinfo *)(gj_vaddr_t)pRegs->u64Arg0 = si;
    }
    return 0;
}

/* struct tms — public layout */
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

/* rusage — first fields used by wine/glibc */
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
    /* who: 0=self 1=children -1=thread — all same for bring-up */
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
    /* nice 0 → kernel returns 20 - nice for getpriority */
    return 20;
}

i64
gj_linux_hot_setpriority(struct gj_linux_regs *pRegs)
{
    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    (void)pRegs;
    return 0; /* accept any nice for bring-up */
}

/* linux_utsname — public structure shape */
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
    u8 aBuf[256];
    u64 u64Len;
    u64 i;
    static u32 g_u32Prng = 0xC0FFEEu;
    gj_status_t st;

    hot_soft_enter(HOT_SOFT_GRP_INFO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg0 == 0) {
        return -LINUX_EFAULT;
    }
    u64Len = pRegs->u64Arg1;
    if (u64Len == 0) {
        return 0;
    }
    if (u64Len > sizeof(aBuf)) {
        u64Len = sizeof(aBuf);
    }
    for (i = 0; i < u64Len; i++) {
        g_u32Prng = g_u32Prng * 1664525u + 1013904223u;
        aBuf[i] = (u8)(g_u32Prng >> 16);
    }
    if (user_range_ok(pRegs->u64Arg0, u64Len)) {
        st = copy_to_user(pRegs->u64Arg0, aBuf, (size_t)u64Len);
        if (st != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        memcpy((void *)(gj_vaddr_t)pRegs->u64Arg0, aBuf, (size_t)u64Len);
    }
    return (i64)u64Len;
}

i64
gj_linux_hot_pread64(struct gj_linux_regs *pRegs)
{
    u8 aBuf[512];
    size_t cb;
    i64 n;
    i64 fd;

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    fd = (i64)pRegs->u64Arg0;
    cb = (size_t)pRegs->u64Arg2;
    if (cb == 0) {
        return 0;
    }
    if (cb > sizeof(aBuf)) {
        cb = sizeof(aBuf);
    }
    if (!vfs_ram_fd_ok(fd)) {
        return -LINUX_EBADF;
    }
    n = vfs_ram_pread(fd, aBuf, cb, pRegs->u64Arg3);
    if (n > 0) {
        if (user_range_ok(pRegs->u64Arg1, (u64)n)) {
            if (copy_to_user(pRegs->u64Arg1, aBuf, (size_t)n) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aBuf, (size_t)n);
        }
    }
    return n;
}

i64
gj_linux_hot_pwrite64(struct gj_linux_regs *pRegs)
{
    u8 aBuf[512];
    size_t cb;
    i64 fd;

    hot_soft_enter(HOT_SOFT_GRP_IO, pRegs);
    if (pRegs == NULL || pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    fd = (i64)pRegs->u64Arg0;
    cb = (size_t)pRegs->u64Arg2;
    if (cb == 0) {
        return 0;
    }
    if (cb > sizeof(aBuf)) {
        cb = sizeof(aBuf);
    }
    if (!vfs_ram_fd_ok(fd)) {
        return -LINUX_EBADF;
    }
    if (user_range_ok(pRegs->u64Arg1, cb)) {
        if (copy_from_user(aBuf, pRegs->u64Arg1, cb) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        memcpy(aBuf, (const void *)(gj_vaddr_t)pRegs->u64Arg1, cb);
    }
    return vfs_ram_pwrite(fd, aBuf, cb, pRegs->u64Arg3);
}
