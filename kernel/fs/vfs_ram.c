/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal ramdisk: fixed files + open/read/write for cold Linux path.
 * Optional /dev/vda (virtio-blk) and /dev/sda (virtio-scsi) when mounted.
 * Also hosts pipes, eventfd, epoll, timerfd, signalfd, pidfd, inotify for
 * the Linux ABI bring-up path — independent of vfs_door product mini-FS.
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

static void
seed_file(const char *szPath, const char *szData)
{
    u32 i;
    size_t cb;

    if (szPath == NULL || szPath[0] == '\0') {
        return;
    }
    for (i = 0; i < VFS_MAX_FILES; i++) {
        if (!g_aFiles[i].u8Used) {
            g_aFiles[i].u8Used = 1;
            g_aFiles[i].u8Kind = VFS_KIND_RAM;
            path_copy(g_aFiles[i].szPath, szPath);
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
    g_fBlkMounted = 0;
    g_fScsiMounted = 0;
    g_iBlkFile = -1;
    g_u64ScsiCapBytes = 0;
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
    kprintf("vfs_ram: init seeded=%u max_files=%u max_fds=%u "
            "stubs=/etc,/proc,/bin,/lib,/tmp\n",
            cSeed, VFS_MAX_FILES, VFS_MAX_FDS);
}

void
vfs_ram_mount_blk(void)
{
    u32 i;

    if (g_fBlkMounted) {
        return;
    }
    if (!virtio_blk_ready()) {
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
            kprintf("vfs_ram: mounted /dev/vda capacity=%lu sectors\n",
                    (unsigned long)virtio_blk_capacity_sectors());
            return;
        }
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
        kprintf("vfs_ram: mount_scsi skipped (no virtio-scsi)\n");
        return;
    }
    memset(&req, 0, sizeof(req));
    scsi_cdb_read_capacity10(&req.cdb);
    req.pData = aCap;
    req.cbData = 8;
    req.fDataIn = 1;
    if (scsi_mid_submit(&req) != 0) {
        kprintf("vfs_ram: mount_scsi capacity failed\n");
        return;
    }
    {
        u32 u32Last = ((u32)aCap[0] << 24) | ((u32)aCap[1] << 16) |
                      ((u32)aCap[2] << 8) | (u32)aCap[3];
        u32 u32Bsz = ((u32)aCap[4] << 24) | ((u32)aCap[5] << 16) |
                     ((u32)aCap[6] << 8) | (u32)aCap[7];

        if (u32Bsz != VFS_SCSI_SEC) {
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
            kprintf("vfs_ram: mounted /dev/sda capacity=%lu bytes\n",
                    (unsigned long)g_u64ScsiCapBytes);
            return;
        }
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

    if (szPath == NULL || szPath[0] == '\0') {
        return -14; /* EFAULT */
    }
    iFile = find_file(szPath);
    if (iFile < 0) {
        if (!fCreate) {
            return -2; /* ENOENT */
        }
        for (iFile = 0; (u32)iFile < VFS_MAX_FILES; iFile++) {
            if (!g_aFiles[iFile].u8Used) {
                g_aFiles[iFile].u8Used = 1;
                g_aFiles[iFile].u8Kind = VFS_KIND_RAM;
                g_aFiles[iFile].u8IsDir = 0;
                g_aFiles[iFile].u32Mode = 0100644u;
                g_aFiles[iFile].u32Nlink = 1;
                path_copy(g_aFiles[iFile].szPath, szPath);
                g_aFiles[iFile].cbData = 0;
                break;
            }
        }
        if ((u32)iFile >= VFS_MAX_FILES) {
            return -28; /* ENOSPC */
        }
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = g_aFiles[iFile].u8Kind;
            g_aFds[iFd].u32File = (u32)iFile;
            g_aFds[iFd].u64Off = 0;
            return (i64)iFd;
        }
    }
    return -24; /* EMFILE */
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
        return -9; /* EBADF */
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd] = g_aFds[i64Fd];
            g_aFds[iFd].u8Used = 1;
            return (i64)iFd;
        }
    }
    return -24; /* EMFILE */
}

i64
vfs_ram_dup2(i64 i64Old, i64 i64New)
{
    if (!vfs_ram_fd_ok(i64Old)) {
        return -9;
    }
    if (i64New < 3 || i64New >= VFS_MAX_FDS) {
        return -9;
    }
    if (i64Old == i64New) {
        return i64New;
    }
    if (g_aFds[i64New].u8Used) {
        (void)vfs_ram_close(i64New);
    }
    g_aFds[i64New] = g_aFds[i64Old];
    g_aFds[i64New].u8Used = 1;
    return i64New;
}

