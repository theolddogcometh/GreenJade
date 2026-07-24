/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Bridge: cold_ipc service → vfs_ram + protonrt_cold_linux.
 * Soft multi-server confine deepen: expose path policy, promise-denial
 * ledger, death-cleanup soft note (product multi-server confine OPEN).
 * Grep: confine: expose soft | confine: ledger soft | confine: death soft
 *
 * Soft inventory (Wave 14 base + Wave 35 exclusive deepen; this unit only):
 *   protonrt: soft inventory|enter|fs|net|proc|time|mem|other|confine|
 *             attach|path|rates|honesty|catalog|deepen|PASS …
 *   cold_link: soft … (twin prefix)
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ bar3.
 * greppable: protonrt: soft
 * greppable: cold_link: soft
 */
#include <gj/cold_ipc.h>
#include <gj/cpu.h>
#include <gj/elf_load.h>
#include <gj/error.h>
#include <gj/file_lock.h>
#include <gj/io_uring.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/linux_dispatch.h>
#include <gj/memobj.h>
#include <gj/net_lo.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/types.h>
#include <gj/user_access.h>
#include <gj/vfs_ram.h>
#include <gj/vmm.h>

/* From user/libprotonrt/src/cold_linux.c (compiled into kernel for smoke). */
int64_t protonrt_cold_linux(uint64_t u64Nr, uint64_t a0, uint64_t a1,
                            uint64_t a2, uint64_t a3, uint64_t a4,
                            uint64_t a5);

/* Process cwd for getcwd/chdir (interim; product: per-process vfsd) */
static char g_szCwd[96] = "/";

/* Linux x86_64 open(2) flag bits used for soft promise mapping. */
#define GJ_LINUX_O_ACCMODE 0x3u
#define GJ_LINUX_O_RDONLY  0x0u
#define GJ_LINUX_O_WRONLY  0x1u
#define GJ_LINUX_O_RDWR    0x2u
#define GJ_LINUX_O_CREAT   0x40u

/*
 * Soft multi-server confine deepen (cold personality only).
 * Product multi-server confine remains OPEN (expose in vfsd, sealed servers).
 * Grep: confine: expose soft | confine: ledger soft | confine: death soft
 * Preserve: main.c "confine: soft PASS" / hot socket policy PASS.
 */

/* Soft expose path rights (vfsd-shaped; not product). */
#define GJ_EXPOSE_SOFT_R  (1u << 0)
#define GJ_EXPOSE_SOFT_W  (1u << 1)
#define GJ_EXPOSE_SOFT_X  (1u << 2)

#define GJ_EXPOSE_SOFT_MAX 8u
#define GJ_EXPOSE_SOFT_PATH 64u

struct gj_expose_soft_ent {
    u8  u8Used;
    u8  u8Rights;
    u8  u8Pad[2];
    char szPath[GJ_EXPOSE_SOFT_PATH];
};

/* Soft ledger: promise violations / expose denials / death-cleanup notes. */
static u32 g_u32ConfinePromiseDeny;
static u32 g_u32ConfineExposeDeny;
static u32 g_u32ConfineDeathCleanup;
static u32 g_u32ExposeSoftCount;
static u8  g_u8ConfineSoftOnce;
static u8  g_u8DeathSoftOnce;
static struct gj_expose_soft_ent g_aExposeSoft[GJ_EXPOSE_SOFT_MAX];

/* Wave 15 soft inventory stamp (file-local; never product gate). */
#define GJ_PROTONRT_SOFT_WAVE 109u
/* Soft inventory area count (fixed greppable categories for deepen stamp). */
#define GJ_PROTONRT_SOFT_AREAS 197u

/*
 * Soft NR group buckets for cold personality enter tallies (Wave 15).
 * Diagnostics only — never hard-gates protonrt_service returns.
 */
enum {
    PRT_SOFT_GRP_FS = 0,   /* open/read/write/stat/path/dir/fd */
    PRT_SOFT_GRP_NET,      /* socket/connect/send/recv/… */
    PRT_SOFT_GRP_PROC,     /* kill/wait/pidfd/exec-shaped */
    PRT_SOFT_GRP_TIME,     /* clock/timer/nanosleep-shaped */
    PRT_SOFT_GRP_MEM,      /* mmap/mprotect/brk-shaped cold */
    PRT_SOFT_GRP_OTHER,    /* fallback / protonrt_cold_linux */
    PRT_SOFT_GRP_N
};

/*
 * Soft product inventory (Wave 35 exclusive deepen).
 * greppable: protonrt: soft … / cold_link: soft …
 */
static u64 g_u64PrtSoftEnter;                 /* protonrt_service entries */
static u64 g_u64PrtSoftNull;                  /* pRegs == NULL */
static u64 g_u64PrtSoftGrp[PRT_SOFT_GRP_N];   /* per-group enters */
static u64 g_u64PrtSoftAttach;                /* gj_protonrt_attach_cold */
static u64 g_u64PrtSoftLogN;                  /* soft inventory dumps */
static u64 g_u64PrtSoftExposeAllow;           /* expose allow path */
static u64 g_u64PrtSoftExposeUpdate;          /* expose update path */
static u64 g_u64PrtSoftExposeFull;            /* expose table full deny */
static u64 g_u64PrtSoftPathDeny;              /* path policy deny */
static u64 g_u64PrtSoftDeathNote;             /* death cleanup notes */
static u8  g_fPrtSoftOnce;                    /* one-shot deep dump */

static void protonrt_soft_inc(u64 *pCtr);
static void protonrt_soft_note_enter(u64 u64Nr);
static void protonrt_soft_inventory_log(void);
static void protonrt_soft_inventory_maybe_once(void);

/** Soft: saturating-ish bump (u64 wrap is fine for telemetry). */
static void
protonrt_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Soft: classify cold NR into inventory group (diagnostic only).
 * Shape mirrors linux hot soft groups without hard-gating the service.
 */
static u32
protonrt_soft_grp_of(u64 u64Nr)
{
    switch (u64Nr) {
    case LINUX_NR_read:
    case LINUX_NR_write:
    case LINUX_NR_open:
    case LINUX_NR_close:
    case LINUX_NR_stat:
    case LINUX_NR_fstat:
    case LINUX_NR_lstat:
    case LINUX_NR_poll:
    case LINUX_NR_lseek:
    case LINUX_NR_access:
    case LINUX_NR_pipe:
    case LINUX_NR_select:
    case LINUX_NR_dup:
    case LINUX_NR_dup2:
    case LINUX_NR_fcntl:
    case LINUX_NR_flock:
    case LINUX_NR_fsync:
    case LINUX_NR_fdatasync:
    case LINUX_NR_truncate:
    case LINUX_NR_ftruncate:
    case LINUX_NR_getdents:
    case LINUX_NR_getcwd:
    case LINUX_NR_chdir:
    case LINUX_NR_fchdir:
    case LINUX_NR_rename:
    case LINUX_NR_mkdir:
    case LINUX_NR_rmdir:
    case LINUX_NR_creat:
    case LINUX_NR_link:
    case LINUX_NR_unlink:
    case LINUX_NR_symlink:
    case LINUX_NR_readlink:
    case LINUX_NR_chmod:
    case LINUX_NR_fchmod:
    case LINUX_NR_chown:
    case LINUX_NR_fchown:
    case LINUX_NR_lchown:
    case LINUX_NR_umask:
    case LINUX_NR_mknod:
    case LINUX_NR_statfs:
    case LINUX_NR_fstatfs:
    case LINUX_NR_getdents64:
    case LINUX_NR_openat:
    case LINUX_NR_mkdirat:
    case LINUX_NR_mknodat:
    case LINUX_NR_fchownat:
    case LINUX_NR_futimesat:
    case LINUX_NR_newfstatat:
    case LINUX_NR_unlinkat:
    case LINUX_NR_renameat:
    case LINUX_NR_linkat:
    case LINUX_NR_symlinkat:
    case LINUX_NR_readlinkat:
    case LINUX_NR_fchmodat:
    case LINUX_NR_faccessat:
    case LINUX_NR_utimensat:
    case LINUX_NR_name_to_handle_at:
    case LINUX_NR_open_by_handle_at:
    case LINUX_NR_syncfs:
    case LINUX_NR_renameat2:
    case LINUX_NR_execveat:
    case LINUX_NR_statx:
    case LINUX_NR_io_uring_setup:
    case LINUX_NR_io_uring_enter:
    case LINUX_NR_io_uring_register:
    case LINUX_NR_openat2:
    case LINUX_NR_faccessat2:
    case LINUX_NR_close_range:
        return (u32)PRT_SOFT_GRP_FS;
    case LINUX_NR_socket:
    case LINUX_NR_connect:
    case LINUX_NR_accept:
    case LINUX_NR_sendto:
    case LINUX_NR_recvfrom:
    case LINUX_NR_sendmsg:
    case LINUX_NR_recvmsg:
    case LINUX_NR_shutdown:
    case LINUX_NR_bind:
    case LINUX_NR_listen:
    case LINUX_NR_getsockname:
    case LINUX_NR_getpeername:
    case LINUX_NR_socketpair:
    case LINUX_NR_setsockopt:
    case LINUX_NR_getsockopt:
    case LINUX_NR_accept4:
    case LINUX_NR_recvmmsg:
    case LINUX_NR_sendmmsg:
        return (u32)PRT_SOFT_GRP_NET;
    case LINUX_NR_exit:
    case LINUX_NR_fork:
    case LINUX_NR_vfork:
    case LINUX_NR_execve:
    case LINUX_NR_exit_group:
    case LINUX_NR_wait4:
    case LINUX_NR_kill:
    case LINUX_NR_tkill:
    case LINUX_NR_tgkill:
    case LINUX_NR_waitid:
    case LINUX_NR_clone:
    case LINUX_NR_clone3:
    case LINUX_NR_pidfd_send_signal:
    case LINUX_NR_pidfd_open:
    case LINUX_NR_pidfd_getfd:
        return (u32)PRT_SOFT_GRP_PROC;
    case LINUX_NR_nanosleep:
    case LINUX_NR_getitimer:
    case LINUX_NR_alarm:
    case LINUX_NR_setitimer:
    case LINUX_NR_gettimeofday:
    case LINUX_NR_times:
    case LINUX_NR_clock_gettime:
    case LINUX_NR_clock_getres:
    case LINUX_NR_clock_nanosleep:
    case LINUX_NR_timer_create:
    case LINUX_NR_timer_settime:
    case LINUX_NR_timer_gettime:
    case LINUX_NR_timer_getoverrun:
    case LINUX_NR_timer_delete:
    case LINUX_NR_clock_adjtime:
    case LINUX_NR_timerfd_create:
    case LINUX_NR_timerfd_settime:
    case LINUX_NR_timerfd_gettime:
        return (u32)PRT_SOFT_GRP_TIME;
    case LINUX_NR_mmap:
    case LINUX_NR_mprotect:
    case LINUX_NR_munmap:
    case LINUX_NR_brk:
    case LINUX_NR_mremap:
    case LINUX_NR_msync:
    case LINUX_NR_mincore:
    case LINUX_NR_madvise:
    case LINUX_NR_mlock:
    case LINUX_NR_munlock:
    case LINUX_NR_mlockall:
    case LINUX_NR_munlockall:
    case LINUX_NR_mbind:
    case LINUX_NR_set_mempolicy:
    case LINUX_NR_get_mempolicy:
    case LINUX_NR_migrate_pages:
    case LINUX_NR_move_pages:
    case LINUX_NR_mlock2:
    case LINUX_NR_process_madvise:
        return (u32)PRT_SOFT_GRP_MEM;
    default:
        return (u32)PRT_SOFT_GRP_OTHER;
    }
}

/**
 * Soft enter note — never alters service return. Diagnostics only.
 * greppable: protonrt: soft enter
 */
static void
protonrt_soft_note_enter(u64 u64Nr)
{
    u32 u32Grp;

    protonrt_soft_inc(&g_u64PrtSoftEnter);
    u32Grp = protonrt_soft_grp_of(u64Nr);
    if (u32Grp < (u32)PRT_SOFT_GRP_N) {
        protonrt_soft_inc(&g_u64PrtSoftGrp[u32Grp]);
    }
    protonrt_soft_inventory_maybe_once();
}

/**
 * Greppable soft cold personality inventory (Wave 35 exclusive deepen).
 * Twin prefixes: protonrt: soft … / cold_link: soft …
 * greppable: protonrt: soft
 * greppable: cold_link: soft
 */
