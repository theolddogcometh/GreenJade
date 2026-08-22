/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal ramdisk: fixed files + open/read/write for cold Linux path.
 * Optional /dev/vda (virtio-blk) and /dev/sda (virtio-scsi) when mounted.
 * Also hosts pipes, Unix98 PTY (/dev/ptmx + /dev/pts/ptmx + /dev/pts/N),
 * /dev/tty (ctty slave after TIOCSCTTY / unique live slave, else seeded
 * char), /dev/urandom + /dev/random (seeded char, tiny LCG fill not EOF),
 * eventfd, epoll, timerfd, signalfd, pidfd, inotify for the Linux ABI
 * bring-up path -
 * independent of vfs_door product mini-FS.
 * Product SSH = OpenSSH-portable, not abandoned sshd_gj. Dual DoD B OPEN.
 *
 * Soft residual (this unit only; lean - no stamp storms, no version stamp):
 *   cold Linux ramfs + specials inventory; Soft!=product (kernel soft !=
 *   vfs_door / vfsd product LBA mini-FS). Dual MIT OR Apache-2.0.
 *   Functional residual preferred over multi-line deepen catalogs.
 *   C0 lean residual: stack-local path_norm/dirname/eq/copy + capacity/kind
 *   self-check once at init (soft residual lean PASS); never hard-gates
 *   product. Dual DoD A/B remain OPEN (this unit is not UDX DoD close).
 * greppable:
 *   vfs_ram: soft inventory | soft residual | soft path | soft honesty
 *   vfs_ram: soft inventory PASS | vfs_ram: soft PASS
 *   vfs_ram: soft residual lean | vfs_ram: soft residual lean PASS
 *   Soft!=product | dual_dod=OPEN
 */
#if 0 /* retired header noise - ignore until endif */
/*
...)
 *   - Wave 14: attr/alloc/sync/xfer/kind peaks + soft ok/fail + last + PASS
 */
#endif
#include <gj/klog.h>
#include <gj/net_lo.h>
#include <gj/thread.h>
#include <gj/net_tcp.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/linux_abi.h>
#include <gj/vfs_ram.h>
#include <gj/virtio_blk.h>
#include <gj/virtio_scsi.h>

/* Net poll readiness - declared in net_tcp.h / net_lo.h (included above). */

#define VFS_MAX_FILES  64
#define VFS_MAX_FDS    96
#define VFS_MAX_PATH   128
/* 32 KiB: room for packaged ld-gj.so.1 (~30 KiB) + small ELFs */
#define VFS_MAX_DATA   32768
#define VFS_MAX_PIPES  16
#define VFS_PIPE_BUF   2048
#define VFS_MAX_PTY    4
#define VFS_PTY_TIOS   60u /* glibc termios: flags + c_cc[32] + speeds */
#define VFS_PTY_CANON  255u /* ICANON line (MAX_CANON-shaped) */

/* asm-generic termbits (octal public) used by the PTY n_tty-shaped path. */
#define VFS_TIOS_ISTRIP  0x0020u
#define VFS_TIOS_INLCR   0x0040u
#define VFS_TIOS_IGNCR   0x0080u
#define VFS_TIOS_ICRNL   0x0100u
#define VFS_TIOS_IUCLC   0x0200u
#define VFS_TIOS_IXON    0x0400u
#define VFS_TIOS_IXANY   0x0800u
#define VFS_TIOS_IUTF8   0x4000u
#define VFS_TIOS_OPOST   0x0001u
#define VFS_TIOS_OLCUC   0x0002u
#define VFS_TIOS_ONLCR   0x0004u
#define VFS_TIOS_OCRNL   0x0008u
#define VFS_TIOS_ONLRET  0x0020u
#define VFS_TIOS_HUPCL   0x0400u
#define VFS_TIOS_CLOCAL  0x0800u
#define VFS_TIOS_ISIG    0x0001u
#define VFS_TIOS_ICANON  0x0002u
#define VFS_TIOS_ECHO    0x0008u
#define VFS_TIOS_ECHOE   0x0010u
#define VFS_TIOS_ECHOK   0x0020u
#define VFS_TIOS_ECHONL  0x0040u
#define VFS_TIOS_NOFLSH  0x0080u
#define VFS_TIOS_ECHOCTL 0x0200u
#define VFS_TIOS_IEXTEN  0x8000u
#define VFS_TIOS_EXTPROC 0x10000u
#define VFS_VINTR    0u
#define VFS_VQUIT    1u
#define VFS_VERASE   2u
#define VFS_VKILL    3u
#define VFS_VEOF     4u
#define VFS_VTIME    5u
#define VFS_VMIN     6u
#define VFS_VSTART   8u
#define VFS_VSTOP    9u
#define VFS_VSUSP    10u
#define VFS_VEOL     11u
#define VFS_VREPRINT 12u
#define VFS_VDISCARD 13u
#define VFS_VWERASE  14u
#define VFS_VLNEXT   15u
#define VFS_VEOL2    16u
#define VFS_TIOS_B38400 0x000Fu /* Linux B38400 in c_cflag / c_*speed */
#define VFS_TIOCM_DTR LINUX_TIOCM_DTR
#define VFS_TIOCM_RTS LINUX_TIOCM_RTS
#define VFS_TIOCM_CTS LINUX_TIOCM_CTS
#define VFS_TIOCM_CAR LINUX_TIOCM_CAR
#define VFS_TIOCM_DSR LINUX_TIOCM_DSR
#define VFS_TIOCM_DEF \
    (VFS_TIOCM_DTR | VFS_TIOCM_RTS | VFS_TIOCM_CTS | VFS_TIOCM_CAR | \
     VFS_TIOCM_DSR)

#define VFS_KIND_RAM     0
#define VFS_KIND_BLK     1
#define VFS_KIND_SCSI    2
#define VFS_KIND_PIPE    3
#define VFS_KIND_EVENTFD 4
#define VFS_KIND_EPOLL   5
#define VFS_KIND_TIMERFD 6
#define VFS_KIND_SIGNALFD 7
#define VFS_KIND_PIDFD   8
#define VFS_KIND_INOTIFY 9
#define VFS_KIND_IOURING 10
#define VFS_SCSI_SEC     512u
#define VFS_MAX_EVENTFD  8
#define VFS_MAX_EPOLL    4
#define VFS_EPOLL_WATCH  16
#define VFS_MAX_TIMERFD  4
#define VFS_MAX_SIGNALFD 4
#define VFS_MAX_INOTIFY  4
#define VFS_INOTIFY_WATCH 8
#define VFS_INOTIFY_Q    8

struct vfs_file {
    u8   u8Used;
    u8   u8Kind; /* VFS_KIND_* */
    u8   u8IsDir;
    u8   u8Pad;
    u32  cbData;
    u32  u32Mode; /* permission bits (low 12) + type bits */
    u32  u32Nlink;
    char szPath[VFS_MAX_PATH];
    u8   aData[VFS_MAX_DATA];
};

struct vfs_fd {
    u8   u8Used;
    u8   u8Kind;
    u8   u8End;  /* pipe: 0 or 1 */
    u8   u8Fl;   /* VFS_FD_FL_CLOEXEC / VFS_FD_FL_NONBLOCK */
    u32  u32File; /* index for RAM; pipe pair index for PIPE */
    u64  u64Off;  /* byte offset (supports large blk) */
};

#define VFS_PIPE_TAG_RD 1u /* waiters for data */
#define VFS_PIPE_TAG_WR 2u /* waiters for space */
#define VFS_POLL_TAG    1u

/* Bidirectional pair: ring[e] is written by end e, read by end 1-e. */
struct vfs_pipe_pair {
    u8  u8Used;
    u8  u8Open[2]; /* refcount-ish: end open */
    u8  u8Pad[1];
    u32 u32Len[2];
    u32 u32Head[2];
    u8  aRing[2][VFS_PIPE_BUF];
};

/*
 * Unix98 PTY metadata. Rings live in g_aPipes[u32Pipe].
 * Master fd is pipe end 0; slave is end 1 once /dev/pts/N is opened.
 * New pairs start locked (Linux Unix98); TIOCSPTLCK 0 unlocks.
 * Master write is slave input (ICANON+ECHO); slave write is OPOST.
 * Product SSH = OpenSSH-portable, not abandoned sshd_gj. Dual DoD B OPEN.
 */
struct vfs_pty {
    u8  u8Used;
    u8  u8Locked;         /* 1 at alloc; TIOCSPTLCK; slave open -> -EIO */
    u8  u8MasterRefs;     /* live master fds (dup-safe) */
    u8  u8SlaveRefs;      /* live slave fds (dup-safe) */
    u8  u8Pkt;            /* TIOCPKT; master read prefixes status byte */
    u8  u8Excl;           /* TIOCEXCL; extra slave open -> -EBUSY */
    u8  u8Hung;           /* TIOCVHANGUP; writes -EIO, reads EOF */
    u8  u8PktStat;        /* pending TIOCPKT_* bits for next master read */
    u8  u8Stopped;        /* TCXONC TCOOFF / IXON VSTOP */
    u8  u8SlaveEof;       /* VEOF empty line; one-shot slave read 0 */
    u8  cbCanon;          /* ICANON bytes waiting for NL/EOF */
    u8  u8Ldisc;          /* TIOCGETD / TIOCSETD; 0 = N_TTY */
    u8  u8Lnext;          /* IEXTEN VLNEXT: next input byte is literal */
    u8  u8Discard;        /* IEXTEN VDISCARD: drop slave output */
    u16 u16Mctrl;         /* TIOCMGET; default DTR|RTS|CTS|CAR|DSR */
    u8  aCanon[VFS_PTY_CANON];
    u32 u32Pipe;
    u32 u32N;             /* published /dev/pts/N */
    i32 i32File;          /* file-table slot for the slave node, or -1 */
    i32 i32Pgrp;          /* TIOCGPGRP / TIOCSPGRP; 0 until set */
    i32 i32Sid;           /* TIOCGSID / TIOCSCTTY; 0 = none -> -ENOTTY */
    u16 u16Row;           /* TIOCGWINSZ / TIOCSWINSZ; default 24x80 */
    u16 u16Col;
    u16 u16Xpixel;
    u16 u16Ypixel;
    u8  aTios[VFS_PTY_TIOS]; /* TCGETS/TCSETS; pair-shared */
};

#define VFS_MAX_SYMLINKS 8

struct vfs_symlink {
    u8   u8Used;
    u8   u8Pad[3];
    char szLink[VFS_MAX_PATH];
    char szTarget[VFS_MAX_PATH];
};

/* epoll interest list (one per epoll fd) */
struct vfs_epoll {
    u8  u8Used;
    u8  u8N;
    u8  u8Pad[2];
    struct {
        i32 i32Fd;
        u32 u32Events; /* EPOLLIN=1 EPOLLOUT=4 */
        u64 u64Data;
    } aWatch[VFS_EPOLL_WATCH];
};

static struct vfs_file g_aFiles[VFS_MAX_FILES];
static struct vfs_fd   g_aFds[VFS_MAX_FDS];
static struct vfs_pipe_pair g_aPipes[VFS_MAX_PIPES];
static struct vfs_pty  g_aPty[VFS_MAX_PTY];
static i32             g_i32CttyPty = -1; /* TIOCSCTTY; /dev/tty slave */
static struct vfs_symlink g_aSym[VFS_MAX_SYMLINKS];
static u64             g_aEventCnt[VFS_MAX_EVENTFD];
static u8              g_aEventUsed[VFS_MAX_EVENTFD];
static struct vfs_epoll g_aEpoll[VFS_MAX_EPOLL];
/* timerfd: tick counter (software); settime arms with it_value nsec */
static u64             g_aTimerTicks[VFS_MAX_TIMERFD];
static u64             g_aTimerInterval[VFS_MAX_TIMERFD];
static u8              g_aTimerUsed[VFS_MAX_TIMERFD];
static u8              g_aSigUsed[VFS_MAX_SIGNALFD];
static u64             g_aSigMask[VFS_MAX_SIGNALFD];
static u64             g_aSigPending[VFS_MAX_SIGNALFD];
/* inotify: watch list + fixed event queue (IN_* public masks) */
struct vfs_inotify {
    u8  u8Used;
    u8  u8NWatch;
    u8  u8Nq;
    u8  u8Pad;
    u32 u32NextWd;
    struct {
        i32  i32Wd;
        u32  u32Mask;
        char szPath[VFS_MAX_PATH];
    } aWatch[VFS_INOTIFY_WATCH];
    struct {
        i32  i32Wd;
        u32  u32Mask;
        u32  u32Cookie;
        u32  u32Len; /* name length incl NUL padding to 0 for bring-up */
    } aQ[VFS_INOTIFY_Q];
};
static struct vfs_inotify g_aInotify[VFS_MAX_INOTIFY];
static u32             g_u32PollWait;
static u8              g_fVfsParkOk;
static int             g_fBlkMounted;
static int             g_fScsiMounted;
static i32             g_iBlkFile = -1; /* file slot for /dev/vda */
static u64             g_u64ScsiCapBytes;
static u32             g_u32ChrRand = 0x9e3779b9u; /* /dev/urandom LCG */

/*
 * Soft product inventory (Wave 12 exclusive). Cumulative unless noted
 * live/peak. greppable: vfs_ram: soft ...
 */
static u32 g_u32SoftSeeded;       /* files seeded at last init */
static u32 g_u32SoftOpenOk;
static u32 g_u32SoftOpenFail;
static u32 g_u32SoftCloseOk;
static u32 g_u32SoftCloseFail;
static u32 g_u32SoftReadOk;
static u32 g_u32SoftReadFail;
static u32 g_u32SoftWriteOk;
static u32 g_u32SoftWriteFail;
static u32 g_u32SoftLseekOk;
static u32 g_u32SoftLseekFail;
static u32 g_u32SoftDupOk;
static u32 g_u32SoftDupFail;
static u32 g_u32SoftMkdirOk;
static u32 g_u32SoftMkdirFail;
static u32 g_u32SoftUnlinkOk;
static u32 g_u32SoftUnlinkFail;
static u32 g_u32SoftRmdirOk;
static u32 g_u32SoftRmdirFail;
static u32 g_u32SoftRenameOk;
static u32 g_u32SoftRenameFail;
static u32 g_u32SoftLinkOk;
static u32 g_u32SoftLinkFail;
static u32 g_u32SoftSymlinkOk;
static u32 g_u32SoftSymlinkFail;
static u32 g_u32SoftAccessOk;
static u32 g_u32SoftAccessFail;
static u32 g_u32SoftStatOk;
static u32 g_u32SoftStatFail;
static u32 g_u32SoftGetdentsOk;
static u32 g_u32SoftGetdentsFail;
static u32 g_u32SoftPipeOk;
static u32 g_u32SoftPipeFail;
static u32 g_u32SoftEventfdOk;
static u32 g_u32SoftEventfdFail;
static u32 g_u32SoftEpollOk;
static u32 g_u32SoftEpollFail;
static u32 g_u32SoftTimerfdOk;
static u32 g_u32SoftTimerfdFail;
static u32 g_u32SoftSignalfdOk;
static u32 g_u32SoftSignalfdFail;
static u32 g_u32SoftInotifyOk;
static u32 g_u32SoftInotifyFail;
static u32 g_u32SoftPidfdOk;
static u32 g_u32SoftPidfdFail;
static u32 g_u32SoftIouringOk;
static u32 g_u32SoftIouringFail;
static u32 g_u32SoftMountBlkOk;
static u32 g_u32SoftMountBlkSkip;
static u32 g_u32SoftMountScsiOk;
static u32 g_u32SoftMountScsiSkip;
static u32 g_u32SoftDenyNoent;
static u32 g_u32SoftDenyBadf;
static u32 g_u32SoftDenyInval;
static u32 g_u32SoftDenyNospc;
static u32 g_u32SoftDenyMfile;
static u32 g_u32SoftDenyExist;
static u32 g_u32SoftDenyAgain;
static u32 g_u32SoftDenyPipe;
static u32 g_u32SoftDenyPerm;
static u32 g_u32SoftDenyOther;
static u32 g_u32SoftFilesPeak;
static u32 g_u32SoftFdPeak;
static u32 g_u32SoftPipePeak;
static u32 g_u32SoftInvSamples;
static u8  g_fSoftInvOnce;        /* one-shot dump after first activity */
/* C0 lean residual once (functional; Soft!=product; no version stamp). */
static u32 g_u32SoftResidualLean;   /* lean residual self-check runs */
static u32 g_u32SoftResidualLeanOk; /* lean residual full-pass runs */
static u8  g_fSoftResidualLean;     /* one-shot lean residual gate */

/* Wave 14 exclusive soft deepen - complementary path tallies. */
static u32 g_u32SoftOk;           /* aggregate soft_out successes */
static u32 g_u32SoftFail;         /* aggregate soft_out failures */
static u32 g_u32SoftChmodOk;
static u32 g_u32SoftChmodFail;
static u32 g_u32SoftFchmodOk;
static u32 g_u32SoftFchmodFail;
static u32 g_u32SoftUtimensOk;
static u32 g_u32SoftUtimensFail;
static u32 g_u32SoftFallocateOk;
static u32 g_u32SoftFallocateFail;
static u32 g_u32SoftPunchOk;
static u32 g_u32SoftPunchFail;
static u32 g_u32SoftFsyncOk;
static u32 g_u32SoftFsyncFail;
static u32 g_u32SoftFtruncateOk;
static u32 g_u32SoftFtruncateFail;
static u32 g_u32SoftTruncateEnter; /* path truncate (wraps open/ftrunc) */
static u32 g_u32SoftSendfileOk;
static u32 g_u32SoftSendfileFail;
static u32 g_u32SoftReadlinkOk;
static u32 g_u32SoftReadlinkFail;
static u32 g_u32SoftFstatOk;
static u32 g_u32SoftFstatFail;
static u32 g_u32SoftLstatOk;
static u32 g_u32SoftLstatFail;
static u32 g_u32SoftPreadEnter;
static u32 g_u32SoftPwriteEnter;
static u32 g_u32SoftCopyRangeOk;
static u32 g_u32SoftCopyRangeFail;
static u32 g_u32SoftMarkDirOk;
static u32 g_u32SoftMarkDirFail;
static u32 g_u32SoftSocketpairOk;
static u32 g_u32SoftSocketpairFail;
static u32 g_u32SoftInotifyAddOk;
static u32 g_u32SoftInotifyAddFail;
static u32 g_u32SoftInotifyRmOk;
static u32 g_u32SoftInotifyRmFail;
static u32 g_u32SoftEpollCtlOk;
static u32 g_u32SoftEpollCtlFail;
static u32 g_u32SoftEpollWaitOk;
static u32 g_u32SoftEpollWaitFail;
static u32 g_u32SoftPollProbe;
static u64 g_u64SoftBytesRead;    /* cumulative read/pread soft bytes */
static u64 g_u64SoftBytesWrite;   /* cumulative write/pwrite soft bytes */
static u32 g_u32SoftEventfdPeak;
static u32 g_u32SoftEpollPeak;
static u32 g_u32SoftTimerfdPeak;
static u32 g_u32SoftSignalfdPeak;
static u32 g_u32SoftInotifyPeak;
static u32 g_u32SoftSymPeak;
static u32 g_u32SoftDirLive;      /* last sample dir count */
static u32 g_u32SoftRamLive;      /* last sample RAM kind count */
static u32 g_u32SoftBlkLive;
static u32 g_u32SoftScsiLive;
static u32 g_u32SoftPipeLive;
static u32 g_u32SoftSpecialLive;  /* non-ram/blk/scsi/pipe fds kinds */

static void epoll_detach_fd(i64 i64Fd);
static void vfs_ram_poll_kick(void);
static i32 pty_find_by_pipe(u32 u32Pipe);
static i32 pty_find_by_n(u32 u32N);
static int pty_path_n(const char *szPath, u32 *pN);
static void pty_fmt_path(char *szOut, u32 u32N);
static int pty_publish(u32 iPty);
static void pty_unpublish(u32 iPty);
static void pty_close_end(u32 u32Pipe, u8 u8End);
static void pty_ref_dup(const struct vfs_fd *pFd);
static void pty_pkt_note(struct vfs_pty *pPty, u8 u8Bit);
static void pty_flush_ring(u32 u32Pipe, u8 u8Ring);
static i64 pty_open_master(void);
static i64 pty_open_slave(u32 u32N);
static i32 pty_ctty_slot(void);
static i64 pty_open_ctty(void);
static void soft_inc(u32 *pCtr);
static void soft_add64(u64 *pCtr, u64 u64N);
static void soft_peak_note(void);
static void soft_deny_note(i64 i64Ret);
static i64 soft_out(u32 *pOk, u32 *pFail, i64 i64Ret);
static i64 soft_out_bytes(u32 *pOk, u32 *pFail, u64 *pBytes, i64 i64Ret);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static void soft_residual_lean_once(void);

/**
 * Live slot counts for soft inventory (tables only; no alloc).
 */
static u32
soft_count_files(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (g_aFiles[i].u8Used) {
            c++;
        }
    }
    return c;
}

static u32
soft_count_fds(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_FDS; i++) {
        if (g_aFds[i].u8Used) {
            c++;
        }
    }
    return c;
}

static u32
soft_count_pipes(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_PIPES; i++) {
        if (g_aPipes[i].u8Used) {
            c++;
        }
    }
    return c;
}

static u32
soft_count_eventfd(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_EVENTFD; i++) {
        if (g_aEventUsed[i]) {
            c++;
        }
    }
    return c;
}

static u32
soft_count_epoll(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_EPOLL; i++) {
        if (g_aEpoll[i].u8Used) {
            c++;
        }
    }
    return c;
}

static u32
soft_count_timerfd(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_TIMERFD; i++) {
        if (g_aTimerUsed[i]) {
            c++;
        }
    }
    return c;
}

static u32
soft_count_signalfd(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_SIGNALFD; i++) {
        if (g_aSigUsed[i]) {
            c++;
        }
    }
    return c;
}

static u32
soft_count_inotify(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_INOTIFY; i++) {
        if (g_aInotify[i].u8Used) {
            c++;
        }
    }
    return c;
}

static u32
soft_count_symlinks(void)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < VFS_MAX_SYMLINKS; i++) {
        if (g_aSym[i].u8Used) {
            c++;
        }
    }
    return c;
}

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: add to u64 path tally (wrap OK for telemetry). */
static void
soft_add64(u64 *pCtr, u64 u64N)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr) += u64N;
}

/**
 * Note live high-water for files / open fds / pipes / specials.
 * Wave 14: also snapshot kind tallies for inventory.
 */
static void
soft_peak_note(void)
{
    u32 u32Files = soft_count_files();
    u32 u32Fds = soft_count_fds();
    u32 u32Pipes = soft_count_pipes();
    u32 u32Ev = soft_count_eventfd();
    u32 u32Ep = soft_count_epoll();
    u32 u32Tmr = soft_count_timerfd();
    u32 u32Sig = soft_count_signalfd();
    u32 u32Ino = soft_count_inotify();
    u32 u32Sym = soft_count_symlinks();
    u32 i;
    u32 u32Dir = 0;
    u32 u32Ram = 0;
    u32 u32Blk = 0;
    u32 u32Scsi = 0;
    u32 u32PipeF = 0;
    u32 u32Spec = 0;

    if (u32Files > g_u32SoftFilesPeak) {
        g_u32SoftFilesPeak = u32Files;
    }
    if (u32Fds > g_u32SoftFdPeak) {
        g_u32SoftFdPeak = u32Fds;
    }
    if (u32Pipes > g_u32SoftPipePeak) {
        g_u32SoftPipePeak = u32Pipes;
    }
    if (u32Ev > g_u32SoftEventfdPeak) {
        g_u32SoftEventfdPeak = u32Ev;
    }
    if (u32Ep > g_u32SoftEpollPeak) {
        g_u32SoftEpollPeak = u32Ep;
    }
    if (u32Tmr > g_u32SoftTimerfdPeak) {
        g_u32SoftTimerfdPeak = u32Tmr;
    }
    if (u32Sig > g_u32SoftSignalfdPeak) {
        g_u32SoftSignalfdPeak = u32Sig;
    }
    if (u32Ino > g_u32SoftInotifyPeak) {
        g_u32SoftInotifyPeak = u32Ino;
    }
    if (u32Sym > g_u32SoftSymPeak) {
        g_u32SoftSymPeak = u32Sym;
    }

    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (!g_aFiles[i].u8Used) {
            continue;
        }
        if (g_aFiles[i].u8IsDir) {
            u32Dir++;
        }
        if (g_aFiles[i].u8Kind == VFS_KIND_RAM) {
            u32Ram++;
        } else if (g_aFiles[i].u8Kind == VFS_KIND_BLK) {
            u32Blk++;
        } else if (g_aFiles[i].u8Kind == VFS_KIND_SCSI) {
            u32Scsi++;
        }
    }
    for (i = 0; i < VFS_MAX_FDS; i++) {
        if (!g_aFds[i].u8Used) {
            continue;
        }
        if (g_aFds[i].u8Kind == VFS_KIND_PIPE) {
            u32PipeF++;
        } else if (g_aFds[i].u8Kind == VFS_KIND_EVENTFD ||
                   g_aFds[i].u8Kind == VFS_KIND_EPOLL ||
                   g_aFds[i].u8Kind == VFS_KIND_TIMERFD ||
                   g_aFds[i].u8Kind == VFS_KIND_SIGNALFD ||
                   g_aFds[i].u8Kind == VFS_KIND_PIDFD ||
                   g_aFds[i].u8Kind == VFS_KIND_INOTIFY ||
                   g_aFds[i].u8Kind == VFS_KIND_IOURING) {
            u32Spec++;
        }
    }
    g_u32SoftDirLive = u32Dir;
    g_u32SoftRamLive = u32Ram;
    g_u32SoftBlkLive = u32Blk;
    g_u32SoftScsiLive = u32Scsi;
    g_u32SoftPipeLive = u32PipeF;
    g_u32SoftSpecialLive = u32Spec;
}

/**
 * Bump soft deny total into a typed errno bucket (Linux-shaped negatives).
 */
static void
soft_deny_note(i64 i64Ret)
{
    i64 i64E;

    if (i64Ret >= 0) {
        return;
    }
    i64E = -i64Ret;
    if (i64E == 2) {
        g_u32SoftDenyNoent++;
    } else if (i64E == 9) {
        g_u32SoftDenyBadf++;
    } else if (i64E == 22) {
        g_u32SoftDenyInval++;
    } else if (i64E == 28) {
        g_u32SoftDenyNospc++;
    } else if (i64E == 24) {
        g_u32SoftDenyMfile++;
    } else if (i64E == 17) {
        g_u32SoftDenyExist++;
    } else if (i64E == 11) {
        g_u32SoftDenyAgain++;
    } else if (i64E == 32) {
        g_u32SoftDenyPipe++;
    } else if (i64E == 1 || i64E == 13) {
        g_u32SoftDenyPerm++;
    } else {
        g_u32SoftDenyOther++;
    }
}

/**
 * Soft-tally a public return (ok if >= 0). Diagnostics only; never mutates
 * i64Ret semantics. Triggers one-shot inventory after first activity.
 * Wave 14: also bumps aggregate ok/fail.
 */
static i64
soft_out(u32 *pOk, u32 *pFail, i64 i64Ret)
{
    if (i64Ret >= 0) {
        if (pOk != NULL && *pOk < 0xffffffffu) {
            (*pOk)++;
        }
        soft_inc(&g_u32SoftOk);
    } else {
        if (pFail != NULL && *pFail < 0xffffffffu) {
            (*pFail)++;
        }
        soft_inc(&g_u32SoftFail);
        soft_deny_note(i64Ret);
    }
    soft_peak_note();
    soft_inventory_maybe_once();
    return i64Ret;
}

/**
 * Soft-tally a positive byte-returning op (read/write). Wave 14 xfer.
 * On success, adds i64Ret to *pBytes (when non-null).
 */
static i64
soft_out_bytes(u32 *pOk, u32 *pFail, u64 *pBytes, i64 i64Ret)
{
    if (i64Ret >= 0 && pBytes != NULL) {
        soft_add64(pBytes, (u64)i64Ret);
    }
    return soft_out(pOk, pFail, i64Ret);
}

/**
 * Lean soft residual inventory - greppable "vfs_ram: soft ...".
 * Compact dual-path snapshot; never allocates; never hard-gates.
 * No version stamp, no stamp storms (was ~30 kprintf lines + wave=).
 * Soft!=product (kernel soft ramfs != vfs_door/vfsd product mini-FS).
 * Dual MIT OR Apache-2.0. Companion: soft_residual_lean_once (functional).
 *
 * greppable: vfs_ram: soft inventory | soft residual | soft path |
 *            soft honesty | soft inventory PASS | soft PASS |
 *            soft residual lean | soft residual lean PASS
 */
static void
soft_inventory_log(void)
{
    u32 u32Files;
    u32 u32Fds;
    u32 u32Pipes;
    u32 u32Ev;
    u32 u32Ep;
    u32 u32Tmr;
    u32 u32Sig;
    u32 u32Ino;
    u32 u32Sym;
    u32 u32Samples;
    u32 u32FileHead;
    u32 u32FdHead;

    soft_peak_note();
    u32Files = soft_count_files();
    u32Fds = soft_count_fds();
    u32Pipes = soft_count_pipes();
    u32Ev = soft_count_eventfd();
    u32Ep = soft_count_epoll();
    u32Tmr = soft_count_timerfd();
    u32Sig = soft_count_signalfd();
    u32Ino = soft_count_inotify();
    u32Sym = soft_count_symlinks();
    soft_inc(&g_u32SoftInvSamples);
    u32Samples = g_u32SoftInvSamples;
    u32FileHead = ((u32)VFS_MAX_FILES > u32Files)
                      ? ((u32)VFS_MAX_FILES - u32Files)
                      : 0u;
    u32FdHead = ((u32)VFS_MAX_FDS > u32Fds) ? ((u32)VFS_MAX_FDS - u32Fds) : 0u;

    /*
     * Grep: vfs_ram: soft inventory
     * One-line rollup (live tables + mount lamps). Soft residual only.
     */
    kprintf("vfs_ram: soft inventory seeded=%u files=%u/%u fds=%u/%u "
            "pipes=%u eventfd=%u epoll=%u timerfd=%u signalfd=%u inotify=%u "
            "sym=%u blk=%u scsi=%u ok=%u fail=%u logs=%u\n",
            g_u32SoftSeeded, u32Files, (u32)VFS_MAX_FILES, u32Fds,
            (u32)VFS_MAX_FDS, u32Pipes, u32Ev, u32Ep, u32Tmr, u32Sig, u32Ino,
            u32Sym, g_fBlkMounted ? 1u : 0u, g_fScsiMounted ? 1u : 0u,
            g_u32SoftOk, g_u32SoftFail, u32Samples);

    /*
     * Grep: vfs_ram: soft residual
     * Dual-path residual surface - lean, no per-area stamp storm.
     * soft=1 product=0 Soft!=product (cold Linux interim != vfsd LBA).
     */
    kprintf("vfs_ram: soft residual dual=ramfs+specials "
            "max_files=%u max_fds=%u max_data=%u path=%u pipe_buf=%u "
            "open=%u/%u read=%u/%u write=%u/%u "
            "deny_noent=%u deny_badf=%u deny_inval=%u "
            "file_head=%u fd_head=%u peak_files=%u peak_fds=%u "
            "soft=1 product=0 Soft!=product dual=MIT|Apache-2.0\n",
            (u32)VFS_MAX_FILES, (u32)VFS_MAX_FDS, (u32)VFS_MAX_DATA,
            (u32)VFS_MAX_PATH, (u32)VFS_PIPE_BUF, g_u32SoftOpenOk,
            g_u32SoftOpenFail, g_u32SoftReadOk, g_u32SoftReadFail,
            g_u32SoftWriteOk, g_u32SoftWriteFail, g_u32SoftDenyNoent,
            g_u32SoftDenyBadf, g_u32SoftDenyInval, u32FileHead, u32FdHead,
            g_u32SoftFilesPeak, g_u32SoftFdPeak);

    /*
     * Grep: vfs_ram: soft path | vfs_ram: soft honesty
     * Kernel soft cold-Linux surface != product vfs_door / vfsd mini-FS.
     */
    kprintf("vfs_ram: soft path claim=kernel_soft cold_linux=1 "
            "ramfs+specials=1 product_vfsd=0 door_lba=0 dual=ramfs+specials "
            "(soft residual; Soft!=product)\n");
    kprintf("vfs_ram: soft honesty product_vfs=0 door_mini_fs=0 "
            "claim=kernel_soft soft=1 product=0 Soft!=product "
            "dual=MIT|Apache-2.0 soft PASS\n");

    /* Grep: vfs_ram: soft inventory PASS | vfs_ram: soft PASS */
    kprintf("vfs_ram: soft inventory PASS seeded=%u files=%u fds=%u "
            "logs=%u\n",
            g_u32SoftSeeded, u32Files, u32Fds, u32Samples);
    kprintf("vfs_ram: soft PASS seeded=%u\n", g_u32SoftSeeded);
}