i64
vfs_ram_dup_from(i64 i64Fd, i64 i64Min)
{
    u32 iFd;
    u32 u32Start;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    if (i64Min < 3) {
        i64Min = 3;
    }
    if (i64Min >= VFS_MAX_FDS) {
        return -9;
    }
    u32Start = (u32)i64Min;
    for (iFd = u32Start; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd] = g_aFds[i64Fd];
            g_aFds[iFd].u8Used = 1;
            return (i64)iFd;
        }
    }
    return -24;
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

    if (szTarget == NULL || szLink == NULL || szLink[0] == '\0' ||
        szTarget[0] == '\0') {
        return -14;
    }
    for (i = 0; i < VFS_MAX_SYMLINKS; i++) {
        if (g_aSym[i].u8Used && path_eq(g_aSym[i].szLink, szLink)) {
            path_copy(g_aSym[i].szTarget, szTarget);
            return 0;
        }
    }
    for (i = 0; i < VFS_MAX_SYMLINKS; i++) {
        if (!g_aSym[i].u8Used) {
            g_aSym[i].u8Used = 1;
            path_copy(g_aSym[i].szLink, szLink);
            path_copy(g_aSym[i].szTarget, szTarget);
            return 0;
        }
    }
    return -28; /* ENOSPC */
}

i64
vfs_ram_utimens(const char *szPath)
{
    i32 iFile;

    if (szPath == NULL || szPath[0] == '\0') {
        return -14;
    }
    if (path_eq(szPath, "/") || path_eq(szPath, "/tmp") ||
        path_eq(szPath, "/proc/self/exe")) {
        return 0;
    }
    iFile = find_file(szPath);
    if (iFile < 0) {
        /* Create empty file as touch */
        i64 fd = vfs_ram_open(szPath, 1);

        if (fd < 0) {
            return fd;
        }
        (void)vfs_ram_close(fd);
        return 0;
    }
    (void)iFile;
    return 0;
}

i64
vfs_ram_link(const char *szOld, const char *szNew)
{
    i32 iOld;
    i32 iNew;
    u32 i;

    if (szOld == NULL || szNew == NULL || szOld[0] == '\0' || szNew[0] == '\0') {
        return -14;
    }
    iOld = find_file(szOld);
    if (iOld < 0) {
        return -2;
    }
    if (g_aFiles[iOld].u8Kind != VFS_KIND_RAM) {
        return -1;
    }
    if (find_file(szNew) >= 0) {
        return -17; /* EEXIST */
    }
    /* New path entry that aliases same content (copy for bring-up) */
    for (iNew = 0; (u32)iNew < VFS_MAX_FILES; iNew++) {
        if (!g_aFiles[iNew].u8Used) {
            g_aFiles[iNew] = g_aFiles[iOld];
            g_aFiles[iNew].u8Used = 1;
            path_copy(g_aFiles[iNew].szPath, szNew);
            g_aFiles[iOld].u32Nlink++;
            g_aFiles[iNew].u32Nlink = g_aFiles[iOld].u32Nlink;
            /* Deep-copy data buffer already done by struct assign */
            for (i = 0; i < g_aFiles[iOld].cbData && i < VFS_MAX_DATA; i++) {
                g_aFiles[iNew].aData[i] = g_aFiles[iOld].aData[i];
            }
            return 0;
        }
    }
    return -28;
}

i64
vfs_ram_unlink(const char *szPath)
{
    i32 iFile;
    u32 s;

    if (szPath == NULL || szPath[0] == '\0') {
        return -14;
    }
    /* Symlinks first */
    for (s = 0; s < VFS_MAX_SYMLINKS; s++) {
        if (g_aSym[s].u8Used && path_eq(g_aSym[s].szLink, szPath)) {
            memset(&g_aSym[s], 0, sizeof(g_aSym[s]));
            return 0;
        }
    }
    iFile = find_file(szPath);
    if (iFile < 0) {
        return -2;
    }
    if (g_aFiles[iFile].u8Kind != VFS_KIND_RAM) {
        return -1; /* device nodes not removable here */
    }
    if (g_aFiles[iFile].u8IsDir) {
        return -21; /* EISDIR */
    }
    if (g_aFiles[iFile].u32Nlink > 1) {
        g_aFiles[iFile].u32Nlink--;
    }
    memset(&g_aFiles[iFile], 0, sizeof(g_aFiles[iFile]));
    return 0;
}

