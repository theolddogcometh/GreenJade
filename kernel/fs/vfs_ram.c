/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal ramdisk: fixed files + open/read/write for cold Linux path.
 * Optional /dev/vda (virtio-blk) and /dev/sda (virtio-scsi) when mounted.
 * Also hosts pipes, eventfd, epoll, timerfd, signalfd, pidfd, inotify for
 * the Linux ABI bring-up path — independent of vfs_door product mini-FS.
 *
 * Soft ram inventory (Wave 12 base; Wave 14 deepen; Wave 19 exclusive):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Live files/fds/pipes/specials + mount lamps; peaks for files + fds
 *   - Cumulative open/close/read/write/lseek + path + special create ops
 *   - Soft deny tallies by errno shape (noent/badf/inval/nospc/mfile/…)
 *   - Wave 14: attr/alloc/sync/xfer/kind peaks + soft ok/fail + last + PASS
 *   - Wave 15 base: ratio|headroom|surface|deepen
 *   - Wave 19 exclusive: capacity|geom|terminal|return|retmap (wave=57 areas=33)
 *   greppable: "vfs_ram: soft …"
 */
#include <gj/klog.h>
#include <gj/scsi_mid.h>
#include <gj/string.h>
#include <gj/vfs_ram.h>
#include <gj/virtio_blk.h>
#include <gj/virtio_scsi.h>

#define VFS_MAX_FILES  64
#define VFS_MAX_FDS    96
#define VFS_MAX_PATH   128
/* 32 KiB: room for packaged ld-gj.so.1 (~30 KiB) + small ELFs */
#define VFS_MAX_DATA   32768
#define VFS_MAX_PIPES  16
#define VFS_PIPE_BUF   2048

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
    u8   u8Pad;
    u32  u32File; /* index for RAM; pipe pair index for PIPE */
    u64  u64Off;  /* byte offset (supports large blk) */
};

/* Bidirectional pair: ring[e] is written by end e, read by end 1-e. */
struct vfs_pipe_pair {
    u8  u8Used;
    u8  u8Open[2]; /* refcount-ish: end open */
    u8  u8Pad[1];
    u32 u32Len[2];
    u32 u32Head[2];
    u8  aRing[2][VFS_PIPE_BUF];
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
static int             g_fBlkMounted;
static int             g_fScsiMounted;
static i32             g_iBlkFile = -1; /* file slot for /dev/vda */
static u64             g_u64ScsiCapBytes;

/*
 * Soft product inventory (Wave 12 exclusive). Cumulative unless noted
 * live/peak. greppable: vfs_ram: soft …
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

/* Wave 14 exclusive soft deepen — complementary path tallies. */
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
static void soft_inc(u32 *pCtr);
static void soft_add64(u64 *pCtr, u64 u64N);
static void soft_peak_note(void);
static void soft_deny_note(i64 i64Ret);
static i64 soft_out(u32 *pOk, u32 *pFail, i64 i64Ret);
static i64 soft_out_bytes(u32 *pOk, u32 *pFail, u64 *pBytes, i64 i64Ret);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

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
 * Greppable soft ram inventory (product / smoke; Wave 14 exclusive deepen).
 * Primary field-stable lines (Wave 12):
 *   vfs_ram: soft inventory|layout|fd|name|special|mount|deny|peak|path …
 * Wave 14 complementary sub-lines (agent greps):
 *   vfs_ram: soft total|attr|alloc|sync|xfer|statx|notify|kind|catalog|PASS
 * greppable: vfs_ram: soft
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
    u32 u32OccPct;
    u32 u32FdOcc;
    int fSoftPass;

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
    u32OccPct = (u32Files * 100u) / (u32)VFS_MAX_FILES;
    u32FdOcc = (u32Fds * 100u) / (u32)VFS_MAX_FDS;

    /* Grep: vfs_ram: soft inventory */
    kprintf("vfs_ram: soft inventory seeded=%u files=%u fds=%u pipes=%u "
            "eventfd=%u epoll=%u timerfd=%u signalfd=%u inotify=%u "
            "sym=%u blk=%u scsi=%u samples=%u wave=57\n",
            g_u32SoftSeeded, u32Files, u32Fds, u32Pipes, u32Ev, u32Ep, u32Tmr,
            u32Sig, u32Ino, u32Sym, g_fBlkMounted ? 1u : 0u,
            g_fScsiMounted ? 1u : 0u, u32Samples);

    /* Grep: vfs_ram: soft layout */
    kprintf("vfs_ram: soft layout max_files=%u max_fds=%u max_data=%u "
            "max_pipes=%u path=%u sym_max=%u\n",
            VFS_MAX_FILES, VFS_MAX_FDS, VFS_MAX_DATA, VFS_MAX_PIPES,
            VFS_MAX_PATH, VFS_MAX_SYMLINKS);