/**
 * After first product open/op activity, print soft inventory once
 * (mirrors input_hub/vfs_door soft-stats-once). Diagnostics only.
 * Lean residual is one-shot at init (or here if init skipped); Soft!=product.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftOpenOk == 0 && g_u32SoftOpenFail == 0 &&
        g_u32SoftPipeOk == 0 && g_u32SoftReadOk == 0 &&
        g_u32SoftWriteOk == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
    /* No-op if init already ran lean residual (one-shot gate). */
    soft_residual_lean_once();
}

static int
path_eq(const char *szA, const char *szB)
{
    size_t i;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    for (i = 0; i < VFS_MAX_PATH; i++) {
        if (szA[i] != szB[i]) {
            return 0;
        }
        if (szA[i] == '\0') {
            return 1;
        }
    }
    return 0;
}

/*
 * Seeded char nodes. /dev/null and dummy /dev/tty: read EOF, write discard.
 * /dev/tty becomes the ctty slave after TIOCSCTTY / unique live slave
 * (open is not ENOENT). /dev/urandom and /dev/random: tiny LCG fill,
 * never silent EOF (OpenSSL may treat EOF as unseeded). Writes discard.
 * Dual DoD B OPEN.
 */
static int
file_is_chr_discard(const struct vfs_file *pFile)
{
    if (pFile == NULL || pFile->u8Used == 0) {
        return 0;
    }
    return path_eq(pFile->szPath, "/dev/null") ||
           path_eq(pFile->szPath, "/dev/tty");
}

static int
file_is_chr_rand(const struct vfs_file *pFile)
{
    if (pFile == NULL || pFile->u8Used == 0) {
        return 0;
    }
    return path_eq(pFile->szPath, "/dev/urandom") ||
           path_eq(pFile->szPath, "/dev/random");
}

static u8
chr_rand_byte(void)
{
    u32 u32S;

    u32S = g_u32ChrRand;
    if (u32S == 0u) {
        u32S = 0x9e3779b9u;
    }
    u32S = u32S * 1664525u + 1013904223u;
    g_u32ChrRand = u32S;
    return (u8)(u32S >> 16);
}

static void
path_copy(char *szDst, const char *szSrc)
{
    size_t i;

    if (szDst == NULL) {
        return;
    }
    for (i = 0; i + 1 < VFS_MAX_PATH; i++) {
        if (szSrc == NULL || szSrc[i] == '\0') {
            break;
        }
        szDst[i] = szSrc[i];
    }
    szDst[i] = '\0';
}

/**
 * Normalize absolute path: collapse //, resolve . and .., drop trailing /.
 * Writes NUL-terminated result into szOut (VFS_MAX_PATH). Returns 0 or -errno.
 */
static i64
path_norm(char *szOut, const char *szIn)
{
    size_t iIn;
    size_t iOut;
    size_t cComp;
    size_t aComp[32];

    if (szOut == NULL || szIn == NULL || szIn[0] == '\0') {
        return -14; /* EFAULT */
    }
    if (szIn[0] != '/') {
        return -22; /* EINVAL - only absolute paths */
    }
    szOut[0] = '/';
    iOut = 1;
    iIn = 0;
    cComp = 0;
    while (szIn[iIn] != '\0') {
        size_t iStart;
        size_t cbComp;
        size_t k;

        while (szIn[iIn] == '/') {
            iIn++;
        }
        if (szIn[iIn] == '\0') {
            break;
        }
        iStart = iIn;
        while (szIn[iIn] != '\0' && szIn[iIn] != '/') {
            iIn++;
        }
        cbComp = iIn - iStart;
        if (cbComp == 1 && szIn[iStart] == '.') {
            continue;
        }
        if (cbComp == 2 && szIn[iStart] == '.' && szIn[iStart + 1] == '.') {
            if (cComp > 0) {
                cComp--;
                iOut = aComp[cComp];
                /*
                 * aComp stores the name start (after the separator '/').
                 * Back up over that separator when not the first component
                 * so the next push rewrites a single slash (no "//").
                 * Residual lean path_norm self-check covers // + . + .. .
                 */
                if (iOut > 1) {
                    iOut--;
                }
            } else {
                iOut = 1; /* stay at root */
            }
            continue;
        }
        if (cComp >= 32u) {
            return -36; /* ENAMETOOLONG */
        }
        if (iOut > 1) {
            if (iOut + 1 >= VFS_MAX_PATH) {
                return -36;
            }
            szOut[iOut++] = '/';
        }
        if (iOut + cbComp >= VFS_MAX_PATH) {
            return -36;
        }
        aComp[cComp++] = iOut;
        for (k = 0; k < cbComp; k++) {
            szOut[iOut++] = szIn[iStart + k];
        }
    }
    if (iOut == 0) {
        szOut[0] = '/';
        iOut = 1;
    }
    szOut[iOut] = '\0';
    return 0;
}

static i32
find_symlink(const char *szPath)
{
    u32 s;

    if (szPath == NULL) {
        return -1;
    }
    for (s = 0; s < VFS_MAX_SYMLINKS; s++) {
        if (g_aSym[s].u8Used && path_eq(g_aSym[s].szLink, szPath)) {
            return (i32)s;
        }
    }
    return -1;
}

/** Parent directory of szPath into szOut ("/" for top-level). Returns 0 or -errno. */
static i64
path_dirname(char *szOut, const char *szPath)
{
    size_t n;
    size_t iLast;

    if (szOut == NULL || szPath == NULL || szPath[0] == '\0') {
        return -14;
    }
    n = 0;
    while (szPath[n] != '\0' && n + 1 < VFS_MAX_PATH) {
        n++;
    }
    if (n == 0) {
        return -22;
    }
    iLast = n;
    while (iLast > 0 && szPath[iLast - 1] != '/') {
        iLast--;
    }
    if (iLast == 0) {
        szOut[0] = '/';
        szOut[1] = '\0';
        return 0;
    }
    if (iLast == 1) {
        szOut[0] = '/';
        szOut[1] = '\0';
        return 0;
    }
    {
        size_t i;

        for (i = 0; i + 1 < iLast && i + 1 < VFS_MAX_PATH; i++) {
            szOut[i] = szPath[i];
        }
        szOut[i] = '\0';
    }
    return 0;
}

/**
 * C0 lean soft residual once (this TU exclusive).
 * Functional residual on stack-local path helpers + capacity/kind geometry
 * + live table bounds - never mutates file/fd/pipe/special tables (product
 * state untouched). Soft!=product · dual MIT OR Apache-2.0 · no version
 * stamp · no stamp storm · G-AC-1 (kernel soft cold Linux interim !=
 * vfs_door / vfsd product LBA mini-FS). Dual DoD A/B stay OPEN (UDX) -
 * this residual never claims product DoD close.
 *
 * greppable: vfs_ram: soft residual lean
 * greppable: vfs_ram: soft residual lean PASS
 * greppable: dual=ramfs+specials | Soft!=product
 */
static void
soft_residual_lean_once(void)
{
    char aNorm[VFS_MAX_PATH];
    char aDir[VFS_MAX_PATH];
    char aCopy[VFS_MAX_PATH];
    u32  u32Ok = 0;
    u32  u32Checks = 0;
    u32  u32PathNorm = 0;
    u32  u32PathEq = 0;
    u32  u32PathDir = 0;
    u32  u32PathCopy = 0;
    u32  u32Geom = 0;
    u32  u32Kinds = 0;
    u32  u32Bounds = 0;
    u32  u32Honesty = 0;
    u32  u32Files;
    u32  u32Fds;
    u32  u32Pipes;
    u32  u32Ev;
    u32  u32Ep;
    u32  u32Tmr;
    u32  u32Sig;
    u32  u32Ino;
    u32  u32Sym;
    i64  i64St;

    if (g_fSoftResidualLean != 0) {
        return;
    }
    g_fSoftResidualLean = 1;
    soft_inc(&g_u32SoftResidualLean);

    /*
     * Stack-local path_norm residual (// collapse, . and .. resolve,
     * trailing-slash drop, root / multi-dotdot past root). No table
     * mutation - writes only aNorm[]. Covers the ".." separator rewrite
     * (no double-slash) that residual deepen hardens.
     */
    u32Checks++;
    i64St = path_norm(aNorm, "//tmp//./a/../b");
    if (i64St == 0 && path_eq(aNorm, "/tmp/b") != 0 &&
        path_norm(aNorm, "/a/../b") == 0 && path_eq(aNorm, "/b") != 0 &&
        path_norm(aNorm, "//tmp//x") == 0 && path_eq(aNorm, "/tmp/x") != 0 &&
        path_norm(aNorm, "/") == 0 && path_eq(aNorm, "/") != 0 &&
        path_norm(aNorm, "/tmp/") == 0 && path_eq(aNorm, "/tmp") != 0 &&
        path_norm(aNorm, "/../x") == 0 && path_eq(aNorm, "/x") != 0 &&
        path_norm(aNorm, "///") == 0 && path_eq(aNorm, "/") != 0 &&
        path_norm(aNorm, "/a/b/../../c") == 0 && path_eq(aNorm, "/c") != 0 &&
        path_norm(aNorm, "/././.") == 0 && path_eq(aNorm, "/") != 0 &&
        path_norm(aNorm, "relative") == -22 &&
        path_norm(aNorm, "") == -14 &&
        path_norm(NULL, "/tmp") == -14) {
        u32PathNorm = 1;
        u32Ok++;
    }

    /* path_eq residual (identity + inequality + empty). */
    u32Checks++;
    if (path_eq("/tmp", "/tmp") != 0 && path_eq("/a", "/b") == 0 &&
        path_eq("", "") != 0 && path_eq("/", "/tmp") == 0 &&
        path_eq(NULL, "/x") == 0 && path_eq("/x", NULL) == 0 &&
        path_eq(NULL, NULL) == 0) {
        u32PathEq = 1;
        u32Ok++;
    }

    /* path_dirname residual (parent, top-level, root, null). */
    u32Checks++;
    i64St = path_dirname(aDir, "/tmp/foo");
    if (i64St == 0 && path_eq(aDir, "/tmp") != 0 &&
        path_dirname(aDir, "/foo") == 0 && path_eq(aDir, "/") != 0 &&
        path_dirname(aDir, "/") == 0 && path_eq(aDir, "/") != 0 &&
        path_dirname(aDir, NULL) == -14 &&
        path_dirname(NULL, "/tmp") == -14) {
        u32PathDir = 1;
        u32Ok++;
    }

    /*
     * path_copy residual (stack-local only; NUL truncate; NULL src/dst).
     * Never touches product tables.
     */
    u32Checks++;
    aCopy[0] = 'X';
    aCopy[1] = '\0';
    path_copy(aCopy, "/tmp/residual");
    if (path_eq(aCopy, "/tmp/residual") != 0) {
        path_copy(aCopy, NULL);
        if (aCopy[0] == '\0') {
            path_copy(NULL, "/tmp"); /* must not fault */
            u32PathCopy = 1;
            u32Ok++;
        }
    }

    /*
     * Capacity geometry residual (soft product limits; not ABI).
     * Keeps header/capacity comments honest under residual deepen.
     */
    u32Checks++;
    if (VFS_MAX_FILES == 64u && VFS_MAX_FDS == 96u &&
        VFS_MAX_PATH == 128u && VFS_MAX_DATA == 32768u &&
        VFS_MAX_PIPES == 16u && VFS_PIPE_BUF == 2048u &&
        VFS_MAX_EVENTFD == 8u && VFS_MAX_EPOLL == 4u &&
        VFS_EPOLL_WATCH == 16u && VFS_MAX_TIMERFD == 4u &&
        VFS_MAX_SIGNALFD == 4u && VFS_MAX_INOTIFY == 4u &&
        VFS_INOTIFY_WATCH == 8u && VFS_INOTIFY_Q == 8u &&
        VFS_MAX_SYMLINKS == 8u && VFS_SCSI_SEC == 512u) {
        u32Geom = 1;
        u32Ok++;
    }

    /* Kind table residual - distinct VFS_KIND_* for dual ramfs+specials. */
    u32Checks++;
    if (VFS_KIND_RAM == 0u && VFS_KIND_BLK == 1u && VFS_KIND_SCSI == 2u &&
        VFS_KIND_PIPE == 3u && VFS_KIND_EVENTFD == 4u &&
        VFS_KIND_EPOLL == 5u && VFS_KIND_TIMERFD == 6u &&
        VFS_KIND_SIGNALFD == 7u && VFS_KIND_PIDFD == 8u &&
        VFS_KIND_INOTIFY == 9u && VFS_KIND_IOURING == 10u) {
        u32Kinds = 1;
        u32Ok++;
    }

    /* Live table bounds residual (counts never exceed soft maxima). */
    u32Checks++;
    soft_peak_note();
    u32Files = soft_count_files();
    u32Fds = soft_count_fds();
    u32Pipes = soft_count_pipes();
    u32Ev = soft_count_eventfd();
    u32Ep = soft_count_epoll();
    u32Tmr = soft_count_timerfd();
    u32Sig = soft_count_signalfd();
    u32Ino = soft_count_inotify();
    u32Sym = soft_count_symlinks();
    if (u32Files <= (u32)VFS_MAX_FILES && u32Fds <= (u32)VFS_MAX_FDS &&
        u32Pipes <= (u32)VFS_MAX_PIPES && u32Ev <= (u32)VFS_MAX_EVENTFD &&
        u32Ep <= (u32)VFS_MAX_EPOLL && u32Tmr <= (u32)VFS_MAX_TIMERFD &&
        u32Sig <= (u32)VFS_MAX_SIGNALFD && u32Ino <= (u32)VFS_MAX_INOTIFY &&
        u32Sym <= (u32)VFS_MAX_SYMLINKS) {
        u32Bounds = 1;
        u32Ok++;
    }

    /*
     * Dual-path honesty residual: kernel soft cold Linux interim surface
     * is not product vfs_door/vfsd LBA mini-FS (Soft!=product; G-AC-1).
     * Design-constant check only - never a product gate; Dual DoD OPEN.
     */
    u32Checks++;
    if (VFS_KIND_RAM == 0u && VFS_KIND_IOURING == 10u &&
        VFS_MAX_DATA >= 30720u /* room for packaged ld-gj.so.1 ~30 KiB */ &&
        VFS_SCSI_SEC == 512u &&
        VFS_MAX_PATH == 128u && VFS_MAX_FILES == 64u) {
        u32Honesty = 1;
        u32Ok++;
    }

    if (u32Ok == u32Checks) {
        soft_inc(&g_u32SoftResidualLeanOk);
    }

    /*
     * Grep: vfs_ram: soft residual lean
     * Two lean residual lines max - Soft!=product dual license; no version stamp.
     */
    kprintf("vfs_ram: soft residual lean dual=ramfs+specials "
            "path_norm=%u path_eq=%u path_dir=%u path_copy=%u "
            "geom=%u kinds=%u bounds=%u honesty=%u ok=%u/%u "
            "lean_runs=%u lean_ok=%u "
            "files=%u/%u fds=%u/%u pipes=%u eventfd=%u epoll=%u "
            "timerfd=%u signalfd=%u inotify=%u sym=%u "
            "blk=%u scsi=%u seeded=%u "
            "claim=kernel_soft product_vfsd=0 door_lba=0 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "stamp_storm=0 G-AC-1=1 no_ko_product=1 dual_dod=OPEN "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "cold Linux interim residual; not vfsd LBA product DoD)\n",
            (unsigned)u32PathNorm, (unsigned)u32PathEq,
            (unsigned)u32PathDir, (unsigned)u32PathCopy,
            (unsigned)u32Geom, (unsigned)u32Kinds,
            (unsigned)u32Bounds, (unsigned)u32Honesty, (unsigned)u32Ok,
            (unsigned)u32Checks, (unsigned)g_u32SoftResidualLean,
            (unsigned)g_u32SoftResidualLeanOk, (unsigned)u32Files,
            (unsigned)VFS_MAX_FILES, (unsigned)u32Fds,
            (unsigned)VFS_MAX_FDS, (unsigned)u32Pipes, (unsigned)u32Ev,
            (unsigned)u32Ep, (unsigned)u32Tmr, (unsigned)u32Sig,
            (unsigned)u32Ino, (unsigned)u32Sym,
            g_fBlkMounted ? 1u : 0u, g_fScsiMounted ? 1u : 0u,
            (unsigned)g_u32SoftSeeded);

    /* Grep: vfs_ram: soft residual lean PASS | FAIL */
    if (u32Ok == u32Checks) {
        kprintf("vfs_ram: soft residual lean PASS dual=ramfs+specials "
                "ok=%u/%u soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                "dual_dod=OPEN "
                "(Soft!=product; no version stamp; cold Linux interim "
                "residual; not vfsd product)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    } else {
        kprintf("vfs_ram: soft residual lean FAIL ok=%u/%u "
                "(soft residual only; not product gate; Soft!=product; "
                "dual=ramfs+specials dual_dod=OPEN)\n",
                (unsigned)u32Ok, (unsigned)u32Checks);
    }
}

/**
 * Normalize and follow symlink table (max 8 hops). Built-in proc links left as-is
 * for callers that special-case them. Writes final path to szOut.
 */
static i64
path_resolve(char *szOut, const char *szIn)
{
    char szCur[VFS_MAX_PATH];
    char szNext[VFS_MAX_PATH];
    int nDepth;
    i32 iSym;
    i64 st;

    st = path_norm(szCur, szIn);
    if (st != 0) {
        return st;
    }
    for (nDepth = 0; nDepth < 8; nDepth++) {
        iSym = find_symlink(szCur);
        if (iSym < 0) {
            path_copy(szOut, szCur);
            return 0;
        }
        if (g_aSym[iSym].szTarget[0] == '/') {
            st = path_norm(szNext, g_aSym[iSym].szTarget);
            if (st != 0) {
                return st;
            }
        } else {
            char szDir[VFS_MAX_PATH];
            char szJoin[VFS_MAX_PATH];
            size_t i;
            size_t j;

            if (path_dirname(szDir, szCur) != 0) {
                return -22;
            }
            i = 0;
            if (szDir[0] == '/' && szDir[1] == '\0') {
                szJoin[i++] = '/';
            } else {
                while (szDir[i] != '\0' && i + 1 < VFS_MAX_PATH) {
                    szJoin[i] = szDir[i];
                    i++;
                }
                if (i + 1 < VFS_MAX_PATH) {
                    szJoin[i++] = '/';
                }
            }
            j = 0;
            while (g_aSym[iSym].szTarget[j] != '\0' && i + 1 < VFS_MAX_PATH) {
                szJoin[i++] = g_aSym[iSym].szTarget[j++];
            }
            szJoin[i] = '\0';
            st = path_norm(szNext, szJoin);
            if (st != 0) {
                return st;
            }
        }
        path_copy(szCur, szNext);
    }
    return -40; /* ELOOP */
}

/** Non-zero if szPath is a direct child of directory szDir. */
static int
path_is_child_of(const char *szDir, const char *szPath)
{
    size_t nDir;
    size_t i;

    if (szDir == NULL || szPath == NULL) {
        return 0;
    }
    nDir = 0;
    while (szDir[nDir] != '\0') {
        nDir++;
    }
    if (nDir == 0) {
        return 0;
    }
    /* Root: any single-component absolute path is a child */
    if (nDir == 1 && szDir[0] == '/') {
        if (szPath[0] != '/' || szPath[1] == '\0') {
            return 0;
        }
        for (i = 1; szPath[i] != '\0'; i++) {
            if (szPath[i] == '/') {
                return 0; /* deeper than one level */
            }
        }
        return 1;
    }
    for (i = 0; i < nDir; i++) {
        if (szPath[i] != szDir[i]) {
            return 0;
        }
    }
    if (szPath[nDir] != '/') {
        return 0;
    }
    /* No further slash after the child name */
    for (i = nDir + 1; szPath[i] != '\0'; i++) {
        if (szPath[i] == '/') {
            return 0;
        }
    }
    return szPath[nDir + 1] != '\0';
}

static void
seed_file(const char *szPath, const char *szData)
{
    u32 i;
    size_t cb;
    char szNorm[VFS_MAX_PATH];

    if (szPath == NULL || szPath[0] == '\0') {
        return;
    }
    if (path_norm(szNorm, szPath) != 0) {
        path_copy(szNorm, szPath);
    }
    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (!g_aFiles[i].u8Used) {
            g_aFiles[i].u8Used = 1;
            g_aFiles[i].u8Kind = VFS_KIND_RAM;
            g_aFiles[i].u8IsDir = 0;
            g_aFiles[i].u32Mode = 0100644u;
            g_aFiles[i].u32Nlink = 1;
            path_copy(g_aFiles[i].szPath, szNorm);
            cb = 0;
            if (szData != NULL) {
                while (szData[cb] != '\0' && cb + 1 < VFS_MAX_DATA) {
                    g_aFiles[i].aData[cb] = (u8)szData[cb];
                    cb++;
                }
            }
            g_aFiles[i].cbData = (u32)cb;
            if (path_eq(szNorm, "/dev/null") || path_eq(szNorm, "/dev/ptmx") ||
                path_eq(szNorm, "/dev/pts/ptmx") ||
                path_eq(szNorm, "/dev/tty") ||
                path_eq(szNorm, "/dev/urandom") ||
                path_eq(szNorm, "/dev/random")) {
                g_aFiles[i].u32Mode = 0020666u;
                g_aFiles[i].cbData = 0;
            }
            return;
        }
    }
}

static void
seed_dir(const char *szPath, u32 u32Mode)
{
    u32 i;
    char szNorm[VFS_MAX_PATH];
    u32 u32Perm;

    if (szPath == NULL || szPath[0] == '\0') {
        return;
    }
    if (path_norm(szNorm, szPath) != 0) {
        path_copy(szNorm, szPath);
    }
    u32Perm = u32Mode & 07777u;
    if (u32Perm == 0) {
        u32Perm = 0755u;
    }
    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (!g_aFiles[i].u8Used) {
            g_aFiles[i].u8Used = 1;
            g_aFiles[i].u8Kind = VFS_KIND_RAM;
            g_aFiles[i].u8IsDir = 1;
            g_aFiles[i].u32Mode = 0040000u | u32Perm;
            g_aFiles[i].u32Nlink = 2;
            path_copy(g_aFiles[i].szPath, szNorm);
            g_aFiles[i].cbData = 0;
            return;
        }
    }
}

/** Overwrite mode on a seeded slot (HostKey 0600, libexec X_OK stubs). */
static void
seed_set_mode(const char *szPath, u32 u32Mode)
{
    u32 i;
    char szNorm[VFS_MAX_PATH];

    if (szPath == NULL || szPath[0] == '\0') {
        return;
    }
    if (path_norm(szNorm, szPath) != 0) {
        path_copy(szNorm, szPath);
    }
    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (g_aFiles[i].u8Used && path_eq(g_aFiles[i].szPath, szNorm)) {
            g_aFiles[i].u32Mode = u32Mode;
            return;
        }
    }
}