i64
vfs_ram_rmdir(const char *szPath)
{
    i32 iFile;

    if (szPath == NULL || szPath[0] == '\0') {
        return -14;
    }
    if (path_eq(szPath, "/") || path_eq(szPath, "/tmp")) {
        return -16; /* EBUSY */
    }
    iFile = find_file(szPath);
    if (iFile < 0) {
        return -2;
    }
    if (!g_aFiles[iFile].u8IsDir && g_aFiles[iFile].cbData != 0) {
        return -20; /* ENOTDIR / not empty-ish */
    }
    /* Treat empty RAM marker as rmdir-able (mkdir creates empty file marker) */
    if (g_aFiles[iFile].u8Kind != VFS_KIND_RAM) {
        return -20;
    }
    if (g_aFiles[iFile].cbData != 0) {
        return -39; /* ENOTEMPTY */
    }
    memset(&g_aFiles[iFile], 0, sizeof(g_aFiles[iFile]));
    return 0;
}

i64
vfs_ram_fchmod(i64 i64Fd, u32 u32Mode)
{
    struct vfs_fd *pFd;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES) {
        return -22;
    }
    /* Keep type bits; replace permission nibble */
    g_aFiles[pFd->u32File].u32Mode =
        (g_aFiles[pFd->u32File].u32Mode & ~07777u) | (u32Mode & 07777u);
    if ((u32Mode & 0040000u) != 0) {
        g_aFiles[pFd->u32File].u8IsDir = 1;
    }
    return 0;
}

i64
vfs_ram_mark_dir(i64 i64Fd)
{
    struct vfs_fd *pFd;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES) {
        return -22;
    }
    g_aFiles[pFd->u32File].u8IsDir = 1;
    g_aFiles[pFd->u32File].u32Mode = 0040755u;
    g_aFiles[pFd->u32File].cbData = 0;
    return 0;
}

i64
vfs_ram_fallocate(i64 i64Fd, i64 i64Off, i64 i64Len)
{
    i64 need;

    if (!vfs_ram_fd_ok(i64Fd) || i64Off < 0 || i64Len < 0) {
        return -22;
    }
    need = i64Off + i64Len;
    if (need < i64Off) {
        return -22; /* overflow */
    }
    return vfs_ram_ftruncate(i64Fd, need);
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
        return -9;
    }
    off = pOff ? *pOff : 0;
    while (done < cb) {
        size_t chunk = cb - done;

        if (chunk > sizeof(aBuf)) {
            chunk = sizeof(aBuf);
        }
        n = vfs_ram_pread(i64In, aBuf, chunk, off);
        if (n < 0) {
            return done ? (i64)done : n;
        }
        if (n == 0) {
            break;
        }
        w = vfs_ram_write(i64Out, aBuf, (size_t)n);
        if (w < 0) {
            return done ? (i64)done : w;
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
    return (i64)done;
}

i64
vfs_ram_readlink(const char *szPath, char *pBuf, size_t cb)
{
    static const char szExe[] = "/bin/greenjade";
    size_t n;
    size_t i;
    u32 s;

    if (szPath == NULL || pBuf == NULL || cb == 0) {
        return -14;
    }
    /* Dynamic symlink table first */
    for (s = 0; s < VFS_MAX_SYMLINKS; s++) {
        if (g_aSym[s].u8Used && path_eq(g_aSym[s].szLink, szPath)) {
            n = 0;
            while (g_aSym[s].szTarget[n] != '\0') {
                n++;
            }
            if (cb < n) {
                return -34;
            }
            for (i = 0; i < n; i++) {
                pBuf[i] = g_aSym[s].szTarget[i];
            }
            return (i64)n;
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
        return 1;
    }
    n = 0;
    while (szExe[n] != '\0') {
        n++;
    }
    if (cb < n) {
        return -34;
    }
    for (i = 0; i < n; i++) {
        pBuf[i] = szExe[i];
    }
    return (i64)n;
}

i64
vfs_ram_access(const char *szPath, int nMode)
{
    (void)nMode;
    if (szPath == NULL || szPath[0] == '\0') {
        return -14;
    }
    /* Known virtual paths */
    if (path_eq(szPath, "/proc/self/exe") || path_eq(szPath, "/proc/self/cwd") ||
        path_eq(szPath, "/") || path_eq(szPath, "/tmp") ||
        path_eq(szPath, "/bin/greenjade")) {
        return 0;
    }
    if (find_file(szPath) >= 0) {
        return 0;
    }
    return -2; /* ENOENT */
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
        return -14;
    }
    if (i64Fd >= 0 && i64Fd <= 2) {
        /* stdin/out/err — char device-ish */
        fill_stat(&st, (u64)i64Fd + 1u, 0020000u | 0666u, 0);
        memcpy(pStat, &st, sizeof(st));
        return 0;
    }
    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    mode = 0100644u; /* regular */
    size = 0;
    if (pFd->u8Kind == VFS_KIND_RAM && pFd->u32File < VFS_MAX_FILES) {
        size = (i64)g_aFiles[pFd->u32File].cbData;
        if (g_aFiles[pFd->u32File].u32Mode != 0) {
            mode = g_aFiles[pFd->u32File].u32Mode;
        }
    } else if (pFd->u8Kind == VFS_KIND_PIPE) {
        mode = 0010644u; /* fifo */
    } else if (pFd->u8Kind == VFS_KIND_BLK || pFd->u8Kind == VFS_KIND_SCSI) {
        mode = 0060644u; /* block */
        size = (i64)(g_u64ScsiCapBytes ? g_u64ScsiCapBytes : 0);
    } else if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        mode = 0020644u;
    }
    fill_stat(&st, (u64)i64Fd + 100u, mode, size);
    if (pFd->u8Kind == VFS_KIND_RAM && pFd->u32File < VFS_MAX_FILES &&
        g_aFiles[pFd->u32File].u32Nlink > 0) {
        st.st_nlink = g_aFiles[pFd->u32File].u32Nlink;
    }
    memcpy(pStat, &st, sizeof(st));
    return 0;
}