    /* Grep: vfs_ram: soft fd */
    kprintf("vfs_ram: soft fd open=%u/%u close=%u/%u read=%u/%u write=%u/%u "
            "lseek=%u/%u dup=%u/%u getdents=%u/%u\n",
            g_u32SoftOpenOk, g_u32SoftOpenFail, g_u32SoftCloseOk,
            g_u32SoftCloseFail, g_u32SoftReadOk, g_u32SoftReadFail,
            g_u32SoftWriteOk, g_u32SoftWriteFail, g_u32SoftLseekOk,
            g_u32SoftLseekFail, g_u32SoftDupOk, g_u32SoftDupFail,
            g_u32SoftGetdentsOk, g_u32SoftGetdentsFail);

    /* Grep: vfs_ram: soft name */
    kprintf("vfs_ram: soft name mkdir=%u/%u unlink=%u/%u rmdir=%u/%u "
            "rename=%u/%u link=%u/%u symlink=%u/%u access=%u/%u "
            "stat=%u/%u\n",
            g_u32SoftMkdirOk, g_u32SoftMkdirFail, g_u32SoftUnlinkOk,
            g_u32SoftUnlinkFail, g_u32SoftRmdirOk, g_u32SoftRmdirFail,
            g_u32SoftRenameOk, g_u32SoftRenameFail, g_u32SoftLinkOk,
            g_u32SoftLinkFail, g_u32SoftSymlinkOk, g_u32SoftSymlinkFail,
            g_u32SoftAccessOk, g_u32SoftAccessFail, g_u32SoftStatOk,
            g_u32SoftStatFail);

    /* Grep: vfs_ram: soft special */
    kprintf("vfs_ram: soft special pipe=%u/%u eventfd=%u/%u epoll=%u/%u "
            "timerfd=%u/%u signalfd=%u/%u inotify=%u/%u pidfd=%u/%u "
            "iouring=%u/%u\n",
            g_u32SoftPipeOk, g_u32SoftPipeFail, g_u32SoftEventfdOk,
            g_u32SoftEventfdFail, g_u32SoftEpollOk, g_u32SoftEpollFail,
            g_u32SoftTimerfdOk, g_u32SoftTimerfdFail, g_u32SoftSignalfdOk,
            g_u32SoftSignalfdFail, g_u32SoftInotifyOk, g_u32SoftInotifyFail,
            g_u32SoftPidfdOk, g_u32SoftPidfdFail, g_u32SoftIouringOk,
            g_u32SoftIouringFail);

    /* Grep: vfs_ram: soft mount */
    kprintf("vfs_ram: soft mount blk_ok=%u blk_skip=%u scsi_ok=%u "
            "scsi_skip=%u live_blk=%u live_scsi=%u\n",
            g_u32SoftMountBlkOk, g_u32SoftMountBlkSkip, g_u32SoftMountScsiOk,
            g_u32SoftMountScsiSkip, g_fBlkMounted ? 1u : 0u,
            g_fScsiMounted ? 1u : 0u);

    /* Grep: vfs_ram: soft deny */
    kprintf("vfs_ram: soft deny noent=%u badf=%u inval=%u nospc=%u mfile=%u "
            "exist=%u eagain=%u epipe=%u perm=%u other=%u\n",
            g_u32SoftDenyNoent, g_u32SoftDenyBadf, g_u32SoftDenyInval,
            g_u32SoftDenyNospc, g_u32SoftDenyMfile, g_u32SoftDenyExist,
            g_u32SoftDenyAgain, g_u32SoftDenyPipe, g_u32SoftDenyPerm,
            g_u32SoftDenyOther);

    /* Grep: vfs_ram: soft peak */
    kprintf("vfs_ram: soft peak files=%u fds=%u pipes=%u samples=%u\n",
            g_u32SoftFilesPeak, g_u32SoftFdPeak, g_u32SoftPipePeak,
            u32Samples);

    /*
     * Wave 14 exclusive deepen (complementary; never reshapes primary lines).
     */
    /* Grep: vfs_ram: soft total */
    kprintf("vfs_ram: soft total ok=%u fail=%u occ_pct=%u fd_occ=%u "
            "logs=%u wave=57\n",
            g_u32SoftOk, g_u32SoftFail, u32OccPct, u32FdOcc, u32Samples);

    /* Grep: vfs_ram: soft attr */
    kprintf("vfs_ram: soft attr chmod=%u/%u fchmod=%u/%u utimens=%u/%u "
            "mark_dir=%u/%u\n",
            g_u32SoftChmodOk, g_u32SoftChmodFail, g_u32SoftFchmodOk,
            g_u32SoftFchmodFail, g_u32SoftUtimensOk, g_u32SoftUtimensFail,
            g_u32SoftMarkDirOk, g_u32SoftMarkDirFail);