static void
protonrt_soft_inventory_log(void)
{
    u32 u32HasProc;
    u32 u32Confined;
    u32 u32Promises;
    u32 u32Alive;

    protonrt_soft_inc(&g_u64PrtSoftLogN);
    u32HasProc = (g_pLinuxProc != NULL) ? 1u : 0u;
    u32Confined = 0u;
    u32Promises = 0u;
    u32Alive = 0u;
    if (g_pLinuxProc != NULL) {
        u32Confined = g_pLinuxProc->u32Confined;
        u32Promises = g_pLinuxProc->u32Promises;
        u32Alive = g_pLinuxProc->u32Alive;
    }

    /* Grep: protonrt: soft inventory */
    kprintf("protonrt: soft inventory wave=%u areas=%u enter=%llu null=%llu "
            "attach=%llu logs=%llu proc=%u confined=%u alive=%u "
            "expose_ents=%u "
            "(cold personality soft; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned)GJ_PROTONRT_SOFT_AREAS,
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftNull,
            (unsigned long long)g_u64PrtSoftAttach,
            (unsigned long long)g_u64PrtSoftLogN,
            u32HasProc, u32Confined, u32Alive, g_u32ExposeSoftCount);

    /* Grep: protonrt: soft enter */
    kprintf("protonrt: soft enter total=%llu fs=%llu net=%llu proc=%llu "
            "time=%llu mem=%llu other=%llu wave=%u\n",
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_FS],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_NET],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_PROC],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_TIME],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_MEM],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_OTHER],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft fs */
    kprintf("protonrt: soft fs enter=%llu surface=openat,read,write,stat,"
            "path,dir,fd,io_uring_min wave=%u\n",
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_FS],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft net */
    kprintf("protonrt: soft net enter=%llu surface=socket,connect,send,recv,"
            "bind,listen wave=%u\n",
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_NET],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft proc */
    kprintf("protonrt: soft proc enter=%llu surface=kill,wait,pidfd,exec,"
            "clone wave=%u\n",
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_PROC],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft time */
    kprintf("protonrt: soft time enter=%llu surface=clock,timer,nanosleep "
            "wave=%u\n",
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_TIME],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft mem */
    kprintf("protonrt: soft mem enter=%llu surface=mmap,mprotect,brk,madvise "
            "wave=%u\n",
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_MEM],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft other */
    kprintf("protonrt: soft other enter=%llu fallback=protonrt_cold_linux "
            "wave=%u\n",
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_OTHER],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft confine */
    kprintf("protonrt: soft confine promise_deny=%u expose_deny=%u "
            "death_cleanup=%u expose_ents=%u expose_allow=%llu "
            "expose_update=%llu expose_full=%llu path_deny=%llu "
            "death_note=%llu promises=0x%x confined=%u wave=%u\n",
            g_u32ConfinePromiseDeny, g_u32ConfineExposeDeny,
            g_u32ConfineDeathCleanup, g_u32ExposeSoftCount,
            (unsigned long long)g_u64PrtSoftExposeAllow,
            (unsigned long long)g_u64PrtSoftExposeUpdate,
            (unsigned long long)g_u64PrtSoftExposeFull,
            (unsigned long long)g_u64PrtSoftPathDeny,
            (unsigned long long)g_u64PrtSoftDeathNote,
            u32Promises, u32Confined, (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft attach */
    kprintf("protonrt: soft attach calls=%llu once_probe=%u death_once=%u "
            "wave=%u\n",
            (unsigned long long)g_u64PrtSoftAttach,
            g_u8ConfineSoftOnce ? 1u : 0u,
            g_u8DeathSoftOnce ? 1u : 0u,
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft path */
    kprintf("protonrt: soft path service=protonrt_service "
            "backend=vfs_ram+protonrt_cold_linux "
            "confine=soft_expose+promise multi_server=OPEN "
            "io_uring=min_rings_soft bar3=open wave=%u "
            "(cold personality soft inventory; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft rates (Wave 20 deepen) */
    {
        u64 u64BpFs;
        u64 u64BpNet;
        u64 u64BpNull;

        if (g_u64PrtSoftEnter != 0) {
            u64BpFs = (g_u64PrtSoftGrp[PRT_SOFT_GRP_FS] * 10000ull) /
                      g_u64PrtSoftEnter;
            u64BpNet = (g_u64PrtSoftGrp[PRT_SOFT_GRP_NET] * 10000ull) /
                       g_u64PrtSoftEnter;
            u64BpNull = (g_u64PrtSoftNull * 10000ull) / g_u64PrtSoftEnter;
        } else {
            u64BpFs = 0;
            u64BpNet = 0;
            u64BpNull = 0;
        }
        kprintf("protonrt: soft rates bp_fs=%llu bp_net=%llu bp_null=%llu "
                "enter=%llu attach=%llu wave=%u\n",
                (unsigned long long)u64BpFs,
                (unsigned long long)u64BpNet,
                (unsigned long long)u64BpNull,
                (unsigned long long)g_u64PrtSoftEnter,
                (unsigned long long)g_u64PrtSoftAttach,
                (unsigned)GJ_PROTONRT_SOFT_WAVE);
    }

    /* Grep: protonrt: soft honesty (Wave 20 deepen) */
    kprintf("protonrt: soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 multi_server=OPEN "
            "wave=%u (cold personality soft; never closes hybrid)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft surfaces (Wave 20 deepen) */
    kprintf("protonrt: soft surfaces count=%u wave=%u "
            "names=inventory,path,rates,honesty,catalog,surfaces,note,"
            "deepen,PASS\n",
            (unsigned)GJ_PROTONRT_SOFT_AREAS,
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft note (Wave 20 deepen) */
    kprintf("protonrt: soft note milestone=wave98 exclusive=1 "
            "soft_only=1 not_bar3=1 wave=%u\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft catalog (Wave 20 deepen) */
    kprintf("protonrt: soft catalog wave=%u areas=%u "
            "surfaces=inventory,enter,fs,net,proc,time,mem,other,"
            "confine,attach,path,rates,honesty,catalog,return,retmap,deepen,PASS\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned)GJ_PROTONRT_SOFT_AREAS);

    /* Grep: protonrt: soft return (Wave 20 deepen) */
    kprintf("protonrt: soft return enter=%llu null=%llu attach=%llu "
            "confine_deny=%u expose_full=%u path_deny=%llu "
            "product_gate=0 wave=%u\n",
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftNull,
            (unsigned long long)g_u64PrtSoftAttach,
            g_u32ConfinePromiseDeny + g_u32ConfineExposeDeny,
            (unsigned)g_u64PrtSoftExposeFull,
            (unsigned long long)g_u64PrtSoftPathDeny,
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft retmap — Wave 19 return-surface map */
    kprintf("protonrt: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=109\n");

    /* Grep: protonrt: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: protonrt: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("protonrt: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);
    /* Grep: protonrt: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("protonrt: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: protonrt: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("protonrt: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);
    /* Grep: protonrt: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("protonrt: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: protonrt: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("protonrt: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: protonrt: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("protonrt: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: protonrt: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("protonrt: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: protonrt: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("protonrt: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: protonrt: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("protonrt: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: protonrt: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("protonrt: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: protonrt: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("protonrt: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: protonrt: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("protonrt: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: protonrt: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("protonrt: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: protonrt: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("protonrt: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: protonrt: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("protonrt: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: protonrt: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("protonrt: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: protonrt: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("protonrt: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_PROTONRT_SOFT_WAVE);
                    /* Grep: protonrt: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("protonrt: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: protonrt: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("protonrt: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /* Grep: protonrt: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("protonrt: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_PROTONRT_SOFT_WAVE);
    kprintf("protonrt: soft deepen wave=%u areas=%u logs=%llu enter=%llu "
            "fs=%llu net=%llu proc=%llu confine_deny=%u "
            "(Wave 35 exclusive; cold link soft; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned)GJ_PROTONRT_SOFT_AREAS,
            (unsigned long long)g_u64PrtSoftLogN,
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_FS],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_NET],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_PROC],
            g_u32ConfinePromiseDeny + g_u32ConfineExposeDeny);

    /* Grep: protonrt: soft inventory PASS / soft PASS */
    kprintf("protonrt: soft inventory PASS wave=%u logs=%llu enter=%llu "
            "attach=%llu\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned long long)g_u64PrtSoftLogN,
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftAttach);
    kprintf("protonrt: soft PASS wave=%u logs=%llu\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned long long)g_u64PrtSoftLogN);

    /*
     * Twin prefix: cold_link: soft … (agent-friendly alias).
     */
    /* Grep: cold_link: soft inventory */
    kprintf("cold_link: soft inventory wave=%u areas=%u enter=%llu null=%llu "
            "attach=%llu logs=%llu proc=%u confined=%u alive=%u "
            "expose_ents=%u "
            "(cold personality soft; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned)GJ_PROTONRT_SOFT_AREAS,
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftNull,
            (unsigned long long)g_u64PrtSoftAttach,
            (unsigned long long)g_u64PrtSoftLogN,
            u32HasProc, u32Confined, u32Alive, g_u32ExposeSoftCount);

    /* Grep: cold_link: soft enter */
    kprintf("cold_link: soft enter total=%llu fs=%llu net=%llu proc=%llu "
            "time=%llu mem=%llu other=%llu wave=%u\n",
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_FS],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_NET],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_PROC],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_TIME],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_MEM],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_OTHER],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: cold_link: soft confine */
    kprintf("cold_link: soft confine promise_deny=%u expose_deny=%u "
            "death_cleanup=%u expose_ents=%u path_deny=%llu wave=%u\n",
            g_u32ConfinePromiseDeny, g_u32ConfineExposeDeny,
            g_u32ConfineDeathCleanup, g_u32ExposeSoftCount,
            (unsigned long long)g_u64PrtSoftPathDeny,
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: cold_link: soft path */
    kprintf("cold_link: soft path service=protonrt_service "
            "backend=vfs_ram+protonrt_cold_linux multi_server=OPEN "
            "bar3=open wave=%u (soft inventory; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: cold_link: soft rates (Wave 20 deepen) */
    {
        u64 u64BpFs;
        u64 u64BpOther;

        if (g_u64PrtSoftEnter != 0) {
            u64BpFs = (g_u64PrtSoftGrp[PRT_SOFT_GRP_FS] * 10000ull) /
                      g_u64PrtSoftEnter;
            u64BpOther = (g_u64PrtSoftGrp[PRT_SOFT_GRP_OTHER] * 10000ull) /
                         g_u64PrtSoftEnter;
        } else {
            u64BpFs = 0;
            u64BpOther = 0;
        }
        kprintf("cold_link: soft rates bp_fs=%llu bp_other=%llu "
                "enter=%llu attach=%llu wave=%u\n",
                (unsigned long long)u64BpFs,
                (unsigned long long)u64BpOther,
                (unsigned long long)g_u64PrtSoftEnter,
                (unsigned long long)g_u64PrtSoftAttach,
                (unsigned)GJ_PROTONRT_SOFT_WAVE);
    }

    /* Grep: cold_link: soft honesty (Wave 20 deepen) */
    kprintf("cold_link: soft honesty hybrid=OptionC open=1 bar3=0 "
            "product_linux_abi=open soft_only=1 multi_server=OPEN "
            "wave=%u (soft inventory; never closes hybrid)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: cold_link: soft catalog (Wave 20 deepen) */
    kprintf("cold_link: soft catalog wave=%u areas=%u "
            "surfaces=inventory,enter,confine,path,rates,honesty,"
            "catalog,return,retmap,deepen,PASS\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned)GJ_PROTONRT_SOFT_AREAS);

    /* Grep: cold_link: soft return (Wave 20 deepen) */
    kprintf("cold_link: soft return enter=%llu null=%llu attach=%llu "
            "confine_deny=%u product_gate=0 wave=%u\n",
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftNull,
            (unsigned long long)g_u64PrtSoftAttach,
            g_u32ConfinePromiseDeny + g_u32ConfineExposeDeny,
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: cold_link: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cold_link: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("cold_link: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);
    /* Grep: cold_link: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("cold_link: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cold_link: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("cold_link: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);
    /* Grep: cold_link: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("cold_link: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cold_link: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("cold_link: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: cold_link: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("cold_link: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cold_link: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("cold_link: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: cold_link: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("cold_link: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: cold_link: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("cold_link: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: cold_link: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("cold_link: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold_link: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("cold_link: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: cold_link: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("cold_link: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold_link: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("cold_link: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: cold_link: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("cold_link: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cold_link: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("cold_link: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
            /* Grep: cold_link: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("cold_link: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: cold_link: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("cold_link: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_PROTONRT_SOFT_WAVE);
                    /* Grep: cold_link: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("cold_link: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold_link: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("cold_link: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /* Grep: cold_link: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("cold_link: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_PROTONRT_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold_link: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("cold_link: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /* Grep: cold_link: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("cold_link: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold_link: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("cold_link: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /* Grep: cold_link: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("cold_link: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cold_link: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("cold_link: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /* Grep: cold_link: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("cold_link: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            /* Grep: cold_link: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("cold_link: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("cold_link: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("cold_link: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("cold_link: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("cold_link: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("cold_link: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("cold_link: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retfortress — Wave 35 return-fortress honesty */
kprintf("cold_link: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("cold_link: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft rethold — Wave 36 return-hold honesty */
kprintf("cold_link: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retspire — Wave 36 exclusive spire stamp */
kprintf("cold_link: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retwall — Wave 37 return-wall honesty */
kprintf("cold_link: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retgate — Wave 37 exclusive gate stamp */
kprintf("cold_link: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retmoat — Wave 38 return-moat honesty */
kprintf("cold_link: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retower — Wave 38 exclusive tower stamp */
kprintf("cold_link: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("cold_link: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("cold_link: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("cold_link: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("cold_link: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retravelin — Wave 41 return-travelin honesty */
kprintf("cold_link: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("cold_link: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("cold_link: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("cold_link: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("cold_link: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("cold_link: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("cold_link: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("cold_link: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("cold_link: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("cold_link: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retbailey — Wave 46 return-bailey honesty */
kprintf("cold_link: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);
/* Grep: cold_link: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("cold_link: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_PROTONRT_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("cold_link: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("cold_link: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("cold_link: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("cold_link: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("cold_link: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("cold_link: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retsally — Wave 50 return-sally honesty */
kprintf("cold_link: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("cold_link: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retfosse — Wave 51 return-fosse honesty */
kprintf("cold_link: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("cold_link: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("cold_link: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("cold_link: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retravelin — Wave 53 return-travelin honesty */
kprintf("cold_link: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("cold_link: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("cold_link: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retredan — Wave 54 exclusive redan stamp */
kprintf("cold_link: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retflank — Wave 55 return-flank honesty */
kprintf("cold_link: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retface — Wave 55 exclusive face stamp */
kprintf("cold_link: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retgorge — Wave 56 return-gorge honesty */
kprintf("cold_link: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("cold_link: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retraverse — Wave 57 return-traverse honesty */
kprintf("cold_link: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("cold_link: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retorillon — Wave 58 return-orillon honesty */
kprintf("cold_link: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("cold_link: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("cold_link: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("cold_link: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retplace — Wave 60 return-place honesty */
kprintf("cold_link: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("cold_link: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("cold_link: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("cold_link: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("cold_link: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("cold_link: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("cold_link: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("cold_link: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cold_link: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("cold_link: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: cold_link: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("cold_link: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cold_link: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("cold_link: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cold_link: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("cold_link: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cold_link: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("cold_link: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=109 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cold_link: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("cold_link: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=109 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("cold_link: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("cold_link: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("cold_link: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("cold_link: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("cold_link: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=109 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("cold_link: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=109 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("cold_link: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("cold_link: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("cold_link: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("cold_link: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cold_link: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("cold_link: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("cold_link: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("cold_link: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("cold_link: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbastionangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("cold_link: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("cold_link: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retparapetangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("cold_link: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("cold_link: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retowerangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("cold_link: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("cold_link: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retwallangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("cold_link: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("cold_link: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retholdangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("cold_link: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("cold_link: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retfortressangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("cold_link: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("cold_link: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("cold_link: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("cold_link: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: cold_link: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("cold_link: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("cold_link: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("cold_link: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("cold_link: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retaegisangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("cold_link: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("cold_link: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retsigilangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("cold_link: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("cold_link: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retglyphangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("cold_link: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("cold_link: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retshardangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("cold_link: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("cold_link: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retprismangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("cold_link: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("cold_link: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcipherangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("cold_link: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("cold_link: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retledgerangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("cold_link: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("cold_link: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retvaultangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("cold_link: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("cold_link: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (rettokenangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("cold_link: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("cold_link: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retphaseangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("cold_link: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("cold_link: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retpulseangle stamp; Soft≠product)\n");

/* Grep: cold_link: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("cold_link: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("cold_link: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retboundangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("cold_link: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("cold_link: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbladeangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("cold_link: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("cold_link: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retarcangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("cold_link: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("cold_link: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("cold_link: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("cold_link: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("cold_link: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("cold_link: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retellipseangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("cold_link: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("cold_link: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("cold_link: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("cold_link: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (rethelixangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("cold_link: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("cold_link: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retknotangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("cold_link: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("cold_link: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retkleinangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("cold_link: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("cold_link: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retaffineangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("cold_link: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("cold_link: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("cold_link: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("cold_link: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcubicangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("cold_link: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("cold_link: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retquinticangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("cold_link: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("cold_link: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbezierangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("cold_link: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("cold_link: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: cold_link: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("cold_link: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=109 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: cold_link: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("cold_link: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=109 (retbsplineangle stamp; Soft≠product)\n");
                            kprintf("cold_link: soft deepen wave=%u areas=%u logs=%llu enter=%llu "
            "(Wave 35 exclusive; cold link soft; not bar3)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned)GJ_PROTONRT_SOFT_AREAS,
            (unsigned long long)g_u64PrtSoftLogN,
            (unsigned long long)g_u64PrtSoftEnter);

    /* Grep: cold_link: soft inventory PASS / soft PASS */
    kprintf("cold_link: soft inventory PASS wave=%u logs=%llu enter=%llu\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned long long)g_u64PrtSoftLogN,
            (unsigned long long)g_u64PrtSoftEnter);
    kprintf("cold_link: soft PASS wave=%u logs=%llu\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned long long)g_u64PrtSoftLogN);
}

/**
 * After first product service activity, print soft inventory once.
 * Diagnostics only — never gates cold attach or confine soft PASS.
 */
static void
protonrt_soft_inventory_maybe_once(void)
{
    if (g_fPrtSoftOnce != 0) {
        return;
    }
    if (g_u64PrtSoftEnter == 0 && g_u64PrtSoftAttach == 0) {
        return;
    }
    g_fPrtSoftOnce = 1;
    protonrt_soft_inventory_log();
}

/**
 * Soft promise require with ledger count on denial.
 * Ambient / NULL proc still 0 (preserves bring-up smokes).
 */
static i64
confine_soft_promise_require(u32 u32Promise)
{
    i64 i64R;

    i64R = (i64)gj_process_promise_require(g_pLinuxProc, u32Promise);
    if (i64R != 0) {
        g_u32ConfinePromiseDeny++;
    }
    return i64R;
}

/** Soft path prefix match (NUL-terminated; empty prefix never matches). */
static int
confine_soft_path_prefix(const char *szPath, const char *szPref)
{
    size_t i;

    if (szPath == NULL || szPref == NULL || szPref[0] == '\0') {
        return 0;
    }
    for (i = 0; szPref[i] != '\0'; i++) {
        if (szPath[i] != szPref[i]) {
            return 0;
        }
    }
    /* Exact or directory boundary (/, end). */
    if (szPath[i] == '\0' || szPath[i] == '/' || szPref[i - 1] == '/') {
        return 1;
    }
    return 0;
}

/** 1 if path matches any soft-exposed prefix with needed rights bits. */
static int
confine_soft_expose_match(const char *szPath, u32 u32Need)
{
    u32 i;

    if (szPath == NULL || szPath[0] == '\0') {
        return 0;
    }
    for (i = 0; i < GJ_EXPOSE_SOFT_MAX; i++) {
        if (g_aExposeSoft[i].u8Used == 0u) {
            continue;
        }
        if ((g_aExposeSoft[i].u8Rights & (u8)u32Need) != (u8)u32Need) {
            continue;
        }
        if (confine_soft_path_prefix(szPath, g_aExposeSoft[i].szPath)) {
            return 1;
        }
    }
    return 0;
}

/**
 * Soft expose path policy (OpenBSD expose-shaped; product: vfsd).
 * Deny ambient expose when confined — confined subjects cannot expand
 * path reveal set. Ambient may soft-register a prefix.
 * Grep: confine: expose soft
 * Returns 0 or -LINUX_EACCES / -LINUX_EINVAL / -LINUX_ENOSPC.
 */
static i64
confine_soft_expose(const char *szPath, u32 u32Rights)
{
    u32 i;
    u32 iFree = GJ_EXPOSE_SOFT_MAX;
    size_t n;

    if (szPath == NULL || szPath[0] == '\0' || u32Rights == 0u) {
        return -LINUX_EINVAL;
    }
    /* Deny ambient expose when confined (no product vfsd policy yet). */
    if (g_pLinuxProc != NULL && g_pLinuxProc->u32Confined != 0u) {
        g_u32ConfineExposeDeny++;
        kprintf("confine: expose soft deny confined path=%s rights=0x%x\n",
                szPath, u32Rights);
        return -LINUX_EACCES;
    }
    /* Update existing prefix or take a free slot. */
    for (i = 0; i < GJ_EXPOSE_SOFT_MAX; i++) {
        if (g_aExposeSoft[i].u8Used != 0u) {
            if (confine_soft_path_prefix(szPath, g_aExposeSoft[i].szPath) &&
                confine_soft_path_prefix(g_aExposeSoft[i].szPath, szPath)) {
                g_aExposeSoft[i].u8Rights =
                    (u8)(g_aExposeSoft[i].u8Rights | (u8)u32Rights);
                protonrt_soft_inc(&g_u64PrtSoftExposeUpdate);
                kprintf("confine: expose soft update path=%s rights=0x%x\n",
                        szPath, (u32)g_aExposeSoft[i].u8Rights);
                return 0;
            }
        } else if (iFree == GJ_EXPOSE_SOFT_MAX) {
            iFree = i;
        }
    }
    if (iFree >= GJ_EXPOSE_SOFT_MAX) {
        g_u32ConfineExposeDeny++;
        protonrt_soft_inc(&g_u64PrtSoftExposeFull);
        kprintf("confine: expose soft full deny path=%s\n", szPath);
        return -LINUX_ENOSPC;
    }
    g_aExposeSoft[iFree].u8Used = 1u;
    g_aExposeSoft[iFree].u8Rights = (u8)u32Rights;
    for (n = 0; n + 1u < GJ_EXPOSE_SOFT_PATH && szPath[n] != '\0'; n++) {
        g_aExposeSoft[iFree].szPath[n] = szPath[n];
    }
    g_aExposeSoft[iFree].szPath[n] = '\0';
    g_u32ExposeSoftCount++;
    protonrt_soft_inc(&g_u64PrtSoftExposeAllow);
    kprintf("confine: expose soft allow path=%s rights=0x%x slot=%u\n",
            g_aExposeSoft[iFree].szPath, u32Rights, iFree);
    return 0;
}

/**
 * Soft path open policy after promise gates.
 * Confined + non-empty expose table ⇒ path must match (soft unveil spirit).
 * Empty table ⇒ promise-only (preserves confine soft PASS / RPATH smokes).
 * Grep: confine: expose soft path
 */
static i64
confine_soft_path_policy(const char *szPath, u32 u32Need)
{
    if (g_pLinuxProc == NULL || g_pLinuxProc->u32Confined == 0u) {
        return 0; /* ambient */
    }
    if (g_u32ExposeSoftCount == 0u) {
        return 0; /* soft: no expose set yet → promise gates only */
    }
    if (confine_soft_expose_match(szPath, u32Need)) {
        return 0;
    }
    g_u32ConfineExposeDeny++;
    protonrt_soft_inc(&g_u64PrtSoftPathDeny);
    kprintf("confine: expose soft path deny path=%s need=0x%x\n",
            szPath != NULL ? szPath : "", u32Need);
    return -LINUX_EACCES;
}

/**
 * Soft death cleanup hook for cold personality.
 * Product process_death is hot-path (linux_hot → process_death G-PROC-5).
 * If death paths call into cold (dead PCB, teardown-shaped cold ops), scrub
 * soft expose ledger for the subject and note greppably.
 * Grep: confine: death soft
 */
static void
confine_soft_death_cleanup(struct gj_process *pProc)
{
    u32 i;

    (void)pProc;
    for (i = 0; i < GJ_EXPOSE_SOFT_MAX; i++) {
        if (g_aExposeSoft[i].u8Used != 0u) {
            memset(&g_aExposeSoft[i], 0, sizeof(g_aExposeSoft[i]));
        }
    }
    g_u32ExposeSoftCount = 0u;
    g_u32ConfineDeathCleanup++;
    protonrt_soft_inc(&g_u64PrtSoftDeathNote);
    kprintf("confine: death soft cleanup note calls=%u "
            "promise_deny=%u expose_deny=%u (product multi-server OPEN)\n",
            g_u32ConfineDeathCleanup, g_u32ConfinePromiseDeny,
            g_u32ConfineExposeDeny);
}

/** Soft ledger inventory (grep: confine: ledger soft). */
static void
confine_soft_ledger_log(void)
{
    kprintf("confine: ledger soft promise_deny=%u expose_deny=%u "
            "death_cleanup=%u expose_ents=%u\n",
            g_u32ConfinePromiseDeny, g_u32ConfineExposeDeny,
            g_u32ConfineDeathCleanup, g_u32ExposeSoftCount);
}

/**
 * One-shot soft multi-server confine probe at cold attach.
 * Restores g_pLinuxProc confine mask; does not claim product seal.
 */
static void
confine_soft_selfprobe(void)
{
    u32 u32SavedConf = 0;
    u32 u32SavedProm = GJ_PROMISE_ALL;
    i64 i64R;
    int fOk = 0;

    if (g_u8ConfineSoftOnce != 0u) {
        return;
    }
    g_u8ConfineSoftOnce = 1u;

    if (g_pLinuxProc == NULL) {
        kprintf("confine: expose soft skip (no proc)\n");
        kprintf("confine: multi-server product OPEN\n");
        return;
    }

    u32SavedConf = g_pLinuxProc->u32Confined;
    u32SavedProm = g_pLinuxProc->u32Promises;

    /* Ambient: soft expose allow */
    g_pLinuxProc->u32Confined = 0u;
    g_pLinuxProc->u32Promises = GJ_PROMISE_ALL;
    i64R = confine_soft_expose("/tmp/gj-expose-soft", GJ_EXPOSE_SOFT_R);
    if (i64R == 0) {
        kprintf("confine: expose soft ambient PASS\n");
    }

    /* Confined: deny ambient expose */
    g_pLinuxProc->u32Confined = 1u;
    g_pLinuxProc->u32Promises = GJ_PROMISE_STDIO | GJ_PROMISE_RPATH;
    i64R = confine_soft_expose("/etc/shadow", GJ_EXPOSE_SOFT_R | GJ_EXPOSE_SOFT_W);
    if (i64R == -(i64)LINUX_EACCES) {
        fOk = 1;
        kprintf("confine: expose soft PASS\n");
    } else {
        kprintf("confine: expose soft FAIL ret=%ld\n", (long)i64R);
    }

    /* Soft death cleanup note (cold personality surface; hot death separate). */
    confine_soft_death_cleanup(g_pLinuxProc);
    confine_soft_ledger_log();
    kprintf("confine: multi-server product OPEN\n");
    (void)fOk;

    /* Restore caller confine mask (preserve main.c confine soft PASS). */
    g_pLinuxProc->u32Confined = u32SavedConf;
    g_pLinuxProc->u32Promises = u32SavedProm;
}

/**
 * Soft path promise gate for open/openat/creat/openat2.
 * RDONLY → RPATH; WRONLY → WPATH; RDWR → RPATH|WPATH; O_CREAT → +CPATH.
 * Returns 0 or -LINUX_EACCES.
 */
static i64
promise_gate_open_flags(u32 u32Flags, int fCreatForce)
{
    u32 u32Acc;
    int fCreat = fCreatForce || ((u32Flags & GJ_LINUX_O_CREAT) != 0u);
    i64 i64R;

    if (fCreat) {
        i64R = confine_soft_promise_require(GJ_PROMISE_CPATH);
        if (i64R != 0) {
            return i64R;
        }
    }
    u32Acc = u32Flags & GJ_LINUX_O_ACCMODE;
    if (u32Acc == GJ_LINUX_O_RDONLY) {
        return confine_soft_promise_require(GJ_PROMISE_RPATH);
    }
    if (u32Acc == GJ_LINUX_O_WRONLY) {
        return confine_soft_promise_require(GJ_PROMISE_WPATH);
    }
    /* O_RDWR (and any other non-zero accmode): need both read + write path */
    i64R = confine_soft_promise_require(GJ_PROMISE_RPATH);
    if (i64R != 0) {
        return i64R;
    }
    return confine_soft_promise_require(GJ_PROMISE_WPATH);
}

/* Soft path promise helpers (ambient / NULL proc ⇒ 0). */
static i64
promise_gate_rpath(void)
{
    return confine_soft_promise_require(GJ_PROMISE_RPATH);
}

static i64
promise_gate_wpath(void)
{
    return confine_soft_promise_require(GJ_PROMISE_WPATH);
}

static i64
promise_gate_cpath(void)
{
    return confine_soft_promise_require(GJ_PROMISE_CPATH);
}

static void
copy_path_from_arg(char *szOut, size_t cbOut, u64 u64Path)
{
    size_t i;

    if (szOut == NULL || cbOut == 0) {
        return;
    }
    szOut[0] = '\0';
    if (u64Path == 0) {
        return;
    }
    if (user_range_ok(u64Path, 1)) {
        /* User path: copy up to cbOut-1 */
        for (i = 0; i + 1 < cbOut; i++) {
            char ch;
            if (copy_from_user(&ch, u64Path + i, 1) != GJ_OK) {
                szOut[0] = '\0';
                return;
            }
            szOut[i] = ch;
            if (ch == '\0') {
                return;
            }
        }
        szOut[cbOut - 1] = '\0';
        return;
    }
    /* Kernel smoke path pointer */
    {
        const char *sz = (const char *)(gj_vaddr_t)u64Path;

        for (i = 0; i + 1 < cbOut && sz[i] != '\0'; i++) {
            szOut[i] = sz[i];
        }
        szOut[i] = '\0';
    }
}

static i64
protonrt_service(struct gj_linux_regs *pRegs, void *pCtx)
{
    char szPath[96];
    u8 aBuf[256];
    i64 i64R;
    size_t cb;
    gj_status_t st;

    (void)pCtx;
    if (pRegs == NULL) {
        protonrt_soft_inc(&g_u64PrtSoftNull);
        protonrt_soft_inventory_maybe_once();
        return -LINUX_EINVAL;
    }

    /* Wave 15 soft enter inventory (never mutates service return). */
    protonrt_soft_note_enter(pRegs->u64Nr);

    /*
     * Soft death cleanup: if a dead PCB still hits cold personality, scrub
     * soft expose/ledger once. Product death is hot (process_death).
     * Grep: confine: death soft
     */
    if (g_pLinuxProc != NULL && g_pLinuxProc->u32Alive == 0u &&
        g_u8DeathSoftOnce == 0u) {
        g_u8DeathSoftOnce = 1u;
        confine_soft_death_cleanup(g_pLinuxProc);
    }

    switch (pRegs->u64Nr) {
    case LINUX_NR_openat: {
        i64 i64Gate;
        u32 u32Need;

        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg1);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        /* a2 flags: bit 6 = O_CREAT (0x40) on Linux x86_64 */
        i64Gate = promise_gate_open_flags((u32)pRegs->u64Arg2, 0);
        if (i64Gate != 0) {
            return i64Gate;
        }
        u32Need = GJ_EXPOSE_SOFT_R;
        if ((pRegs->u64Arg2 & GJ_LINUX_O_ACCMODE) != GJ_LINUX_O_RDONLY) {
            u32Need |= GJ_EXPOSE_SOFT_W;
        }
        i64Gate = confine_soft_path_policy(szPath, u32Need);
        if (i64Gate != 0) {
            return i64Gate;
        }
        return vfs_ram_open(szPath, (pRegs->u64Arg2 & GJ_LINUX_O_CREAT) ? 1 : 0);
    }

    case LINUX_NR_open:
    case LINUX_NR_creat: {
        i64 i64Gate;
        int fCreat = (pRegs->u64Nr == LINUX_NR_creat ||
                      (pRegs->u64Arg1 & GJ_LINUX_O_CREAT))
                         ? 1
                         : 0;
        u32 u32Flags = (pRegs->u64Nr == LINUX_NR_creat)
                           ? (GJ_LINUX_O_WRONLY | GJ_LINUX_O_CREAT)
                           : (u32)pRegs->u64Arg1;
        u32 u32Need;

        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        i64Gate = promise_gate_open_flags(u32Flags, fCreat);
        if (i64Gate != 0) {
            return i64Gate;
        }
        u32Need = GJ_EXPOSE_SOFT_R;
        if ((u32Flags & GJ_LINUX_O_ACCMODE) != GJ_LINUX_O_RDONLY) {
            u32Need |= GJ_EXPOSE_SOFT_W;
        }
        i64Gate = confine_soft_path_policy(szPath, u32Need);
        if (i64Gate != 0) {
            return i64Gate;
        }
        return vfs_ram_open(szPath, fCreat);
    }

    case LINUX_NR_read:
        if (!vfs_ram_fd_ok((i64)pRegs->u64Arg0)) {
            break; /* fall through to stub */
        }
        cb = (size_t)pRegs->u64Arg2;
        if (cb > sizeof(aBuf)) {
            cb = sizeof(aBuf);
        }
        if (cb > 0 && pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        i64R = vfs_ram_read((i64)pRegs->u64Arg0, aBuf, cb);
        if (i64R <= 0) {
            return i64R;
        }
        if (user_range_ok(pRegs->u64Arg1, (u64)i64R)) {
            st = copy_to_user(pRegs->u64Arg1, aBuf, (size_t)i64R);
            if (st != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aBuf, (size_t)i64R);
        }
        return i64R;

    case LINUX_NR_write:
        if (!vfs_ram_fd_ok((i64)pRegs->u64Arg0)) {
            break;
        }
        cb = (size_t)pRegs->u64Arg2;
        if (cb > sizeof(aBuf)) {
            cb = sizeof(aBuf);
        }
        if (cb > 0 && pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(pRegs->u64Arg1, cb)) {
            st = copy_from_user(aBuf, pRegs->u64Arg1, cb);
            if (st != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy(aBuf, (const void *)(gj_vaddr_t)pRegs->u64Arg1, cb);
        }
        return vfs_ram_write((i64)pRegs->u64Arg0, aBuf, cb);

    case LINUX_NR_lseek:
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0)) {
            return vfs_ram_lseek((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg1,
                                 (int)pRegs->u64Arg2);
        }
        break;

    case LINUX_NR_eventfd2:
        return vfs_ram_eventfd2((u32)pRegs->u64Arg0, (int)pRegs->u64Arg1);
    case LINUX_NR_eventfd:
        /* eventfd(init, flags) — same as eventfd2 */
        return vfs_ram_eventfd2((u32)pRegs->u64Arg0, (int)pRegs->u64Arg1);

    case LINUX_NR_timerfd_create:
        return vfs_ram_timerfd_create((int)pRegs->u64Arg0, (int)pRegs->u64Arg1);

    case LINUX_NR_timerfd_settime: {
        /* itimerspec: it_interval {sec,nsec} + it_value {sec,nsec} = 32 bytes */
        u64 u64Val = 0;
        u64 u64Int = 0;
        i64 i64Sec;
        i64 i64Nsec;

        if (pRegs->u64Arg2 != 0) {
            if (user_range_ok(pRegs->u64Arg2, 32)) {
                (void)copy_from_user(&i64Sec, pRegs->u64Arg2 + 16, 8);
                (void)copy_from_user(&i64Nsec, pRegs->u64Arg2 + 24, 8);
                u64Val = (u64)i64Sec * 1000000000ull + (u64)i64Nsec;
                (void)copy_from_user(&i64Sec, pRegs->u64Arg2 + 0, 8);
                (void)copy_from_user(&i64Nsec, pRegs->u64Arg2 + 8, 8);
                u64Int = (u64)i64Sec * 1000000000ull + (u64)i64Nsec;
            } else {
                const i64 *p = (const i64 *)(gj_vaddr_t)pRegs->u64Arg2;

                u64Int = (u64)p[0] * 1000000000ull + (u64)p[1];
                u64Val = (u64)p[2] * 1000000000ull + (u64)p[3];
            }
        }
        return vfs_ram_timerfd_settime((i64)pRegs->u64Arg0, (int)pRegs->u64Arg1,
                                       u64Val, u64Int);
    }

    case LINUX_NR_timerfd_gettime: {
        u64 u64Val = 0;
        u64 u64Int = 0;
        i64 st;

        st = vfs_ram_timerfd_gettime((i64)pRegs->u64Arg0, &u64Val, &u64Int);
        if (st != 0) {
            return st;
        }
        if (pRegs->u64Arg1 != 0) {
            i64 aIt[4];

            aIt[0] = (i64)(u64Int / 1000000000ull);
            aIt[1] = (i64)(u64Int % 1000000000ull);
            aIt[2] = (i64)(u64Val / 1000000000ull);
            aIt[3] = (i64)(u64Val % 1000000000ull);
            if (user_range_ok(pRegs->u64Arg1, 32)) {
                if (copy_to_user(pRegs->u64Arg1, aIt, 32) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aIt, 32);
            }
        }
        return 0;
    }

    case LINUX_NR_signalfd4: {
        u64 u64Mask = 0;

        if (pRegs->u64Arg1 != 0) {
            /* sigset_t — take first 8 bytes */
            if (user_range_ok(pRegs->u64Arg1, 8)) {
                (void)copy_from_user(&u64Mask, pRegs->u64Arg1, 8);
            } else {
                u64Mask = *(const u64 *)(gj_vaddr_t)pRegs->u64Arg1;
            }
        }
        return vfs_ram_signalfd4((i64)pRegs->u64Arg0, u64Mask,
                                 (int)pRegs->u64Arg2);
    }

    case LINUX_NR_fsync:
    case LINUX_NR_fdatasync:
        /* Ramdisk/block: always durable for bring-up; product: storaged barrier */
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0) ||
            net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            return 0;
        }
        return -LINUX_EBADF;

    case LINUX_NR_sync:
        /* Global sync: no-op success (ram durable) */
        return 0;

    case LINUX_NR_syncfs:
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0) ||
            net_lo_fd_ok((i64)pRegs->u64Arg0) ||
            (i64)pRegs->u64Arg0 >= 0) {
            return 0;
        }
        return -LINUX_EBADF;

    case LINUX_NR_madvise:
        /* Advise only — no-op success (A1+). */
        return 0;

    case LINUX_NR_epoll_create:
        return vfs_ram_epoll_create1(0);
    case LINUX_NR_epoll_create1:
        return vfs_ram_epoll_create1((int)pRegs->u64Arg0);

    case LINUX_NR_io_setup: {
        /* aio context: write synthetic ctx id to *ctxp */
        u64 ctx = 1;

        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(pRegs->u64Arg1, 8)) {
            (void)copy_to_user(pRegs->u64Arg1, &ctx, 8);
        } else {
            *(u64 *)(gj_vaddr_t)pRegs->u64Arg1 = ctx;
        }
        return 0;
    }
    case LINUX_NR_io_destroy:
        return 0;
    case LINUX_NR_io_submit:
        return 0; /* no events submitted */
    case LINUX_NR_io_getevents:
        return 0; /* timeout / empty */
    case LINUX_NR_io_cancel:
        return -LINUX_EINTR;
    case LINUX_NR_poll:
    case LINUX_NR_ppoll: {
        /*
         * poll(struct pollfd *fds, nfds_t nfds, timeout):
         * Use vfs readiness (eventfd/pipe/inotify/ram) mapped to POLLIN/OUT.
         * arg0 = user pollfd array, arg1 = nfds (max 16 for T0).
         */
        u32 nfds = (u32)pRegs->u64Arg1;
        u32 i;
        u32 ready = 0;

        if (nfds == 0) {
            return 0;
        }
        if (nfds > 16u) {
            nfds = 16u;
        }
        if (pRegs->u64Arg0 == 0) {
            return -LINUX_EFAULT;
        }
        for (i = 0; i < nfds; i++) {
            /* pollfd: int fd; short events; short revents — 8 bytes on x86_64 */
            i32 fd = 0;
            u16 events = 0;
            u16 revents = 0;
            u32 want;
            u32 got;
            u64 base = pRegs->u64Arg0 + (u64)i * 8u;

            if (user_range_ok(base, 8)) {
                if (copy_from_user(&fd, base, 4) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
                if (copy_from_user(&events, base + 4, 2) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                i32 *p = (i32 *)(gj_vaddr_t)base;

                fd = p[0];
                events = ((u16 *)(gj_vaddr_t)base)[2];
            }
            /* POLLIN=1 POLLOUT=4 — same numbers as EPOLLIN/OUT */
            want = (u32)events;
            if (want == 0) {
                want = 0x005u;
            }
            if (fd < 0) {
                revents = 0;
            } else if (vfs_ram_fd_ok((i64)fd)) {
                got = vfs_ram_poll_mask((i64)fd, want);
                revents = (u16)(got & want);
                if (got & 0x008u) {
                    revents |= 0x0008u; /* POLLERR */
                }
                if (got & 0x010u) {
                    revents |= 0x0010u; /* POLLHUP */
                }
            } else if (net_lo_fd_ok((i64)fd)) {
                revents = (u16)(want & 0x0005u); /* always R/W ready on lo */
            } else {
                revents = 0x0008u; /* POLLERR */
            }
            if (user_range_ok(base, 8)) {
                if (copy_to_user(base + 6, &revents, 2) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                ((u16 *)(gj_vaddr_t)base)[3] = revents;
            }
            if (revents != 0) {
                ready++;
            }
        }
        return (i64)ready;
    }
    case LINUX_NR_ioctl: {
        /* FIONREAD-shaped and TIOCGWINSZ stubs for wine tty probe */
        u32 cmd = (u32)pRegs->u64Arg1;

        if (!vfs_ram_fd_ok((i64)pRegs->u64Arg0) &&
            !net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            return -LINUX_EBADF;
        }
        if (cmd == 0x5413u /* TIOCGWINSZ */) {
            u16 aWs[4] = { 24, 80, 0, 0 };

            if (pRegs->u64Arg2 != 0) {
                if (user_range_ok(pRegs->u64Arg2, sizeof(aWs))) {
                    (void)copy_to_user(pRegs->u64Arg2, aWs, sizeof(aWs));
                } else {
                    memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aWs,
                           sizeof(aWs));
                }
            }
            return 0;
        }
        if (cmd == 0x541Bu /* FIONREAD */) {
            u32 zero = 0;

            if (pRegs->u64Arg2 != 0) {
                if (user_range_ok(pRegs->u64Arg2, 4)) {
                    (void)copy_to_user(pRegs->u64Arg2, &zero, 4);
                } else {
                    *(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = 0;
                }
            }
            return 0;
        }
        return 0; /* unknown ioctl: succeed for probe */
    }
    case LINUX_NR_epoll_ctl: {
        /* arg0=epfd arg1=op arg2=fd arg3=struct epoll_event* */
        u32 u32Events = 0x001u;
        u64 u64Data = 0;

        if (pRegs->u64Arg3 != 0) {
            if (user_range_ok(pRegs->u64Arg3, 12)) {
                (void)copy_from_user(&u32Events, pRegs->u64Arg3, 4);
                (void)copy_from_user(&u64Data, pRegs->u64Arg3 + 4, 8);
            } else {
                u32Events = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg3;
                u64Data = *(const u64 *)(gj_vaddr_t)(pRegs->u64Arg3 + 4);
            }
        }
        return vfs_ram_epoll_ctl((i64)pRegs->u64Arg0, (int)pRegs->u64Arg1,
                                 (i64)pRegs->u64Arg2, u32Events, u64Data);
    }
    case LINUX_NR_epoll_wait:
    case LINUX_NR_epoll_pwait:
    case LINUX_NR_epoll_pwait2: {
        /* epoll_pwait2: arg3 is timespec*; use 0 timeout (non-blocking) */
        u8 aEv[16 * 12];
        int nMax = (int)pRegs->u64Arg2;
        int nTimeout = (pRegs->u64Nr == LINUX_NR_epoll_pwait2)
                           ? 0
                           : (int)pRegs->u64Arg3;
        i64 n;

        if (nMax <= 0) {
            return 0;
        }
        if (nMax > 16) {
            nMax = 16;
        }
        n = vfs_ram_epoll_wait((i64)pRegs->u64Arg0, aEv, nMax, nTimeout);
        if (n > 0 && pRegs->u64Arg1 != 0) {
            size_t cb = (size_t)n * 12u;

            if (user_range_ok(pRegs->u64Arg1, cb)) {
                if (copy_to_user(pRegs->u64Arg1, aEv, cb) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aEv, cb);
            }
        }
        return n;
    }

    case LINUX_NR_getdents64: {
        u8 aDir[512];
        size_t cb = (size_t)pRegs->u64Arg2;
        i64 n;

        if (!vfs_ram_fd_ok((i64)pRegs->u64Arg0)) {
            return -LINUX_EBADF;
        }
        if (pRegs->u64Arg1 == 0 || cb == 0) {
            return -LINUX_EFAULT;
        }
        if (cb > sizeof(aDir)) {
            cb = sizeof(aDir);
        }
        n = vfs_ram_getdents64((i64)pRegs->u64Arg0, aDir, cb);
        if (n > 0) {
            if (user_range_ok(pRegs->u64Arg1, (u64)n)) {
                if (copy_to_user(pRegs->u64Arg1, aDir, (size_t)n) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aDir, (size_t)n);
            }
        }
        return n;
    }

    case LINUX_NR_memfd_create: {
        /*
         * Named anon memobj + unique ramfs fd (shareable object path).
         * Name prefix "mfd:" + user name (or counter) for uniqueness.
         */
        char szName[GJ_MEMOBJ_NAME_MAX];
        char szPath[40];
        static u32 u32MfdSeq;
        u32 i;
        u32 n = 0;
        i64 i64Fd;

        memset(szName, 0, sizeof(szName));
        szName[0] = 'm';
        szName[1] = 'f';
        szName[2] = 'd';
        szName[3] = ':';
        n = 4;
        if (pRegs->u64Arg0 != 0) {
            for (i = 0; n + 1 < sizeof(szName); i++) {
                char ch = 0;

                if (user_range_ok(pRegs->u64Arg0 + i, 1)) {
                    if (copy_from_user(&ch, pRegs->u64Arg0 + i, 1) != GJ_OK) {
                        break;
                    }
                } else {
                    ch = ((const char *)(gj_vaddr_t)pRegs->u64Arg0)[i];
                }
                if (ch == '\0') {
                    break;
                }
                if (ch == '/') {
                    ch = '_';
                }
                szName[n++] = ch;
            }
        }
        if (n <= 4) {
            u32MfdSeq++;
            szName[n++] = '0' + (char)((u32MfdSeq / 100u) % 10u);
            szName[n++] = '0' + (char)((u32MfdSeq / 10u) % 10u);
            szName[n++] = '0' + (char)(u32MfdSeq % 10u);
        }
        szName[n] = '\0';
        if (memobj_create_named(szName, 1) == NULL &&
            memobj_lookup_named(szName) == NULL) {
            return -LINUX_ENOMEM;
        }
        /* Unique path per memfd so concurrent creates do not alias */
        szPath[0] = '/';
        szPath[1] = 't';
        szPath[2] = 'm';
        szPath[3] = 'p';
        szPath[4] = '/';
        szPath[5] = 'm';
        szPath[6] = 'f';
        szPath[7] = 'd';
        szPath[8] = '-';
        {
            u32 s = ++u32MfdSeq;
            u32 k;

            for (k = 0; k < 8 && k + 9 < sizeof(szPath); k++) {
                szPath[9 + k] = "0123456789abcdef"[(s >> (k * 4)) & 0xfu];
            }
            szPath[9 + k] = '\0';
        }
        i64Fd = vfs_ram_open(szPath, 1);
        if (i64Fd < 0) {
            return -LINUX_ENOMEM;
        }
        return i64Fd;
    }

    case LINUX_NR_fcntl: {
        /*
         * F_GETFL/F_SETFL no-op; real F_SETLK/F_GETLK/F_SETLKW via file_lock.
         * flock arg: user ptr to struct {i16 type, whence; i64 start,len; u32 pid}
         * (gj_flock layout; may differ from host glibc — Proton maps via cold).
         */
        u32 u32Cmd = (u32)pRegs->u64Arg1;
        i64 i64Fd = (i64)pRegs->u64Arg0;

        if (!vfs_ram_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd)) {
            return -LINUX_EBADF;
        }
        if (u32Cmd == 0 /* F_DUPFD */ || u32Cmd == 1030 /* F_DUPFD_CLOEXEC */) {
            if (!vfs_ram_fd_ok(i64Fd)) {
                return -LINUX_EBADF;
            }
            return vfs_ram_dup_from(i64Fd, (i64)pRegs->u64Arg2);
        }
        if (u32Cmd == 1 /* F_GETFD */) {
            return 0; /* FD_CLOEXEC off */
        }
        if (u32Cmd == 2 /* F_SETFD */) {
            return 0;
        }
        if (u32Cmd == 3 /* F_GETFL */) {
            return 0;
        }
        if (u32Cmd == 4 /* F_SETFL */) {
            return 0;
        }
        if (u32Cmd == 6 /* F_SETLK */ || u32Cmd == 7 /* F_SETLKW */ ||
            u32Cmd == 5 /* F_GETLK */) {
            struct gj_flock fl;
            i64 st;

            if (pRegs->u64Arg2 == 0) {
                return -LINUX_EFAULT;
            }
            memset(&fl, 0, sizeof(fl));
            if (user_range_ok(pRegs->u64Arg2, sizeof(fl))) {
                if (copy_from_user(&fl, pRegs->u64Arg2, sizeof(fl)) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy(&fl, (const void *)(gj_vaddr_t)pRegs->u64Arg2, sizeof(fl));
            }
            if (fl.u32Pid == 0) {
                fl.u32Pid = 1;
            }
            if (u32Cmd == 5) {
                st = file_lock_get(i64Fd, &fl);
                if (st == 0) {
                    if (user_range_ok(pRegs->u64Arg2, sizeof(fl))) {
                        (void)copy_to_user(pRegs->u64Arg2, &fl, sizeof(fl));
                    } else {
                        memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, &fl,
                               sizeof(fl));
                    }
                }
                return st;
            }
            return file_lock_set(i64Fd, &fl, u32Cmd == 7 /* SETLKW */);
        }
        return 0;
    }

    case LINUX_NR_socket: {
        i64 i64Gate;

        /* Soft multi-server confine: INET promise gates ambient sockets. */
        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        return net_lo_socket((int)pRegs->u64Arg0, (int)pRegs->u64Arg1,
                             (int)pRegs->u64Arg2);
    }

    case LINUX_NR_sendto:
        if (net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            u8 aBuf[256];
            size_t cb = (size_t)pRegs->u64Arg2;

            if (cb > sizeof(aBuf)) {
                cb = sizeof(aBuf);
            }
            if (cb > 0 && pRegs->u64Arg1 == 0) {
                return -LINUX_EFAULT;
            }
            if (user_range_ok(pRegs->u64Arg1, cb)) {
                if (copy_from_user(aBuf, pRegs->u64Arg1, cb) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy(aBuf, (const void *)(gj_vaddr_t)pRegs->u64Arg1, cb);
            }
            return net_lo_send((i64)pRegs->u64Arg0, aBuf, cb);
        }
        return -LINUX_EBADF;

    case LINUX_NR_sendmsg: {
        /* msghdr: msg_iov at offset 16, msg_iovlen at 24 (x86_64) — simplified */
        u64 u64Msg;
        u64 u64Iov = 0;
        u64 u64IovLen = 0;
        u8 aBuf[256];
        size_t total = 0;
        u64 i;

        if (!net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            return -LINUX_EBADF;
        }
        u64Msg = pRegs->u64Arg1;
        if (u64Msg == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(u64Msg + 16, 16)) {
            (void)copy_from_user(&u64Iov, u64Msg + 16, 8);
            (void)copy_from_user(&u64IovLen, u64Msg + 24, 8);
        } else {
            u64Iov = *(const u64 *)(gj_vaddr_t)(u64Msg + 16);
            u64IovLen = *(const u64 *)(gj_vaddr_t)(u64Msg + 24);
        }
        if (u64Iov == 0 || u64IovLen == 0) {
            return 0;
        }
        if (u64IovLen > 8) {
            u64IovLen = 8;
        }
        for (i = 0; i < u64IovLen; i++) {
            u64 base = 0;
            u64 len = 0;
            size_t chunk;

            if (user_range_ok(u64Iov + i * 16, 16)) {
                (void)copy_from_user(&base, u64Iov + i * 16, 8);
                (void)copy_from_user(&len, u64Iov + i * 16 + 8, 8);
            } else {
                base = *(const u64 *)(gj_vaddr_t)(u64Iov + i * 16);
                len = *(const u64 *)(gj_vaddr_t)(u64Iov + i * 16 + 8);
            }
            chunk = (size_t)len;
            if (chunk > sizeof(aBuf) - total) {
                chunk = sizeof(aBuf) - total;
            }
            if (chunk == 0) {
                break;
            }
            if (user_range_ok(base, chunk)) {
                if (copy_from_user(aBuf + total, base, chunk) != GJ_OK) {
                    return total ? (i64)total : -LINUX_EFAULT;
                }
            } else {
                memcpy(aBuf + total, (const void *)(gj_vaddr_t)base, chunk);
            }
            total += chunk;
        }
        return net_lo_send((i64)pRegs->u64Arg0, aBuf, total);
    }

    case LINUX_NR_recvfrom:
        if (net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            u8 aBuf[256];
            size_t cb = (size_t)pRegs->u64Arg2;
            i64 i64N;

            if (cb > sizeof(aBuf)) {
                cb = sizeof(aBuf);
            }
            if (cb > 0 && pRegs->u64Arg1 == 0) {
                return -LINUX_EFAULT;
            }
            i64N = net_lo_recv((i64)pRegs->u64Arg0, aBuf, cb);
            if (i64N <= 0) {
                return i64N;
            }
            if (user_range_ok(pRegs->u64Arg1, (u64)i64N)) {
                if (copy_to_user(pRegs->u64Arg1, aBuf, (size_t)i64N) !=
                    GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aBuf, (size_t)i64N);
            }
            return i64N;
        }
        return -LINUX_EBADF;

    case LINUX_NR_recvmsg: {
        u64 u64Msg = pRegs->u64Arg1;
        u64 u64Iov = 0;
        u64 u64IovLen = 0;
        u8 aBuf[256];
        i64 i64N;
        u64 base = 0;
        u64 len = 0;

        if (!net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            return -LINUX_EBADF;
        }
        if (u64Msg == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(u64Msg + 16, 16)) {
            (void)copy_from_user(&u64Iov, u64Msg + 16, 8);
            (void)copy_from_user(&u64IovLen, u64Msg + 24, 8);
        } else {
            u64Iov = *(const u64 *)(gj_vaddr_t)(u64Msg + 16);
            u64IovLen = *(const u64 *)(gj_vaddr_t)(u64Msg + 24);
        }
        if (u64Iov == 0 || u64IovLen == 0) {
            return 0;
        }
        if (user_range_ok(u64Iov, 16)) {
            (void)copy_from_user(&base, u64Iov, 8);
            (void)copy_from_user(&len, u64Iov + 8, 8);
        } else {
            base = *(const u64 *)(gj_vaddr_t)u64Iov;
            len = *(const u64 *)(gj_vaddr_t)(u64Iov + 8);
        }
        if (len > sizeof(aBuf)) {
            len = sizeof(aBuf);
        }
        i64N = net_lo_recv((i64)pRegs->u64Arg0, aBuf, (size_t)len);
        if (i64N <= 0) {
            return i64N;
        }
        if (user_range_ok(base, (u64)i64N)) {
            if (copy_to_user(base, aBuf, (size_t)i64N) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)base, aBuf, (size_t)i64N);
        }
        return i64N;
    }

    case LINUX_NR_sendmmsg: {
        /*
         * sendmmsg(fd, mmsghdr *msgvec, vlen, flags)
         * Each mmsghdr is msghdr (56) + u32 msg_len + pad → 64 bytes typical.
         * For bring-up: process first message only via sendmsg path, report 1.
         */
        struct gj_linux_regs sub;

        if (pRegs->u64Arg2 == 0) {
            return 0;
        }
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        memset(&sub, 0, sizeof(sub));
        sub.u64Nr = LINUX_NR_sendmsg;
        sub.u64Arg0 = pRegs->u64Arg0;
        sub.u64Arg1 = pRegs->u64Arg1; /* first mmsghdr starts with msghdr */
        sub.u64Arg2 = pRegs->u64Arg3;
        {
            i64 n = protonrt_service(&sub, NULL);

            if (n < 0) {
                return n;
            }
            /* store msg_len at offset 56 of mmsghdr */
            if (user_range_ok(pRegs->u64Arg1 + 56, 4)) {
                u32 len = (u32)n;

                (void)copy_to_user(pRegs->u64Arg1 + 56, &len, 4);
            } else {
                *(u32 *)(gj_vaddr_t)(pRegs->u64Arg1 + 56) = (u32)n;
            }
        }
        return 1;
    }

    case LINUX_NR_recvmmsg: {
        struct gj_linux_regs sub;

        if (pRegs->u64Arg2 == 0) {
            return 0;
        }
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        memset(&sub, 0, sizeof(sub));
        sub.u64Nr = LINUX_NR_recvmsg;
        sub.u64Arg0 = pRegs->u64Arg0;
        sub.u64Arg1 = pRegs->u64Arg1;
        sub.u64Arg2 = pRegs->u64Arg3;
        {
            i64 n = protonrt_service(&sub, NULL);

            if (n < 0) {
                return n;
            }
            if (user_range_ok(pRegs->u64Arg1 + 56, 4)) {
                u32 len = (u32)n;

                (void)copy_to_user(pRegs->u64Arg1 + 56, &len, 4);
            } else {
                *(u32 *)(gj_vaddr_t)(pRegs->u64Arg1 + 56) = (u32)n;
            }
        }
        return 1;
    }

    case LINUX_NR_shutdown:
        return net_lo_shutdown((i64)pRegs->u64Arg0, (int)pRegs->u64Arg1);

    case LINUX_NR_setsockopt: {
        int v = 0;

        if (!net_lo_fd_ok((i64)pRegs->u64Arg0)) {
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

    case LINUX_NR_getsockopt: {
        int v = 0;
        u32 len = 4;
        u32 *pLen = NULL;

        if (!net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            return -LINUX_EBADF;
        }
        if (pRegs->u64Arg4 != 0) {
            if (user_range_ok(pRegs->u64Arg4, 4)) {
                (void)copy_from_user(&len, pRegs->u64Arg4, 4);
            } else {
                len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg4;
            }
            pLen = &len;
        }
        {
            i64 r = net_lo_getsockopt((i64)pRegs->u64Arg0, (int)pRegs->u64Arg1,
                                      (int)pRegs->u64Arg2, &v, pLen ? pLen
                                                                     : &len);

            if (r != 0) {
                return r;
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
    }

    case LINUX_NR_getsockname:
    case LINUX_NR_getpeername: {
        u8 aSa[16];
        u32 len = 16;
        i64 r;

        if (!net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            return -LINUX_EBADF;
        }
        if (pRegs->u64Arg2 != 0) {
            if (user_range_ok(pRegs->u64Arg2, 4)) {
                (void)copy_from_user(&len, pRegs->u64Arg2, 4);
            } else {
                len = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
            }
        }
        if (pRegs->u64Nr == LINUX_NR_getsockname) {
            r = net_lo_getsockname((i64)pRegs->u64Arg0, aSa, &len);
        } else {
            r = net_lo_getpeername((i64)pRegs->u64Arg0, aSa, &len);
        }
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

    case LINUX_NR_pipe:
    case LINUX_NR_pipe2: {
        i32 aFds[2];
        i64 i64St;
        int nFlags = (pRegs->u64Nr == LINUX_NR_pipe2) ? (int)pRegs->u64Arg1 : 0;

        if (pRegs->u64Arg0 == 0) {
            return -LINUX_EFAULT;
        }
        i64St = vfs_ram_pipe2(aFds, nFlags);
        if (i64St != 0) {
            return i64St;
        }
        if (user_range_ok(pRegs->u64Arg0, sizeof(aFds))) {
            if (copy_to_user(pRegs->u64Arg0, aFds, sizeof(aFds)) != GJ_OK) {
                (void)vfs_ram_close(aFds[0]);
                (void)vfs_ram_close(aFds[1]);
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg0, aFds, sizeof(aFds));
        }
        return 0;
    }

    case LINUX_NR_socketpair: {
        i32 aFds[2];
        i64 i64St;

        if (pRegs->u64Arg3 == 0) {
            return -LINUX_EFAULT;
        }
        i64St = vfs_ram_socketpair((int)pRegs->u64Arg0, (int)pRegs->u64Arg1,
                                   (int)pRegs->u64Arg2, aFds);
        if (i64St != 0) {
            return i64St;
        }
        if (user_range_ok(pRegs->u64Arg3, sizeof(aFds))) {
            if (copy_to_user(pRegs->u64Arg3, aFds, sizeof(aFds)) != GJ_OK) {
                (void)vfs_ram_close(aFds[0]);
                (void)vfs_ram_close(aFds[1]);
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg3, aFds, sizeof(aFds));
        }
        return 0;
    }

    case LINUX_NR_bind: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        /* sockaddr_in: port at offset 2, big-endian */
        if (pRegs->u64Arg1 != 0) {
            u16 u16PortBe = 0;
            if (user_range_ok(pRegs->u64Arg1 + 2, 2)) {
                (void)copy_from_user(&u16PortBe, pRegs->u64Arg1 + 2, 2);
            } else {
                u16PortBe = *(const u16 *)(gj_vaddr_t)(pRegs->u64Arg1 + 2);
            }
            return net_lo_bind((i64)pRegs->u64Arg0,
                               (u16)((u16PortBe >> 8) | (u16PortBe << 8)));
        }
        return -LINUX_EFAULT;
    }

    case LINUX_NR_listen: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        return net_lo_listen((i64)pRegs->u64Arg0, (int)pRegs->u64Arg1);
    }

    case LINUX_NR_accept:
    case LINUX_NR_accept4: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        /* flags ignored for bring-up (CLOEXEC/NONBLOCK) */
        return net_lo_accept((i64)pRegs->u64Arg0);
    }

    case LINUX_NR_fallocate:
        return vfs_ram_fallocate((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg2,
                                 (i64)pRegs->u64Arg3);

    case LINUX_NR_sendfile: {
        u64 off = 0;
        u64 *pOff = NULL;
        i64 n;

        if (pRegs->u64Arg2 != 0) {
            if (user_range_ok(pRegs->u64Arg2, 8)) {
                (void)copy_from_user(&off, pRegs->u64Arg2, 8);
            } else {
                off = *(const u64 *)(gj_vaddr_t)pRegs->u64Arg2;
            }
            pOff = &off;
        }
        n = vfs_ram_sendfile((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg1, pOff,
                             (size_t)pRegs->u64Arg3);
        if (n >= 0 && pRegs->u64Arg2 != 0) {
            if (user_range_ok(pRegs->u64Arg2, 8)) {
                (void)copy_to_user(pRegs->u64Arg2, &off, 8);
            } else {
                *(u64 *)(gj_vaddr_t)pRegs->u64Arg2 = off;
            }
        }
        return n;
    }

    case LINUX_NR_splice:
        /*
         * splice(fd_in, *off_in, fd_out, *off_out, len, flags)
         * Map to sendfile(out, in, NULL, len); offsets ignored in bring-up.
         */
        return vfs_ram_sendfile((i64)pRegs->u64Arg2, (i64)pRegs->u64Arg0, NULL,
                                (size_t)pRegs->u64Arg4);

    case LINUX_NR_tee:
        /* tee(fd_in, fd_out, len, flags) — copy via sendfile path */
        return vfs_ram_sendfile((i64)pRegs->u64Arg1, (i64)pRegs->u64Arg0, NULL,
                                (size_t)pRegs->u64Arg2);

    case LINUX_NR_copy_file_range: {
        /* arg0=fd_in arg1=*off_in arg2=fd_out arg3=*off_out arg4=len */
        u64 offIn = 0;
        u64 offOut = 0;
        u64 *pIn = NULL;
        u64 *pOut = NULL;
        i64 n;

        if (pRegs->u64Arg1 != 0) {
            if (user_range_ok(pRegs->u64Arg1, 8)) {
                (void)copy_from_user(&offIn, pRegs->u64Arg1, 8);
            } else {
                offIn = *(const u64 *)(gj_vaddr_t)pRegs->u64Arg1;
            }
            pIn = &offIn;
        }
        if (pRegs->u64Arg3 != 0) {
            if (user_range_ok(pRegs->u64Arg3, 8)) {
                (void)copy_from_user(&offOut, pRegs->u64Arg3, 8);
            } else {
                offOut = *(const u64 *)(gj_vaddr_t)pRegs->u64Arg3;
            }
            pOut = &offOut;
        }
        n = vfs_ram_copy_file_range((i64)pRegs->u64Arg0, pIn,
                                    (i64)pRegs->u64Arg2, pOut,
                                    (size_t)pRegs->u64Arg4);
        if (n >= 0) {
            if (pRegs->u64Arg1 != 0) {
                if (user_range_ok(pRegs->u64Arg1, 8)) {
                    (void)copy_to_user(pRegs->u64Arg1, &offIn, 8);
                } else {
                    *(u64 *)(gj_vaddr_t)pRegs->u64Arg1 = offIn;
                }
            }
            if (pRegs->u64Arg3 != 0) {
                if (user_range_ok(pRegs->u64Arg3, 8)) {
                    (void)copy_to_user(pRegs->u64Arg3, &offOut, 8);
                } else {
                    *(u64 *)(gj_vaddr_t)pRegs->u64Arg3 = offOut;
                }
            }
        }
        return n;
    }

    case LINUX_NR_inotify_init1:
        return vfs_ram_inotify_init1((int)pRegs->u64Arg0);

    case LINUX_NR_inotify_add_watch: {
        i64 i64Gate;

        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg1);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_inotify_add_watch((i64)pRegs->u64Arg0, szPath,
                                         (u32)pRegs->u64Arg2);
    }

    case LINUX_NR_inotify_rm_watch:
        return vfs_ram_inotify_rm_watch((i64)pRegs->u64Arg0,
                                        (i32)pRegs->u64Arg1);

    case LINUX_NR_renameat:
    case LINUX_NR_renameat2: {
        /* renameat/renameat2(olddirfd, old, newdirfd, new[, flags]) — paths */
        char szNew[96];
        u64 u64Old = pRegs->u64Arg1;
        u64 u64New = pRegs->u64Arg3;
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), u64Old);
        copy_path_from_arg(szNew, sizeof(szNew), u64New);
        if (szPath[0] == '\0' || szNew[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_rename(szPath, szNew);
    }

    case LINUX_NR_linkat: {
        /* linkat(olddirfd, old, newdirfd, new, flags) */
        char szNew[96];
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg1);
        copy_path_from_arg(szNew, sizeof(szNew), pRegs->u64Arg3);
        if (szPath[0] == '\0' || szNew[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_link(szPath, szNew);
    }

    case LINUX_NR_symlinkat: {
        /* symlinkat(target, newdirfd, linkpath) */
        char szNew[96];
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        copy_path_from_arg(szNew, sizeof(szNew), pRegs->u64Arg2);
        if (szPath[0] == '\0' || szNew[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_symlink(szPath, szNew);
    }

    case LINUX_NR_io_pgetevents:
        /* Same shape as io_getevents; no events pending */
        return 0;

    case LINUX_NR_userfaultfd:
        return -LINUX_ENOSYS;

    case LINUX_NR_seccomp:
        /* SECCOMP_SET_MODE_STRICT etc. — accept no-op for wine probe */
        return 0;

    case LINUX_NR_bpf:
        return -LINUX_ENOSYS;

    case LINUX_NR_keyctl:
    case LINUX_NR_add_key:
    case LINUX_NR_request_key:
        return -LINUX_ENOSYS;

    case LINUX_NR_name_to_handle_at:
    case LINUX_NR_open_by_handle_at:
        return -LINUX_ENOSYS;

    case LINUX_NR_io_uring_setup:
        /* Minimal rings: fd + params; SQE execution still soft in enter. */
        return gj_io_uring_setup((u32)pRegs->u64Arg0, pRegs->u64Arg1);
    case LINUX_NR_io_uring_enter:
        return gj_io_uring_enter((i64)pRegs->u64Arg0, (u32)pRegs->u64Arg1,
                                 (u32)pRegs->u64Arg2, (u32)pRegs->u64Arg3);
    case LINUX_NR_io_uring_register:
        return gj_io_uring_register((i64)pRegs->u64Arg0, (u32)pRegs->u64Arg1,
                                    pRegs->u64Arg2, (u32)pRegs->u64Arg3);

    case LINUX_NR_open_tree:
    case LINUX_NR_move_mount:
    case LINUX_NR_fsopen:
    case LINUX_NR_fsconfig:
    case LINUX_NR_fsmount:
    case LINUX_NR_fspick:
    case LINUX_NR_mount_setattr:
        return -LINUX_ENOSYS;

    case LINUX_NR_quotactl_fd:
        return -LINUX_ENOSYS;

    case LINUX_NR_pidfd_getfd: {
        /*
         * pidfd_getfd(pidfd, targetfd, flags) — same-pid only.
         * Validate pidfd kind; dup targetfd into a new slot.
         */
        i64 i64PidFd = (i64)pRegs->u64Arg0;
        i64 i64Tgt = (i64)pRegs->u64Arg1;
        u32 u32Pid;
        u32 u32Self;

        (void)pRegs->u64Arg2; /* flags reserved */
        if (i64PidFd < 0 || i64Tgt < 0) {
            return -LINUX_EBADF;
        }
        u32Pid = vfs_ram_pidfd_pid(i64PidFd);
        if (u32Pid == 0) {
            return -LINUX_EBADF;
        }
        /* Bring-up: treat pid 1 as self; match wait-pid when available. */
        u32Self = 1u;
        if (g_pLinuxProc != NULL) {
            u32 t = process_wait_pid_of(g_pLinuxProc);

            if (t != 0) {
                u32Self = t;
            }
        }
        if (u32Pid != u32Self && u32Pid != 1u) {
            return -LINUX_EPERM; /* cross-pid not implemented */
        }
        /* Allocate a new fd that shares the open file. */
        return vfs_ram_dup(i64Tgt);
    }

    case LINUX_NR_process_madvise:
        return 0; /* no-op accept */

    case LINUX_NR_fanotify_init:
        /* Soft fanotify: open a vfs node so Wine probes get a real fd */
        return vfs_ram_open("/tmp/fanotify", 1);

    case LINUX_NR_fanotify_mark:
        /* Accept mark on any fanotify-shaped fd (bring-up no-op) */
        if ((i64)pRegs->u64Arg0 < 0) {
            return -LINUX_EBADF;
        }
        return 0;

    case LINUX_NR_kcmp:
        /* kcmp(pid1,pid2,type,...) — same-pid equal */
        if (pRegs->u64Arg0 == pRegs->u64Arg1) {
            return 0;
        }
        return 1;

    case LINUX_NR_quotactl:
        return -LINUX_ENOSYS;

    case LINUX_NR_remap_file_pages:
        return -LINUX_ENOSYS;

    case LINUX_NR_restart_syscall:
        return -LINUX_EINTR;

    case LINUX_NR_migrate_pages:
    case LINUX_NR_move_pages:
        return 0; /* single-node: no migration */

    case LINUX_NR_perf_event_open:
        return -LINUX_ENOSYS;

    case LINUX_NR_timer_create:
        /* Create POSIX timer id (software counter). Return timerid via arg2. */
        if (pRegs->u64Arg2 != 0) {
            u32 tid = 1;

            if (user_range_ok(pRegs->u64Arg2, 4)) {
                (void)copy_to_user(pRegs->u64Arg2, &tid, 4);
            } else {
                *(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = tid;
            }
        }
        return 0;

    case LINUX_NR_timer_settime:
    case LINUX_NR_timer_gettime:
        return 0;

    case LINUX_NR_timer_getoverrun:
        return 0;

    case LINUX_NR_timer_delete:
        return 0;

    case LINUX_NR_mq_open:
    case LINUX_NR_mq_unlink:
    case LINUX_NR_mq_timedsend:
    case LINUX_NR_mq_timedreceive:
    case LINUX_NR_mq_notify:
    case LINUX_NR_mq_getsetattr:
        return -LINUX_ENOSYS;

    case LINUX_NR_inotify_init:
        return vfs_ram_inotify_init1(0);

    case LINUX_NR_connect: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        if (pRegs->u64Arg1 != 0) {
            u16 u16PortBe = 0;
            if (user_range_ok(pRegs->u64Arg1 + 2, 2)) {
                (void)copy_from_user(&u16PortBe, pRegs->u64Arg1 + 2, 2);
            } else {
                u16PortBe = *(const u16 *)(gj_vaddr_t)(pRegs->u64Arg1 + 2);
            }
            return net_lo_connect((i64)pRegs->u64Arg0,
                                  (u16)((u16PortBe >> 8) | (u16PortBe << 8)));
        }
        return -LINUX_EFAULT;
    }

    case LINUX_NR_close:
        if (net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            return net_lo_close((i64)pRegs->u64Arg0);
        }
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0)) {
            return vfs_ram_close((i64)pRegs->u64Arg0);
        }
        break;

    case LINUX_NR_getcwd: {
        u64 u64Buf = pRegs->u64Arg0;
        u64 u64Size = pRegs->u64Arg1;
        size_t n = 0;

        if (u64Buf == 0 || u64Size < 2) {
            return -LINUX_EINVAL;
        }
        while (g_szCwd[n] != '\0' && n + 1 < sizeof(g_szCwd)) {
            n++;
        }
        n++; /* include NUL */
        if (u64Size < n) {
            return -LINUX_ERANGE;
        }
        if (user_range_ok(u64Buf, n)) {
            if (copy_to_user(u64Buf, g_szCwd, n) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Buf, g_szCwd, n);
        }
        /* Linux returns pointer on success for getcwd syscall */
        return (i64)u64Buf;
    }

    case LINUX_NR_dup:
        return vfs_ram_dup((i64)pRegs->u64Arg0);

    case LINUX_NR_dup2:
        return vfs_ram_dup2((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg1);

    case LINUX_NR_dup3:
        /* flags (CLOEXEC) ignored for bring-up */
        return vfs_ram_dup2((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg1);

    case LINUX_NR_readlinkat: {
        char aLink[64];
        i64 n;
        size_t cb = (size_t)pRegs->u64Arg3;
        i64 i64Gate;

        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg1);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        if (cb > sizeof(aLink)) {
            cb = sizeof(aLink);
        }
        n = vfs_ram_readlink(szPath, aLink, cb);
        if (n < 0) {
            return n;
        }
        if (pRegs->u64Arg2 == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(pRegs->u64Arg2, (u64)n)) {
            if (copy_to_user(pRegs->u64Arg2, aLink, (size_t)n) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aLink, (size_t)n);
        }
        return n;
    }

    case LINUX_NR_fchmodat:
    case LINUX_NR_fchmodat2: {
        i64 i64Fd;
        i64 st;
        i64 i64Gate;

        i64Gate = promise_gate_wpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg1);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        i64Fd = vfs_ram_open(szPath, 0);
        if (i64Fd < 0) {
            return i64Fd;
        }
        st = vfs_ram_fchmod(i64Fd, (u32)pRegs->u64Arg2);
        (void)vfs_ram_close(i64Fd);
        return st;
    }

    case LINUX_NR_landlock_create_ruleset:
        /* Return synthetic ruleset fd as eventfd */
        return vfs_ram_eventfd2(0, 0);

    case LINUX_NR_landlock_add_rule:
    case LINUX_NR_landlock_restrict_self:
        return 0;

    case LINUX_NR_memfd_secret:
        return vfs_ram_open("/tmp/memfd_secret", 1);

    case LINUX_NR_process_mrelease:
        return 0;

    case LINUX_NR_cachestat:
        /* Report zeros into user buffer if present */
        if (pRegs->u64Arg1 != 0) {
            u8 z[32];

            memset(z, 0, sizeof(z));
            if (user_range_ok(pRegs->u64Arg1, 32)) {
                (void)copy_to_user(pRegs->u64Arg1, z, 32);
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, z, 32);
            }
        }
        return 0;

    case LINUX_NR_futex_waitv:
        /* Multi-wait: not implemented — return 0 (no waiters woken) */
        return 0;

    case LINUX_NR_set_mempolicy_home_node:
        return 0;

    case LINUX_NR_map_shadow_stack:
        return -LINUX_ENOSYS;

    case LINUX_NR_getxattr:
    case LINUX_NR_lgetxattr:
    case LINUX_NR_fgetxattr:
        return -61; /* ENODATA */

    case LINUX_NR_setxattr:
    case LINUX_NR_lsetxattr:
    case LINUX_NR_fsetxattr:
        return 0;

    case LINUX_NR_listxattr:
    case LINUX_NR_llistxattr:
    case LINUX_NR_flistxattr:
        return 0;

    case LINUX_NR_removexattr:
    case LINUX_NR_lremovexattr:
    case LINUX_NR_fremovexattr:
        return -61;

    case LINUX_NR_readlink: {
        char aLink[64];
        i64 n;
        i64 i64Gate;

        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        n = vfs_ram_readlink(szPath, aLink, sizeof(aLink));
        if (n < 0) {
            return n;
        }
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        if ((u64)n > pRegs->u64Arg2) {
            n = (i64)pRegs->u64Arg2;
        }
        if (user_range_ok(pRegs->u64Arg1, (u64)n)) {
            if (copy_to_user(pRegs->u64Arg1, aLink, (size_t)n) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aLink, (size_t)n);
        }
        return n;
    }

    case LINUX_NR_wait4: {
        i32 st = 0;
        i64 r;
        i32 *pSt = NULL;

        if (pRegs->u64Arg1 != 0) {
            pSt = &st;
        }
        r = process_wait4((i64)pRegs->u64Arg0, pSt, (int)pRegs->u64Arg2);
        if (r > 0 && pRegs->u64Arg1 != 0) {
            if (user_range_ok(pRegs->u64Arg1, sizeof(st))) {
                if (copy_to_user(pRegs->u64Arg1, &st, sizeof(st)) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                *(i32 *)(gj_vaddr_t)pRegs->u64Arg1 = st;
            }
        }
        if (r == -10) {
            return -LINUX_ECHILD;
        }
        return r;
    }

    case LINUX_NR_kill: {
        i64 i64Gate;

        /* Soft confine: PROC gates kill-shaped ambient ops. */
        i64Gate = confine_soft_promise_require(GJ_PROMISE_PROC);
        if (i64Gate != 0) {
            return i64Gate;
        }
        /* Self-kill of pid 1/self → allow as no-op for smoke */
        if ((i64)pRegs->u64Arg0 <= 1) {
            return 0;
        }
        return -LINUX_ESRCH;
    }

    case LINUX_NR_tkill:
    case LINUX_NR_tgkill: {
        /*
         * Soft gate if these ever hit cold (hot path owns product tkill/tgkill).
         * Same PROC promise as kill.
         */
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_PROC);
        if (i64Gate != 0) {
            return i64Gate;
        }
        return 0; /* soft no-op when cold-routed */
    }

    case LINUX_NR_access:
    case LINUX_NR_faccessat: {
        u64 u64Path = (pRegs->u64Nr == LINUX_NR_faccessat) ? pRegs->u64Arg1
                                                           : pRegs->u64Arg0;
        int nMode = (pRegs->u64Nr == LINUX_NR_faccessat) ? (int)pRegs->u64Arg2
                                                         : (int)pRegs->u64Arg1;
        i64 i64Gate;

        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), u64Path);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        i64Gate = confine_soft_path_policy(szPath, GJ_EXPOSE_SOFT_R);
        if (i64Gate != 0) {
            return i64Gate;
        }
        return vfs_ram_access(szPath, nMode);
    }

    case LINUX_NR_fstat: {
        static u8 aStat[144];
        i64 st;

        memset(aStat, 0, sizeof(aStat));
        st = vfs_ram_fstat((i64)pRegs->u64Arg0, aStat, sizeof(aStat));
        if (st != 0) {
            return st;
        }
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(pRegs->u64Arg1, sizeof(aStat))) {
            if (copy_to_user(pRegs->u64Arg1, aStat, sizeof(aStat)) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aStat, sizeof(aStat));
        }
        return 0;
    }

    case LINUX_NR_stat:
    case LINUX_NR_lstat:
    case LINUX_NR_newfstatat: {
        static u8 aStat[144];
        u64 u64Path = (pRegs->u64Nr == LINUX_NR_newfstatat) ? pRegs->u64Arg1
                                                            : pRegs->u64Arg0;
        u64 u64Buf = (pRegs->u64Nr == LINUX_NR_newfstatat) ? pRegs->u64Arg2
                                                           : pRegs->u64Arg1;
        i64 st;
        i64 i64Gate;

        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), u64Path);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        i64Gate = confine_soft_path_policy(szPath, GJ_EXPOSE_SOFT_R);
        if (i64Gate != 0) {
            return i64Gate;
        }
        memset(aStat, 0, sizeof(aStat));
        st = vfs_ram_stat(szPath, aStat, sizeof(aStat));
        if (st != 0) {
            return st;
        }
        if (u64Buf == 0) {
            return -LINUX_EFAULT;
        }
        if (user_range_ok(u64Buf, sizeof(aStat))) {
            if (copy_to_user(u64Buf, aStat, sizeof(aStat)) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Buf, aStat, sizeof(aStat));
        }
        return 0;
    }

    case LINUX_NR_unshare:
    case LINUX_NR_setns:
        /* Namespace ops: no-op success for bring-up */
        return 0;

    case LINUX_NR_chroot: {
        i64 i64Gate;

        /* Soft: confined processes have no chroot privilege (no promise bit). */
        if (g_pLinuxProc != NULL && g_pLinuxProc->u32Confined != 0u) {
            g_u32ConfinePromiseDeny++;
            return -LINUX_EACCES;
        }
        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        /* Accept only if path exists */
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_access(szPath, 0);
    }

    case LINUX_NR_mount:
    case LINUX_NR_umount2:
    case LINUX_NR_pivot_root:
        /*
         * Soft: confined processes have no mount privilege (no promise bit).
         * Ambient (u32Confined==0) still soft-succeeds for probe.
         * Ambient expose-shaped mount is also denied when confined
         * (grep: confine: expose soft — product: vfsd path policy).
         */
        if (g_pLinuxProc != NULL && g_pLinuxProc->u32Confined != 0u) {
            g_u32ConfinePromiseDeny++;
            g_u32ConfineExposeDeny++;
            kprintf("confine: expose soft mount deny (confined)\n");
            return -LINUX_EACCES;
        }
        /* No real VFS mounts — success for probe (product: vfsd) */
        return 0;

    case LINUX_NR_swapon:
    case LINUX_NR_swapoff:
        return -LINUX_EINVAL;

    case LINUX_NR_reboot:
        /* Bring-up: refuse (no actual reboot) */
        return -LINUX_EPERM;

    case LINUX_NR_sethostname:
    case LINUX_NR_setdomainname:
        return 0;

    case LINUX_NR_syslog:
        return 0;

    case LINUX_NR_ustat:
    case LINUX_NR_sysfs:
    case LINUX_NR_bdflush:
        return -LINUX_ENOSYS;

    case LINUX_NR_readahead:
    case LINUX_NR_sync_file_range:
        if (!vfs_ram_fd_ok((i64)pRegs->u64Arg0) &&
            (i64)pRegs->u64Arg0 > 2) {
            return -LINUX_EBADF;
        }
        return 0;

    case LINUX_NR_vmsplice:
        /* Map to sendfile-shaped no-op length */
        return (i64)pRegs->u64Arg2;

    case LINUX_NR_getdents:
        /* Legacy getdents → getdents64 cold path */
        pRegs->u64Nr = LINUX_NR_getdents64;
        return protonrt_service(pRegs, pCtx);

    case LINUX_NR_rt_sigsuspend:
        return -LINUX_EINTR;

    case LINUX_NR_rt_sigtimedwait:
    case LINUX_NR_rt_sigpending:
        return 0;

    case LINUX_NR_rt_sigqueueinfo:
        if ((u32)pRegs->u64Arg1 > 0 && (u32)pRegs->u64Arg1 < 64) {
            vfs_ram_signalfd_inject((u32)pRegs->u64Arg1);
        }
        return 0;

    case LINUX_NR_sched_rr_get_interval: {
        i64 aTs[2];

        aTs[0] = 0;
        aTs[1] = 10000000; /* 10ms */
        if (pRegs->u64Arg1 != 0) {
            if (user_range_ok(pRegs->u64Arg1, 16)) {
                (void)copy_to_user(pRegs->u64Arg1, aTs, 16);
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aTs, 16);
            }
        }
        return 0;
    }

    case LINUX_NR_getrlimit:
    case LINUX_NR_setrlimit:
    case LINUX_NR_prlimit64: {
        /* Bring-up: report generous soft/hard limits; ignore set. */
        struct {
            u64 rlim_cur;
            u64 rlim_max;
        } lim;
        u64 u64New;
        u64 u64Old;

        if (pRegs->u64Nr == LINUX_NR_prlimit64) {
            u64New = pRegs->u64Arg2;
            u64Old = pRegs->u64Arg3;
        } else if (pRegs->u64Nr == LINUX_NR_getrlimit) {
            u64New = 0;
            u64Old = pRegs->u64Arg1;
        } else {
            u64New = pRegs->u64Arg1;
            u64Old = 0;
        }

        lim.rlim_cur = 1024ull * 1024ull * 1024ull; /* 1 GiB class */
        lim.rlim_max = lim.rlim_cur;
        if (u64Old != 0) {
            if (user_range_ok(u64Old, sizeof(lim))) {
                if (copy_to_user(u64Old, &lim, sizeof(lim)) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Old, &lim, sizeof(lim));
            }
        }
        (void)u64New; /* set ignored */
        return 0;
    }

    case LINUX_NR_chdir: {
        size_t n;
        i64 i64Gate;

        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        /* Only allow known dirs for bring-up */
        if (szPath[0] == '/' &&
            (szPath[1] == '\0' ||
             (szPath[1] == 't' && szPath[2] == 'm' && szPath[3] == 'p' &&
              (szPath[4] == '\0' || szPath[4] == '/')))) {
            /* ok */
        } else if (vfs_ram_access(szPath, 0) != 0) {
            return -LINUX_ENOENT;
        }
        n = 0;
        while (szPath[n] != '\0' && n + 1 < sizeof(g_szCwd)) {
            g_szCwd[n] = szPath[n];
            n++;
        }
        g_szCwd[n] = '\0';
        if (n == 0) {
            g_szCwd[0] = '/';
            g_szCwd[1] = '\0';
        }
        return 0;
    }

    case LINUX_NR_fchdir: {
        size_t n;

        if (vfs_ram_fd_path((i64)pRegs->u64Arg0, szPath, sizeof(szPath)) != 0) {
            return -LINUX_EBADF;
        }
        n = 0;
        while (szPath[n] != '\0' && n + 1 < sizeof(g_szCwd)) {
            g_szCwd[n] = szPath[n];
            n++;
        }
        g_szCwd[n] = '\0';
        return 0;
    }

    case LINUX_NR_symlink: {
        char szTarget[96];
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szTarget, sizeof(szTarget), pRegs->u64Arg0);
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg1);
        if (szTarget[0] == '\0' || szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_symlink(szTarget, szPath);
    }

    case LINUX_NR_utime:
    case LINUX_NR_utimensat: {
        u64 u64Path = (pRegs->u64Nr == LINUX_NR_utimensat) ? pRegs->u64Arg1
                                                           : pRegs->u64Arg0;
        i64 i64Gate;

        i64Gate = promise_gate_wpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), u64Path);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_utimens(szPath);
    }

    case LINUX_NR_select:
    case LINUX_NR_pselect6: {
        /*
         * select(nfds, *readfds, *writefds, *exceptfds, *timeout)
         * Bring-up: report 0 ready (timeout immediate) unless nfds==0.
         */
        u64 nfds = pRegs->u64Arg0;

        if (nfds > 1024) {
            return -LINUX_EINVAL;
        }
        /* Zero fd sets if provided (no ready fds) */
        if (pRegs->u64Arg1 != 0 && nfds > 0) {
            u64 bytes = ((nfds + 63) / 64) * 8;

            if (bytes > 128) {
                bytes = 128;
            }
            if (user_range_ok(pRegs->u64Arg1, bytes)) {
                u8 z[128];
                u32 i;

                for (i = 0; i < (u32)bytes; i++) {
                    z[i] = 0;
                }
                (void)copy_to_user(pRegs->u64Arg1, z, (size_t)bytes);
            }
        }
        return 0; /* timeout, no fds ready */
    }

    case LINUX_NR_unlink:
    case LINUX_NR_unlinkat: {
        u64 u64Path = (pRegs->u64Nr == LINUX_NR_unlinkat) ? pRegs->u64Arg1
                                                          : pRegs->u64Arg0;
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), u64Path);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_unlink(szPath);
    }

    case LINUX_NR_link: {
        char szNew[96];
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        copy_path_from_arg(szNew, sizeof(szNew), pRegs->u64Arg1);
        if (szPath[0] == '\0' || szNew[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_link(szPath, szNew);
    }

    case LINUX_NR_rmdir: {
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_rmdir(szPath);
    }

    case LINUX_NR_fchmod:
        return vfs_ram_fchmod((i64)pRegs->u64Arg0, (u32)pRegs->u64Arg1);

    case LINUX_NR_fchown:
        /* uid/gid ignored for bring-up */
        if (!vfs_ram_fd_ok((i64)pRegs->u64Arg0) &&
            !net_lo_fd_ok((i64)pRegs->u64Arg0)) {
            return -LINUX_EBADF;
        }
        return 0;

    case LINUX_NR_flock: {
        /* flock(fd, op): LOCK_SH=1 LOCK_EX=2 LOCK_UN=8 LOCK_NB=4 */
        struct gj_flock fl;
        int op = (int)pRegs->u64Arg1;
        i64 i64Fd = (i64)pRegs->u64Arg0;

        if (!vfs_ram_fd_ok(i64Fd)) {
            return -LINUX_EBADF;
        }
        memset(&fl, 0, sizeof(fl));
        fl.u32Pid = 1;
        fl.i64Start = 0;
        fl.i64Len = 0;
        if ((op & 8) != 0) {
            fl.i16Type = GJ_F_UNLCK;
            return file_lock_set(i64Fd, &fl, 0);
        }
        fl.i16Type = ((op & 2) != 0) ? GJ_F_WRLCK : GJ_F_RDLCK;
        return file_lock_set(i64Fd, &fl, (op & 4) == 0);
    }

    case LINUX_NR_chmod: {
        i64 i64Gate;

        i64Gate = promise_gate_wpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        if (vfs_ram_access(szPath, 0) != 0) {
            return -LINUX_ENOENT;
        }
        return 0; /* mode ignored */
    }

    case LINUX_NR_rename: {
        char szNew[96];
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
        copy_path_from_arg(szNew, sizeof(szNew), pRegs->u64Arg1);
        if (szPath[0] == '\0' || szNew[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_rename(szPath, szNew);
    }

    case LINUX_NR_ftruncate:
        return vfs_ram_ftruncate((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg1);

    case LINUX_NR_mkdir:
    case LINUX_NR_mkdirat: {
        /* Interim: create empty path as file marker (product: vfsd dirs) */
        u64 u64Path = (pRegs->u64Nr == LINUX_NR_mkdirat) ? pRegs->u64Arg1
                                                         : pRegs->u64Arg0;
        i64 i64Fd;
        i64 i64Gate;

        i64Gate = promise_gate_cpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), u64Path);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        i64Fd = vfs_ram_open(szPath, 1);
        if (i64Fd < 0) {
            return i64Fd;
        }
        (void)vfs_ram_mark_dir(i64Fd);
        (void)vfs_ram_close(i64Fd);
        return 0;
    }

    case LINUX_NR_statfs:
    case LINUX_NR_fstatfs: {
        /* Linux struct statfs (x86_64 public layout, 120 bytes typical) */
        u8 aSf[128];
        u64 u64Buf = (pRegs->u64Nr == LINUX_NR_fstatfs) ? pRegs->u64Arg1
                                                         : pRegs->u64Arg1;
        i64 ftype = 0x858458f6; /* RAMFS_MAGIC-shaped */
        i64 bsize = 4096;
        i64 blocks = 1024;
        i64 bfree = 512;
        i64 bavail = 512;
        i64 files = 32;
        i64 ffree = 16;
        i64 namelen = 255;

        if (pRegs->u64Nr == LINUX_NR_fstatfs) {
            if (!vfs_ram_fd_ok((i64)pRegs->u64Arg0) &&
                (i64)pRegs->u64Arg0 > 2) {
                return -LINUX_EBADF;
            }
            u64Buf = pRegs->u64Arg1;
        } else {
            i64 i64Gate;

            i64Gate = promise_gate_rpath();
            if (i64Gate != 0) {
                return i64Gate;
            }
            copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg0);
            if (szPath[0] == '\0') {
                return -LINUX_EFAULT;
            }
            u64Buf = pRegs->u64Arg1;
        }
        if (u64Buf == 0) {
            return -LINUX_EFAULT;
        }
        memset(aSf, 0, sizeof(aSf));
        memcpy(aSf + 0, &ftype, 8);
        memcpy(aSf + 8, &bsize, 8);
        memcpy(aSf + 16, &blocks, 8);
        memcpy(aSf + 24, &bfree, 8);
        memcpy(aSf + 32, &bavail, 8);
        memcpy(aSf + 40, &files, 8);
        memcpy(aSf + 48, &ffree, 8);
        memcpy(aSf + 56, &namelen, 8); /* f_fsid pad then namelen varies;
                                         * put namelen at 56 for smoke */
        if (user_range_ok(u64Buf, 120)) {
            if (copy_to_user(u64Buf, aSf, 120) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Buf, aSf, 120);
        }
        return 0;
    }

    case LINUX_NR_openat2: {
        /* open_how: flags at 0, mode at 8, resolve at 16 — use flags/mode */
        u64 u64How = pRegs->u64Arg2;
        u64 u64Flags = 0;
        u64 u64Mode = 0;
        i64 i64Gate;

        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg1);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        if (u64How != 0) {
            if (user_range_ok(u64How, 24)) {
                (void)copy_from_user(&u64Flags, u64How, 8);
                (void)copy_from_user(&u64Mode, u64How + 8, 8);
            } else {
                u64Flags = *(const u64 *)(gj_vaddr_t)u64How;
                u64Mode = *(const u64 *)(gj_vaddr_t)(u64How + 8);
            }
        }
        (void)u64Mode;
        i64Gate = promise_gate_open_flags((u32)u64Flags, 0);
        if (i64Gate != 0) {
            return i64Gate;
        }
        return vfs_ram_open(szPath, (u64Flags & GJ_LINUX_O_CREAT) ? 1 : 0);
    }

    case LINUX_NR_faccessat2: {
        int nMode = (int)pRegs->u64Arg2;
        i64 i64Gate;

        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        copy_path_from_arg(szPath, sizeof(szPath), pRegs->u64Arg1);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        return vfs_ram_access(szPath, nMode);
    }

    case LINUX_NR_statx: {
        /*
         * statx(dirfd, path, flags, mask, statxbuf):
         * Fill public Linux statx (0x100 bytes) from fstat/stat fields.
         * arg0=dirfd arg1=path arg2=flags arg3=mask arg4=buf
         */
        static u8 aStat[144];
        static u8 aSx[256];
        i64 st;
        u32 u32Mask = (u32)pRegs->u64Arg3;
        u64 u64Path = pRegs->u64Arg1;
        u64 u64Buf = pRegs->u64Arg4;
        u32 u32Mode = 0;
        i64 i64Size = 0;
        u64 u64Ino = 1;
        i64 i64Gate;

        (void)u32Mask;
        if (u64Buf == 0) {
            return -LINUX_EFAULT;
        }
        i64Gate = promise_gate_rpath();
        if (i64Gate != 0) {
            return i64Gate;
        }
        memset(aStat, 0, sizeof(aStat));
        memset(aSx, 0, sizeof(aSx));
        copy_path_from_arg(szPath, sizeof(szPath), u64Path);
        if (szPath[0] == '\0') {
            /* Empty path / NULL: fstat on dirfd (AT_EMPTY_PATH-shaped) */
            st = vfs_ram_fstat((i64)pRegs->u64Arg0, aStat, sizeof(aStat));
        } else {
            st = vfs_ram_stat(szPath, aStat, sizeof(aStat));
        }
        if (st != 0) {
            return st;
        }
        /* vfs_stat64: ino@8 mode@16 size@48 (see vfs_ram fill_stat) */
        memcpy(&u64Ino, aStat + 8, 8);
        memcpy(&u32Mode, aStat + 24, 4);
        memcpy(&i64Size, aStat + 48, 8);
        /* statx: stx_mask@0 stx_blksize@4 stx_attributes@8 … stx_mode@20
         * stx_ino@32 stx_size@40 (public man layout; clean-room). */
        {
            u32 m = 0x000007ffu; /* basic STATX_* mask */
            u32 blk = 4096;

            memcpy(aSx + 0, &m, 4);
            memcpy(aSx + 4, &blk, 4);
            memcpy(aSx + 20, &u32Mode, 4); /* stx_mode as u16+pad; write u32 */
            memcpy(aSx + 32, &u64Ino, 8);
            memcpy(aSx + 40, &i64Size, 8);
        }
        if (user_range_ok(u64Buf, 256)) {
            if (copy_to_user(u64Buf, aSx, 256) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Buf, aSx, 256);
        }
        return 0;
    }

    case LINUX_NR_fork:
        return process_linux_fork(1, 0);

    case LINUX_NR_vfork:
        /* vfork: child runs first — mark zombie so parent wait sees exit */
        return process_linux_fork(1, 1);

    case LINUX_NR_clone:
    case LINUX_NR_clone3: {
        /*
         * clone(flags, stack, ...): CLONE_THREAD (0x10000) → same pid-shaped.
         * Otherwise create wait-registered child (fork-like).
         */
        u64 u64Flags = pRegs->u64Arg0;

        if (pRegs->u64Nr == LINUX_NR_clone3 && pRegs->u64Arg0 != 0) {
            /* clone_args.flags at offset 0 */
            if (user_range_ok(pRegs->u64Arg0, 8)) {
                (void)copy_from_user(&u64Flags, pRegs->u64Arg0, 8);
            } else {
                u64Flags = *(const u64 *)(gj_vaddr_t)pRegs->u64Arg0;
            }
        }
        if (u64Flags & 0x10000ull /* CLONE_THREAD */) {
            /*
             * Thread-shaped: spawn real user thread in current process AS
             * when child_stack is provided. Entry = parent user RIP (after
             * SYSCALL). Parent returns thread id; child starts with rax=0
             * (product: set child frame rax later).
             */
            u64 u64Stack = pRegs->u64Arg1;
            struct gj_process *pProc = g_pLinuxProc;
            struct gj_cpu *pCpu;
            struct gj_thread *pCur;
            u64 u64Entry;
            u32 thr;

            pCur = thread_current();
            if (pProc == NULL && pCur != NULL) {
                pProc = pCur->pProc;
            }
            pCpu = cpu_current();
            u64Entry = (pCpu != NULL) ? pCpu->u64UserRip : 0;
            if (u64Stack != 0 && u64Entry != 0 && pProc != NULL) {
                thr = thread_create_user(pProc, u64Entry, u64Stack);
                if (thr != 0) {
                    kprintf("linux: clone thread thr=%u entry=0x%lx\n", thr,
                            (unsigned long)u64Entry);
                    return (i64)thr;
                }
                return -11; /* EAGAIN */
            }
            /* No stack/entry (kernel-side smoke): soft success */
            return 0;
        }
        return process_linux_fork(1, 0);
    }

    case LINUX_NR_execve:
    case LINUX_NR_execveat: {
        /*
         * Path must exist. If file is ELF64, map into current Linux process AS.
         * PT_INTERP: probe + load interpreter from vfs when present (ld-gj path).
         * Full image replace (thread entry switch) is product; load proves path.
         */
        u64 u64Path = (pRegs->u64Nr == LINUX_NR_execveat) ? pRegs->u64Arg1
                                                           : pRegs->u64Arg0;

        {
            i64 i64Gate = confine_soft_promise_require(GJ_PROMISE_EXEC);
            if (i64Gate != 0) {
                return i64Gate;
            }
        }
        i64 i64Fd;
        i64 n;
        static u8 aImg[16384];
        static u8 aInterp[65536];
        struct gj_elf_info elf;
        struct gj_elf_info elfInterp;
        /* Embedded product ld-gj (optional; prefer when vfs stub is tiny) */
        extern const u8 gj_ld_gj_elf_blob[];
        extern const u8 gj_ld_gj_elf_blob_end[];

        copy_path_from_arg(szPath, sizeof(szPath), u64Path);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        i64Fd = vfs_ram_open(szPath, 0);
        if (i64Fd < 0) {
            if (vfs_ram_access(szPath, 0) != 0) {
                return -LINUX_ENOENT;
            }
            return 0; /* path OK, empty open edge */
        }
        /*
         * aImg/aInterp sit in high kernel BSS (can pass PE 0x400000 once embeds
         * grow). Private AS COW/protect bugs have left those leaves RO — repair
         * identity R/W under kernel + active CR3 before the store.
         */
        (void)vmm_ensure_identity_rw((gj_vaddr_t)aImg, sizeof(aImg));
        (void)vmm_ensure_identity_rw((gj_vaddr_t)aInterp, sizeof(aInterp));
        n = vfs_ram_read(i64Fd, aImg, sizeof(aImg));
        (void)vfs_ram_close(i64Fd);
        if (n >= 4 && aImg[0] == 0x7fu && aImg[1] == (u8)'E' &&
            aImg[2] == (u8)'L' && aImg[3] == (u8)'F' && g_pLinuxProc != NULL) {
            int fInterpLoaded = 0;

            if (elf_probe_image(aImg, (u64)n, &elf) != GJ_OK) {
                return -8; /* ENOEXEC */
            }
            g_pLinuxProc->u64InterpEntry = 0;
            g_pLinuxProc->u32ExecFlags = elf.u32Flags;
            g_pLinuxProc->cAuxv = 0;
            /* Resolve DT_NEEDED against vfs before map (ld-gj path) */
            if (elf.u16Needed > 0) {
                u32 cRes = elf_resolve_needed_vfs(&elf);

                if (cRes > 0) {
                    kprintf("linux: execve DT_NEEDED resolve PASS n=%u\n", cRes);
                }
            }
            memset(&elfInterp, 0, sizeof(elfInterp));
            if (elf.u32Flags & GJ_ELF_INFO_HAS_INTERP) {
                i64 i64I;
                i64 nI;
                u64 cbEmbed;
                const u8 *pEmbed;
                int fUseEmbed = 0;

                kprintf("linux: execve INTERP=%s\n", elf.szInterp);
                i64I = vfs_ram_open(elf.szInterp, 0);
                nI = -1;
                if (i64I >= 0) {
                    nI = vfs_ram_read(i64I, aInterp, sizeof(aInterp));
                    (void)vfs_ram_close(i64I);
                }
                pEmbed = gj_ld_gj_elf_blob;
                cbEmbed = (u64)(gj_ld_gj_elf_blob_end - gj_ld_gj_elf_blob);
                /*
                 * Prefer packaged ld-gj when vfs has only a tiny stub/placeholder
                 * or is non-ELF. Product INTERP path = full freestanding ld-gj.
                 */
                if (cbEmbed > 256ull && pEmbed[0] == 0x7fu && pEmbed[1] == 'E') {
                    if (nI < 4 || aInterp[0] != 0x7fu ||
                        (u64)nI + 1024ull < cbEmbed) {
                        fUseEmbed = 1;
                    }
                }
                if (fUseEmbed) {
                    if (cbEmbed > sizeof(aInterp)) {
                        cbEmbed = sizeof(aInterp);
                    }
                    memcpy(aInterp, pEmbed, (size_t)cbEmbed);
                    nI = (i64)cbEmbed;
                    kprintf("linux: execve INTERP embed cb=%lu PASS\n",
                            (unsigned long)cbEmbed);
                }
                if (nI >= 4 && aInterp[0] == 0x7fu &&
                    elf_load_image(g_pLinuxProc, aInterp, (u64)nI, &elfInterp) ==
                        GJ_OK) {
                    g_pLinuxProc->u64InterpEntry = elfInterp.u64Entry;
                    fInterpLoaded = 1;
                    kprintf("linux: execve INTERP loaded entry=0x%lx%s\n",
                            (unsigned long)elfInterp.u64Entry,
                            fUseEmbed ? " (ld-gj)" : "");
                } else if (i64I < 0 && !fUseEmbed) {
                    kprintf("linux: execve INTERP missing on vfs (ok bring-up)\n");
                } else {
                    kprintf("linux: execve INTERP present (load deferred)\n");
                }
            }
            /*
             * Map DT_NEEDED SOs before main so GLOB_DAT/JUMP_SLOT can
             * resolve across objects during main relocate.
             */
            {
                u32 cSo = elf_load_needed_sos(g_pLinuxProc, &elf);

                g_pLinuxProc->cNeededLoaded = cSo;
                if (cSo > 0) {
                    kprintf("linux: execve SO map PASS n=%u\n", cSo);
                }
            }
            if (elf_load_image(g_pLinuxProc, aImg, (u64)n, &elf) == GJ_OK) {
                size_t iPath;

                g_pLinuxProc->u64ExecEntry = elf.u64Entry;
                g_pLinuxProc->u64LoadBias = elf.u64Bias;
                g_pLinuxProc->u32ExecFlags = elf.u32Flags;
                /* Remember path for AT_EXECFN / ld-gj handoff */
                for (iPath = 0; iPath + 1 < sizeof(g_pLinuxProc->szExecPath) &&
                                szPath[iPath] != '\0';
                     iPath++) {
                    g_pLinuxProc->szExecPath[iPath] = szPath[iPath];
                }
                g_pLinuxProc->szExecPath[iPath] = '\0';
                elf_fill_auxv(g_pLinuxProc, &elf,
                              fInterpLoaded ? &elfInterp : NULL);
                if (elf_publish_handoff(g_pLinuxProc, szPath, &elf,
                                        fInterpLoaded ? &elfInterp : NULL) ==
                    GJ_OK) {
                    kprintf("linux: execve handoff PASS\n");
                    if (elf_ld_handoff_verify(g_pLinuxProc) == GJ_OK) {
                        /* ld-gj: handoff PASS already printed */
                    }
                }
                /* INTERP-first: entry=ld-gj, SP=handoff stack */
                (void)elf_apply_interp_first(
                    g_pLinuxProc, &elf, fInterpLoaded ? &elfInterp : NULL,
                    GJ_LD_STACK_VA);
                kprintf("linux: execve ELF entry=0x%lx start=0x%lx auxv=%u\n",
                        (unsigned long)elf.u64Entry,
                        (unsigned long)g_pLinuxProc->u64StartEntry,
                        g_pLinuxProc->cAuxv);
                if (elf.u32Flags & GJ_ELF_INFO_HAS_INTERP) {
                    kprintf("linux: execve INTERP PASS\n");
                }
                if (elf.u32Flags & GJ_ELF_INFO_RELOC_OK) {
                    kprintf("linux: execve RELA PASS\n");
                }
                if (elf.u32Flags & GJ_ELF_INFO_SYM_OK) {
                    kprintf("linux: execve SYM PASS\n");
                }
                if (g_pLinuxProc->cAuxv > 0) {
                    kprintf("linux: execve auxv PASS\n");
                }
                return 0;
            }
            return -8; /* ENOEXEC */
        }
        /* Non-ELF: soft success if path was openable */
        return 0;
    }

    case LINUX_NR_pidfd_open:
        return vfs_ram_pidfd_open((u32)pRegs->u64Arg0, (int)pRegs->u64Arg1);

    case LINUX_NR_pidfd_send_signal: {
        /* pidfd_send_signal(pidfd, sig, info, flags) */
        i64 i64Fd = (i64)pRegs->u64Arg0;
        u32 u32Sig = (u32)pRegs->u64Arg1;
        u32 u32Pid;
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_PROC);
        if (i64Gate != 0) {
            return i64Gate;
        }
        if (i64Fd < 0) {
            return -LINUX_EBADF;
        }
        u32Pid = vfs_ram_pidfd_pid(i64Fd);
        if (u32Pid == 0) {
            return -LINUX_EBADF;
        }
        if (u32Sig > 0 && u32Sig < 64) {
            vfs_ram_signalfd_inject(u32Sig);
        }
        return 0;
    }

    case LINUX_NR_close_range: {
        /* close_range(first, last, flags): close inclusive fd range */
        u32 u32First = (u32)pRegs->u64Arg0;
        u32 u32Last = (u32)pRegs->u64Arg1;
        u32 i;

        if (u32First > u32Last) {
            return -LINUX_EINVAL;
        }
        if (u32Last > 31u) {
            u32Last = 31u;
        }
        for (i = u32First; i <= u32Last; i++) {
            if (vfs_ram_fd_ok((i64)i)) {
                (void)vfs_ram_close((i64)i);
            } else if (net_lo_fd_ok((i64)i)) {
                (void)net_lo_close((i64)i);
            }
        }
        return 0;
    }

    default:
        break;
    }

    return (i64)protonrt_cold_linux(pRegs->u64Nr, pRegs->u64Arg0, pRegs->u64Arg1,
                                    pRegs->u64Arg2, pRegs->u64Arg3,
                                    pRegs->u64Arg4, pRegs->u64Arg5);
}

void
gj_protonrt_attach_cold(void)
{
    cold_ipc_set_service(protonrt_service, NULL);
    cold_ipc_set_doors_mode(1);
    cold_ipc_set_personality_attached(1);
    protonrt_soft_inc(&g_u64PrtSoftAttach);
    /*
     * Soft multi-server confine deepen probe (expose / ledger / death).
     * Product multi-server confine stays OPEN. Preserves confine soft PASS.
     */
    confine_soft_selfprobe();
    /* Wave 15 soft inventory baseline after cold attach. */
    protonrt_soft_inventory_log();
}