i64
vfs_ram_rename(const char *szOld, const char *szNew)
{
    i32 iFile;
    i32 iExist;

    if (szOld == NULL || szNew == NULL || szOld[0] == '\0' ||
        szNew[0] == '\0') {
        return -14;
    }
    iFile = find_file(szOld);
    if (iFile < 0) {
        return -2;
    }
    if (g_aFiles[iFile].u8Kind != VFS_KIND_RAM) {
        return -1; /* EPERM-shaped for device nodes */
    }
    iExist = find_file(szNew);
    if (iExist >= 0 && iExist != iFile) {
        /* Replace target (unlink first) */
        g_aFiles[iExist].u8Used = 0;
        g_aFiles[iExist].cbData = 0;
    }
    path_copy(g_aFiles[iFile].szPath, szNew);
    return 0;
}

i64
vfs_ram_ftruncate(i64 i64Fd, i64 i64Len)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    u32 u32New;
    u32 i;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    if (i64Len < 0) {
        return -22;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_RAM || pFd->u32File >= VFS_MAX_FILES) {
        return -22;
    }
    pFile = &g_aFiles[pFd->u32File];
    if (!pFile->u8Used) {
        return -9;
    }
    u32New = (u32)i64Len;
    if (u32New > VFS_MAX_DATA) {
        u32New = VFS_MAX_DATA;
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
    return 0;
}

i64
vfs_ram_stat(const char *szPath, void *pStat, size_t cbStat)
{
    i32 iFile;
    struct vfs_stat64 st;
    u32 mode = 0100644u;
    i64 size = 0;

    if (szPath == NULL || pStat == NULL || cbStat < sizeof(st)) {
        return -14;
    }
    if (path_eq(szPath, "/") || path_eq(szPath, "/tmp")) {
        fill_stat(&st, 2, 0040755u, 0); /* directory */
        memcpy(pStat, &st, sizeof(st));
        return 0;
    }
    if (path_eq(szPath, "/proc/self/exe") || path_eq(szPath, "/bin/greenjade")) {
        fill_stat(&st, 3, 0100755u, 4096);
        memcpy(pStat, &st, sizeof(st));
        return 0;
    }
    iFile = find_file(szPath);
    if (iFile < 0) {
        return -2;
    }
    if (g_aFiles[iFile].u8Kind == VFS_KIND_RAM) {
        size = (i64)g_aFiles[iFile].cbData;
    } else if (g_aFiles[iFile].u8Kind == VFS_KIND_BLK ||
               g_aFiles[iFile].u8Kind == VFS_KIND_SCSI) {
        mode = 0060644u;
    }
    fill_stat(&st, (u64)iFile + 200u, mode, size);
    memcpy(pStat, &st, sizeof(st));
    return 0;
}