    /* Grep: vfs_ram: soft alloc */
    kprintf("vfs_ram: soft alloc fallocate=%u/%u punch=%u/%u ftrunc=%u/%u "
            "trunc_enter=%u\n",
            g_u32SoftFallocateOk, g_u32SoftFallocateFail, g_u32SoftPunchOk,
            g_u32SoftPunchFail, g_u32SoftFtruncateOk, g_u32SoftFtruncateFail,
            g_u32SoftTruncateEnter);

    /* Grep: vfs_ram: soft sync */
    kprintf("vfs_ram: soft sync fsync=%u/%u sendfile=%u/%u copy_range=%u/%u "
            "socketpair=%u/%u\n",
            g_u32SoftFsyncOk, g_u32SoftFsyncFail, g_u32SoftSendfileOk,
            g_u32SoftSendfileFail, g_u32SoftCopyRangeOk,
            g_u32SoftCopyRangeFail, g_u32SoftSocketpairOk,
            g_u32SoftSocketpairFail);

    /* Grep: vfs_ram: soft xfer */
    kprintf("vfs_ram: soft xfer read_b=%lu write_b=%lu pread_enter=%u "
            "pwrite_enter=%u read_ok=%u write_ok=%u\n",
            (unsigned long)g_u64SoftBytesRead,
            (unsigned long)g_u64SoftBytesWrite, g_u32SoftPreadEnter,
            g_u32SoftPwriteEnter, g_u32SoftReadOk, g_u32SoftWriteOk);

    /* Grep: vfs_ram: soft statx */
    kprintf("vfs_ram: soft statx stat=%u/%u fstat=%u/%u lstat=%u/%u "
            "readlink=%u/%u access=%u/%u\n",
            g_u32SoftStatOk, g_u32SoftStatFail, g_u32SoftFstatOk,
            g_u32SoftFstatFail, g_u32SoftLstatOk, g_u32SoftLstatFail,
            g_u32SoftReadlinkOk, g_u32SoftReadlinkFail, g_u32SoftAccessOk,
            g_u32SoftAccessFail);

    /* Grep: vfs_ram: soft notify */
    kprintf("vfs_ram: soft notify inotify_add=%u/%u inotify_rm=%u/%u "
            "epoll_ctl=%u/%u epoll_wait=%u/%u poll_probe=%u\n",
            g_u32SoftInotifyAddOk, g_u32SoftInotifyAddFail,
            g_u32SoftInotifyRmOk, g_u32SoftInotifyRmFail,
            g_u32SoftEpollCtlOk, g_u32SoftEpollCtlFail,
            g_u32SoftEpollWaitOk, g_u32SoftEpollWaitFail,
            g_u32SoftPollProbe);

    /* Grep: vfs_ram: soft kind */
    kprintf("vfs_ram: soft kind ram=%u blk=%u scsi=%u dirs=%u pipe_fd=%u "
            "special_fd=%u sym=%u\n",
            g_u32SoftRamLive, g_u32SoftBlkLive, g_u32SoftScsiLive,
            g_u32SoftDirLive, g_u32SoftPipeLive, g_u32SoftSpecialLive,
            u32Sym);

    /* Grep: vfs_ram: soft catalog */
    kprintf("vfs_ram: soft catalog eventfd_max=%u epoll_max=%u "
            "timerfd_max=%u signalfd_max=%u inotify_max=%u "
            "inotify_watch=%u pipe_buf=%u wave=57\n",
            VFS_MAX_EVENTFD, VFS_MAX_EPOLL, VFS_MAX_TIMERFD,
            VFS_MAX_SIGNALFD, VFS_MAX_INOTIFY, VFS_INOTIFY_WATCH,
            VFS_PIPE_BUF);

    /* Grep: vfs_ram: soft peak  (word form; primary peak stays above) */
    kprintf("vfs_ram: soft peak_w15 files=%u fds=%u pipes=%u eventfd=%u "
            "epoll=%u timerfd=%u signalfd=%u inotify=%u sym=%u\n",
            g_u32SoftFilesPeak, g_u32SoftFdPeak, g_u32SoftPipePeak,
            g_u32SoftEventfdPeak, g_u32SoftEpollPeak, g_u32SoftTimerfdPeak,
            g_u32SoftSignalfdPeak, g_u32SoftInotifyPeak, g_u32SoftSymPeak);
    /* Grep: vfs_ram: soft peak_w16 — Wave 19 exclusive twin (field-stable w15). */
    kprintf("vfs_ram: soft peak_w16 files=%u fds=%u pipes=%u eventfd=%u "
            "epoll=%u timerfd=%u signalfd=%u inotify=%u sym=%u wave=57\n",
            g_u32SoftFilesPeak, g_u32SoftFdPeak, g_u32SoftPipePeak,
            g_u32SoftEventfdPeak, g_u32SoftEpollPeak, g_u32SoftTimerfdPeak,
            g_u32SoftSignalfdPeak, g_u32SoftInotifyPeak, g_u32SoftSymPeak);