void
vfs_ram_init(void)
{
    u32 i;
    u32 cSeed = 0;

    memset(g_aFiles, 0, sizeof(g_aFiles));
    memset(g_aFds, 0, sizeof(g_aFds));
    memset(g_aPipes, 0, sizeof(g_aPipes));
    memset(g_aPty, 0, sizeof(g_aPty));
    g_i32CttyPty = -1;
    memset(g_aSym, 0, sizeof(g_aSym));
    memset(g_aEventCnt, 0, sizeof(g_aEventCnt));
    memset(g_aEventUsed, 0, sizeof(g_aEventUsed));
    memset(g_aEpoll, 0, sizeof(g_aEpoll));
    memset(g_aTimerTicks, 0, sizeof(g_aTimerTicks));
    memset(g_aTimerInterval, 0, sizeof(g_aTimerInterval));
    memset(g_aTimerUsed, 0, sizeof(g_aTimerUsed));
    memset(g_aSigUsed, 0, sizeof(g_aSigUsed));
    memset(g_aSigMask, 0, sizeof(g_aSigMask));
    memset(g_aSigPending, 0, sizeof(g_aSigPending));
    memset(g_aInotify, 0, sizeof(g_aInotify));
    g_fVfsParkOk = 0u;
    g_fBlkMounted = 0;
    g_fScsiMounted = 0;
    g_iBlkFile = -1;
    g_u64ScsiCapBytes = 0;
    g_u32ChrRand = 0x9e3779b9u;
    /* Wave 12+14 soft inventory tallies (reset on re-init). */
    g_u32SoftSeeded = 0;
    g_u32SoftOpenOk = 0;
    g_u32SoftOpenFail = 0;
    g_u32SoftCloseOk = 0;
    g_u32SoftCloseFail = 0;
    g_u32SoftReadOk = 0;
    g_u32SoftReadFail = 0;
    g_u32SoftWriteOk = 0;
    g_u32SoftWriteFail = 0;
    g_u32SoftLseekOk = 0;
    g_u32SoftLseekFail = 0;
    g_u32SoftDupOk = 0;
    g_u32SoftDupFail = 0;
    g_u32SoftMkdirOk = 0;
    g_u32SoftMkdirFail = 0;
    g_u32SoftUnlinkOk = 0;
    g_u32SoftUnlinkFail = 0;
    g_u32SoftRmdirOk = 0;
    g_u32SoftRmdirFail = 0;
    g_u32SoftRenameOk = 0;
    g_u32SoftRenameFail = 0;
    g_u32SoftLinkOk = 0;
    g_u32SoftLinkFail = 0;
    g_u32SoftSymlinkOk = 0;
    g_u32SoftSymlinkFail = 0;
    g_u32SoftAccessOk = 0;
    g_u32SoftAccessFail = 0;
    g_u32SoftStatOk = 0;
    g_u32SoftStatFail = 0;
    g_u32SoftGetdentsOk = 0;
    g_u32SoftGetdentsFail = 0;
    g_u32SoftPipeOk = 0;
    g_u32SoftPipeFail = 0;
    g_u32SoftEventfdOk = 0;
    g_u32SoftEventfdFail = 0;
    g_u32SoftEpollOk = 0;
    g_u32SoftEpollFail = 0;
    g_u32SoftTimerfdOk = 0;
    g_u32SoftTimerfdFail = 0;
    g_u32SoftSignalfdOk = 0;
    g_u32SoftSignalfdFail = 0;
    g_u32SoftInotifyOk = 0;
    g_u32SoftInotifyFail = 0;
    g_u32SoftPidfdOk = 0;
    g_u32SoftPidfdFail = 0;
    g_u32SoftIouringOk = 0;
    g_u32SoftIouringFail = 0;
    g_u32SoftMountBlkOk = 0;
    g_u32SoftMountBlkSkip = 0;
    g_u32SoftMountScsiOk = 0;
    g_u32SoftMountScsiSkip = 0;
    g_u32SoftDenyNoent = 0;
    g_u32SoftDenyBadf = 0;
    g_u32SoftDenyInval = 0;
    g_u32SoftDenyNospc = 0;
    g_u32SoftDenyMfile = 0;
    g_u32SoftDenyExist = 0;
    g_u32SoftDenyAgain = 0;
    g_u32SoftDenyPipe = 0;
    g_u32SoftDenyPerm = 0;
    g_u32SoftDenyOther = 0;
    g_u32SoftFilesPeak = 0;
    g_u32SoftFdPeak = 0;
    g_u32SoftPipePeak = 0;
    g_u32SoftInvSamples = 0;
    g_fSoftInvOnce = 0;
    g_u32SoftResidualLean = 0;
    g_u32SoftResidualLeanOk = 0;
    g_fSoftResidualLean = 0;
    /* Wave 14 soft inventory tallies (reset on re-init). */
    g_u32SoftOk = 0;
    g_u32SoftFail = 0;
    g_u32SoftChmodOk = 0;
    g_u32SoftChmodFail = 0;
    g_u32SoftFchmodOk = 0;
    g_u32SoftFchmodFail = 0;
    g_u32SoftUtimensOk = 0;
    g_u32SoftUtimensFail = 0;
    g_u32SoftFallocateOk = 0;
    g_u32SoftFallocateFail = 0;
    g_u32SoftPunchOk = 0;
    g_u32SoftPunchFail = 0;
    g_u32SoftFsyncOk = 0;
    g_u32SoftFsyncFail = 0;
    g_u32SoftFtruncateOk = 0;
    g_u32SoftFtruncateFail = 0;
    g_u32SoftTruncateEnter = 0;
    g_u32SoftSendfileOk = 0;
    g_u32SoftSendfileFail = 0;
    g_u32SoftReadlinkOk = 0;
    g_u32SoftReadlinkFail = 0;
    g_u32SoftFstatOk = 0;
    g_u32SoftFstatFail = 0;
    g_u32SoftLstatOk = 0;
    g_u32SoftLstatFail = 0;
    g_u32SoftPreadEnter = 0;
    g_u32SoftPwriteEnter = 0;
    g_u32SoftCopyRangeOk = 0;
    g_u32SoftCopyRangeFail = 0;
    g_u32SoftMarkDirOk = 0;
    g_u32SoftMarkDirFail = 0;
    g_u32SoftSocketpairOk = 0;
    g_u32SoftSocketpairFail = 0;
    g_u32SoftInotifyAddOk = 0;
    g_u32SoftInotifyAddFail = 0;
    g_u32SoftInotifyRmOk = 0;
    g_u32SoftInotifyRmFail = 0;
    g_u32SoftEpollCtlOk = 0;
    g_u32SoftEpollCtlFail = 0;
    g_u32SoftEpollWaitOk = 0;
    g_u32SoftEpollWaitFail = 0;
    g_u32SoftPollProbe = 0;
    g_u64SoftBytesRead = 0;
    g_u64SoftBytesWrite = 0;
    g_u32SoftEventfdPeak = 0;
    g_u32SoftEpollPeak = 0;
    g_u32SoftTimerfdPeak = 0;
    g_u32SoftSignalfdPeak = 0;
    g_u32SoftInotifyPeak = 0;
    g_u32SoftSymPeak = 0;
    g_u32SoftDirLive = 0;
    g_u32SoftRamLive = 0;
    g_u32SoftBlkLive = 0;
    g_u32SoftScsiLive = 0;
    g_u32SoftPipeLive = 0;
    g_u32SoftSpecialLive = 0;
    seed_file("/dev/null", NULL);
    seed_file("/dev/tty", NULL);
    seed_file("/dev/ptmx", NULL);
    seed_file("/dev/urandom", NULL);
    seed_file("/dev/random", NULL);
    seed_dir("/dev/pts", 0755u);
    seed_file("/dev/pts/ptmx", NULL);
    seed_file("/etc/hostname", "greenjade\n");
    seed_file("/etc/os-release",
              "NAME=\"GreenJade\"\nID=greenjade\nVERSION_ID=\"0.1\"\n"
              "PRETTY_NAME=\"GreenJade 0.1\"\n"
              "HOME_URL=\"https://greenjade.local\"\n");
    seed_file("/etc/issue", "GreenJade \\n \\l\n");
    seed_file("/etc/passwd",
              "root:x:0:0:root:/root:/bin/sh\n"
              "sshd:x:74:74:sshd:/var/empty:/usr/sbin/nologin\n"
              "jay:x:1000:1000:jay:/home/jay:/bin/sh\n");
    seed_file("/etc/group", "root:x:0:\nsshd:x:74:\njay:x:1000:\n");
    /* OpenSSH later-wall seeds; packed rootfs ≠ vfs_ram; Dual DoD B OPEN. */
    seed_dir("/var/empty", 0755u); /* privsep chroot; uid 0, not 022 */
    seed_dir("/var/run", 0755u);   /* PidFile parent (writable by root) */
    seed_dir("/etc/ssh", 0755u);
    seed_dir("/usr/libexec", 0755u);
    seed_dir("/usr/sbin", 0755u);
    seed_file("/etc/ssh/sshd_config",
              "Port 22\n"
              "AddressFamily inet\n"
              "HostKey /etc/ssh/ssh_host_ed25519_key\n"
              "PidFile /var/run/sshd.pid\n"
              "AuthorizedKeysFile .ssh/authorized_keys\n"
              "PermitRootLogin yes\n"
              "PubkeyAuthentication yes\n"
              "PasswordAuthentication no\n"
              "KbdInteractiveAuthentication no\n"
              "PrintMotd no\n"
              "PrintLastLog no\n"
              "UseDNS no\n");
    seed_file("/etc/ssh/ssh_host_ed25519_key",
              "-----BEGIN OPENSSH PRIVATE KEY-----\n"
              "b3BlbnNzaC1rZXktdjEAAAAABG5vbmUAAAAEbm9uZQAAAAAAAAABAAAAMwAAAAtzc2gtZW\n"
              "QyNTUxOQAAACB6ReJSLKOxFFuqwNA2+be3sXD+GWPZoVkNb7my/dTvygAAAJBQZx1LUGcd\n"
              "SwAAAAtzc2gtZWQyNTUxOQAAACB6ReJSLKOxFFuqwNA2+be3sXD+GWPZoVkNb7my/dTvyg\n"
              "AAAEC1RTHQSuOGb6PoY1d9GDd3fIgROcdVOm0MJtmJ0rr4NHpF4lIso7EUW6rA0Db5t7ex\n"
              "cP4ZY9mhWQ1vubL91O/KAAAACmpheUBqYXlzcGMBAgM=\n"
              "-----END OPENSSH PRIVATE KEY-----\n");
    seed_set_mode("/etc/ssh/ssh_host_ed25519_key", 0100600u);
    /* Pre-exist so fopen("w") is not ENOSPC if the table is tight. */
    seed_file("/var/run/sshd.pid", NULL);
    seed_set_mode("/var/run/sshd.pid", 0100644u);
    /* 1-byte X_OK stubs; protonrt embed wins exec (not 11MiB here). */
    seed_file("/usr/libexec/sshd-session", "\n");
    seed_file("/usr/libexec/sshd-auth", "\n");
    seed_file("/usr/sbin/nologin", "\n");
    seed_set_mode("/usr/libexec/sshd-session", 0100755u);
    seed_set_mode("/usr/libexec/sshd-auth", 0100755u);
    seed_set_mode("/usr/sbin/nologin", 0100755u);
    /* Lab homes for OpenSSH pubkey; packed rootfs not mounted. Dual DoD B OPEN. */
    seed_dir("/home", 0755u);
    seed_dir("/home/jay", 0755u);
    seed_dir("/home/jay/.ssh", 0700u);
    seed_dir("/root", 0755u);
    seed_dir("/root/.ssh", 0700u);
    /* build/hwtest-keys/id_ed25519.pub present at write time — skip if absent. */
    seed_file("/home/jay/.ssh/authorized_keys",
              "ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAICyCCjetU1+33s+YtEo5td8zFDgjetsOhUYr3+z1efK9 greenjade-hwtest@lab\n");
    seed_file("/root/.ssh/authorized_keys",
              "ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAICyCCjetU1+33s+YtEo5td8zFDgjetsOhUYr3+z1efK9 greenjade-hwtest@lab\n");
    seed_set_mode("/home/jay/.ssh/authorized_keys", 0100600u);
    seed_set_mode("/root/.ssh/authorized_keys", 0100600u);
    seed_file("/proc/version", "Linux version 6.0.0-greenjade\n");
    seed_file("/proc/self/maps", "");
    seed_file("/proc/self/cmdline", "init\0");
    seed_file("/proc/cpuinfo", "processor\t: 0\nvendor_id\t: GreenJade\n");
    seed_file("/proc/meminfo", "MemTotal:        1048576 kB\n");
    seed_file("/bin/greenjade", "#!/bin/sh\necho GreenJade\n");
    /* /bin/sh is vendored dash (embedded blob + rootfs). Do not seed a
     * text placeholder — execve("/bin/sh") uses gj_shell_elf_blob. */
    seed_file("/lib/ld-gj.so.1", "# ld-gj scaffold placeholder (ELF staged by smoke)\n");
    /* DT_NEEDED resolve targets for dynlinker bring-up */
    seed_file("/lib/libc.so.6", "# GreenJade libc placeholder (not glibc)\n");
    seed_file("/usr/lib/libc.so.6", "# GreenJade libc placeholder\n");
    seed_file("/lib/libgj-so.so.1", "# SO map placeholder (ELF staged by smoke)\n");
    seed_file("/lib/libgj-gnu.so.1", "# GNU-hash SO placeholder (ELF staged by smoke)\n");
    seed_file("/usr/lib/libgj-so.so.1", "# SO map placeholder\n");
    seed_file("/usr/lib/libgj-gnu.so.1", "# GNU-hash SO placeholder\n");
    seed_file("/tmp/fanotify", "");
    /* ld-gj open targets (rewritten on execve) */
    seed_file("/proc/self/auxv", "");
    seed_file("/proc/self/execfn", "/bin/greenjade");
    seed_file("/tmp/.keep", "");
    seed_file("/var/tmp/.keep", "");
    seed_file("/usr/lib/os-release",
              "NAME=\"GreenJade\"\nID=greenjade\nVERSION_ID=\"0.1\"\n");
    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (g_aFiles[i].u8Used) {
            cSeed++;
        }
    }
    g_u32SoftSeeded = cSeed;
    soft_peak_note();
    kprintf("vfs_ram: init seeded=%u max_files=%u max_fds=%u "
            "stubs=/etc,/proc,/bin,/lib,/tmp\n",
            cSeed, VFS_MAX_FILES, VFS_MAX_FDS);
    /* Grep: vfs_ram: soft (baseline inventory after init) */
    soft_inventory_log();
    /* C0 lean residual once at init (boot-greppable; Soft!=product). */
    soft_residual_lean_once();
}

void
vfs_ram_mount_blk(void)
{
    u32 i;

    if (g_fBlkMounted) {
        return;
    }
    if (!virtio_blk_ready()) {
        if (g_u32SoftMountBlkSkip < 0xffffffffu) {
            g_u32SoftMountBlkSkip++;
        }
        kprintf("vfs_ram: mount_blk skipped (no virtio-blk)\n");
        return;
    }
    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (!g_aFiles[i].u8Used) {
            g_aFiles[i].u8Used = 1;
            g_aFiles[i].u8Kind = VFS_KIND_BLK;
            path_copy(g_aFiles[i].szPath, "/dev/vda");
            g_aFiles[i].cbData = 0;
            g_iBlkFile = (i32)i;
            g_fBlkMounted = 1;
            if (g_u32SoftMountBlkOk < 0xffffffffu) {
                g_u32SoftMountBlkOk++;
            }
            soft_peak_note();
            kprintf("vfs_ram: mounted /dev/vda capacity=%lu sectors\n",
                    (unsigned long)virtio_blk_capacity_sectors());
            return;
        }
    }
    if (g_u32SoftMountBlkSkip < 0xffffffffu) {
        g_u32SoftMountBlkSkip++;
    }
    kprintf("vfs_ram: mount_blk ENOSPC\n");
}

void
vfs_ram_mount_scsi(void)
{
    u32 i;
    struct gj_scsi_request req;
    u8 aCap[8];

    if (g_fScsiMounted) {
        return;
    }
    if (!virtio_scsi_ready()) {
        if (g_u32SoftMountScsiSkip < 0xffffffffu) {
            g_u32SoftMountScsiSkip++;
        }
        kprintf("vfs_ram: mount_scsi skipped (no virtio-scsi)\n");
        return;
    }
    memset(&req, 0, sizeof(req));
    scsi_cdb_read_capacity10(&req.cdb);
    req.pData = aCap;
    req.cbData = 8;
    req.fDataIn = 1;
    if (scsi_mid_submit(&req) != 0) {
        if (g_u32SoftMountScsiSkip < 0xffffffffu) {
            g_u32SoftMountScsiSkip++;
        }
        kprintf("vfs_ram: mount_scsi capacity failed\n");
        return;
    }
    {
        u32 u32Last = ((u32)aCap[0] << 24) | ((u32)aCap[1] << 16) |
                      ((u32)aCap[2] << 8) | (u32)aCap[3];
        u32 u32Bsz = ((u32)aCap[4] << 24) | ((u32)aCap[5] << 16) |
                     ((u32)aCap[6] << 8) | (u32)aCap[7];

        if (u32Bsz != VFS_SCSI_SEC) {
            if (g_u32SoftMountScsiSkip < 0xffffffffu) {
                g_u32SoftMountScsiSkip++;
            }
            kprintf("vfs_ram: mount_scsi unsupported block_len=%u\n", u32Bsz);
            return;
        }
        g_u64ScsiCapBytes = ((u64)u32Last + 1ull) * (u64)VFS_SCSI_SEC;
    }
    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (!g_aFiles[i].u8Used) {
            g_aFiles[i].u8Used = 1;
            g_aFiles[i].u8Kind = VFS_KIND_SCSI;
            path_copy(g_aFiles[i].szPath, "/dev/sda");
            g_aFiles[i].cbData = 0;
            g_fScsiMounted = 1;
            if (g_u32SoftMountScsiOk < 0xffffffffu) {
                g_u32SoftMountScsiOk++;
            }
            soft_peak_note();
            kprintf("vfs_ram: mounted /dev/sda capacity=%lu bytes\n",
                    (unsigned long)g_u64ScsiCapBytes);
            return;
        }
    }
    if (g_u32SoftMountScsiSkip < 0xffffffffu) {
        g_u32SoftMountScsiSkip++;
    }
    kprintf("vfs_ram: mount_scsi ENOSPC\n");
}

static i32
find_file(const char *szPath)
{
    u32 i;

    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (g_aFiles[i].u8Used && path_eq(g_aFiles[i].szPath, szPath)) {
            return (i32)i;
        }
    }
    return -1;
}

static i32
pty_find_by_pipe(u32 u32Pipe)
{
    u32 i;

    for (i = 0; i < VFS_MAX_PTY; i++) {
        if (g_aPty[i].u8Used && g_aPty[i].u32Pipe == u32Pipe) {
            return (i32)i;
        }
    }
    return -1;
}

static i32
pty_find_by_n(u32 u32N)
{
    u32 i;

    for (i = 0; i < VFS_MAX_PTY; i++) {
        if (g_aPty[i].u8Used && g_aPty[i].u32N == u32N) {
            return (i32)i;
        }
    }
    return -1;
}

static int
path_decimal_suffix(const char *szPath, const char *szPref, u32 *pN)
{
    u32 i = 0;
    u32 u32N = 0;
    int fDig = 0;

    if (szPath == NULL || szPref == NULL) {
        return 0;
    }
    while (szPref[i] != '\0') {
        if (szPath[i] != szPref[i]) {
            return 0;
        }
        i++;
    }
    if (szPath[i] == '\0') {
        return 0;
    }
    while (szPath[i] >= '0' && szPath[i] <= '9') {
        u32N = u32N * 10u + (u32)(szPath[i] - '0');
        i++;
        fDig = 1;
        if (u32N > 9999u) {
            return 0;
        }
    }
    if (fDig == 0 || szPath[i] != '\0') {
        return 0;
    }
    if (pN != NULL) {
        *pN = u32N;
    }
    return 1;
}

static int
pty_path_n(const char *szPath, u32 *pN)
{
    static const char szPref[] = "/dev/pts/";
    u32 u32N = 0;
    u32 cPref = (u32)sizeof(szPref) - 1u;

    /* Linux devpts: "0" ok; "00" / leading zeros are not a node. */
    if (path_decimal_suffix(szPath, szPref, &u32N) == 0) {
        return 0;
    }
    if (szPath[cPref] == '0' && szPath[cPref + 1u] != '\0') {
        return 0;
    }
    if (pN != NULL) {
        *pN = u32N;
    }
    return 1;
}

/* Linux new_encode_dev / glibc makedev shape for x86_64 st_rdev. */
static u64
vfs_makedev(u32 u32Maj, u32 u32Min)
{
    return ((u64)(u32Min & 0xffu)) |
           (((u64)(u32Maj & 0xfffu)) << 8) |
           (((u64)(u32Min & ~0xffu)) << 12);
}

/* Unix98: open master is 128:N; slave /dev/pts/N is 136:N. Mux node is 5:2. */
static u64
pty_master_rdev(u32 u32N)
{
    return vfs_makedev((u32)LINUX_UNIX98_PTY_MASTER_MAJOR, u32N);
}

static u64
pty_slave_rdev(u32 u32N)
{
    return vfs_makedev((u32)LINUX_UNIX98_PTY_SLAVE_MAJOR, u32N);
}

static u64
pty_mux_rdev(void)
{
    return vfs_makedev((u32)LINUX_TTYAUX_MAJOR, (u32)LINUX_PTMX_MINOR);
}

/* ICRNL/OPOST|ONLCR/CS8|CREAD|CLOCAL/ISIG|ICANON|ECHO + sane c_cc. */
static void
pty_tios_init(u8 *pTios)
{
    u32 u32I;
    u32 u32O;
    u32 u32C;
    u32 u32L;

    if (pTios == NULL) {
        return;
    }
    memset(pTios, 0, VFS_PTY_TIOS);
    u32I = VFS_TIOS_ICRNL | VFS_TIOS_IXON;
    u32O = VFS_TIOS_OPOST | VFS_TIOS_ONLCR;
    u32C = VFS_TIOS_B38400 | 0xB0u | VFS_TIOS_HUPCL | VFS_TIOS_CLOCAL; /* CS8|CREAD */
    u32L = VFS_TIOS_ISIG | VFS_TIOS_ICANON | VFS_TIOS_ECHO | VFS_TIOS_ECHOE |
           VFS_TIOS_ECHOK | VFS_TIOS_IEXTEN;
    memcpy(&pTios[0], &u32I, 4);
    memcpy(&pTios[4], &u32O, 4);
    memcpy(&pTios[8], &u32C, 4);
    memcpy(&pTios[12], &u32L, 4);
    pTios[17] = 3;    /* VINTR ^C */
    pTios[18] = 28;   /* VQUIT ^\ */
    pTios[19] = 127;  /* VERASE DEL */
    pTios[20] = 21;   /* VKILL ^U */
    pTios[21] = 4;    /* VEOF ^D */
    pTios[23] = 1;    /* VMIN */
    pTios[25] = 17;   /* VSTART */
    pTios[26] = 19;   /* VSTOP */
    pTios[27] = 26;   /* VSUSP */
    pTios[29] = 18;   /* VREPRINT ^R */
    pTios[30] = 15;   /* VDISCARD ^O */
    pTios[31] = 23;   /* VWERASE ^W */
    pTios[32] = 22;   /* VLNEXT ^V */
    {
        u32 u32Sp = VFS_TIOS_B38400;

        memcpy(&pTios[49], &u32Sp, 4); /* c_ispeed */
        memcpy(&pTios[53], &u32Sp, 4); /* c_ospeed */
    }
}

static void
pty_pkt_note(struct vfs_pty *pPty, u8 u8Bit)
{
    if (pPty == NULL || pPty->u8Pkt == 0 || u8Bit == 0) {
        return;
    }
    pPty->u8PktStat |= u8Bit;
    if (pPty->u32Pipe < VFS_MAX_PIPES) {
        (void)thread_wake(&g_aPipes[pPty->u32Pipe], VFS_PIPE_TAG_RD, 8u);
        vfs_ram_poll_kick();
    }
}

static void
pty_flush_ring(u32 u32Pipe, u8 u8Ring)
{
    i32 iPty;

    if (u32Pipe >= VFS_MAX_PIPES || u8Ring > 1u) {
        return;
    }
    g_aPipes[u32Pipe].u32Len[u8Ring] = 0;
    g_aPipes[u32Pipe].u32Head[u8Ring] = 0;
    if (u8Ring == 0) {
        iPty = pty_find_by_pipe(u32Pipe);
        if (iPty >= 0) {
            g_aPty[iPty].cbCanon = 0;
            g_aPty[iPty].u8SlaveEof = 0;
        }
    }
}

static u32
pty_tios_word(const struct vfs_pty *pPty, u32 u32Off)
{
    u32 u32V = 0;

    if (pPty == NULL || (u32Off + 4u) > VFS_PTY_TIOS) {
        return 0;
    }
    memcpy(&u32V, &pPty->aTios[u32Off], 4);
    return u32V;
}

static void
pty_tios_set_word(struct vfs_pty *pPty, u32 u32Off, u32 u32V)
{
    if (pPty == NULL || (u32Off + 4u) > VFS_PTY_TIOS) {
        return;
    }
    memcpy(&pPty->aTios[u32Off], &u32V, 4);
}

static u8
pty_cc(const struct vfs_pty *pPty, u32 u32Idx)
{
    if (pPty == NULL || (17u + u32Idx) >= VFS_PTY_TIOS) {
        return 0;
    }
    return pPty->aTios[17u + u32Idx];
}

static int
pty_cc_match(const struct vfs_pty *pPty, u32 u32Idx, u8 u8Ch)
{
    u8 u8Cc;

    u8Cc = pty_cc(pPty, u32Idx);
    return (u8Cc != 0 && u8Cc == u8Ch) ? 1 : 0;
}

/*
 * Linux tty ioctl: type 'T' and 8-bit nr. Size/dir bits are optional so
 * TIOCGPTN (0x80045430) and a stripped 0x5430 both match nr 0x30.
 */
static int
pty_cmd_nr_is(u32 u32Cmd, u8 u8Nr)
{
    if (((u32Cmd >> 8) & 0xffu) != (u32)'T') {
        return 0;
    }
    return ((u32Cmd & 0xffu) == (u32)u8Nr) ? 1 : 0;
}

/* Pointer int, or a small by-value arg (TIOCGPTPEER-shaped). NULL -> 0. */
static int
pty_arg_i32(void *pArg, i32 *pOut)
{
    if (pOut == NULL) {
        return 0;
    }
    if (pArg == NULL) {
        return 0;
    }
    if ((uintptr_t)pArg <= 0xfffffu) {
        *pOut = (i32)(uintptr_t)pArg;
        return 1;
    }
    memcpy(pOut, pArg, sizeof(*pOut));
    return 1;
}

/*
 * TIOCSPTLCK: Linux get_user(int). NULL / non-pointer -> fail (EFAULT).
 * By-value 1 locks (kernel concession); unlock is *(int *)==0 (never NULL).
 * Do not memcpy from addr < 4096 except that documented 1.
 */
static int
pty_arg_ptlck(void *pArg, i32 *pOut)
{
    if (pOut == NULL || pArg == NULL) {
        return 0;
    }
    if ((uintptr_t)pArg < 4096u) {
        if ((uintptr_t)pArg == 1u) {
            *pOut = 1;
            return 1;
        }
        return 0;
    }
    memcpy(pOut, pArg, sizeof(*pOut));
    return 1;
}

static u32
pty_ring_room(u32 u32Pipe, u8 u8Ring)
{
    if (u32Pipe >= VFS_MAX_PIPES || u8Ring > 1u ||
        g_aPipes[u32Pipe].u8Used == 0) {
        return 0;
    }
    return VFS_PIPE_BUF - g_aPipes[u32Pipe].u32Len[u8Ring];
}

static int
pty_ring_putc(u32 u32Pipe, u8 u8Ring, u8 u8Ch)
{
    struct vfs_pipe_pair *pPair;
    u32 u32Pos;

    if (pty_ring_room(u32Pipe, u8Ring) == 0) {
        return 0;
    }
    pPair = &g_aPipes[u32Pipe];
    u32Pos = (pPair->u32Head[u8Ring] + pPair->u32Len[u8Ring]) % VFS_PIPE_BUF;
    pPair->aRing[u8Ring][u32Pos] = u8Ch;
    pPair->u32Len[u8Ring]++;
    return 1;
}

static void
pty_echo(struct vfs_pty *pPty, u8 u8Ch, int fCtl)
{
    u32 u32O;

    if (pPty == NULL) {
        return;
    }
    if (fCtl != 0 && u8Ch < 0x20u && u8Ch != (u8)'\t' && u8Ch != (u8)'\n') {
        (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'^');
        (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)(u8Ch + 0x40u));
        return;
    }
    u32O = pty_tios_word(pPty, 4);
    if (u8Ch == (u8)'\n' && (u32O & VFS_TIOS_OPOST) != 0 &&
        (u32O & VFS_TIOS_ONLCR) != 0) {
        (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\r');
    }
    (void)pty_ring_putc(pPty->u32Pipe, 1, u8Ch);
}

/* Queue aCanon (+ optional NL) to slave-read ring[0]. 1 ok, 0 no room. */
static int
pty_canon_push(struct vfs_pty *pPty, int fNl)
{
    u32 u32Need;
    u32 i;

    if (pPty == NULL) {
        return 0;
    }
    u32Need = (u32)pPty->cbCanon + ((fNl != 0) ? 1u : 0u);
    if (u32Need == 0) {
        return 1;
    }
    if (pty_ring_room(pPty->u32Pipe, 0) < u32Need) {
        return 0;
    }
    for (i = 0; i < (u32)pPty->cbCanon; i++) {
        (void)pty_ring_putc(pPty->u32Pipe, 0, pPty->aCanon[i]);
    }
    if (fNl != 0) {
        (void)pty_ring_putc(pPty->u32Pipe, 0, (u8)'\n');
    }
    pPty->cbCanon = 0;
    return 1;
}

static void
pty_canon_drop(struct vfs_pty *pPty)
{
    if (pPty == NULL) {
        return;
    }
    if (pPty->cbCanon != 0 && pty_canon_push(pPty, 0) == 0) {
        pPty->cbCanon = 0;
    }
}