i64
vfs_ram_read(i64 i64Fd, void *pBuf, size_t cb)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    u32 u32Avail;
    u32 u32N;
    u32 i;

    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL) {
        return -9; /* EBADF / EFAULT */
    }
    pFd = &g_aFds[i64Fd];
    pFile = &g_aFiles[pFd->u32File];

    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u64 u64V;
        u32 iEv = pFd->u32File;

        if (iEv >= VFS_MAX_EVENTFD || !g_aEventUsed[iEv] || cb < 8) {
            return -22;
        }
        if (g_aEventCnt[iEv] == 0) {
            return -11; /* EAGAIN */
        }
        u64V = g_aEventCnt[iEv];
        g_aEventCnt[iEv] = 0;
        memcpy(pBuf, &u64V, 8);
        return 8;
    }

    if (pFd->u8Kind == VFS_KIND_TIMERFD) {
        u64 u64V;
        u32 iT = pFd->u32File;

        if (iT >= VFS_MAX_TIMERFD || !g_aTimerUsed[iT] || cb < 8) {
            return -22;
        }
        if (g_aTimerTicks[iT] == 0) {
            return -11; /* EAGAIN */
        }
        u64V = g_aTimerTicks[iT];
        g_aTimerTicks[iT] = 0;
        memcpy(pBuf, &u64V, 8);
        return 8;
    }

    if (pFd->u8Kind == VFS_KIND_SIGNALFD) {
        /* signalfd_siginfo is 128 bytes; return one synthetic info */
        u32 iS = pFd->u32File;
        u8 aInfo[128];
        u32 sig;

        if (iS >= VFS_MAX_SIGNALFD || !g_aSigUsed[iS] || cb < 128) {
            return -22;
        }
        if (g_aSigPending[iS] == 0) {
            return -11;
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
        return (i64)cb;
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
            return -9;
        }
        if (cb < 16) {
            return -22;
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
            return -11; /* EAGAIN */
        }
        return (i64)nOut;
    }

    if (pFd->u8Kind == VFS_KIND_PIPE) {
        struct vfs_pipe_pair *pPair;
        u8 u8From; /* ring written by peer */
        u32 u32Want = (u32)cb;
        u32 u32Done = 0;

        if (pFd->u32File >= VFS_MAX_PIPES || !g_aPipes[pFd->u32File].u8Used) {
            return -9;
        }
        pPair = &g_aPipes[pFd->u32File];
        u8From = (u8)(1u - pFd->u8End);
        if (pPair->u32Len[u8From] == 0) {
            /* Peer closed with empty buffer → EOF; else would block → 0 for now */
            if (!pPair->u8Open[u8From]) {
                return 0;
            }
            return -11; /* EAGAIN */
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
        return (i64)u32Done;
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
                return -5;
            }
            u64CapBytes =
                virtio_blk_capacity_sectors() * (u64)GJ_VIRTIO_BLK_SECTOR;
        } else {
            if (!virtio_scsi_ready()) {
                return -5;
            }
            u64CapBytes = g_u64ScsiCapBytes;
        }
        if (pFd->u64Off >= u64CapBytes) {
            return 0;
        }
        if ((u64)u32Want > u64CapBytes - pFd->u64Off) {
            u32Want = (u32)(u64CapBytes - pFd->u64Off);
        }
        while (u32Done < u32Want) {
            u64Sector = pFd->u64Off / u32SecSz;
            u32SecOff = (u32)(pFd->u64Off % u32SecSz);
            if (pFd->u8Kind == VFS_KIND_BLK) {
                if (virtio_blk_read(u64Sector, aSec, u32SecSz) != 0) {
                    return u32Done > 0 ? (i64)u32Done : -5;
                }
            } else {
                struct gj_scsi_request req;

                memset(&req, 0, sizeof(req));
                scsi_cdb_read10(&req.cdb, (u32)u64Sector, 1);
                req.pData = aSec;
                req.cbData = u32SecSz;
                req.fDataIn = 1;
                if (scsi_mid_submit(&req) != 0) {
                    return u32Done > 0 ? (i64)u32Done : -5;
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
        return (i64)u32Done;
    }

    if (pFd->u64Off >= pFile->cbData) {
        return 0;
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
    return (i64)u32N;
}

i64
vfs_ram_write(i64 i64Fd, const void *pBuf, size_t cb)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    u32 u32N;
    u32 i;

    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    pFile = &g_aFiles[pFd->u32File];

    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u64 u64Add = 0;
        u32 iEv = pFd->u32File;

        if (iEv >= VFS_MAX_EVENTFD || !g_aEventUsed[iEv] || cb < 8) {
            return -22;
        }
        memcpy(&u64Add, pBuf, 8);
        g_aEventCnt[iEv] += u64Add;
        return 8;
    }

    if (pFd->u8Kind == VFS_KIND_PIPE) {
        struct vfs_pipe_pair *pPair;
        u8 u8To = pFd->u8End;
        u32 u32Want = (u32)cb;
        u32 u32Done = 0;
        u32 u32Space;

        if (pFd->u32File >= VFS_MAX_PIPES || !g_aPipes[pFd->u32File].u8Used) {
            return -9;
        }
        pPair = &g_aPipes[pFd->u32File];
        if (!pPair->u8Open[1u - u8To]) {
            return -32; /* EPIPE */
        }
        u32Space = VFS_PIPE_BUF - pPair->u32Len[u8To];
        if (u32Space == 0) {
            return -11; /* EAGAIN */
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
        return (i64)u32Done;
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
            return -5;
        }
        u64CapBytes = virtio_blk_capacity_sectors() * (u64)GJ_VIRTIO_BLK_SECTOR;
        if (pFd->u64Off >= u64CapBytes) {
            return -28;
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
                    return u32Done > 0 ? (i64)u32Done : -5;
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
                return u32Done > 0 ? (i64)u32Done : -5;
            }
            pFd->u64Off += u32Copy;
            u32Done += u32Copy;
        }
        return (i64)u32Done;
    }

    u32N = (u32)cb;
    if ((u32)pFd->u64Off + u32N > VFS_MAX_DATA) {
        if ((u32)pFd->u64Off >= VFS_MAX_DATA) {
            return -28;
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
    return (i64)u32N;
}

i64
vfs_ram_close(i64 i64Fd)
{
    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
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
    return 0;
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
    return pipe_alloc_pair(pFds);
}

i64
vfs_ram_socketpair(int nDomain, int nType, int nProtocol, i32 *pFds)
{
    (void)nDomain;
    (void)nProtocol;
    /* SOCK_STREAM = 1; SOCK_DGRAM = 2 — accept either for bring-up */
    if (nType != 1 && nType != 2) {
        return -22;
    }
    return pipe_alloc_pair(pFds);
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
        return -24;
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
            return (i64)iFd;
        }
    }
    return -24;
}

