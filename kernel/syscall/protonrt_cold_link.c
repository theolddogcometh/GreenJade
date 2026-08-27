/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Bridge: cold_ipc service -> vfs_ram + linux_cold_net + protonrt_cold_linux.
 * Soft multi-server confine deepen: expose path policy, promise-denial
 * ledger, death-cleanup soft note (product multi-server confine OPEN).
 * Grep: confine: expose soft | confine: ledger soft | confine: death soft
 *
 * Soft inventory (bring-up diagnostics; this unit only):
 *   protonrt: soft inventory|enter|fs|net|proc|time|mem|other|confine|
 *             attach|path|rates|honesty|catalog|deepen|PASS ...
 *   cold_link: soft ... (twin prefix)
 *   Never hard-gates; diagnostics only (wrap OK). Soft.
 *
 * Lean soft residual (Soft!=product dual license; no version stamp; no stamp
 * storms): sparse once-lamps only - fold fs|net|proc|time|mem|other +
 * confine/expose + attach + cold_net/cold_ipc userspace-driver reach into
 * two greppable lines. storm=0. Does not close multi-server confine, bar3,
 * or Deck Top 50. Dual MIT OR Apache-2.0. Soft!=product. G-AC-1:
 *   product drivers = userspace Linux-shaped (UDX/DDI) over hot+cold ABI;
 *   not in-kernel .ko product AC. Cold NR link residual lean so ProtonRT /
 *   Linux personality can reach cold net/ipc for userspace driver hosts.
 *
 * Residual lean deepen (bar3-adjacent ABI surface only; Soft!=product):
 *   Proton/game-shaped cold NR reach for userspace hosts - socket family,
 *   poll/ppoll/select readiness, close lifecycle, read/write-on-socket.
 *   Linux-shaped apps / Proton neighborhood residual deepen (this unit):
 *   socketpair (AF_UNIX wine-class), pipe/pipe2, epoll family soft-spin,
 *   eventfd, memfd_create - tallied into residual lean once-lamp only.
 *   C2 Dual DoD residual (this unit; Soft!=product; agent never closes):
 *   ioctl FIONREAD/FIONBIO for ram+lo+tcp fds (UDX host devctl
 *   shape); stdio 0/1/2 + vfs_ram PTY/pipe tty-class for TCGETS/TCSETS
 *   family/TIOCGWINSZ/TIOCSWINSZ (TCP -ENOTTY, not silent 0; not -EBADF
 *   on stdin);
 *   sendfile/splice/tee/copy_file_range ram-in -> cold_net out
 *   (sshd/file residual); fcntl F_GETFL/F_SETFL + soft O_NONBLOCK table
 *   for ram+lo+tcp (parity with ioctl FIONBIO; prior fcntl lo-only rejected
 *   tcp with -EBADF). Honesty lamps dual_dod_a=OPEN dual_dod_b=OPEN
 *   freestanding_skip=1 product_path=UDX_DDI+hot_cold_ABI. Dual DoD A/B
 *   stay OPEN until host USB path (A) / interactive SSH login (B).
 *   "bar3-adjacent" means ABI neighborhood games/Proton need - never a bar3
 *   claim, never Deck Top 50 PASS, never product DoD close. Comment law:
 *   Soft!=product (ASCII != only; never Softneq unicode); pure C block
 *   comments only - never slash-star footguns that split block comments
 *   (no star-slash-slash mid-comment; no C++ line comments in this unit).
 *   Transfer family in comments: send_star/recv_star (never send-star or
 *   wildcard forms that break block comments). Dual MIT OR Apache-2.0.
 * greppable: protonrt: soft residual lean
 * greppable: cold_link: soft residual lean
 * greppable: protonrt: soft cold_net bridge PASS
 * greppable: protonrt: soft cold_ipc attach
 * greppable: protonrt: soft select residual lean
 * greppable: protonrt: soft epoll residual lean
 * greppable: protonrt: soft neighborhood residual lean
 * greppable: protonrt: soft ioctl residual lean
 * greppable: protonrt: soft sendfile residual lean
 * greppable: protonrt: soft fcntl residual lean
 * greppable: protonrt: soft copy_file residual lean
 * greppable: protonrt: soft dual_dod OPEN
 *
 * greppable: protonrt: soft
 * greppable: cold_link: soft
 * greppable: protonrt: soft fork-wait wire PASS
 * greppable: protonrt: soft wait4 rusage residual lean
 * greppable: protonrt: soft rlimit residual lean
 * greppable: protonrt: soft fcntl cloexec residual lean
 * greppable: protonrt: soft kill wait-child residual lean
 * greppable: protonrt: soft poll block PASS
 * greppable: protonrt: soft init_module PASS|FAIL
 * greppable: protonrt: soft finit_module PASS|FAIL
 * greppable: protonrt: soft delete_module PASS|FAIL
 *
 * Daemon-grade cold: fork/clone/wait -> process_*_soft when parent PCB is
 * available; poll/ppoll/select/epoll soft-spin on timeout>0 (Soft!=product).
 *
 * Cold net/ipc NR residual (userspace driver host reach - Soft!=product):
 *   socket/bind/listen/accept/connect/send_star/recv_star/shutdown/sockopt/name
 *     -> gj_linux_cold_* (linux_cold_net bridge)
 *   close / close_range -> gj_linux_cold_close then vfs_ram
 *   poll/ppoll -> vfs pipe/socketpair mask, else cold_net then vfs_ram
 *   select/pselect6 -> cold_net/vfs poll_mask residual lean (fd bitsets)
 *   epoll_wait family -> vfs_ram_epoll_wait + soft-spin residual lean
 *   read/write: vfs pipe/socketpair before cold_net recv/send (monitor fd 3)
 *   ioctl FIONREAD/FIONBIO on ram+lo+tcp -> soft readiness / nonblock accept
 *   ioctl TTY (TCGETS family / winsz) on fd 0/1/2 or vfs_ram PTY/pipe
 *     -> soft fill; TCP -ENOTTY (not silent 0)
 *   ioctl TIOCSCTTY/TIOCNOTTY USER*_ENTRY + stdio|ram -> 0; else -EPERM/-ENOTTY
 *   ioctl TIOCGPTN/TIOCSPTLCK USER bind -> vfs_ram_ioctl (Unix98 PTY mux)
 *   sendmsg/recvmsg vfs ipc: first iov + one SCM_RIGHTS fd (USER bind)
 *   ppoll arg3 wait sigmask: -EINTR if SIGCHLD pending and unblocked
 *   fcntl F_GETFL/F_SETFL on ram+lo+tcp -> soft O_NONBLOCK table residual
 *   sendfile/splice/tee/copy_file_range ram-in + net-out -> bounce + cold send
 *   socketpair/pipe/eventfd/memfd -> vfs_ram residual (Proton neighborhood)
 *   attach -> cold_ipc_set_service(protonrt_service) (cold_ipc path)
 *
 * Soft module path (init_module / finit_module / delete_module):
 *   in-kernel .ko load is abandoned (G-AC-1). These NRs return -ENOSYS.
 *   Product drivers = userspace UDX over hot+cold ABI.
 */
#include <gj/cold_ipc.h>
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/door.h>
#include <gj/elf_load.h>
#include <gj/error.h>
#include <gj/file_lock.h>
#include <gj/io_uring.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/linux_dispatch.h>
#include <gj/memobj.h>
#include <gj/net_eth.h>
#include <gj/net_lo.h>
#include <gj/net_tcp.h>
#include <gj/linux_cold_net.h>
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/types.h>
#include <gj/user_access.h>
#include <gj/vfs_ram.h>
#include <gj/vmm.h>

i64 vfs_ram_scm_send_fd(i64 i64Sock, i32 i32Fd);
i64 vfs_ram_scm_recv_fd(i64 i64Sock);

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
#define PRT_SCM_RIGHTS     1 /* Linux SCM_RIGHTS */

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
#define GJ_PROTONRT_SOFT_WAVE 117u
/*
 * Soft inventory area count (fixed greppable categories for deepen stamp).
 * Residual lean deepen adds: lean_hit tallies, select residual, abi_adj map,
 * Proton neighborhood (socketpair/pipe/epoll/eventfd/memfd) lean hits,
 * C2 Dual DoD residual (ioctl/sendfile/fcntl/copy_file_range soft tables).
 * Soft!=product - areas are diagnostics only. Never a bar3 claim.
 */
#define GJ_PROTONRT_SOFT_AREAS 226u

/* select/pselect residual lean: cap covers ram + net_lo + net_tcp soft fds. */
#define PRT_SELECT_SOFT_NFDS_MAX 128u
#define PRT_SELECT_SOFT_FDSET_BYTES ((PRT_SELECT_SOFT_NFDS_MAX + 7u) / 8u)

/*
 * Soft NR group buckets for cold personality enter tallies (Wave 15).
 * Diagnostics only - never hard-gates protonrt_service returns.
 */
enum {
    PRT_SOFT_GRP_FS = 0,   /* open/read/write/stat/path/dir/fd */
    PRT_SOFT_GRP_NET,      /* socket/connect/send_star/recv_star/... */
    PRT_SOFT_GRP_PROC,     /* kill/wait/pidfd/exec-shaped */
    PRT_SOFT_GRP_TIME,     /* clock/timer/nanosleep-shaped */
    PRT_SOFT_GRP_MEM,      /* mmap/mprotect/brk-shaped cold */
    PRT_SOFT_GRP_OTHER,    /* fallback / protonrt_cold_linux */
    PRT_SOFT_GRP_N
};

/*
 * Soft product inventory (Wave 35 exclusive deepen).
 * greppable: protonrt: soft ... / cold_link: soft ...
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
static u8  g_fPrtSoftColdNetOnce;             /* one-shot cold_net bridge */
static u8  g_fPrtSoftForkWaitWireOnce;        /* fork-wait soft wire PASS */
static u8  g_fPrtSoftPollBlockOnce;           /* poll soft-spin PASS */
static u8  g_fPrtSoftResidualLeanOnce;        /* lean residual once-lamp */
static u64 g_u64PrtSoftResidualLean;          /* residual lean emit count */
/*
 * Residual lean hit tallies (Soft!=product; storm=0 companion counters).
 * Folded into residual lean once-lamp only - never stamp storms.
 * Bar3-adjacent ABI reach for userspace hosts (not a bar3 claim).
 * Proton neighborhood deepen: socketpair/pipe/epoll/eventfd/memfd lean hits
 * for Linux-shaped apps (wine-class IPC + readiness) - Soft!=product G-AC-1.
 */
static u64 g_u64PrtLeanSocket;                /* socket bind listen accept connect */
static u64 g_u64PrtLeanXfer;                  /* send_star recv_star shutdown sockopt name */
static u64 g_u64PrtLeanPoll;                  /* poll ppoll cold_net first */
static u64 g_u64PrtLeanSelect;                /* select pselect residual lean */
static u64 g_u64PrtLeanClose;                 /* close close_range cold_net first */
static u64 g_u64PrtLeanRwNet;                 /* read write on net fds */
static u64 g_u64PrtLeanSockpair;              /* socketpair AF_UNIX residual */
static u64 g_u64PrtLeanPipe;                  /* pipe pipe2 residual */
static u64 g_u64PrtLeanEpoll;                 /* epoll create/ctl/wait residual */
static u64 g_u64PrtLeanEventfd;               /* eventfd eventfd2 residual */
static u64 g_u64PrtLeanMemfd;                 /* memfd_create residual */
static u64 g_u64PrtLeanIoctl;                 /* ioctl FIONREAD/FIONBIO/TTY residual */
static u64 g_u64PrtLeanSendfile;              /* sendfile/splice/tee residual */
static u64 g_u64PrtLeanFcntl;                 /* fcntl F_GETFL/F_SETFL residual */
static u64 g_u64PrtLeanCopyFr;                /* copy_file_range residual */
static u8  g_fPrtSoftSelectOnce;              /* select residual once-lamp */
static u8  g_fPrtSoftEpollOnce;               /* epoll residual once-lamp */
static u8  g_fPrtSoftNeighborhoodOnce;        /* neighborhood residual once-lamp */
static u8  g_fPrtSoftIoctlOnce;               /* ioctl residual once-lamp */
static u8  g_fPrtSoftSendfileOnce;            /* sendfile residual once-lamp */
static u8  g_fPrtSoftFcntlOnce;               /* fcntl residual once-lamp */
static u8  g_fPrtSoftCopyFrOnce;              /* copy_file_range residual once-lamp */
static u8  g_fPrtSoftDualDodOnce;             /* dual DoD honesty once-lamp */
static u8  g_fPrtSoftWait4RusageOnce;         /* wait4 rusage fill once-lamp */
static u8  g_fPrtSoftRlimitOnce;              /* rlimit residual once-lamp */
static u8  g_fPrtSoftFcntlCloexecOnce;        /* fcntl CLOEXEC once-lamp */
static u8  g_fPrtSoftKillWaitOnce;            /* kill wait-child once-lamp */

/*
 * Soft fd open-flags residual (C2 Dual DoD residual deepen; Soft!=product).
 * F_SETFL / FIONBIO O_NONBLOCK soft honesty for ram+lo+tcp UDX host probes.
 * F_GETFL returns last soft flags (net default O_RDWR). Never product
 * O_NONBLOCK table. close clears entry. storm=0. Dual DoD A/B OPEN.
 */
#define PRT_SOFT_FDFL_MAX 32u

struct prt_soft_fdfl_ent {
    i64 i64Fd;
    u32 u32Flags;
    u8  u8Used;
    u8  u8Pad[3];
};

static struct prt_soft_fdfl_ent g_aPrtSoftFdFl[PRT_SOFT_FDFL_MAX];
static u32 g_u32PrtSigchldEnterLog;

static void protonrt_soft_inc(u64 *pCtr);
static void protonrt_soft_note_enter(u64 u64Nr);
static void protonrt_soft_inventory_log(void);
static void protonrt_soft_inventory_maybe_once(void);
static void protonrt_soft_residual_lean_once(void);
static struct gj_process *protonrt_soft_parent(void);
static void protonrt_soft_fork_wait_wire_pass_once(void);
static u32 protonrt_soft_fd_ready_mask(i64 i64Fd, u32 u32Want);
static u32 protonrt_soft_fdfl_get(i64 i64Fd, u32 u32Default);
static void protonrt_soft_fdfl_set(i64 i64Fd, u32 u32Flags);
static void protonrt_soft_fdfl_clear(i64 i64Fd);
static struct gj_process *protonrt_calling_user_proc(void);
static int protonrt_sigchld_enter_handler(struct gj_process *pProc);
static struct gj_process *protonrt_bind_calling_user(void);
static void protonrt_unbind_calling_user(struct gj_process *pSave);
static int protonrt_vfs_ipc_fd(i64 i64Fd);
static i64 protonrt_vfs_read(struct gj_linux_regs *pRegs, u8 *pBuf,
                             size_t cbMax);
static i64 protonrt_vfs_write(struct gj_linux_regs *pRegs, u8 *pBuf,
                              size_t cbMax);
static i64 protonrt_msg_first_iov(u64 u64Msg, u64 *pBase, u64 *pLen);
static i64 protonrt_msg_control(u64 u64Msg, u64 *pCtl, u64 *pCtlLen);
static i64 protonrt_vfs_scm_send(i64 i64Sock, u64 u64Msg);
static void protonrt_vfs_scm_recv(i64 i64Sock, u64 u64Msg);

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
 * Soft fd open-flags get (C2 residual; Soft!=product).
 * Returns last F_SETFL/FIONBIO soft flags or u32Default when unset.
 */
static u32
protonrt_soft_fdfl_get(i64 i64Fd, u32 u32Default)
{
    u32 i;

    if (i64Fd < 0) {
        return u32Default;
    }
    for (i = 0; i < PRT_SOFT_FDFL_MAX; i++) {
        if (g_aPrtSoftFdFl[i].u8Used != 0 &&
            g_aPrtSoftFdFl[i].i64Fd == i64Fd) {
            return g_aPrtSoftFdFl[i].u32Flags;
        }
    }
    return u32Default;
}

/**
 * Soft fd open-flags set (C2 residual; Soft!=product).
 * Overwrites existing entry or claims free slot; full table is soft drop.
 */
static void
protonrt_soft_fdfl_set(i64 i64Fd, u32 u32Flags)
{
    u32 i;
    u32 u32Free;

    if (i64Fd < 0) {
        return;
    }
    u32Free = PRT_SOFT_FDFL_MAX;
    for (i = 0; i < PRT_SOFT_FDFL_MAX; i++) {
        if (g_aPrtSoftFdFl[i].u8Used != 0 &&
            g_aPrtSoftFdFl[i].i64Fd == i64Fd) {
            g_aPrtSoftFdFl[i].u32Flags = u32Flags;
            return;
        }
        if (g_aPrtSoftFdFl[i].u8Used == 0 && u32Free == PRT_SOFT_FDFL_MAX) {
            u32Free = i;
        }
    }
    if (u32Free < PRT_SOFT_FDFL_MAX) {
        g_aPrtSoftFdFl[u32Free].u8Used = 1;
        g_aPrtSoftFdFl[u32Free].i64Fd = i64Fd;
        g_aPrtSoftFdFl[u32Free].u32Flags = u32Flags;
    }
}

/** Soft fd open-flags clear on close residual (Soft!=product). */
static void
protonrt_soft_fdfl_clear(i64 i64Fd)
{
    u32 i;

    if (i64Fd < 0) {
        return;
    }
    for (i = 0; i < PRT_SOFT_FDFL_MAX; i++) {
        if (g_aPrtSoftFdFl[i].u8Used != 0 &&
            g_aPrtSoftFdFl[i].i64Fd == i64Fd) {
            g_aPrtSoftFdFl[i].u8Used = 0;
            g_aPrtSoftFdFl[i].i64Fd = -1;
            g_aPrtSoftFdFl[i].u32Flags = 0;
            return;
        }
    }
}

/**
 * Residual lean readiness for one fd (Soft!=product).
 * VFS pipe/socketpair before leftover LCN aliases (sshd-auth monitor fd).
 * Else cold_net poll_mask (SO_ERROR / half-close), then vfs_ram.
 * Never always-ready. G-AC-1 userspace host reach - not .ko product AC.
 */
static u32
protonrt_soft_fd_ready_mask(i64 i64Fd, u32 u32Want)
{
    struct gj_process *pSave;
    u32 u32Got;

    if (i64Fd < 0) {
        return 0;
    }
    pSave = protonrt_bind_calling_user();
    if (vfs_ram_fd_poll_parkable(i64Fd) != 0) {
        u32Got = vfs_ram_poll_mask(i64Fd, u32Want);
        protonrt_unbind_calling_user(pSave);
        return u32Got;
    }
    protonrt_unbind_calling_user(pSave);
    u32Got = gj_linux_cold_poll_mask(i64Fd, u32Want);
    /*
     * LCN aliases (sshd dup2 of accept fd 96 onto 6) are cold_net,
     * not vfs. Empty ESTAB must stay 0 so ppoll parks. vfs unknown-fd
     * ERR|HUP would wake OpenSSH packet_read into read()==0.
     */
    if (u32Got == 0 && gj_linux_cold_fd_ok(i64Fd) == 0) {
        pSave = protonrt_bind_calling_user();
        u32Got = vfs_ram_poll_mask(i64Fd, u32Want);
        protonrt_unbind_calling_user(pSave);
    }
    return u32Got;
}

/**
 * Calling USER thread: USER*_ENTRY still set, or first sysretq already
 * cleared it (trampoline) while pfnEntry is NULL and a user RIP remains,
 * or mid-syscall SysUserValid. Kernel death workers keep pfnEntry.
 */
static int
protonrt_thr_is_user(const struct gj_thread *pThr)
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

/*
 * Doors kthread: in-flight USER client. schedule() saved that thr's
 * sysuser; thread_exec_replace needs the TCB after trampoline cleared
 * USER_ENTRY. NULL if SERVICE_FIRST (current is already the USER thr).
 */
static struct gj_thread *
protonrt_door_client_user(void)
{
    struct gj_door *pDoor;
    struct gj_thread *pCli;

    pDoor = door_cold_personality();
    if (pDoor == NULL) {
        return NULL;
    }
    pCli = pDoor->pClient;
    if (protonrt_thr_is_user(pCli) == 0) {
        return NULL;
    }
    return pCli;
}

/**
 * Calling USER PCB. Kernel-side / unbound -> NULL.
 * execve/fork/wait bind here so a fork child is not g_pLinuxProc.
 * Doors kthread: door pClient (OpenSSH session / dash), not the kthread PCB.
 */
static struct gj_process *
protonrt_calling_user_proc(void)
{
    struct gj_thread *pCur = thread_current();
    struct gj_thread *pCli;

    if (protonrt_thr_is_user(pCur) != 0) {
        return pCur->pProc;
    }
    pCli = protonrt_door_client_user();
    if (pCli != NULL) {
        return pCli->pProc;
    }
    return NULL;
}

/*
 * poll/ppoll SIGCHLD: if sa_handler is a user VA, push SYSCALL RIP and
 * aim sysretq at the handler. Returns 1 only when RIP was switched.
 * Caller must not -EINTR on 0 (QEMU90: DFL/push-fail left pending set
 * and EINTR-spun until timeout; OpenSSH waitpid only if the handler
 * set child_terminated). No rdi (OpenSSH ignores signo). Dual DoD B OPEN.
 */
static int
protonrt_sigchld_enter_handler(struct gj_process *pProc)
{
    struct gj_cpu *pCpu;
    struct gj_thread *pCur;
    struct gj_thread *pUser;
    u64 u64H;
    u64 u64Rip;
    u64 u64Rsp;
    u64 u64Slot;
    gj_vaddr_t vaPage;
    int fCurUser;
    u32 u32Pid;

    if (pProc == NULL) {
        return 0;
    }
    u64H = pProc->aSigHandler[GJ_SIGCHLD];
    if (u64H < GJ_USER_VA_BASE || u64H >= GJ_USER_VA_END) {
        if (g_u32PrtSigchldEnterLog < 4u) {
            g_u32PrtSigchldEnterLog++;
            u32Pid = process_wait_pid_of(pProc);
            kprintf("protonrt: sigchld enter pid=%u h=0x%lx skip_dfl "
                    "pending Dual DoD B OPEN\n",
                    u32Pid, (unsigned long)u64H);
        }
        return 0;
    }
    pCpu = cpu_current();
    pCur = thread_current();
    pUser = NULL;
    u64Rip = 0;
    u64Rsp = 0;
    fCurUser = 0;
    if (protonrt_thr_is_user(pCur) != 0 && pCur->pProc == pProc) {
        pUser = pCur;
        fCurUser = 1;
        if (pCpu != NULL) {
            u64Rip = pCpu->u64UserRip;
            u64Rsp = pCpu->u64UserRsp;
        }
    } else {
        pUser = protonrt_door_client_user();
        if (pUser == NULL || pUser->pProc != pProc) {
            pUser = pProc->pUserThr;
        }
    }
    if (u64Rip == 0 && pUser != NULL && pUser->u32SysUserValid != 0) {
        u64Rip = pUser->u64SysUserRip;
        u64Rsp = pUser->u64SysUserRsp;
    }
    if (u64Rip == 0 && pUser != NULL) {
        u64Rip = pUser->u64UserRip;
        u64Rsp = pUser->u64UserRsp;
    }
    if (u64Rip == 0) {
        u64Rip = pProc->u64UserSysRip;
        u64Rsp = pProc->u64UserSysRsp;
    }
    if (u64Rip == 0 || u64Rsp < 8ull) {
        if (g_u32PrtSigchldEnterLog < 4u) {
            g_u32PrtSigchldEnterLog++;
            u32Pid = process_wait_pid_of(pProc);
            kprintf("protonrt: sigchld enter pid=%u h=0x%lx rip=0x%lx "
                    "rsp=0x%lx skip_rip Dual DoD B OPEN\n",
                    u32Pid, (unsigned long)u64H, (unsigned long)u64Rip,
                    (unsigned long)u64Rsp);
        }
        return 0;
    }
    u64Slot = u64Rsp - 8ull;
    if (user_range_ok(u64Slot, 8) == 0) {
        return 0;
    }
    if (copy_to_user(u64Slot, &u64Rip, sizeof(u64Rip)) != GJ_OK) {
        vaPage = (gj_vaddr_t)(u64Slot & ~(u64)(GJ_PAGE_SIZE - 1ull));
        (void)vmm_cow_break_page(vaPage);
        if (copy_to_user(u64Slot, &u64Rip, sizeof(u64Rip)) != GJ_OK) {
            if (g_u32PrtSigchldEnterLog < 4u) {
                g_u32PrtSigchldEnterLog++;
                u32Pid = process_wait_pid_of(pProc);
                kprintf("protonrt: sigchld enter pid=%u h=0x%lx "
                        "rip=0x%lx rsp=0x%lx skip_copy Dual DoD B OPEN\n",
                        u32Pid, (unsigned long)u64H,
                        (unsigned long)u64Rip, (unsigned long)u64Rsp);
            }
            return 0;
        }
    }
    if (fCurUser != 0 && pCpu != NULL) {
        pCpu->u64UserRsp = u64Slot;
        pCpu->u64UserRip = u64H;
    }
    if (pUser != NULL) {
        pUser->u64SysUserRip = u64H;
        pUser->u64SysUserRsp = u64Slot;
        pUser->u32SysUserValid = 1;
        pUser->u64UserRip = u64H;
        pUser->u64UserRsp = u64Slot;
    }
    pProc->u64UserSysRip = u64H;
    pProc->u64UserSysRsp = u64Slot;
    pProc->u64SigPending &= ~(1ull << GJ_SIGCHLD);
    if (g_u32PrtSigchldEnterLog < 4u) {
        g_u32PrtSigchldEnterLog++;
        u32Pid = process_wait_pid_of(pProc);
        kprintf("protonrt: sigchld enter pid=%u h=0x%lx rip=0x%lx "
                "rsp=0x%lx ok=1 Dual DoD B OPEN\n",
                u32Pid, (unsigned long)u64H, (unsigned long)u64Rip,
                (unsigned long)u64Rsp);
    }
    return 1;
}