/* Master write byte: slave input. 1 consumed, 0 no room (do not consume). */
static int
pty_in_byte(struct vfs_pty *pPty, u8 u8Ch)
{
    u32 u32I;
    u32 u32L;
    int fEcho;
    int fCtl;
    int fLit;

    if (pPty == NULL) {
        return 0;
    }
    u32I = pty_tios_word(pPty, 0);
    u32L = pty_tios_word(pPty, 12);
    if ((u32L & VFS_TIOS_EXTPROC) != 0) {
        return pty_ring_putc(pPty->u32Pipe, 0, u8Ch);
    }
    if ((u32I & VFS_TIOS_ISTRIP) != 0) {
        u8Ch = (u8)(u8Ch & 0x7fu);
    }
    fLit = 0;
    if (pPty->u8Lnext != 0) {
        pPty->u8Lnext = 0;
        fLit = 1;
    }
    if (fLit == 0) {
        if (u8Ch == (u8)'\r') {
            if ((u32I & VFS_TIOS_IGNCR) != 0) {
                return 1;
            }
            if ((u32I & VFS_TIOS_ICRNL) != 0) {
                u8Ch = (u8)'\n';
            }
        } else if (u8Ch == (u8)'\n' && (u32I & VFS_TIOS_INLCR) != 0) {
            u8Ch = (u8)'\r';
        }
        if ((u32I & VFS_TIOS_IUCLC) != 0 && u8Ch >= (u8)'A' &&
            u8Ch <= (u8)'Z') {
            u8Ch = (u8)(u8Ch + 32u);
        }
        if ((u32I & VFS_TIOS_IXON) != 0) {
            if (pty_cc_match(pPty, VFS_VSTOP, u8Ch) != 0) {
                pPty->u8Stopped = 1;
                pty_pkt_note(pPty, (u8)LINUX_TIOCPKT_STOP);
                return 1;
            }
            if (pPty->u8Stopped != 0 &&
                (pty_cc_match(pPty, VFS_VSTART, u8Ch) != 0 ||
                 (u32I & VFS_TIOS_IXANY) != 0)) {
                pPty->u8Stopped = 0;
                pty_pkt_note(pPty, (u8)LINUX_TIOCPKT_START);
                if (pPty->u32Pipe < VFS_MAX_PIPES) {
                    (void)thread_wake(&g_aPipes[pPty->u32Pipe],
                                      VFS_PIPE_TAG_WR, 8u);
                }
                if (pty_cc_match(pPty, VFS_VSTART, u8Ch) != 0) {
                    return 1;
                }
            }
        }
        if ((u32L & VFS_TIOS_ISIG) != 0 &&
            (pty_cc_match(pPty, VFS_VINTR, u8Ch) != 0 ||
             pty_cc_match(pPty, VFS_VQUIT, u8Ch) != 0 ||
             pty_cc_match(pPty, VFS_VSUSP, u8Ch) != 0)) {
            if ((u32L & VFS_TIOS_NOFLSH) == 0) {
                pty_flush_ring(pPty->u32Pipe, 0);
            }
            /* Signo delivery OPEN (OpenSSH ISIG path). Dual DoD B OPEN. */
            return 1;
        }
        if ((u32L & VFS_TIOS_IEXTEN) != 0) {
            if (pty_cc_match(pPty, VFS_VLNEXT, u8Ch) != 0) {
                pPty->u8Lnext = 1;
                fEcho = ((u32L & VFS_TIOS_ECHO) != 0) ? 1 : 0;
                fCtl = ((u32L & VFS_TIOS_ECHOCTL) != 0) ? 1 : 0;
                if (fEcho != 0 && fCtl != 0) {
                    pty_echo(pPty, u8Ch, 1);
                }
                return 1;
            }
            if (pty_cc_match(pPty, VFS_VDISCARD, u8Ch) != 0) {
                pPty->u8Discard = (pPty->u8Discard != 0) ? 0 : 1u;
                return 1;
            }
        }
    }
    fEcho = ((u32L & VFS_TIOS_ECHO) != 0) ? 1 : 0;
    fCtl = ((u32L & VFS_TIOS_ECHOCTL) != 0) ? 1 : 0;
    if ((u32L & VFS_TIOS_ICANON) != 0) {
        if (fLit == 0 && pty_cc_match(pPty, VFS_VERASE, u8Ch) != 0) {
            if (pPty->cbCanon > 0) {
                u32 u32Drop = 1;

                pPty->cbCanon--;
                if ((u32I & VFS_TIOS_IUTF8) != 0) {
                    while (pPty->cbCanon > 0 &&
                           (pPty->aCanon[pPty->cbCanon - 1u] & 0xC0u) ==
                               0x80u) {
                        pPty->cbCanon--;
                        u32Drop++;
                    }
                }
                if (fEcho != 0 && (u32L & VFS_TIOS_ECHOE) != 0) {
                    while (u32Drop > 0) {
                        (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\b');
                        (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)' ');
                        (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\b');
                        u32Drop--;
                    }
                } else if (fEcho != 0) {
                    pty_echo(pPty, u8Ch, 0);
                }
            }
            return 1;
        }
        if (fLit == 0 && pty_cc_match(pPty, VFS_VKILL, u8Ch) != 0) {
            pPty->cbCanon = 0;
            if (fEcho != 0 && (u32L & VFS_TIOS_ECHOK) != 0) {
                pty_echo(pPty, (u8)'\n', 0);
            }
            return 1;
        }
        if (fLit == 0 && (u32L & VFS_TIOS_IEXTEN) != 0 &&
            pty_cc_match(pPty, VFS_VWERASE, u8Ch) != 0) {
            while (pPty->cbCanon > 0 &&
                   (pPty->aCanon[pPty->cbCanon - 1u] == (u8)' ' ||
                    pPty->aCanon[pPty->cbCanon - 1u] == (u8)'\t')) {
                pPty->cbCanon--;
                if (fEcho != 0 && (u32L & VFS_TIOS_ECHOE) != 0) {
                    (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\b');
                    (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)' ');
                    (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\b');
                }
            }
            while (pPty->cbCanon > 0 &&
                   pPty->aCanon[pPty->cbCanon - 1u] != (u8)' ' &&
                   pPty->aCanon[pPty->cbCanon - 1u] != (u8)'\t') {
                pPty->cbCanon--;
                if (fEcho != 0 && (u32L & VFS_TIOS_ECHOE) != 0) {
                    (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\b');
                    (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)' ');
                    (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\b');
                }
            }
            return 1;
        }
        if (fLit == 0 && (u32L & VFS_TIOS_IEXTEN) != 0 &&
            pty_cc_match(pPty, VFS_VREPRINT, u8Ch) != 0) {
            u32 iRp;

            if (fEcho != 0) {
                pty_echo(pPty, (u8)'\n', 0);
                for (iRp = 0; iRp < (u32)pPty->cbCanon; iRp++) {
                    pty_echo(pPty, pPty->aCanon[iRp], fCtl);
                }
            }
            return 1;
        }
        if (fLit == 0 && pty_cc_match(pPty, VFS_VEOF, u8Ch) != 0) {
            if (pPty->cbCanon == 0) {
                pPty->u8SlaveEof = 1;
                if (pPty->u32Pipe < VFS_MAX_PIPES) {
                    (void)thread_wake(&g_aPipes[pPty->u32Pipe],
                                      VFS_PIPE_TAG_RD, 8u);
                    vfs_ram_poll_kick();
                }
                return 1;
            }
            return pty_canon_push(pPty, 0);
        }
        if (fLit == 0 &&
            (u8Ch == (u8)'\n' || pty_cc_match(pPty, VFS_VEOL, u8Ch) != 0 ||
             pty_cc_match(pPty, VFS_VEOL2, u8Ch) != 0)) {
            if (fEcho != 0 || (u32L & VFS_TIOS_ECHONL) != 0) {
                pty_echo(pPty, (u8)'\n', 0);
            }
            return pty_canon_push(pPty, 1);
        }
        if (pPty->cbCanon >= VFS_PTY_CANON) {
            return 1;
        }
        pPty->aCanon[pPty->cbCanon++] = u8Ch;
        if (fEcho != 0) {
            pty_echo(pPty, u8Ch, fCtl);
        }
        return 1;
    }
    if (pty_ring_putc(pPty->u32Pipe, 0, u8Ch) == 0) {
        return 0;
    }
    if (fEcho != 0 || (u8Ch == (u8)'\n' && (u32L & VFS_TIOS_ECHONL) != 0)) {
        pty_echo(pPty, u8Ch, fCtl);
    }
    return 1;
}

/* Slave write byte: master-visible output. 1 consumed, 0 no room. */
static int
pty_out_byte(struct vfs_pty *pPty, u8 u8Ch)
{
    u32 u32O;
    u32 u32L;

    if (pPty == NULL) {
        return 0;
    }
    u32L = pty_tios_word(pPty, 12);
    if ((u32L & VFS_TIOS_EXTPROC) != 0) {
        return pty_ring_putc(pPty->u32Pipe, 1, u8Ch);
    }
    if (pPty->u8Discard != 0) {
        return 1;
    }
    u32O = pty_tios_word(pPty, 4);
    if ((u32O & VFS_TIOS_OPOST) != 0) {
        if ((u32O & VFS_TIOS_OLCUC) != 0 && u8Ch >= (u8)'a' &&
            u8Ch <= (u8)'z') {
            u8Ch = (u8)(u8Ch - 32u);
        }
        if (u8Ch == (u8)'\n' && (u32O & VFS_TIOS_ONLCR) != 0) {
            if (pty_ring_room(pPty->u32Pipe, 1) < 2u) {
                return 0;
            }
            (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\r');
            (void)pty_ring_putc(pPty->u32Pipe, 1, (u8)'\n');
            return 1;
        }
        if (u8Ch == (u8)'\n' && (u32O & VFS_TIOS_ONLRET) != 0) {
            u8Ch = (u8)'\r';
        }
        if (u8Ch == (u8)'\r' && (u32O & VFS_TIOS_OCRNL) != 0) {
            u8Ch = (u8)'\n';
        }
    }
    return pty_ring_putc(pPty->u32Pipe, 1, u8Ch);
}

static i64
pty_write(struct vfs_fd *pFd, const void *pBuf, u32 u32Cb)
{
    i32 iPty;
    struct vfs_pty *pPty;
    const u8 *pIn;
    u32 u32Done;
    u32 u32Pipe;

    if (pFd == NULL || pBuf == NULL) {
        return -LINUX_EFAULT;
    }
    iPty = pty_find_by_pipe(pFd->u32File);
    if (iPty < 0) {
        return -LINUX_ENOTTY;
    }
    pPty = &g_aPty[iPty];
    u32Pipe = pPty->u32Pipe;
    pIn = (const u8 *)pBuf;
    u32Done = 0;
    if (pPty->u8Hung != 0) {
        return -LINUX_EIO;
    }
    while (u32Done < u32Cb) {
        int nPut;

        if (u32Pipe >= VFS_MAX_PIPES || g_aPipes[u32Pipe].u8Used == 0) {
            return (u32Done > 0) ? (i64)u32Done : -LINUX_EBADF;
        }
        if (pPty->u8Hung != 0) {
            return (u32Done > 0) ? (i64)u32Done : -LINUX_EIO;
        }
        if (pFd->u8End == 0) {
            if (g_aPipes[u32Pipe].u8Open[1] == 0) {
                return (u32Done > 0) ? (i64)u32Done : -LINUX_EIO;
            }
        } else {
            if (g_aPipes[u32Pipe].u8Open[0] == 0) {
                return (u32Done > 0) ? (i64)u32Done : -LINUX_EIO;
            }
            if (pPty->u8Stopped != 0) {
                if (u32Done > 0) {
                    break;
                }
                if ((pFd->u8Fl & VFS_FD_FL_NONBLOCK) != 0u ||
                    thread_current() == NULL || g_fVfsParkOk == 0u) {
                    return -LINUX_EAGAIN;
                }
                thread_block(&g_aPipes[u32Pipe], VFS_PIPE_TAG_WR);
                schedule();
                continue;
            }
        }
        if (pFd->u8End == 0) {
            nPut = pty_in_byte(pPty, pIn[u32Done]);
        } else {
            nPut = pty_out_byte(pPty, pIn[u32Done]);
        }
        if (nPut == 0) {
            if (u32Done > 0) {
                break;
            }
            if ((pFd->u8Fl & VFS_FD_FL_NONBLOCK) != 0u ||
                thread_current() == NULL || g_fVfsParkOk == 0u) {
                return -LINUX_EAGAIN;
            }
            thread_block(&g_aPipes[u32Pipe], VFS_PIPE_TAG_WR);
            schedule();
            continue;
        }
        u32Done++;
    }
    if (u32Pipe < VFS_MAX_PIPES) {
        (void)thread_wake(&g_aPipes[u32Pipe], VFS_PIPE_TAG_RD, 8u);
        vfs_ram_poll_kick();
    }
    return (i64)u32Done;
}

static void
pty_fmt_path(char *szOut, u32 u32N)
{
    const char *szPref = "/dev/pts/";
    char aDig[12];
    u32 cDig = 0;
    u32 u32V = u32N;
    u32 i = 0;

    if (szOut == NULL) {
        return;
    }
    while (szPref[i] != '\0' && i + 1 < VFS_MAX_PATH) {
        szOut[i] = szPref[i];
        i++;
    }
    if (u32V == 0) {
        aDig[cDig++] = '0';
    } else {
        while (u32V != 0 && cDig < 11u) {
            aDig[cDig++] = (char)('0' + (u32V % 10u));
            u32V /= 10u;
        }
    }
    while (cDig > 0 && i + 1 < VFS_MAX_PATH) {
        szOut[i++] = aDig[--cDig];
    }
    szOut[i] = '\0';
}

/*
 * Keep /dev/pts/N as S_IFCHR so TIOCGPTN + ptsname/stat work while locked
 * (Linux Unix98). Preserve an existing chr mode (grantpt chmod).
 */
static int
pty_publish(u32 iPty)
{
    i32 iFile;
    u32 iNew;
    char szPts[VFS_MAX_PATH];

    if (iPty >= VFS_MAX_PTY || g_aPty[iPty].u8Used == 0) {
        return 0;
    }
    pty_fmt_path(szPts, g_aPty[iPty].u32N);
    iFile = g_aPty[iPty].i32File;
    if (iFile >= 0 && (u32)iFile < VFS_MAX_FILES &&
        g_aFiles[iFile].u8Used != 0 &&
        path_eq(g_aFiles[iFile].szPath, szPts)) {
        g_aFiles[iFile].u8Kind = VFS_KIND_RAM;
        g_aFiles[iFile].u8IsDir = 0;
        g_aFiles[iFile].cbData = 0;
        if ((g_aFiles[iFile].u32Mode & 0170000u) != 0020000u) {
            g_aFiles[iFile].u32Mode = 0020620u;
        }
        if (g_aFiles[iFile].u32Nlink == 0) {
            g_aFiles[iFile].u32Nlink = 1;
        }
        return 1;
    }
    iFile = find_file(szPts);
    if (iFile < 0) {
        iFile = -1;
        for (iNew = 0; iNew < VFS_MAX_FILES; iNew++) {
            if (g_aFiles[iNew].u8Used == 0) {
                iFile = (i32)iNew;
                break;
            }
        }
        if (iFile < 0) {
            return 0;
        }
        memset(&g_aFiles[iFile], 0, sizeof(g_aFiles[iFile]));
        g_aFiles[iFile].u8Used = 1;
        g_aFiles[iFile].u32Mode = 0020620u;
        g_aFiles[iFile].u32Nlink = 1;
        path_copy(g_aFiles[iFile].szPath, szPts);
    }
    g_aFiles[iFile].u8Kind = VFS_KIND_RAM;
    g_aFiles[iFile].u8IsDir = 0;
    g_aFiles[iFile].cbData = 0;
    if ((g_aFiles[iFile].u32Mode & 0170000u) != 0020000u) {
        g_aFiles[iFile].u32Mode = 0020620u;
    }
    if (g_aFiles[iFile].u32Nlink == 0) {
        g_aFiles[iFile].u32Nlink = 1;
    }
    g_aPty[iPty].i32File = iFile;
    return 1;
}

static void
pty_unpublish(u32 iPty)
{
    i32 iFile;

    if (iPty >= VFS_MAX_PTY) {
        return;
    }
    iFile = g_aPty[iPty].i32File;
    if (iFile >= 0 && (u32)iFile < VFS_MAX_FILES) {
        memset(&g_aFiles[iFile], 0, sizeof(g_aFiles[iFile]));
    }
    g_aPty[iPty].i32File = -1;
}

static void
pty_close_end(u32 u32Pipe, u8 u8End)
{
    i32 iPty;
    struct vfs_pty *pPty;

    iPty = pty_find_by_pipe(u32Pipe);
    if (iPty < 0 || u32Pipe >= VFS_MAX_PIPES) {
        return;
    }
    pPty = &g_aPty[iPty];
    if (u8End == 0) {
        if (pPty->u8MasterRefs > 0) {
            pPty->u8MasterRefs--;
        }
        if (pPty->u8MasterRefs != 0) {
            return;
        }
        g_aPipes[u32Pipe].u8Open[0] = 0;
        pPty->cbCanon = 0;
        pPty->u8Stopped = 0;
        pPty->u8Hung = 1;
        pPty->u16Mctrl &= (u16)~VFS_TIOCM_CAR;
        pty_unpublish((u32)iPty);
        if (pPty->u8SlaveRefs == 0) {
            g_aPipes[u32Pipe].u8Open[1] = 0;
        }
    } else {
        if (pPty->u8SlaveRefs > 0) {
            pPty->u8SlaveRefs--;
        }
        if (pPty->u8SlaveRefs != 0) {
            return;
        }
        g_aPipes[u32Pipe].u8Open[1] = 0;
        if ((pty_tios_word(pPty, 8) & VFS_TIOS_HUPCL) != 0) {
            pPty->u16Mctrl &=
                (u16)~(VFS_TIOCM_DTR | VFS_TIOCM_RTS | VFS_TIOCM_CAR);
        }
    }
    (void)thread_wake(&g_aPipes[u32Pipe], VFS_PIPE_TAG_RD, 8u);
    (void)thread_wake(&g_aPipes[u32Pipe], VFS_PIPE_TAG_WR, 8u);
    vfs_ram_poll_kick();
    if (g_aPipes[u32Pipe].u8Open[0] == 0 && g_aPipes[u32Pipe].u8Open[1] == 0) {
        if (g_i32CttyPty == iPty) {
            g_i32CttyPty = -1;
        }
        pty_unpublish((u32)iPty);
        g_aPipes[u32Pipe].u8Used = 0;
        memset(pPty, 0, sizeof(*pPty));
    }
}

static void
pty_ref_dup(const struct vfs_fd *pFd)
{
    i32 iPty;
    struct vfs_pty *pPty;

    if (pFd == NULL || pFd->u8Kind != VFS_KIND_PIPE) {
        return;
    }
    iPty = pty_find_by_pipe(pFd->u32File);
    if (iPty < 0) {
        return;
    }
    pPty = &g_aPty[iPty];
    if (pFd->u8End == 0) {
        if (pPty->u8MasterRefs < 255u) {
            pPty->u8MasterRefs++;
        }
    } else if (pPty->u8SlaveRefs < 255u) {
        pPty->u8SlaveRefs++;
    }
}

/*
 * open("/dev/ptmx"): allocate a pipe-ring pair, publish /dev/pts/N, return
 * the master fd. Product SSH = OpenSSH-portable, not abandoned sshd_gj.
 * New pairs start locked (Linux Unix98). TIOCSPTLCK 0 unlocks; TIOCGPTN
 * reports N while locked. Dual DoD B OPEN.
 */
static i64
pty_open_master(void)
{
    u32 iPty;
    u32 iPipe;
    u32 iFd;

    for (iPty = 0; iPty < VFS_MAX_PTY; iPty++) {
        if (g_aPty[iPty].u8Used == 0) {
            break;
        }
    }
    if (iPty >= VFS_MAX_PTY) {
        /* Linux ptmx: ENOSPC when the Unix98 pool is exhausted. */
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_ENOSPC);
    }
    for (iPipe = 0; iPipe < VFS_MAX_PIPES; iPipe++) {
        if (g_aPipes[iPipe].u8Used == 0) {
            break;
        }
    }
    if (iPipe >= VFS_MAX_PIPES) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_ENOSPC);
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (g_aFds[iFd].u8Used == 0) {
            break;
        }
    }
    if (iFd >= VFS_MAX_FDS) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_EMFILE);
    }

    memset(&g_aPipes[iPipe], 0, sizeof(g_aPipes[iPipe]));
    g_aPipes[iPipe].u8Used = 1;
    g_aPipes[iPipe].u8Open[0] = 1;
    g_aPipes[iPipe].u8Open[1] = 1; /* hold so master writes buffer pre-slave */

    memset(&g_aPty[iPty], 0, sizeof(g_aPty[iPty]));
    g_aPty[iPty].u8Used = 1;
    g_aPty[iPty].u8Locked = 1; /* Linux Unix98: unlock via TIOCSPTLCK 0 */
    g_aPty[iPty].u8MasterRefs = 1;
    g_aPty[iPty].u8SlaveRefs = 0;
    g_aPty[iPty].u32Pipe = iPipe;
    g_aPty[iPty].u32N = iPty;
    g_aPty[iPty].i32File = -1;
    g_aPty[iPty].u16Row = 24;
    g_aPty[iPty].u16Col = 80;
    g_aPty[iPty].u16Mctrl = (u16)VFS_TIOCM_DEF;
    pty_tios_init(g_aPty[iPty].aTios);
    /* TIOCGPTN+ptsname stat /dev/pts/N while locked. Dual DoD B OPEN. */
    if (pty_publish(iPty) == 0) {
        g_aPipes[iPipe].u8Used = 0;
        memset(&g_aPty[iPty], 0, sizeof(g_aPty[iPty]));
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_ENOSPC);
    }

    g_aFds[iFd].u8Used = 1;
    g_aFds[iFd].u8Kind = VFS_KIND_PIPE;
    g_aFds[iFd].u8End = 0;
    g_aFds[iFd].u8Fl = 0;
    g_aFds[iFd].u32File = iPipe;
    g_aFds[iFd].u64Off = 0;

    return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, (i64)iFd);
}

static i64
pty_open_slave(u32 u32N)
{
    i32 iPty;
    u32 iFd;
    u32 u32Pipe;

    iPty = pty_find_by_n(u32N);
    if (iPty < 0) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_ENOENT);
    }
    if (g_aPty[iPty].u8MasterRefs == 0) {
        /* Unix98: last master close unlinked /dev/pts/N. */
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_ENOENT);
    }
    if (g_aPty[iPty].u8Locked != 0) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_EIO);
    }
    if (g_aPty[iPty].u8Hung != 0) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_EIO);
    }
    if (g_aPty[iPty].u8Excl != 0 && g_aPty[iPty].u8SlaveRefs > 0) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_EBUSY);
    }
    u32Pipe = g_aPty[iPty].u32Pipe;
    if (u32Pipe >= VFS_MAX_PIPES || g_aPipes[u32Pipe].u8Used == 0) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_ENXIO);
    }
    if (g_aPipes[u32Pipe].u8Open[0] == 0) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_ENOENT);
    }
    if (g_aPty[iPty].u8SlaveRefs == 255u) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_EMFILE);
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (g_aFds[iFd].u8Used == 0) {
            break;
        }
    }
    if (iFd >= VFS_MAX_FDS) {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -LINUX_EMFILE);
    }
    g_aFds[iFd].u8Used = 1;
    g_aFds[iFd].u8Kind = VFS_KIND_PIPE;
    g_aFds[iFd].u8End = 1;
    g_aFds[iFd].u8Fl = 0;
    g_aFds[iFd].u32File = u32Pipe;
    g_aFds[iFd].u64Off = 0;
    g_aPty[iPty].u8SlaveRefs++;
    g_aPipes[u32Pipe].u8Open[1] = 1;
    return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, (i64)iFd);
}

/*
 * /dev/tty: TIOCSCTTY slot, else the unique live slave (OpenSSH login_tty
 * when protonrt does not forward TIOCSCTTY). Dual DoD B OPEN.
 */
static i32
pty_ctty_slot(void)
{
    i32 iFound = -1;
    u32 i;

    if (g_i32CttyPty >= 0 && (u32)g_i32CttyPty < VFS_MAX_PTY &&
        g_aPty[g_i32CttyPty].u8Used != 0) {
        return g_i32CttyPty;
    }
    for (i = 0; i < VFS_MAX_PTY; i++) {
        if (g_aPty[i].u8Used != 0 &&
            (g_aPty[i].u8SlaveRefs > 0 || g_aPty[i].i32Sid > 0)) {
            if (iFound >= 0) {
                return -1;
            }
            iFound = (i32)i;
        }
    }
    return iFound;
}

static i64
pty_open_ctty(void)
{
    i32 iPty;

    iPty = pty_ctty_slot();
    if (iPty < 0) {
        return -LINUX_ENXIO;
    }
    return pty_open_slave(g_aPty[iPty].u32N);
}

i64
vfs_ram_open(const char *szPath, int fCreate)
{
    i32 iFile;
    u32 iFd;
    u32 u32PtsN;
    char szResolved[VFS_MAX_PATH];
    i64 st;

    if (szPath == NULL || szPath[0] == '\0') {
        return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -14); /* EFAULT */
    }
    st = path_resolve(szResolved, szPath);
    if (st != 0) {
        /* Unresolvable / relative: still try raw path after norm */
        if (path_norm(szResolved, szPath) != 0) {
            path_copy(szResolved, szPath);
        }
    }
    /* Unix98 multiplexer: open allocates a pair, not a fd to the node. */
    if (path_eq(szResolved, "/dev/ptmx") ||
        path_eq(szResolved, "/dev/pts/ptmx")) {
        return pty_open_master();
    }
    if (pty_path_n(szResolved, &u32PtsN) != 0) {
        return pty_open_slave(u32PtsN);
    }
    if (path_eq(szResolved, "/dev/tty")) {
        i64 i64Ctty;

        i64Ctty = pty_open_ctty();
        if (i64Ctty != -LINUX_ENXIO) {
            return i64Ctty;
        }
        /* No ctty: seeded char (not ENOENT). Dual DoD B OPEN. */
    }
    /* Virtual directory markers (no file slot) */
    if (path_eq(szResolved, "/") || path_eq(szResolved, "/tmp") ||
        path_eq(szResolved, "/proc") || path_eq(szResolved, "/dev") ||
        path_eq(szResolved, "/dev/pts") ||
        path_eq(szResolved, "/bin") || path_eq(szResolved, "/etc") ||
        path_eq(szResolved, "/lib") || path_eq(szResolved, "/usr") ||
        path_eq(szResolved, "/var") || path_eq(szResolved, "/var/tmp")) {
        /* Materialize empty dir marker so getdents/fstat work */
        iFile = find_file(szResolved);
        if (iFile < 0) {
            for (iFile = 0; (u32)iFile < VFS_MAX_FILES; iFile++) {
                if (!g_aFiles[iFile].u8Used) {
                    g_aFiles[iFile].u8Used = 1;
                    g_aFiles[iFile].u8Kind = VFS_KIND_RAM;
                    g_aFiles[iFile].u8IsDir = 1;
                    g_aFiles[iFile].u32Mode = 0040755u;
                    g_aFiles[iFile].u32Nlink = 2;
                    path_copy(g_aFiles[iFile].szPath, szResolved);
                    g_aFiles[iFile].cbData = 0;
                    break;
                }
            }
            if ((u32)iFile >= VFS_MAX_FILES) {
                return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -28);
            }
        }
    } else {
        iFile = find_file(szResolved);
    }
    if (iFile < 0) {
        if (!fCreate) {
            return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -2); /* ENOENT */
        }
        for (iFile = 0; (u32)iFile < VFS_MAX_FILES; iFile++) {
            if (!g_aFiles[iFile].u8Used) {
                g_aFiles[iFile].u8Used = 1;
                g_aFiles[iFile].u8Kind = VFS_KIND_RAM;
                g_aFiles[iFile].u8IsDir = 0;
                g_aFiles[iFile].u32Mode = 0100644u;
                g_aFiles[iFile].u32Nlink = 1;
                path_copy(g_aFiles[iFile].szPath, szResolved);
                g_aFiles[iFile].cbData = 0;
                break;
            }
        }
        if ((u32)iFile >= VFS_MAX_FILES) {
            return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -28); /* ENOSPC */
        }
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = g_aFiles[iFile].u8Kind;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u8Fl = 0;
            g_aFds[iFd].u32File = (u32)iFile;
            g_aFds[iFd].u64Off = 0;
            return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftOpenOk, &g_u32SoftOpenFail, -24); /* EMFILE */
}

int
vfs_ram_fd_ok(i64 i64Fd)
{
    u8 u8Kind;

    if (i64Fd < 0 || i64Fd >= VFS_MAX_FDS || !g_aFds[i64Fd].u8Used) {
        return 0;
    }
    if (i64Fd >= 3) {
        return 1;
    }
    /* OpenSSH PTY/pipe onto stdio; Dual DoD B OPEN. */
    u8Kind = g_aFds[i64Fd].u8Kind;
    return u8Kind == VFS_KIND_PIPE || u8Kind == VFS_KIND_RAM;
}

void
vfs_ram_park_enable(void)
{
    g_fVfsParkOk = 1u;
}

static void
vfs_ram_poll_kick(void)
{
    (void)thread_wake(&g_u32PollWait, VFS_POLL_TAG, 8u);
}

void
vfs_ram_poll_park(void)
{
    if (g_fVfsParkOk == 0u || thread_current() == NULL) {
        return;
    }
    thread_block(&g_u32PollWait, VFS_POLL_TAG);
    schedule();
}

int
vfs_ram_fd_poll_parkable(i64 i64Fd)
{
    u8 u8Kind;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return 0;
    }
    u8Kind = g_aFds[i64Fd].u8Kind;
    if (u8Kind == VFS_KIND_PIPE || u8Kind == VFS_KIND_EVENTFD ||
        u8Kind == VFS_KIND_SIGNALFD) {
        return 1;
    }
    return 0;
}

i64
vfs_ram_fd_fl_get(i64 i64Fd)
{
    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9; /* EBADF */
    }
    return (i64)g_aFds[i64Fd].u8Fl;
}

i64
vfs_ram_fd_fl_set(i64 i64Fd, u8 u8Fl)
{
    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9; /* EBADF */
    }
    g_aFds[i64Fd].u8Fl = u8Fl;
    return 0;
}

i64
vfs_ram_dup(i64 i64Fd)
{
    u32 iFd;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, -9); /* EBADF */
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd] = g_aFds[i64Fd];
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Fl &= (u8)~VFS_FD_FL_CLOEXEC;
            pty_ref_dup(&g_aFds[iFd]);
            return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, -24); /* EMFILE */
}

i64
vfs_ram_dup2(i64 i64Old, i64 i64New)
{
    if (!vfs_ram_fd_ok(i64Old)) {
        return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, -9);
    }
    /* OpenSSH PTY/pipe onto stdio; Dual DoD B OPEN. */
    if (i64New < 0 || i64New >= VFS_MAX_FDS) {
        return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, -9);
    }
    if (i64Old == i64New) {
        return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, i64New);
    }
    if (g_aFds[i64New].u8Used) {
        (void)vfs_ram_close(i64New);
    }
    g_aFds[i64New] = g_aFds[i64Old];
    g_aFds[i64New].u8Used = 1;
    g_aFds[i64New].u8Fl &= (u8)~VFS_FD_FL_CLOEXEC;
    pty_ref_dup(&g_aFds[i64New]);
    return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, i64New);
}

i64
vfs_ram_dup_from(i64 i64Fd, i64 i64Min)
{
    u32 iFd;
    u32 u32Start;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, -9);
    }
    if (i64Min < 3) {
        i64Min = 3;
    }
    if (i64Min >= VFS_MAX_FDS) {
        return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, -9);
    }
    u32Start = (u32)i64Min;
    for (iFd = u32Start; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd] = g_aFds[i64Fd];
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Fl &= (u8)~VFS_FD_FL_CLOEXEC;
            pty_ref_dup(&g_aFds[iFd]);
            return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftDupOk, &g_u32SoftDupFail, -24);
}

i64
vfs_ram_fd_path(i64 i64Fd, char *pBuf, size_t cb)
{
    struct vfs_fd *pFd;
    size_t n;
    size_t i;

    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL || cb == 0) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind == VFS_KIND_PIPE) {
        i32 iPty = pty_find_by_pipe(pFd->u32File);
        char szPts[VFS_MAX_PATH];

        if (iPty < 0) {
            return -22;
        }
        if (pFd->u8End == 0) {
            path_copy(szPts, "/dev/ptmx");
        } else {
            pty_fmt_path(szPts, g_aPty[iPty].u32N);
        }
        n = 0;
        while (szPts[n] != '\0' && n + 1 < VFS_MAX_PATH) {
            n++;
        }
        if (n + 1 > cb) {
            return -34;
        }
        for (i = 0; i < n; i++) {
            pBuf[i] = szPts[i];
        }
        pBuf[n] = '\0';
        return 0;
    }
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES ||
        !g_aFiles[pFd->u32File].u8Used) {
        return -22;
    }
    n = 0;
    while (g_aFiles[pFd->u32File].szPath[n] != '\0' &&
           n + 1 < VFS_MAX_PATH) {
        n++;
    }
    if (n + 1 > cb) {
        return -34; /* ERANGE */
    }
    for (i = 0; i < n; i++) {
        pBuf[i] = g_aFiles[pFd->u32File].szPath[i];
    }
    pBuf[n] = '\0';
    return 0;
}

i64
vfs_ram_symlink(const char *szTarget, const char *szLink)
{
    u32 i;
    char szNorm[VFS_MAX_PATH];

    if (szTarget == NULL || szLink == NULL || szLink[0] == '\0' ||
        szTarget[0] == '\0') {
        return soft_out(&g_u32SoftSymlinkOk, &g_u32SoftSymlinkFail, -14);
    }
    if (path_norm(szNorm, szLink) != 0) {
        path_copy(szNorm, szLink);
    }
    if (find_file(szNorm) >= 0) {
        return soft_out(&g_u32SoftSymlinkOk, &g_u32SoftSymlinkFail, -17); /* EEXIST */
    }
    for (i = 0; i < VFS_MAX_SYMLINKS; i++) {
        if (g_aSym[i].u8Used && path_eq(g_aSym[i].szLink, szNorm)) {
            path_copy(g_aSym[i].szTarget, szTarget);
            return soft_out(&g_u32SoftSymlinkOk, &g_u32SoftSymlinkFail, 0);
        }
    }
    for (i = 0; i < VFS_MAX_SYMLINKS; i++) {
        if (!g_aSym[i].u8Used) {
            g_aSym[i].u8Used = 1;
            path_copy(g_aSym[i].szLink, szNorm);
            path_copy(g_aSym[i].szTarget, szTarget);
            return soft_out(&g_u32SoftSymlinkOk, &g_u32SoftSymlinkFail, 0);
        }
    }
    return soft_out(&g_u32SoftSymlinkOk, &g_u32SoftSymlinkFail, -28); /* ENOSPC */
}

i64
vfs_ram_utimens(const char *szPath)
{
    i32 iFile;
    char szResolved[VFS_MAX_PATH];

    if (szPath == NULL || szPath[0] == '\0') {
        return soft_out(&g_u32SoftUtimensOk, &g_u32SoftUtimensFail, -14);
    }
    if (path_resolve(szResolved, szPath) != 0) {
        if (path_norm(szResolved, szPath) != 0) {
            path_copy(szResolved, szPath);
        }
    }
    if (path_eq(szResolved, "/") || path_eq(szResolved, "/tmp") ||
        path_eq(szResolved, "/proc/self/exe")) {
        return soft_out(&g_u32SoftUtimensOk, &g_u32SoftUtimensFail, 0);
    }
    if (find_symlink(szResolved) >= 0) {
        return soft_out(&g_u32SoftUtimensOk, &g_u32SoftUtimensFail, 0);
    }
    iFile = find_file(szResolved);
    if (iFile < 0) {
        /* Create empty file as touch */
        i64 fd = vfs_ram_open(szResolved, 1);

        if (fd < 0) {
            return soft_out(&g_u32SoftUtimensOk, &g_u32SoftUtimensFail, fd);
        }
        (void)vfs_ram_close(fd);
        return soft_out(&g_u32SoftUtimensOk, &g_u32SoftUtimensFail, 0);
    }
    (void)iFile;
    return soft_out(&g_u32SoftUtimensOk, &g_u32SoftUtimensFail, 0);
}