/* ---- epoll (interest list + ready probe for pipes/eventfd/ram) ----------- */

static u32
epoll_ready_mask(i32 i32Fd, u32 u32Want)
{
    u32 u32Got = 0;
    struct vfs_fd *pFd;

    if (i32Fd < 0 || !vfs_ram_fd_ok((i64)i32Fd)) {
        return 0x008u; /* EPOLLERR for dead fd */
    }
    pFd = &g_aFds[i32Fd];
    if (pFd->u8Kind == VFS_KIND_EVENTFD) {
        u32 iEv = pFd->u32File;

        if (iEv < VFS_MAX_EVENTFD && g_aEventUsed[iEv] &&
            g_aEventCnt[iEv] != 0 && (u32Want & 0x001u)) {
            u32Got |= 0x001u; /* EPOLLIN */
        }
        if (u32Want & 0x004u) {
            u32Got |= 0x004u; /* always writable */
        }
    } else if (pFd->u8Kind == VFS_KIND_TIMERFD) {
        u32 iT = pFd->u32File;

        if (iT < VFS_MAX_TIMERFD && g_aTimerUsed[iT] &&
            g_aTimerTicks[iT] != 0 && (u32Want & 0x001u)) {
            u32Got |= 0x001u;
        }
    } else if (pFd->u8Kind == VFS_KIND_SIGNALFD) {
        u32 iS = pFd->u32File;

        if (iS < VFS_MAX_SIGNALFD && g_aSigUsed[iS] &&
            g_aSigPending[iS] != 0 && (u32Want & 0x001u)) {
            u32Got |= 0x001u;
        }
    } else if (pFd->u8Kind == VFS_KIND_INOTIFY) {
        u32 iIn = pFd->u32File;

        if (iIn < VFS_MAX_INOTIFY && g_aInotify[iIn].u8Used &&
            g_aInotify[iIn].u8Nq > 0 && (u32Want & 0x001u)) {
            u32Got |= 0x001u;
        }
    } else if (pFd->u8Kind == VFS_KIND_PIPE) {
        struct vfs_pipe_pair *pPair;
        u8 u8From;
        u8 u8To;

        if (pFd->u32File >= VFS_MAX_PIPES || !g_aPipes[pFd->u32File].u8Used) {
            return 0x008u;
        }
        pPair = &g_aPipes[pFd->u32File];
        u8From = (u8)(1u - pFd->u8End);
        u8To = pFd->u8End;
        if ((u32Want & 0x001u) && pPair->u32Len[u8From] > 0) {
            u32Got |= 0x001u;
        }
        if ((u32Want & 0x001u) && !pPair->u8Open[u8From] &&
            pPair->u32Len[u8From] == 0) {
            u32Got |= 0x010u; /* EPOLLHUP */
        }
        if ((u32Want & 0x004u) &&
            pPair->u32Len[u8To] < VFS_PIPE_BUF && pPair->u8Open[1u - u8To]) {
            u32Got |= 0x004u;
        }
    } else {
        /* RAM/block/scsi/epoll: treat as always writable; readable if non-empty */
        if (u32Want & 0x004u) {
            u32Got |= 0x004u;
        }
        if ((u32Want & 0x001u) && pFd->u8Kind == VFS_KIND_RAM &&
            pFd->u32File < VFS_MAX_FILES &&
            g_aFiles[pFd->u32File].cbData > pFd->u64Off) {
            u32Got |= 0x001u;
        }
    }
    return u32Got;
}

u32
vfs_ram_poll_mask(i64 i64Fd, u32 u32Want)
{
    return epoll_ready_mask((i32)i64Fd, u32Want);
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
        return -24;
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
            return (i64)iFd;
        }
    }
    return -24;
}