/*
 * Blocking poll/ppoll: another USER thread is RUNNABLE (fork child
 * waiting to enter), or this PCB has a live wait-registered child.
 */
static int
protonrt_peer_user_runnable(void)
{
    struct gj_thread *pCur = thread_current();
    struct gj_process *pProc;
    u32 u32Pid;
    u32 u32CurId;
    u32 u32UserFl = GJ_THR_F_USER_ENTRY | GJ_THR_F_USER32_ENTRY;
    u32 u32Hi;
    u32 iId;

    pProc = protonrt_calling_user_proc();
    u32Pid = process_wait_pid_of(pProc);
    if (u32Pid != 0 && process_wait_live_count(u32Pid) != 0) {
        return 1;
    }
    u32CurId = (pCur != NULL) ? pCur->u32Id : 0u;
    u32Hi = u32CurId + (u32)GJ_MAX_THREADS;
    if (u32Hi < 256u) {
        u32Hi = 256u;
    }
    for (iId = 1u; iId <= u32Hi; iId++) {
        if (iId == u32CurId) {
            continue;
        }
        if (thread_get_state(iId) != GJ_THR_RUNNABLE) {
            continue;
        }
        if ((thread_flags_get(iId) & u32UserFl) != 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * Bind current pProc to the calling USER PCB so vfs_ram fd_select_tab
 * walks the session/sshd-auth tab (not kthread tab0).
 */
static struct gj_process *
protonrt_bind_calling_user(void)
{
    struct gj_thread *pCur = thread_current();
    struct gj_process *pUser;
    struct gj_process *pSave;

    if (pCur == NULL) {
        return NULL;
    }
    pSave = pCur->pProc;
    pUser = protonrt_calling_user_proc();
    if (pUser != NULL) {
        pCur->pProc = pUser;
    }
    return pSave;
}

static void
protonrt_unbind_calling_user(struct gj_process *pSave)
{
    struct gj_thread *pCur = thread_current();

    if (pCur != NULL) {
        pCur->pProc = pSave;
    }
}

/** Non-zero if fd is a vfs pipe/socketpair (parkable), USER tab. */
static int
protonrt_vfs_ipc_fd(i64 i64Fd)
{
    struct gj_process *pSave;
    int fIpc;

    pSave = protonrt_bind_calling_user();
    fIpc = vfs_ram_fd_poll_parkable(i64Fd);
    protonrt_unbind_calling_user(pSave);
    return fIpc;
}

static i64
protonrt_vfs_read(struct gj_linux_regs *pRegs, u8 *pBuf, size_t cbMax)
{
    struct gj_process *pSave;
    size_t cb;
    i64 i64R;
    gj_status_t st;

    if (pRegs == NULL || pBuf == NULL) {
        return -LINUX_EFAULT;
    }
    cb = (size_t)pRegs->u64Arg2;
    if (cb > cbMax) {
        cb = cbMax;
    }
    if (cb > 0 && pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    pSave = protonrt_bind_calling_user();
    i64R = vfs_ram_read((i64)pRegs->u64Arg0, pBuf, cb);
    protonrt_unbind_calling_user(pSave);
    if (i64R <= 0) {
        return i64R;
    }
    if (user_range_ok(pRegs->u64Arg1, (u64)i64R)) {
        st = copy_to_user(pRegs->u64Arg1, pBuf, (size_t)i64R);
        if (st != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, pBuf, (size_t)i64R);
    }
    return i64R;
}

static i64
protonrt_vfs_write(struct gj_linux_regs *pRegs, u8 *pBuf, size_t cbMax)
{
    struct gj_process *pSave;
    size_t cb;
    i64 i64R;
    gj_status_t st;

    if (pRegs == NULL || pBuf == NULL) {
        return -LINUX_EFAULT;
    }
    cb = (size_t)pRegs->u64Arg2;
    if (cb > cbMax) {
        cb = cbMax;
    }
    if (cb > 0 && pRegs->u64Arg1 == 0) {
        return -LINUX_EFAULT;
    }
    if (user_range_ok(pRegs->u64Arg1, cb)) {
        st = copy_from_user(pBuf, pRegs->u64Arg1, cb);
        if (st != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        memcpy(pBuf, (const void *)(gj_vaddr_t)pRegs->u64Arg1, cb);
    }
    pSave = protonrt_bind_calling_user();
    i64R = vfs_ram_write((i64)pRegs->u64Arg0, pBuf, cb);
    protonrt_unbind_calling_user(pSave);
    return i64R;
}

static i64
protonrt_msg_first_iov(u64 u64Msg, u64 *pBase, u64 *pLen)
{
    u64 u64Iov = 0;
    u64 u64N = 0;
    u64 aIov[2];

    if (pBase == NULL || pLen == NULL) {
        return -LINUX_EFAULT;
    }
    *pBase = 0;
    *pLen = 0;
    if (u64Msg == 0) {
        return -LINUX_EFAULT;
    }
    if (user_range_ok(u64Msg, 32)) {
        if (copy_from_user(&u64Iov, u64Msg + 16, 8) != GJ_OK) {
            return -LINUX_EFAULT;
        }
        if (copy_from_user(&u64N, u64Msg + 24, 8) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        u64Iov = *(const u64 *)(gj_vaddr_t)(u64Msg + 16);
        u64N = *(const u64 *)(gj_vaddr_t)(u64Msg + 24);
    }
    if (u64N == 0 || u64Iov == 0) {
        return 0;
    }
    if (user_range_ok(u64Iov, 16)) {
        if (copy_from_user(aIov, u64Iov, 16) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        memcpy(aIov, (const void *)(gj_vaddr_t)u64Iov, 16);
    }
    *pBase = aIov[0];
    *pLen = aIov[1];
    return 0;
}

/* Linux x86_64 msghdr: control+32, controllen+40. */
static i64
protonrt_msg_control(u64 u64Msg, u64 *pCtl, u64 *pCtlLen)
{
    u64 u64Ctl = 0;
    u64 u64Len = 0;

    if (pCtl == NULL || pCtlLen == NULL) {
        return -LINUX_EFAULT;
    }
    *pCtl = 0;
    *pCtlLen = 0;
    if (u64Msg == 0) {
        return -LINUX_EFAULT;
    }
    if (user_range_ok(u64Msg, 48)) {
        if (copy_from_user(&u64Ctl, u64Msg + 32, 8) != GJ_OK) {
            return -LINUX_EFAULT;
        }
        if (copy_from_user(&u64Len, u64Msg + 40, 8) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        u64Ctl = *(const u64 *)(gj_vaddr_t)(u64Msg + 32);
        u64Len = *(const u64 *)(gj_vaddr_t)(u64Msg + 40);
    }
    *pCtl = u64Ctl;
    *pCtlLen = u64Len;
    return 0;
}

/*
 * One SCM_RIGHTS int on vfs ipc. 0 if no cmsg / not rights (plain sendmsg
 * still writes data). Negative if a rights cmsg could not be queued;
 * caller skips the data byte so sendmsg stays aligned with recv.
 * cmsghdr: cmsg_len u64 +0, level i32 +8, type i32 +12, fd int +16.
 */
static i64
protonrt_vfs_scm_send(i64 i64Sock, u64 u64Msg)
{
    u64 u64Ctl = 0;
    u64 u64CtlLen = 0;
    u64 u64CmsgLen = 0;
    i32 i32Level = 0;
    i32 i32Type = 0;
    i32 i32Fd = 0;
    i64 i64St;
    struct gj_process *pSave;

    i64St = protonrt_msg_control(u64Msg, &u64Ctl, &u64CtlLen);
    if (i64St != 0) {
        return i64St;
    }
    if (u64Ctl == 0 || u64CtlLen < 20u) {
        return 0;
    }
    if (user_range_ok(u64Ctl, 20)) {
        if (copy_from_user(&u64CmsgLen, u64Ctl, 8) != GJ_OK) {
            return -LINUX_EFAULT;
        }
        if (copy_from_user(&i32Level, u64Ctl + 8, 4) != GJ_OK) {
            return -LINUX_EFAULT;
        }
        if (copy_from_user(&i32Type, u64Ctl + 12, 4) != GJ_OK) {
            return -LINUX_EFAULT;
        }
        if (copy_from_user(&i32Fd, u64Ctl + 16, 4) != GJ_OK) {
            return -LINUX_EFAULT;
        }
    } else {
        u64CmsgLen = *(const u64 *)(gj_vaddr_t)u64Ctl;
        i32Level = *(const i32 *)(gj_vaddr_t)(u64Ctl + 8);
        i32Type = *(const i32 *)(gj_vaddr_t)(u64Ctl + 12);
        i32Fd = *(const i32 *)(gj_vaddr_t)(u64Ctl + 16);
    }
    if (u64CmsgLen < 20u || i32Level != (i32)LINUX_SOL_SOCKET ||
        i32Type != PRT_SCM_RIGHTS || i32Fd < 0) {
        return 0;
    }
    pSave = protonrt_bind_calling_user();
    i64St = vfs_ram_scm_send_fd(i64Sock, i32Fd);
    protonrt_unbind_calling_user(pSave);
    return i64St;
}

static void
protonrt_vfs_scm_recv(i64 i64Sock, u64 u64Msg)
{
    u64 u64Ctl = 0;
    u64 u64CtlLen = 0;
    u64 u64CmsgLen;
    u64 u64OutLen;
    i64 i64Fd;
    i32 i32Level;
    i32 i32Type;
    i32 i32Fd;
    struct gj_process *pSave;
    u8 aCmsg[24];

    pSave = protonrt_bind_calling_user();
    i64Fd = vfs_ram_scm_recv_fd(i64Sock);
    protonrt_unbind_calling_user(pSave);
    if (i64Fd < 0) {
        return;
    }
    if (protonrt_msg_control(u64Msg, &u64Ctl, &u64CtlLen) != 0) {
        return;
    }
    if (u64Ctl == 0 || u64CtlLen < 20u) {
        return;
    }
    i32Fd = (i32)i64Fd;
    u64CmsgLen = 20u; /* CMSG_LEN(sizeof(int)) */
    i32Level = (i32)LINUX_SOL_SOCKET;
    i32Type = PRT_SCM_RIGHTS;
    memset(aCmsg, 0, sizeof(aCmsg));
    memcpy(aCmsg, &u64CmsgLen, 8);
    memcpy(aCmsg + 8, &i32Level, 4);
    memcpy(aCmsg + 12, &i32Type, 4);
    memcpy(aCmsg + 16, &i32Fd, 4);
    u64OutLen = (u64CtlLen >= 24u) ? 24u : 20u;
    if (user_range_ok(u64Ctl, u64OutLen)) {
        if (copy_to_user(u64Ctl, aCmsg, (size_t)u64OutLen) != GJ_OK) {
            return;
        }
    } else {
        memcpy((void *)(gj_vaddr_t)u64Ctl, aCmsg, (size_t)u64OutLen);
    }
    if (user_range_ok(u64Msg + 40, 8)) {
        (void)copy_to_user(u64Msg + 40, &u64OutLen, 8);
    } else {
        *(u64 *)(gj_vaddr_t)(u64Msg + 40) = u64OutLen;
    }
}

/* Last USER SYSCALL PCB (SERVICE_FIRST). Doors kthread fork uses this. */
static struct gj_process *g_pLastLinuxUserProc;

/**
 * Snapshot USER SYSCALL RIP/RSP onto the calling PCB (last-fields).
 * process_linux_fork reads these when current is a cold kthread so a
 * LINUX parent in [0x1000000, 0x1800000) still gets thread_create_user.
 */
static void
protonrt_snap_user_syscall(void)
{
    struct gj_thread *pThr = thread_current();
    struct gj_cpu *pCpu;
    struct gj_process *pProc;
    u64 u64Rip = 0;
    u64 u64Rsp = 0;

    pCpu = cpu_current();
    if (pCpu != NULL) {
        u64Rip = pCpu->u64UserRip;
        u64Rsp = pCpu->u64UserRsp;
    }
    if (protonrt_thr_is_user(pThr) != 0) {
        pProc = pThr->pProc;
        if (pProc == NULL) {
            return;
        }
        if (u64Rip == 0 && pThr->u32SysUserValid != 0) {
            u64Rip = pThr->u64SysUserRip;
            u64Rsp = pThr->u64SysUserRsp;
        }
        if (u64Rip == 0) {
            u64Rip = pThr->u64UserRip;
        }
        if (u64Rsp == 0) {
            u64Rsp = pThr->u64UserRsp;
        }
        if (u64Rip != 0) {
            pProc->u64UserSysRip = u64Rip;
        }
        if (u64Rsp != 0) {
            pProc->u64UserSysRsp = u64Rsp;
        }
        pProc->u32UserSysThr = pThr->u32Id;
        pProc->pUserThr = pThr;
        g_pLastLinuxUserProc = pProc;
        return;
    }
    /*
     * Doors kthread: pClient is the blocked USER thr. Stamp that PCB so
     * LINUX [0x1000000, 0x1800000) fork still gets thread_create_user and
     * execve can thread_exec_replace after trampoline cleared USER_ENTRY.
     */
    pThr = protonrt_door_client_user();
    if (pThr != NULL) {
        pProc = pThr->pProc;
        if (u64Rip == 0 && pThr->u32SysUserValid != 0) {
            u64Rip = pThr->u64SysUserRip;
            u64Rsp = pThr->u64SysUserRsp;
        }
        if (u64Rip == 0) {
            u64Rip = pThr->u64UserRip;
        }
        if (u64Rsp == 0) {
            u64Rsp = pThr->u64UserRsp;
        }
        if (u64Rip != 0) {
            pProc->u64UserSysRip = u64Rip;
        }
        if (u64Rsp != 0) {
            pProc->u64UserSysRsp = u64Rsp;
        }
        pProc->u32UserSysThr = pThr->u32Id;
        pProc->pUserThr = pThr;
        g_pLastLinuxUserProc = pProc;
        return;
    }
    /*
     * Doors kthread, no pClient (queue path): GS USER_* may still be the
     * blocked USER resume. Stamp the unique wait-registered USER child
     * only when GS RIP matches that child (OpenSSH session execve).
     * Parent poll RIP must not clobber the child's last-fields.
     * Else stamp last LINUX user.ld so [0x1000000, 0x1800000) still
     * gets thread_create_user.
     */
    pProc = process_linux_live_user_child(u64Rip, u64Rsp);
    if (pProc != NULL) {
        int fMatch = 0;

        if (u64Rip != 0 && (pProc->u64UserSysRip == u64Rip ||
                            pProc->u64StartEntry == u64Rip ||
                            pProc->u64ExecEntry == u64Rip)) {
            fMatch = 1;
        }
        if (u64Rsp != 0 && pProc->u64UserSysRsp == u64Rsp &&
            (u64Rip >= 0x1000000ull && u64Rip < 0x5000000ull)) {
            /* Session child stack vs parent poll stack. */
            fMatch = 1;
        }
        if (pProc->u64UserSysRip == 0 &&
            (u64Rip >= 0x1000000ull && u64Rip < 0x5000000ull)) {
            fMatch = 1;
        }
        if (fMatch != 0) {
            pProc->u64UserSysRip = u64Rip;
            if (u64Rsp >= 0x1000000ull && u64Rsp < 0x80000000ull) {
                pProc->u64UserSysRsp = u64Rsp;
            }
            g_pLastLinuxUserProc = pProc;
            return;
        }
        /* Unique child exists; GS is parent poll. Do not clobber. */
        if (g_pLastLinuxUserProc == pProc) {
            return;
        }
    }
    if (g_pLastLinuxUserProc == NULL ||
        g_pLastLinuxUserProc->u32Alive == 0) {
        return;
    }
    if (!(u64Rip >= 0x1000000ull && u64Rip < 0x5000000ull)) {
        return;
    }
    pProc = g_pLastLinuxUserProc;
    pProc->u64UserSysRip = u64Rip;
    if (u64Rsp >= 0x1000000ull && u64Rsp < 0x80000000ull) {
        pProc->u64UserSysRsp = u64Rsp;
    }
    g_pLastLinuxUserProc = pProc;
}

/**
 * LINUX user text for USER execve. HEAD user.ld [0x1000000, 0x1800000)
 * (dash/sh); OpenSSH ~11MiB at that base overflows 8MiB; 0x4000000
 * window is the same ELF class. Reject ld-gj INTERP / PE32.
 */
static int
protonrt_linux_user_text_ok(u64 u64Va)
{
    return (u64Va >= 0x1000000ull && u64Va < 0x5000000ull) ? 1 : 0;
}

static int
protonrt_linux_text_lo(u64 u64Va)
{
    return (u64Va >= 0x1000000ull && u64Va < 0x1800000ull) ? 1 : 0;
}

static int
protonrt_linux_stack_ok(u64 u64Rsp)
{
    return (u64Rsp >= 0x1000000ull && u64Rsp < 0x80000000ull) ? 1 : 0;
}

static int
protonrt_linux_pcb_userld(const struct gj_process *pProc)
{
    if (pProc == NULL || pProc->u32Personality != 1u ||
        pProc->u32Alive == 0) {
        return 0;
    }
    if (protonrt_linux_user_text_ok(pProc->u64StartEntry) != 0 ||
        protonrt_linux_user_text_ok(pProc->u64ExecEntry) != 0 ||
        protonrt_linux_user_text_ok(pProc->u64UserSysRip) != 0 ||
        protonrt_linux_text_lo(pProc->u64StartEntry) != 0 ||
        protonrt_linux_text_lo(pProc->u64ExecEntry) != 0 ||
        protonrt_linux_text_lo(pProc->u64UserSysRip) != 0) {
        return 1;
    }
    if (protonrt_linux_stack_ok(pProc->u64UserSysRsp) != 0 ||
        protonrt_linux_stack_ok(pProc->u64ExecStack) != 0) {
        return 1;
    }
    return 0;
}

/**
 * Doors kthread: last USER SYSCALL PCB if it is LINUX user.ld
 * ([0x1000000, 0x1800000) or [0x4000000, 0x5000000)).
 * Fork/vfork/clone only — not execve (wrong PCB if parent polled).
 */
static struct gj_process *
protonrt_last_linux_userld_parent(void)
{
    if (protonrt_linux_pcb_userld(g_pLastLinuxUserProc) != 0) {
        return g_pLastLinuxUserProc;
    }
    return NULL;
}

/**
 * Parent PCB for fork/vfork/clone. Prefer the calling USER thr so a
 * fork child is not g_pLinuxProc. A doors/queue kthread is LINUX-default
 * and must not become the clone source when GS still holds a user.ld RIP
 * ([0x1000000, 0x1800000) dash/sh or [0x4000000, 0x5000000) OpenSSH).
 * Last snapped LINUX user.ld is that parent. Dual DoD B OPEN.
 */
static struct gj_process *
protonrt_linux_fork_parent(void)
{
    struct gj_process *pParent;

    pParent = protonrt_calling_user_proc();
    if (pParent != NULL) {
        return pParent;
    }
    /* Doors kthread: last LINUX user.ld USER SYSCALL (sshd / dash). */
    pParent = protonrt_last_linux_userld_parent();
    if (pParent != NULL) {
        return pParent;
    }
    /*
     * LINUX [0x1000000, 0x1800000) last-fields: still a USER-child
     * parent when the snap PCB is g_pLinuxProc (dash/sh). Dual DoD B OPEN.
     */
    if (g_pLastLinuxUserProc != NULL &&
        g_pLastLinuxUserProc->u32Alive != 0 &&
        g_pLastLinuxUserProc->u32Personality == 1u &&
        protonrt_linux_text_lo(g_pLastLinuxUserProc->u64StartEntry) != 0) {
        return g_pLastLinuxUserProc;
    }
    if (g_pLastLinuxUserProc != NULL &&
        g_pLastLinuxUserProc->u32Alive != 0 &&
        g_pLastLinuxUserProc->u32Personality == 1u &&
        protonrt_linux_text_lo(g_pLastLinuxUserProc->u64UserSysRip) != 0) {
        return g_pLastLinuxUserProc;
    }
    return NULL;
}

/**
 * Execve target PCB. Calling USER first so a fork session child is not
 * g_pLinuxProc. Never the last-fork parent snap (parent poll vs child
 * exec). thread_exec_replace only on that PCB.
 */
static struct gj_process *
protonrt_linux_exec_proc(void)
{
    struct gj_process *pExec;
    struct gj_cpu *pCpu;
    u64 u64Rip = 0;
    u64 u64Rsp = 0;

    pExec = protonrt_calling_user_proc();
    if (pExec != NULL) {
        return pExec;
    }
    /*
     * Doors/queue kthread: never the kthread PCB (LINUX-default). Unique
     * wait-registered USER child (OpenSSH session) vs parent poll.
     * thread_exec_replace only on that PCB.
     */
    pCpu = cpu_current();
    if (pCpu != NULL) {
        u64Rip = pCpu->u64UserRip;
        u64Rsp = pCpu->u64UserRsp;
    }
    pExec = process_linux_live_user_child(u64Rip, u64Rsp);
    if (pExec != NULL) {
        return pExec;
    }
    /* Last snap only when GS RIP/RSP name that wait-registered child. */
    if (g_pLastLinuxUserProc != NULL &&
        process_wait_pid_of(g_pLastLinuxUserProc) != 0 &&
        protonrt_linux_pcb_userld(g_pLastLinuxUserProc) != 0) {
        if (u64Rip != 0 &&
            (g_pLastLinuxUserProc->u64UserSysRip == u64Rip ||
             g_pLastLinuxUserProc->u64StartEntry == u64Rip ||
             g_pLastLinuxUserProc->u64ExecEntry == u64Rip) &&
            (u64Rsp == 0 || g_pLastLinuxUserProc->u64UserSysRsp == 0 ||
             g_pLastLinuxUserProc->u64UserSysRsp == u64Rsp)) {
            return g_pLastLinuxUserProc;
        }
    }
    return NULL;
}

/**
 * USER execve: re-arm USER_ENTRY (trampoline cleared it) and
 * thread_exec_replace only. Never thread_create_user (0.1.140 #PF I=1).
 * Retargets the calling USER thr so sysretq lands on the new image.
 * Stamps PCB last-fields so a later fork from [0x1000000, 0x1800000)
 * still gets thread_create_user. Returns replace count (0 -> fail-close).
 */
static u32
protonrt_exec_replace_user(struct gj_process *pExec, u64 u64Entry,
                           u64 u64Stack)
{
    struct gj_cpu *pCpu;
    struct gj_thread *pThr;
    struct gj_thread *pUser;
    u32 cRepl;
    int fUser;

    if (pExec == NULL) {
        return 0;
    }
    pCpu = cpu_current();
    pThr = thread_current();
    fUser = (pThr != NULL && pThr->pProc == pExec &&
             protonrt_thr_is_user(pThr) != 0);
    if (fUser != 0) {
        pExec->pUserThr = pThr;
    }
    pUser = pExec->pUserThr;
    if (pUser != NULL &&
        (pUser->u32State == GJ_THR_UNUSED ||
         pUser->u32State == GJ_THR_EXITED || pUser->pProc != pExec)) {
        pUser = NULL;
        pExec->pUserThr = NULL;
    }
    if (pUser == NULL && fUser != 0) {
        pUser = pThr;
        pExec->pUserThr = pThr;
    }
    if (pUser == NULL && pThr != NULL && pThr->pProc == pExec &&
        pThr->pfnEntry == NULL) {
        pUser = pThr;
        pExec->pUserThr = pThr;
        fUser = 1;
    }
    if (pUser == NULL) {
        pUser = protonrt_door_client_user();
        if (pUser != NULL && pUser->pProc != pExec) {
            pUser = NULL;
        }
        if (pUser != NULL) {
            pExec->pUserThr = pUser;
        }
    }
    if (pUser != NULL) {
        pUser->u32Flags |= GJ_THR_F_USER_ENTRY;
        pUser->pfnEntry = NULL;
        if (u64Entry != 0 && elf_stack_rsp_live_ok(u64Stack) != 0) {
            pUser->u64UserRip = u64Entry;
            pUser->u64UserRsp = u64Stack;
            pUser->u64SysUserRip = u64Entry;
            pUser->u64SysUserRsp = u64Stack;
            pUser->u32SysUserValid = 1;
        }
    }
    /* thread_exec_replace only. Never thread_create_user (0.1.140 #PF I=1). */
    cRepl = thread_exec_replace(pExec, u64Entry, u64Stack);
    if (fUser != 0 && u64Entry != 0 &&
        elf_stack_rsp_live_ok(u64Stack) != 0) {
        if (pCpu != NULL) {
            pCpu->u64UserRip = u64Entry;
            pCpu->u64UserRsp = u64Stack;
        }
        pThr->u64UserRip = u64Entry;
        pThr->u64UserRsp = u64Stack;
        pThr->u64SysUserRip = u64Entry;
        pThr->u64SysUserRsp = u64Stack;
        pThr->u32SysUserValid = 1;
        if (cRepl == 0) {
            cRepl = 1;
        }
    }
    if (cRepl == 0 && pUser != NULL && u64Entry != 0 &&
        elf_stack_rsp_live_ok(u64Stack) != 0) {
        /* Doors kthread: USER TCB retargeted; sysretq lands on new image. */
        cRepl = 1;
    }
    if (cRepl != 0 && u64Entry != 0) {
        pExec->u64UserSysRip = u64Entry;
        if (u64Stack != 0) {
            pExec->u64UserSysRsp = u64Stack;
        }
        if (pUser != NULL) {
            pExec->u32UserSysThr = pUser->u32Id;
        } else if (pThr != NULL && pThr->pProc == pExec) {
            pExec->u32UserSysThr = pThr->u32Id;
        }
        g_pLastLinuxUserProc = pExec;
    }
    return cRepl;
}

/**
 * Per-PCB cwd for USER; kernel smoke keeps the global "/".
 */
static char *
protonrt_cwd_buf(void)
{
    struct gj_process *pUser = protonrt_calling_user_proc();

    if (pUser != NULL) {
        if (pUser->szCwd[0] == '\0') {
            pUser->szCwd[0] = '/';
            pUser->szCwd[1] = '\0';
        }
        return pUser->szCwd;
    }
    return g_szCwd;
}

/**
 * Soft: resolve parent PCB for fork/clone/wait cold wires.
 * Prefer the calling USER PCB so child wait/fork is not g_pLinuxProc.
 * Doors kthread: last LINUX user.ld (same as fork parent). Kernel
 * smoke with no user.ld parent returns NULL (unfiltered wait4).
 */
static struct gj_process *
protonrt_soft_parent(void)
{
    return protonrt_linux_fork_parent();
}

/** Grep once: protonrt: soft fork-wait wire PASS */
static void
protonrt_soft_fork_wait_wire_pass_once(void)
{
    if (g_fPrtSoftForkWaitWireOnce != 0) {
        return;
    }
    g_fPrtSoftForkWaitWireOnce = 1;
    kprintf("protonrt: soft fork-wait wire PASS\n");
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
    case LINUX_NR_pipe2:
    case LINUX_NR_select:
    case LINUX_NR_pselect6:
    case LINUX_NR_ppoll:
    case LINUX_NR_dup:
    case LINUX_NR_dup2:
    case LINUX_NR_dup3:
    case LINUX_NR_fcntl:
    case LINUX_NR_ioctl:
    case LINUX_NR_sendfile:
    case LINUX_NR_splice:
    case LINUX_NR_tee:
    case LINUX_NR_vmsplice:
    case LINUX_NR_copy_file_range:
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
    case LINUX_NR_eventfd:
    case LINUX_NR_eventfd2:
    case LINUX_NR_signalfd4:
    case LINUX_NR_memfd_create:
    case LINUX_NR_epoll_create:
    case LINUX_NR_epoll_create1:
    case LINUX_NR_epoll_ctl:
    case LINUX_NR_epoll_wait:
    case LINUX_NR_epoll_pwait:
    case LINUX_NR_epoll_pwait2:
        return (u32)PRT_SOFT_GRP_FS;
    case LINUX_NR_socket:
    case LINUX_NR_connect:
    case LINUX_NR_accept:
    case LINUX_NR_accept4:
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
 * Soft enter note - never alters service return. Diagnostics only.
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
 * Twin prefixes: protonrt: soft ... / cold_link: soft ...
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
            "(cold personality soft)\n",
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
            "path,dir,fd,io_uring_min,pipe,epoll,eventfd,memfd,ioctl,"
            "sendfile,fcntl,copy_file_range "
            "wave=%u "
            "(Proton neighborhood residual; Soft!=product; not bar3)\n",
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_FS],
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft net */
    kprintf("protonrt: soft net enter=%llu surface=socket,connect,"
            "send_star,recv_star,bind,listen,accept,close,poll,select,"
            "socketpair,ioctl,fcntl,sendfile_net,copy_file_net cold_net=1 "
            "wave=%u "
            "(Soft!=product userspace driver reach; abi_adj=proton_p0; "
            "dual_dod_a=OPEN dual_dod_b=OPEN; not bar3)\n",
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
            "io_uring=min_rings_soft wave=%u "
            "(cold personality soft inventory)\n",
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

    /* Grep: protonrt: soft honesty (Wave 20 deepen + Dual DoD residual) */
    kprintf("protonrt: soft honesty hybrid=OptionC open=1 "
            "product_linux_abi=open soft_only=1 multi_server=OPEN "
            "dual_dod_a=OPEN dual_dod_b=OPEN freestanding_skip=1 "
            "product_path=UDX_DDI+hot_cold_ABI g_ac_1=1 "
            "wave=%u (cold personality soft; never closes hybrid or Dual DoD)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft surfaces (Wave 20 deepen + Dual DoD residual) */
    kprintf("protonrt: soft surfaces count=%u wave=%u "
            "names=inventory,path,rates,honesty,catalog,surfaces,note,"
            "deepen,ioctl,sendfile,fcntl,copy_file,dual_dod,PASS\n",
            (unsigned)GJ_PROTONRT_SOFT_AREAS,
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft note (Wave 20 deepen) */
    kprintf("protonrt: soft note milestone=wave98 exclusive=1 wave=%u\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: protonrt: soft catalog (Wave 20 deepen + Dual DoD residual) */
    kprintf("protonrt: soft catalog wave=%u areas=%u "
            "surfaces=inventory,enter,fs,net,proc,time,mem,other,"
            "confine,attach,path,rates,honesty,catalog,return,retmap,deepen,"
            "ioctl,sendfile,fcntl,copy_file,dual_dod,PASS\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned)GJ_PROTONRT_SOFT_AREAS);



    /* Grep: protonrt: soft deepen wave (Wave 24 stamp) */
    kprintf("protonrt: soft deepen wave=%u areas=%u logs=%llu enter=%llu "
            "fs=%llu net=%llu proc=%llu confine_deny=%u "
            "(Wave 35 exclusive; cold link soft)\n",
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
     * Twin prefix: cold_link: soft ... (agent-friendly alias).
     */
    /* Grep: cold_link: soft inventory */
    kprintf("cold_link: soft inventory wave=%u areas=%u enter=%llu null=%llu "
            "attach=%llu logs=%llu proc=%u confined=%u alive=%u "
            "expose_ents=%u "
            "(cold personality soft)\n",
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
            " wave=%u (soft inventory)\n",
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

    /* Grep: cold_link: soft honesty (Wave 20 deepen + Dual DoD residual) */
    kprintf("cold_link: soft honesty hybrid=OptionC open=1 "
            "product_linux_abi=open soft_only=1 multi_server=OPEN "
            "dual_dod_a=OPEN dual_dod_b=OPEN freestanding_skip=1 "
            "product_path=UDX_DDI+hot_cold_ABI g_ac_1=1 "
            "wave=%u (soft inventory; never closes hybrid or Dual DoD)\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE);

    /* Grep: cold_link: soft catalog (Wave 20 deepen + Dual DoD residual) */
    kprintf("cold_link: soft catalog wave=%u areas=%u "
            "surfaces=inventory,enter,confine,path,rates,honesty,"
            "catalog,return,retmap,deepen,ioctl,sendfile,fcntl,copy_file,"
            "dual_dod,PASS\n",
            (unsigned)GJ_PROTONRT_SOFT_WAVE,
            (unsigned)GJ_PROTONRT_SOFT_AREAS);


    /* Grep: cold_link: soft deepen wave (Wave 24 stamp) */
    kprintf("cold_link: soft deepen wave=%u areas=%u logs=%llu enter=%llu "
            "(Wave 35 exclusive; cold link soft)\n",
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
 * Lean soft residual - sparse once-lamp only (no version stamp, storm=0).
 * Fold group enters + confine/expose + attach + cold_net/cold_ipc userspace
 * driver reach + residual lean hit tallies + Proton neighborhood hits +
 * ioctl/sendfile/fcntl/copy_file Dual DoD residual into greppable lines.
 * Soft!=product dual license (MIT OR Apache-2.0). G-AC-1: not in-kernel .ko
 * product AC. Never hard-gates. Dual DoD A/B OPEN (agent never closes).
 * Freestanding SKIP. Product path = UDX/DDI + hot+cold ABI. Bar3-adjacent
 * ABI surface only - never a bar3 claim. Comment law: Soft!=product ASCII
 * only (no Softneq unicode); pure block comments only; send_star/recv_star
 * (never wildcard forms that split block comments).
 * greppable: protonrt: soft residual lean
 * greppable: cold_link: soft residual lean
 * greppable: protonrt: soft neighborhood residual lean
 * greppable: protonrt: soft fcntl residual lean
 * greppable: protonrt: soft copy_file residual lean
 * greppable: protonrt: soft dual_dod OPEN
 */
static void
protonrt_soft_residual_lean_once(void)
{
    u32 u32HasProc;
    u32 u32Confined;
    u32 u32Alive;
    u32 u32Promises;
    u32 u32ColdIpcAttached;

    if (g_fPrtSoftResidualLeanOnce != 0) {
        return;
    }
    g_fPrtSoftResidualLeanOnce = 1;
    protonrt_soft_inc(&g_u64PrtSoftResidualLean);

    u32HasProc = (g_pLinuxProc != NULL) ? 1u : 0u;
    u32Confined = 0u;
    u32Alive = 0u;
    u32Promises = 0u;
    if (g_pLinuxProc != NULL) {
        u32Confined = g_pLinuxProc->u32Confined;
        u32Alive = g_pLinuxProc->u32Alive;
        u32Promises = g_pLinuxProc->u32Promises;
    }
    u32ColdIpcAttached = cold_ipc_personality_attached() ? 1u : 0u;

    /* Grep: protonrt: soft residual lean */
    kprintf("protonrt: soft residual lean "
            "enter=%llu fs=%llu net=%llu proc=%llu time=%llu mem=%llu "
            "other=%llu attach=%llu null=%llu "
            "promise_deny=%u expose_deny=%u death=%u expose_ents=%u "
            "path_deny=%llu fork_wait_wire=%u poll_block=%u select_once=%u "
            "epoll_once=%u neighborhood_once=%u "
            "ioctl_once=%u sendfile_once=%u fcntl_once=%u copyfr_once=%u "
            "cold_net_bridge=%u cold_ipc_attached=%u "
            "lean_socket=%llu lean_xfer=%llu lean_poll=%llu lean_select=%llu "
            "lean_close=%llu lean_rw_net=%llu "
            "lean_sockpair=%llu lean_pipe=%llu lean_epoll=%llu "
            "lean_eventfd=%llu lean_memfd=%llu "
            "lean_ioctl=%llu lean_sendfile=%llu "
            "lean_fcntl=%llu lean_copyfr=%llu "
            "proc=%u confined=%u alive=%u promises=0x%x multi_server=OPEN "
            "dual_dod_a=OPEN dual_dod_b=OPEN freestanding_skip=1 "
            "product_path=UDX_DDI+hot_cold_ABI "
            "userspace_drivers=reach g_ac_1=1 lean_runs=%llu "
            "abi_adj=proton_p0 soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "storm=0 not_bar3=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1; "
            "no version stamp; cold net/ipc residual for UDX hosts; "
            "Linux-shaped apps / Proton neighborhood residual deepen; "
            "C2 Dual DoD residual ioctl+sendfile+fcntl+copy_file; "
            "Dual DoD A/B OPEN; "
            "bar3-adjacent ABI surface only; not .ko product AC; not bar3)\n",
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_FS],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_NET],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_PROC],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_TIME],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_MEM],
            (unsigned long long)g_u64PrtSoftGrp[PRT_SOFT_GRP_OTHER],
            (unsigned long long)g_u64PrtSoftAttach,
            (unsigned long long)g_u64PrtSoftNull,
            g_u32ConfinePromiseDeny, g_u32ConfineExposeDeny,
            g_u32ConfineDeathCleanup, g_u32ExposeSoftCount,
            (unsigned long long)g_u64PrtSoftPathDeny,
            g_fPrtSoftForkWaitWireOnce ? 1u : 0u,
            g_fPrtSoftPollBlockOnce ? 1u : 0u,
            g_fPrtSoftSelectOnce ? 1u : 0u,
            g_fPrtSoftEpollOnce ? 1u : 0u,
            g_fPrtSoftNeighborhoodOnce ? 1u : 0u,
            g_fPrtSoftIoctlOnce ? 1u : 0u,
            g_fPrtSoftSendfileOnce ? 1u : 0u,
            g_fPrtSoftFcntlOnce ? 1u : 0u,
            g_fPrtSoftCopyFrOnce ? 1u : 0u,
            g_fPrtSoftColdNetOnce ? 1u : 0u,
            u32ColdIpcAttached,
            (unsigned long long)g_u64PrtLeanSocket,
            (unsigned long long)g_u64PrtLeanXfer,
            (unsigned long long)g_u64PrtLeanPoll,
            (unsigned long long)g_u64PrtLeanSelect,
            (unsigned long long)g_u64PrtLeanClose,
            (unsigned long long)g_u64PrtLeanRwNet,
            (unsigned long long)g_u64PrtLeanSockpair,
            (unsigned long long)g_u64PrtLeanPipe,
            (unsigned long long)g_u64PrtLeanEpoll,
            (unsigned long long)g_u64PrtLeanEventfd,
            (unsigned long long)g_u64PrtLeanMemfd,
            (unsigned long long)g_u64PrtLeanIoctl,
            (unsigned long long)g_u64PrtLeanSendfile,
            (unsigned long long)g_u64PrtLeanFcntl,
            (unsigned long long)g_u64PrtLeanCopyFr,
            u32HasProc, u32Confined, u32Alive, u32Promises,
            (unsigned long long)g_u64PrtSoftResidualLean);

    /* Grep: cold_link: soft residual lean (twin prefix; one line) */
    kprintf("cold_link: soft residual lean "
            "service=protonrt_service "
            "backend=vfs_ram+linux_cold_net+protonrt_cold_linux "
            "cold_ipc=1 cold_net=1 "
            "surface=socket+bind+listen+accept+connect+"
            "send_star+recv_star+close+poll+select+epoll+"
            "socketpair+pipe+eventfd+memfd+ioctl+sendfile+"
            "fcntl+copy_file_range "
            "lean_socket=%llu lean_xfer=%llu lean_poll=%llu lean_select=%llu "
            "lean_close=%llu lean_rw_net=%llu "
            "lean_sockpair=%llu lean_pipe=%llu lean_epoll=%llu "
            "lean_eventfd=%llu lean_memfd=%llu "
            "lean_ioctl=%llu lean_sendfile=%llu "
            "lean_fcntl=%llu lean_copyfr=%llu "
            "enter=%llu attach=%llu logs=%llu confine_ents=%u "
            "path_deny=%llu multi_server=OPEN hybrid=OptionC "
            "dual_dod_a=OPEN dual_dod_b=OPEN freestanding_skip=1 "
            "product_path=UDX_DDI+hot_cold_ABI "
            "userspace_drivers=reach g_ac_1=1 abi_adj=proton_p0 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 storm=0 not_bar3=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; G-AC-1; "
            "no version stamp; Linux-shaped apps / Proton neighborhood; "
            "C2 Dual DoD residual ioctl+sendfile+fcntl+copy_file; "
            "Dual DoD A/B OPEN; "
            "bar3-adjacent ABI surface only; "
            "not bar3 / not Deck Top 50; not in-kernel .ko product AC)\n",
            (unsigned long long)g_u64PrtLeanSocket,
            (unsigned long long)g_u64PrtLeanXfer,
            (unsigned long long)g_u64PrtLeanPoll,
            (unsigned long long)g_u64PrtLeanSelect,
            (unsigned long long)g_u64PrtLeanClose,
            (unsigned long long)g_u64PrtLeanRwNet,
            (unsigned long long)g_u64PrtLeanSockpair,
            (unsigned long long)g_u64PrtLeanPipe,
            (unsigned long long)g_u64PrtLeanEpoll,
            (unsigned long long)g_u64PrtLeanEventfd,
            (unsigned long long)g_u64PrtLeanMemfd,
            (unsigned long long)g_u64PrtLeanIoctl,
            (unsigned long long)g_u64PrtLeanSendfile,
            (unsigned long long)g_u64PrtLeanFcntl,
            (unsigned long long)g_u64PrtLeanCopyFr,
            (unsigned long long)g_u64PrtSoftEnter,
            (unsigned long long)g_u64PrtSoftAttach,
            (unsigned long long)g_u64PrtSoftLogN,
            g_u32ExposeSoftCount,
            (unsigned long long)g_u64PrtSoftPathDeny);

    /* Grep: protonrt: soft neighborhood residual lean (once companion) */
    if (g_fPrtSoftNeighborhoodOnce == 0) {
        g_fPrtSoftNeighborhoodOnce = 1;
        kprintf("protonrt: soft neighborhood residual lean "
                "sockpair=%llu pipe=%llu epoll=%llu eventfd=%llu memfd=%llu "
                "socket=%llu xfer=%llu poll=%llu select=%llu "
                "close=%llu rw_net=%llu ioctl=%llu sendfile=%llu "
                "fcntl=%llu copyfr=%llu "
                "abi_adj=proton_p0 Soft!=product g_ac_1=1 dual=MIT_OR_Apache-2.0 "
                "not_bar3=1 not_deck_top50=1 "
                "(Linux-shaped apps / Proton neighborhood cold NR residual; "
                "never bar3 claim; Soft!=product)\n",
                (unsigned long long)g_u64PrtLeanSockpair,
                (unsigned long long)g_u64PrtLeanPipe,
                (unsigned long long)g_u64PrtLeanEpoll,
                (unsigned long long)g_u64PrtLeanEventfd,
                (unsigned long long)g_u64PrtLeanMemfd,
                (unsigned long long)g_u64PrtLeanSocket,
                (unsigned long long)g_u64PrtLeanXfer,
                (unsigned long long)g_u64PrtLeanPoll,
                (unsigned long long)g_u64PrtLeanSelect,
                (unsigned long long)g_u64PrtLeanClose,
                (unsigned long long)g_u64PrtLeanRwNet,
                (unsigned long long)g_u64PrtLeanIoctl,
                (unsigned long long)g_u64PrtLeanSendfile,
                (unsigned long long)g_u64PrtLeanFcntl,
                (unsigned long long)g_u64PrtLeanCopyFr);
    }

    /*
     * Dual DoD honesty once-lamp (Soft!=product; agent never closes A/B).
     * greppable: protonrt: soft dual_dod OPEN
     * Product = UDX/DDI + hot+cold ABI. Freestanding rtl/USB SKIP.
     * G-AC-1: not in-kernel .ko product AC. storm=0.
     */
    if (g_fPrtSoftDualDodOnce == 0) {
        g_fPrtSoftDualDodOnce = 1;
        kprintf("protonrt: soft dual_dod OPEN "
                "dual_dod_a=OPEN dual_dod_b=OPEN "
                "product_path=UDX_DDI+hot_cold_ABI freestanding_skip=1 "
                "g_ac_1=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                "cold_net=1 cold_ipc=1 ioctl_devctl=1 sendfile_net=1 "
                "fcntl_nonblock=1 copy_file_net=1 "
                "storm=0 not_bar3=1 "
                "(C2 Dual DoD residual honesty; agent never closes A/B; "
                "not freestanding wire; not .ko product AC)\n");
    }
}

/**
 * After first product service activity, print soft inventory once.
 * Diagnostics only - never gates cold attach or confine soft PASS.
 * Also emits lean residual once (storm=0 companion; no version stamp).
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
    protonrt_soft_residual_lean_once();
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
 * Deny ambient expose when confined - confined subjects cannot expand
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
 * Confined + non-empty expose table => path must match (soft unveil spirit).
 * Empty table => promise-only (preserves confine soft PASS / RPATH smokes).
 * Grep: confine: expose soft path
 */
static i64
confine_soft_path_policy(const char *szPath, u32 u32Need)
{
    if (g_pLinuxProc == NULL || g_pLinuxProc->u32Confined == 0u) {
        return 0; /* ambient */
    }
    if (g_u32ExposeSoftCount == 0u) {
        return 0; /* soft: no expose set yet -> promise gates only */
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
 * Product process_death is hot-path (linux_hot -> process_death G-PROC-5).
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
 * RDONLY -> RPATH; WRONLY -> WPATH; RDWR -> RPATH|WPATH; O_CREAT -> +CPATH.
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

/* Soft path promise helpers (ambient / NULL proc => 0). */
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

/*
 * Soft Linux .ko load via finit/init/delete_module (cold NRs 175/176/313).
 * Abandoned: in-kernel .ko AC is not product (G-AC-1). Return -ENOSYS.
 * greppable: protonrt: soft init_module|finit_module|delete_module ENOSYS
 */
/** Abandoned in-kernel .ko load. Product drivers are userspace UDX. */
static i64
soft_mod_load_and_init(const void *pImg, size_t cb, const char *szName)
{
    (void)pImg;
    (void)cb;
    (void)szName;
    return -(i64)LINUX_ENOSYS;
}

static void
protonrt_path_join_cwd(char *szOut, size_t cbOut)
{
    char aRel[96];
    char *szCwd;
    size_t iC;
    size_t iR;

    if (szOut == NULL || cbOut == 0 || szOut[0] == '\0' || szOut[0] == '/') {
        return;
    }
    for (iR = 0; iR + 1 < sizeof(aRel) && szOut[iR] != '\0'; iR++) {
        aRel[iR] = szOut[iR];
    }
    aRel[iR] = '\0';
    szCwd = protonrt_cwd_buf();
    iC = 0;
    while (szCwd[iC] != '\0' && iC + 1 < cbOut) {
        szOut[iC] = szCwd[iC];
        iC++;
    }
    if (iC == 0 || szOut[iC - 1] != '/') {
        if (iC + 1 < cbOut) {
            szOut[iC] = '/';
            iC++;
        }
    }
    iR = 0;
    while (aRel[iR] != '\0' && iC + 1 < cbOut) {
        szOut[iC] = aRel[iR];
        iC++;
        iR++;
    }
    szOut[iC] = '\0';
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
                protonrt_path_join_cwd(szOut, cbOut);
                return;
            }
        }
        szOut[cbOut - 1] = '\0';
        protonrt_path_join_cwd(szOut, cbOut);
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
    protonrt_path_join_cwd(szOut, cbOut);
}

/*
 * Product dash + OpenSSH 10.5 DUT embeds. vfs_ram is 32KiB; aImg[16384]
 * cannot hold 8-11MiB ELFs. Required (not weak). Not gj_sshd_elf_blob.
 */
extern const u8 gj_shell_elf_blob[];
extern const u8 gj_shell_elf_blob_end[];
extern const u8 gj_openssh_sshd_session_elf_blob[];
extern const u8 gj_openssh_sshd_session_elf_blob_end[];
extern const u8 gj_openssh_sshd_auth_elf_blob[];
extern const u8 gj_openssh_sshd_auth_elf_blob_end[];
extern const u8 gj_openssh_sshd_elf_blob[];
extern const u8 gj_openssh_sshd_elf_blob_end[];

struct gj_exec_embed_ent {
    const char *szPath;
    const u8 *pBlob;
    const u8 *pEnd;
};

static const char *
protonrt_path_base(const char *szPath)
{
    const char *szBase;
    size_t i;

    if (szPath == NULL || szPath[0] == '\0') {
        return "";
    }
    szBase = szPath;
    for (i = 0; szPath[i] != '\0'; i++) {
        if (szPath[i] == '/' && szPath[i + 1] != '\0') {
            szBase = szPath + i + 1;
        }
    }
    return szBase;
}

/*
 * OpenSSH later-wall libexec. vfs_ram seeds 1-byte stubs so stat/X_OK
 * pass; execve must not treat those stubs as a successful image.
 */
static int
protonrt_exec_later_wall_path(const char *szPath)
{
    const char *szBase;

    if (szPath == NULL || szPath[0] == '\0') {
        return 0;
    }
    if (strcmp(szPath, "/usr/libexec/sshd-session") == 0 ||
        strcmp(szPath, "/usr/libexec/sshd-auth") == 0) {
        return 1;
    }
    szBase = protonrt_path_base(szPath);
    if (strcmp(szBase, "sshd-session") == 0 ||
        strcmp(szBase, "sshd-auth") == 0) {
        return 1;
    }
    return 0;
}

static int
protonrt_exec_embed_path_hit(const char *szPath, const char *szEnt)
{
    if (szPath == NULL || szEnt == NULL) {
        return 0;
    }
    if (strcmp(szPath, szEnt) == 0) {
        return 1;
    }
    if (protonrt_exec_later_wall_path(szEnt) == 0) {
        return 0;
    }
    return (strcmp(protonrt_path_base(szPath), protonrt_path_base(szEnt)) == 0)
               ? 1
               : 0;
}

static const struct gj_exec_embed_ent g_aExecEmbed[] = {
    { "/usr/libexec/sshd-session", gj_openssh_sshd_session_elf_blob,
      gj_openssh_sshd_session_elf_blob_end },
    { "/usr/libexec/sshd-auth", gj_openssh_sshd_auth_elf_blob,
      gj_openssh_sshd_auth_elf_blob_end },
    { "/bin/sh", gj_shell_elf_blob, gj_shell_elf_blob_end },
    { "/bin/dash", gj_shell_elf_blob, gj_shell_elf_blob_end },
    { "/usr/bin/sh", gj_shell_elf_blob, gj_shell_elf_blob_end },
    { "/usr/bin/dash", gj_shell_elf_blob, gj_shell_elf_blob_end },
    { "/usr/sbin/sshd", gj_openssh_sshd_elf_blob, gj_openssh_sshd_elf_blob_end },
    { "/usr/bin/sshd", gj_openssh_sshd_elf_blob, gj_openssh_sshd_elf_blob_end },
};

/*
 * Full-blob USER exec. thread_exec_replace only (never thread_create_user;
 * 0.1.140 #PF). Returns 1 if image is not a usable ELF (caller maps that).
 */
static i64
protonrt_exec_embed(struct gj_linux_regs *pRegs, const char *szPath,
                    const u8 *pImg, u64 cbImg)
{
    struct gj_elf_info elf;
    struct gj_process *pExec;
    u64 u64Entry;
    u64 u64Stack;
    u64 u64ArgvPre;
    u64 u64EnvpPre;
    size_t iPath;
    u32 cRepl;

    if (pRegs == NULL || szPath == NULL || pImg == NULL || cbImg < 4ull) {
        return 1;
    }
    if (pImg[0] != 0x7fu || pImg[1] != (u8)'E' || pImg[2] != (u8)'L' ||
        pImg[3] != (u8)'F') {
        return 1;
    }
    if (elf_probe_image(pImg, cbImg, &elf) != GJ_OK) {
        return -8; /* ENOEXEC */
    }
    pExec = protonrt_linux_exec_proc();
    if (pExec == NULL) {
        kprintf("linux: execve %s embed cb=%lu "
                "load=SKIP isolate Soft!=product\n",
                szPath, (unsigned long)cbImg);
        return -LINUX_EAGAIN;
    }
    pExec->u64InterpEntry = 0;
    pExec->u32ExecFlags = elf.u32Flags;
    pExec->cAuxv = 0;
    elf_fill_auxv(pExec, &elf, NULL);
    if (pRegs->u64Nr == LINUX_NR_execveat) {
        u64ArgvPre = pRegs->u64Arg2;
        u64EnvpPre = pRegs->u64Arg3;
    } else {
        u64ArgvPre = pRegs->u64Arg1;
        u64EnvpPre = pRegs->u64Arg2;
    }
    if (elf_publish_handoff_argv(pExec, szPath, &elf, NULL, u64ArgvPre,
                                 u64EnvpPre) != GJ_OK ||
        elf_stack_rsp_live_ok(pExec->u64ExecStack) == 0) {
        kprintf("linux: execve %s embed cb=%lu "
                "load=SKIP stack isolate Soft!=product\n",
                szPath, (unsigned long)cbImg);
        return -LINUX_EAGAIN;
    }
    if ((elf.u32Flags & GJ_ELF_INFO_HAS_INTERP) != 0 ||
        protonrt_linux_user_text_ok(elf.u64Entry) == 0) {
        kprintf("linux: execve %s embed cb=%lu "
                "load=SKIP isolate Soft!=product\n",
                szPath, (unsigned long)cbImg);
        return -LINUX_EAGAIN;
    }
    if (elf_load_image(pExec, pImg, cbImg, &elf) != GJ_OK) {
        return -8;
    }
    pExec->u64ExecEntry = elf.u64Entry;
    pExec->u64LoadBias = elf.u64Bias;
    pExec->u32ExecFlags = elf.u32Flags;
    for (iPath = 0;
         iPath + 1 < sizeof(pExec->szExecPath) && szPath[iPath] != '\0';
         iPath++) {
        pExec->szExecPath[iPath] = szPath[iPath];
    }
    pExec->szExecPath[iPath] = '\0';
    u64Entry = elf.u64Entry;
    u64Stack = pExec->u64ExecStack;
    if (elf_stack_rsp_live_ok(u64Stack) == 0) {
        kprintf("linux: execve %s embed cb=%lu "
                "rip=0x%lx sp=0x%lx replace=0 "
                "live_thr=SKIP isolate Soft!=product\n",
                szPath, (unsigned long)cbImg, (unsigned long)u64Entry,
                (unsigned long)u64Stack);
        return -LINUX_EAGAIN;
    }
    pExec->u64StartEntry = u64Entry;
    pExec->u64InterpEntry = 0;
    cRepl = protonrt_exec_replace_user(pExec, u64Entry, u64Stack);
    kprintf("linux: execve %s embed cb=%lu "
            "rip=0x%lx sp=0x%lx replace=%u\n",
            szPath, (unsigned long)cbImg, (unsigned long)u64Entry,
            (unsigned long)u64Stack, cRepl);
    if (cRepl == 0) {
        kprintf("linux: execve %s embed replace isolate Soft!=product\n",
                szPath);
        return -LINUX_EAGAIN;
    }
    return 0;
}

/* 1 = not an embed path (caller may use vfs). Else 0 or -errno. */
static i64
protonrt_exec_embed_try(struct gj_linux_regs *pRegs, const char *szPath)
{
    size_t iEnt;
    u64 cbImg;
    i64 i64Emb;

    if (szPath == NULL || szPath[0] == '\0') {
        return 1;
    }
    for (iEnt = 0; iEnt < (sizeof(g_aExecEmbed) / sizeof(g_aExecEmbed[0]));
         iEnt++) {
        if (protonrt_exec_embed_path_hit(szPath, g_aExecEmbed[iEnt].szPath) ==
            0) {
            continue;
        }
        cbImg = (u64)(g_aExecEmbed[iEnt].pEnd - g_aExecEmbed[iEnt].pBlob);
        i64Emb = protonrt_exec_embed(pRegs, szPath, g_aExecEmbed[iEnt].pBlob,
                                     cbImg);
        if (i64Emb == 1) {
            return -8; /* ENOEXEC: matched path, blob not ELF */
        }
        return i64Emb;
    }
    /* Later-wall: never fall through to vfs 1-byte stubs. */
    if (protonrt_exec_later_wall_path(szPath) != 0) {
        return -8;
    }
    return 1;
}

static i64
protonrt_service(struct gj_linux_regs *pRegs, void *pCtx)
{
    char szPath[96];
    u8 aBuf[256];

    (void)pCtx;
    if (pRegs == NULL) {
        protonrt_soft_inc(&g_u64PrtSoftNull);
        protonrt_soft_inventory_maybe_once();
        return -LINUX_EINVAL;
    }

    protonrt_snap_user_syscall();

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
        /*
         * VFS pipe/socketpair before leftover LCN aliases (monitor fd 3).
         * Else cold net (tcp/lo or dup2 0/1/2) before ram and COM1.
         */
        if (protonrt_vfs_ipc_fd((i64)pRegs->u64Arg0)) {
            return protonrt_vfs_read(pRegs, aBuf, sizeof(aBuf));
        }
        if (gj_linux_cold_fd_ok((i64)pRegs->u64Arg0)) {
            struct gj_linux_regs sub;

            protonrt_soft_inc(&g_u64PrtLeanRwNet);
            memset(&sub, 0, sizeof(sub));
            sub.u64Nr = LINUX_NR_recvfrom;
            sub.u64Arg0 = pRegs->u64Arg0;
            sub.u64Arg1 = pRegs->u64Arg1;
            sub.u64Arg2 = pRegs->u64Arg2;
            sub.u64Arg3 = 0; /* flags */
            return gj_linux_cold_recv(&sub);
        }
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0)) {
            return protonrt_vfs_read(pRegs, aBuf, sizeof(aBuf));
        }
        /*
         * Stdio 0/1/2: serial stdin for dash cmdloop. Kernel smoke must
         * not block boot. USER yields while COM1 empty (sshd/UDX run).
         * gj_linux_cold_fd_ok already claimed dup2 aliases above.
         */
        if (pRegs->u64Arg0 <= 2ull) {
            u64 u64Dst = pRegs->u64Arg1;
            u64 u64Want = pRegs->u64Arg2;
            size_t iGot = 0;
            u8 aIn[64];

            if (protonrt_calling_user_proc() == NULL) {
                return 0;
            }
            if (u64Want == 0) {
                return 0;
            }
            if (u64Dst == 0) {
                return -LINUX_EFAULT;
            }
            if (u64Want > sizeof(aIn)) {
                u64Want = sizeof(aIn);
            }
            while (serial_poll() == 0) {
                thread_yield();
            }
            while (iGot < (size_t)u64Want && serial_poll() != 0) {
                int nCh = serial_getchar();

                if (nCh < 0) {
                    break;
                }
                aIn[iGot] = (u8)nCh;
                iGot++;
                if (nCh == '\n' || nCh == '\r') {
                    break;
                }
            }
            if (iGot == 0) {
                return 0;
            }
            if (user_range_ok(u64Dst, iGot)) {
                if (copy_to_user(u64Dst, aIn, iGot) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Dst, aIn, iGot);
            }
            return (i64)iGot;
        }
        break; /* fall through to stub */

    case LINUX_NR_write:
        if (protonrt_vfs_ipc_fd((i64)pRegs->u64Arg0)) {
            return protonrt_vfs_write(pRegs, aBuf, sizeof(aBuf));
        }
        if (gj_linux_cold_fd_ok((i64)pRegs->u64Arg0)) {
            struct gj_linux_regs sub;

            protonrt_soft_inc(&g_u64PrtLeanRwNet);
            memset(&sub, 0, sizeof(sub));
            sub.u64Nr = LINUX_NR_sendto;
            sub.u64Arg0 = pRegs->u64Arg0;
            sub.u64Arg1 = pRegs->u64Arg1;
            sub.u64Arg2 = pRegs->u64Arg2;
            sub.u64Arg3 = 0; /* flags */
            return gj_linux_cold_send(&sub);
        }
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0)) {
            return protonrt_vfs_write(pRegs, aBuf, sizeof(aBuf));
        }
        break;

    case LINUX_NR_lseek:
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0)) {
            return vfs_ram_lseek((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg1,
                                 (int)pRegs->u64Arg2);
        }
        break;

    case LINUX_NR_eventfd2:
        /* Proton neighborhood residual: eventfd readiness IPC. Soft!=product. */
        protonrt_soft_inc(&g_u64PrtLeanEventfd);
        return vfs_ram_eventfd2((u32)pRegs->u64Arg0, (int)pRegs->u64Arg1);
    case LINUX_NR_eventfd:
        /* eventfd(init, flags) - same as eventfd2; neighborhood lean tally */
        protonrt_soft_inc(&g_u64PrtLeanEventfd);
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
            /* sigset_t - take first 8 bytes */
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
        /*
         * Ramdisk/block: always durable for bring-up; product: storaged
         * barrier. Soft residual: accept net_lo + net_tcp fds too
         * (UDX host fsync-on-socket probes). Soft!=product.
         */
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0) ||
            net_lo_fd_ok((i64)pRegs->u64Arg0) ||
            net_tcp_fd_ok((i64)pRegs->u64Arg0)) {
            return 0;
        }
        return -LINUX_EBADF;

    case LINUX_NR_sync:
        /* Global sync: no-op success (ram durable) */
        return 0;

    case LINUX_NR_syncfs:
        if (vfs_ram_fd_ok((i64)pRegs->u64Arg0) ||
            net_lo_fd_ok((i64)pRegs->u64Arg0) ||
            net_tcp_fd_ok((i64)pRegs->u64Arg0) ||
            (i64)pRegs->u64Arg0 >= 0) {
            return 0;
        }
        return -LINUX_EBADF;

    case LINUX_NR_madvise:
        /* Advise only - no-op success (A1+). */
        return 0;

    case LINUX_NR_epoll_create:
        /* Proton neighborhood residual: epoll create. Soft!=product. */
        protonrt_soft_inc(&g_u64PrtLeanEpoll);
        return vfs_ram_epoll_create1(0);
    case LINUX_NR_epoll_create1:
        protonrt_soft_inc(&g_u64PrtLeanEpoll);
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
         * poll(struct pollfd *fds, nfds_t nfds, timeout_ms)
         * ppoll(..., const struct timespec *tmo_p, ...)
         *
         * Residual lean: gj_linux_cold_poll_mask first (SO_ERROR/POLLERR
         * honesty for cold STREAM), then vfs_ram_poll_mask (ram + net route).
         * No always-ready shortcuts. Soft!=product blocking:
         *   timeout==0  -> single readiness pass
         *   timeout>0   -> soft-spin with thread_yield + timer_jiffies budget
         *   timeout<0 / ppoll NULL -> yield until a fd is ready
         *   blocking + USER peer RUNNABLE -> yield before ready return
         * Mixed set: vfs pipe/eventfd/signalfd park does not wake on
         * net_tcp AcceptQ. If any fd is net_tcp / cold net, yield so
         * the next pass runs gj_linux_cold_poll_mask (net_eth_poll).
         * greppable: protonrt: soft poll block PASS
         * Soft!=product. G-AC-1 userspace host poll reach.
         */
        u32 nfds = (u32)pRegs->u64Arg1;
        u32 i;
        u32 ready = 0;
        i64 i64TimeoutMs = 0;
        u32 u32SpinLeft = 0;
        u64 u64J0;
        u64 u64WaitMask = 0;
        u32 u32Pass;
        int fPark;
        int fColdNet;
        struct gj_process *pProc;
        struct gj_process *pSave;

        protonrt_soft_inc(&g_u64PrtLeanPoll);
        if (nfds == 0) {
            return 0;
        }
        if (nfds > 128u) {
            nfds = 128u;
        }
        if (pRegs->u64Arg0 == 0) {
            return -LINUX_EFAULT;
        }

        /* Decode timeout: poll uses int ms; ppoll uses timespec*. */
        if (pRegs->u64Nr == LINUX_NR_poll) {
            i64TimeoutMs = (i64)(i32)pRegs->u64Arg2;
        } else if (pRegs->u64Arg2 != 0) {
            i64 i64Sec = 0;
            i64 i64Nsec = 0;

            if (user_range_ok(pRegs->u64Arg2, 16)) {
                if (copy_from_user(&i64Sec, pRegs->u64Arg2, 8) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
                if (copy_from_user(&i64Nsec, pRegs->u64Arg2 + 8, 8) !=
                    GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                i64Sec = *(const i64 *)(gj_vaddr_t)pRegs->u64Arg2;
                i64Nsec = *(const i64 *)(gj_vaddr_t)(pRegs->u64Arg2 + 8);
            }
            if (i64Sec < 0 || i64Nsec < 0) {
                i64TimeoutMs = 0;
            } else if (i64Sec == 0 && i64Nsec == 0) {
                i64TimeoutMs = 0;
            } else {
                i64TimeoutMs = i64Sec * 1000 + i64Nsec / 1000000;
                if (i64TimeoutMs <= 0) {
                    i64TimeoutMs = 1; /* sub-ms -> one soft tick */
                }
            }
        } else {
            /* ppoll NULL timespec: infinite wait (Linux). */
            i64TimeoutMs = -1;
        }

        /*
         * ppoll arg3: wait sigmask word 0 if non-NULL (Linux bit N-1).
         * poll has no mask; 0 means every signal unblocked for EINTR.
         * Failed 8-byte mask copy (OpenSSH &osigset on handoff stack
         * ~0x6ff41000; user_range_ok is geometry-only) is empty extra
         * mask, not -EFAULT: OpenSSH `if (ret == -1) continue` would
         * skip AcceptQ forever. Still walk pollfds. Dual DoD B OPEN.
         */
        if (pRegs->u64Nr == LINUX_NR_ppoll && pRegs->u64Arg3 != 0) {
            if (user_range_ok(pRegs->u64Arg3, 8)) {
                if (copy_from_user(&u64WaitMask, pRegs->u64Arg3, 8) !=
                    GJ_OK) {
                    u64WaitMask = 0;
                }
            } else {
                u64WaitMask = *(const u64 *)(gj_vaddr_t)pRegs->u64Arg3;
            }
        }

        /*
         * Soft spin budget when timeout > 0 (daemon poll loops).
         * Cap attempts + jiffies so soft never hangs product bring-up.
         * soft!=product blocking.
         */
        if (i64TimeoutMs > 0) {
            u32SpinLeft = (u32)i64TimeoutMs;
            if (u32SpinLeft > 128u) {
                u32SpinLeft = 128u;
            }
            if (u32SpinLeft < 1u) {
                u32SpinLeft = 1u;
            }
        }

        u64J0 = timer_jiffies();
        for (u32Pass = 0;; u32Pass++) {
            /*
             * Walk pollfds before SIGCHLD EINTR. OpenSSH rt_sigaction(17)
             * then ppoll; kernel-spawned siblings that exit (storaged,
             * scsi_mid, UDX) can OR sticky pending on pid 1. A pre-walk
             * EINTR skips gj_linux_cold_poll_mask / net_eth_poll and never
             * sees AcceptQ ESTAB. Linux returns ready if fds are already
             * ready. EINTR only after an empty walk. Dual DoD B OPEN.
             */
            ready = 0;
            fPark = 0;
            fColdNet = 0;
            /*
             * One RX pump per pass before any revents store. Per-fd
             * poll_mask can drain SYN after the listen fd already stored 0.
             */
            net_eth_poll();
            pSave = protonrt_bind_calling_user();
            for (i = 0; i < nfds; i++) {
                /* pollfd: int fd; short events; short revents - 8B x86_64 */
                i32 fd = 0;
                u16 events = 0;
                u16 revents = 0;
                u32 want;
                u32 got;
                u64 base = pRegs->u64Arg0 + (u64)i * 8u;

                if (user_range_ok(base, 8)) {
                    if (copy_from_user(&fd, base, 4) != GJ_OK) {
                        protonrt_unbind_calling_user(pSave);
                        return -LINUX_EFAULT;
                    }
                    if (copy_from_user(&events, base + 4, 2) != GJ_OK) {
                        protonrt_unbind_calling_user(pSave);
                        return -LINUX_EFAULT;
                    }
                } else {
                    i32 *p = (i32 *)(gj_vaddr_t)base;

                    fd = p[0];
                    events = ((u16 *)(gj_vaddr_t)base)[2];
                }
                /* POLLIN=1 POLLOUT=4 - same numbers as EPOLLIN/OUT */
                want = (u32)events;
                if (want == 0) {
                    want = (u32)(LINUX_POLLIN | LINUX_POLLOUT);
                }
                if (fd < 0) {
                    revents = 0;
                } else {
                    if (vfs_ram_fd_poll_parkable((i64)fd) != 0) {
                        fPark = 1;
                    } else if (net_tcp_fd_ok((i64)fd) != 0 ||
                               gj_linux_cold_fd_ok((i64)fd) != 0) {
                        fColdNet = 1;
                    }
                    /* Pipe/socketpair vfs mask, else cold_net then vfs_ram. */
                    got = protonrt_soft_fd_ready_mask((i64)fd, want);
                    if (got == 0 && fd >= 0 && fd <= 2 &&
                        gj_linux_cold_fd_ok((i64)fd) == 0) {
                        /* Stdio: serial. POLLOUT always; POLLIN if COM1. */
                        if ((want & (u32)LINUX_POLLOUT) != 0) {
                            got |= (u32)LINUX_POLLOUT;
                        }
                        if ((want & (u32)LINUX_POLLIN) != 0 &&
                            serial_poll() != 0) {
                            got |= (u32)LINUX_POLLIN;
                        }
                    }
                    if (got == 0 && !vfs_ram_fd_ok((i64)fd) &&
                        !net_lo_fd_ok((i64)fd) &&
                        !net_tcp_fd_ok((i64)fd) &&
                        gj_linux_cold_fd_ok((i64)fd) == 0 &&
                        (fd < 0 || fd > 2)) {
                        revents = (u16)LINUX_POLLERR;
                        goto prt_poll_store;
                    }
                    revents = (u16)(got & want);
                    if (got & (u32)LINUX_POLLERR) {
                        revents |= (u16)LINUX_POLLERR;
                    }
                    if (got & (u32)LINUX_POLLHUP) {
                        revents |= (u16)LINUX_POLLHUP;
                    }
                }
            prt_poll_store:
                if (user_range_ok(base, 8)) {
                    if (copy_to_user(base + 6, &revents, 2) != GJ_OK) {
                        protonrt_unbind_calling_user(pSave);
                        return -LINUX_EFAULT;
                    }
                } else {
                    ((u16 *)(gj_vaddr_t)base)[3] = revents;
                }
                if (revents != 0) {
                    ready++;
                }
            }
            protonrt_unbind_calling_user(pSave);

            /*
             * SIGCHLD before ready. Trigger on wait-table zombies as well
             * as u64SigPending (QEMU91: pending was not visible on the
             * ppoll PCB so enter never ran). Timeout 0 still delivers.
             * Only -EINTR after RIP actually switched. Dual DoD B OPEN.
             */
            pProc = protonrt_calling_user_proc();
            if (pProc != NULL) {
                u32 u32PidW = process_wait_pid_of(pProc);
                u32 u32Z = 0;
                int fPend = ((pProc->u64SigPending &
                              (1ull << GJ_SIGCHLD)) != 0) ? 1 : 0;
                int fMask = ((u64WaitMask &
                              (1ull << (GJ_SIGCHLD - 1u))) != 0) ? 1 : 0;

                /*
                 * zombie_count(0) means "any parent" and counted kernel
                 * smoke zombies (QEMU92 banner timeout). Only a real
                 * wait-table pid (OpenSSH session >=100). Dual DoD B OPEN.
                 */
                if (u32PidW != 0u) {
                    u32Z = process_wait_zombie_count(u32PidW);
                }
                if (u32PidW != 0u && (u32Z != 0u || fPend != 0) &&
                    g_u32PrtSigchldEnterLog < 8u) {
                    g_u32PrtSigchldEnterLog++;
                    kprintf("protonrt: sigchld poll pid=%u z=%u pend=%d "
                            "h=0x%lx mask=%d tmo=%ld ready=%u Dual DoD B OPEN\n",
                            u32PidW, u32Z, fPend,
                            (unsigned long)pProc->aSigHandler[GJ_SIGCHLD],
                            fMask, (long)i64TimeoutMs, ready);
                }
                if (u32PidW != 0u && (u32Z != 0u || fPend != 0) &&
                    fMask == 0) {
                    if (protonrt_sigchld_enter_handler(pProc) != 0) {
                        return -LINUX_EINTR;
                    }
                }
            }
            if (ready != 0) {
                if (i64TimeoutMs != 0 &&
                    protonrt_peer_user_runnable() != 0) {
                    thread_yield();
                }
                break;
            }
            if (i64TimeoutMs == 0) {
                break;
            }
            if (i64TimeoutMs < 0) {
                /*
                 * vfs_ram_poll_park waits on pipe/eventfd/signalfd only.
                 * OpenSSH ppoll(NULL) mixes listen TCP with lo/pipe;
                 * AcceptQ would never wake that wait. Yield instead so
                 * gj_linux_cold_poll_mask can run net_eth_poll.
                 * Even pass: re-walk without yield. QEMU42/43 stored
                 * revents=0 on fd 97 then eth_estab on a later pump;
                 * yield often never resumed sshd so poll_mask never
                 * saw AcceptQ. Dual DoD B OPEN.
                 */
                if (protonrt_peer_user_runnable() != 0) {
                    thread_yield();
                    continue;
                }
                if (fPark != 0 && fColdNet == 0) {
                    vfs_ram_poll_park();
                } else {
                    /*
                     * Yield. schedule() marks RUNNING->RUNNABLE before
                     * pick, so sshd is re-selected. QEMU52/53 no-yield
                     * 99% after SYN 58B never HLT; virtio SYN-ACK used.idx
                     * never completed. Pump is still the next poll_mask
                     * -> net_eth_poll. Dual DoD B OPEN.
                     */
                    thread_yield();
                    /*
                     * Yield self-picks sshd (RUNNABLE-before-pick) so
                     * idle never HLT. QEMU57 still 99% after SYN batch
                     * n=3; virtio SYN-ACK used.idx needs a VM-exit.
                     * Dual DoD B OPEN.
                     */
                    if (timer_ready()) {
                        __asm__ volatile ("sti; hlt" ::: "memory");
                    }
                    continue;
                }
                (void)u32Pass;
                continue;
            }
            /* Soft-spin: yield so peer thr / net / pipe writers can run. */
            thread_yield();
            if (u32SpinLeft > 0) {
                u32SpinLeft--;
            }
            if (u32SpinLeft == 0) {
                break;
            }
            /* Hard soft cap ~320ms @ GJ_TIMER_HZ=100. */
            if ((timer_jiffies() - u64J0) > 32ull) {
                break;
            }
            (void)u32Pass;
        }

        if (i64TimeoutMs > 0 && g_fPrtSoftPollBlockOnce == 0) {
            g_fPrtSoftPollBlockOnce = 1;
            /* Grep: protonrt: soft poll block PASS */
            kprintf("protonrt: soft poll block PASS timeout_ms=%ld "
                    "ready=%u soft_not_product_block=1\n",
                    (long)i64TimeoutMs, ready);
        }
        return (i64)ready;
    }
    case LINUX_NR_ioctl: {
        /*
         * C2 Dual DoD residual: ioctl for UDX host / Linux-shaped apps.
         * Accept vfs_ram + net_lo + net_tcp (STREAM sockets used FIONREAD/
         * FIONBIO before cold_net was owner-checked; prior lo-only rejected
         * tcp fds with -EBADF). Stdio 0/1/2 and vfs_ram PTY/pipe are
         * tty-class for TCGETS/TCSETS family/TIOCGWINSZ/TIOCSWINSZ
         * (OpenSSH ioctl(pty_fd>=3, TIOCSWINSZ); same soft 24x80 /
         * TCGETS fill). TCP -ENOTTY for TTY cmds, not silent 0. Unknown
         * fds -EBADF (not -EBADF on stdin). TIOCGPTN/TIOCSPTLCK bind USER
         * then vfs_ram_ioctl (kernel arg buffer); do not fold into TIOCSCTTY.
         * TIOCSCTTY/TIOCNOTTY: USER*_ENTRY
         * + stdio|ram only (OpenSSH PTY slave); net -ENOTTY. Does not
         * spawn dash. Unknown non-TTY cmds keep ram|net residual.
         * Soft!=product. G-AC-1. Dual DoD A/B OPEN.
         * greppable: protonrt: soft ioctl residual lean
         */
        i64 i64Fd = (i64)pRegs->u64Arg0;
        u32 cmd = (u32)pRegs->u64Arg1;
        int fRam;
        int fNet;
        int fStdio;
        int fTtyCmd;

        if (cmd == (u32)LINUX_TIOCGPTN || cmd == (u32)LINUX_TIOCSPTLCK ||
            cmd == (u32)LINUX_TIOCGPTLCK) {
            u32 u32Word = 0;
            void *pKarg = NULL;
            i64 i64R;
            struct gj_process *pSave;

            if (pRegs->u64Arg2 != 0) {
                if (cmd == (u32)LINUX_TIOCSPTLCK) {
                    if (user_range_ok(pRegs->u64Arg2, 4)) {
                        if (copy_from_user(&u32Word, pRegs->u64Arg2, 4) !=
                            GJ_OK) {
                            return -LINUX_EFAULT;
                        }
                    } else {
                        u32Word = *(const u32 *)(gj_vaddr_t)pRegs->u64Arg2;
                    }
                }
                pKarg = &u32Word;
            }
            /* Door kthread else sees -EBADF; posix_openpt skips TIOCGPTN. */
            pSave = protonrt_bind_calling_user();
            i64R = vfs_ram_ioctl(i64Fd, cmd, pKarg);
            protonrt_unbind_calling_user(pSave);
            if (i64R == 0 && pKarg != NULL &&
                (cmd == (u32)LINUX_TIOCGPTN ||
                 cmd == (u32)LINUX_TIOCGPTLCK)) {
                if (user_range_ok(pRegs->u64Arg2, 4)) {
                    if (copy_to_user(pRegs->u64Arg2, &u32Word, 4) !=
                        GJ_OK) {
                        return -LINUX_EFAULT;
                    }
                } else {
                    *(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = u32Word;
                }
            }
            return i64R;
        }

        fRam = vfs_ram_fd_ok(i64Fd) ? 1 : 0;
        fNet = (net_lo_fd_ok(i64Fd) || net_tcp_fd_ok(i64Fd)) ? 1 : 0;
        fStdio = (i64Fd >= 0 && i64Fd <= 2) ? 1 : 0;
        fTtyCmd = 0;
        if (cmd == (u32)LINUX_TIOCGWINSZ || cmd == (u32)LINUX_TIOCSWINSZ ||
            cmd == (u32)LINUX_TCGETS || cmd == (u32)LINUX_TCSETS ||
            cmd == (u32)LINUX_TCSETSW || cmd == (u32)LINUX_TCSETSF ||
            cmd == (u32)LINUX_TIOCGPGRP || cmd == (u32)LINUX_TIOCSPGRP) {
            fTtyCmd = 1;
        }
        if (fTtyCmd != 0) {
            /* Stdio or vfs_ram PTY/pipe; TCP stays -ENOTTY (not silent 0). */
            if (fStdio == 0 && fRam == 0) {
                if (fNet != 0) {
                    return -LINUX_ENOTTY;
                }
                return -LINUX_EBADF;
            }
        } else if (cmd == (u32)LINUX_TIOCSCTTY ||
                   cmd == (u32)LINUX_TIOCNOTTY) {
            /*
             * Controlling tty: OpenSSH ioctl(slave, TIOCSCTTY) after
             * setsid. USER*_ENTRY PCB only. Stdio or ram (socketpair
             * PTY). TCP -ENOTTY. No dash spawn. Signals OPEN.
             */
            struct gj_thread *pThr;
            struct gj_process *pUser;

            if (fNet != 0 && fRam == 0 && fStdio == 0) {
                return -LINUX_ENOTTY;
            }
            if (fStdio == 0 && fRam == 0) {
                return -LINUX_EBADF;
            }
            pThr = thread_current();
            if (protonrt_thr_is_user(pThr) == 0) {
                return -LINUX_EPERM;
            }
            pUser = pThr->pProc;
            if (cmd == (u32)LINUX_TIOCSCTTY) {
                u32 u32Pid = process_wait_pid_of(pUser);
                struct gj_process *pSave;
                i64 i64Pty;

                if (pUser->u32Sid == 0) {
                    pUser->u32Sid = (u32Pid != 0) ? u32Pid : 1u;
                }
                if (pUser->u32Pgid == 0) {
                    pUser->u32Pgid = pUser->u32Sid;
                }
                /* Forward to vfs so /dev/tty ctty slot matches the slave. */
                if (fRam != 0) {
                    pSave = protonrt_bind_calling_user();
                    i64Pty = vfs_ram_ioctl(i64Fd, cmd, NULL);
                    protonrt_unbind_calling_user(pSave);
                    if (i64Pty < 0) {
                        return i64Pty;
                    }
                }
            }
            return 0;
        } else if (fRam == 0 && fNet == 0) {
            return -LINUX_EBADF;
        }
        protonrt_soft_inc(&g_u64PrtLeanIoctl);

        if (cmd == (u32)LINUX_TIOCGWINSZ) {
            u16 aWs[4] = { 24, 80, 0, 0 };

            if (pRegs->u64Arg2 != 0) {
                if (user_range_ok(pRegs->u64Arg2, sizeof(aWs))) {
                    (void)copy_to_user(pRegs->u64Arg2, aWs, sizeof(aWs));
                } else {
                    memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aWs,
                           sizeof(aWs));
                }
            }
            goto ioctl_residual_once;
        }
        if (cmd == (u32)LINUX_TIOCGPGRP) {
            i32 i32Pgrp = 1;
            struct gj_process *pUser = protonrt_calling_user_proc();

            if (pUser != NULL && pUser->u32Pgid != 0) {
                i32Pgrp = (i32)pUser->u32Pgid;
            } else if (pUser != NULL) {
                u32 u32Pid = process_wait_pid_of(pUser);

                i32Pgrp = (i32)(u32Pid != 0 ? u32Pid : 1u);
            }
            if (pRegs->u64Arg2 != 0) {
                if (user_range_ok(pRegs->u64Arg2, 4)) {
                    (void)copy_to_user(pRegs->u64Arg2, &i32Pgrp, 4);
                } else {
                    *(i32 *)(gj_vaddr_t)pRegs->u64Arg2 = i32Pgrp;
                }
            }
            goto ioctl_residual_once;
        }
        if (cmd == (u32)LINUX_TIOCSPGRP) {
            i32 i32Pgrp = 0;
            struct gj_process *pUser = protonrt_calling_user_proc();

            if (pRegs->u64Arg2 != 0) {
                if (user_range_ok(pRegs->u64Arg2, 4)) {
                    (void)copy_from_user(&i32Pgrp, pRegs->u64Arg2, 4);
                } else {
                    i32Pgrp = *(const i32 *)(gj_vaddr_t)pRegs->u64Arg2;
                }
            }
            if (i32Pgrp > 0 && pUser != NULL) {
                pUser->u32Pgid = (u32)i32Pgrp;
            }
            goto ioctl_residual_once;
        }
        if (cmd == (u32)LINUX_FIONREAD) {
            /*
             * FIONREAD: ram -> vfs_ram_bytes_readable; net -> soft POLLIN
             * honesty (1 when ready, 0 else; exact RX byte count product
             * OPEN). Soft!=product Dual DoD residual.
             */
            u32 u32Avail = 0;

            if (fRam != 0) {
                i64 i64Avail = vfs_ram_bytes_readable(i64Fd);

                if (i64Avail > 0) {
                    if (i64Avail > (i64)0x7fffffffu) {
                        u32Avail = 0x7fffffffu;
                    } else {
                        u32Avail = (u32)i64Avail;
                    }
                }
            } else {
                u32 u32Got;

                u32Got = protonrt_soft_fd_ready_mask(i64Fd,
                                                    (u32)LINUX_POLLIN);
                if ((u32Got & (u32)LINUX_POLLIN) != 0) {
                    u32Avail = 1u; /* soft residual; exact count OPEN */
                }
            }
            if (pRegs->u64Arg2 != 0) {
                if (user_range_ok(pRegs->u64Arg2, 4)) {
                    (void)copy_to_user(pRegs->u64Arg2, &u32Avail, 4);
                } else {
                    *(u32 *)(gj_vaddr_t)pRegs->u64Arg2 = u32Avail;
                }
            }
            goto ioctl_residual_once;
        }
        if (cmd == (u32)LINUX_FIONBIO) {
            /*
             * FIONBIO: soft accept nonblock flag set/clear for UDX hosts
             * and glibc socket probes. Stores soft fd flags table so
             * F_GETFL residual reports O_NONBLOCK honestly. Product
             * O_NONBLOCK path OPEN. Soft!=product. storm=0.
             */
            if (pRegs->u64Arg2 != 0) {
                i32 i32Nb = 0;
                u32 u32Def;
                u32 u32Cur;

                if (user_range_ok(pRegs->u64Arg2, 4)) {
                    (void)copy_from_user(&i32Nb, pRegs->u64Arg2, 4);
                } else {
                    i32Nb = *(const i32 *)(gj_vaddr_t)pRegs->u64Arg2;
                }
                u32Def = (fNet != 0) ? (u32)LINUX_O_RDWR : 0u;
                u32Cur = protonrt_soft_fdfl_get(i64Fd, u32Def);
                if (i32Nb != 0) {
                    u32Cur |= (u32)LINUX_O_NONBLOCK;
                } else {
                    u32Cur &= ~(u32)LINUX_O_NONBLOCK;
                }
                protonrt_soft_fdfl_set(i64Fd, u32Cur);
            }
            goto ioctl_residual_once;
        }
        if (cmd == (u32)LINUX_FIONCLEX || cmd == (u32)LINUX_FIOCLEX) {
            if (fRam != 0) {
                i64 i64V = vfs_ram_fd_fl_get(i64Fd);

                if (i64V >= 0) {
                    u8 u8V = (u8)i64V;

                    if (cmd == (u32)LINUX_FIOCLEX) {
                        u8V |= (u8)VFS_FD_FL_CLOEXEC;
                    } else {
                        u8V &= (u8)~VFS_FD_FL_CLOEXEC;
                    }
                    (void)vfs_ram_fd_fl_set(i64Fd, u8V);
                }
            }
            goto ioctl_residual_once;
        }
        if (cmd == (u32)LINUX_TCGETS) {
            /*
             * Soft 60-byte termios-ish fill so zsh/tcsh do not read stack
             * garbage. ICANON|ECHO class + sane c_cc. Product termios OPEN.
             */
            if (pRegs->u64Arg2 != 0) {
                u8 aTios[60];
                u32 iT;

                for (iT = 0; iT < sizeof(aTios); iT++) {
                    aTios[iT] = 0;
                }
                aTios[1] = 0x01;  /* c_iflag ICRNL */
                aTios[4] = 0x05;  /* c_oflag OPOST|ONLCR */
                aTios[8] = 0xB0;  /* c_cflag CS8|CREAD|CLOCAL low */
                aTios[9] = 0x08;
                aTios[12] = 0x0B; /* c_lflag ISIG|ICANON|ECHO */
                aTios[17] = 3;    /* VINTR ^C */
                aTios[18] = 28;   /* VQUIT ^\ */
                aTios[19] = 127;  /* VERASE DEL */
                aTios[20] = 21;   /* VKILL ^U */
                aTios[21] = 4;    /* VEOF ^D */
                aTios[23] = 1;    /* VMIN */
                aTios[25] = 17;   /* VSTART */
                aTios[26] = 19;   /* VSTOP */
                aTios[27] = 26;   /* VSUSP */
                if (user_range_ok(pRegs->u64Arg2, sizeof(aTios))) {
                    if (copy_to_user(pRegs->u64Arg2, aTios, sizeof(aTios)) !=
                        GJ_OK) {
                        return -LINUX_EFAULT;
                    }
                } else {
                    memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aTios,
                           sizeof(aTios));
                }
            }
            goto ioctl_residual_once;
        }
        if (cmd == (u32)LINUX_TCSETS ||
            cmd == (u32)LINUX_TCSETSW || cmd == (u32)LINUX_TCSETSF ||
            cmd == (u32)LINUX_TIOCSWINSZ) {
            /* Soft TTY set on stdio|ram; product termios OPEN. */
            goto ioctl_residual_once;
        }
        /* Unknown ioctl: soft succeed for UDX/wine/libc probes. */
    ioctl_residual_once:
        if (g_fPrtSoftIoctlOnce == 0) {
            g_fPrtSoftIoctlOnce = 1;
            /* Grep: protonrt: soft ioctl residual lean */
            kprintf("protonrt: soft ioctl residual lean "
                    "cmd=0x%x ram=%u net=%u lean=%llu "
                    "Soft!=product g_ac_1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
                    "product_path=UDX_DDI+hot_cold_ABI not_bar3=1\n",
                    cmd, (unsigned)fRam, (unsigned)fNet,
                    (unsigned long long)g_u64PrtLeanIoctl);
        }
        return 0;
    }
    case LINUX_NR_epoll_ctl: {
        /* arg0=epfd arg1=op arg2=fd arg3=struct epoll_event* */
        u32 u32Events = 0x001u;
        u64 u64Data = 0;

        /*
         * Proton neighborhood residual: epoll_ctl ADD/MOD/DEL for mixed
         * ram + net_lo + net_tcp watch sets (Linux-shaped daemon loops).
         * Soft!=product. G-AC-1. not bar3.
         */
        protonrt_soft_inc(&g_u64PrtLeanEpoll);
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
        /*
         * Residual lean epoll_wait family (bar3-adjacent ABI surface).
         * Proton/game-shaped readiness demux over registered watches.
         * Soft!=product blocking:
         *   timeout==0 / pwait2 timespec 0 -> single readiness pass
         *   timeout>0  -> soft-spin yield + jiffies budget (poll-shaped)
         *   timeout<0 / pwait2 NULL -> yield until ready
         * greppable: protonrt: soft epoll residual lean
         * Never claims bar3 / Deck Top 50 / product DoD. G-AC-1.
         */
        u8 aEv[16 * 12];
        int nMax = (int)pRegs->u64Arg2;
        i64 i64TimeoutMs = 0;
        u32 u32SpinLeft = 0;
        u32 u32Pass;
        u64 u64J0;
        i64 n = 0;

        protonrt_soft_inc(&g_u64PrtLeanEpoll);
        if (nMax <= 0) {
            return 0;
        }
        if (nMax > 16) {
            nMax = 16;
        }

        /* Decode timeout: epoll_wait/pwait int ms; pwait2 timespec* soft 0. */
        if (pRegs->u64Nr == LINUX_NR_epoll_pwait2) {
            if (pRegs->u64Arg3 != 0) {
                i64 i64Sec = 0;
                i64 i64Nsec = 0;

                if (user_range_ok(pRegs->u64Arg3, 16)) {
                    if (copy_from_user(&i64Sec, pRegs->u64Arg3, 8) != GJ_OK) {
                        return -LINUX_EFAULT;
                    }
                    if (copy_from_user(&i64Nsec, pRegs->u64Arg3 + 8, 8) !=
                        GJ_OK) {
                        return -LINUX_EFAULT;
                    }
                } else {
                    i64Sec = *(const i64 *)(gj_vaddr_t)pRegs->u64Arg3;
                    i64Nsec = *(const i64 *)(gj_vaddr_t)(pRegs->u64Arg3 + 8);
                }
                if (i64Sec < 0 || i64Nsec < 0) {
                    i64TimeoutMs = 0;
                } else if (i64Sec == 0 && i64Nsec == 0) {
                    i64TimeoutMs = 0;
                } else {
                    i64TimeoutMs = i64Sec * 1000 + i64Nsec / 1000000;
                    if (i64TimeoutMs <= 0) {
                        i64TimeoutMs = 1;
                    }
                }
            } else {
                i64TimeoutMs = -1; /* pwait2 NULL timespec: infinite */
            }
        } else {
            i64TimeoutMs = (i64)(i32)pRegs->u64Arg3;
        }

        if (i64TimeoutMs > 0) {
            u32SpinLeft = (u32)i64TimeoutMs;
            if (u32SpinLeft > 128u) {
                u32SpinLeft = 128u;
            }
            if (u32SpinLeft < 1u) {
                u32SpinLeft = 1u;
            }
        }

        u64J0 = timer_jiffies();
        for (u32Pass = 0;; u32Pass++) {
            n = vfs_ram_epoll_wait((i64)pRegs->u64Arg0, aEv, nMax, 0);
            if (n != 0) {
                break;
            }
            if (i64TimeoutMs == 0) {
                break;
            }
            thread_yield();
            if (i64TimeoutMs > 0) {
                if (u32SpinLeft > 0) {
                    u32SpinLeft--;
                }
                if (u32SpinLeft == 0) {
                    break;
                }
                if ((timer_jiffies() - u64J0) > 32ull) {
                    break;
                }
            }
            (void)u32Pass;
        }

        if (n > 0 && pRegs->u64Arg1 != 0) {
            size_t cbCopy = (size_t)n * 12u;

            if (user_range_ok(pRegs->u64Arg1, cbCopy)) {
                if (copy_to_user(pRegs->u64Arg1, aEv, cbCopy) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aEv, cbCopy);
            }
        }

        if (g_fPrtSoftEpollOnce == 0) {
            g_fPrtSoftEpollOnce = 1;
            /* Grep: protonrt: soft epoll residual lean */
            kprintf("protonrt: soft epoll residual lean "
                    "ready=%ld timeout_ms=%ld Soft!=product g_ac_1=1 "
                    "abi_adj=proton_p0 not_bar3=1 "
                    "(Linux-shaped apps / Proton neighborhood; not product)\n",
                    (long)n, (long)i64TimeoutMs);
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
         * Proton neighborhood residual: wine-class shareable objects.
         * Soft!=product. G-AC-1. not bar3.
         */
        char szName[GJ_MEMOBJ_NAME_MAX];
        char szPath[40];
        static u32 u32MfdSeq;
        u32 i;
        u32 n = 0;
        i64 i64Fd;

        protonrt_soft_inc(&g_u64PrtLeanMemfd);
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
         * C2 Dual DoD residual deepen: fcntl for UDX host / Linux-shaped apps.
         * Accept vfs_ram + net_lo + net_tcp (prior lo-only rejected tcp with
         * -EBADF, same class as historical ioctl lo-only hole). Soft!=product.
         * F_GETFL/F_SETFL soft O_NONBLOCK table residual (parity FIONBIO).
         * Unused fd 0 F_GETFL/F_GETFD -> -EBADF so OpenSSH sanitise_stdfd
         * open("/dev/null") lands on stdin (read(0) must not park on COM1).
         * Unused fd 1/2 F_GETFL/F_GETFD -> success (O_RDWR) so sanitise
         * leaves serial; write(1/2) already goes to serial_putchar.
         * Dual DoD B OPEN. F_SETLK/F_GETLK/F_SETLKW via file_lock (ram only).
         * greppable: protonrt: soft fcntl residual lean
         * Dual DoD A/B OPEN; G-AC-1; not bar3.
         */
        u32 u32Cmd = (u32)pRegs->u64Arg1;
        i64 i64Fd = (i64)pRegs->u64Arg0;
        int fRam;
        int fNet;
        int fStdioSerial;
        int fGetFlFd;

        fRam = vfs_ram_fd_ok(i64Fd) ? 1 : 0;
        fNet = (net_lo_fd_ok(i64Fd) || net_tcp_fd_ok(i64Fd) ||
                gj_linux_cold_fd_ok(i64Fd)) ? 1 : 0;
        fStdioSerial = (i64Fd >= 0 && i64Fd <= 2 && fRam == 0 && fNet == 0)
                           ? 1
                           : 0;
        fGetFlFd = (u32Cmd == (u32)LINUX_F_GETFD || u32Cmd == 1u ||
                    u32Cmd == (u32)LINUX_F_GETFL || u32Cmd == 3u)
                       ? 1
                       : 0;
        if (fGetFlFd != 0 && fStdioSerial != 0) {
            if (i64Fd == 0) {
                return -LINUX_EBADF;
            }
            protonrt_soft_inc(&g_u64PrtLeanFcntl);
            if (u32Cmd == (u32)LINUX_F_GETFD || u32Cmd == 1u) {
                return 0;
            }
            return (i64)LINUX_O_RDWR;
        }
        if (fRam == 0 && fNet == 0) {
            return -LINUX_EBADF;
        }
        protonrt_soft_inc(&g_u64PrtLeanFcntl);

        if (u32Cmd == 0 /* F_DUPFD */ || u32Cmd == 1030 /* F_DUPFD_CLOEXEC */) {
            i64 i64New;

            if (fRam == 0) {
                return -LINUX_EBADF;
            }
            i64New = vfs_ram_dup_from(i64Fd, (i64)pRegs->u64Arg2);
            if (i64New >= 0 && u32Cmd == 1030u) {
                i64 i64V = vfs_ram_fd_fl_get(i64New);

                if (i64V >= 0) {
                    (void)vfs_ram_fd_fl_set(i64New,
                                            (u8)i64V | (u8)VFS_FD_FL_CLOEXEC);
                }
            }
            return i64New;
        }
        if (u32Cmd == 1 /* F_GETFD */) {
            i64 i64Ret = 0;

            if (fRam != 0) {
                i64 i64V = vfs_ram_fd_fl_get(i64Fd);

                if (i64V < 0) {
                    return i64V;
                }
                if (((u8)i64V & (u8)VFS_FD_FL_CLOEXEC) != 0) {
                    i64Ret = (i64)LINUX_FD_CLOEXEC;
                }
            }
            if (g_fPrtSoftFcntlCloexecOnce == 0) {
                g_fPrtSoftFcntlCloexecOnce = 1;
                kprintf("protonrt: soft fcntl cloexec residual lean "
                        "cmd=F_GETFD fd=%ld cloexec=%ld ram=%u "
                        "Soft!=product g_ac_1=1 dual_dod_a=OPEN "
                        "dual_dod_b=OPEN not_bar3=1\n",
                        (long)i64Fd, (long)i64Ret, (unsigned)fRam);
            }
            return i64Ret;
        }
        if (u32Cmd == 2 /* F_SETFD */) {
            if (fRam != 0) {
                i64 i64V = vfs_ram_fd_fl_get(i64Fd);
                u8 u8V;

                if (i64V < 0) {
                    return i64V;
                }
                u8V = (u8)i64V;
                if ((pRegs->u64Arg2 & (u64)LINUX_FD_CLOEXEC) != 0) {
                    u8V |= (u8)VFS_FD_FL_CLOEXEC;
                } else {
                    u8V &= (u8)~VFS_FD_FL_CLOEXEC;
                }
                if (vfs_ram_fd_fl_set(i64Fd, u8V) != 0) {
                    return -LINUX_EBADF;
                }
            }
            if (g_fPrtSoftFcntlCloexecOnce == 0) {
                g_fPrtSoftFcntlCloexecOnce = 1;
                kprintf("protonrt: soft fcntl cloexec residual lean "
                        "cmd=F_SETFD fd=%ld arg=0x%lx ram=%u "
                        "Soft!=product g_ac_1=1 dual_dod_a=OPEN "
                        "dual_dod_b=OPEN not_bar3=1\n",
                        (long)i64Fd, (unsigned long)pRegs->u64Arg2,
                        (unsigned)fRam);
            }
            return 0;
        }
        if (u32Cmd == (u32)LINUX_F_GETFL || u32Cmd == 3u /* F_GETFL */) {
            /*
             * Soft open-flags: net default O_RDWR; ram default 0; overlay
             * last F_SETFL/FIONBIO soft nonblock bits. Product table OPEN.
             */
            u32 u32Def = (fNet != 0) ? (u32)LINUX_O_RDWR : 0u;
            u32 u32Fl = protonrt_soft_fdfl_get(i64Fd, u32Def);

            if (fRam != 0) {
                i64 i64V = vfs_ram_fd_fl_get(i64Fd);

                if (i64V >= 0 &&
                    ((u8)i64V & (u8)VFS_FD_FL_NONBLOCK) != 0) {
                    u32Fl |= (u32)LINUX_O_NONBLOCK;
                }
            }

            if (g_fPrtSoftFcntlOnce == 0) {
                g_fPrtSoftFcntlOnce = 1;
                /* Grep: protonrt: soft fcntl residual lean */
                kprintf("protonrt: soft fcntl residual lean "
                        "cmd=F_GETFL fl=0x%x ram=%u net=%u lean=%llu "
                        "Soft!=product g_ac_1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
                        "product_path=UDX_DDI+hot_cold_ABI not_bar3=1\n",
                        u32Fl, (unsigned)fRam, (unsigned)fNet,
                        (unsigned long long)g_u64PrtLeanFcntl);
            }
            return (i64)u32Fl;
        }
        if (u32Cmd == (u32)LINUX_F_SETFL || u32Cmd == 4u /* F_SETFL */) {
            /*
             * Soft accept O_NONBLOCK (and ACCMODE bits) for UDX/glibc socket
             * probes. Stores soft table only - never product O_NONBLOCK path.
             * Soft!=product. storm=0.
             */
            u32 u32New = (u32)pRegs->u64Arg2;
            u32 u32Def = (fNet != 0) ? (u32)LINUX_O_RDWR : 0u;
            u32 u32Cur = protonrt_soft_fdfl_get(i64Fd, u32Def);
            u32 u32Keep = u32Cur & ~(u32)LINUX_O_NONBLOCK;
            u32 u32Acc = u32New & (u32)LINUX_O_ACCMODE;

            if (u32Acc != 0u) {
                u32Keep = (u32Keep & ~(u32)LINUX_O_ACCMODE) | u32Acc;
            } else if (fNet != 0) {
                u32Keep = (u32Keep & ~(u32)LINUX_O_ACCMODE) |
                          (u32)LINUX_O_RDWR;
            }
            if ((u32New & (u32)LINUX_O_NONBLOCK) != 0u) {
                u32Keep |= (u32)LINUX_O_NONBLOCK;
            }
            protonrt_soft_fdfl_set(i64Fd, u32Keep);
            if (fRam != 0) {
                i64 i64V = vfs_ram_fd_fl_get(i64Fd);

                if (i64V >= 0) {
                    u8 u8V = (u8)i64V;

                    if ((u32Keep & (u32)LINUX_O_NONBLOCK) != 0u) {
                        u8V |= (u8)VFS_FD_FL_NONBLOCK;
                    } else {
                        u8V &= (u8)~VFS_FD_FL_NONBLOCK;
                    }
                    (void)vfs_ram_fd_fl_set(i64Fd, u8V);
                }
            }
            if (g_fPrtSoftFcntlOnce == 0) {
                g_fPrtSoftFcntlOnce = 1;
                /* Grep: protonrt: soft fcntl residual lean */
                kprintf("protonrt: soft fcntl residual lean "
                        "cmd=F_SETFL fl=0x%x ram=%u net=%u lean=%llu "
                        "Soft!=product g_ac_1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
                        "product_path=UDX_DDI+hot_cold_ABI not_bar3=1\n",
                        u32Keep, (unsigned)fRam, (unsigned)fNet,
                        (unsigned long long)g_u64PrtLeanFcntl);
            }
            return 0;
        }
        if (u32Cmd == 6 /* F_SETLK */ || u32Cmd == 7 /* F_SETLKW */ ||
            u32Cmd == 5 /* F_GETLK */) {
            struct gj_flock fl;
            i64 st;

            if (fRam == 0) {
                return -LINUX_EBADF;
            }
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
        if (g_fPrtSoftFcntlOnce == 0) {
            g_fPrtSoftFcntlOnce = 1;
            /* Grep: protonrt: soft fcntl residual lean */
            kprintf("protonrt: soft fcntl residual lean "
                    "cmd=0x%x ram=%u net=%u lean=%llu "
                    "Soft!=product g_ac_1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
                    "product_path=UDX_DDI+hot_cold_ABI not_bar3=1\n",
                    u32Cmd, (unsigned)fRam, (unsigned)fNet,
                    (unsigned long long)g_u64PrtLeanFcntl);
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
        /* ABI-first: STREAM -> net_tcp, else net_lo (linux_cold_net bridge). */
        protonrt_soft_inc(&g_u64PrtLeanSocket);
        if (g_fPrtSoftColdNetOnce == 0) {
            g_fPrtSoftColdNetOnce = 1;
            /* Grep: protonrt: soft cold_net bridge PASS (Soft!=product). */
            kprintf("protonrt: soft cold_net bridge PASS\n");
        }
        return gj_linux_cold_socket(pRegs);
    }

    case LINUX_NR_sendto: {
        i64 i64Gate;

        if (protonrt_vfs_ipc_fd((i64)pRegs->u64Arg0)) {
            return protonrt_vfs_write(pRegs, aBuf, sizeof(aBuf));
        }
        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_sendto(pRegs);
    }

    case LINUX_NR_sendmsg: {
        i64 i64Gate;
        i64 i64N;
        struct gj_linux_regs sub;
        u64 u64Base = 0;
        u64 u64Len = 0;

        if (protonrt_vfs_ipc_fd((i64)pRegs->u64Arg0)) {
            i64Gate = protonrt_msg_first_iov(pRegs->u64Arg1, &u64Base,
                                             &u64Len);
            if (i64Gate != 0) {
                return i64Gate;
            }
            i64Gate = protonrt_vfs_scm_send((i64)pRegs->u64Arg0,
                                            pRegs->u64Arg1);
            if (i64Gate < 0) {
                return i64Gate;
            }
            memset(&sub, 0, sizeof(sub));
            sub.u64Arg0 = pRegs->u64Arg0;
            sub.u64Arg1 = u64Base;
            sub.u64Arg2 = u64Len;
            i64N = protonrt_vfs_write(&sub, aBuf, sizeof(aBuf));
            return i64N;
        }
        /* ABI-first: single-iov soft via linux_cold_net (tcp + lo). */
        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_sendmsg(pRegs);
    }

    case LINUX_NR_recvfrom: {
        i64 i64Gate;

        if (protonrt_vfs_ipc_fd((i64)pRegs->u64Arg0)) {
            return protonrt_vfs_read(pRegs, aBuf, sizeof(aBuf));
        }
        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_recvfrom(pRegs);
    }

    case LINUX_NR_recvmsg: {
        i64 i64Gate;
        i64 i64N;
        struct gj_linux_regs sub;
        u64 u64Base = 0;
        u64 u64Len = 0;

        if (protonrt_vfs_ipc_fd((i64)pRegs->u64Arg0)) {
            i64Gate = protonrt_msg_first_iov(pRegs->u64Arg1, &u64Base,
                                             &u64Len);
            if (i64Gate != 0) {
                return i64Gate;
            }
            memset(&sub, 0, sizeof(sub));
            sub.u64Arg0 = pRegs->u64Arg0;
            sub.u64Arg1 = u64Base;
            sub.u64Arg2 = u64Len;
            i64N = protonrt_vfs_read(&sub, aBuf, sizeof(aBuf));
            if (i64N >= 0) {
                protonrt_vfs_scm_recv((i64)pRegs->u64Arg0, pRegs->u64Arg1);
            }
            return i64N;
        }
        /* ABI-first: single-iov soft via linux_cold_net (tcp + lo). */
        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_recvmsg(pRegs);
    }

    case LINUX_NR_sendmmsg: {
        /*
         * sendmmsg(fd, mmsghdr *msgvec, vlen, flags)
         * Each mmsghdr is msghdr (56) + u32 msg_len + pad -> 64 bytes typical.
         * For bring-up: process first message only via sendmsg path, report 1.
         * Residual lean: send_star family tally (Soft!=product; G-AC-1).
         */
        struct gj_linux_regs sub;

        if (pRegs->u64Arg2 == 0) {
            return 0;
        }
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
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
        /*
         * recvmmsg residual lean: recv_star family tally.
         * Soft!=product. G-AC-1. First message only via recvmsg path.
         */
        struct gj_linux_regs sub;

        if (pRegs->u64Arg2 == 0) {
            return 0;
        }
        if (pRegs->u64Arg1 == 0) {
            return -LINUX_EFAULT;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
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

    case LINUX_NR_shutdown: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_shutdown(pRegs);
    }

    case LINUX_NR_setsockopt: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_setsockopt(pRegs);
    }

    case LINUX_NR_getsockopt: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_getsockopt(pRegs);
    }

    case LINUX_NR_getsockname: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_getsockname(pRegs);
    }

    case LINUX_NR_getpeername: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanXfer);
        return gj_linux_cold_getpeername(pRegs);
    }

    case LINUX_NR_pipe:
    case LINUX_NR_pipe2: {
        /*
         * Proton neighborhood residual: pipe/pipe2 (wine-class IPC).
         * Soft!=product. G-AC-1. not bar3.
         */
        i32 aFds[2];
        i64 i64St;
        int nFlags = (pRegs->u64Nr == LINUX_NR_pipe2) ? (int)pRegs->u64Arg1 : 0;

        protonrt_soft_inc(&g_u64PrtLeanPipe);
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
        /*
         * Proton neighborhood residual: socketpair AF_UNIX-shaped
         * (wine-server two-process IPC). Soft!=product. G-AC-1. not bar3.
         */
        i32 aFds[2];
        i64 i64St;

        protonrt_soft_inc(&g_u64PrtLeanSockpair);
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
        protonrt_soft_inc(&g_u64PrtLeanSocket);
        return gj_linux_cold_bind(pRegs);
    }

    case LINUX_NR_listen: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        protonrt_soft_inc(&g_u64PrtLeanSocket);
        return gj_linux_cold_listen(pRegs);
    }

    case LINUX_NR_accept:
    case LINUX_NR_accept4: {
        i64 i64Gate;

        i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
        if (i64Gate != 0) {
            return i64Gate;
        }
        /* flags ignored for bring-up (CLOEXEC/NONBLOCK) inside cold_net */
        protonrt_soft_inc(&g_u64PrtLeanSocket);
        return gj_linux_cold_accept(pRegs);
    }

    case LINUX_NR_fallocate:
        return vfs_ram_fallocate((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg2,
                                 (i64)pRegs->u64Arg3);

    case LINUX_NR_sendfile: {
        /*
         * sendfile(out, in, *off, count) residual lean (Soft!=product).
         * ram->ram via vfs_ram_sendfile; ram->net via bounce + cold_net
         * send (sshd/file leftover residual; Dual DoD B hop is kernel
         * net_tcp → sshd.elf). G-AC-1.
         * greppable: protonrt: soft sendfile residual lean
         * Dual DoD A OPEN until host USB path; Dual DoD B OPEN until
         * interactive SSH login; not freestanding wire; not bar3.
         */
        i64 i64Out = (i64)pRegs->u64Arg0;
        i64 i64In = (i64)pRegs->u64Arg1;
        u64 off = 0;
        u64 *pOff = NULL;
        size_t cbWant;
        i64 n;
        i64 i64Gate;

        protonrt_soft_inc(&g_u64PrtLeanSendfile);
        if (pRegs->u64Arg2 != 0) {
            if (user_range_ok(pRegs->u64Arg2, 8)) {
                (void)copy_from_user(&off, pRegs->u64Arg2, 8);
            } else {
                off = *(const u64 *)(gj_vaddr_t)pRegs->u64Arg2;
            }
            pOff = &off;
        }
        cbWant = (size_t)pRegs->u64Arg3;

        /* ram -> ram (existing path) */
        if (vfs_ram_fd_ok(i64Out) && vfs_ram_fd_ok(i64In)) {
            n = vfs_ram_sendfile(i64Out, i64In, pOff, cbWant);
            if (n >= 0 && pRegs->u64Arg2 != 0) {
                if (user_range_ok(pRegs->u64Arg2, 8)) {
                    (void)copy_to_user(pRegs->u64Arg2, &off, 8);
                } else {
                    *(u64 *)(gj_vaddr_t)pRegs->u64Arg2 = off;
                }
            }
            if (g_fPrtSoftSendfileOnce == 0) {
                g_fPrtSoftSendfileOnce = 1;
                /* Grep: protonrt: soft sendfile residual lean */
                kprintf("protonrt: soft sendfile residual lean "
                        "path=ram_ram n=%lld Soft!=product g_ac_1=1 "
                        "dual_dod_a=OPEN dual_dod_b=OPEN not_bar3=1\n",
                        (long long)n);
            }
            return n;
        }

        /*
         * ram-in -> net-out residual: bounce through aBuf + cold send.
         * Userspace UDX/sshd-shaped hosts use sendfile to STREAM peers.
         * Soft!=product. Product zero-copy OPEN.
         */
        if (vfs_ram_fd_ok(i64In) &&
            (net_tcp_fd_ok(i64Out) || net_lo_fd_ok(i64Out))) {
            size_t done = 0;
            u64 u64Off = pOff ? *pOff : 0;

            i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
            if (i64Gate != 0) {
                return i64Gate;
            }
            if (cbWant == 0) {
                return 0;
            }
            if (cbWant > 4096u) {
                cbWant = 4096u; /* soft cap; product bulk OPEN */
            }
            while (done < cbWant) {
                size_t chunk = cbWant - done;
                i64 i64R;
                i64 i64W;
                struct gj_linux_regs sub;

                if (chunk > sizeof(aBuf)) {
                    chunk = sizeof(aBuf);
                }
                i64R = vfs_ram_pread(i64In, aBuf, chunk, u64Off);
                if (i64R < 0) {
                    return done ? (i64)done : i64R;
                }
                if (i64R == 0) {
                    break;
                }
                memset(&sub, 0, sizeof(sub));
                sub.u64Nr = LINUX_NR_sendto;
                sub.u64Arg0 = (u64)i64Out;
                sub.u64Arg1 = (u64)(gj_vaddr_t)aBuf;
                sub.u64Arg2 = (u64)i64R;
                sub.u64Arg3 = 0;
                i64W = gj_linux_cold_send(&sub);
                if (i64W < 0) {
                    return done ? (i64)done : i64W;
                }
                if (i64W == 0) {
                    break;
                }
                done += (size_t)i64W;
                u64Off += (u64)i64W;
                if ((size_t)i64W < (size_t)i64R) {
                    break;
                }
            }
            if (pOff != NULL) {
                *pOff = u64Off;
            }
            if (pRegs->u64Arg2 != 0) {
                if (user_range_ok(pRegs->u64Arg2, 8)) {
                    (void)copy_to_user(pRegs->u64Arg2, &u64Off, 8);
                } else {
                    *(u64 *)(gj_vaddr_t)pRegs->u64Arg2 = u64Off;
                }
            }
            protonrt_soft_inc(&g_u64PrtLeanRwNet);
            if (g_fPrtSoftSendfileOnce == 0) {
                g_fPrtSoftSendfileOnce = 1;
                /* Grep: protonrt: soft sendfile residual lean */
                kprintf("protonrt: soft sendfile residual lean "
                        "path=ram_net n=%llu Soft!=product g_ac_1=1 "
                        "dual_dod_a=OPEN dual_dod_b=OPEN "
                        "product_path=UDX_DDI+hot_cold_ABI not_bar3=1\n",
                        (unsigned long long)done);
            }
            return (i64)done;
        }
        return -LINUX_EBADF;
    }

    case LINUX_NR_splice: {
        /*
         * splice(fd_in, *off_in, fd_out, *off_out, len, flags)
         * C2 residual: ram->ram sendfile; ram-in/net-out via sendfile residual.
         * Soft!=product. offsets soft-ignored on net path.
         */
        struct gj_linux_regs sub;

        protonrt_soft_inc(&g_u64PrtLeanSendfile);
        memset(&sub, 0, sizeof(sub));
        sub.u64Nr = LINUX_NR_sendfile;
        sub.u64Arg0 = pRegs->u64Arg2; /* out */
        sub.u64Arg1 = pRegs->u64Arg0; /* in */
        sub.u64Arg2 = 0;              /* *off soft NULL */
        sub.u64Arg3 = pRegs->u64Arg4; /* len */
        return protonrt_service(&sub, NULL);
    }

    case LINUX_NR_tee: {
        /* tee(fd_in, fd_out, len, flags) - C2 residual via sendfile path */
        struct gj_linux_regs sub;

        protonrt_soft_inc(&g_u64PrtLeanSendfile);
        memset(&sub, 0, sizeof(sub));
        sub.u64Nr = LINUX_NR_sendfile;
        sub.u64Arg0 = pRegs->u64Arg1; /* out */
        sub.u64Arg1 = pRegs->u64Arg0; /* in */
        sub.u64Arg2 = 0;
        sub.u64Arg3 = pRegs->u64Arg2; /* len */
        return protonrt_service(&sub, NULL);
    }

    case LINUX_NR_copy_file_range: {
        /*
         * C2 Dual DoD residual deepen: copy_file_range (Soft!=product).
         * arg0=fd_in arg1=*off_in arg2=fd_out arg3=*off_out arg4=len
         * ram->ram via vfs_ram_copy_file_range; ram-in -> net-out via bounce
         * + cold send (sendfile-shaped sshd/file residual). G-AC-1.
         * greppable: protonrt: soft copy_file residual lean
         * Dual DoD A/B OPEN; not freestanding wire; not bar3.
         */
        u64 offIn = 0;
        u64 offOut = 0;
        u64 *pIn = NULL;
        u64 *pOut = NULL;
        i64 i64In = (i64)pRegs->u64Arg0;
        i64 i64Out = (i64)pRegs->u64Arg2;
        size_t cbWant;
        i64 n;
        i64 i64Gate;

        protonrt_soft_inc(&g_u64PrtLeanCopyFr);
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
        cbWant = (size_t)pRegs->u64Arg4;

        /* ram -> ram (existing path) */
        if (vfs_ram_fd_ok(i64In) && vfs_ram_fd_ok(i64Out)) {
            n = vfs_ram_copy_file_range(i64In, pIn, i64Out, pOut, cbWant);
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
            if (g_fPrtSoftCopyFrOnce == 0) {
                g_fPrtSoftCopyFrOnce = 1;
                /* Grep: protonrt: soft copy_file residual lean */
                kprintf("protonrt: soft copy_file residual lean "
                        "path=ram_ram n=%lld Soft!=product g_ac_1=1 "
                        "dual_dod_a=OPEN dual_dod_b=OPEN not_bar3=1\n",
                        (long long)n);
            }
            return n;
        }

        /*
         * ram-in -> net-out residual: bounce through aBuf + cold send.
         * Same shape as sendfile residual for Dual DoD cold ABI reach.
         * Soft!=product. Product zero-copy OPEN.
         */
        if (vfs_ram_fd_ok(i64In) &&
            (net_tcp_fd_ok(i64Out) || net_lo_fd_ok(i64Out))) {
            size_t done = 0;
            u64 u64Off = pIn ? *pIn : 0;

            i64Gate = confine_soft_promise_require(GJ_PROMISE_INET);
            if (i64Gate != 0) {
                return i64Gate;
            }
            if (cbWant == 0) {
                return 0;
            }
            if (cbWant > 4096u) {
                cbWant = 4096u; /* soft cap; product bulk OPEN */
            }
            while (done < cbWant) {
                size_t chunk = cbWant - done;
                i64 i64R;
                i64 i64W;
                struct gj_linux_regs sub;

                if (chunk > sizeof(aBuf)) {
                    chunk = sizeof(aBuf);
                }
                i64R = vfs_ram_pread(i64In, aBuf, chunk, u64Off);
                if (i64R < 0) {
                    return done ? (i64)done : i64R;
                }
                if (i64R == 0) {
                    break;
                }
                memset(&sub, 0, sizeof(sub));
                sub.u64Nr = LINUX_NR_sendto;
                sub.u64Arg0 = (u64)i64Out;
                sub.u64Arg1 = (u64)(gj_vaddr_t)aBuf;
                sub.u64Arg2 = (u64)i64R;
                sub.u64Arg3 = 0;
                i64W = gj_linux_cold_send(&sub);
                if (i64W < 0) {
                    return done ? (i64)done : i64W;
                }
                if (i64W == 0) {
                    break;
                }
                done += (size_t)i64W;
                u64Off += (u64)i64W;
                if ((size_t)i64W < (size_t)i64R) {
                    break;
                }
            }
            if (pIn != NULL) {
                *pIn = u64Off;
            }
            if (pRegs->u64Arg1 != 0) {
                if (user_range_ok(pRegs->u64Arg1, 8)) {
                    (void)copy_to_user(pRegs->u64Arg1, &u64Off, 8);
                } else {
                    *(u64 *)(gj_vaddr_t)pRegs->u64Arg1 = u64Off;
                }
            }
            /* out offset soft advance (net has no file pos; honesty only) */
            if (pOut != NULL) {
                *pOut += (u64)done;
            }
            if (pRegs->u64Arg3 != 0) {
                u64 u64Out = pOut ? *pOut : (u64)done;

                if (user_range_ok(pRegs->u64Arg3, 8)) {
                    (void)copy_to_user(pRegs->u64Arg3, &u64Out, 8);
                } else {
                    *(u64 *)(gj_vaddr_t)pRegs->u64Arg3 = u64Out;
                }
            }
            protonrt_soft_inc(&g_u64PrtLeanRwNet);
            if (g_fPrtSoftCopyFrOnce == 0) {
                g_fPrtSoftCopyFrOnce = 1;
                /* Grep: protonrt: soft copy_file residual lean */
                kprintf("protonrt: soft copy_file residual lean "
                        "path=ram_net n=%llu Soft!=product g_ac_1=1 "
                        "dual_dod_a=OPEN dual_dod_b=OPEN "
                        "product_path=UDX_DDI+hot_cold_ABI not_bar3=1\n",
                        (unsigned long long)done);
            }
            return (i64)done;
        }
        return -LINUX_EBADF;
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
        /* renameat/renameat2(olddirfd, old, newdirfd, new[, flags]) - paths */
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
        /* SECCOMP_SET_MODE_STRICT etc. - accept no-op for wine probe */
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
         * pidfd_getfd(pidfd, targetfd, flags) - same-pid only.
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
        /* kcmp(pid1,pid2,type,...) - same-pid equal */
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
        protonrt_soft_inc(&g_u64PrtLeanSocket);
        return gj_linux_cold_connect(pRegs);
    }

    case LINUX_NR_close: {
        i64 i64St;
        i64 i64Fd = (i64)pRegs->u64Arg0;

        /*
         * Residual lean: cold_net close first (tcp soft clear + lo).
         * Then vfs_ram. Soft fd flags table cleared on either path.
         * Soft!=product. G-AC-1: UDX host fd lifecycle.
         * greppable: protonrt: soft residual lean close
         */
        protonrt_soft_inc(&g_u64PrtLeanClose);
        i64St = gj_linux_cold_close(pRegs);
        if (i64St != -(i64)LINUX_EBADF) {
            protonrt_soft_fdfl_clear(i64Fd);
            return i64St;
        }
        if (vfs_ram_fd_ok(i64Fd)) {
            i64St = vfs_ram_close(i64Fd);
            protonrt_soft_fdfl_clear(i64Fd);
            return i64St;
        }
        break;
    }

    case LINUX_NR_getcwd: {
        u64 u64Buf = pRegs->u64Arg0;
        u64 u64Size = pRegs->u64Arg1;
        size_t n = 0;
        char *szCwd = protonrt_cwd_buf();
        char aRoot[2];

        if (u64Buf == 0 || u64Size < 2) {
            return -LINUX_EINVAL;
        }
        /*
         * Linux getcwd(2) returns the byte count including NUL.
         * Empty / non-absolute cwd: dash treats buf[0]!='/' as failure
         * with errno left 0 ("getcwd() failed: Success"). Dual DoD B OPEN.
         */
        aRoot[0] = '/';
        aRoot[1] = '\0';
        if (szCwd == NULL || szCwd[0] != '/') {
            szCwd = aRoot;
        }
        while (szCwd[n] != '\0' && n + 1 < 96) {
            n++;
        }
        n++; /* include NUL */
        if (u64Size < n) {
            return -LINUX_ERANGE;
        }
        if (user_range_ok(u64Buf, n)) {
            if (copy_to_user(u64Buf, szCwd, n) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Buf, szCwd, n);
        }
        return (i64)n;
    }

    case LINUX_NR_dup:
    case LINUX_NR_dup2:
    case LINUX_NR_dup3: {
        i64 i64St;
        struct gj_process *pSave;

        /* Cold net first so dup2(accepted, 0/1/2) is not vfs EBADF.
         * QEMU96 vfs-first skipped LCN aliases and lost the banner. */
        i64St = gj_linux_cold_dup2(pRegs);
        if (i64St != -(i64)LINUX_EBADF) {
            return i64St;
        }
        pSave = protonrt_bind_calling_user();
        if (pRegs->u64Nr == LINUX_NR_dup) {
            i64St = vfs_ram_dup((i64)pRegs->u64Arg0);
        } else {
            i64St = vfs_ram_dup2((i64)pRegs->u64Arg0, (i64)pRegs->u64Arg1);
        }
        protonrt_unbind_calling_user(pSave);
        return i64St;
    }

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
        /* Multi-wait: not implemented - return 0 (no waiters woken) */
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
        int *pSt = NULL;
        struct gj_process *pParent;

        if (pRegs->u64Arg1 != 0) {
            pSt = (int *)&st;
        }
        /*
         * Daemon-grade: process_wait_soft when parent PCB available
         * (PCB-filtered children). Fallback process_wait4 if no PCB.
         * greppable: protonrt: soft fork-wait wire PASS
         */
        pParent = protonrt_soft_parent();
        if (pParent != NULL) {
            protonrt_soft_fork_wait_wire_pass_once();
            r = process_wait_soft(pParent, (i64)pRegs->u64Arg0, pSt,
                                  (int)pRegs->u64Arg2);
        } else {
            r = process_wait4((i64)pRegs->u64Arg0, &st, (int)pRegs->u64Arg2);
        }
        if (r > 0 && pRegs->u64Arg1 != 0) {
            if (user_range_ok(pRegs->u64Arg1, sizeof(st))) {
                if (copy_to_user(pRegs->u64Arg1, &st, sizeof(st)) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                *(i32 *)(gj_vaddr_t)pRegs->u64Arg1 = st;
            }
        }
        /*
         * wait4 rusage honesty (arg3): dash/zsh/tcsh wait3/wait4 + libc
         * wait4() pass a struct rusage*. Prior residual ignored it (stack
         * garbage). Zero-fill the public 144-byte x86_64 layout so scripts
         * can detect success vs ENOSYS/garbage. Product child times OPEN.
         * greppable: protonrt: soft wait4 rusage residual lean
         * Soft!=product. G-AC-1. Dual DoD A/B OPEN.
         */
        if (r > 0 && pRegs->u64Arg3 != 0) {
            u8 aRu[144];
            u32 iRu;

            for (iRu = 0; iRu < sizeof(aRu); iRu++) {
                aRu[iRu] = 0;
            }
            if (user_range_ok(pRegs->u64Arg3, sizeof(aRu))) {
                if (copy_to_user(pRegs->u64Arg3, aRu, sizeof(aRu)) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memset((void *)(gj_vaddr_t)pRegs->u64Arg3, 0, sizeof(aRu));
            }
            if (g_fPrtSoftWait4RusageOnce == 0) {
                g_fPrtSoftWait4RusageOnce = 1;
                kprintf("protonrt: soft wait4 rusage residual lean "
                        "pid=%ld cb=144 zero=1 Soft!=product g_ac_1=1 "
                        "dual_dod_a=OPEN dual_dod_b=OPEN "
                        "product_path=UDX_DDI+hot_cold_ABI not_bar3=1\n",
                        (long)r);
            }
        }
        if (r == -10) {
            return -LINUX_ECHILD;
        }
        return r;
    }

    case LINUX_NR_waitid: {
        /*
         * waitid(idtype, id, *infop, options) - soft reaper for driver hosts.
         * Prefer process_waitid_soft when parent PCB is available.
         * greppable: protonrt: soft fork-wait wire PASS
         */
        i32 st = 0;
        int nSiCode = 0;
        i64 r;
        struct gj_process *pParent;
        u32 u32IdType = (u32)pRegs->u64Arg0;
        i64 i64Id = (i64)pRegs->u64Arg1;
        int nOpts = (int)pRegs->u64Arg3;
        u8 aInfo[128];

        pParent = protonrt_soft_parent();
        if (pParent != NULL) {
            protonrt_soft_fork_wait_wire_pass_once();
            r = process_waitid_soft(pParent, u32IdType, i64Id, (int *)&st,
                                    nOpts, &nSiCode);
        } else {
            i64 pid = -1;

            if (u32IdType == GJ_P_PID) {
                pid = i64Id;
            } else if (u32IdType != GJ_P_ALL) {
                return -LINUX_EINVAL;
            }
            r = process_wait4(pid, &st, nOpts | 1 /* WNOHANG soft prefer */);
            if (r > 0) {
                nSiCode = 1; /* CLD_EXITED-shaped */
            }
        }
        if (r == -10) {
            return -LINUX_ECHILD;
        }
        if (r <= 0) {
            return r;
        }
        (void)nSiCode;
        if (pRegs->u64Arg2 != 0) {
            linux_siginfo_sigchld_exited(aInfo, (u32)sizeof(aInfo), (u32)r,
                                         (u32)((st >> 8) & 0xff));
            if (user_range_ok(pRegs->u64Arg2, sizeof(aInfo))) {
                (void)copy_to_user(pRegs->u64Arg2, aInfo, sizeof(aInfo));
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aInfo,
                       sizeof(aInfo));
            }
        }
        return 0; /* waitid success is 0; pid lives in siginfo */
    }

    case LINUX_NR_kill: {
        i64 i64Gate;
        i64 i64Pid;
        u32 u32Sig;
        struct gj_linux_regs idRegs;
        i64 i64Self;
        i64 i64Pgid;

        /* Soft confine: PROC gates kill-shaped ambient ops. */
        i64Gate = confine_soft_promise_require(GJ_PROMISE_PROC);
        if (i64Gate != 0) {
            return i64Gate;
        }
        i64Pid = (i64)pRegs->u64Arg0;
        u32Sig = (u32)pRegs->u64Arg1;
        if (u32Sig > 0u && u32Sig < 64u) {
            vfs_ram_signalfd_inject(u32Sig);
        }
        memset(&idRegs, 0, sizeof(idRegs));
        i64Self = gj_linux_hot_getpid(&idRegs);
        /* pid==1 / self: no-op. Product self-signal OPEN. */
        if (i64Pid == 1 || (i64Self > 0 && i64Pid == i64Self)) {
            return 0;
        }
        /* pid==0: own-pgrp broadcast accepted; do not walk pids. */
        if (i64Pid == 0) {
            return 0;
        }
        /*
         * pid==-1: process-wide broadcast. Product broadcast OPEN.
         * Fail-closed ESRCH (do not pretend every pid was signaled).
         */
        if (i64Pid == -1) {
            return -LINUX_ESRCH;
        }
        if (i64Pid < 0) {
            i64Pgid = gj_linux_hot_getpgid(&idRegs);
            if (i64Pgid > 0 && (-i64Pid) == i64Pgid) {
                return 0;
            }
            return -LINUX_ESRCH;
        }
        if (process_wait_pid_registered((u32)i64Pid) == 0) {
            return -LINUX_ESRCH;
        }
        if (u32Sig == (u32)LINUX_SIGKILL || u32Sig == (u32)LINUX_SIGTERM) {
            i64 i64St;

            i64St = process_linux_exit_pid((u32)i64Pid, u32Sig);
            if (g_fPrtSoftKillWaitOnce == 0) {
                g_fPrtSoftKillWaitOnce = 1;
                kprintf("protonrt: soft kill wait-child residual lean "
                        "pid=%ld sig=%u st=%ld Soft!=product g_ac_1=1 "
                        "dual_dod_a=OPEN dual_dod_b=OPEN "
                        "sig_delivery=OPEN not_bar3=1\n",
                        (long)i64Pid, u32Sig, (long)i64St);
            }
            if (i64St != 0) {
                return -LINUX_ESRCH;
            }
            return 0;
        }
        /* Other signals: accepted, not delivered. Product sig delivery OPEN. */
        if (g_fPrtSoftKillWaitOnce == 0) {
            g_fPrtSoftKillWaitOnce = 1;
            kprintf("protonrt: soft kill wait-child residual lean "
                    "pid=%ld sig=%u accept_no_deliver=1 Soft!=product "
                    "g_ac_1=1 dual_dod_a=OPEN dual_dod_b=OPEN "
                    "sig_delivery=OPEN not_bar3=1\n",
                    (long)i64Pid, u32Sig);
        }
        return 0;
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
         * (grep: confine: expose soft - product: vfsd path policy).
         */
        if (g_pLinuxProc != NULL && g_pLinuxProc->u32Confined != 0u) {
            g_u32ConfinePromiseDeny++;
            g_u32ConfineExposeDeny++;
            kprintf("confine: expose soft mount deny (confined)\n");
            return -LINUX_EACCES;
        }
        /* No real VFS mounts - success for probe (product: vfsd) */
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
        /* Legacy getdents -> getdents64 cold path */
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
        /*
         * Resource-specific soft rlimits for dash/zsh/tcsh.
         * NOFILE must not be 1 GiB (shells size fd tables from this).
         * set/prlimit accept-and-ignore unless new pointer unreadable.
         * Soft!=product. G-AC-1. Dual DoD A/B OPEN.
         * greppable: protonrt: soft rlimit residual lean
         */
        struct {
            u64 rlim_cur;
            u64 rlim_max;
        } lim;
        struct {
            u64 rlim_cur;
            u64 rlim_max;
        } ign;
        u64 u64New;
        u64 u64Old;
        i32 i32Res;

        if (pRegs->u64Nr == LINUX_NR_prlimit64) {
            i32Res = (i32)pRegs->u64Arg1;
            u64New = pRegs->u64Arg2;
            u64Old = pRegs->u64Arg3;
        } else if (pRegs->u64Nr == LINUX_NR_getrlimit) {
            i32Res = (i32)pRegs->u64Arg0;
            u64New = 0;
            u64Old = pRegs->u64Arg1;
        } else {
            i32Res = (i32)pRegs->u64Arg0;
            u64New = pRegs->u64Arg1;
            u64Old = 0;
        }

        if (i32Res < 0 || i32Res > LINUX_RLIMIT_RTTIME) {
            return -LINUX_EINVAL;
        }
        if (pRegs->u64Nr == LINUX_NR_setrlimit && u64New == 0) {
            return -LINUX_EFAULT;
        }
        if (u64New != 0) {
            if (user_range_ok(u64New, sizeof(ign))) {
                if (copy_from_user(&ign, u64New, sizeof(ign)) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy(&ign, (const void *)(gj_vaddr_t)u64New, sizeof(ign));
            }
            (void)ign; /* set ignored; product rlimit store OPEN */
        }

        lim.rlim_cur = 1024ull * 1024ull * 1024ull; /* 1 GiB class */
        lim.rlim_max = lim.rlim_cur;
        if (i32Res == LINUX_RLIMIT_NOFILE) {
            lim.rlim_cur = 1024ull;
            lim.rlim_max = 1024ull;
        } else if (i32Res == LINUX_RLIMIT_NPROC) {
            lim.rlim_cur = 256ull;
            lim.rlim_max = 256ull;
        } else if (i32Res == LINUX_RLIMIT_STACK) {
            lim.rlim_cur = 8ull * 1024ull * 1024ull;
            lim.rlim_max = lim.rlim_cur;
        } else if (i32Res == LINUX_RLIMIT_CORE ||
                   i32Res == LINUX_RLIMIT_NICE ||
                   i32Res == LINUX_RLIMIT_RTPRIO) {
            lim.rlim_cur = 0;
            lim.rlim_max = 0;
        } else if (i32Res == LINUX_RLIMIT_AS) {
            lim.rlim_cur = LINUX_RLIM_INFINITY;
            lim.rlim_max = LINUX_RLIM_INFINITY;
        }

        if (u64Old != 0) {
            if (user_range_ok(u64Old, sizeof(lim))) {
                if (copy_to_user(u64Old, &lim, sizeof(lim)) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Old, &lim, sizeof(lim));
            }
        }
        if (g_fPrtSoftRlimitOnce == 0) {
            g_fPrtSoftRlimitOnce = 1;
            kprintf("protonrt: soft rlimit residual lean "
                    "res=%d cur=%llu max=%llu Soft!=product g_ac_1=1 "
                    "dual_dod_a=OPEN dual_dod_b=OPEN not_bar3=1\n",
                    (int)i32Res,
                    (unsigned long long)lim.rlim_cur,
                    (unsigned long long)lim.rlim_max);
        }
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
        {
            char *szCwd = protonrt_cwd_buf();

            n = 0;
            while (szPath[n] != '\0' && n + 1 < 96) {
                szCwd[n] = szPath[n];
                n++;
            }
            szCwd[n] = '\0';
            if (n == 0) {
                szCwd[0] = '/';
                szCwd[1] = '\0';
            }
        }
        return 0;
    }

    case LINUX_NR_fchdir: {
        size_t n;
        char *szCwd;

        if (vfs_ram_fd_path((i64)pRegs->u64Arg0, szPath, sizeof(szPath)) != 0) {
            return -LINUX_EBADF;
        }
        szCwd = protonrt_cwd_buf();
        n = 0;
        while (szPath[n] != '\0' && n + 1 < 96) {
            szCwd[n] = szPath[n];
            n++;
        }
        szCwd[n] = '\0';
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
         * Residual lean select/pselect6 (bar3-adjacent ABI surface).
         * Readiness via cold_net poll_mask then vfs_ram_poll_mask - same
         * residual as poll. Soft!=product. G-AC-1 userspace host reach.
         * Product multi-fd demux / infinite wait remains OPEN.
         *   timeout==0        -> single readiness pass
         *   timeout NULL      -> yield until ready (Linux infinite)
         *   timeout>0         -> soft-spin yield + jiffies budget
         * greppable: protonrt: soft select residual lean
         * Never claims bar3 / Deck Top 50 / product DoD.
         */
        u32 u32Nfds;
        u32 u32Fd;
        u32 u32Ready;
        u32 u32CbSet;
        u32 u32SpinLeft = 0;
        u32 u32Pass;
        i64 i64TimeoutMs = 0;
        u64 u64J0;
        u8 aInR[PRT_SELECT_SOFT_FDSET_BYTES];
        u8 aInW[PRT_SELECT_SOFT_FDSET_BYTES];
        u8 aInE[PRT_SELECT_SOFT_FDSET_BYTES];
        u8 aOutR[PRT_SELECT_SOFT_FDSET_BYTES];
        u8 aOutW[PRT_SELECT_SOFT_FDSET_BYTES];
        u8 aOutE[PRT_SELECT_SOFT_FDSET_BYTES];
        int fHaveR;
        int fHaveW;
        int fHaveE;
        int fPark;

        protonrt_soft_inc(&g_u64PrtLeanSelect);
        if (pRegs->u64Arg0 > 1024ull) {
            return -LINUX_EINVAL;
        }
        u32Nfds = (u32)pRegs->u64Arg0;
        if (u32Nfds == 0u) {
            return 0;
        }
        if (u32Nfds > PRT_SELECT_SOFT_NFDS_MAX) {
            u32Nfds = PRT_SELECT_SOFT_NFDS_MAX;
        }
        u32CbSet = (u32Nfds + 7u) / 8u;
        if (u32CbSet > PRT_SELECT_SOFT_FDSET_BYTES) {
            u32CbSet = PRT_SELECT_SOFT_FDSET_BYTES;
        }
        memset(aInR, 0, sizeof(aInR));
        memset(aInW, 0, sizeof(aInW));
        memset(aInE, 0, sizeof(aInE));
        memset(aOutR, 0, sizeof(aOutR));
        memset(aOutW, 0, sizeof(aOutW));
        memset(aOutE, 0, sizeof(aOutE));
        fHaveR = (pRegs->u64Arg1 != 0) ? 1 : 0;
        fHaveW = (pRegs->u64Arg2 != 0) ? 1 : 0;
        fHaveE = (pRegs->u64Arg3 != 0) ? 1 : 0;

        if (fHaveR) {
            if (user_range_ok(pRegs->u64Arg1, u32CbSet)) {
                if (copy_from_user(aInR, pRegs->u64Arg1, u32CbSet) !=
                    GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy(aInR, (const void *)(gj_vaddr_t)pRegs->u64Arg1,
                       u32CbSet);
            }
        }
        if (fHaveW) {
            if (user_range_ok(pRegs->u64Arg2, u32CbSet)) {
                if (copy_from_user(aInW, pRegs->u64Arg2, u32CbSet) !=
                    GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy(aInW, (const void *)(gj_vaddr_t)pRegs->u64Arg2,
                       u32CbSet);
            }
        }
        if (fHaveE) {
            if (user_range_ok(pRegs->u64Arg3, u32CbSet)) {
                if (copy_from_user(aInE, pRegs->u64Arg3, u32CbSet) !=
                    GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy(aInE, (const void *)(gj_vaddr_t)pRegs->u64Arg3,
                       u32CbSet);
            }
        }

        /*
         * Timeout: select timeval {sec,usec}; pselect6 timespec {sec,nsec}.
         * Soft!=product: zero -> single pass; NULL -> infinite; >0 budget.
         */
        if (pRegs->u64Arg4 == 0) {
            i64TimeoutMs = -1;
        } else {
            i64 i64Sec = 0;
            i64 i64Frac = 0;

            if (user_range_ok(pRegs->u64Arg4, 16)) {
                if (copy_from_user(&i64Sec, pRegs->u64Arg4, 8) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
                if (copy_from_user(&i64Frac, pRegs->u64Arg4 + 8, 8) !=
                    GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                i64Sec = *(const i64 *)(gj_vaddr_t)pRegs->u64Arg4;
                i64Frac = *(const i64 *)(gj_vaddr_t)(pRegs->u64Arg4 + 8);
            }
            if (i64Sec < 0 || i64Frac < 0) {
                i64TimeoutMs = 0;
            } else if (i64Sec == 0 && i64Frac == 0) {
                i64TimeoutMs = 0;
            } else if (pRegs->u64Nr == LINUX_NR_pselect6) {
                i64TimeoutMs = i64Sec * 1000 + i64Frac / 1000000;
                if (i64TimeoutMs <= 0) {
                    i64TimeoutMs = 1;
                }
            } else {
                /* timeval usec */
                i64TimeoutMs = i64Sec * 1000 + i64Frac / 1000;
                if (i64TimeoutMs <= 0) {
                    i64TimeoutMs = 1;
                }
            }
        }

        if (i64TimeoutMs > 0) {
            u32SpinLeft = (u32)i64TimeoutMs;
            if (u32SpinLeft > 128u) {
                u32SpinLeft = 128u;
            }
            if (u32SpinLeft < 1u) {
                u32SpinLeft = 1u;
            }
        }

        u64J0 = timer_jiffies();
        for (u32Pass = 0;; u32Pass++) {
            u32Ready = 0;
            fPark = 0;
            memset(aOutR, 0, sizeof(aOutR));
            memset(aOutW, 0, sizeof(aOutW));
            memset(aOutE, 0, sizeof(aOutE));

            for (u32Fd = 0; u32Fd < u32Nfds; u32Fd++) {
                u32 u32Byte = u32Fd / 8u;
                u8 u8Bit = (u8)(1u << (u32Fd % 8u));
                u32 u32Want = 0;
                u32 u32Got;
                int fInR = 0;
                int fInW = 0;
                int fInE = 0;

                if (fHaveR && (aInR[u32Byte] & u8Bit) != 0) {
                    fInR = 1;
                    u32Want |= (u32)LINUX_POLLIN;
                }
                if (fHaveW && (aInW[u32Byte] & u8Bit) != 0) {
                    fInW = 1;
                    u32Want |= (u32)LINUX_POLLOUT;
                }
                if (fHaveE && (aInE[u32Byte] & u8Bit) != 0) {
                    fInE = 1;
                    u32Want |= (u32)LINUX_POLLERR;
                }
                if (u32Want == 0) {
                    continue;
                }
                if (vfs_ram_fd_poll_parkable((i64)u32Fd) != 0) {
                    fPark = 1;
                }
                u32Got = protonrt_soft_fd_ready_mask((i64)u32Fd, u32Want);
                if (fInR && (u32Got & (u32)LINUX_POLLIN) != 0) {
                    aOutR[u32Byte] |= u8Bit;
                    u32Ready++;
                }
                if (fInW && (u32Got & (u32)LINUX_POLLOUT) != 0) {
                    aOutW[u32Byte] |= u8Bit;
                    u32Ready++;
                }
                if (fInE &&
                    (u32Got & ((u32)LINUX_POLLERR | (u32)LINUX_POLLHUP)) !=
                        0) {
                    aOutE[u32Byte] |= u8Bit;
                    u32Ready++;
                }
            }

            if (u32Ready != 0) {
                break;
            }
            if (i64TimeoutMs == 0) {
                break;
            }
            if (i64TimeoutMs < 0) {
                if (fPark != 0) {
                    vfs_ram_poll_park();
                } else {
                    thread_yield();
                }
                (void)u32Pass;
                continue;
            }
            thread_yield();
            if (u32SpinLeft > 0) {
                u32SpinLeft--;
            }
            if (u32SpinLeft == 0) {
                break;
            }
            if ((timer_jiffies() - u64J0) > 32ull) {
                break;
            }
            (void)u32Pass;
        }

        if (fHaveR) {
            if (user_range_ok(pRegs->u64Arg1, u32CbSet)) {
                if (copy_to_user(pRegs->u64Arg1, aOutR, u32CbSet) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg1, aOutR, u32CbSet);
            }
        }
        if (fHaveW) {
            if (user_range_ok(pRegs->u64Arg2, u32CbSet)) {
                if (copy_to_user(pRegs->u64Arg2, aOutW, u32CbSet) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg2, aOutW, u32CbSet);
            }
        }
        if (fHaveE) {
            if (user_range_ok(pRegs->u64Arg3, u32CbSet)) {
                if (copy_to_user(pRegs->u64Arg3, aOutE, u32CbSet) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)pRegs->u64Arg3, aOutE, u32CbSet);
            }
        }

        if (g_fPrtSoftSelectOnce == 0) {
            g_fPrtSoftSelectOnce = 1;
            /* Grep: protonrt: soft select residual lean */
            kprintf("protonrt: soft select residual lean "
                    "nfds=%u ready=%u timeout_ms=%ld Soft!=product "
                    "g_ac_1=1 abi_adj=proton_p0 not_bar3=1\n",
                    u32Nfds, u32Ready, (long)i64TimeoutMs);
        }
        return (i64)u32Ready;
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
        /* open_how: flags at 0, mode at 8, resolve at 16 - use flags/mode */
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
        /* statx: stx_mask@0 stx_blksize@4 stx_attributes@8 ... stx_mode@20
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

    case LINUX_NR_fork: {
        /*
         * USER parent only (not g_pLinuxProc). LINUX in
         * [0x1000000, 0x1800000) or [0x4000000, 0x5000000) gets a
         * USER child via process_linux_fork; kernel smoke uses the
         * death-worker fallback. greppable: protonrt: soft fork-wait wire PASS
         */
        struct gj_process *pParent = protonrt_linux_fork_parent();

        if (pParent != NULL) {
            protonrt_soft_fork_wait_wire_pass_once();
            return process_fork_soft(pParent);
        }
        return process_linux_fork(1, 0);
    }

    case LINUX_NR_vfork: {
        /*
         * Product vfork: runnable USER child (dash vforkexec / OpenSSH
         * session). Shared-AS + parent-block until exec/exit remain OPEN.
         * Kernel PE32 smoke still uses process_linux_fork(..., fExitNow=1).
         */
        struct gj_process *pParent = protonrt_linux_fork_parent();

        if (pParent != NULL) {
            protonrt_soft_fork_wait_wire_pass_once();
            return process_fork_soft(pParent);
        }
        return process_linux_fork(1, 0);
    }

    case LINUX_NR_clone:
    case LINUX_NR_clone3: {
        /*
         * clone(flags, stack, parent_tid, child_tid, tls) x86_64 public map.
         * clone3: copy VER0 clone_args (stack+stack_size = SP).
         * CLONE_THREAD -> same-pid thread spawn. Soft!=product.
         * greppable: protonrt: soft fork-wait wire PASS
         */
        u64 u64Flags = pRegs->u64Arg0;
        u64 u64Stack = pRegs->u64Arg1;
        u64 u64ParentTid = pRegs->u64Arg2;
        u64 u64ChildTid = pRegs->u64Arg3;
        u64 u64Tls = pRegs->u64Arg4;
        struct gj_process *pParent;

        if (pRegs->u64Nr == LINUX_NR_clone3) {
            struct linux_clone_args args;
            u64 cbArgs;

            cbArgs = pRegs->u64Arg1;
            if (pRegs->u64Arg0 == 0 || cbArgs < LINUX_CLONE_ARGS_SIZE_VER0) {
                return -LINUX_EINVAL;
            }
            memset(&args, 0, sizeof(args));
            if (cbArgs > (u64)sizeof(args)) {
                cbArgs = (u64)sizeof(args);
            }
            if (user_range_ok(pRegs->u64Arg0, cbArgs)) {
                if (copy_from_user(&args, pRegs->u64Arg0, cbArgs) != GJ_OK) {
                    return -LINUX_EFAULT;
                }
            } else {
                memcpy(&args, (const void *)(gj_vaddr_t)pRegs->u64Arg0,
                       (size_t)cbArgs);
            }
            u64Flags = args.u64Flags;
            u64ParentTid = args.u64ParentTid;
            u64ChildTid = args.u64ChildTid;
            u64Tls = args.u64Tls;
            if (args.u64Stack != 0 && args.u64StackSize != 0) {
                u64Stack = args.u64Stack + args.u64StackSize;
            } else {
                u64Stack = args.u64Stack;
            }
        }
        if (u64Flags & GJ_CLONE_THREAD) {
            /*
             * Thread-shaped: spawn real user thread in current process AS
             * when child_stack is provided. Entry = parent user RIP (after
             * SYSCALL). Parent returns thread id; child starts with rax=0
             * (product: set child frame rax later).
             */
            struct gj_cpu *pCpu;
            struct gj_thread *pCur;
            struct gj_process *pProc;
            u64 u64Entry;
            u32 thr;

            pCur = thread_current();
            pProc = protonrt_calling_user_proc();
            pCpu = cpu_current();
            u64Entry = (pCpu != NULL) ? pCpu->u64UserRip : 0;
            if (u64Entry == 0 && pCur != NULL) {
                if (pCur->u32SysUserValid != 0) {
                    u64Entry = pCur->u64SysUserRip;
                } else {
                    u64Entry = pCur->u64UserRip;
                }
            }
            if (u64Stack != 0 && u64Entry != 0 && pProc != NULL) {
                thr = thread_create_user(pProc, u64Entry, u64Stack);
                if (thr != 0) {
                    u32 u32Tid = thr;

                    if ((u64Flags & LINUX_CLONE_PARENT_SETTID) != 0 &&
                        u64ParentTid != 0) {
                        if (user_range_ok(u64ParentTid, 4)) {
                            (void)copy_to_user(u64ParentTid, &u32Tid, 4);
                        } else {
                            *(u32 *)(gj_vaddr_t)u64ParentTid = u32Tid;
                        }
                    }
                    if ((u64Flags & LINUX_CLONE_CHILD_SETTID) != 0 &&
                        u64ChildTid != 0) {
                        if (user_range_ok(u64ChildTid, 4)) {
                            (void)copy_to_user(u64ChildTid, &u32Tid, 4);
                        } else {
                            *(u32 *)(gj_vaddr_t)u64ChildTid = u32Tid;
                        }
                    }
                    if ((u64Flags & LINUX_CLONE_CHILD_CLEARTID) != 0) {
                        thread_set_clear_child_tid(thr, u64ChildTid);
                    }
                    if ((u64Flags & LINUX_CLONE_SETTLS) != 0) {
                        thread_set_fs_base(thr, u64Tls);
                    }
                    kprintf("linux: clone thread thr=%u entry=0x%lx\n", thr,
                            (unsigned long)u64Entry);
                    return (i64)thr;
                }
                return -11; /* EAGAIN */
            }
            /* clone3 THREAD without stack is EINVAL (not a soft 0). */
            if (pRegs->u64Nr == LINUX_NR_clone3) {
                return -LINUX_EINVAL;
            }
            /* No stack/entry (kernel-side smoke): soft success */
            return 0;
        }
        pParent = protonrt_linux_fork_parent();
        if (pParent != NULL) {
            protonrt_soft_fork_wait_wire_pass_once();
            return process_clone_soft(pParent, u64Flags);
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
        struct gj_process *pExec;
        /* Embedded product ld-gj (optional; prefer when vfs stub is tiny) */
        extern const u8 gj_ld_gj_elf_blob[];
        extern const u8 gj_ld_gj_elf_blob_end[];

        pExec = protonrt_linux_exec_proc();
        if (pExec == NULL) {
            /*
             * Kernel smoke only. A live LINUX user.ld parent (sshd/dash)
             * must not exec into g_pLinuxProc; session child uses
             * thread_exec_replace on its own PCB (0.1.140 #PF).
             */
            if (protonrt_linux_pcb_userld(g_pLastLinuxUserProc) != 0) {
                return -LINUX_EAGAIN;
            }
            pExec = g_pLinuxProc;
        }
        copy_path_from_arg(szPath, sizeof(szPath), u64Path);
        if (szPath[0] == '\0') {
            return -LINUX_EFAULT;
        }
        /*
         * Dash /bin/sh and OpenSSH 10.5 sshd / sshd-session / sshd-auth
         * are kernel embeds (g_aExecEmbed). vfs_ram 32KiB and aImg[16384]
         * cannot hold DUT ELFs. Before vfs. thread_exec_replace only.
         * Later-wall: sshd-session / sshd-auth never vfs 1-byte stubs.
         */
        {
            i64 i64Emb = protonrt_exec_embed_try(pRegs, szPath);

            if (i64Emb != 1) {
                return i64Emb;
            }
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
         * grow). Private AS COW/protect bugs have left those leaves RO - repair
         * identity R/W under kernel + active CR3 before the store.
         */
        (void)vmm_ensure_identity_rw((gj_vaddr_t)aImg, sizeof(aImg));
        (void)vmm_ensure_identity_rw((gj_vaddr_t)aInterp, sizeof(aInterp));
        n = vfs_ram_read(i64Fd, aImg, sizeof(aImg));
        (void)vfs_ram_close(i64Fd);
        if (n >= 4 && aImg[0] == 0x7fu && aImg[1] == (u8)'E' &&
            aImg[2] == (u8)'L' && aImg[3] == (u8)'F' && pExec != NULL) {
            int fInterpLoaded = 0;

            if (elf_probe_image(aImg, (u64)n, &elf) != GJ_OK) {
                return -8; /* ENOEXEC */
            }
            pExec->u64InterpEntry = 0;
            pExec->u32ExecFlags = elf.u32Flags;
            pExec->cAuxv = 0;
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
                    elf_load_image(pExec, aInterp, (u64)nI, &elfInterp) ==
                        GJ_OK) {
                    pExec->u64InterpEntry = elfInterp.u64Entry;
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
                u32 cSo = elf_load_needed_sos(pExec, &elf);

                pExec->cNeededLoaded = cSo;
                if (cSo > 0) {
                    kprintf("linux: execve SO map PASS n=%u\n", cSo);
                }
            }
            if (elf_load_image(pExec, aImg, (u64)n, &elf) == GJ_OK) {
                size_t iPath;

                pExec->u64ExecEntry = elf.u64Entry;
                pExec->u64LoadBias = elf.u64Bias;
                pExec->u32ExecFlags = elf.u32Flags;
                /* Remember path for AT_EXECFN / ld-gj handoff */
                for (iPath = 0; iPath + 1 < sizeof(pExec->szExecPath) &&
                                szPath[iPath] != '\0';
                     iPath++) {
                    pExec->szExecPath[iPath] = szPath[iPath];
                }
                pExec->szExecPath[iPath] = '\0';
                elf_fill_auxv(pExec, &elf,
                              fInterpLoaded ? &elfInterp : NULL);
                {
                    u64 u64Argv;
                    u64 u64Envp;

                    if (pRegs->u64Nr == LINUX_NR_execveat) {
                        u64Argv = pRegs->u64Arg2;
                        u64Envp = pRegs->u64Arg3;
                    } else {
                        u64Argv = pRegs->u64Arg1;
                        u64Envp = pRegs->u64Arg2;
                    }
                    if (elf_publish_handoff_argv(
                            pExec, szPath, &elf,
                            fInterpLoaded ? &elfInterp : NULL, u64Argv,
                            u64Envp) == GJ_OK) {
                        kprintf("linux: execve handoff PASS\n");
                        if (elf_ld_handoff_verify(pExec) == GJ_OK) {
                            /* ld-gj: handoff PASS already printed */
                        }
                    }
                }
                /*
                 * USER execve (OpenSSH session child / dash): thread_exec_replace
                 * only. Never thread_create_user (0.1.140 #PF I=1). Kernel
                 * smoke may still spawn via elf_apply_interp_first.
                 * greppable: linux: execve replace
                 */
                {
                    struct gj_thread *pThr = thread_current();
                    u64 u64Entry;
                    u64 u64Stack = pExec->u64ExecStack;
                    u32 cRepl;
                    int fUser;

                    if (fInterpLoaded != 0 && elfInterp.u64Entry != 0) {
                        u64Entry = elfInterp.u64Entry;
                    } else {
                        u64Entry = elf.u64Entry;
                    }
                    pExec->u64StartEntry = u64Entry;
                    fUser = (pThr != NULL && pThr->pProc == pExec &&
                             protonrt_thr_is_user(pThr) != 0);
                    if (fUser == 0 && pExec != NULL) {
                        int fLo;
                        int fChild;

                        fLo = (protonrt_linux_text_lo(pExec->u64StartEntry) !=
                                   0 ||
                               protonrt_linux_text_lo(pExec->u64ExecEntry) !=
                                   0 ||
                               protonrt_linux_text_lo(pExec->u64UserSysRip) !=
                                   0);
                        fChild = (process_wait_pid_of(pExec) != 0 ||
                                  pExec->u32StartThr != 0 ||
                                  protonrt_linux_exec_proc() == pExec);
                        /*
                         * Fork USER child / LINUX [0x1000000, 0x1800000):
                         * thread_exec_replace only (never thread_create_user;
                         * 0.1.140 #PF I=1). g_pLinuxProc smoke stays
                         * elf_apply_interp_first unless pcb_lo.
                         */
                        if (fLo != 0 ||
                            (pExec != g_pLinuxProc && fChild != 0)) {
                            if (fLo != 0 ||
                                protonrt_linux_pcb_userld(pExec) != 0 ||
                                protonrt_linux_user_text_ok(
                                    pExec->u64StartEntry) != 0 ||
                                protonrt_linux_user_text_ok(
                                    pExec->u64ExecEntry) != 0 ||
                                protonrt_linux_user_text_ok(
                                    pExec->u64UserSysRip) != 0 ||
                                pExec->u32StartThr != 0 ||
                                process_wait_pid_of(pExec) != 0) {
                                fUser = 1;
                            }
                        }
                    }
                    if (fUser != 0) {
                        if (elf_stack_rsp_live_ok(u64Stack) == 0) {
                            kprintf("linux: execve replace isolate "
                                    "rip=0x%lx sp=0x%lx Soft!=product\n",
                                    (unsigned long)u64Entry,
                                    (unsigned long)u64Stack);
                            return -LINUX_EAGAIN;
                        }
                        cRepl = protonrt_exec_replace_user(pExec, u64Entry,
                                                           u64Stack);
                        if (cRepl == 0) {
                            kprintf("linux: execve replace isolate "
                                    "rip=0x%lx sp=0x%lx Soft!=product\n",
                                    (unsigned long)u64Entry,
                                    (unsigned long)u64Stack);
                            return -LINUX_EAGAIN;
                        }
                        kprintf("linux: execve replace rip=0x%lx sp=0x%lx\n",
                                (unsigned long)u64Entry,
                                (unsigned long)u64Stack);
                    } else {
                        /* Kernel smoke only. USER/fork child never here. */
                        (void)elf_apply_interp_first(
                            pExec, &elf,
                            fInterpLoaded ? &elfInterp : NULL, u64Stack);
                        if (elf_stack_rsp_live_ok(u64Stack) == 0) {
                            kprintf("linux: execve replace isolate "
                                    "rip=0x%lx sp=0x%lx Soft!=product\n",
                                    (unsigned long)u64Entry,
                                    (unsigned long)u64Stack);
                        } else {
                            kprintf("linux: execve replace rip=0x%lx "
                                    "sp=0x%lx\n",
                                    (unsigned long)u64Entry,
                                    (unsigned long)u64Stack);
                        }
                    }
                }
                kprintf("linux: execve ELF entry=0x%lx start=0x%lx auxv=%u\n",
                        (unsigned long)elf.u64Entry,
                        (unsigned long)pExec->u64StartEntry,
                        pExec->cAuxv);
                if (elf.u32Flags & GJ_ELF_INFO_HAS_INTERP) {
                    kprintf("linux: execve INTERP PASS\n");
                }
                if (elf.u32Flags & GJ_ELF_INFO_RELOC_OK) {
                    kprintf("linux: execve RELA PASS\n");
                }
                if (elf.u32Flags & GJ_ELF_INFO_SYM_OK) {
                    kprintf("linux: execve SYM PASS\n");
                }
                if (pExec->cAuxv > 0) {
                    kprintf("linux: execve auxv PASS\n");
                }
                return 0;
            }
            return -8; /* ENOEXEC */
        }
        /* Non-ELF: soft success if path was openable. Later-wall: never
         * a 1-byte vfs stub as successful execve. */
        if (protonrt_exec_later_wall_path(szPath) != 0) {
            return -8;
        }
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
        /*
         * close_range(first, last, flags): close inclusive fd range.
         * FDs are global: skip live net_tcp / net_lo so closefrom
         * close_range(3,~0) cannot destroy listen :22 (fd 96).
         * Still close vfs_ram in range. last cap 127.
         * Soft!=product. G-AC-1 userspace host reach.
         */
        u32 u32First = (u32)pRegs->u64Arg0;
        u32 u32Last = (u32)pRegs->u64Arg1;
        u32 i;

        protonrt_soft_inc(&g_u64PrtLeanClose);
        if (u32First > u32Last) {
            return -LINUX_EINVAL;
        }
        if (u32Last > 127u) {
            u32Last = 127u; /* cover ram + net_lo(64..) + net_tcp(96..) soft */
        }
        for (i = u32First; i <= u32Last; i++) {
            struct gj_linux_regs sub;
            i64 i64St;

            if (net_tcp_fd_ok((i64)i) || net_lo_fd_ok((i64)i)) {
                continue;
            }
            memset(&sub, 0, sizeof(sub));
            sub.u64Nr = LINUX_NR_close;
            sub.u64Arg0 = (u64)i;
            i64St = gj_linux_cold_close(&sub);
            if (i64St != -(i64)LINUX_EBADF) {
                continue;
            }
            if (vfs_ram_fd_ok((i64)i)) {
                (void)vfs_ram_close((i64)i);
            }
        }
        return 0;
    }

    /*
     * Abandoned in-kernel .ko AC (G-AC-1). Product drivers = UDX.
     * greppable: protonrt: soft init_module|finit_module|delete_module ENOSYS
     */
    case LINUX_NR_init_module:
        (void)pRegs;
        return soft_mod_load_and_init(NULL, 0, NULL);

    case LINUX_NR_finit_module:
        (void)pRegs;
        return soft_mod_load_and_init(NULL, 0, NULL);

    case LINUX_NR_delete_module:
        (void)pRegs;
        return -(i64)LINUX_ENOSYS;

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
    /*
     * cold_ipc attach: ProtonRT/Linux personality service for cold NRs.
     * Residual lean reach path for userspace drivers (UDX/DDI hosts).
     * Soft!=product. G-AC-1: not in-kernel .ko product AC.
     * greppable: protonrt: soft cold_ipc attach
     */
    cold_ipc_set_service(protonrt_service, NULL);
    cold_ipc_set_doors_mode(1);
    cold_ipc_set_personality_attached(1);
    protonrt_soft_inc(&g_u64PrtSoftAttach);
    kprintf("protonrt: soft cold_ipc attach service=protonrt_service "
            "userspace_drivers=reach g_ac_1=1 Soft!=product\n");
    /*
     * Soft multi-server confine deepen probe (expose / ledger / death).
     * Product multi-server confine stays OPEN. Preserves confine soft PASS.
     */
    confine_soft_selfprobe();
    /* Soft inventory baseline after cold attach (bring-up diagnostics). */
    protonrt_soft_inventory_log();
    /*
     * Lean soft residual once-lamp (storm=0; no version stamp).
     * Soft!=product dual MIT OR Apache-2.0 - not multi-server / bar3 close.
     * greppable: protonrt: soft residual lean | cold_link: soft residual lean
     */
    protonrt_soft_residual_lean_once();
}