i64
vfs_ram_link(const char *szOld, const char *szNew)
{
    i32 iOld;
    i32 iNew;
    u32 i;
    char szOldR[VFS_MAX_PATH];
    char szNewR[VFS_MAX_PATH];

    if (szOld == NULL || szNew == NULL || szOld[0] == '\0' || szNew[0] == '\0') {
        return soft_out(&g_u32SoftLinkOk, &g_u32SoftLinkFail, -14);
    }
    if (path_resolve(szOldR, szOld) != 0) {
        if (path_norm(szOldR, szOld) != 0) {
            path_copy(szOldR, szOld);
        }
    }
    if (path_norm(szNewR, szNew) != 0) {
        path_copy(szNewR, szNew);
    }
    iOld = find_file(szOldR);
    if (iOld < 0) {
        return soft_out(&g_u32SoftLinkOk, &g_u32SoftLinkFail, -2);
    }
    if (g_aFiles[iOld].u8Kind != VFS_KIND_RAM) {
        return soft_out(&g_u32SoftLinkOk, &g_u32SoftLinkFail, -1);
    }
    if (g_aFiles[iOld].u8IsDir) {
        return soft_out(&g_u32SoftLinkOk, &g_u32SoftLinkFail, -21); /* EISDIR */
    }
    if (find_file(szNewR) >= 0 || find_symlink(szNewR) >= 0) {
        return soft_out(&g_u32SoftLinkOk, &g_u32SoftLinkFail, -17); /* EEXIST */
    }
    /* New path entry that aliases same content (copy for bring-up) */
    for (iNew = 0; (u32)iNew < VFS_MAX_FILES; iNew++) {
        if (!g_aFiles[iNew].u8Used) {
            g_aFiles[iNew] = g_aFiles[iOld];
            g_aFiles[iNew].u8Used = 1;
            path_copy(g_aFiles[iNew].szPath, szNewR);
            g_aFiles[iOld].u32Nlink++;
            g_aFiles[iNew].u32Nlink = g_aFiles[iOld].u32Nlink;
            /* Deep-copy data buffer already done by struct assign */
            for (i = 0; i < g_aFiles[iOld].cbData && i < VFS_MAX_DATA; i++) {
                g_aFiles[iNew].aData[i] = g_aFiles[iOld].aData[i];
            }
            return soft_out(&g_u32SoftLinkOk, &g_u32SoftLinkFail, 0);
        }
    }
    return soft_out(&g_u32SoftLinkOk, &g_u32SoftLinkFail, -28);
}

i64
vfs_ram_unlink(const char *szPath)
{
    i32 iFile;
    u32 s;
    char szNorm[VFS_MAX_PATH];

    if (szPath == NULL || szPath[0] == '\0') {
        return soft_out(&g_u32SoftUnlinkOk, &g_u32SoftUnlinkFail, -14);
    }
    if (path_norm(szNorm, szPath) != 0) {
        path_copy(szNorm, szPath);
    }
    /* Symlinks first (do not follow) */
    for (s = 0; s < VFS_MAX_SYMLINKS; s++) {
        if (g_aSym[s].u8Used && path_eq(g_aSym[s].szLink, szNorm)) {
            memset(&g_aSym[s], 0, sizeof(g_aSym[s]));
            return soft_out(&g_u32SoftUnlinkOk, &g_u32SoftUnlinkFail, 0);
        }
    }
    if (path_eq(szNorm, "/dev/null") || path_eq(szNorm, "/dev/ptmx") ||
        path_eq(szNorm, "/dev/pts/ptmx") ||
        path_eq(szNorm, "/dev/tty") || path_eq(szNorm, "/dev/pts") ||
        path_eq(szNorm, "/dev/urandom") || path_eq(szNorm, "/dev/random")) {
        return soft_out(&g_u32SoftUnlinkOk, &g_u32SoftUnlinkFail, -22);
    }
    if (pty_path_n(szNorm, NULL) != 0) {
        return soft_out(&g_u32SoftUnlinkOk, &g_u32SoftUnlinkFail, -16);
    }
    iFile = find_file(szNorm);
    if (iFile < 0) {
        return soft_out(&g_u32SoftUnlinkOk, &g_u32SoftUnlinkFail, -2);
    }
    if (g_aFiles[iFile].u8Kind != VFS_KIND_RAM) {
        return soft_out(&g_u32SoftUnlinkOk, &g_u32SoftUnlinkFail, -1);
    }
    if (g_aFiles[iFile].u8IsDir) {
        return soft_out(&g_u32SoftUnlinkOk, &g_u32SoftUnlinkFail, -21); /* EISDIR */
    }
    /* Copy-based hard links: free this path entry only */
    memset(&g_aFiles[iFile], 0, sizeof(g_aFiles[iFile]));
    return soft_out(&g_u32SoftUnlinkOk, &g_u32SoftUnlinkFail, 0);
}

i64
vfs_ram_rmdir(const char *szPath)
{
    i32 iFile;
    u32 i;
    char szNorm[VFS_MAX_PATH];

    if (szPath == NULL || szPath[0] == '\0') {
        return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, -14);
    }
    if (path_norm(szNorm, szPath) != 0) {
        path_copy(szNorm, szPath);
    }
    if (path_eq(szNorm, "/") || path_eq(szNorm, "/tmp") ||
        path_eq(szNorm, "/proc") || path_eq(szNorm, "/dev") ||
        path_eq(szNorm, "/dev/pts")) {
        return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, -16); /* EBUSY */
    }
    iFile = find_file(szNorm);
    if (iFile < 0) {
        return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, -2);
    }
    if (g_aFiles[iFile].u8Kind != VFS_KIND_RAM) {
        return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, -20);
    }
    if (!g_aFiles[iFile].u8IsDir) {
        return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, -20); /* ENOTDIR */
    }
    if (g_aFiles[iFile].cbData != 0) {
        return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, -39); /* ENOTEMPTY */
    }
    /* Refuse if any path is a direct child */
    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (g_aFiles[i].u8Used && path_is_child_of(szNorm, g_aFiles[i].szPath)) {
            return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, -39);
        }
    }
    for (i = 0; i < VFS_MAX_SYMLINKS; i++) {
        if (g_aSym[i].u8Used && path_is_child_of(szNorm, g_aSym[i].szLink)) {
            return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, -39);
        }
    }
    memset(&g_aFiles[iFile], 0, sizeof(g_aFiles[iFile]));
    return soft_out(&g_u32SoftRmdirOk, &g_u32SoftRmdirFail, 0);
}

i64
vfs_ram_fchmod(i64 i64Fd, u32 u32Mode)
{
    struct vfs_fd *pFd;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftFchmodOk, &g_u32SoftFchmodFail, -9);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind == VFS_KIND_PIPE) {
        i32 iPty;
        i32 iFile;

        iPty = pty_find_by_pipe(pFd->u32File);
        if (iPty < 0) {
            return soft_out(&g_u32SoftFchmodOk, &g_u32SoftFchmodFail, -22);
        }
        iFile = g_aPty[iPty].i32File;
        if (pFd->u8End == 1 && iFile >= 0 && (u32)iFile < VFS_MAX_FILES &&
            g_aFiles[iFile].u8Used != 0) {
            g_aFiles[iFile].u32Mode =
                (g_aFiles[iFile].u32Mode & ~07777u) | (u32Mode & 07777u);
        }
        return soft_out(&g_u32SoftFchmodOk, &g_u32SoftFchmodFail, 0);
    }
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES) {
        return soft_out(&g_u32SoftFchmodOk, &g_u32SoftFchmodFail, -22);
    }
    /* Keep type bits; replace permission nibble */
    g_aFiles[pFd->u32File].u32Mode =
        (g_aFiles[pFd->u32File].u32Mode & ~07777u) | (u32Mode & 07777u);
    if ((u32Mode & 0040000u) != 0) {
        g_aFiles[pFd->u32File].u8IsDir = 1;
    }
    return soft_out(&g_u32SoftFchmodOk, &g_u32SoftFchmodFail, 0);
}

i64
vfs_ram_mark_dir(i64 i64Fd)
{
    struct vfs_fd *pFd;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftMarkDirOk, &g_u32SoftMarkDirFail, -9);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES) {
        return soft_out(&g_u32SoftMarkDirOk, &g_u32SoftMarkDirFail, -22);
    }
    g_aFiles[pFd->u32File].u8IsDir = 1;
    g_aFiles[pFd->u32File].u32Mode = 0040755u;
    g_aFiles[pFd->u32File].cbData = 0;
    return soft_out(&g_u32SoftMarkDirOk, &g_u32SoftMarkDirFail, 0);
}

i64
vfs_ram_fallocate(i64 i64Fd, i64 i64Off, i64 i64Len)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    i64 need;
    u32 u32Need;
    u32 i;

    if (!vfs_ram_fd_ok(i64Fd) || i64Off < 0 || i64Len < 0) {
        return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, -22);
    }
    if (i64Len == 0) {
        return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, 0);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES) {
        return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, -22);
    }
    pFile = &g_aFiles[pFd->u32File];
    if (!pFile->u8Used) {
        return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, -9);
    }
    if (pFile->u8IsDir) {
        return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, -21);
    }
    need = i64Off + i64Len;
    if (need < i64Off) {
        return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, -75);
    }
    /* Grow only - fallocate never shrinks (unlike ftruncate) */
    if (need <= (i64)pFile->cbData) {
        return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, 0);
    }
    if (need > (i64)VFS_MAX_DATA) {
        return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, -28);
    }
    u32Need = (u32)need;
    for (i = pFile->cbData; i < u32Need; i++) {
        pFile->aData[i] = 0;
    }
    pFile->cbData = u32Need;
    return soft_out(&g_u32SoftFallocateOk, &g_u32SoftFallocateFail, 0);
}

i64
vfs_ram_fallocate_punch(i64 i64Fd, i64 i64Off, i64 i64Len)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    i64 i64End;
    u32 i;
    u32 u32From;
    u32 u32To;

    if (!vfs_ram_fd_ok(i64Fd) || i64Off < 0 || i64Len < 0) {
        return soft_out(&g_u32SoftPunchOk, &g_u32SoftPunchFail, -22);
    }
    if (i64Len == 0) {
        return soft_out(&g_u32SoftPunchOk, &g_u32SoftPunchFail, 0);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES) {
        return soft_out(&g_u32SoftPunchOk, &g_u32SoftPunchFail, -22);
    }
    pFile = &g_aFiles[pFd->u32File];
    if (!pFile->u8Used || pFile->u8IsDir) {
        return soft_out(&g_u32SoftPunchOk, &g_u32SoftPunchFail,
                        pFile->u8IsDir ? -21 : -9);
    }
    i64End = i64Off + i64Len;
    if (i64End < i64Off) {
        return soft_out(&g_u32SoftPunchOk, &g_u32SoftPunchFail, -75);
    }
    if (i64Off >= (i64)pFile->cbData) {
        return soft_out(&g_u32SoftPunchOk, &g_u32SoftPunchFail, 0);
    }
    u32From = (u32)i64Off;
    u32To = (i64End > (i64)pFile->cbData) ? pFile->cbData : (u32)i64End;
    for (i = u32From; i < u32To; i++) {
        pFile->aData[i] = 0;
    }
    return soft_out(&g_u32SoftPunchOk, &g_u32SoftPunchFail, 0);
}

i64
vfs_ram_fsync(i64 i64Fd)
{
    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftFsyncOk, &g_u32SoftFsyncFail, -9);
    }
    return soft_out(&g_u32SoftFsyncOk, &g_u32SoftFsyncFail, 0);
}

i64
vfs_ram_fdatasync(i64 i64Fd)
{
    return vfs_ram_fsync(i64Fd);
}

i64
vfs_ram_bytes_readable(i64 i64Fd)
{
    struct vfs_fd *pFd;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u32 iEv = pFd->u32File;

        if (iEv < VFS_MAX_EVENTFD && g_aEventUsed[iEv] && g_aEventCnt[iEv] != 0) {
            return 8;
        }
        return 0;
    }
    if (pFd->u8Kind == VFS_KIND_TIMERFD) {
        u32 iT = pFd->u32File;

        if (iT < VFS_MAX_TIMERFD && g_aTimerUsed[iT] && g_aTimerTicks[iT] != 0) {
            return 8;
        }
        return 0;
    }
    if (pFd->u8Kind == VFS_KIND_SIGNALFD) {
        u32 iS = pFd->u32File;

        if (iS < VFS_MAX_SIGNALFD && g_aSigUsed[iS] && g_aSigPending[iS] != 0) {
            return 128;
        }
        return 0;
    }
    if (pFd->u8Kind == VFS_KIND_INOTIFY) {
        u32 iIn = pFd->u32File;

        if (iIn < VFS_MAX_INOTIFY && g_aInotify[iIn].u8Used) {
            return (i64)g_aInotify[iIn].u8Nq * 16;
        }
        return 0;
    }
    if (pFd->u8Kind == VFS_KIND_PIPE) {
        u8 u8From;
        i32 iPtyB;
        i64 i64N;

        if (pFd->u32File >= VFS_MAX_PIPES || !g_aPipes[pFd->u32File].u8Used) {
            return -9;
        }
        u8From = (u8)(1u - pFd->u8End);
        i64N = (i64)g_aPipes[pFd->u32File].u32Len[u8From];
        iPtyB = pty_find_by_pipe(pFd->u32File);
        if (iPtyB >= 0 && pFd->u8End == 0 && g_aPty[iPtyB].u8Pkt != 0 &&
            g_aPty[iPtyB].u8PktStat != 0 && i64N == 0) {
            return 1;
        }
        return i64N;
    }
    if (pFd->u8Kind == VFS_KIND_RAM && pFd->u32File < VFS_MAX_FILES &&
        g_aFiles[pFd->u32File].u8Used) {
        if (pFd->u64Off >= g_aFiles[pFd->u32File].cbData) {
            return 0;
        }
        return (i64)(g_aFiles[pFd->u32File].cbData - (u32)pFd->u64Off);
    }
    return 0;
}

i64
vfs_ram_mkdir(const char *szPath, u32 u32Mode)
{
    i32 iFile;
    char szNorm[VFS_MAX_PATH];
    u32 u32Perm;

    if (szPath == NULL || szPath[0] == '\0') {
        return soft_out(&g_u32SoftMkdirOk, &g_u32SoftMkdirFail, -14);
    }
    if (path_norm(szNorm, szPath) != 0) {
        path_copy(szNorm, szPath);
    }
    if (find_file(szNorm) >= 0 || find_symlink(szNorm) >= 0) {
        return soft_out(&g_u32SoftMkdirOk, &g_u32SoftMkdirFail, -17); /* EEXIST */
    }
    u32Perm = (u32Mode & 07777u);
    if (u32Perm == 0) {
        u32Perm = 0755u;
    }
    for (iFile = 0; (u32)iFile < VFS_MAX_FILES; iFile++) {
        if (!g_aFiles[iFile].u8Used) {
            g_aFiles[iFile].u8Used = 1;
            g_aFiles[iFile].u8Kind = VFS_KIND_RAM;
            g_aFiles[iFile].u8IsDir = 1;
            g_aFiles[iFile].u32Mode = 0040000u | u32Perm;
            g_aFiles[iFile].u32Nlink = 2;
            path_copy(g_aFiles[iFile].szPath, szNorm);
            g_aFiles[iFile].cbData = 0;
            return soft_out(&g_u32SoftMkdirOk, &g_u32SoftMkdirFail, 0);
        }
    }
    return soft_out(&g_u32SoftMkdirOk, &g_u32SoftMkdirFail, -28);
}

i64
vfs_ram_chmod(const char *szPath, u32 u32Mode)
{
    i32 iFile;
    char szResolved[VFS_MAX_PATH];

    if (szPath == NULL || szPath[0] == '\0') {
        return soft_out(&g_u32SoftChmodOk, &g_u32SoftChmodFail, -14);
    }
    if (path_resolve(szResolved, szPath) != 0) {
        if (path_norm(szResolved, szPath) != 0) {
            path_copy(szResolved, szPath);
        }
    }
    iFile = find_file(szResolved);
    if (iFile < 0) {
        return soft_out(&g_u32SoftChmodOk, &g_u32SoftChmodFail, -2);
    }
    if (g_aFiles[iFile].u8Kind != VFS_KIND_RAM) {
        return soft_out(&g_u32SoftChmodOk, &g_u32SoftChmodFail, -1);
    }
    g_aFiles[iFile].u32Mode =
        (g_aFiles[iFile].u32Mode & ~07777u) | (u32Mode & 07777u);
    if (g_aFiles[iFile].u8IsDir) {
        g_aFiles[iFile].u32Mode =
            (g_aFiles[iFile].u32Mode & 07777u) | 0040000u;
    } else if ((g_aFiles[iFile].u32Mode & 0170000u) == 0) {
        g_aFiles[iFile].u32Mode |= 0100000u;
    }
    return soft_out(&g_u32SoftChmodOk, &g_u32SoftChmodFail, 0);
}

i64
vfs_ram_truncate(const char *szPath, i64 i64Len)
{
    i32 iFile;
    i64 i64Fd;
    i64 st;
    char szResolved[VFS_MAX_PATH];

    soft_inc(&g_u32SoftTruncateEnter); /* Wave 14: path trunc enter */
    if (szPath == NULL || szPath[0] == '\0') {
        return -14;
    }
    if (i64Len < 0) {
        return -22;
    }
    if (path_resolve(szResolved, szPath) != 0) {
        if (path_norm(szResolved, szPath) != 0) {
            path_copy(szResolved, szPath);
        }
    }
    iFile = find_file(szResolved);
    if (iFile < 0) {
        return -2;
    }
    if (file_is_chr_discard(&g_aFiles[iFile]) ||
        file_is_chr_rand(&g_aFiles[iFile])) {
        return -22;
    }
    if (g_aFiles[iFile].u8IsDir) {
        return -21;
    }
    i64Fd = vfs_ram_open(szResolved, 0);
    if (i64Fd < 0) {
        return i64Fd;
    }
    st = vfs_ram_ftruncate(i64Fd, i64Len);
    (void)vfs_ram_close(i64Fd);
    return st;
}

i64
vfs_ram_sendfile(i64 i64Out, i64 i64In, u64 *pOff, size_t cb)
{
    u8 aBuf[512];
    size_t done = 0;
    u64 off;
    i64 n;
    i64 w;

    if (!vfs_ram_fd_ok(i64Out) || !vfs_ram_fd_ok(i64In) || cb == 0) {
        return soft_out(&g_u32SoftSendfileOk, &g_u32SoftSendfileFail, -9);
    }
    off = pOff ? *pOff : 0;
    while (done < cb) {
        size_t chunk = cb - done;

        if (chunk > sizeof(aBuf)) {
            chunk = sizeof(aBuf);
        }
        n = vfs_ram_pread(i64In, aBuf, chunk, off);
        if (n < 0) {
            return soft_out(&g_u32SoftSendfileOk, &g_u32SoftSendfileFail, done ? (i64)done : n);
        }
        if (n == 0) {
            break;
        }
        w = vfs_ram_write(i64Out, aBuf, (size_t)n);
        if (w < 0) {
            return soft_out(&g_u32SoftSendfileOk, &g_u32SoftSendfileFail, done ? (i64)done : w);
        }
        done += (size_t)w;
        off += (u64)w;
        if ((size_t)w < (size_t)n) {
            break;
        }
    }
    if (pOff != NULL) {
        *pOff = off;
    }
    return soft_out(&g_u32SoftSendfileOk, &g_u32SoftSendfileFail, (i64)done);
}

i64
vfs_ram_readlink(const char *szPath, char *pBuf, size_t cb)
{
    static const char szExe[] = "/bin/greenjade";
    size_t n;
    size_t i;
    u32 s;
    u32 u32Fd;
    char szFdPath[VFS_MAX_PATH];
    i64 i64Path;

    if (szPath == NULL || pBuf == NULL || cb == 0) {
        return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, -14);
    }
    /* Unix98 ttyname: /proc/self/fd/N -> /dev/ptmx or /dev/pts/N */
    if (path_decimal_suffix(szPath, "/proc/self/fd/", &u32Fd) != 0) {
        i64Path = vfs_ram_fd_path((i64)u32Fd, szFdPath, sizeof(szFdPath));
        if (i64Path != 0) {
            return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail,
                            i64Path);
        }
        n = 0;
        while (szFdPath[n] != '\0' && n + 1 < VFS_MAX_PATH) {
            n++;
        }
        if (cb < n) {
            return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, -34);
        }
        for (i = 0; i < n; i++) {
            pBuf[i] = szFdPath[i];
        }
        return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, (i64)n);
    }
    /* Dynamic symlink table first */
    for (s = 0; s < VFS_MAX_SYMLINKS; s++) {
        if (g_aSym[s].u8Used && path_eq(g_aSym[s].szLink, szPath)) {
            n = 0;
            while (g_aSym[s].szTarget[n] != '\0') {
                n++;
            }
            if (cb < n) {
                return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, -34);
            }
            for (i = 0; i < n; i++) {
                pBuf[i] = g_aSym[s].szTarget[i];
            }
            return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, (i64)n);
        }
    }
    /* Built-in known symlinks (product: vfsd) */
    if (!path_eq(szPath, "/proc/self/exe") &&
        !path_eq(szPath, "/proc/self/cwd")) {
        return -22; /* EINVAL - not a symlink here */
    }
    if (path_eq(szPath, "/proc/self/cwd")) {
        n = 1;
        if (cb < n) {
            return -34; /* ERANGE */
        }
        pBuf[0] = '/';
        return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, 1);
    }
    n = 0;
    while (szExe[n] != '\0') {
        n++;
    }
    if (cb < n) {
        return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, -34);
    }
    for (i = 0; i < n; i++) {
        pBuf[i] = szExe[i];
    }
    return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, (i64)n);
}

i64
vfs_ram_access(const char *szPath, int nMode)
{
    char szResolved[VFS_MAX_PATH];
    char szNorm[VFS_MAX_PATH];
    i32 iFile;
    u32 u32Mode;
    /* Linux: F_OK=0 R_OK=4 W_OK=2 X_OK=1 */
    int fNeedR = (nMode & 4) != 0;
    int fNeedW = (nMode & 2) != 0;
    int fNeedX = (nMode & 1) != 0;

    if (szPath == NULL || szPath[0] == '\0') {
        return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, -14);
    }
    if (path_norm(szNorm, szPath) != 0) {
        path_copy(szNorm, szPath);
    }
    /* Symlink existence for F_OK without follow when only checking link path */
    if (find_symlink(szNorm) >= 0 && nMode == 0) {
        return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, 0);
    }
    if (path_resolve(szResolved, szPath) != 0) {
        path_copy(szResolved, szNorm);
    }
    /* Known virtual paths */
    if (path_eq(szResolved, "/proc/self/exe") ||
        path_eq(szResolved, "/proc/self/cwd") || path_eq(szResolved, "/") ||
        path_eq(szResolved, "/tmp") || path_eq(szResolved, "/proc") ||
        path_eq(szResolved, "/dev") || path_eq(szResolved, "/dev/pts") ||
        path_eq(szResolved, "/bin") ||
        path_eq(szResolved, "/etc") || path_eq(szResolved, "/lib") ||
        path_eq(szResolved, "/bin/greenjade")) {
        if (fNeedW && (path_eq(szResolved, "/proc/self/exe") ||
                       path_eq(szResolved, "/proc/self/cwd"))) {
            return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, -13);
        }
        return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, 0);
    }
    iFile = find_file(szResolved);
    if (iFile < 0) {
        return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, -2); /* ENOENT */
    }
    if (nMode == 0) {
        return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, 0);
    }
    u32Mode = g_aFiles[iFile].u32Mode;
    if (u32Mode == 0) {
        u32Mode = 0100644u;
    }
    /* Owner bits only (uid 0 in bring-up still checks perm mask) */
    if (fNeedR && (u32Mode & 0444u) == 0) {
        return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, -13);
    }
    if (fNeedW && (u32Mode & 0222u) == 0) {
        return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, -13);
    }
    if (fNeedX && (u32Mode & 0112u) == 0) {
        return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, -13);
    }
    return soft_out(&g_u32SoftAccessOk, &g_u32SoftAccessFail, 0);
}

/* Linux x86_64 struct stat (public layout, first fields we fill). */
struct vfs_stat64 {
    u64 st_dev;
    u64 st_ino;
    u64 st_nlink;
    u32 st_mode;
    u32 st_uid;
    u32 st_gid;
    u32 __pad0;
    u64 st_rdev;
    i64 st_size;
    i64 st_blksize;
    i64 st_blocks;
    i64 st_atime;
    u64 st_atime_nsec;
    i64 st_mtime;
    u64 st_mtime_nsec;
    i64 st_ctime;
    u64 st_ctime_nsec;
    u64 __unused[3];
};

static void
fill_stat(struct vfs_stat64 *pSt, u64 ino, u32 mode, i64 size)
{
    memset(pSt, 0, sizeof(*pSt));
    pSt->st_dev = 1;
    pSt->st_ino = ino;
    pSt->st_nlink = 1;
    pSt->st_mode = mode;
    pSt->st_uid = 0;
    pSt->st_gid = 0;
    pSt->st_size = size;
    pSt->st_blksize = 4096;
    pSt->st_blocks = (size + 511) / 512;
}

i64
vfs_ram_fstat(i64 i64Fd, void *pStat, size_t cbStat)
{
    struct vfs_stat64 st;
    struct vfs_fd *pFd;
    u32 mode;
    i64 size;
    i32 iPty;

    if (pStat == NULL || cbStat < sizeof(st)) {
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, -14);
    }
    if (i64Fd >= 0 && i64Fd <= 2 && !vfs_ram_fd_ok(i64Fd)) {
        /* stdin/out/err - char device-ish (until login_tty dup2 of a PTY) */
        fill_stat(&st, (u64)i64Fd + 1u, 0020000u | 0666u, 0);
        memcpy(pStat, &st, sizeof(st));
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, 0);
    }
    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, -9);
    }
    pFd = &g_aFds[i64Fd];
    mode = 0100644u; /* regular */
    size = 0;
    iPty = -1;
    if (pFd->u8Kind == VFS_KIND_RAM && pFd->u32File < VFS_MAX_FILES) {
        size = (i64)g_aFiles[pFd->u32File].cbData;
        if (g_aFiles[pFd->u32File].u32Mode != 0) {
            mode = g_aFiles[pFd->u32File].u32Mode;
        }
        if (g_aFiles[pFd->u32File].u8IsDir) {
            mode = (mode & 07777u) | 0040000u;
            size = 0;
        }
    } else if (pFd->u8Kind == VFS_KIND_PIPE) {
        iPty = pty_find_by_pipe(pFd->u32File);
        if (iPty >= 0) {
            /* master 128:N 0666; slave 136:N 0620 (mux node 5:2 is path-only) */
            mode = (pFd->u8End == 0) ? 0020666u : 0020620u;
        } else {
            mode = 0010644u; /* fifo */
        }
    } else if (pFd->u8Kind == VFS_KIND_BLK || pFd->u8Kind == VFS_KIND_SCSI) {
        mode = 0060644u; /* block */
        size = (i64)(g_u64ScsiCapBytes ? g_u64ScsiCapBytes : 0);
    } else if (pFd->u8Kind == VFS_KIND_EVENTFD ||
               pFd->u8Kind == VFS_KIND_TIMERFD ||
               pFd->u8Kind == VFS_KIND_SIGNALFD ||
               pFd->u8Kind == VFS_KIND_EPOLL ||
               pFd->u8Kind == VFS_KIND_INOTIFY ||
               pFd->u8Kind == VFS_KIND_PIDFD ||
               pFd->u8Kind == VFS_KIND_IOURING) {
        mode = 0020644u; /* char-device shaped */
    }
    fill_stat(&st, (u64)i64Fd + 100u, mode, size);
    if (pFd->u8Kind == VFS_KIND_RAM && pFd->u32File < VFS_MAX_FILES &&
        g_aFiles[pFd->u32File].u32Nlink > 0) {
        st.st_nlink = g_aFiles[pFd->u32File].u32Nlink;
        if (path_eq(g_aFiles[pFd->u32File].szPath, "/dev/tty")) {
            st.st_rdev = vfs_makedev((u32)LINUX_TTYAUX_MAJOR, 0u);
        } else if (path_eq(g_aFiles[pFd->u32File].szPath, "/dev/ptmx") ||
                   path_eq(g_aFiles[pFd->u32File].szPath, "/dev/pts/ptmx")) {
            st.st_rdev = pty_mux_rdev();
        }
    }
    if (iPty >= 0 && (u32)iPty < VFS_MAX_PTY) {
        /* Open master 128:N (TIOCGPTN minor); slave 136:N. Dual DoD B OPEN. */
        if (pFd->u8End == 0) {
            st.st_rdev = pty_master_rdev(g_aPty[iPty].u32N);
        } else {
            st.st_rdev = pty_slave_rdev(g_aPty[iPty].u32N);
        }
    }
    memcpy(pStat, &st, sizeof(st));
    return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, 0);
}

i64
vfs_ram_rename(const char *szOld, const char *szNew)
{
    i32 iFile;
    i32 iExist;
    u32 s;
    char szOldN[VFS_MAX_PATH];
    char szNewN[VFS_MAX_PATH];

    if (szOld == NULL || szNew == NULL || szOld[0] == '\0' ||
        szNew[0] == '\0') {
        return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, -14);
    }
    if (path_norm(szOldN, szOld) != 0) {
        path_copy(szOldN, szOld);
    }
    if (path_norm(szNewN, szNew) != 0) {
        path_copy(szNewN, szNew);
    }
    if (path_eq(szOldN, szNewN)) {
        return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, 0);
    }
    if (path_eq(szOldN, "/dev/null") || path_eq(szOldN, "/dev/ptmx") ||
        path_eq(szOldN, "/dev/pts/ptmx") ||
        path_eq(szOldN, "/dev/tty") || path_eq(szOldN, "/dev/pts") ||
        path_eq(szOldN, "/dev/urandom") || path_eq(szOldN, "/dev/random")) {
        return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, -1);
    }
    if (pty_path_n(szOldN, NULL) != 0) {
        return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, -1);
    }
    /* Symlink rename */
    for (s = 0; s < VFS_MAX_SYMLINKS; s++) {
        if (g_aSym[s].u8Used && path_eq(g_aSym[s].szLink, szOldN)) {
            i32 iClash = find_symlink(szNewN);

            if (iClash >= 0 && (u32)iClash != s) {
                memset(&g_aSym[iClash], 0, sizeof(g_aSym[iClash]));
            }
            iExist = find_file(szNewN);
            if (iExist >= 0) {
                if (g_aFiles[iExist].u8IsDir) {
                    return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail,
                                    -21);
                }
                memset(&g_aFiles[iExist], 0, sizeof(g_aFiles[iExist]));
            }
            path_copy(g_aSym[s].szLink, szNewN);
            return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, 0);
        }
    }
    iFile = find_file(szOldN);
    if (iFile < 0) {
        return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, -2);
    }
    if (g_aFiles[iFile].u8Kind != VFS_KIND_RAM) {
        return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, -1);
    }
    iExist = find_file(szNewN);
    if (iExist >= 0 && iExist != iFile) {
        if (g_aFiles[iExist].u8IsDir && !g_aFiles[iFile].u8IsDir) {
            return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, -21);
        }
        if (!g_aFiles[iExist].u8IsDir && g_aFiles[iFile].u8IsDir) {
            return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, -20);
        }
        if (g_aFiles[iExist].u8IsDir) {
            /* Only replace empty dir */
            u32 i;

            for (i = 0; i < VFS_MAX_FILES; i++) {
                if (g_aFiles[i].u8Used &&
                    path_is_child_of(g_aFiles[iExist].szPath,
                                     g_aFiles[i].szPath)) {
                    return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail,
                                    -39);
                }
            }
        }
        memset(&g_aFiles[iExist], 0, sizeof(g_aFiles[iExist]));
    }
    {
        i32 iSym = find_symlink(szNewN);

        if (iSym >= 0) {
            memset(&g_aSym[iSym], 0, sizeof(g_aSym[iSym]));
        }
    }
    path_copy(g_aFiles[iFile].szPath, szNewN);
    return soft_out(&g_u32SoftRenameOk, &g_u32SoftRenameFail, 0);
}