i64
vfs_ram_epoll_ctl(i64 i64Ep, int nOp, i64 i64Fd, u32 u32Events, u64 u64Data)
{
    struct vfs_fd *pEp;
    struct vfs_epoll *pE;
    u32 i;

    if (!vfs_ram_fd_ok(i64Ep)) {
        return -9;
    }
    pEp = &g_aFds[i64Ep];
    if (pEp->u8Kind != VFS_KIND_EPOLL || pEp->u32File >= VFS_MAX_EPOLL) {
        return -22;
    }
    pE = &g_aEpoll[pEp->u32File];
    if (i64Fd == i64Ep) {
        return -22; /* cannot watch self */
    }
    if (nOp == 1 /* EPOLL_CTL_ADD */) {
        if (!vfs_ram_fd_ok(i64Fd)) {
            return -9;
        }
        for (i = 0; i < pE->u8N; i++) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                return -17; /* EEXIST */
            }
        }
        if (pE->u8N >= VFS_EPOLL_WATCH) {
            return -24;
        }
        pE->aWatch[pE->u8N].i32Fd = (i32)i64Fd;
        pE->aWatch[pE->u8N].u32Events = u32Events ? u32Events : 0x001u;
        pE->aWatch[pE->u8N].u64Data = u64Data;
        pE->u8N++;
        return 0;
    }
    if (nOp == 2 /* EPOLL_CTL_DEL */) {
        for (i = 0; i < pE->u8N; i++) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                pE->aWatch[i] = pE->aWatch[pE->u8N - 1u];
                pE->u8N--;
                return 0;
            }
        }
        return -2; /* ENOENT */
    }
    if (nOp == 3 /* EPOLL_CTL_MOD */) {
        for (i = 0; i < pE->u8N; i++) {
            if (pE->aWatch[i].i32Fd == (i32)i64Fd) {
                pE->aWatch[i].u32Events = u32Events ? u32Events : 0x001u;
                pE->aWatch[i].u64Data = u64Data;
                return 0;
            }
        }
        return -2;
    }
    return -22;
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
        return -24;
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
            return (i64)iFd;
        }
    }
    return -24;
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
        return -24;
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
            return (i64)iFd;
        }
    }
    return -24;
}

i64
vfs_ram_inotify_add_watch(i64 i64Fd, const char *szPath, u32 u32Mask)
{
    struct vfs_fd *pFd;
    struct vfs_inotify *pIn;
    u32 i;
    i32 wd;

    if (!vfs_ram_fd_ok(i64Fd) || szPath == NULL || szPath[0] == '\0') {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_INOTIFY || pFd->u32File >= VFS_MAX_INOTIFY) {
        return -22;
    }
    pIn = &g_aInotify[pFd->u32File];
    if (u32Mask == 0) {
        u32Mask = 0x00000300u; /* IN_CREATE|IN_DELETE-shaped subset */
    }
    /* Update existing watch on same path */
    for (i = 0; i < pIn->u8NWatch; i++) {
        if (path_eq(pIn->aWatch[i].szPath, szPath)) {
            pIn->aWatch[i].u32Mask = u32Mask;
            return (i64)pIn->aWatch[i].i32Wd;
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
    return (i64)wd;
}

i64
vfs_ram_inotify_rm_watch(i64 i64Fd, i32 i32Wd)
{
    struct vfs_fd *pFd;
    struct vfs_inotify *pIn;
    u32 i;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
    }
    pFd = &g_aFds[i64Fd];
    if (pFd->u8Kind != VFS_KIND_INOTIFY || pFd->u32File >= VFS_MAX_INOTIFY) {
        return -22;
    }
    pIn = &g_aInotify[pFd->u32File];
    for (i = 0; i < pIn->u8NWatch; i++) {
        if (pIn->aWatch[i].i32Wd == i32Wd) {
            pIn->aWatch[i] = pIn->aWatch[pIn->u8NWatch - 1u];
            pIn->u8NWatch--;
            return 0;
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
        return -9;
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
            return done ? (i64)done : n;
        }
        if (n == 0) {
            break;
        }
        w = vfs_ram_pwrite(i64Out, aBuf, (size_t)n, offOut);
        if (w < 0) {
            return done ? (i64)done : w;
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
    return (i64)done;
}

i64
vfs_ram_pidfd_open(u32 u32Pid, int nFlags)
{
    u32 iFd;

    (void)nFlags;
    if (u32Pid == 0) {
        return -22;
    }
    for (iFd = 3; iFd < VFS_MAX_FDS; iFd++) {
        if (!g_aFds[iFd].u8Used) {
            g_aFds[iFd].u8Used = 1;
            g_aFds[iFd].u8Kind = VFS_KIND_PIDFD;
            g_aFds[iFd].u8End = 0;
            g_aFds[iFd].u32File = 0;
            g_aFds[iFd].u64Off = (u64)u32Pid; /* store target pid */
            return (i64)iFd;
        }
    }
    return -24;
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
            return (i64)iFd;
        }
    }
    return -24; /* EMFILE */
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
            return -9;
        }
        if (g_aFds[i64Fd].u8Kind != VFS_KIND_SIGNALFD ||
            g_aFds[i64Fd].u32File >= VFS_MAX_SIGNALFD) {
            return -22;
        }
        g_aSigMask[g_aFds[i64Fd].u32File] = u64Mask;
        return i64Fd;
    }
    for (iS = 0; iS < VFS_MAX_SIGNALFD; iS++) {
        if (!g_aSigUsed[iS]) {
            break;
        }
    }
    if (iS >= VFS_MAX_SIGNALFD) {
        return -24;
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
            return (i64)iFd;
        }
    }
    return -24;
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
        return -9;
    }
    pEp = &g_aFds[i64Ep];
    if (pEp->u8Kind != VFS_KIND_EPOLL || pEp->u32File >= VFS_MAX_EPOLL) {
        return -22;
    }
    if (nMax <= 0) {
        return 0;
    }
    if (nMax > 16) {
        nMax = 16;
    }
    pE = &g_aEpoll[pEp->u32File];
    pOut = (u8 *)pEvents;
    for (i = 0; i < pE->u8N && (int)nOut < nMax; i++) {
        u32 u32R =
            epoll_ready_mask(pE->aWatch[i].i32Fd, pE->aWatch[i].u32Events);

        if (u32R != 0) {
            if (pOut != NULL) {
                /* packed epoll_event: u32 events + u64 data (12 bytes) */
                memcpy(pOut + (size_t)nOut * 12u, &u32R, 4);
                memcpy(pOut + (size_t)nOut * 12u + 4, &pE->aWatch[i].u64Data,
                       8);
            }
            nOut++;
        }
    }
    return (i64)nOut;
}