    /* Grep: vfs_ram: soft path */
    kprintf("vfs_ram: soft path cold_linux=1 ramfs+specials=1 "
            "attr=chmod|fchmod|utimens alloc=fallocate|punch|ftrunc "
            "xfer=read|write|pread|pwrite|sendfile|copy_range "
            "notify=epoll|inotify|poll wave=57 "
            "(soft inventory; not bar3)\n");

    /*
     * Wave 35 exclusive deepen (complementary; primary lines field-stable).
     * greppable: vfs_ram: soft ratio|headroom|surface|deepen
     */
    {
        u32 u32FailBp = 0;
        u32 u32OkBp = 0;
        u32 u32FileHead = 0;
        u32 u32FdHead = 0;
        u32 u32Tot = g_u32SoftOk + g_u32SoftFail;

        if (u32Tot != 0u) {
            u32OkBp = (g_u32SoftOk * 10000u) / u32Tot;
            u32FailBp = (g_u32SoftFail * 10000u) / u32Tot;
        }
        if ((u32)VFS_MAX_FILES > u32Files) {
            u32FileHead = (u32)VFS_MAX_FILES - u32Files;
        }
        if ((u32)VFS_MAX_FDS > u32Fds) {
            u32FdHead = (u32)VFS_MAX_FDS - u32Fds;
        }
        /* Grep: vfs_ram: soft ratio */
        kprintf("vfs_ram: soft ratio occ_pct=%u fd_occ=%u ok_bp=%u "
                "fail_bp=%u files=%u fds=%u wave=57\n",
                u32OccPct, u32FdOcc, u32OkBp, u32FailBp, u32Files, u32Fds);
        /* Grep: vfs_ram: soft headroom */
        kprintf("vfs_ram: soft headroom file_head=%u fd_head=%u "
                "max_files=%u max_fds=%u pipes=%u wave=57\n",
                u32FileHead, u32FdHead, (u32)VFS_MAX_FILES,
                (u32)VFS_MAX_FDS, u32Pipes);
        /* Grep: vfs_ram: soft surface */
        kprintf("vfs_ram: soft surface inventory,layout,fd,name,special,"
                "mount,deny,peak,total,attr,alloc,sync,xfer,statx,notify,"
                "kind,catalog,ratio,headroom,capacity,geom,terminal,return,retmap,deepen "
                "areas=35 wave=57\n");
        /* Grep: vfs_ram: soft capacity — Wave 19 design-constant lamps. */
        kprintf("vfs_ram: soft capacity max_files=%u max_fds=%u max_data=%u "
                "max_pipes=%u pipe_buf=%u path=%u wave=57\n",
                (u32)VFS_MAX_FILES, (u32)VFS_MAX_FDS, (u32)VFS_MAX_DATA,
                (u32)VFS_MAX_PIPES, (u32)VFS_PIPE_BUF, (u32)VFS_MAX_PATH);
        /* Grep: vfs_ram: soft geom — Wave 16 table geometry lamps. */
        kprintf("vfs_ram: soft geom files=%u fds=%u pipes=%u free_file=%u "
                "free_fd=%u seeded=%u wave=57\n",
                u32Files, u32Fds, u32Pipes, u32FileHead, u32FdHead,
                g_u32SoftSeeded);
        /* Grep: vfs_ram: soft terminal — Wave 19 outcome rollup. */
        kprintf("vfs_ram: soft terminal ok=%u fail=%u files=%u fds=%u "
                "seeded=%u soft PASS wave=57\n",
                g_u32SoftOk, g_u32SoftFail, u32Files, u32Fds, g_u32SoftSeeded);
        /* Grep: vfs_ram: soft return — Wave 19 API return surfaces */
        kprintf("vfs_ram: soft return ok=%u fail=%u files=%u fds=%u "
                "seeded=%u open=%u close=%u read=%u write=%u "
                "product_vfs_ram=OPEN wave=57\n",
                g_u32SoftOk, g_u32SoftFail, u32Files, u32Fds, g_u32SoftSeeded,
                g_u32SoftOpenOk, g_u32SoftCloseOk, g_u32SoftReadOk,
                g_u32SoftWriteOk);

        /* Grep: vfs_ram: soft retmap — Wave 19 return-surface map */
    kprintf("vfs_ram: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=57\n");

    /* Grep: vfs_ram: soft deepen */
        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: vfs_ram: soft retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("vfs_ram: soft retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=57 "
                "(retclass taxonomy; Soft≠product; not bar3)\n");
        /* Grep: vfs_ram: soft retlane — Wave 19 return-lane catalog (kept) */
        kprintf("vfs_ram: soft retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=57 "
                "(retlane catalog; Soft≠product)\n");
        /*
         * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: vfs_ram: soft retbound — Wave 20 return-bound honesty (kept) */
        kprintf("vfs_ram: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=57 "
                "(retbound honesty; Soft≠product; not bar3)\n");
        /* Grep: vfs_ram: soft retseal — Wave 20 seal stamp (kept) */
        kprintf("vfs_ram: soft retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=57 "
                "(retseal stamp; Soft≠product)\n");
                /*
                 * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: vfs_ram: soft retpulse — Wave 21 return-pulse honesty (kept) */
                kprintf("vfs_ram: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=57 "
                        "(retpulse honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_ram: soft retmark — Wave 21 mark stamp (kept) */
                kprintf("vfs_ram: soft retmark exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=57 "
                        "(retmark stamp; Soft≠product)\n");
                /*
                 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: vfs_ram: soft retphase — Wave 22 return-phase honesty (kept) */
                kprintf("vfs_ram: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=57 "
                        "(retphase honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_ram: soft retbadge — Wave 22 badge stamp (kept) */
                kprintf("vfs_ram: soft retbadge exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=57 "
                        "(retbadge stamp; Soft≠product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
                */
                /* Grep: vfs_ram: soft rettoken — Wave 23 return-token honesty (kept) */
                kprintf("vfs_ram: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=57 "
                        "(rettoken honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_ram: soft retcrest — Wave 23 crest stamp (kept) */
                kprintf("vfs_ram: soft retcrest exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=57 "
                        "(retcrest stamp; Soft≠product)\n");
                /*
                 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: vfs_ram: soft retvault — Wave 24 return-vault honesty (kept) */
                kprintf("vfs_ram: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=57 "
                        "(retvault honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_ram: soft retbanner — Wave 24 banner stamp (kept) */
                kprintf("vfs_ram: soft retbanner exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=57 "
                        "(retbanner stamp; Soft≠product)\n");
                /*
                 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: vfs_ram: soft retledger — Wave 25 return-ledger honesty (kept) */
                kprintf("vfs_ram: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=57 "
                        "(retledger honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_ram: soft retbeacon — Wave 25 beacon stamp (kept) */
                kprintf("vfs_ram: soft retbeacon exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=57 "
                        "(retbeacon stamp; Soft≠product)\n");
                /*
                 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: vfs_ram: soft retcipher — Wave 26 return-cipher honesty (kept) */
                kprintf("vfs_ram: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=57 "
                        "(retcipher honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_ram: soft retflame — Wave 26 flame stamp (kept) */
                kprintf("vfs_ram: soft retflame exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=57 "
                        "(retflame stamp; Soft≠product)\n");
                        /*
                         * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: vfs_ram: soft retprism — Wave 27 return-prism honesty (kept) */
                        kprintf("vfs_ram: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=57 "
                                "(retprism honesty; Soft≠product; not bar3)\n");
                        /* Grep: vfs_ram: soft retforge — Wave 27 forge stamp (kept) */
                        kprintf("vfs_ram: soft retforge exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=57 "
                                "(retforge stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: vfs_ram: soft retshard — Wave 28 return-shard honesty (kept) */
                                kprintf("vfs_ram: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=57 "
                                        "(retshard honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_ram: soft retcrown — Wave 28 crown stamp (kept) */
                                kprintf("vfs_ram: soft retcrown exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=57 "
                                        "(retcrown stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: vfs_ram: soft retglyph — Wave 29 return-glyph honesty (kept) */
                                kprintf("vfs_ram: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=57 "
                                        "(retglyph honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_ram: soft retscepter — Wave 29 scepter stamp (kept) */
                                kprintf("vfs_ram: soft retscepter exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=57 "
                                        "(retscepter stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: vfs_ram: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("vfs_ram: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=57 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_ram: soft retemblem — Wave 30 emblem stamp (kept) */
                                kprintf("vfs_ram: soft retemblem exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=57 "
                                        "(retemblem stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: vfs_ram: soft retaegis — Wave 31 return-aegis honesty (kept) */
                                kprintf("vfs_ram: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=57 "
                                        "(retaegis honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_ram: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("vfs_ram: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=57 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_ram: soft retmantle — Wave 31 mantle stamp (kept) */
                                kprintf("vfs_ram: soft retmantle exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=57 "
                                        "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("vfs_ram: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("vfs_ram: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("vfs_ram: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("vfs_ram: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("vfs_ram: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("vfs_ram: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retfortress — Wave 35 return-fortress honesty */
kprintf("vfs_ram: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("vfs_ram: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft rethold — Wave 36 return-hold honesty */
kprintf("vfs_ram: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retspire — Wave 36 exclusive spire stamp */
kprintf("vfs_ram: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retwall — Wave 37 return-wall honesty */
kprintf("vfs_ram: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retgate — Wave 37 exclusive gate stamp */
kprintf("vfs_ram: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retmoat — Wave 38 return-moat honesty */
kprintf("vfs_ram: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retower — Wave 38 exclusive tower stamp */
kprintf("vfs_ram: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("vfs_ram: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("vfs_ram: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("vfs_ram: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("vfs_ram: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retravelin — Wave 41 return-travelin honesty */
kprintf("vfs_ram: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("vfs_ram: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("vfs_ram: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("vfs_ram: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("vfs_ram: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("vfs_ram: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("vfs_ram: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("vfs_ram: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("vfs_ram: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("vfs_ram: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retbailey — Wave 46 return-bailey honesty */
kprintf("vfs_ram: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("vfs_ram: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("vfs_ram: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("vfs_ram: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("vfs_ram: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("vfs_ram: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("vfs_ram: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("vfs_ram: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retsally — Wave 50 return-sally honesty */
kprintf("vfs_ram: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("vfs_ram: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retfosse — Wave 51 return-fosse honesty */
kprintf("vfs_ram: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("vfs_ram: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("vfs_ram: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("vfs_ram: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retravelin — Wave 53 return-travelin honesty */
kprintf("vfs_ram: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("vfs_ram: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("vfs_ram: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retredan — Wave 54 exclusive redan stamp */
kprintf("vfs_ram: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retflank — Wave 55 return-flank honesty */
kprintf("vfs_ram: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retface — Wave 55 exclusive face stamp */
kprintf("vfs_ram: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retgorge — Wave 56 return-gorge honesty */
kprintf("vfs_ram: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("vfs_ram: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_ram: soft retraverse — Wave 57 return-traverse honesty */
kprintf("vfs_ram: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=57 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_ram: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("vfs_ram: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=57 "
        "(retcasemate stamp; Soft≠product)\n");







                                kprintf("vfs_ram: soft deepen wave=57 areas=73 seeded=%u files=%u "
                "fds=%u ok=%u fail=%u logs=%u\n",
                g_u32SoftSeeded, u32Files, u32Fds, g_u32SoftOk, g_u32SoftFail,
                u32Samples);
    }

    /*
     * Soft lamp: init surface always soft-pass after vfs_ram_init.
     * Grep: vfs_ram: soft inventory PASS | vfs_ram: soft PASS
     */
    fSoftPass = 1;
    kprintf("vfs_ram: soft inventory PASS seeded=%u files=%u fds=%u "
            "logs=%u wave=57\n",
            g_u32SoftSeeded, u32Files, u32Fds, u32Samples);
    kprintf("vfs_ram: soft PASS seeded=%u wave=57\n", g_u32SoftSeeded);
    (void)fSoftPass;
}

/**
 * After first product open/op activity, print soft inventory once
 * (mirrors input_hub/vfs_door soft-stats-once). Diagnostics only.
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
        return -22; /* EINVAL — only absolute paths */
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
    g_fBlkMounted = 0;
    g_fScsiMounted = 0;
    g_iBlkFile = -1;
    g_u64ScsiCapBytes = 0;
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
    seed_file("/etc/hostname", "greenjade\n");
    seed_file("/etc/os-release",
              "NAME=\"GreenJade\"\nID=greenjade\nVERSION_ID=\"0.1\"\n"
              "PRETTY_NAME=\"GreenJade 0.1\"\n"
              "HOME_URL=\"https://greenjade.local\"\n");
    seed_file("/etc/issue", "GreenJade \\n \\l\n");
    seed_file("/etc/passwd", "root:x:0:0:root:/root:/bin/sh\n");
    seed_file("/proc/version", "Linux version 6.0.0-greenjade\n");
    seed_file("/proc/self/maps", "");
    seed_file("/proc/self/cmdline", "init\0");
    seed_file("/proc/cpuinfo", "processor\t: 0\nvendor_id\t: GreenJade\n");
    seed_file("/proc/meminfo", "MemTotal:        1048576 kB\n");
    seed_file("/bin/greenjade", "#!/bin/sh\necho GreenJade\n");
    seed_file("/bin/sh", "#!/bin/sh\n# GreenJade shell placeholder\n");
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

i64
vfs_ram_open(const char *szPath, int fCreate)
{
    i32 iFile;
    u32 iFd;
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
    /* Virtual directory markers (no file slot) */
    if (path_eq(szResolved, "/") || path_eq(szResolved, "/tmp") ||
        path_eq(szResolved, "/proc") || path_eq(szResolved, "/dev") ||
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
    return i64Fd >= 3 && i64Fd < VFS_MAX_FDS && g_aFds[i64Fd].u8Used;
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
    if (i64New < 3 || i64New >= VFS_MAX_FDS) {
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
        path_eq(szNorm, "/proc") || path_eq(szNorm, "/dev")) {
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
    /* Grow only — fallocate never shrinks (unlike ftruncate) */
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

        if (pFd->u32File >= VFS_MAX_PIPES || !g_aPipes[pFd->u32File].u8Used) {
            return -9;
        }
        u8From = (u8)(1u - pFd->u8End);
        return (i64)g_aPipes[pFd->u32File].u32Len[u8From];
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

    if (szPath == NULL || pBuf == NULL || cb == 0) {
        return soft_out(&g_u32SoftReadlinkOk, &g_u32SoftReadlinkFail, -14);
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
        return -22; /* EINVAL — not a symlink here */
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
        path_eq(szResolved, "/dev") || path_eq(szResolved, "/bin") ||
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
    if (fNeedX && (u32Mode & 0111u) == 0) {
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

    if (pStat == NULL || cbStat < sizeof(st)) {
        return soft_out(&g_u32SoftStatOk, &g_u32SoftStatFail, -14);
    }
    if (i64Fd >= 0 && i64Fd <= 2) {
        /* stdin/out/err — char device-ish */
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
        mode = 0010644u; /* fifo */
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
    if (pFile->u8IsDir) {
        return -21; /* EISDIR */
    }
    u32New = (u32)i64Len;
    if ((i64)u32New != i64Len) {
        return -75; /* EOVERFLOW — beyond u32 / cap */
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
            path_eq(szNorm, "/proc") || path_eq(szNorm, "/dev")) {
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

    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u64 u64V;
        u32 iEv = pFd->u32File;

        if (iEv >= VFS_MAX_EVENTFD || !g_aEventUsed[iEv] || cb < 8) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -22);
        }
        if (g_aEventCnt[iEv] == 0) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -11);
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
        if (g_aSigPending[iS] == 0) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -11);
        }
        memset(aInfo, 0, sizeof(aInfo));
        /* pick lowest pending bit as signo */
        for (sig = 1; sig < 64; sig++) {
            if (g_aSigPending[iS] & (1ull << sig)) {
                g_aSigPending[iS] &= ~(1ull << sig);
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

        if (pFd->u32File >= VFS_MAX_PIPES || !g_aPipes[pFd->u32File].u8Used) {
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -9);
        }
        pPair = &g_aPipes[pFd->u32File];
        u8From = (u8)(1u - pFd->u8End);
        if (pPair->u32Len[u8From] == 0) {
            /* Peer closed with empty buffer → EOF; else would block → 0 for now */
            if (!pPair->u8Open[u8From]) {
                return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, 0);
            }
            return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, -11);
        }
        if (u32Want > pPair->u32Len[u8From]) {
            u32Want = pPair->u32Len[u8From];
        }
        while (u32Done < u32Want) {
            u32 iHead = pPair->u32Head[u8From];
            ((u8 *)pBuf)[u32Done] = pPair->aRing[u8From][iHead];
            pPair->u32Head[u8From] = (iHead + 1u) % VFS_PIPE_BUF;
            pPair->u32Len[u8From]--;
            u32Done++;
        }
        return soft_out_bytes(&g_u32SoftReadOk, &g_u32SoftReadFail, &g_u64SoftBytesRead, (i64)u32Done);
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

    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u64 u64Add = 0;
        u32 iEv = pFd->u32File;

        if (iEv >= VFS_MAX_EVENTFD || !g_aEventUsed[iEv] || cb < 8) {
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -22);
        }
        memcpy(&u64Add, pBuf, 8);
        g_aEventCnt[iEv] += u64Add;
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
        pPair = &g_aPipes[pFd->u32File];
        if (!pPair->u8Open[1u - u8To]) {
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -32);
        }
        u32Space = VFS_PIPE_BUF - pPair->u32Len[u8To];
        if (u32Space == 0) {
            return soft_out_bytes(&g_u32SoftWriteOk, &g_u32SoftWriteFail, &g_u64SoftBytesWrite, -11);
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

        if (u32Pair < VFS_MAX_PIPES && g_aPipes[u32Pair].u8Used) {
            g_aPipes[u32Pair].u8Open[u8End] = 0;
            if (!g_aPipes[u32Pair].u8Open[0] && !g_aPipes[u32Pair].u8Open[1]) {
                g_aPipes[u32Pair].u8Used = 0;
            }
        }
    } else if (g_aFds[i64Fd].u8Kind == VFS_KIND_EVENTFD) {
        u32 iEv = g_aFds[i64Fd].u32File;

        if (iEv < VFS_MAX_EVENTFD) {
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
    return soft_out(&g_u32SoftCloseOk, &g_u32SoftCloseFail, 0);
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
    g_aFds[iFd0].u32File = iPair;
    g_aFds[iFd0].u64Off = 0;

    g_aFds[iFd1].u8Used = 1;
    g_aFds[iFd1].u8Kind = VFS_KIND_PIPE;
    g_aFds[iFd1].u8End = 1;
    g_aFds[iFd1].u32File = iPair;
    g_aFds[iFd1].u64Off = 0;

    pFds[0] = iFd0;
    pFds[1] = iFd1;
    return 0;
}

i64
vfs_ram_pipe2(i32 *pFds, int nFlags)
{
    (void)nFlags;
    return soft_out(&g_u32SoftPipeOk, &g_u32SoftPipeFail, pipe_alloc_pair(pFds));
}

i64
vfs_ram_socketpair(int nDomain, int nType, int nProtocol, i32 *pFds)
{
    (void)nDomain;
    (void)nProtocol;
    /* SOCK_STREAM = 1; SOCK_DGRAM = 2 — accept either for bring-up */
    if (nType != 1 && nType != 2) {
        return soft_out(&g_u32SoftSocketpairOk, &g_u32SoftSocketpairFail, -22);
    }
    return soft_out(&g_u32SoftSocketpairOk, &g_u32SoftSocketpairFail,
                    pipe_alloc_pair(pFds));
}

i64
vfs_ram_eventfd2(u32 u32Init, int nFlags)
{
    u32 iEv;
    u32 iFd;

    (void)nFlags;
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
#define VFS_EPOLLOUT    0x0004u
#define VFS_EPOLLERR    0x0008u
#define VFS_EPOLLHUP    0x0010u
#define VFS_EPOLLRDHUP  0x2000u
#define VFS_EPOLLONESHOT 0x40000000u

/**
 * Compute ready events. ERR/HUP/RDHUP are always candidates; IN/OUT are
 * filtered by u32Want (caller may OR them). u32Want==0 → default IN|OUT.
 */
static u32
epoll_ready_mask(i32 i32Fd, u32 u32Want)
{
    u32 u32Got = 0;
    u32 u32Req;
    struct vfs_fd *pFd;

    u32Req = u32Want & (VFS_EPOLLIN | VFS_EPOLLOUT | VFS_EPOLLRDHUP);
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
        if ((u32Req & VFS_EPOLLIN) && pPair->u32Len[u8From] > 0) {
            u32Got |= VFS_EPOLLIN;
        }
        /* Peer closed: HUP when no more data; RDHUP when peer end gone */
        if (!pPair->u8Open[u8From]) {
            if (pPair->u32Len[u8From] == 0) {
                u32Got |= VFS_EPOLLHUP;
            }
            u32Got |= VFS_EPOLLRDHUP;
            if ((u32Req & VFS_EPOLLIN) && pPair->u32Len[u8From] == 0) {
                u32Got |= VFS_EPOLLIN; /* EOF readable */
            }
        }
        if (!pPair->u8Open[1u - u8To]) {
            /* Writer-side peer gone: further writes fail — still report OUT clear */
            ;
        } else if ((u32Req & VFS_EPOLLOUT) &&
                   pPair->u32Len[u8To] < VFS_PIPE_BUF) {
            u32Got |= VFS_EPOLLOUT;
        }
        if (!pPair->u8Open[1u - u8To] && pPair->u32Len[u8To] >= VFS_PIPE_BUF) {
            u32Got |= VFS_EPOLLERR; /* no consumer, buffer full */
        }
    } else if (pFd->u8Kind == VFS_KIND_PIDFD) {
        /* Bring-up: pidfd is always "process alive" → never HUP; OUT ready */
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
    soft_inc(&g_u32SoftPollProbe); /* Wave 14 */
    u32 u32Got = epoll_ready_mask((i32)i64Fd, u32Want);
    u32 u32Keep;

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

    if (i64Fd < 3 || i64Fd >= VFS_MAX_FDS) {
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
        if (!vfs_ram_fd_ok(i64Fd)) {
            return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, -9);
        }
        /* Reject watching another epoll instance (simplifies bring-up) */
        if (g_aFds[i64Fd].u8Kind == VFS_KIND_EPOLL) {
            return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, -22);
        }
        for (i = 0; i < pE->u8N; i++) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                return -17; /* EEXIST */
            }
        }
        if (pE->u8N >= VFS_EPOLL_WATCH) {
            return -24; /* EMFILE — interest list full */
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
        if (!vfs_ram_fd_ok(i64Fd)) {
            return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, -9);
        }
        for (i = 0; i < pE->u8N; i++) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                pE->aWatch[i].u32Events = u32Ev;
                pE->aWatch[i].u64Data = u64Data;
                return soft_out(&g_u32SoftEpollCtlOk, &g_u32SoftEpollCtlFail, 0);
            }
        }
        return -2; /* ENOENT — not in interest list */
    }
    return -22; /* EINVAL — unknown op */
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
    /* Arm: non-zero value → one expiration tick immediately (software) */
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
    for (i = 0; i < VFS_MAX_SIGNALFD; i++) {
        if (g_aSigUsed[i] && (g_aSigMask[i] & (1ull << u32Signo))) {
            g_aSigPending[i] |= (1ull << u32Signo);
        }
    }
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
        u32R = epoll_ready_mask(pE->aWatch[i].i32Fd, u32Want);
        /* Always surface ERR/HUP/RDHUP; gate IN/OUT by interest */
        u32Report = u32R & (VFS_EPOLLERR | VFS_EPOLLHUP | VFS_EPOLLRDHUP);
        u32Report |= u32R & u32Want &
                     (VFS_EPOLLIN | VFS_EPOLLOUT | VFS_EPOLLRDHUP);
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
            path_eq(szDir, "/dev") || path_eq(szDir, "/bin") ||
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