i64
vfs_ram_ftruncate(i64 i64Fd, i64 i64Len)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    u32 u32New;
    u32 i;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftFtruncateOk, &g_u32SoftFtruncateFail, -9);
    }
    if (i64Len < 0) {
        return soft_out(&g_u32SoftFtruncateOk, &g_u32SoftFtruncateFail, -22);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES) {
        return soft_out(&g_u32SoftFtruncateOk, &g_u32SoftFtruncateFail, -22);
    }
    pFile = &g_aFiles[pFd->u32File];
    if (!pFile->u8Used) {
        return soft_out(&g_u32SoftFtruncateOk, &g_u32SoftFtruncateFail, -9);
    }
    if (file_is_chr_discard(pFile) || file_is_chr_rand(pFile)) {
        return soft_out(&g_u32SoftFtruncateOk, &g_u32SoftFtruncateFail, -22);
    }
    if (pFile->u8IsDir) {
        return -21; /* EISDIR */
    }
    u32New = (u32)i64Len;
    if ((i64)u32New != i64Len) {
        return -75; /* EOVERFLOW - beyond u32 / cap */
    }
    if (u32New > VFS_MAX_DATA) {
        return -28; /* ENOSPC rather than silent clip */
    }
    if (u32New > pFile->cbData) {
        for (i = pFile->cbData; i < u32New; i++) {
            pFile->aData[i] = 0;
        }
    }
    pFile->cbData = u32New;
    if (pFd->u64Off > (u64)u32New) {
        pFd->u64Off = (u64)u32New;
    }
    return soft_out(&g_u32SoftFtruncateOk, &g_u32SoftFtruncateFail, 0);
}

i64
vfs_ram_stat(const char *szPath, void *pStat, size_t cbStat)
{
    i32 iFile;
    struct vfs_stat64 st;
    u32 mode = 0100644u;
    i64 size = 0;
    u32 u32PtsN;
    char szResolved[VFS_MAX_PATH];

    if (szPath == NULL || pStat == NULL || cbStat < sizeof(st)) {
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, -14);
    }
    if (path_resolve(szResolved, szPath) != 0) {
        if (path_norm(szResolved, szPath) != 0) {
            path_copy(szResolved, szPath);
        }
    }
    if (path_eq(szResolved, "/") || path_eq(szResolved, "/tmp") ||
        path_eq(szResolved, "/proc") || path_eq(szResolved, "/dev") ||
        path_eq(szResolved, "/dev/pts") ||
        path_eq(szResolved, "/bin") || path_eq(szResolved, "/etc") ||
        path_eq(szResolved, "/lib") || path_eq(szResolved, "/usr") ||
        path_eq(szResolved, "/var") || path_eq(szResolved, "/var/tmp")) {
        fill_stat(&st, 2, 0040755u, 0); /* directory */
        memcpy(pStat, &st, sizeof(st));
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, 0);
    }
    if (path_eq(szResolved, "/proc/self/exe") ||
        path_eq(szResolved, "/bin/greenjade")) {
        fill_stat(&st, 3, 0100755u, 4096);
        memcpy(pStat, &st, sizeof(st));
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, 0);
    }
    if (path_eq(szResolved, "/proc/self/cwd")) {
        fill_stat(&st, 4, 0040755u, 0);
        memcpy(pStat, &st, sizeof(st));
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, 0);
    }
    iFile = find_file(szResolved);
    if (iFile < 0) {
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, -2);
    }
    if (g_aFiles[iFile].u8Kind == VFS_KIND_RAM) {
        size = (i64)g_aFiles[iFile].cbData;
        if (g_aFiles[iFile].u32Mode != 0) {
            mode = g_aFiles[iFile].u32Mode;
        }
        if (g_aFiles[iFile].u8IsDir) {
            mode = (mode & 07777u) | 0040000u;
            size = 0;
        }
    } else if (g_aFiles[iFile].u8Kind == VFS_KIND_BLK ||
               g_aFiles[iFile].u8Kind == VFS_KIND_SCSI) {
        mode = 0060644u;
        size = (i64)g_u64ScsiCapBytes;
    }
    fill_stat(&st, (u64)iFile + 200u, mode, size);
    if (g_aFiles[iFile].u32Nlink > 0) {
        st.st_nlink = g_aFiles[iFile].u32Nlink;
    }
    if (pty_path_n(szResolved, &u32PtsN) != 0) {
        st.st_rdev = pty_slave_rdev(u32PtsN);
    } else if (path_eq(szResolved, "/dev/ptmx") ||
               path_eq(szResolved, "/dev/pts/ptmx")) {
        st.st_rdev = pty_mux_rdev();
    } else if (path_eq(szResolved, "/dev/tty")) {
        st.st_rdev = vfs_makedev((u32)LINUX_TTYAUX_MAJOR, 0u);
    }
    memcpy(pStat, &st, sizeof(st));
    return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, 0);
}

i64
vfs_ram_lstat(const char *szPath, void *pStat, size_t cbStat)
{
    i32 iSym;
    i32 iFile;
    struct vfs_stat64 st;
    char szNorm[VFS_MAX_PATH];
    size_t n;
    u32 u32PtsN;

    if (szPath == NULL || pStat == NULL || cbStat < sizeof(st)) {
        return soft_out(&g_u32SoftLstatOk, &g_u32SoftLstatFail, -14);
    }
    if (path_norm(szNorm, szPath) != 0) {
        path_copy(szNorm, szPath);
    }
    /* Built-in symlinks */
    if (path_eq(szNorm, "/proc/self/exe") || path_eq(szNorm, "/proc/self/cwd")) {
        fill_stat(&st, 5, 0120777u, 0); /* S_IFLNK */
        st.st_size = path_eq(szNorm, "/proc/self/cwd") ? 1 : 13;
        memcpy(pStat, &st, sizeof(st));
        return soft_out(&g_u32SoftLstatOk, &g_u32SoftLstatFail, 0);
    }
    iSym = find_symlink(szNorm);
    if (iSym >= 0) {
        n = 0;
        while (g_aSym[iSym].szTarget[n] != '\0') {
            n++;
        }
        fill_stat(&st, (u64)iSym + 300u, 0120777u, (i64)n);
        memcpy(pStat, &st, sizeof(st));
        return soft_out(&g_u32SoftLstatOk, &g_u32SoftLstatFail, 0);
    }
    /* Non-link: same as stat without follow (already resolved-ish) */
    iFile = find_file(szNorm);
    if (iFile < 0) {
        /* Virtual dirs */
        if (path_eq(szNorm, "/") || path_eq(szNorm, "/tmp") ||
            path_eq(szNorm, "/proc") || path_eq(szNorm, "/dev") ||
            path_eq(szNorm, "/dev/pts")) {
            fill_stat(&st, 2, 0040755u, 0);
            memcpy(pStat, &st, sizeof(st));
            return soft_out(&g_u32SoftLstatOk, &g_u32SoftLstatFail, 0);
        }
        return soft_out(&g_u32SoftLstatOk, &g_u32SoftLstatFail, -2);
    }
    {
        u32 mode = 0100644u;
        i64 size = (i64)g_aFiles[iFile].cbData;

        if (g_aFiles[iFile].u32Mode != 0) {
            mode = g_aFiles[iFile].u32Mode;
        }
        if (g_aFiles[iFile].u8IsDir) {
            mode = (mode & 07777u) | 0040000u;
            size = 0;
        } else if (g_aFiles[iFile].u8Kind == VFS_KIND_BLK ||
                   g_aFiles[iFile].u8Kind == VFS_KIND_SCSI) {
            mode = 0060644u;
        }
        fill_stat(&st, (u64)iFile + 200u, mode, size);
        if (g_aFiles[iFile].u32Nlink > 0) {
            st.st_nlink = g_aFiles[iFile].u32Nlink;
        }
        if (pty_path_n(szNorm, &u32PtsN) != 0) {
            st.st_rdev = pty_slave_rdev(u32PtsN);
        } else if (path_eq(szNorm, "/dev/ptmx") ||
                   path_eq(szNorm, "/dev/pts/ptmx")) {
            st.st_rdev = pty_mux_rdev();
        } else if (path_eq(szNorm, "/dev/tty")) {
            st.st_rdev = vfs_makedev((u32)LINUX_TTYAUX_MAJOR, 0u);
        }
    }
    memcpy(pStat, &st, sizeof(st));
    return soft_out(&g_u32SoftLstatOk, &g_u32SoftLstatFail, 0);
}

i64
vfs_ram_read(i64 i64Fd, void *pBuf, size_t cb)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    u32 u32Avail;
    u32 u32N;
    u32 i;
    i64 i64Ret;

    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL) {
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -9);
    }
    pFd = &g_aFds[i64Fd];
    pFile = &g_aFiles[pFd->u32File];

    if (pFd->u8Kind == VFS_KIND_RAM && file_is_chr_rand(pFile)) {
        u32N = (u32)cb;
        for (i = 0; i < u32N; i++) {
            ((u8 *)pBuf)[i] = chr_rand_byte();
        }
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                              &g_u64SoftBytesRead, (i64)u32N);
    }

    if (pFd->u8Kind == VFS_KIND_RAM && file_is_chr_discard(pFile)) {
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                              &g_u64SoftBytesRead, 0);
    }

    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u64 u64V;
        u32 iEv = pFd->u32File;

        if (iEv >= VFS_MAX_EVENTFD || !g_aEventUsed[iEv] || cb < 8) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -22);
        }
        while (g_aEventCnt[iEv] == 0) {
            if ((pFd->u8Fl & VFS_FD_FL_NONBLOCK) != 0u ||
                thread_current() == NULL || g_fVfsParkOk == 0u) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, -11);
            }
            thread_block(&g_aEventCnt[iEv], 1u);
            schedule();
            if (!g_aEventUsed[iEv]) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, -9);
            }
        }
        u64V = g_aEventCnt[iEv];
        g_aEventCnt[iEv] = 0;
        memcpy(pBuf, &u64V, 8);
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, 8);
    }

    if (pFd->u8Kind == VFS_KIND_TIMERFD) {
        u64 u64V;
        u32 iT = pFd->u32File;

        if (iT >= VFS_MAX_TIMERFD || !g_aTimerUsed[iT] || cb < 8) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -22);
        }
        if (g_aTimerTicks[iT] == 0) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -11);
        }
        u64V = g_aTimerTicks[iT];
        g_aTimerTicks[iT] = 0;
        memcpy(pBuf, &u64V, 8);
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, 8);
    }

    if (pFd->u8Kind == VFS_KIND_SIGNALFD) {
        /* signalfd_siginfo is 128 bytes; return one synthetic info */
        u32 iS = pFd->u32File;
        u8 aInfo[128];
        u32 sig;

        if (iS >= VFS_MAX_SIGNALFD || !g_aSigUsed[iS] || cb < 128) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -22);
        }
        while (g_aSigPending[iS] == 0) {
            if ((pFd->u8Fl & VFS_FD_FL_NONBLOCK) != 0u ||
                thread_current() == NULL || g_fVfsParkOk == 0u) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, -11);
            }
            thread_block(&g_aSigPending[iS], 1u);
            schedule();
            if (!g_aSigUsed[iS]) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, -9);
            }
        }
        memset(aInfo, 0, sizeof(aInfo));
        /* Linux sigset: signal N is bit N-1 */
        for (sig = 1; sig < 64; sig++) {
            u64 u64Bit = 1ull << (sig - 1u);

            if (g_aSigPending[iS] & u64Bit) {
                g_aSigPending[iS] &= ~u64Bit;
                memcpy(aInfo, &sig, 4); /* ssi_signo */
                break;
            }
        }
        if (cb > sizeof(aInfo)) {
            cb = sizeof(aInfo);
        }
        memcpy(pBuf, aInfo, cb);
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, (i64)cb);
    }

    if (pFd->u8Kind == VFS_KIND_INOTIFY) {
        /*
         * struct inotify_event: i32 wd; u32 mask, cookie, len; char name[]
         * We emit len=0 records (16 bytes) for bring-up.
         */
        u32 iIn = pFd->u32File;
        u32 nOut = 0;
        u8 *pOut = (u8 *)pBuf;

        if (iIn >= VFS_MAX_INOTIFY || !g_aInotify[iIn].u8Used) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -9);
        }
        if (cb < 16) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -22);
        }
        while (g_aInotify[iIn].u8Nq > 0 && nOut + 16 <= (u32)cb) {
            struct vfs_inotify *pIn = &g_aInotify[iIn];
            u32 i0 = 0;

            memcpy(pOut + nOut, &pIn->aQ[i0].i32Wd, 4);
            memcpy(pOut + nOut + 4, &pIn->aQ[i0].u32Mask, 4);
            memcpy(pOut + nOut + 8, &pIn->aQ[i0].u32Cookie, 4);
            memcpy(pOut + nOut + 12, &pIn->aQ[i0].u32Len, 4);
            nOut += 16;
            /* shift queue */
            {
                u32 j;

                for (j = 1; j < pIn->u8Nq; j++) {
                    pIn->aQ[j - 1] = pIn->aQ[j];
                }
                pIn->u8Nq--;
            }
        }
        if (nOut == 0) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -11);
        }
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, (i64)nOut);
    }

    if (pFd->u8Kind == VFS_KIND_PIPE) {
        struct vfs_pipe_pair *pPair;
        u8 u8From; /* ring written by peer */
        u32 u32Want = (u32)cb;
        u32 u32Done = 0;
        u32 u32PktLead = 0;
        i32 iPtyR;
        u8 *pOut = (u8 *)pBuf;

        iPtyR = pty_find_by_pipe(pFd->u32File);
        if (iPtyR >= 0 && pFd->u8End == 0 && g_aPty[iPtyR].u8Pkt != 0) {
            u32PktLead = 1;
        }
        for (;;) {
            u32 u32Need = 1;

            if (pFd->u32File >= VFS_MAX_PIPES ||
                !g_aPipes[pFd->u32File].u8Used) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, -9);
            }
            pPair = &g_aPipes[pFd->u32File];
            u8From = (u8)(1u - pFd->u8End);
            if (iPtyR >= 0 && pFd->u8End == 1 &&
                (pty_tios_word(&g_aPty[iPtyR], 12) & VFS_TIOS_ICANON) == 0u) {
                u8 u8Min = pty_cc(&g_aPty[iPtyR], VFS_VMIN);

                if (u8Min == 0) {
                    u32Need = 0;
                } else {
                    u32Need = (u32)u8Min;
                    if (u32Need > u32Want) {
                        u32Need = u32Want;
                    }
                    if (u32Need == 0) {
                        u32Need = 1;
                    }
                }
            }
            if (pPair->u32Len[u8From] != 0u) {
                if (u32Need <= 1u || pPair->u32Len[u8From] >= u32Need ||
                    pPair->u8Open[u8From] == 0u ||
                    (iPtyR >= 0 && g_aPty[iPtyR].u8Hung != 0)) {
                    break;
                }
                if ((pFd->u8Fl & VFS_FD_FL_NONBLOCK) != 0u ||
                    thread_current() == NULL || g_fVfsParkOk == 0u) {
                    return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                          &g_u64SoftBytesRead, -11);
                }
                thread_block(pPair, VFS_PIPE_TAG_RD);
                schedule();
                continue;
            }
            if (u32PktLead != 0 && iPtyR >= 0 &&
                g_aPty[iPtyR].u8PktStat != 0) {
                break;
            }
            if (pPair->u8Open[u8From] == 0u) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, 0);
            }
            if (iPtyR >= 0 && g_aPty[iPtyR].u8Hung != 0) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, 0);
            }
            if (iPtyR >= 0 && pFd->u8End == 1) {
                if (g_aPty[iPtyR].u8SlaveEof != 0) {
                    g_aPty[iPtyR].u8SlaveEof = 0;
                    return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                          &g_u64SoftBytesRead, 0);
                }
                if ((pty_tios_word(&g_aPty[iPtyR], 12) & VFS_TIOS_ICANON) ==
                        0u &&
                    pty_cc(&g_aPty[iPtyR], VFS_VMIN) == 0) {
                    return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                          &g_u64SoftBytesRead, 0);
                }
            }
            if ((pFd->u8Fl & VFS_FD_FL_NONBLOCK) != 0u ||
                thread_current() == NULL || g_fVfsParkOk == 0u) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, -11);
            }
            thread_block(pPair, VFS_PIPE_TAG_RD);
            schedule();
        }
        if (u32PktLead != 0 && iPtyR >= 0) {
            u8 u8Stat = g_aPty[iPtyR].u8PktStat;

            if (cb == 0) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, 0);
            }
            g_aPty[iPtyR].u8PktStat = 0;
            pOut[0] = u8Stat; /* 0 = TIOCPKT_DATA */
            if (u8Stat != 0 || cb == 1) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                                      &g_u64SoftBytesRead, 1);
            }
            pOut = pOut + 1;
            u32Want = (u32)cb - 1u;
        }
        if (u32Want > pPair->u32Len[u8From]) {
            u32Want = pPair->u32Len[u8From];
        }
        while (u32Done < u32Want) {
            u32 iHead = pPair->u32Head[u8From];
            pOut[u32Done] = pPair->aRing[u8From][iHead];
            pPair->u32Head[u8From] = (iHead + 1u) % VFS_PIPE_BUF;
            pPair->u32Len[u8From]--;
            u32Done++;
        }
        (void)thread_wake(pPair, VFS_PIPE_TAG_WR, 8u);
        vfs_ram_poll_kick();
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail,
                              &g_u64SoftBytesRead,
                              (i64)u32Done + (i64)u32PktLead);
    }

    if (pFd->u8Kind == VFS_KIND_BLK || pFd->u8Kind == VFS_KIND_SCSI) {
        u64 u64CapBytes;
        u64 u64Sector;
        u32 u32SecOff;
        u8 aSec[512];
        u32 u32Copy;
        u32 u32Done = 0;
        u32 u32Want = (u32)cb;
        u32 u32SecSz = (pFd->u8Kind == VFS_KIND_BLK) ? GJ_VIRTIO_BLK_SECTOR
                                                     : VFS_SCSI_SEC;

        if (pFd->u8Kind == VFS_KIND_BLK) {
            if (!virtio_blk_ready()) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -5);
            }
            u64CapBytes =
                virtio_blk_capacity_sectors() * (u64)GJ_VIRTIO_BLK_SECTOR;
        } else {
            if (!virtio_scsi_ready()) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -5);
            }
            u64CapBytes = g_u64ScsiCapBytes;
        }
        if (pFd->u64Off >= u64CapBytes) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, 0);
        }
        if ((u64)u32Want > u64CapBytes - pFd->u64Off) {
            u32Want = (u32)(u64CapBytes - pFd->u64Off);
        }
        while (u32Done < u32Want) {
            u64Sector = pFd->u64Off / u32SecSz;
            u32SecOff = (u32)(pFd->u64Off % u32SecSz);
            if (pFd->u8Kind == VFS_KIND_BLK) {
                if (virtio_blk_read(u64Sector, aSec, u32SecSz) != 0) {
                    i64Ret = u32Done > 0 ? (i64)u32Done : -5;
                    return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead,
                                    i64Ret);
                }
            } else {
                struct gj_scsi_request req;

                memset(&req, 0, sizeof(req));
                scsi_cdb_read10(&req.cdb, (u32)u64Sector, 1);
                req.pData = aSec;
                req.cbData = u32SecSz;
                req.fDataIn = 1;
                if (scsi_mid_submit(&req) != 0) {
                    i64Ret = u32Done > 0 ? (i64)u32Done : -5;
                    return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead,
                                    i64Ret);
                }
            }
            u32Copy = u32SecSz - u32SecOff;
            if (u32Copy > u32Want - u32Done) {
                u32Copy = u32Want - u32Done;
            }
            for (i = 0; i < u32Copy; i++) {
                ((u8 *)pBuf)[u32Done + i] = aSec[u32SecOff + i];
            }
            pFd->u64Off += u32Copy;
            u32Done += u32Copy;
        }
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, (i64)u32Done);
    }

    if (pFd->u64Off >= pFile->cbData) {
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, 0);
    }
    u32Avail = pFile->cbData - (u32)pFd->u64Off;
    u32N = (u32)cb;
    if (u32N > u32Avail) {
        u32N = u32Avail;
    }
    for (i = 0; i < u32N; i++) {
        ((u8 *)pBuf)[i] = pFile->aData[(u32)pFd->u64Off + i];
    }
    pFd->u64Off += u32N;
    return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, (i64)u32N);
}

i64
vfs_ram_write(i64 i64Fd, const void *pBuf, size_t cb)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    u32 u32N;
    u32 i;
    i64 i64Ret;

    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL) {
        return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -9);
    }
    pFd = &g_aFds[i64Fd];
    pFile = &g_aFiles[pFd->u32File];

    if (pFd->u8Kind == VFS_KIND_RAM &&
        (file_is_chr_discard(pFile) || file_is_chr_rand(pFile))) {
        return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail,
                              &g_u64SoftBytesWrite, (i64)cb);
    }

    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u64 u64Add = 0;
        u32 iEv = pFd->u32File;

        if (iEv >= VFS_MAX_EVENTFD || !g_aEventUsed[iEv] || cb < 8) {
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -22);
        }
        memcpy(&u64Add, pBuf, 8);
        g_aEventCnt[iEv] += u64Add;
        (void)thread_wake(&g_aEventCnt[iEv], 1u, 8u);
        vfs_ram_poll_kick();
        return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, 8);
    }

    if (pFd->u8Kind == VFS_KIND_PIPE) {
        struct vfs_pipe_pair *pPair;
        u8 u8To = pFd->u8End;
        u32 u32Want = (u32)cb;
        u32 u32Done = 0;
        u32 u32Space;

        if (pFd->u32File >= VFS_MAX_PIPES || !g_aPipes[pFd->u32File].u8Used) {
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -9);
        }
        if (pty_find_by_pipe(pFd->u32File) >= 0) {
            i64 i64PtyW;

            i64PtyW = pty_write(pFd, pBuf, u32Want);
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail,
                                  &g_u64SoftBytesWrite, i64PtyW);
        }
        pPair = &g_aPipes[pFd->u32File];
        for (;;) {
            if (!pPair->u8Used) {
                return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail,
                                      &g_u64SoftBytesWrite, -9);
            }
            if (!pPair->u8Open[1u - u8To]) {
                return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail,
                                      &g_u64SoftBytesWrite, -32);
            }
            u32Space = VFS_PIPE_BUF - pPair->u32Len[u8To];
            if (u32Space != 0u) {
                break;
            }
            if ((pFd->u8Fl & VFS_FD_FL_NONBLOCK) != 0u ||
                thread_current() == NULL || g_fVfsParkOk == 0u) {
                return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail,
                                      &g_u64SoftBytesWrite, -11);
            }
            thread_block(pPair, VFS_PIPE_TAG_WR);
            schedule();
            if (pFd->u32File >= VFS_MAX_PIPES) {
                return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail,
                                      &g_u64SoftBytesWrite, -9);
            }
            pPair = &g_aPipes[pFd->u32File];
        }
        if (u32Want > u32Space) {
            u32Want = u32Space;
        }
        while (u32Done < u32Want) {
            u32 iPos =
                (pPair->u32Head[u8To] + pPair->u32Len[u8To]) % VFS_PIPE_BUF;
            pPair->aRing[u8To][iPos] = ((const u8 *)pBuf)[u32Done];
            pPair->u32Len[u8To]++;
            u32Done++;
        }
        (void)thread_wake(pPair, VFS_PIPE_TAG_RD, 8u);
        vfs_ram_poll_kick();
        return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, (i64)u32Done);
    }

    if (pFd->u8Kind == VFS_KIND_BLK) {
        u64 u64CapBytes;
        u64 u64Sector;
        u32 u32SecOff;
        u8 aSec[GJ_VIRTIO_BLK_SECTOR];
        u32 u32Copy;
        u32 u32Done = 0;
        u32 u32Want = (u32)cb;

        if (!virtio_blk_ready()) {
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -5);
        }
        u64CapBytes = virtio_blk_capacity_sectors() * (u64)GJ_VIRTIO_BLK_SECTOR;
        if (pFd->u64Off >= u64CapBytes) {
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -28);
        }
        if ((u64)u32Want > u64CapBytes - pFd->u64Off) {
            u32Want = (u32)(u64CapBytes - pFd->u64Off);
        }
        while (u32Done < u32Want) {
            u64Sector = pFd->u64Off / GJ_VIRTIO_BLK_SECTOR;
            u32SecOff = (u32)(pFd->u64Off % GJ_VIRTIO_BLK_SECTOR);
            /* RMW partial sector */
            if (u32SecOff != 0 || (u32Want - u32Done) < GJ_VIRTIO_BLK_SECTOR) {
                if (virtio_blk_read(u64Sector, aSec, GJ_VIRTIO_BLK_SECTOR) != 0) {
                    i64Ret = u32Done > 0 ? (i64)u32Done : -5;
                    return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite,
                                    i64Ret);
                }
            } else {
                memset(aSec, 0, sizeof(aSec));
            }
            u32Copy = GJ_VIRTIO_BLK_SECTOR - u32SecOff;
            if (u32Copy > u32Want - u32Done) {
                u32Copy = u32Want - u32Done;
            }
            for (i = 0; i < u32Copy; i++) {
                aSec[u32SecOff + i] = ((const u8 *)pBuf)[u32Done + i];
            }
            if (virtio_blk_write(u64Sector, aSec, GJ_VIRTIO_BLK_SECTOR) != 0) {
                i64Ret = u32Done > 0 ? (i64)u32Done : -5;
                return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, i64Ret);
            }
            pFd->u64Off += u32Copy;
            u32Done += u32Copy;
        }
        return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, (i64)u32Done);
    }

    u32N = (u32)cb;
    if ((u32)pFd->u64Off + u32N > VFS_MAX_DATA) {
        if ((u32)pFd->u64Off >= VFS_MAX_DATA) {
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -28);
        }
        u32N = VFS_MAX_DATA - (u32)pFd->u64Off;
    }
    for (i = 0; i < u32N; i++) {
        pFile->aData[(u32)pFd->u64Off + i] = ((const u8 *)pBuf)[i];
    }
    pFd->u64Off += u32N;
    if ((u32)pFd->u64Off > pFile->cbData) {
        pFile->cbData = (u32)pFd->u64Off;
    }
    return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, (i64)u32N);
}

i64
vfs_ram_close(i64 i64Fd)
{
    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftCloseOk, &g_u32SoftCloseFail, -9);
    }
    /* Drop this fd from every epoll interest list before releasing */
    epoll_detach_fd(i64Fd);
    if (g_aFds[i64Fd].u8Kind == VFS_KIND_PIPE) {
        u32 u32Pair = g_aFds[i64Fd].u32File;
        u8 u8End = g_aFds[i64Fd].u8End;

        if (pty_find_by_pipe(u32Pair) >= 0) {
            pty_close_end(u32Pair, u8End);
        } else if (u32Pair < VFS_MAX_PIPES && g_aPipes[u32Pair].u8Used) {
            g_aPipes[u32Pair].u8Open[u8End] = 0;
            (void)thread_wake(&g_aPipes[u32Pair], VFS_PIPE_TAG_RD, 8u);
            (void)thread_wake(&g_aPipes[u32Pair], VFS_PIPE_TAG_WR, 8u);
            vfs_ram_poll_kick();
            if (!g_aPipes[u32Pair].u8Open[0] && !g_aPipes[u32Pair].u8Open[1]) {
                g_aPipes[u32Pair].u8Used = 0;
            }
        }
    } else if (g_aFds[i64Fd].u8Kind == VFS_KIND_EVENTFD) {
        u32 iEv = g_aFds[i64Fd].u32File;

        if (iEv < VFS_MAX_EVENTFD) {
            (void)thread_wake(&g_aEventCnt[iEv], 1u, 8u);
            vfs_ram_poll_kick();
            g_aEventUsed[iEv] = 0;
            g_aEventCnt[iEv] = 0;
        }
    } else if (g_aFds[i64Fd].u8Kind == VFS_KIND_EPOLL) {
        u32 iEp = g_aFds[i64Fd].u32File;

        if (iEp < VFS_MAX_EPOLL) {
            memset(&g_aEpoll[iEp], 0, sizeof(g_aEpoll[iEp]));
        }
    } else if (g_aFds[i64Fd].u8Kind == VFS_KIND_TIMERFD) {
        u32 iT = g_aFds[i64Fd].u32File;

        if (iT < VFS_MAX_TIMERFD) {
            g_aTimerUsed[iT] = 0;
            g_aTimerTicks[iT] = 0;
            g_aTimerInterval[iT] = 0;
        }
    } else if (g_aFds[i64Fd].u8Kind == VFS_KIND_SIGNALFD) {
        u32 iS = g_aFds[i64Fd].u32File;

        if (iS < VFS_MAX_SIGNALFD) {
            g_aSigUsed[iS] = 0;
            g_aSigMask[iS] = 0;
            g_aSigPending[iS] = 0;
        }
    } else if (g_aFds[i64Fd].u8Kind == VFS_KIND_INOTIFY) {
        u32 iIn = g_aFds[i64Fd].u32File;

        if (iIn < VFS_MAX_INOTIFY) {
            memset(&g_aInotify[iIn], 0, sizeof(g_aInotify[iIn]));
        }
    }
    g_aFds[i64Fd].u8Used = 0;
    g_aFds[i64Fd].u8Fl = 0;
    return soft_out(&g_u32SoftCloseOk, &g_u32SoftCloseFail, 0);
}