i64
vfs_ram_lseek(i64 i64Fd, i64 i64Off, int nWhence)
{
    struct vfs_fd *pFd;
    struct vfs_file *pFile;
    i64 i64New;
    i64 i64End;

    if (!vfs_ram_fd_ok(i64Fd)) {
        return -9;
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
        return -22; /* EINVAL */
    }
    if (i64New < 0) {
        return -22;
    }
    pFd->u64Off = (u64)i64New;
    return i64New;
}

/** How many used files have index < iFile. */
static u32
skip_count_before(u32 iFile)
{
    u32 i;
    u32 n = 0;

    for (i = 0; i < iFile && i < VFS_MAX_FILES; i++) {
        if (g_aFiles[i].u8Used) {
            n++;
        }
    }
    return n;
}

i64
vfs_ram_pread(i64 i64Fd, void *pBuf, size_t cb, u64 u64Off)
{
    u64 u64Saved;
    i64 n;

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
 */
i64
vfs_ram_getdents64(i64 i64Fd, void *pBuf, size_t cb)
{
    struct vfs_fd *pFd;
    u8 *pOut;
    u32 idx;
    u32 written = 0;
    u32 iFile;

    if (!vfs_ram_fd_ok(i64Fd) || pBuf == NULL) {
        return -9;
    }
    if (cb < 32) {
        return -22;
    }
    pFd = &g_aFds[i64Fd];
    pOut = (u8 *)pBuf;
    idx = (u32)pFd->u64Off;
    for (iFile = 0; iFile < VFS_MAX_FILES; iFile++) {
        const char *sz;
        u32 nameLen;
        u32 reclen;
        u32 j;
        u32 pos;

        if (!g_aFiles[iFile].u8Used) {
            continue;
        }
        pos = skip_count_before(iFile);
        if (pos < idx) {
            continue;
        }
        if (pos > idx) {
            break;
        }
        sz = g_aFiles[iFile].szPath;
        {
            const char *b = sz;
            u32 k;

            for (k = 0; sz[k] != '\0'; k++) {
                if (sz[k] == '/' && sz[k + 1] != '\0') {
                    b = sz + k + 1;
                }
            }
            sz = b;
        }
        nameLen = 0;
        while (sz[nameLen] != '\0' && nameLen < 64) {
            nameLen++;
        }
        reclen = (u32)(19u + nameLen + 1u);
        reclen = (reclen + 7u) & ~7u;
        if (written + reclen > cb) {
            break;
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
        pOut[written + 18] = 8;
        for (j = 0; j < nameLen; j++) {
            pOut[written + 19 + j] = (u8)sz[j];
        }
        pOut[written + 19 + nameLen] = 0;
        for (j = 20 + nameLen; j < reclen; j++) {
            pOut[written + j] = 0;
        }
        written += reclen;
        idx++;
        pFd->u64Off = idx;
    }
    return (i64)written;
}