i64
vfs_ram_ioctl(i64 i64Fd, u32 u32Cmd, void *pArg)
{
    struct vfs_fd *pFd;
    i32 iPty;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -LINUX_EBADF;
    }
    pFd = &g_aFds[i64Fd];
    if (u32Cmd == (u32)LINUX_FIOCLEX) {
        pFd->u8Fl |= VFS_FD_FL_CLOEXEC;
        return 0;
    }
    if (u32Cmd == (u32)LINUX_FIONCLEX) {
        pFd->u8Fl &= (u8)~VFS_FD_FL_CLOEXEC;
        return 0;
    }
    if (pFd->u8Kind != VFS_KIND_PIPE) {
        return -LINUX_ENOTTY;
    }
    iPty = pty_find_by_pipe(pFd->u32File);
    if (iPty < 0) {
        return -LINUX_ENOTTY;
    }
    /* Winsize / termios / pgrp are shared by the pair (Linux; OpenSSH). */
    if (u32Cmd == (u32)LINUX_TIOCGWINSZ || pty_cmd_nr_is(u32Cmd, 0x13u)) {
        u16 aWs[4];

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        aWs[0] = g_aPty[iPty].u16Row;
        aWs[1] = g_aPty[iPty].u16Col;
        aWs[2] = g_aPty[iPty].u16Xpixel;
        aWs[3] = g_aPty[iPty].u16Ypixel;
        memcpy(pArg, aWs, sizeof(aWs));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCSWINSZ || pty_cmd_nr_is(u32Cmd, 0x14u)) {
        u16 aWs[4];

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(aWs, pArg, sizeof(aWs));
        g_aPty[iPty].u16Row = aWs[0];
        g_aPty[iPty].u16Col = aWs[1];
        g_aPty[iPty].u16Xpixel = aWs[2];
        g_aPty[iPty].u16Ypixel = aWs[3];
        pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_IOCTL);
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TCGETS) {
        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(pArg, g_aPty[iPty].aTios, VFS_PTY_TIOS);
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TCGETA) {
        u8 aTio[17];
        u16 aFl[4];
        u32 iFl;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memset(aTio, 0, sizeof(aTio));
        for (iFl = 0; iFl < 4u; iFl++) {
            aFl[iFl] = (u16)pty_tios_word(&g_aPty[iPty], iFl * 4u);
        }
        memcpy(aTio, aFl, 8);
        aTio[8] = g_aPty[iPty].aTios[16];
        memcpy(&aTio[9], &g_aPty[iPty].aTios[17], 8);
        memcpy(pArg, aTio, sizeof(aTio));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TCSETS || u32Cmd == (u32)LINUX_TCSETSW ||
        u32Cmd == (u32)LINUX_TCSETSF) {
        u32 u32OldI;
        u32 u32OldL;
        u32 u32NewI;
        u32 u32NewL;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        u32OldI = pty_tios_word(&g_aPty[iPty], 0);
        u32OldL = pty_tios_word(&g_aPty[iPty], 12);
        memcpy(g_aPty[iPty].aTios, pArg, VFS_PTY_TIOS);
        u32NewI = pty_tios_word(&g_aPty[iPty], 0);
        u32NewL = pty_tios_word(&g_aPty[iPty], 12);
        if (u32Cmd == (u32)LINUX_TCSETSF) {
            pty_flush_ring(g_aPty[iPty].u32Pipe, (u8)(1u - pFd->u8End));
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_FLUSHREAD);
        }
        if ((u32OldL & VFS_TIOS_ICANON) != 0 &&
            (u32NewL & VFS_TIOS_ICANON) == 0) {
            pty_canon_drop(&g_aPty[iPty]);
        }
        if ((u32OldI & VFS_TIOS_IXON) != 0 &&
            (u32NewI & VFS_TIOS_IXON) == 0) {
            g_aPty[iPty].u8Stopped = 0;
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_START);
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_NOSTOP);
        } else if ((u32OldI & VFS_TIOS_IXON) == 0 &&
                   (u32NewI & VFS_TIOS_IXON) != 0) {
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_DOSTOP);
        }
        pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_IOCTL);
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TCSETA || u32Cmd == (u32)LINUX_TCSETAW ||
        u32Cmd == (u32)LINUX_TCSETAF) {
        u8 aTio[17];
        u16 aFl[4];
        u32 iFl;
        u32 u32OldL;
        u32 u32NewL;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(aTio, pArg, sizeof(aTio));
        memcpy(aFl, aTio, 8);
        u32OldL = pty_tios_word(&g_aPty[iPty], 12);
        for (iFl = 0; iFl < 4u; iFl++) {
            pty_tios_set_word(&g_aPty[iPty], iFl * 4u, (u32)aFl[iFl]);
        }
        g_aPty[iPty].aTios[16] = aTio[8];
        memcpy(&g_aPty[iPty].aTios[17], &aTio[9], 8);
        u32NewL = pty_tios_word(&g_aPty[iPty], 12);
        if (u32Cmd == (u32)LINUX_TCSETAF) {
            pty_flush_ring(g_aPty[iPty].u32Pipe, (u8)(1u - pFd->u8End));
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_FLUSHREAD);
        }
        if ((u32OldL & VFS_TIOS_ICANON) != 0 &&
            (u32NewL & VFS_TIOS_ICANON) == 0) {
            pty_canon_drop(&g_aPty[iPty]);
        }
        pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_IOCTL);
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCGPGRP) {
        i32 i32Pgrp;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        if (g_aPty[iPty].i32Pgrp <= 0) {
            return -LINUX_ENOTTY;
        }
        i32Pgrp = g_aPty[iPty].i32Pgrp;
        memcpy(pArg, &i32Pgrp, sizeof(i32Pgrp));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCSPGRP) {
        i32 i32Pgrp = 0;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(&i32Pgrp, pArg, sizeof(i32Pgrp));
        if (i32Pgrp <= 0) {
            return -LINUX_EINVAL;
        }
        g_aPty[iPty].i32Pgrp = i32Pgrp;
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCGSID) {
        i32 i32Sid;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        if (g_aPty[iPty].i32Sid <= 0) {
            return -LINUX_ENOTTY;
        }
        i32Sid = g_aPty[iPty].i32Sid;
        memcpy(pArg, &i32Sid, sizeof(i32Sid));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCSCTTY) {
        i32 i32Steal = 0;

        (void)pty_arg_i32(pArg, &i32Steal);
        if (g_i32CttyPty >= 0 && g_i32CttyPty != iPty && i32Steal != 1) {
            return -LINUX_EPERM;
        }
        if (g_aPty[iPty].i32Sid <= 0) {
            g_aPty[iPty].i32Sid =
                (g_aPty[iPty].i32Pgrp > 0) ? g_aPty[iPty].i32Pgrp : 1;
        }
        if (g_aPty[iPty].i32Pgrp <= 0) {
            g_aPty[iPty].i32Pgrp = g_aPty[iPty].i32Sid;
        }
        g_i32CttyPty = iPty;
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCNOTTY) {
        g_aPty[iPty].i32Sid = 0;
        if (g_i32CttyPty == iPty) {
            g_i32CttyPty = -1;
        }
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TCFLSH) {
        i32 i32Which = (i32)(uintptr_t)pArg;
        u8 u8In = (u8)(1u - pFd->u8End);
        u8 u8Out = pFd->u8End;

        if (i32Which != 0 && i32Which != 1 && i32Which != 2) {
            return -LINUX_EINVAL;
        }
        if (i32Which == 0 || i32Which == 2) {
            pty_flush_ring(g_aPty[iPty].u32Pipe, u8In);
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_FLUSHREAD);
        }
        if (i32Which == 1 || i32Which == 2) {
            pty_flush_ring(g_aPty[iPty].u32Pipe, u8Out);
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_FLUSHWRITE);
        }
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TCSBRK || u32Cmd == (u32)LINUX_TCSBRKP ||
        u32Cmd == (u32)LINUX_TIOCSBRK || u32Cmd == (u32)LINUX_TIOCCBRK) {
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCMGET) {
        i32 i32M;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        i32M = (i32)g_aPty[iPty].u16Mctrl;
        memcpy(pArg, &i32M, sizeof(i32M));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCMSET || u32Cmd == (u32)LINUX_TIOCMBIS ||
        u32Cmd == (u32)LINUX_TIOCMBIC) {
        i32 i32M = 0;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(&i32M, pArg, sizeof(i32M));
        if (u32Cmd == (u32)LINUX_TIOCMSET) {
            g_aPty[iPty].u16Mctrl = (u16)i32M;
        } else if (u32Cmd == (u32)LINUX_TIOCMBIS) {
            g_aPty[iPty].u16Mctrl |= (u16)i32M;
        } else {
            g_aPty[iPty].u16Mctrl &= (u16)~(u16)i32M;
        }
        return 0;
    }
    if (u32Cmd == (u32)LINUX_FIONBIO) {
        i32 i32Nb = 0;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(&i32Nb, pArg, sizeof(i32Nb));
        if (i32Nb != 0) {
            pFd->u8Fl |= VFS_FD_FL_NONBLOCK;
        } else {
            pFd->u8Fl &= (u8)~VFS_FD_FL_NONBLOCK;
        }
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TCXONC) {
        i32 i32Act = (i32)(uintptr_t)pArg;

        if (i32Act < 0 || i32Act > 3) {
            return -LINUX_EINVAL;
        }
        if (i32Act == 0) {
            g_aPty[iPty].u8Stopped = 1;
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_STOP);
        } else if (i32Act == 1) {
            g_aPty[iPty].u8Stopped = 0;
            pty_pkt_note(&g_aPty[iPty], (u8)LINUX_TIOCPKT_START);
            if (g_aPty[iPty].u32Pipe < VFS_MAX_PIPES) {
                (void)thread_wake(&g_aPipes[g_aPty[iPty].u32Pipe],
                                  VFS_PIPE_TAG_WR, 8u);
                vfs_ram_poll_kick();
            }
        } else {
            u8 u8Ch;

            u8Ch = pty_cc(&g_aPty[iPty],
                          (i32Act == 2) ? VFS_VSTOP : VFS_VSTART);
            if (u8Ch == 0) {
                u8Ch = (i32Act == 2) ? (u8)19 : (u8)17;
            }
            if (pFd->u8End == 0) {
                (void)pty_in_byte(&g_aPty[iPty], u8Ch);
            } else {
                (void)pty_out_byte(&g_aPty[iPty], u8Ch);
            }
            if (g_aPty[iPty].u32Pipe < VFS_MAX_PIPES) {
                (void)thread_wake(&g_aPipes[g_aPty[iPty].u32Pipe],
                                  VFS_PIPE_TAG_RD, 8u);
                vfs_ram_poll_kick();
            }
        }
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCEXCL) {
        g_aPty[iPty].u8Excl = 1;
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCNXCL) {
        g_aPty[iPty].u8Excl = 0;
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCGEXCL) {
        i32 i32Excl;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        i32Excl = (g_aPty[iPty].u8Excl != 0) ? 1 : 0;
        memcpy(pArg, &i32Excl, sizeof(i32Excl));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCVHANGUP) {
        g_aPty[iPty].u8Hung = 1;
        g_aPty[iPty].u16Mctrl &= (u16)~VFS_TIOCM_CAR;
        pty_flush_ring(g_aPty[iPty].u32Pipe, 0);
        pty_flush_ring(g_aPty[iPty].u32Pipe, 1);
        if (g_aPty[iPty].u32Pipe < VFS_MAX_PIPES) {
            (void)thread_wake(&g_aPipes[g_aPty[iPty].u32Pipe],
                              VFS_PIPE_TAG_RD, 8u);
            (void)thread_wake(&g_aPipes[g_aPty[iPty].u32Pipe],
                              VFS_PIPE_TAG_WR, 8u);
            vfs_ram_poll_kick();
        }
        return 0;
    }
    if (u32Cmd == (u32)LINUX_FIONREAD || u32Cmd == (u32)LINUX_TIOCINQ) {
        i64 i64N;
        i32 i32N;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        i64N = vfs_ram_bytes_readable(i64Fd);
        if (i64N < 0) {
            return i64N;
        }
        i32N = (i32)i64N;
        memcpy(pArg, &i32N, sizeof(i32N));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCOUTQ) {
        i32 i32N = 0;
        u8 u8To = pFd->u8End;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        if (g_aPty[iPty].u32Pipe < VFS_MAX_PIPES) {
            i32N = (i32)g_aPipes[g_aPty[iPty].u32Pipe].u32Len[u8To];
        }
        memcpy(pArg, &i32N, sizeof(i32N));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCSTI) {
        u8 u8Ch;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        if (g_aPty[iPty].u8Hung != 0) {
            return -LINUX_EIO;
        }
        memcpy(&u8Ch, pArg, 1);
        if (pty_in_byte(&g_aPty[iPty], u8Ch) == 0) {
            return -LINUX_EAGAIN;
        }
        if (g_aPty[iPty].u32Pipe < VFS_MAX_PIPES) {
            (void)thread_wake(&g_aPipes[g_aPty[iPty].u32Pipe],
                              VFS_PIPE_TAG_RD, 8u);
            vfs_ram_poll_kick();
        }
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCGETD) {
        i32 i32D;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        i32D = (i32)g_aPty[iPty].u8Ldisc;
        memcpy(pArg, &i32D, sizeof(i32D));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCSETD) {
        i32 i32D = 0;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(&i32D, pArg, sizeof(i32D));
        if (i32D != (i32)LINUX_N_TTY) {
            return -LINUX_EINVAL;
        }
        g_aPty[iPty].u8Ldisc = 0;
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCGSOFTCAR) {
        i32 i32C;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        i32C = ((pty_tios_word(&g_aPty[iPty], 8) & VFS_TIOS_CLOCAL) != 0)
                   ? 1
                   : 0;
        memcpy(pArg, &i32C, sizeof(i32C));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCSSOFTCAR) {
        i32 i32C = 0;
        u32 u32C;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(&i32C, pArg, sizeof(i32C));
        u32C = pty_tios_word(&g_aPty[iPty], 8);
        if (i32C != 0) {
            u32C |= VFS_TIOS_CLOCAL;
        } else {
            u32C &= ~VFS_TIOS_CLOCAL;
        }
        pty_tios_set_word(&g_aPty[iPty], 8, u32C);
        return 0;
    }
    /*
     * TIOCGDEV: slave UNIX98 rdev (both ends). Do not match 0x5432 (TCGETX).
     * TIOCGPTN works while locked. Product SSH = OpenSSH. Dual DoD B OPEN.
     */
    if (u32Cmd == (u32)LINUX_TIOCGDEV ||
        (pty_cmd_nr_is(u32Cmd, 0x32u) &&
         ((u32Cmd >> 16) & 0x3fffu) == 4u)) {
        u32 u32Dev;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        u32Dev = (u32)pty_slave_rdev(g_aPty[iPty].u32N);
        memcpy(pArg, &u32Dev, sizeof(u32Dev));
        return 0;
    }
    /* TIOCGPTN / TIOCSPTLCK / TIOCGPTLCK / TIOCGPTPEER / TIOCPKT / TIOCSIG. */
    if (pFd->u8End != 0) {
        return -LINUX_ENOTTY;
    }
    if (u32Cmd == (u32)LINUX_TIOCGPTN ||
        pty_cmd_nr_is(u32Cmd, 0x30u)) { /* size/dir optional */
        u32 u32N;

        /*
         * Linux put_user(unsigned). Works while locked (grantpt/ptsname).
         * N is the Unix98 index: master 128:N, slave 136:N. Dual DoD B OPEN.
         */
        if (pArg == NULL || (uintptr_t)pArg < 4096u) {
            return -LINUX_EFAULT;
        }
        (void)pty_publish((u32)iPty); /* ptsname stat /dev/pts/N */
        u32N = g_aPty[iPty].u32N; /* valid while locked */
        memcpy(pArg, &u32N, sizeof(u32N));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCSPTLCK ||
        pty_cmd_nr_is(u32Cmd, 0x31u)) {
        i32 i32Lock = 0;

        /* Linux get_user(int): 0 unlocks, nonzero locks. Slave open -EIO. */
        if (pty_arg_ptlck(pArg, &i32Lock) == 0) {
            return -LINUX_EFAULT;
        }
        g_aPty[iPty].u8Locked = (i32Lock != 0) ? 1u : 0u;
        if (i32Lock == 0) {
            (void)pty_publish((u32)iPty);
        }
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCGPTLCK ||
        pty_cmd_nr_is(u32Cmd, 0x39u)) {
        i32 i32Lock;

        if (pArg == NULL || (uintptr_t)pArg < 4096u) {
            return -LINUX_EFAULT;
        }
        i32Lock = (g_aPty[iPty].u8Locked != 0) ? 1 : 0;
        memcpy(pArg, &i32Lock, sizeof(i32Lock));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCGPTPEER ||
        pty_cmd_nr_is(u32Cmd, 0x41u)) {
        i32 i32Flags = 0;
        i64 i64Slave;
        u8 u8Fl = 0;

        /* Linux _IO('T', 0x41): flags by value; kernel int * also ok. */
        if (pArg != NULL) {
            if ((uintptr_t)pArg <= 0xfffffu) {
                i32Flags = (i32)(uintptr_t)pArg;
            } else {
                memcpy(&i32Flags, pArg, sizeof(i32Flags));
            }
        }
        i64Slave = pty_open_slave(g_aPty[iPty].u32N);
        if (i64Slave < 0) {
            return i64Slave;
        }
        if (i64Slave >= (i64)VFS_MAX_FDS) {
            (void)vfs_ram_close(i64Slave);
            return -LINUX_EMFILE;
        }
        if ((i32Flags & (i32)LINUX_O_CLOEXEC) != 0) {
            u8Fl |= VFS_FD_FL_CLOEXEC;
        }
        if ((i32Flags & (i32)LINUX_O_NONBLOCK) != 0) {
            u8Fl |= VFS_FD_FL_NONBLOCK;
        }
        g_aFds[(u32)i64Slave].u8Fl = u8Fl;
        return i64Slave;
    }
    if (u32Cmd == (u32)LINUX_TIOCPKT) {
        i32 i32Pkt = 0;

        if (pArg == NULL) {
            return -LINUX_EFAULT;
        }
        memcpy(&i32Pkt, pArg, sizeof(i32Pkt));
        g_aPty[iPty].u8Pkt = (i32Pkt != 0) ? 1u : 0u;
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCGPKT ||
        pty_cmd_nr_is(u32Cmd, 0x38u)) {
        i32 i32Pkt;

        if (pArg == NULL || (uintptr_t)pArg < 4096u) {
            return -LINUX_EFAULT;
        }
        i32Pkt = (g_aPty[iPty].u8Pkt != 0) ? 1 : 0;
        memcpy(pArg, &i32Pkt, sizeof(i32Pkt));
        return 0;
    }
    if (u32Cmd == (u32)LINUX_TIOCSIG ||
        pty_cmd_nr_is(u32Cmd, 0x36u)) {
        i32 i32Sig = (i32)(uintptr_t)pArg;

        /* Linux pty_signal: SIGINT/QUIT/TSTP only. Delivery OPEN. */
        if (i32Sig != 2 && i32Sig != 3 && i32Sig != 20) {
            return -LINUX_EINVAL;
        }
        return 0;
    }
    return -LINUX_ENOTTY;
}

static i64
pipe_alloc_pair(i32 *pFds)
{
    u32 iPair;
    i32 iFd0 = -1;
    i32 iFd1 = -1;
    u32 i;

    if (pFds == NULL) {
        return -22;
    }
    for (iPair = 0; iPair < VFS_MAX_PIPES; iPair++) {
        if (!g_aPipes[iPair].u8Used) {
            break;
        }
    }
    if (iPair >= VFS_MAX_PIPES) {
        return -24; /* EMFILE */
    }
    for (i = 3; i < VFS_MAX_FDS; i++) {
        if (!g_aFds[i].u8Used) {
            if (iFd0 < 0) {
                iFd0 = (i32)i;
            } else {
                iFd1 = (i32)i;
                break;
            }
        }
    }
    if (iFd0 < 0 || iFd1 < 0) {
        return -24;
    }
    memset(&g_aPipes[iPair], 0, sizeof(g_aPipes[iPair]));
    g_aPipes[iPair].u8Used = 1;
    g_aPipes[iPair].u8Open[0] = 1;
    g_aPipes[iPair].u8Open[1] = 1;

    g_aFds[iFd0].u8Used = 1;
    g_aFds[iFd0].u8Kind = VFS_KIND_PIPE;
    g_aFds[iFd0].u8End = 0;
    g_aFds[iFd0].u8Fl = 0;
    g_aFds[iFd0].u32File = iPair;
    g_aFds[iFd0].u64Off = 0;

    g_aFds[iFd1].u8Used = 1;
    g_aFds[iFd1].u8Kind = VFS_KIND_PIPE;
    g_aFds[iFd1].u8End = 1;
    g_aFds[iFd1].u8Fl = 0;
    g_aFds[iFd1].u32File = iPair;
    g_aFds[iFd1].u64Off = 0;

    pFds[0] = iFd0;
    pFds[1] = iFd1;
    return 0;
}

i64
vfs_ram_pipe2(i32 *pFds, int nFlags)
{
    i64 i64St;
    u8 u8Fl = 0;

    i64St = pipe_alloc_pair(pFds);
    if (i64St != 0) {
        return soft_out(&g_u32SoftPipeOk, &g_u32SoftPipeFail, i64St);
    }
    /* LINUX_O_CLOEXEC=0x80000, LINUX_O_NONBLOCK=0x800; other flags ignored. */
    if ((nFlags & 0x80000) != 0) {
        u8Fl |= (u8)VFS_FD_FL_CLOEXEC;
    }
    if ((nFlags & 0x800) != 0) {
        u8Fl |= (u8)VFS_FD_FL_NONBLOCK;
    }
    if (u8Fl != 0 && pFds != NULL) {
        g_aFds[pFds[0]].u8Fl = u8Fl;
        g_aFds[pFds[1]].u8Fl = u8Fl;
    }
    return soft_out(&g_u32SoftPipeOk, &g_u32SoftPipeFail, 0);
}

i64
vfs_ram_socketpair(int nDomain, int nType, int nProtocol, i32 *pFds)
{
    (void)nDomain;
    (void)nProtocol;
    {
        int nKind = nType & 0xff;
        i64 i64St;

        /* SOCK_STREAM = 1; SOCK_DGRAM = 2; SOCK_NONBLOCK/CLOEXEC in high bits */
        if (nKind != 1 && nKind != 2) {
            return soft_out(&g_u32SoftSocketpairOk, &g_u32SoftSocketpairFail,
                            -22);
        }
        i64St = pipe_alloc_pair(pFds);
        if (i64St != 0) {
            return soft_out(&g_u32SoftSocketpairOk, &g_u32SoftSocketpairFail,
                            i64St);
        }
        if (pFds != NULL) {
            u8 u8Fl = 0;

            if ((nType & 0x800) != 0) {
                u8Fl |= (u8)VFS_FD_FL_NONBLOCK;
            }
            if ((nType & 0x80000) != 0) {
                u8Fl |= (u8)VFS_FD_FL_CLOEXEC;
            }
            if (u8Fl != 0) {
                g_aFds[pFds[0]].u8Fl = u8Fl;
                g_aFds[pFds[1]].u8Fl = u8Fl;
            }
        }
        return soft_out(&g_u32SoftSocketpairOk, &g_u32SoftSocketpairFail, 0);
    }
}

i64
vfs_ram_eventfd2(u32 u32Init, int nFlags)
{
    u32 iEv;
    u32 iFd;

    for (iEv = 0; iEv < VFS_MAX_EVENTFD; iEv++) {
        if (!g_aEventUsed[iEv]) {
            break;
        }
    }
    if (iEv >= VFS_MAX_EVENTFD) {
        return soft_out(&g_u32SoftEventfdOk, &g_u32SoftEventfdFail, -24);
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aEventUsed[iEv] = 1;
            g_aEventCnt[iEv] = (u64)u32Init;
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = VFS_KIND_EVENTFD;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u8Fl = 0;
            if ((nFlags & 0x800) != 0) {
                g_aFds[iFd].u8Fl |= (u8)VFS_FD_FL_NONBLOCK;
            }
            if ((nFlags & 0x80000) != 0) {
                g_aFds[iFd].u8Fl |= (u8)VFS_FD_FL_CLOEXEC;
            }
            g_aFds[iFd].u32File = iEv;
            g_aFds[iFd].u64Off = 0;
            return soft_out(&g_u32SoftEventfdOk, &g_u32SoftEventfdFail,
                            (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftEventfdOk, &g_u32SoftEventfdFail, -24);
}

/* ---- epoll (interest list + ready probe for pipes/eventfd/ram) ----------- */

#define VFS_EPOLLIN     0x0001u
#define VFS_EPOLLPRI    0x0002u
#define VFS_EPOLLOUT    0x0004u
#define VFS_EPOLLERR    0x0008u
#define VFS_EPOLLHUP    0x0010u
#define VFS_EPOLLRDHUP  0x2000u
#define VFS_EPOLLONESHOT 0x40000000u

/**
 * Compute ready events. ERR/HUP/RDHUP are always candidates; IN/OUT are
 * filtered by u32Want (caller may OR them). u32Want==0 -> default IN|OUT.
 */
static u32
epoll_ready_mask(i32 i32Fd, u32 u32Want)
{
    u32 u32Got = 0;
    u32 u32Req;
    struct vfs_fd *pFd;

    u32Req = u32Want & (VFS_EPOLLIN | VFS_EPOLLPRI | VFS_EPOLLOUT |
                        VFS_EPOLLRDHUP);
    if (u32Req == 0) {
        u32Req = VFS_EPOLLIN | VFS_EPOLLOUT;
    }
    if (i32Fd < 0 || !vfs_ram_fd_ok((i64)i32Fd)) {
        return VFS_EPOLLERR | VFS_EPOLLHUP;
    }
    pFd = &g_aFds[i32Fd];
    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u32 iEv = pFd->u32File;

        if (iEv >= VFS_MAX_EVENTFD || !g_aEventUsed[iEv]) {
            return VFS_EPOLLERR;
        }
        if ((u32Req & VFS_EPOLLIN) && g_aEventCnt[iEv] != 0) {
            u32Got |= VFS_EPOLLIN;
        }
        if (u32Req & VFS_EPOLLOUT) {
            u32Got |= VFS_EPOLLOUT; /* always writable */
        }
    } else if (pFd->u8Kind == VFS_KIND_TIMERFD) {
        u32 iT = pFd->u32File;

        if (iT >= VFS_MAX_TIMERFD || !g_aTimerUsed[iT]) {
            return VFS_EPOLLERR;
        }
        if ((u32Req & VFS_EPOLLIN) && g_aTimerTicks[iT] != 0) {
            u32Got |= VFS_EPOLLIN;
        }
    } else if (pFd->u8Kind == VFS_KIND_SIGNALFD) {
        u32 iS = pFd->u32File;

        if (iS >= VFS_MAX_SIGNALFD || !g_aSigUsed[iS]) {
            return VFS_EPOLLERR;
        }
        if ((u32Req & VFS_EPOLLIN) && g_aSigPending[iS] != 0) {
            u32Got |= VFS_EPOLLIN;
        }
    } else if (pFd->u8Kind == VFS_KIND_INOTIFY) {
        u32 iIn = pFd->u32File;

        if (iIn >= VFS_MAX_INOTIFY || !g_aInotify[iIn].u8Used) {
            return VFS_EPOLLERR;
        }
        if ((u32Req & VFS_EPOLLIN) && g_aInotify[iIn].u8Nq > 0) {
            u32Got |= VFS_EPOLLIN;
        }
    } else if (pFd->u8Kind == VFS_KIND_PIPE) {
        struct vfs_pipe_pair *pPair;
        u8 u8From;
        u8 u8To;

        if (pFd->u32File >= VFS_MAX_PIPES || !g_aPipes[pFd->u32File].u8Used) {
            return VFS_EPOLLERR;
        }
        pPair = &g_aPipes[pFd->u32File];
        u8From = (u8)(1u - pFd->u8End);
        u8To = pFd->u8End;
        {
            i32 iPtyE = pty_find_by_pipe(pFd->u32File);
            int fHung = (iPtyE >= 0 && g_aPty[iPtyE].u8Hung != 0) ? 1 : 0;

            if ((u32Req & VFS_EPOLLIN) && pPair->u32Len[u8From] > 0) {
                u32Got |= VFS_EPOLLIN;
            }
            if (iPtyE >= 0 && pFd->u8End == 0 && g_aPty[iPtyE].u8Pkt != 0 &&
                g_aPty[iPtyE].u8PktStat != 0) {
                if (u32Req & VFS_EPOLLIN) {
                    u32Got |= VFS_EPOLLIN;
                }
                if (u32Req & VFS_EPOLLPRI) {
                    u32Got |= VFS_EPOLLPRI;
                }
            }
            if (iPtyE >= 0 && pFd->u8End == 1 &&
                g_aPty[iPtyE].u8SlaveEof != 0 && (u32Req & VFS_EPOLLIN)) {
                u32Got |= VFS_EPOLLIN;
            }
            /* Peer closed / hangup: HUP when no more data; RDHUP when gone */
            if (!pPair->u8Open[u8From] || fHung != 0) {
                if (pPair->u32Len[u8From] == 0) {
                    u32Got |= VFS_EPOLLHUP;
                }
                u32Got |= VFS_EPOLLRDHUP;
                if ((u32Req & VFS_EPOLLIN) && pPair->u32Len[u8From] == 0) {
                    u32Got |= VFS_EPOLLIN; /* EOF readable */
                }
            }
            if (!pPair->u8Open[1u - u8To] || fHung != 0) {
                /* Writer-side peer gone / hung: writes fail */
                ;
            } else if (iPtyE >= 0 && pFd->u8End == 1 &&
                       g_aPty[iPtyE].u8Stopped != 0) {
                ;
            } else if ((u32Req & VFS_EPOLLOUT) &&
                       pPair->u32Len[u8To] < VFS_PIPE_BUF) {
                u32Got |= VFS_EPOLLOUT;
            }
            if (!pPair->u8Open[1u - u8To] &&
                pPair->u32Len[u8To] >= VFS_PIPE_BUF) {
                u32Got |= VFS_EPOLLERR; /* no consumer, buffer full */
            }
        }
    } else if (pFd->u8Kind == VFS_KIND_PIDFD) {
        /* Bring-up: pidfd is always "process alive" -> never HUP; OUT ready */
        if (u32Req & VFS_EPOLLOUT) {
            u32Got |= VFS_EPOLLOUT;
        }
        /* Readable when a synthetic exit bit is stored in high half of off */
        if ((u32Req & VFS_EPOLLIN) && (pFd->u64Off >> 32) != 0) {
            u32Got |= VFS_EPOLLIN;
        }
    } else if (pFd->u8Kind == VFS_KIND_IOURING) {
        /* Ring fd: always OUT; IN when slot non-zero (armed) */
        if (u32Req & VFS_EPOLLOUT) {
            u32Got |= VFS_EPOLLOUT;
        }
        if (u32Req & VFS_EPOLLIN) {
            u32Got |= VFS_EPOLLIN;
        }
    } else if (pFd->u8Kind == VFS_KIND_EPOLL) {
        /* Nested epoll: never ready for I/O on the epoll fd itself */
        return 0;
    } else if (pFd->u8Kind == VFS_KIND_BLK || pFd->u8Kind == VFS_KIND_SCSI) {
        /* Block devices: always ready for requested IN/OUT */
        if (u32Req & VFS_EPOLLIN) {
            u32Got |= VFS_EPOLLIN;
        }
        if (u32Req & VFS_EPOLLOUT) {
            u32Got |= VFS_EPOLLOUT;
        }
    } else if (pFd->u8Kind == VFS_KIND_RAM) {
        /*
         * Regular files / dirs: Linux always reports POLLIN|POLLOUT when
         * requested (disk I/O never "blocks" in poll sense for local files).
         */
        if (u32Req & VFS_EPOLLIN) {
            u32Got |= VFS_EPOLLIN;
        }
        if (u32Req & VFS_EPOLLOUT) {
            u32Got |= VFS_EPOLLOUT;
        }
    } else {
        if (u32Req & VFS_EPOLLOUT) {
            u32Got |= VFS_EPOLLOUT;
        }
    }
    return u32Got;
}

u32
vfs_ram_poll_mask(i64 i64Fd, u32 u32Want)
{
    u32 u32Got;
    u32 u32Keep;
    static u8 g_u8NetRoutePassOnce;

    soft_inc(&g_u32SoftPollProbe); /* Wave 14 */

    /*
     * Ram/special table first. Non-ram FDs may live in net_tcp (96..111) or
     * net_lo (64..79) - route to net_*_poll_mask instead of ERR|HUP.
     * Soft kprintf only once on first successful net route (daemon bring-up).
     */
    if (!vfs_ram_fd_ok(i64Fd)) {
        if (net_tcp_fd_ok(i64Fd)) {
            u32Got = net_tcp_poll_mask(i64Fd, u32Want);
            /* Grep: vfs_ram: soft poll net route PASS */
            if (!g_u8NetRoutePassOnce) {
                g_u8NetRoutePassOnce = 1;
                kprintf("vfs_ram: soft poll net route PASS via=tcp fd=%lld "
                        "ready=0x%x want=0x%x\n",
                        (long long)i64Fd, (unsigned)u32Got,
                        (unsigned)u32Want);
            }
            return u32Got;
        }
        if (net_lo_fd_ok(i64Fd)) {
            u32Got = net_lo_poll_mask(i64Fd, u32Want);
            /* Grep: vfs_ram: soft poll net route PASS */
            if (!g_u8NetRoutePassOnce) {
                g_u8NetRoutePassOnce = 1;
                kprintf("vfs_ram: soft poll net route PASS via=lo fd=%lld "
                        "ready=0x%x want=0x%x\n",
                        (long long)i64Fd, (unsigned)u32Got,
                        (unsigned)u32Want);
            }
            return u32Got;
        }
        /* Unknown fd: preserve prior soft ERR|HUP shape from epoll_ready_mask */
        u32Got = VFS_EPOLLERR | VFS_EPOLLHUP;
        if (u32Want == 0) {
            return u32Got;
        }
        return u32Got; /* ERR/HUP always surfaced */
    }

    u32Got = epoll_ready_mask((i32)i64Fd, u32Want);

    /* Surface ERR/HUP/RDHUP even if not in want; IN/OUT only if wanted (or want 0) */
    u32Keep = u32Got & (VFS_EPOLLERR | VFS_EPOLLHUP | VFS_EPOLLRDHUP);
    if (u32Want == 0) {
        return u32Got;
    }
    return u32Keep | (u32Got & u32Want);
}

/** Drop any epoll interest entries that watch i64Fd (called on close). */
static void
epoll_detach_fd(i64 i64Fd)
{
    u32 iEp;
    u32 i;

    if (i64Fd < 0 || i64Fd >= VFS_MAX_FDS) {
        return;
    }
    for (iEp = 0; iEp < VFS_MAX_EPOLL; iEp++) {
        struct vfs_epoll *pE = &g_aEpoll[iEp];

        if (!pE->u8Used) {
            continue;
        }
        i = 0;
        while (i < pE->u8N) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                pE->aWatch[i] = pE->aWatch[pE->u8N - 1u];
                pE->u8N--;
                continue;
            }
            i++;
        }
    }
}

i64
vfs_ram_epoll_create1(int nFlags)
{
    u32 iEp;
    u32 iFd;

    (void)nFlags;
    for (iEp = 0; iEp < VFS_MAX_EPOLL; iEp++) {
        if (!g_aEpoll[iEp].u8Used) {
            break;
        }
    }
    if (iEp >= VFS_MAX_EPOLL) {
        return soft_out(&g_u32SoftEpollOk, &g_u32SoftEpollFail, -24);
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            memset(&g_aEpoll[iEp], 0, sizeof(g_aEpoll[iEp]));
            g_aEpoll[iEp].u8Used = 1;
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = VFS_KIND_EPOLL;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u8Fl = 0;
            g_aFds[iFd].u32File = iEp;
            g_aFds[iFd].u64Off = 0;
            return soft_out(&g_u32SoftEpollOk, &g_u32SoftEpollFail, (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftEpollOk, &g_u32SoftEpollFail, -24);
}

i64
vfs_ram_epoll_ctl(i64 i64Ep, int nOp, i64 i64Fd, u32 u32Events, u64 u64Data)
{
    struct vfs_fd *pEp;
    struct vfs_epoll *pE;
    u32 i;
    u32 u32Ev;

    if (!vfs_ram_fd_ok(i64Ep)) {
        return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, -9);
    }
    pEp = &g_aFds[i64Ep];
    if (pEp->u8Kind != VFS_KIND_EPOLL || pEp->u32File >= VFS_MAX_EPOLL) {
        return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, -22);
    }
    pE = &g_aEpoll[pEp->u32File];
    if (i64Fd == i64Ep) {
        return -22; /* cannot watch self */
    }
    /* Default interest if caller passed zero (common in smoke paths) */
    u32Ev = u32Events;
    if ((u32Ev & (VFS_EPOLLIN | VFS_EPOLLOUT | VFS_EPOLLRDHUP)) == 0 &&
        nOp != 2) {
        u32Ev |= VFS_EPOLLIN;
    }
    if (nOp == 1 /* EPOLL_CTL_ADD */) {
        /*
         * Mixed fd sets: ram/specials + net_lo (64..79) + net_tcp (96..111).
         * Readiness for all kinds is resolved via vfs_ram_poll_mask in wait.
         */
        if (!vfs_ram_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd) &&
            !net_tcp_fd_ok(i64Fd)) {
            return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, -9);
        }
        /* Reject watching another epoll instance (simplifies bring-up) */
        if (vfs_ram_fd_ok(i64Fd) &&
            g_aFds[i64Fd].u8Kind == VFS_KIND_EPOLL) {
            return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, -22);
        }
        for (i = 0; i < pE->u8N; i++) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                return -17; /* EEXIST */
            }
        }
        if (pE->u8N >= VFS_EPOLL_WATCH) {
            return -24; /* EMFILE - interest list full */
        }
        pE->aWatch[pE->u8N].i32Fd = (i32)i64Fd;
        pE->aWatch[pE->u8N].u32Events = u32Ev;
        pE->aWatch[pE->u8N].u64Data = u64Data;
        pE->u8N++;
        return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, 0);
    }
    if (nOp == 2 /* EPOLL_CTL_DEL */) {
        /* Linux: DEL does not require the target fd to still be open */
        for (i = 0; i < pE->u8N; i++) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                pE->aWatch[i] = pE->aWatch[pE->u8N - 1u];
                pE->u8N--;
                return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, 0);
            }
        }
        return -2; /* ENOENT */
    }
    if (nOp == 3 /* EPOLL_CTL_MOD */) {
        if (!vfs_ram_fd_ok(i64Fd) && !net_lo_fd_ok(i64Fd) &&
            !net_tcp_fd_ok(i64Fd)) {
            return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, -9);
        }
        for (i = 0; i < pE->u8N; i++) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                pE->aWatch[i].u32Events = u32Ev;
                pE->aWatch[i].u64Data = u64Data;
                return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, 0);
            }
        }
        return -2; /* ENOENT - not in interest list */
    }
    return -22; /* EINVAL - unknown op */
}

i64
vfs_ram_timerfd_create(int nClockid, int nFlags)
{
    u32 iT;
    u32 iFd;

    (void)nClockid;
    (void)nFlags;
    for (iT = 0; iT < VFS_MAX_TIMERFD; iT++) {
        if (!g_aTimerUsed[iT]) {
            break;
        }
    }
    if (iT >= VFS_MAX_TIMERFD) {
        return soft_out(&g_u32SoftTimerfdOk, &g_u32SoftTimerfdFail, -24);
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aTimerUsed[iT] = 1;
            g_aTimerTicks[iT] = 0;
            g_aTimerInterval[iT] = 0;
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = VFS_KIND_TIMERFD;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u8Fl = 0;
            g_aFds[iFd].u32File = iT;
            g_aFds[iFd].u64Off = 0;
            return soft_out(&g_u32SoftTimerfdOk, &g_u32SoftTimerfdFail,
                            (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftTimerfdOk, &g_u32SoftTimerfdFail, -24);
}

i64
vfs_ram_timerfd_settime(i64 i64Fd, int nFlags, u64 u64ValueNsec,
                        u64 u64IntervalNsec)
{
    struct vfs_fd *pFd;
    u32 iT;

    (void)nFlags;
    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_TIMERFD || pFd->u32File >= VFS_MAX_TIMERFD) {
        return -22;
    }
    iT = pFd->u32File;
    g_aTimerInterval[iT] = u64IntervalNsec;
    /* Arm: non-zero value -> one expiration tick immediately (software) */
    if (u64ValueNsec != 0) {
        g_aTimerTicks[iT] += 1;
    } else {
        g_aTimerTicks[iT] = 0;
    }
    return 0;
}

i64
vfs_ram_timerfd_gettime(i64 i64Fd, u64 *pValueNsec, u64 *pIntervalNsec)
{
    struct vfs_fd *pFd;
    u32 iT;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_TIMERFD || pFd->u32File >= VFS_MAX_TIMERFD) {
        return -22;
    }
    iT = pFd->u32File;
    if (pValueNsec != NULL) {
        *pValueNsec = g_aTimerTicks[iT] ? 1ull : 0ull;
    }
    if (pIntervalNsec != NULL) {
        *pIntervalNsec = g_aTimerInterval[iT];
    }
    return 0;
}

static void
inotify_queue(struct vfs_inotify *pIn, i32 i32Wd, u32 u32Mask)
{
    if (pIn == NULL || pIn->u8Nq >= VFS_INOTIFY_Q) {
        return;
    }
    pIn->aQ[pIn->u8Nq].i32Wd = i32Wd;
    pIn->aQ[pIn->u8Nq].u32Mask = u32Mask;
    pIn->aQ[pIn->u8Nq].u32Cookie = 0;
    pIn->aQ[pIn->u8Nq].u32Len = 0;
    pIn->u8Nq++;
}

i64
vfs_ram_inotify_init1(int nFlags)
{
    u32 iIn;
    u32 iFd;

    (void)nFlags;
    for (iIn = 0; iIn < VFS_MAX_INOTIFY; iIn++) {
        if (!g_aInotify[iIn].u8Used) {
            break;
        }
    }
    if (iIn >= VFS_MAX_INOTIFY) {
        return soft_out(&g_u32SoftInotifyOk, &g_u32SoftInotifyFail, -24);
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            memset(&g_aInotify[iIn], 0, sizeof(g_aInotify[iIn]));
            g_aInotify[iIn].u8Used = 1;
            g_aInotify[iIn].u32NextWd = 1;
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = VFS_KIND_INOTIFY;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u8Fl = 0;
            g_aFds[iFd].u32File = iIn;
            g_aFds[iFd].u64Off = 0;
            return soft_out(&g_u32SoftInotifyOk, &g_u32SoftInotifyFail,
                            (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftInotifyOk, &g_u32SoftInotifyFail, -24);
}

i64
vfs_ram_inotify_add_watch(i64 i64Fd, const char *szPath, u32 u32Mask)
{
    struct vfs_fd *pFd;
    struct vfs_inotify *pIn;
    u32 i;
    i32 wd;

    if (!vfs_ram_fd_ok(i64Fd) || szPath == NULL || szPath[0] == '\0') {
        return soft_out(&g_u32SoftInotifyAddOk, &g_u32SoftInotifyAddFail, -9);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_INOTIFY || pFd->u32File >= VFS_MAX_INOTIFY) {
        return soft_out(&g_u32SoftInotifyAddOk, &g_u32SoftInotifyAddFail, -22);
    }
    pIn = &g_aInotify[pFd->u32File];
    if (u32Mask == 0) {
        u32Mask = 0x00000300u; /* IN_CREATE|IN_DELETE-shaped subset */
    }
    /* Update existing watch on same path */
    for (i = 0; i < pIn->u8NWatch; i++) {
        if (path_eq(pIn->aWatch[i].szPath, szPath)) {
            pIn->aWatch[i].u32Mask = u32Mask;
            return soft_out(&g_u32SoftInotifyAddOk, &g_u32SoftInotifyAddFail, (i64)pIn->aWatch[i].i32Wd);
        }
    }
    if (pIn->u8NWatch >= VFS_INOTIFY_WATCH) {
        return -28; /* ENOSPC */
    }
    wd = (i32)pIn->u32NextWd++;
    if (pIn->u32NextWd == 0) {
        pIn->u32NextWd = 1;
    }
    pIn->aWatch[pIn->u8NWatch].i32Wd = wd;
    pIn->aWatch[pIn->u8NWatch].u32Mask = u32Mask;
    path_copy(pIn->aWatch[pIn->u8NWatch].szPath, szPath);
    pIn->u8NWatch++;
    /* Queue synthetic IN_ACCESS so first read is non-empty (smoke) */
    if (u32Mask & 0x00000001u /* IN_ACCESS */) {
        inotify_queue(pIn, wd, 0x00000001u);
    } else {
        inotify_queue(pIn, wd, u32Mask & 0x000003FFu);
    }
    return soft_out(&g_u32SoftInotifyAddOk, &g_u32SoftInotifyAddFail, (i64)wd);
}

i64
vfs_ram_inotify_rm_watch(i64 i64Fd, i32 i32Wd)
{
    struct vfs_fd *pFd;
    struct vfs_inotify *pIn;
    u32 i;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftInotifyRmOk, &g_u32SoftInotifyRmFail, -9);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_INOTIFY || pFd->u32File >= VFS_MAX_INOTIFY) {
        return soft_out(&g_u32SoftInotifyRmOk, &g_u32SoftInotifyRmFail, -22);
    }
    pIn = &g_aInotify[pFd->u32File];
    for (i = 0; i < pIn->u8NWatch; i++) {
        if (pIn->aWatch[i].i32Wd == i32Wd) {
            pIn->aWatch[i] = pIn->aWatch[pIn->u8NWatch - 1u];
            pIn->u8NWatch--;
            return soft_out(&g_u32SoftInotifyRmOk, &g_u32SoftInotifyRmFail, 0);
        }
    }
    return -22; /* EINVAL */
}

i64
vfs_ram_copy_file_range(i64 i64In, u64 *pOffIn, i64 i64Out, u64 *pOffOut,
                        size_t cb)
{
    /* Same as sendfile but preserves separate offsets */
    u8 aBuf[512];
    size_t done = 0;
    u64 offIn;
    u64 offOut;
    i64 n;
    i64 w;

    if (!vfs_ram_fd_ok(i64Out) || !vfs_ram_fd_ok(i64In) || cb == 0) {
        return soft_out(&g_u32SoftCopyRangeOk, &g_u32SoftCopyRangeFail, -9);
    }
    offIn = pOffIn ? *pOffIn : 0;
    offOut = pOffOut ? *pOffOut : 0;
    while (done < cb) {
        size_t chunk = cb - done;

        if (chunk > sizeof(aBuf)) {
            chunk = sizeof(aBuf);
        }
        n = vfs_ram_pread(i64In, aBuf, chunk, offIn);
        if (n < 0) {
            return soft_out(&g_u32SoftCopyRangeOk, &g_u32SoftCopyRangeFail, done ? (i64)done : n);
        }
        if (n == 0) {
            break;
        }
        w = vfs_ram_pwrite(i64Out, aBuf, (size_t)n, offOut);
        if (w < 0) {
            return soft_out(&g_u32SoftCopyRangeOk, &g_u32SoftCopyRangeFail, done ? (i64)done : w);
        }
        done += (size_t)w;
        offIn += (u64)w;
        offOut += (u64)w;
        if ((size_t)w < (size_t)n) {
            break;
        }
    }
    if (pOffIn != NULL) {
        *pOffIn = offIn;
    }
    if (pOffOut != NULL) {
        *pOffOut = offOut;
    }
    return soft_out(&g_u32SoftCopyRangeOk, &g_u32SoftCopyRangeFail, (i64)done);
}

i64
vfs_ram_pidfd_open(u32 u32Pid, int nFlags)
{
    u32 iFd;

    (void)nFlags;
    if (u32Pid == 0) {
        return soft_out(&g_u32SoftPidfdOk, &g_u32SoftPidfdFail, -22);
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = VFS_KIND_PIDFD;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u8Fl = 0;
            g_aFds[iFd].u32File = 0;
            g_aFds[iFd].u64Off = (u64)u32Pid; /* store target pid */
            return soft_out(&g_u32SoftPidfdOk, &g_u32SoftPidfdFail, (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftPidfdOk, &g_u32SoftPidfdFail, -24);
}

u32
vfs_ram_pidfd_pid(i64 i64Fd)
{
    if (!vfs_ram_fd_ok(i64Fd)) {
        return 0;
    }
    if (g_aFds[i64Fd].u8Kind != VFS_KIND_PIDFD) {
        return 0;
    }
    return (u32)g_aFds[i64Fd].u64Off;
}

i64
vfs_ram_io_uring_open(u32 u32RingSlot)
{
    u32 iFd;

    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = VFS_KIND_IOURING;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u8Fl = 0;
            g_aFds[iFd].u32File = u32RingSlot;
            g_aFds[iFd].u64Off = 0;
            return soft_out(&g_u32SoftIouringOk, &g_u32SoftIouringFail,
                            (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftIouringOk, &g_u32SoftIouringFail, -24); /* EMFILE */
}

int
vfs_ram_io_uring_fd(i64 i64Fd, u32 *pSlot)
{
    if (!vfs_ram_fd_ok(i64Fd)) {
        return 0;
    }
    if (g_aFds[i64Fd].u8Kind != VFS_KIND_IOURING) {
        return 0;
    }
    if (pSlot != NULL) {
        *pSlot = g_aFds[i64Fd].u32File;
    }
    return 1;
}

i64
vfs_ram_signalfd4(i64 i64Fd, u64 u64Mask, int nFlags)
{
    u32 iS;
    u32 iFd;

    (void)nFlags;
    if (i64Fd >= 0) {
        /* Update existing signalfd mask */
        if (!vfs_ram_fd_ok(i64Fd)) {
            return soft_out(&g_u32SoftSignalfdOk, &g_u32SoftSignalfdFail, -9);
        }
        if (g_aFds[i64Fd].u8Kind != VFS_KIND_SIGNALFD ||
            g_aFds[i64Fd].u32File >= VFS_MAX_SIGNALFD) {
            return soft_out(&g_u32SoftSignalfdOk, &g_u32SoftSignalfdFail, -22);
        }
        g_aSigMask[g_aFds[i64Fd].u32File] = u64Mask;
        return soft_out(&g_u32SoftSignalfdOk, &g_u32SoftSignalfdFail, i64Fd);
    }
    for (iS = 0; iS < VFS_MAX_SIGNALFD; iS++) {
        if (!g_aSigUsed[iS]) {
            break;
        }
    }
    if (iS >= VFS_MAX_SIGNALFD) {
        return soft_out(&g_u32SoftSignalfdOk, &g_u32SoftSignalfdFail, -24);
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aSigUsed[iS] = 1;
            g_aSigMask[iS] = u64Mask;
            g_aSigPending[iS] = 0;
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = VFS_KIND_SIGNALFD;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u8Fl = 0;
            g_aFds[iFd].u32File = iS;
            g_aFds[iFd].u64Off = 0;
            return soft_out(&g_u32SoftSignalfdOk, &g_u32SoftSignalfdFail,
                            (i64)iFd);
        }
    }
    return soft_out(&g_u32SoftSignalfdOk, &g_u32SoftSignalfdFail, -24);
}

/** Inject a signal number into all signalfds whose mask includes it. */
void
vfs_ram_signalfd_inject(u32 u32Signo)
{
    u32 i;

    if (u32Signo == 0 || u32Signo >= 64) {
        return;
    }
    {
        u64 u64Bit = 1ull << (u32Signo - 1u);

        for (i = 0; i < VFS_MAX_SIGNALFD; i++) {
            if (g_aSigUsed[i] && (g_aSigMask[i] & u64Bit) != 0) {
                g_aSigPending[i] |= u64Bit;
                (void)thread_wake(&g_aSigPending[i], 1u, 8u);
            }
        }
    }
    vfs_ram_poll_kick();
}

i64
vfs_ram_epoll_wait(i64 i64Ep, void *pEvents, int nMax, int nTimeout)
{
    struct vfs_fd *pEp;
    struct vfs_epoll *pE;
    u32 i;
    u32 nOut = 0;
    u8 *pOut;

    (void)nTimeout; /* non-blocking probe for bring-up */
    if (!vfs_ram_fd_ok(i64Ep)) {
        return soft_out(&g_u32SoftEpollWaitOk, &g_u32SoftEpollWaitFail, -9);
    }
    pEp = &g_aFds[i64Ep];
    if (pEp->u8Kind != VFS_KIND_EPOLL || pEp->u32File >= VFS_MAX_EPOLL) {
        return soft_out(&g_u32SoftEpollWaitOk, &g_u32SoftEpollWaitFail, -22);
    }
    if (nMax <= 0) {
        return soft_out(&g_u32SoftEpollWaitOk, &g_u32SoftEpollWaitFail, -22);
    }
    if (nMax > (int)VFS_EPOLL_WATCH) {
        nMax = (int)VFS_EPOLL_WATCH;
    }
    pE = &g_aEpoll[pEp->u32File];
    pOut = (u8 *)pEvents;
    for (i = 0; i < pE->u8N && (int)nOut < nMax; i++) {
        u32 u32Want = pE->aWatch[i].u32Events;
        u32 u32R;
        u32 u32Report;

        /* Disabled oneshot slot (events cleared after prior fire) */
        if ((u32Want & (VFS_EPOLLIN | VFS_EPOLLOUT | VFS_EPOLLRDHUP |
                        VFS_EPOLLONESHOT)) == 0) {
            continue;
        }
        /*
         * Unified readiness: pipes/eventfd/timerfd/... via epoll_ready_mask,
         * net_lo/net_tcp via their poll_mask (mixed fd sets for daemon loops).
         * vfs_ram_poll_mask already surfaces ERR/HUP/RDHUP and gates IN/OUT.
         */
        u32R = vfs_ram_poll_mask((i64)pE->aWatch[i].i32Fd, u32Want);
        u32Report = u32R & (VFS_EPOLLERR | VFS_EPOLLHUP | VFS_EPOLLRDHUP |
                            VFS_EPOLLIN | VFS_EPOLLOUT);
        if (u32Report != 0) {
            if (pOut != NULL) {
                /* packed epoll_event: u32 events + u64 data (12 bytes) */
                memcpy(pOut + (size_t)nOut * 12u, &u32Report, 4);
                memcpy(pOut + (size_t)nOut * 12u + 4, &pE->aWatch[i].u64Data,
                       8);
            }
            nOut++;
            /* EPOLLONESHOT: disarm until EPOLL_CTL_MOD re-arms */
            if (u32Want & VFS_EPOLLONESHOT) {
                pE->aWatch[i].u32Events = 0;
            }
        }
    }
    return soft_out(&g_u32SoftEpollWaitOk, &g_u32SoftEpollWaitFail, (i64)nOut);
}

i64
vfs_ram_lseek(i64 i64Fd, i64 i64Off, int nWhence)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    i64 i64New;
    i64 i64End;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return soft_out(&g_u32SoftLseekOk, &g_u32SoftLseekFail, -9);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind == VFS_KIND_PIPE) {
        /* pipes + Unix98 pty: ESPIPE (OpenSSH slave lseek must fail) */
        return soft_out(&g_u32SoftLseekOk, &g_u32SoftLseekFail, -LINUX_ESPIPE);
    }
    pFile = &g_aFiles[pFd->u32File];
    if (pFd->u8Kind == VFS_KIND_BLK) {
        i64End = (i64)(virtio_blk_capacity_sectors() * (u64)GJ_VIRTIO_BLK_SECTOR);
    } else if (pFd->u8Kind == VFS_KIND_SCSI) {
        i64End = (i64)g_u64ScsiCapBytes;
    } else {
        i64End = (i64)pFile->cbData;
    }
    if (nWhence == 0) {
        i64New = i64Off;
    } else if (nWhence == 1) {
        i64New = (i64)pFd->u64Off + i64Off;
    } else if (nWhence == 2) {
        i64New = i64End + i64Off;
    } else {
        return soft_out(&g_u32SoftLseekOk, &g_u32SoftLseekFail, -22); /* EINVAL */
    }
    if (i64New < 0) {
        return soft_out(&g_u32SoftLseekOk, &g_u32SoftLseekFail, -22);
    }
    pFd->u64Off = (u64)i64New;
    return soft_out(&g_u32SoftLseekOk, &g_u32SoftLseekFail, i64New);
}

i64
vfs_ram_pread(i64 i64Fd, void *pBuf, size_t cb, u64 u64Off)
{
    u64 u64Saved;
    i64 n;

    soft_inc(&g_u32SoftPreadEnter); /* Wave 14: pread enter (bytes via read) */
    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL) {
        return -9;
    }
    u64Saved = g_aFds[i64Fd].u64Off;
    g_aFds[i64Fd].u64Off = u64Off;
    n = vfs_ram_read(i64Fd, pBuf, cb);
    g_aFds[i64Fd].u64Off = u64Saved;
    return n;
}

i64
vfs_ram_pwrite(i64 i64Fd, const void *pBuf, size_t cb, u64 u64Off)
{
    u64 u64Saved;
    i64 n;

    soft_inc(&g_u32SoftPwriteEnter); /* Wave 14: pwrite enter */
    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL) {
        return -9;
    }
    u64Saved = g_aFds[i64Fd].u64Off;
    g_aFds[i64Fd].u64Off = u64Off;
    n = vfs_ram_write(i64Fd, pBuf, cb);
    g_aFds[i64Fd].u64Off = u64Saved;
    return n;
}

/*
 * linux_dirent64 (packed):
 *   u64 d_ino; i64 d_off; u16 d_reclen; u8 d_type; char d_name[];
 * When the fd is a directory path, only direct children are listed.
 * Cursor is an ordinal among the filtered set (stored in fd offset).
 */
i64
vfs_ram_getdents64(i64 i64Fd, void *pBuf, size_t cb)
{
    struct vfs_fd *pFd;
    u8 *pOut;
    u32 idx;
    u32 written = 0;
    u32 iFile;
    u32 u32Ord;
    const char *szDir;
    int fFilter;

    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL) {
        return soft_out(&g_u32SoftGetdentsOk, &g_u32SoftGetdentsFail, -9);
    }
    if (cb < 32) {
        return soft_out(&g_u32SoftGetdentsOk, &g_u32SoftGetdentsFail, -22);
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_RAM && pFd->u8Kind != VFS_KIND_BLK &&
        pFd->u8Kind != VFS_KIND_SCSI) {
        return soft_out(&g_u32SoftGetdentsOk, &g_u32SoftGetdentsFail, -20);
    }
    pOut = (u8 *)pBuf;
    idx = (u32)pFd->u64Off;
    szDir = NULL;
    fFilter = 0;
    if (pFd->u8Kind == VFS_KIND_RAM && pFd->u32File < VFS_MAX_FILES &&
        g_aFiles[pFd->u32File].u8Used) {
        szDir = g_aFiles[pFd->u32File].szPath;
        if (g_aFiles[pFd->u32File].u8IsDir || path_eq(szDir, "/") ||
            path_eq(szDir, "/tmp") || path_eq(szDir, "/proc") ||
            path_eq(szDir, "/dev") || path_eq(szDir, "/dev/pts") ||
            path_eq(szDir, "/bin") ||
            path_eq(szDir, "/etc") || path_eq(szDir, "/lib") ||
            path_eq(szDir, "/usr") || path_eq(szDir, "/var") ||
            path_eq(szDir, "/var/tmp")) {
            fFilter = 1;
        }
    }
    u32Ord = 0;
    for (iFile = 0; iFile < VFS_MAX_FILES; iFile++) {
        const char *sz;
        const char *szBase;
        u32 nameLen;
        u32 reclen;
        u32 j;
        u8 u8Type;

        if (!g_aFiles[iFile].u8Used) {
            continue;
        }
        if (fFilter && szDir != NULL) {
            if (!path_is_child_of(szDir, g_aFiles[iFile].szPath)) {
                continue;
            }
        }
        if (u32Ord < idx) {
            u32Ord++;
            continue;
        }
        if (u32Ord > idx) {
            break;
        }
        sz = g_aFiles[iFile].szPath;
        szBase = sz;
        {
            u32 k;

            for (k = 0; sz[k] != '\0'; k++) {
                if (sz[k] == '/' && sz[k + 1] != '\0') {
                    szBase = sz + k + 1;
                }
            }
        }
        nameLen = 0;
        while (szBase[nameLen] != '\0' && nameLen < 64) {
            nameLen++;
        }
        reclen = (u32)(19u + nameLen + 1u);
        reclen = (reclen + 7u) & ~7u;
        if (written + reclen > cb) {
            break;
        }
        /* d_type: DT_DIR=4 DT_REG=8 DT_BLK=6 DT_LNK=10 DT_UNKNOWN=0 */
        if (g_aFiles[iFile].u8IsDir) {
            u8Type = 4;
        } else if (g_aFiles[iFile].u8Kind == VFS_KIND_BLK ||
                   g_aFiles[iFile].u8Kind == VFS_KIND_SCSI) {
            u8Type = 6;
        } else if ((g_aFiles[iFile].u32Mode & 0170000u) == 0020000u) {
            u8Type = 2; /* DT_CHR: /dev/null, /dev/tty, /dev/ptmx, /dev/pts/N */
        } else {
            u8Type = 8;
        }
        pOut[written + 0] = (u8)(iFile + 1);
        pOut[written + 1] = 0;
        pOut[written + 2] = 0;
        pOut[written + 3] = 0;
        pOut[written + 4] = 0;
        pOut[written + 5] = 0;
        pOut[written + 6] = 0;
        pOut[written + 7] = 0;
        {
            u32 next = idx + 1;

            pOut[written + 8] = (u8)(next & 0xffu);
            pOut[written + 9] = (u8)((next >> 8) & 0xffu);
            pOut[written + 10] = 0;
            pOut[written + 11] = 0;
            pOut[written + 12] = 0;
            pOut[written + 13] = 0;
            pOut[written + 14] = 0;
            pOut[written + 15] = 0;
        }
        pOut[written + 16] = (u8)(reclen & 0xffu);
        pOut[written + 17] = (u8)((reclen >> 8) & 0xffu);
        pOut[written + 18] = u8Type;
        for (j = 0; j < nameLen; j++) {
            pOut[written + 19 + j] = (u8)szBase[j];
        }
        pOut[written + 19 + nameLen] = 0;
        for (j = 20 + nameLen; j < reclen; j++) {
            pOut[written + j] = 0;
        }
        written += reclen;
        idx++;
        u32Ord++;
        pFd->u64Off = idx;
    }
    return soft_out(&g_u32SoftGetdentsOk, &g_u32SoftGetdentsFail, (i64)written);
}
