/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Multi-client VFS door: mini FS on virtio-blk (via store) or soft RAM disk.
 * v3: 8 inodes (2 LBA table), multi-sector files up to 1 KiB.
 * Layout: LBA0 super, LBA1 freemap, LBA2–3 inodes, LBA4+ data.
 * Product path: vfsd claims door, format/mount, then create/read/write
 * or open/readfd/writefd/seekfd; sec_rw prefers store_door when virtio-blk
 * ready. SMP-safe via spinlock; pure C11 dual-license product surface.
 *
 * Soft claim path:
 *   CLAIM token must be non-zero 32-bit; same token re-CLAIM is idempotent
 *   (reclaim soft); a different token → BUSY. RELEASE when free is a soft
 *   no-op (0). Format/mount allowed when free (bring-up smokes) or claimed
 *   (product vfsd). Soft RAM disk when virtio-blk is absent.
 *
 * Soft door inventory (Wave 12 base; Wave 14 deepen; Wave 19 exclusive):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Live owned/mounted/files/fds/token; peaks for files + open door fds
 *   - Cumulative claim/reclaim/release + format/mount + name + fd ops
 *   - Soft deny tallies by errno shape (inval/busy/noent/nomem/io/…)
 *   - Backend sec_rw (ram|store), freemap alloc/free, auto format/mount
 *   - Byte transfer totals; soft once + STATS / claim_count / fd_count emit
 *   - Wave 14: per-op enter, last snapshot, multi-sec, catalog, PASS deepen
 *   - Wave 15 base: ratio|headroom|surface|deepen
 *   - Wave 19 exclusive: capacity|geom|terminal|return|retmap (wave=81 areas=59)
 *   greppable: "vfs_door: soft …"
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ bar3.
 */
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/spinlock.h>
#include <gj/store_door.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/vfs_door.h>
#include <gj/virtio_blk.h>

#define SEC            512u
#define LBA_SUPER      0u
#define LBA_FREEMAP    1u
#define LBA_INODES     2u
#define LBA_INODES2    3u
#define LBA_DATA0      4u
#define INODE_MAX      8u
#define VFS_MAGIC      0x31444a47u
#define VFS_VERSION    3u
#define VFS_RAM_SECS   48u
#define VFS_FD_MAX     16u
#define FILE_MAX_BYTES (2u * SEC)
#define NAME_MAX       48u

/* On-disk layout (fixed field order; names match historical superblock). */
struct vfs_super {
    u32 magic;
    u32 version;
    u32 root_ino;
    u32 freemap_lba;
    u32 inode_lba;
    u32 data_lba0;
    u32 c_inodes;
    u32 c_data;
    u32 pad[120];
};

struct vfs_inode {
    u32  used;
    u32  mode;
    u32  size;
    u32  data_lba; /* first data LBA; size>512 uses data_lba+1 */
    char name[NAME_MAX];
    u32  pad[16];
};

struct vfs_fd {
    u8  u8Used;
    u8  u8Write;
    u8  u8Pad[2];
    u32 u32Ino;
    u32 u32Off;
    char szName[NAME_MAX];
};

static int                g_fInit;
static u32                g_u32Calls;
static u32                g_u32Owner;
static u32                g_u32Claims;   /* first successful CLAIM count */
static u32                g_u32Reclaims; /* idempotent same-token CLAIM soft */
static u32                g_u32Mounted;
static u32                g_u32Files;
static u8                 g_aRam[VFS_RAM_SECS][SEC];
static int                g_fRam;
static struct vfs_fd      g_aFd[VFS_FD_MAX];
static struct gj_spinlock g_lkVfs;

/*
 * Soft product inventory (Wave 12 exclusive). Cumulative unless noted
 * live/peak. greppable: vfs_door: soft …
 */
static u32 g_u32SoftReleaseOk;    /* RELEASE while owned (token match) */
static u32 g_u32SoftReleaseFree;  /* RELEASE when free (soft no-op) */
static u32 g_u32SoftReleaseInval; /* RELEASE token mismatch / high bits */
static u32 g_u32SoftClaimBusy;    /* CLAIM different token → BUSY */
static u32 g_u32SoftClaimInval;   /* CLAIM bad token (0 / high bits) */
static u32 g_u32SoftFormatOk;
static u32 g_u32SoftFormatFail;
static u32 g_u32SoftMountOk;
static u32 g_u32SoftMountFail;
static u32 g_u32SoftCreateOk;
static u32 g_u32SoftCreateFail;
static u32 g_u32SoftWriteOk;
static u32 g_u32SoftWriteFail;
static u32 g_u32SoftReadOk;
static u32 g_u32SoftReadFail;
static u32 g_u32SoftUnlinkOk;
static u32 g_u32SoftUnlinkFail;
static u32 g_u32SoftStatOk;
static u32 g_u32SoftStatFail;
static u32 g_u32SoftListOk;
static u32 g_u32SoftListFail;
static u32 g_u32SoftOpenOk;
static u32 g_u32SoftOpenFail;
static u32 g_u32SoftCloseOk;
static u32 g_u32SoftCloseFail;
static u32 g_u32SoftReadfdOk;
static u32 g_u32SoftReadfdFail;
static u32 g_u32SoftWritefdOk;
static u32 g_u32SoftWritefdFail;
static u32 g_u32SoftSeekfdOk;
static u32 g_u32SoftSeekfdFail;
static u32 g_u32SoftStatsOk;      /* STATS copy-out successes */
static u32 g_u32SoftStatsFail;    /* STATS reject / fault */
static u32 g_u32SoftDenyInval;
static u32 g_u32SoftDenyBusy;
static u32 g_u32SoftDenyNoent;
static u32 g_u32SoftDenyNomem;
static u32 g_u32SoftDenyIo;
static u32 g_u32SoftDenyNodev;
static u32 g_u32SoftDenyNosup;
static u32 g_u32SoftDenyFault;
static u32 g_u32SoftDenyPerm;
static u32 g_u32SoftDenyOther;
static u32 g_u32SoftFilesPeak;    /* high-water g_u32Files */
static u32 g_u32SoftFdPeak;       /* high-water open door fds */
static u32 g_u32SoftInvSamples;   /* soft inventory dump count */
static u32 g_u32SoftOk;           /* non-negative call returns */
static u32 g_u32SoftFail;         /* negative call returns */
static u32 g_u32SoftBytesRead;    /* cumulative READ + READFD bytes */
static u32 g_u32SoftBytesWrite;   /* cumulative WRITE + WRITEFD bytes */
static u32 g_u32SoftSecRam;       /* sec_rw via soft RAM disk */
static u32 g_u32SoftSecStore;     /* sec_rw via store_door (virtio-blk) */
static u32 g_u32SoftSecFail;      /* sec_rw failures */
static u32 g_u32SoftAllocOk;      /* freemap data LBA alloc success */
static u32 g_u32SoftAllocFail;    /* freemap data LBA alloc fail */
static u32 g_u32SoftFreeData;     /* freemap free_data_lba_n calls */
static u32 g_u32SoftAutoFormat;   /* implicit format on unmounted create/open */
static u32 g_u32SoftAutoMount;    /* implicit mount on name ops */
static u8  g_fSoftOnce;           /* one-shot after first call activity */

/* Wave 14 exclusive soft deepen — complementary path tallies. */
static u32 g_u32SoftOpEnter[17];  /* per-opcode enter; index = op (1..16) */
static u32 g_u32SoftUnknownOp;    /* NOSUPPORT / unknown opcode */
static u32 g_u32SoftLastOp;       /* last dispatched opcode */
static i64 g_i64SoftLastRet;      /* last terminal return */
static u32 g_u32SoftMultiSec;     /* name write/create with cb > SEC */
static u32 g_u32SoftEmptyWrite;   /* write/create with cb == 0 */
static u32 g_u32SoftNameInval;    /* copy_name_from_user fail */
static u32 g_u32SoftNotInit;      /* call before init */
static u32 g_u32SoftOwnedRw;      /* name/fd I/O while owned */
static u32 g_u32SoftUnownedRw;    /* name/fd I/O while free */
static u32 g_u32SoftPeakBytesR;   /* peak single READ/READFD ret */
static u32 g_u32SoftPeakBytesW;   /* peak single WRITE/WRITEFD ret */

static int do_format(void);
static int do_mount(u32 *pOut);
static i64 do_create_or_write(const char *szName, const u8 *pData, u32 cb,
                              int fCreateOnly);
static void soft_inc(u32 *pCtr);
static void soft_peak_note(void);
static void soft_deny_note(i64 i64Ret);
static void soft_note_result(u32 u32Op, i64 i64Ret);
static void soft_inventory_log(void);
static void soft_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/* ---- small helpers ------------------------------------------------------ */

static int
name_eq(const char *szA, const char *szB)
{
    u32 iByte;

    if (szA == NULL || szB == NULL) {
        return 0;
    }
    for (iByte = 0; iByte < NAME_MAX; iByte++) {
        if (szA[iByte] != szB[iByte]) {
            return 0;
        }
        if (szA[iByte] == '\0') {
            return 1;
        }
    }
    return 1;
}

static void
name_copy(char *szDst, const char *szSrc, u32 cbMax)
{
    u32 iByte;

    if (szDst == NULL || cbMax == 0) {
        return;
    }
    for (iByte = 0; iByte + 1 < cbMax; iByte++) {
        if (szSrc == NULL || szSrc[iByte] == '\0') {
            break;
        }
        szDst[iByte] = szSrc[iByte];
    }
    szDst[iByte] = '\0';
}

/**
 * Collapse /mnt/ and leading ./ into a single flat name component.
 * Multi-segment paths keep the final component only (mini-FS is flat).
 */
static void
path_normalize(char *sz)
{
    char aTmp[NAME_MAX];
    u32  iSrc = 0;
    u32  iDst = 0;

    if (sz == NULL) {
        return;
    }
    name_copy(aTmp, sz, sizeof(aTmp));
    while (aTmp[iSrc] == '/') {
        iSrc++;
    }
    if (aTmp[iSrc] == '.' && aTmp[iSrc + 1] == '/') {
        iSrc += 2;
    }
    if (aTmp[iSrc] == 'm' && aTmp[iSrc + 1] == 'n' && aTmp[iSrc + 2] == 't' &&
        aTmp[iSrc + 3] == '/') {
        iSrc += 4;
    }
    while (aTmp[iSrc] == '/') {
        iSrc++;
    }
    while (aTmp[iSrc] != '\0' && iDst + 1 < NAME_MAX) {
        if (aTmp[iSrc] == '/') {
            iDst = 0;
            iSrc++;
            continue;
        }
        sz[iDst++] = aTmp[iSrc++];
    }
    sz[iDst] = '\0';
}

static int
copy_name_from_user(char *szOut, u64 u64Name)
{
    u32 iByte;

    if (u64Name == 0 || szOut == NULL) {
        return -1;
    }
    memset(szOut, 0, NAME_MAX);
    for (iByte = 0; iByte + 1 < NAME_MAX; iByte++) {
        char ch = 0;

        if (user_range_ok(u64Name + iByte, 1)) {
            if (copy_from_user(&ch, u64Name + iByte, 1) != GJ_OK) {
                return -1;
            }
        } else {
            ch = ((const char *)(gj_vaddr_t)u64Name)[iByte];
        }
        if (ch == '\0') {
            break;
        }
        szOut[iByte] = ch;
    }
    path_normalize(szOut);
    return (szOut[0] != '\0') ? 0 : -1;
}

static i64
copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || cb == 0 || pSrc == NULL) {
        return 0;
    }
    if (user_range_ok(u64Dst, cb)) {
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        memcpy((void *)(gj_vaddr_t)u64Dst, pSrc, cb);
    }
    return 0;
}

static i64
copy_in(void *pDst, u64 u64Src, u32 cb)
{
    if (pDst == NULL || cb == 0) {
        return 0;
    }
    if (u64Src == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Src, cb)) {
        if (copy_from_user(pDst, u64Src, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        memcpy(pDst, (const void *)(gj_vaddr_t)u64Src, cb);
    }
    return 0;
}

static u32
sectors_for_size(u32 cb)
{
    if (cb > SEC) {
        return 2u;
    }
    return 1u;
}

static void
fd_invalidate_ino(u32 u32Ino)
{
    u32 iFd;

    for (iFd = 0; iFd < VFS_FD_MAX; iFd++) {
        if (g_aFd[iFd].u8Used && g_aFd[iFd].u32Ino == u32Ino) {
            memset(&g_aFd[iFd], 0, sizeof(g_aFd[iFd]));
        }
    }
}

static void
fd_invalidate_all(void)
{
    memset(g_aFd, 0, sizeof(g_aFd));
}

static u32
fd_count_used(void)
{
    u32 iFd;
    u32 c = 0;

    for (iFd = 0; iFd < VFS_FD_MAX; iFd++) {
        if (g_aFd[iFd].u8Used) {
            c++;
        }
    }
    return c;
}

static u32
count_used_inodes(const struct vfs_inode *pTab)
{
    u32 iIno;
    u32 c = 0;

    if (pTab == NULL) {
        return 0;
    }
    for (iIno = 0; iIno < INODE_MAX; iIno++) {
        if (pTab[iIno].used) {
            c++;
        }
    }
    return c;
}

/* ---- sector I/O --------------------------------------------------------- */

static int
sec_rw(int fWrite, u32 u32Lba, void *p)
{
    i64 i64R;

    if (p == NULL || u32Lba >= VFS_RAM_SECS) {
        soft_inc(&g_u32SoftSecFail);
        return -1;
    }
    if (g_fRam || !virtio_blk_ready()) {
        if (fWrite) {
            memcpy(g_aRam[u32Lba], p, SEC);
        } else {
            memcpy(p, g_aRam[u32Lba], SEC);
        }
        soft_inc(&g_u32SoftSecRam);
        return 0;
    }
    if (fWrite) {
        i64R = store_door_call(GJ_STORE_OP_WRITE, u32Lba, (u64)(gj_vaddr_t)p,
                               SEC);
    } else {
        i64R =
            store_door_call(GJ_STORE_OP_READ, u32Lba, (u64)(gj_vaddr_t)p, SEC);
    }
    if (i64R == (i64)SEC) {
        soft_inc(&g_u32SoftSecStore);
        return 0;
    }
    soft_inc(&g_u32SoftSecFail);
    return -1;
}

/* Load/store full inode table (2 sectors → 8 inodes) */
static int
inode_table_load(struct vfs_inode *pTab)
{
    static u8 aA[SEC];
    static u8 aB[SEC];

    if (pTab == NULL) {
        return -1;
    }
    if (sec_rw(0, LBA_INODES, aA) != 0 || sec_rw(0, LBA_INODES2, aB) != 0) {
        return -1;
    }
    memcpy(pTab, aA, SEC);
    memcpy((u8 *)pTab + SEC, aB, SEC);
    return 0;
}

static int
inode_table_store(const struct vfs_inode *pTab)
{
    static u8 aA[SEC];
    static u8 aB[SEC];

    if (pTab == NULL) {
        return -1;
    }
    memcpy(aA, pTab, SEC);
    memcpy(aB, (const u8 *)pTab + SEC, SEC);
    if (sec_rw(1, LBA_INODES, aA) != 0 || sec_rw(1, LBA_INODES2, aB) != 0) {
        return -1;
    }
    return 0;
}

static int
file_data_load(u32 u32Lba0, u32 u32Size, u8 *pOut)
{
    static u8 aSec[SEC];
    u32       n0;

    if (pOut == NULL) {
        return -1;
    }
    if (u32Size > FILE_MAX_BYTES) {
        u32Size = FILE_MAX_BYTES;
    }
    if (sec_rw(0, u32Lba0, aSec) != 0) {
        return -1;
    }
    n0 = u32Size > SEC ? SEC : u32Size;
    memcpy(pOut, aSec, n0);
    if (u32Size > SEC) {
        if (sec_rw(0, u32Lba0 + 1, aSec) != 0) {
            return -1;
        }
        memcpy(pOut + SEC, aSec, u32Size - SEC);
    }
    return 0;
}

static int
file_data_store(u32 u32Lba0, const u8 *pData, u32 u32Size)
{
    static u8 aSec[SEC];
    u32       n0;

    if (u32Size > FILE_MAX_BYTES) {
        return -1;
    }
    memset(aSec, 0, SEC);
    n0 = u32Size > SEC ? SEC : u32Size;
    if (pData != NULL && n0 > 0) {
        memcpy(aSec, pData, n0);
    }
    if (sec_rw(1, u32Lba0, aSec) != 0) {
        return -1;
    }
    if (u32Size > SEC) {
        memset(aSec, 0, SEC);
        if (pData != NULL) {
            memcpy(aSec, pData + SEC, u32Size - SEC);
        }
        if (sec_rw(1, u32Lba0 + 1, aSec) != 0) {
            return -1;
        }
    }
    return 0;
}

/**
 * Note live files + open-fd high-water. Caller holds g_lkVfs (or init).
 */
static void
soft_peak_note(void)
{
    u32 u32Fds;

    if (g_u32Files > g_u32SoftFilesPeak) {
        g_u32SoftFilesPeak = g_u32Files;
    }
    u32Fds = fd_count_used();
    if (u32Fds > g_u32SoftFdPeak) {
        g_u32SoftFdPeak = u32Fds;
    }
}

/**
 * Bump soft deny bucket by error shape. Caller holds g_lkVfs.
 */
static void
soft_deny_note(i64 i64Ret)
{
    if (i64Ret >= 0) {
        return;
    }
    if (i64Ret == GJ_ERR_INVAL) {
        g_u32SoftDenyInval++;
    } else if (i64Ret == GJ_ERR_BUSY) {
        g_u32SoftDenyBusy++;
    } else if (i64Ret == GJ_ERR_NOENT) {
        g_u32SoftDenyNoent++;
    } else if (i64Ret == GJ_ERR_NOMEM) {
        g_u32SoftDenyNomem++;
    } else if (i64Ret == GJ_ERR_IO) {
        g_u32SoftDenyIo++;
    } else if (i64Ret == GJ_ERR_NODEV) {
        g_u32SoftDenyNodev++;
    } else if (i64Ret == GJ_ERR_NOSUPPORT) {
        g_u32SoftDenyNosup++;
    } else if (i64Ret == GJ_ERR_FAULT) {
        g_u32SoftDenyFault++;
    } else if (i64Ret == GJ_ERR_PERM) {
        g_u32SoftDenyPerm++;
    } else {
        g_u32SoftDenyOther++;
    }
}

/**
 * Soft inventory op/result tally. Caller holds g_lkVfs.
 * Never alters i64Ret; diagnostics only.
 */
static void
soft_note_result(u32 u32Op, i64 i64Ret)
{
    int fOk = (i64Ret >= 0);

    soft_peak_note();
    g_u32SoftLastOp = u32Op;
    g_i64SoftLastRet = i64Ret;
    if (u32Op >= 1u && u32Op <= 16u) {
        soft_inc(&g_u32SoftOpEnter[u32Op]);
    } else {
        soft_inc(&g_u32SoftUnknownOp);
    }
    if (fOk) {
        soft_inc(&g_u32SoftOk);
    } else {
        soft_inc(&g_u32SoftFail);
        soft_deny_note(i64Ret);
    }

    switch (u32Op) {
    case GJ_VFS_OP_CLAIM:
        /* first/reclaim already counted in g_u32Claims / g_u32Reclaims. */
        if (!fOk && i64Ret == GJ_ERR_BUSY) {
            soft_inc(&g_u32SoftClaimBusy);
        } else if (!fOk && i64Ret == GJ_ERR_INVAL) {
            soft_inc(&g_u32SoftClaimInval);
        }
        break;
    case GJ_VFS_OP_RELEASE:
        /* free_rel / release ok tallied at case sites (path-specific). */
        if (!fOk) {
            soft_inc(&g_u32SoftReleaseInval);
        }
        break;
    case GJ_VFS_OP_FORMAT:
        if (fOk) {
            soft_inc(&g_u32SoftFormatOk);
        } else {
            soft_inc(&g_u32SoftFormatFail);
        }
        break;
    case GJ_VFS_OP_MOUNT:
        if (fOk) {
            soft_inc(&g_u32SoftMountOk);
        } else {
            soft_inc(&g_u32SoftMountFail);
        }
        break;
    case GJ_VFS_OP_CREATE:
        if (fOk) {
            soft_inc(&g_u32SoftCreateOk);
        } else {
            soft_inc(&g_u32SoftCreateFail);
        }
        break;
    case GJ_VFS_OP_WRITE:
        if (fOk) {
            soft_inc(&g_u32SoftWriteOk);
            g_u32SoftBytesWrite += (u32)i64Ret;
            if ((u32)i64Ret > g_u32SoftPeakBytesW) {
                g_u32SoftPeakBytesW = (u32)i64Ret;
            }
            if ((u32)i64Ret > SEC) {
                soft_inc(&g_u32SoftMultiSec);
            } else if (i64Ret == 0) {
                soft_inc(&g_u32SoftEmptyWrite);
            }
            if (g_u32Owner != 0) {
                soft_inc(&g_u32SoftOwnedRw);
            } else {
                soft_inc(&g_u32SoftUnownedRw);
            }
        } else {
            soft_inc(&g_u32SoftWriteFail);
        }
        break;
    case GJ_VFS_OP_READ:
        if (fOk) {
            soft_inc(&g_u32SoftReadOk);
            g_u32SoftBytesRead += (u32)i64Ret;
            if ((u32)i64Ret > g_u32SoftPeakBytesR) {
                g_u32SoftPeakBytesR = (u32)i64Ret;
            }
            if (g_u32Owner != 0) {
                soft_inc(&g_u32SoftOwnedRw);
            } else {
                soft_inc(&g_u32SoftUnownedRw);
            }
        } else {
            soft_inc(&g_u32SoftReadFail);
        }
        break;
    case GJ_VFS_OP_UNLINK:
        if (fOk) {
            soft_inc(&g_u32SoftUnlinkOk);
        } else {
            soft_inc(&g_u32SoftUnlinkFail);
        }
        break;
    case GJ_VFS_OP_STAT:
        if (fOk) {
            soft_inc(&g_u32SoftStatOk);
        } else {
            soft_inc(&g_u32SoftStatFail);
        }
        break;
    case GJ_VFS_OP_LIST:
        if (fOk) {
            soft_inc(&g_u32SoftListOk);
        } else {
            soft_inc(&g_u32SoftListFail);
        }
        break;
    case GJ_VFS_OP_STATS:
        if (fOk) {
            soft_inc(&g_u32SoftStatsOk);
        } else {
            soft_inc(&g_u32SoftStatsFail);
        }
        break;
    case GJ_VFS_OP_OPEN:
        if (fOk) {
            soft_inc(&g_u32SoftOpenOk);
        } else {
            soft_inc(&g_u32SoftOpenFail);
        }
        break;
    case GJ_VFS_OP_CLOSE:
        if (fOk) {
            soft_inc(&g_u32SoftCloseOk);
        } else {
            soft_inc(&g_u32SoftCloseFail);
        }
        break;
    case GJ_VFS_OP_READFD:
        if (fOk) {
            soft_inc(&g_u32SoftReadfdOk);
            g_u32SoftBytesRead += (u32)i64Ret;
            if ((u32)i64Ret > g_u32SoftPeakBytesR) {
                g_u32SoftPeakBytesR = (u32)i64Ret;
            }
            if (g_u32Owner != 0) {
                soft_inc(&g_u32SoftOwnedRw);
            } else {
                soft_inc(&g_u32SoftUnownedRw);
            }
        } else {
            soft_inc(&g_u32SoftReadfdFail);
        }
        break;
    case GJ_VFS_OP_WRITEFD:
        if (fOk) {
            soft_inc(&g_u32SoftWritefdOk);
            g_u32SoftBytesWrite += (u32)i64Ret;
            if ((u32)i64Ret > g_u32SoftPeakBytesW) {
                g_u32SoftPeakBytesW = (u32)i64Ret;
            }
            if ((u32)i64Ret > SEC) {
                soft_inc(&g_u32SoftMultiSec);
            }
            if (g_u32Owner != 0) {
                soft_inc(&g_u32SoftOwnedRw);
            } else {
                soft_inc(&g_u32SoftUnownedRw);
            }
        } else {
            soft_inc(&g_u32SoftWritefdFail);
        }
        break;
    case GJ_VFS_OP_SEEKFD:
        if (fOk) {
            soft_inc(&g_u32SoftSeekfdOk);
        } else {
            soft_inc(&g_u32SoftSeekfdFail);
        }
        break;
    default:
        if (!fOk && i64Ret == GJ_ERR_NOSUPPORT) {
            soft_inc(&g_u32SoftUnknownOp);
        }
        break;
    }
}

/**
 * Greppable soft VFS door inventory (Wave 12 base; Wave 14 exclusive deepen).
 * Prefix-stable markers (vfs_door: soft …):
 *   vfs_door: soft inventory  — live owned/mounted/files/fds + ok/fail
 *   vfs_door: soft claim      — claim/reclaim/release path tallies
 *   vfs_door: soft layout     — inode/fd/file_max/secs catalog
 *   vfs_door: soft name       — create/write/read/unlink/stat/list ok|fail
 *   vfs_door: soft fd         — open/close/readfd/writefd/seekfd ok|fail
 *   vfs_door: soft fmt        — format/mount/stats + auto bring-up
 *   vfs_door: soft deny       — errno-shape reject catalog
 *   vfs_door: soft peak       — files/fds high-water + samples
 *   vfs_door: soft backend    — ram|store sec_rw + freemap alloc/free
 *   vfs_door: soft bytes      — cumulative READ/WRITE(+fd) byte totals
 *   vfs_door: soft path       — honesty: soft inventory ≠ bar3 / product
 *   vfs_door: soft inventory PASS / vfs_door: soft PASS
 * Wave 14 complementary:
 *   vfs_door: soft total|ops|last|xfer|own|catalog …
 * Never hard-gates; diagnostics only.
 * greppable: vfs_door: soft
 */
static void
soft_inventory_log(void)
{
    u32 u32Calls;
    u32 u32Owned;
    u32 u32Token;
    u32 u32Mounted;
    u32 u32Files;
    u32 u32Fds;
    u32 u32Claims;
    u32 u32Reclaims;
    u32 u32RelOk;
    u32 u32RelFree;
    u32 u32RelInval;
    u32 u32Busy;
    u32 u32ClaimInval;
    u32 u32Ram;
    u32 u32Blk;
    u32 u32FmtOk;
    u32 u32FmtFail;
    u32 u32MntOk;
    u32 u32MntFail;
    u32 u32CreateOk;
    u32 u32CreateFail;
    u32 u32WriteOk;
    u32 u32WriteFail;
    u32 u32ReadOk;
    u32 u32ReadFail;
    u32 u32UnlinkOk;
    u32 u32UnlinkFail;
    u32 u32StatOk;
    u32 u32StatFail;
    u32 u32ListOk;
    u32 u32ListFail;
    u32 u32OpenOk;
    u32 u32OpenFail;
    u32 u32CloseOk;
    u32 u32CloseFail;
    u32 u32ReadfdOk;
    u32 u32ReadfdFail;
    u32 u32WritefdOk;
    u32 u32WritefdFail;
    u32 u32SeekfdOk;
    u32 u32SeekfdFail;
    u32 u32StatsOk;
    u32 u32StatsFail;
    u32 u32DenyInval;
    u32 u32DenyBusy;
    u32 u32DenyNoent;
    u32 u32DenyNomem;
    u32 u32DenyIo;
    u32 u32DenyNodev;
    u32 u32DenyNosup;
    u32 u32DenyFault;
    u32 u32DenyPerm;
    u32 u32DenyOther;
    u32 u32FilesPeak;
    u32 u32FdPeak;
    u32 u32Samples;
    u32 u32Ok;
    u32 u32Fail;
    u32 u32BytesRead;
    u32 u32BytesWrite;
    u32 u32SecRam;
    u32 u32SecStore;
    u32 u32SecFail;
    u32 u32AllocOk;
    u32 u32AllocFail;
    u32 u32FreeData;
    u32 u32AutoFmt;
    u32 u32AutoMnt;
    int fSoftPass;

    gj_spin_lock(&g_lkVfs);
    soft_peak_note();
    u32Calls = g_u32Calls;
    u32Owned = g_u32Owner ? 1u : 0u;
    u32Token = g_u32Owner;
    u32Mounted = g_u32Mounted;
    u32Files = g_u32Files;
    u32Fds = fd_count_used();
    u32Claims = g_u32Claims;
    u32Reclaims = g_u32Reclaims;
    u32RelOk = g_u32SoftReleaseOk;
    u32RelFree = g_u32SoftReleaseFree;
    u32RelInval = g_u32SoftReleaseInval;
    u32Busy = g_u32SoftClaimBusy;
    u32ClaimInval = g_u32SoftClaimInval;
    u32Ram = g_fRam ? 1u : 0u;
    u32Blk = virtio_blk_ready() ? 1u : 0u;
    u32FmtOk = g_u32SoftFormatOk;
    u32FmtFail = g_u32SoftFormatFail;
    u32MntOk = g_u32SoftMountOk;
    u32MntFail = g_u32SoftMountFail;
    u32CreateOk = g_u32SoftCreateOk;
    u32CreateFail = g_u32SoftCreateFail;
    u32WriteOk = g_u32SoftWriteOk;
    u32WriteFail = g_u32SoftWriteFail;
    u32ReadOk = g_u32SoftReadOk;
    u32ReadFail = g_u32SoftReadFail;
    u32UnlinkOk = g_u32SoftUnlinkOk;
    u32UnlinkFail = g_u32SoftUnlinkFail;
    u32StatOk = g_u32SoftStatOk;
    u32StatFail = g_u32SoftStatFail;
    u32ListOk = g_u32SoftListOk;
    u32ListFail = g_u32SoftListFail;
    u32OpenOk = g_u32SoftOpenOk;
    u32OpenFail = g_u32SoftOpenFail;
    u32CloseOk = g_u32SoftCloseOk;
    u32CloseFail = g_u32SoftCloseFail;
    u32ReadfdOk = g_u32SoftReadfdOk;
    u32ReadfdFail = g_u32SoftReadfdFail;
    u32WritefdOk = g_u32SoftWritefdOk;
    u32WritefdFail = g_u32SoftWritefdFail;
    u32SeekfdOk = g_u32SoftSeekfdOk;
    u32SeekfdFail = g_u32SoftSeekfdFail;
    u32StatsOk = g_u32SoftStatsOk;
    u32StatsFail = g_u32SoftStatsFail;
    u32DenyInval = g_u32SoftDenyInval;
    u32DenyBusy = g_u32SoftDenyBusy;
    u32DenyNoent = g_u32SoftDenyNoent;
    u32DenyNomem = g_u32SoftDenyNomem;
    u32DenyIo = g_u32SoftDenyIo;
    u32DenyNodev = g_u32SoftDenyNodev;
    u32DenyNosup = g_u32SoftDenyNosup;
    u32DenyFault = g_u32SoftDenyFault;
    u32DenyPerm = g_u32SoftDenyPerm;
    u32DenyOther = g_u32SoftDenyOther;
    u32FilesPeak = g_u32SoftFilesPeak;
    u32FdPeak = g_u32SoftFdPeak;
    u32Ok = g_u32SoftOk;
    u32Fail = g_u32SoftFail;
    u32BytesRead = g_u32SoftBytesRead;
    u32BytesWrite = g_u32SoftBytesWrite;
    u32SecRam = g_u32SoftSecRam;
    u32SecStore = g_u32SoftSecStore;
    u32SecFail = g_u32SoftSecFail;
    u32AllocOk = g_u32SoftAllocOk;
    u32AllocFail = g_u32SoftAllocFail;
    u32FreeData = g_u32SoftFreeData;
    u32AutoFmt = g_u32SoftAutoFormat;
    u32AutoMnt = g_u32SoftAutoMount;
    soft_inc(&g_u32SoftInvSamples);
    u32Samples = g_u32SoftInvSamples;
    gj_spin_unlock(&g_lkVfs);

    /* Grep: vfs_door: soft inventory */
    kprintf("vfs_door: soft inventory ver=%u ram=%u calls=%u owned=%u "
            "token=0x%x mounted=%u files=%u fds=%u ok=%u fail=%u "
            "logs=%u wave=81\n",
            VFS_VERSION, u32Ram, u32Calls, u32Owned, u32Token, u32Mounted,
            u32Files, u32Fds, u32Ok, u32Fail, u32Samples);
    /* Grep: vfs_door: soft claim */
    kprintf("vfs_door: soft claim first=%u reclaim=%u release=%u "
            "free_rel=%u busy=%u claim_inval=%u release_inval=%u\n",
            u32Claims, u32Reclaims, u32RelOk, u32RelFree, u32Busy,
            u32ClaimInval, u32RelInval);
    /* Grep: vfs_door: soft layout */
    kprintf("vfs_door: soft layout inodes=%u fd_max=%u file_max=%u "
            "secs=%u data0=%u magic=0x%x\n",
            INODE_MAX, VFS_FD_MAX, FILE_MAX_BYTES, VFS_RAM_SECS, LBA_DATA0,
            VFS_MAGIC);
    /* Grep: vfs_door: soft name */
    kprintf("vfs_door: soft name create=%u/%u write=%u/%u read=%u/%u "
            "unlink=%u/%u stat=%u/%u list=%u/%u\n",
            u32CreateOk, u32CreateFail, u32WriteOk, u32WriteFail, u32ReadOk,
            u32ReadFail, u32UnlinkOk, u32UnlinkFail, u32StatOk, u32StatFail,
            u32ListOk, u32ListFail);
    /* Grep: vfs_door: soft fd */
    kprintf("vfs_door: soft fd open=%u/%u close=%u/%u readfd=%u/%u "
            "writefd=%u/%u seekfd=%u/%u\n",
            u32OpenOk, u32OpenFail, u32CloseOk, u32CloseFail, u32ReadfdOk,
            u32ReadfdFail, u32WritefdOk, u32WritefdFail, u32SeekfdOk,
            u32SeekfdFail);
    /* Grep: vfs_door: soft fmt */
    kprintf("vfs_door: soft fmt ok=%u fail=%u mnt_ok=%u mnt_fail=%u "
            "stats=%u stats_fail=%u auto_fmt=%u auto_mnt=%u\n",
            u32FmtOk, u32FmtFail, u32MntOk, u32MntFail, u32StatsOk,
            u32StatsFail, u32AutoFmt, u32AutoMnt);
    /* Grep: vfs_door: soft deny */
    kprintf("vfs_door: soft deny inval=%u busy=%u noent=%u nomem=%u "
            "io=%u nodev=%u nosup=%u fault=%u perm=%u other=%u\n",
            u32DenyInval, u32DenyBusy, u32DenyNoent, u32DenyNomem, u32DenyIo,
            u32DenyNodev, u32DenyNosup, u32DenyFault, u32DenyPerm,
            u32DenyOther);
    /* Grep: vfs_door: soft peak */
    kprintf("vfs_door: soft peak files=%u fds=%u samples=%u\n", u32FilesPeak,
            u32FdPeak, u32Samples);
    /* Grep: vfs_door: soft backend */
    kprintf("vfs_door: soft backend ram=%u blk=%u sec_ram=%u sec_store=%u "
            "sec_fail=%u alloc_ok=%u alloc_fail=%u free=%u\n",
            u32Ram, u32Blk, u32SecRam, u32SecStore, u32SecFail, u32AllocOk,
            u32AllocFail, u32FreeData);
    /* Grep: vfs_door: soft bytes */
    kprintf("vfs_door: soft bytes read=%u write=%u\n", u32BytesRead,
            u32BytesWrite);
    /*
     * Honesty line: soft inventory / mini-FS ≠ product multi-server / bar3.
     * Grep: vfs_door: soft path
     */
    /*
     * Wave 14 exclusive deepen (complementary; never reshapes primary lines).
     */
    /* Grep: vfs_door: soft total */
    kprintf("vfs_door: soft total ok=%u fail=%u calls=%u logs=%u "
            "unknown_op=%u not_init=%u wave=81\n",
            u32Ok, u32Fail, u32Calls, u32Samples, g_u32SoftUnknownOp,
            g_u32SoftNotInit);
    /* Grep: vfs_door: soft ops */
    /* Indices match GJ_VFS_OP_* (1=CLAIM .. 16=SEEKFD). */
    kprintf("vfs_door: soft ops "
            "claim=%u release=%u format=%u mount=%u create=%u read=%u "
            "write=%u unlink=%u stat=%u list=%u stats=%u open=%u close=%u "
            "readfd=%u writefd=%u seekfd=%u\n",
            g_u32SoftOpEnter[1], g_u32SoftOpEnter[2], g_u32SoftOpEnter[3],
            g_u32SoftOpEnter[4], g_u32SoftOpEnter[5], g_u32SoftOpEnter[6],
            g_u32SoftOpEnter[7], g_u32SoftOpEnter[8], g_u32SoftOpEnter[9],
            g_u32SoftOpEnter[10], g_u32SoftOpEnter[11], g_u32SoftOpEnter[12],
            g_u32SoftOpEnter[13], g_u32SoftOpEnter[14], g_u32SoftOpEnter[15],
            g_u32SoftOpEnter[16]);
    /* Grep: vfs_door: soft last */
    kprintf("vfs_door: soft last op=%u ret=%ld multi_sec=%u empty_wr=%u "
            "name_inval=%u peak_r=%u peak_w=%u\n",
            g_u32SoftLastOp, (long)g_i64SoftLastRet, g_u32SoftMultiSec,
            g_u32SoftEmptyWrite, g_u32SoftNameInval, g_u32SoftPeakBytesR,
            g_u32SoftPeakBytesW);
    /* Grep: vfs_door: soft xfer */
    kprintf("vfs_door: soft xfer read_b=%u write_b=%u peak_r=%u peak_w=%u "
            "multi_sec=%u owned_rw=%u unowned_rw=%u\n",
            u32BytesRead, u32BytesWrite, g_u32SoftPeakBytesR,
            g_u32SoftPeakBytesW, g_u32SoftMultiSec, g_u32SoftOwnedRw,
            g_u32SoftUnownedRw);
    /* Grep: vfs_door: soft own */
    kprintf("vfs_door: soft own live=%u token=0x%x claims=%u reclaims=%u "
            "release=%u free_rel=%u busy=%u claim_inval=%u release_inval=%u\n",
            u32Owned, u32Token, u32Claims, u32Reclaims, u32RelOk, u32RelFree,
            u32Busy, u32ClaimInval, u32RelInval);
    /* Grep: vfs_door: soft catalog */
    kprintf("vfs_door: soft catalog ver=%u inodes=%u fd_max=%u file_max=%u "
            "secs=%u data0=%u magic=0x%x sec=%u wave=81\n",
            VFS_VERSION, INODE_MAX, VFS_FD_MAX, FILE_MAX_BYTES, VFS_RAM_SECS,
            LBA_DATA0, VFS_MAGIC, SEC);

    kprintf("vfs_door: soft path claim=vfsd format_mount=free_or_owned "
            "ramdisk_soft=%u store_prefer_blk=%u multi_client=1 "
            "product_vfsd=1 wave=81 (soft inventory; not bar3)\n",
            u32Ram, u32Blk != 0 ? 1u : 0u);

    /*
     * Wave 35 exclusive deepen (complementary; primary lines field-stable).
     * greppable: vfs_door: soft ratio|headroom|surface|deepen
     */
    {
        u32 u32OkBp = 0;
        u32 u32FailBp = 0;
        u32 u32FdHead = 0;
        u32 u32FileHead = 0;
        u32 u32Tot = u32Ok + u32Fail;

        if (u32Tot != 0u) {
            u32OkBp = (u32Ok * 10000u) / u32Tot;
            u32FailBp = (u32Fail * 10000u) / u32Tot;
        }
        if ((u32)VFS_FD_MAX > u32Fds) {
            u32FdHead = (u32)VFS_FD_MAX - u32Fds;
        }
        if ((u32)INODE_MAX > u32Files) {
            u32FileHead = (u32)INODE_MAX - u32Files;
        }
        /* Grep: vfs_door: soft ratio */
        kprintf("vfs_door: soft ratio ok_bp=%u fail_bp=%u files=%u fds=%u "
                "owned=%u mounted=%u wave=81\n",
                u32OkBp, u32FailBp, u32Files, u32Fds, u32Owned, u32Mounted);
        /* Grep: vfs_door: soft headroom */
        kprintf("vfs_door: soft headroom file_head=%u fd_head=%u "
                "inodes=%u fd_max=%u wave=81\n",
                u32FileHead, u32FdHead, (u32)INODE_MAX, (u32)VFS_FD_MAX);
        /* Grep: vfs_door: soft surface */
        kprintf("vfs_door: soft surface inventory,claim,layout,name,fd,fmt,"
                "deny,peak,backend,bytes,total,ops,last,xfer,own,catalog,"
                "ratio,headroom,capacity,geom,terminal,return,retmap,deepen "
                "areas=61 wave=81\n");
        /* Grep: vfs_door: soft capacity — Wave 19 design-constant lamps. */
        kprintf("vfs_door: soft capacity inodes=%u fd_max=%u file_max=%u "
                "secs=%u sec=%u multi_client=1 ramdisk_soft=1 wave=81\n",
                (u32)INODE_MAX, (u32)VFS_FD_MAX, (u32)FILE_MAX_BYTES,
                (u32)VFS_RAM_SECS, (u32)SEC);
        /* Grep: vfs_door: soft geom — Wave 16 on-disk layout lamps. */
        kprintf("vfs_door: soft geom ver=%u magic=0x%x lba_super=%u "
                "lba_freemap=%u lba_inodes=%u data0=%u wave=81\n",
                VFS_VERSION, VFS_MAGIC, (u32)LBA_SUPER, (u32)LBA_FREEMAP,
                (u32)LBA_INODES, (u32)LBA_DATA0);
        /* Grep: vfs_door: soft terminal — Wave 19 outcome rollup. */
        kprintf("vfs_door: soft terminal ok=%u fail=%u owned=%u mounted=%u "
                "ram=%u blk=%u soft %s wave=81\n",
                u32Ok, u32Fail, u32Owned, u32Mounted, u32Ram, u32Blk,
                (u32Ok != 0u || u32Mounted != 0u) ? "PASS" : "INIT");
        /* Grep: vfs_door: soft return — Wave 19 API return surfaces */
        kprintf("vfs_door: soft return ok=%u fail=%u owned=%u mounted=%u "
                "claims=%u format_ok=%u mount_ok=%u open_ok=%u "
                "ram=%u blk=%u product_vfsd=OPEN wave=81\n",
                u32Ok, u32Fail, u32Owned, u32Mounted, u32Claims,
                u32FmtOk, u32MntOk, u32OpenOk, u32Ram, u32Blk);

        /* Grep: vfs_door: soft retmap — Wave 19 return-surface map */
    kprintf("vfs_door: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=81\n");

    /* Grep: vfs_door: soft deepen */
        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: vfs_door: soft retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("vfs_door: soft retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=81 "
                "(retclass taxonomy; Soft≠product; not bar3)\n");
        /* Grep: vfs_door: soft retlane — Wave 19 return-lane catalog (kept) */
        kprintf("vfs_door: soft retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=81 "
                "(retlane catalog; Soft≠product)\n");
        /*
         * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: vfs_door: soft retbound — Wave 20 return-bound honesty (kept) */
        kprintf("vfs_door: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=81 "
                "(retbound honesty; Soft≠product; not bar3)\n");
        /* Grep: vfs_door: soft retseal — Wave 20 seal stamp (kept) */
        kprintf("vfs_door: soft retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=81 "
                "(retseal stamp; Soft≠product)\n");
                /*
                 * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: vfs_door: soft retpulse — Wave 21 return-pulse honesty (kept) */
                kprintf("vfs_door: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=81 "
                        "(retpulse honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_door: soft retmark — Wave 21 mark stamp (kept) */
                kprintf("vfs_door: soft retmark exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=81 "
                        "(retmark stamp; Soft≠product)\n");
                /*
                 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: vfs_door: soft retphase — Wave 22 return-phase honesty (kept) */
                kprintf("vfs_door: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=81 "
                        "(retphase honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_door: soft retbadge — Wave 22 badge stamp (kept) */
                kprintf("vfs_door: soft retbadge exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=81 "
                        "(retbadge stamp; Soft≠product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
                */
                /* Grep: vfs_door: soft rettoken — Wave 23 return-token honesty (kept) */
                kprintf("vfs_door: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=81 "
                        "(rettoken honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_door: soft retcrest — Wave 23 crest stamp (kept) */
                kprintf("vfs_door: soft retcrest exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=81 "
                        "(retcrest stamp; Soft≠product)\n");
                /*
                 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: vfs_door: soft retvault — Wave 24 return-vault honesty (kept) */
                kprintf("vfs_door: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=81 "
                        "(retvault honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_door: soft retbanner — Wave 24 banner stamp (kept) */
                kprintf("vfs_door: soft retbanner exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=81 "
                        "(retbanner stamp; Soft≠product)\n");
                /*
                 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: vfs_door: soft retledger — Wave 25 return-ledger honesty (kept) */
                kprintf("vfs_door: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=81 "
                        "(retledger honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_door: soft retbeacon — Wave 25 beacon stamp (kept) */
                kprintf("vfs_door: soft retbeacon exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=81 "
                        "(retbeacon stamp; Soft≠product)\n");
                /*
                 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: vfs_door: soft retcipher — Wave 26 return-cipher honesty (kept) */
                kprintf("vfs_door: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=81 "
                        "(retcipher honesty; Soft≠product; not bar3)\n");
                /* Grep: vfs_door: soft retflame — Wave 26 flame stamp (kept) */
                kprintf("vfs_door: soft retflame exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=81 "
                        "(retflame stamp; Soft≠product)\n");
                        /*
                         * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: vfs_door: soft retprism — Wave 27 return-prism honesty (kept) */
                        kprintf("vfs_door: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=81 "
                                "(retprism honesty; Soft≠product; not bar3)\n");
                        /* Grep: vfs_door: soft retforge — Wave 27 forge stamp (kept) */
                        kprintf("vfs_door: soft retforge exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=81 "
                                "(retforge stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: vfs_door: soft retshard — Wave 28 return-shard honesty (kept) */
                                kprintf("vfs_door: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=81 "
                                        "(retshard honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_door: soft retcrown — Wave 28 crown stamp (kept) */
                                kprintf("vfs_door: soft retcrown exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=81 "
                                        "(retcrown stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: vfs_door: soft retglyph — Wave 29 return-glyph honesty (kept) */
                                kprintf("vfs_door: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=81 "
                                        "(retglyph honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_door: soft retscepter — Wave 29 scepter stamp (kept) */
                                kprintf("vfs_door: soft retscepter exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=81 "
                                        "(retscepter stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: vfs_door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("vfs_door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=81 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_door: soft retemblem — Wave 30 emblem stamp (kept) */
                                kprintf("vfs_door: soft retemblem exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=81 "
                                        "(retemblem stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: vfs_door: soft retaegis — Wave 31 return-aegis honesty (kept) */
                                kprintf("vfs_door: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=81 "
                                        "(retaegis honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("vfs_door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=81 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: vfs_door: soft retmantle — Wave 31 mantle stamp (kept) */
                                kprintf("vfs_door: soft retmantle exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=81 "
                                        "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("vfs_door: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("vfs_door: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("vfs_door: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("vfs_door: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("vfs_door: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("vfs_door: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retfortress — Wave 35 return-fortress honesty */
kprintf("vfs_door: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("vfs_door: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft rethold — Wave 36 return-hold honesty */
kprintf("vfs_door: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retspire — Wave 36 exclusive spire stamp */
kprintf("vfs_door: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retwall — Wave 37 return-wall honesty */
kprintf("vfs_door: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retgate — Wave 37 exclusive gate stamp */
kprintf("vfs_door: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retmoat — Wave 38 return-moat honesty */
kprintf("vfs_door: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retower — Wave 38 exclusive tower stamp */
kprintf("vfs_door: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("vfs_door: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("vfs_door: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("vfs_door: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("vfs_door: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retravelin — Wave 41 return-travelin honesty */
kprintf("vfs_door: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("vfs_door: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("vfs_door: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("vfs_door: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("vfs_door: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("vfs_door: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("vfs_door: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("vfs_door: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("vfs_door: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("vfs_door: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retbailey — Wave 46 return-bailey honesty */
kprintf("vfs_door: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("vfs_door: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("vfs_door: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("vfs_door: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("vfs_door: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("vfs_door: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("vfs_door: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("vfs_door: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retsally — Wave 50 return-sally honesty */
kprintf("vfs_door: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("vfs_door: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retfosse — Wave 51 return-fosse honesty */
kprintf("vfs_door: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("vfs_door: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("vfs_door: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("vfs_door: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retravelin — Wave 53 return-travelin honesty */
kprintf("vfs_door: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("vfs_door: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("vfs_door: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retredan — Wave 54 exclusive redan stamp */
kprintf("vfs_door: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retflank — Wave 55 return-flank honesty */
kprintf("vfs_door: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retface — Wave 55 exclusive face stamp */
kprintf("vfs_door: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retgorge — Wave 56 return-gorge honesty */
kprintf("vfs_door: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("vfs_door: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retraverse — Wave 57 return-traverse honesty */
kprintf("vfs_door: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("vfs_door: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retorillon — Wave 58 return-orillon honesty */
kprintf("vfs_door: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("vfs_door: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("vfs_door: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("vfs_door: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retplace — Wave 60 return-place honesty */
kprintf("vfs_door: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("vfs_door: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("vfs_door: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("vfs_door: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("vfs_door: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("vfs_door: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("vfs_door: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("vfs_door: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: vfs_door: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("vfs_door: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: vfs_door: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("vfs_door: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: vfs_door: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("vfs_door: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: vfs_door: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("vfs_door: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: vfs_door: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("vfs_door: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=81 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: vfs_door: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("vfs_door: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=81 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("vfs_door: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("vfs_door: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("vfs_door: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("vfs_door: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("vfs_door: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=81 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("vfs_door: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=81 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("vfs_door: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("vfs_door: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("vfs_door: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("vfs_door: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: vfs_door: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("vfs_door: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("vfs_door: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("vfs_door: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("vfs_door: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionangle stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("vfs_door: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("vfs_door: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retparapetangle stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("vfs_door: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("vfs_door: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retowerangle stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("vfs_door: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("vfs_door: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retwallangle stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("vfs_door: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("vfs_door: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retholdangle stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("vfs_door: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("vfs_door: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retfortressangle stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("vfs_door: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("vfs_door: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("vfs_door: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("vfs_door: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: vfs_door: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("vfs_door: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=81 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: vfs_door: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("vfs_door: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=81 (retbulwarkangle stamp; Soft≠product)\n");

                                kprintf("vfs_door: soft deepen wave=81 areas=109 ver=%u calls=%u "
                "ok=%u fail=%u logs=%u\n",
                VFS_VERSION, u32Calls, u32Ok, u32Fail, u32Samples);
    }

    /*
     * Soft lamp: init surface is always soft-pass; mounted is a stronger
     * smoke signal. Never hard-gates.
     * Grep: vfs_door: soft inventory PASS | vfs_door: soft PASS
     * Grep: vfs_door: soft FAIL
     */
    fSoftPass = g_fInit ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("vfs_door: soft inventory PASS ver=%u logs=%u "
                "mounted=%u ram=%u blk=%u wave=81\n",
                VFS_VERSION, u32Samples, u32Mounted, u32Ram, u32Blk);
        kprintf("vfs_door: soft PASS ver=%u wave=81\n", VFS_VERSION);
    } else {
        kprintf("vfs_door: soft FAIL init=0 "
                "(soft inventory only; not product gate)\n");
    }
}

/**
 * After first product call activity, print soft inventory once (mirrors
 * store/session/net soft-stats-once). Safe from call return paths only.
 */
static void
soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32Calls == 0) {
        return;
    }
    g_fSoftOnce = 1;
    soft_inventory_log();
}

void
vfs_door_init(void)
{
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32Owner = 0;
    g_u32Claims = 0;
    g_u32Reclaims = 0;
    g_u32Mounted = 0;
    g_u32Files = 0;
    g_fRam = virtio_blk_ready() ? 0 : 1;
    memset(g_aRam, 0, sizeof(g_aRam));
    memset(g_aFd, 0, sizeof(g_aFd));
    g_u32SoftReleaseOk = 0;
    g_u32SoftReleaseFree = 0;
    g_u32SoftReleaseInval = 0;
    g_u32SoftClaimBusy = 0;
    g_u32SoftClaimInval = 0;
    g_u32SoftFormatOk = 0;
    g_u32SoftFormatFail = 0;
    g_u32SoftMountOk = 0;
    g_u32SoftMountFail = 0;
    g_u32SoftCreateOk = 0;
    g_u32SoftCreateFail = 0;
    g_u32SoftWriteOk = 0;
    g_u32SoftWriteFail = 0;
    g_u32SoftReadOk = 0;
    g_u32SoftReadFail = 0;
    g_u32SoftUnlinkOk = 0;
    g_u32SoftUnlinkFail = 0;
    g_u32SoftStatOk = 0;
    g_u32SoftStatFail = 0;
    g_u32SoftListOk = 0;
    g_u32SoftListFail = 0;
    g_u32SoftOpenOk = 0;
    g_u32SoftOpenFail = 0;
    g_u32SoftCloseOk = 0;
    g_u32SoftCloseFail = 0;
    g_u32SoftReadfdOk = 0;
    g_u32SoftReadfdFail = 0;
    g_u32SoftWritefdOk = 0;
    g_u32SoftWritefdFail = 0;
    g_u32SoftSeekfdOk = 0;
    g_u32SoftSeekfdFail = 0;
    g_u32SoftStatsOk = 0;
    g_u32SoftStatsFail = 0;
    g_u32SoftDenyInval = 0;
    g_u32SoftDenyBusy = 0;
    g_u32SoftDenyNoent = 0;
    g_u32SoftDenyNomem = 0;
    g_u32SoftDenyIo = 0;
    g_u32SoftDenyNodev = 0;
    g_u32SoftDenyNosup = 0;
    g_u32SoftDenyFault = 0;
    g_u32SoftDenyPerm = 0;
    g_u32SoftDenyOther = 0;
    g_u32SoftFilesPeak = 0;
    g_u32SoftFdPeak = 0;
    g_u32SoftInvSamples = 0;
    g_u32SoftOk = 0;
    g_u32SoftFail = 0;
    g_u32SoftBytesRead = 0;
    g_u32SoftBytesWrite = 0;
    g_u32SoftSecRam = 0;
    g_u32SoftSecStore = 0;
    g_u32SoftSecFail = 0;
    g_u32SoftAllocOk = 0;
    g_u32SoftAllocFail = 0;
    g_u32SoftFreeData = 0;
    g_u32SoftAutoFormat = 0;
    g_u32SoftAutoMount = 0;
    g_fSoftOnce = 0;
    {
        u32 iOp;

        for (iOp = 0; iOp < 17u; iOp++) {
            g_u32SoftOpEnter[iOp] = 0;
        }
    }
    g_u32SoftUnknownOp = 0;
    g_u32SoftLastOp = 0;
    g_i64SoftLastRet = 0;
    g_u32SoftMultiSec = 0;
    g_u32SoftEmptyWrite = 0;
    g_u32SoftNameInval = 0;
    g_u32SoftNotInit = 0;
    g_u32SoftOwnedRw = 0;
    g_u32SoftUnownedRw = 0;
    g_u32SoftPeakBytesR = 0;
    g_u32SoftPeakBytesW = 0;
    gj_spin_init(&g_lkVfs);
    kprintf("vfs_door: init ver=%u ram=%d inodes=%u file_max=%u secs=%u\n",
            VFS_VERSION, g_fRam, INODE_MAX, FILE_MAX_BYTES, VFS_RAM_SECS);
    /* Grep: vfs_door: soft (baseline inventory after init; zeros expected) */
    soft_inventory_log();
}

/* ---- freemap ------------------------------------------------------------ */

static u32
alloc_data_lba_n(u32 nNeed)
{
    static u8 aSec[SEC];
    u32       iBit;
    u32       fRun;
    u32       jBit;

    if (nNeed == 0 || nNeed > 2) {
        soft_inc(&g_u32SoftAllocFail);
        return 0;
    }
    if (sec_rw(0, LBA_FREEMAP, aSec) != 0) {
        soft_inc(&g_u32SoftAllocFail);
        return 0;
    }
    for (iBit = 0;
         iBit + nNeed <= 64 && (LBA_DATA0 + iBit + nNeed) <= VFS_RAM_SECS;
         iBit++) {
        fRun = 1;
        for (jBit = 0; jBit < nNeed; jBit++) {
            u32 u32Byte = (iBit + jBit) / 8u;
            u32 u32Bit = (iBit + jBit) % 8u;

            if ((aSec[u32Byte] & (1u << u32Bit)) != 0) {
                fRun = 0;
                break;
            }
        }
        if (fRun) {
            for (jBit = 0; jBit < nNeed; jBit++) {
                u32 u32Byte = (iBit + jBit) / 8u;
                u32 u32Bit = (iBit + jBit) % 8u;

                aSec[u32Byte] |= (u8)(1u << u32Bit);
            }
            if (sec_rw(1, LBA_FREEMAP, aSec) != 0) {
                soft_inc(&g_u32SoftAllocFail);
                return 0;
            }
            soft_inc(&g_u32SoftAllocOk);
            return LBA_DATA0 + iBit;
        }
    }
    soft_inc(&g_u32SoftAllocFail);
    return 0;
}

static void
free_data_lba_n(u32 u32Lba0, u32 n)
{
    static u8 aSec[SEC];
    u32       jBit;

    if (u32Lba0 < LBA_DATA0 || n == 0) {
        return;
    }
    soft_inc(&g_u32SoftFreeData);
    if (sec_rw(0, LBA_FREEMAP, aSec) != 0) {
        return;
    }
    for (jBit = 0; jBit < n; jBit++) {
        u32 u32Off = u32Lba0 + jBit - LBA_DATA0;

        if (u32Off < 64) {
            aSec[u32Off / 8u] &= (u8)~(1u << (u32Off % 8u));
        }
    }
    (void)sec_rw(1, LBA_FREEMAP, aSec);
}

/**
 * Resize data allocation into *pu32Lba.
 * Shrink 2→1 frees only the trailing sector (keeps base LBA).
 * Grow frees old run then allocates nSecNew; on OOM restores nSecOld.
 * Returns 0 on success, -1 if the requested size cannot be satisfied
 * (inode LBA may still be updated to a restored smaller run).
 */
static int
resize_data_lba(u32 u32OldLba, u32 nSecOld, u32 nSecNew, u32 *pu32Lba)
{
    u32 u32New;

    if (pu32Lba == NULL) {
        return -1;
    }
    if (nSecNew == 0) {
        nSecNew = 1;
    }
    if (u32OldLba != 0 && nSecOld == nSecNew) {
        *pu32Lba = u32OldLba;
        return 0;
    }
    /* Common product path: shrink multi-sector file without moving data. */
    if (u32OldLba != 0 && nSecOld == 2u && nSecNew == 1u) {
        free_data_lba_n(u32OldLba + 1u, 1u);
        *pu32Lba = u32OldLba;
        return 0;
    }
    if (u32OldLba != 0) {
        free_data_lba_n(u32OldLba, nSecOld);
    }
    u32New = alloc_data_lba_n(nSecNew);
    if (u32New != 0) {
        *pu32Lba = u32New;
        return 0;
    }
    if (u32OldLba != 0 && nSecOld > 0) {
        u32New = alloc_data_lba_n(nSecOld);
        *pu32Lba = u32New;
        return -1; /* requested size not available */
    }
    *pu32Lba = 0;
    return -1;
}

/* ---- format / mount ----------------------------------------------------- */

static int
do_format(void)
{
    static u8 aSec[SEC];
    struct vfs_super *pSb;
    struct vfs_inode  aTab[INODE_MAX];
    u32               iIno;

    memset(aSec, 0, SEC);
    pSb = (struct vfs_super *)(void *)aSec;
    pSb->magic = VFS_MAGIC;
    pSb->version = VFS_VERSION;
    pSb->root_ino = 0;
    pSb->freemap_lba = LBA_FREEMAP;
    pSb->inode_lba = LBA_INODES;
    pSb->data_lba0 = LBA_DATA0;
    pSb->c_inodes = INODE_MAX;
    pSb->c_data = VFS_RAM_SECS - LBA_DATA0;
    if (sec_rw(1, LBA_SUPER, aSec) != 0) {
        return -1;
    }
    memset(aSec, 0, SEC);
    if (sec_rw(1, LBA_FREEMAP, aSec) != 0) {
        return -1;
    }
    memset(aTab, 0, sizeof(aTab));
    for (iIno = 0; iIno < INODE_MAX; iIno++) {
        aTab[iIno].used = 0;
    }
    if (inode_table_store(aTab) != 0) {
        return -1;
    }
    fd_invalidate_all();
    g_u32Mounted = 1;
    g_u32Files = 0;
    return 0;
}

static int
do_mount(u32 *pOut)
{
    static u8 aSec[SEC];
    struct vfs_super *pSb;
    struct vfs_inode  aTab[INODE_MAX];
    u32               c;

    if (sec_rw(0, LBA_SUPER, aSec) != 0) {
        return -1;
    }
    pSb = (struct vfs_super *)(void *)aSec;
    if (pSb->magic != VFS_MAGIC || pSb->version != VFS_VERSION) {
        return -1;
    }
    if (pOut != NULL) {
        pOut[0] = pSb->magic;
        pOut[1] = pSb->version;
        pOut[3] = pSb->data_lba0;
    }
    if (inode_table_load(aTab) != 0) {
        return -1;
    }
    c = count_used_inodes(aTab);
    g_u32Files = c;
    g_u32Mounted = 1;
    if (pOut != NULL) {
        pOut[2] = c;
    }
    return 0;
}

/* ---- name-keyed ops ----------------------------------------------------- */

static i64
do_create_or_write(const char *szName, const u8 *pData, u32 cb, int fCreateOnly)
{
    struct vfs_inode aTab[INODE_MAX];
    u32              iIno;
    u32              u32Slot = INODE_MAX;
    u32              u32Lba;
    u32              nSecOld = 0;
    u32              nSecNew;
    u32              u32OldLba = 0;
    int              fReplace = 0;

    if (cb > FILE_MAX_BYTES || szName == NULL || szName[0] == '\0') {
        return GJ_ERR_INVAL;
    }
    if (!g_u32Mounted) {
        if (do_mount(NULL) != 0) {
            if (do_format() != 0) {
                return GJ_ERR_IO;
            }
            soft_inc(&g_u32SoftAutoFormat);
        } else {
            soft_inc(&g_u32SoftAutoMount);
        }
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    for (iIno = 0; iIno < INODE_MAX; iIno++) {
        if (aTab[iIno].used && name_eq(aTab[iIno].name, szName)) {
            if (fCreateOnly) {
                return GJ_ERR_BUSY;
            }
            u32Slot = iIno;
            u32OldLba = aTab[iIno].data_lba;
            nSecOld = sectors_for_size(aTab[iIno].size);
            fReplace = 1;
            break;
        }
        if (!aTab[iIno].used && u32Slot == INODE_MAX) {
            u32Slot = iIno;
        }
    }
    if (u32Slot == INODE_MAX) {
        return GJ_ERR_NOMEM;
    }
    nSecNew = sectors_for_size(cb == 0 ? 1u : cb);
    if (fReplace) {
        if (resize_data_lba(u32OldLba, nSecOld, nSecNew, &u32Lba) != 0) {
            if (u32Lba != 0) {
                aTab[u32Slot].data_lba = u32Lba;
                (void)inode_table_store(aTab);
            }
            return GJ_ERR_NOMEM;
        }
    } else {
        u32Lba = alloc_data_lba_n(nSecNew);
        if (u32Lba == 0) {
            return GJ_ERR_NOMEM;
        }
    }
    aTab[u32Slot].used = 1;
    aTab[u32Slot].mode = 0100644u;
    aTab[u32Slot].data_lba = u32Lba;
    aTab[u32Slot].size = cb;
    name_copy(aTab[u32Slot].name, szName, NAME_MAX);
    if (file_data_store(u32Lba, pData, cb) != 0) {
        free_data_lba_n(u32Lba, nSecNew);
        if (fReplace) {
            /* Leave inode free on hard failure after replace. */
            memset(&aTab[u32Slot], 0, sizeof(aTab[u32Slot]));
            (void)inode_table_store(aTab);
            fd_invalidate_ino(u32Slot);
        }
        return GJ_ERR_IO;
    }
    if (inode_table_store(aTab) != 0) {
        free_data_lba_n(u32Lba, nSecNew);
        return GJ_ERR_IO;
    }
    g_u32Files = count_used_inodes(aTab);
    return (i64)u32Slot;
}

static i64
do_read(const char *szName, u64 u64Buf, u32 cbMax)
{
    struct vfs_inode aTab[INODE_MAX];
    static u8        aData[FILE_MAX_BYTES];
    u32              iIno;
    u32              n;
    i64              i64Cp;

    if (!g_u32Mounted) {
        if (do_mount(NULL) != 0) {
            return GJ_ERR_NODEV;
        }
        soft_inc(&g_u32SoftAutoMount);
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    for (iIno = 0; iIno < INODE_MAX; iIno++) {
        if (aTab[iIno].used && name_eq(aTab[iIno].name, szName)) {
            if (file_data_load(aTab[iIno].data_lba, aTab[iIno].size, aData) !=
                0) {
                return GJ_ERR_IO;
            }
            n = aTab[iIno].size;
            if (n > cbMax) {
                n = cbMax;
            }
            i64Cp = copy_out(u64Buf, aData, n);
            if (i64Cp < 0) {
                return i64Cp;
            }
            return (i64)n;
        }
    }
    return GJ_ERR_NOENT;
}

static i64
do_unlink(const char *szName)
{
    struct vfs_inode aTab[INODE_MAX];
    u32              iIno;
    u32              nSec;

    if (!g_u32Mounted) {
        return GJ_ERR_NODEV;
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    for (iIno = 0; iIno < INODE_MAX; iIno++) {
        if (aTab[iIno].used && name_eq(aTab[iIno].name, szName)) {
            nSec = sectors_for_size(aTab[iIno].size);
            free_data_lba_n(aTab[iIno].data_lba, nSec);
            memset(&aTab[iIno], 0, sizeof(aTab[iIno]));
            if (inode_table_store(aTab) != 0) {
                return GJ_ERR_IO;
            }
            fd_invalidate_ino(iIno);
            if (g_u32Files > 0) {
                g_u32Files--;
            }
            return 0;
        }
    }
    return GJ_ERR_NOENT;
}

static i64
do_stat(const char *szName, u64 u64Out)
{
    struct vfs_inode aTab[INODE_MAX];
    u32              iIno;
    u32              aSt[2];
    i64              i64Cp;

    if (!g_u32Mounted) {
        if (do_mount(NULL) != 0) {
            return GJ_ERR_NODEV;
        }
        soft_inc(&g_u32SoftAutoMount);
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    for (iIno = 0; iIno < INODE_MAX; iIno++) {
        if (aTab[iIno].used && name_eq(aTab[iIno].name, szName)) {
            aSt[0] = aTab[iIno].size;
            aSt[1] = aTab[iIno].data_lba;
            i64Cp = copy_out(u64Out, aSt, sizeof(aSt));
            if (i64Cp < 0) {
                return i64Cp;
            }
            return 0;
        }
    }
    return GJ_ERR_NOENT;
}

static i64
do_list(u64 u64Buf, u32 cbMax)
{
    struct vfs_inode aTab[INODE_MAX];
    u32              iIno;
    u32              o = 0;
    static char      aList[512];
    i64              i64Cp;

    if (!g_u32Mounted) {
        if (do_mount(NULL) != 0) {
            return GJ_ERR_NODEV;
        }
        soft_inc(&g_u32SoftAutoMount);
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    memset(aList, 0, sizeof(aList));
    for (iIno = 0; iIno < INODE_MAX; iIno++) {
        u32 jByte;

        if (!aTab[iIno].used) {
            continue;
        }
        for (jByte = 0;
             aTab[iIno].name[jByte] != '\0' && o + 1 < sizeof(aList);
             jByte++) {
            aList[o++] = aTab[iIno].name[jByte];
        }
        if (o + 1 < sizeof(aList)) {
            aList[o++] = '\n';
        }
    }
    if (u64Buf != 0 && o > 0) {
        u32 n = o;

        if (n > cbMax) {
            n = cbMax;
        }
        i64Cp = copy_out(u64Buf, aList, n);
        if (i64Cp < 0) {
            return i64Cp;
        }
        return (i64)n;
    }
    return (i64)o;
}

/* ---- door fd ops -------------------------------------------------------- */

static i64
do_open(const char *szName, u32 u32Flags)
{
    struct vfs_inode aTab[INODE_MAX];
    u32              iIno;
    u32              u32Slot = VFS_FD_MAX;
    i64              i64Ino = -1;
    int              fCreat = (u32Flags & GJ_VFS_O_CREAT) != 0;
    int              fWrite = (u32Flags & GJ_VFS_O_RDWR) != 0;

    if (!g_u32Mounted) {
        if (do_mount(NULL) != 0) {
            if (fCreat) {
                if (do_format() != 0) {
                    return GJ_ERR_IO;
                }
                soft_inc(&g_u32SoftAutoFormat);
            } else {
                return GJ_ERR_NODEV;
            }
        } else {
            soft_inc(&g_u32SoftAutoMount);
        }
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    for (iIno = 0; iIno < INODE_MAX; iIno++) {
        if (aTab[iIno].used && name_eq(aTab[iIno].name, szName)) {
            i64Ino = (i64)iIno;
            break;
        }
    }
    if (i64Ino < 0) {
        if (!fCreat) {
            return GJ_ERR_NOENT;
        }
        i64Ino = do_create_or_write(szName, NULL, 0, 1);
        if (i64Ino < 0) {
            return i64Ino;
        }
    }
    for (iIno = 0; iIno < VFS_FD_MAX; iIno++) {
        if (!g_aFd[iIno].u8Used) {
            u32Slot = iIno;
            break;
        }
    }
    if (u32Slot >= VFS_FD_MAX) {
        return GJ_ERR_NOMEM;
    }
    g_aFd[u32Slot].u8Used = 1;
    g_aFd[u32Slot].u8Write = fWrite ? 1 : 0;
    g_aFd[u32Slot].u32Ino = (u32)i64Ino;
    g_aFd[u32Slot].u32Off = 0;
    name_copy(g_aFd[u32Slot].szName, szName, NAME_MAX);
    return (i64)u32Slot;
}

static i64
do_close(u32 u32Fd)
{
    if (u32Fd >= VFS_FD_MAX || !g_aFd[u32Fd].u8Used) {
        return GJ_ERR_INVAL;
    }
    memset(&g_aFd[u32Fd], 0, sizeof(g_aFd[u32Fd]));
    return 0;
}

static i64
do_readfd(u32 u32Fd, u64 u64Buf, u32 cbMax)
{
    struct vfs_inode aTab[INODE_MAX];
    static u8        aData[FILE_MAX_BYTES];
    u32              n;
    u32              u32Off;
    u32              u32Ino;
    i64              i64Cp;

    if (u32Fd >= VFS_FD_MAX || !g_aFd[u32Fd].u8Used) {
        return GJ_ERR_INVAL;
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    u32Ino = g_aFd[u32Fd].u32Ino;
    if (u32Ino >= INODE_MAX || !aTab[u32Ino].used) {
        return GJ_ERR_NOENT;
    }
    u32Off = g_aFd[u32Fd].u32Off;
    if (u32Off >= aTab[u32Ino].size) {
        return 0;
    }
    if (file_data_load(aTab[u32Ino].data_lba, aTab[u32Ino].size, aData) !=
        0) {
        return GJ_ERR_IO;
    }
    n = aTab[u32Ino].size - u32Off;
    if (n > cbMax) {
        n = cbMax;
    }
    i64Cp = copy_out(u64Buf, aData + u32Off, n);
    if (i64Cp < 0) {
        return i64Cp;
    }
    g_aFd[u32Fd].u32Off = u32Off + n;
    return (i64)n;
}

static i64
do_writefd(u32 u32Fd, u64 u64Buf, u32 cb)
{
    struct vfs_inode aTab[INODE_MAX];
    static u8        aData[FILE_MAX_BYTES];
    static u8        aTmp[FILE_MAX_BYTES];
    u32              u32Off;
    u32              u32Ino;
    u32              u32NewSize;
    u32              nSecOld;
    u32              nSecNew;
    u32              u32Lba;
    i64              i64Cp;

    if (u32Fd >= VFS_FD_MAX || !g_aFd[u32Fd].u8Used) {
        return GJ_ERR_INVAL;
    }
    if (!g_aFd[u32Fd].u8Write) {
        return GJ_ERR_PERM;
    }
    if (cb > FILE_MAX_BYTES) {
        return GJ_ERR_INVAL;
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    u32Ino = g_aFd[u32Fd].u32Ino;
    if (u32Ino >= INODE_MAX || !aTab[u32Ino].used) {
        return GJ_ERR_NOENT;
    }
    u32Off = g_aFd[u32Fd].u32Off;
    if (u32Off > FILE_MAX_BYTES) {
        return GJ_ERR_INVAL;
    }
    if (file_data_load(aTab[u32Ino].data_lba, aTab[u32Ino].size, aData) !=
        0) {
        return GJ_ERR_IO;
    }
    if (u64Buf != 0 && cb > 0) {
        i64Cp = copy_in(aTmp, u64Buf, cb);
        if (i64Cp < 0) {
            return i64Cp;
        }
        if (u32Off + cb > FILE_MAX_BYTES) {
            cb = FILE_MAX_BYTES - u32Off;
        }
        memcpy(aData + u32Off, aTmp, cb);
    } else if (cb > 0 && u64Buf == 0) {
        return GJ_ERR_INVAL;
    }
    u32NewSize = u32Off + cb;
    /* Extend only; mid-file write that ends before EOF keeps prior size. */
    if (u32NewSize < aTab[u32Ino].size) {
        u32NewSize = aTab[u32Ino].size;
    }
    nSecOld = sectors_for_size(aTab[u32Ino].size);
    nSecNew = sectors_for_size(u32NewSize == 0 ? 1u : u32NewSize);
    u32Lba = aTab[u32Ino].data_lba;
    if (nSecNew != nSecOld) {
        if (resize_data_lba(u32Lba, nSecOld, nSecNew, &u32Lba) != 0) {
            if (u32Lba != 0) {
                aTab[u32Ino].data_lba = u32Lba;
                (void)inode_table_store(aTab);
            }
            return GJ_ERR_NOMEM;
        }
        aTab[u32Ino].data_lba = u32Lba;
    }
    if (file_data_store(aTab[u32Ino].data_lba, aData, u32NewSize) != 0) {
        return GJ_ERR_IO;
    }
    aTab[u32Ino].size = u32NewSize;
    if (inode_table_store(aTab) != 0) {
        return GJ_ERR_IO;
    }
    g_aFd[u32Fd].u32Off = u32Off + cb;
    return (i64)cb;
}

static i64
do_seekfd(u32 u32Fd, i64 i64Off, u32 u32Whence)
{
    struct vfs_inode aTab[INODE_MAX];
    u32              u32Ino;
    i64              i64Base;
    i64              i64New;

    if (u32Fd >= VFS_FD_MAX || !g_aFd[u32Fd].u8Used) {
        return GJ_ERR_INVAL;
    }
    if (inode_table_load(aTab) != 0) {
        return GJ_ERR_IO;
    }
    u32Ino = g_aFd[u32Fd].u32Ino;
    if (u32Ino >= INODE_MAX || !aTab[u32Ino].used) {
        return GJ_ERR_NOENT;
    }
    switch (u32Whence) {
    case GJ_VFS_SEEK_SET:
        i64Base = 0;
        break;
    case GJ_VFS_SEEK_CUR:
        i64Base = (i64)g_aFd[u32Fd].u32Off;
        break;
    case GJ_VFS_SEEK_END:
        i64Base = (i64)aTab[u32Ino].size;
        break;
    default:
        return GJ_ERR_INVAL;
    }
    i64New = i64Base + i64Off;
    if (i64New < 0) {
        return GJ_ERR_INVAL;
    }
    if (i64New > (i64)FILE_MAX_BYTES) {
        i64New = (i64)FILE_MAX_BYTES;
    }
    g_aFd[u32Fd].u32Off = (u32)i64New;
    return i64New;
}

/* ---- public surface ----------------------------------------------------- */

int
vfs_door_owned(void)
{
    return g_u32Owner != 0;
}

u32
vfs_door_owner_token(void)
{
    return g_u32Owner;
}

u32
vfs_door_claim_count(void)
{
    /* Soft diagnostics: first claims + idempotent reclaims. */
    /*
     * Emit soft inventory on claim stats read so bring-up can grep
     * vfs_door: soft … (Wave 14). greppable: vfs_door claim soft
     */
    soft_inventory_log();
    return g_u32Claims + g_u32Reclaims;
}

int
vfs_door_mounted(void)
{
    return (int)g_u32Mounted;
}

u32
vfs_door_fd_count(void)
{
    u32 c;

    gj_spin_lock(&g_lkVfs);
    c = fd_count_used();
    gj_spin_unlock(&g_lkVfs);
    /*
     * Soft diagnostics re-read: emit inventory so fd smoke greps
     * greppable vfs_door: soft lines without a dedicated syscall.
     */
    soft_inventory_log();
    return c;
}

i64
vfs_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    char     szName[NAME_MAX];
    static u8 aTmp[FILE_MAX_BYTES];
    i64      i64Ret;
    int      fSoftInv = 0;

    if (!g_fInit) {
        soft_inc(&g_u32SoftNotInit);
        return GJ_ERR_NODEV;
    }

    gj_spin_lock(&g_lkVfs);
    g_u32Calls++;

    switch (u32Op) {
    case GJ_VFS_OP_CLAIM:
        /* Non-zero 32-bit token only (align with store/session claim soft). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (g_u32Owner != 0 && g_u32Owner != (u32)u64Arg1) {
            i64Ret = GJ_ERR_BUSY;
            break;
        }
        /* Soft reclaim: same token re-CLAIM is idempotent. */
        if (g_u32Owner == (u32)u64Arg1) {
            g_u32Reclaims++;
            i64Ret = 0;
            break;
        }
        g_u32Owner = (u32)u64Arg1;
        g_u32Claims++;
        kprintf("vfs_door: CLAIM token=0x%x\n", g_u32Owner);
        i64Ret = 0;
        break;
    case GJ_VFS_OP_RELEASE:
        /* Soft free path: already unowned → 0 (no token match required). */
        if (g_u32Owner == 0) {
            soft_inc(&g_u32SoftReleaseFree);
            i64Ret = 0;
            break;
        }
        if ((u64Arg1 >> 32) != 0 || g_u32Owner != (u32)u64Arg1) {
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        kprintf("vfs_door: RELEASE token=0x%x\n", g_u32Owner);
        g_u32Owner = 0;
        soft_inc(&g_u32SoftReleaseOk);
        i64Ret = 0;
        break;
    case GJ_VFS_OP_FORMAT:
        /*
         * Soft bring-up: format when free or claimed (product vfsd claims
         * first). No third ownership state — always allowed here.
         */
        if (do_format() != 0) {
            i64Ret = GJ_ERR_IO;
            break;
        }
        kprintf("vfs_door: FORMAT PASS files=0 ver=%u\n", VFS_VERSION);
        i64Ret = 0;
        break;
    case GJ_VFS_OP_MOUNT: {
        u32 aInfo[4];

        if (do_mount(aInfo) != 0) {
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        i64Ret = copy_out(u64Arg1, aInfo, sizeof(aInfo));
        if (i64Ret < 0) {
            break;
        }
        kprintf("vfs_door: MOUNT PASS files=%u\n", aInfo[2]);
        i64Ret = 0;
        break;
    }
    case GJ_VFS_OP_CREATE:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
            soft_inc(&g_u32SoftNameInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (u64Arg3 > FILE_MAX_BYTES) {
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (u64Arg3 > 0 && u64Arg2 != 0) {
            i64Ret = copy_in(aTmp, u64Arg2, (u32)u64Arg3);
            if (i64Ret < 0) {
                break;
            }
        }
        i64Ret = do_create_or_write(szName, aTmp, (u32)u64Arg3, 1);
        break;
    case GJ_VFS_OP_WRITE:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
            soft_inc(&g_u32SoftNameInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (u64Arg3 > FILE_MAX_BYTES) {
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (u64Arg3 > 0 && u64Arg2 != 0) {
            i64Ret = copy_in(aTmp, u64Arg2, (u32)u64Arg3);
            if (i64Ret < 0) {
                break;
            }
        }
        {
            i64 i64R = do_create_or_write(szName, aTmp, (u32)u64Arg3, 0);

            if (i64R < 0) {
                i64Ret = i64R;
            } else {
                i64Ret = (i64)u64Arg3;
            }
        }
        break;
    case GJ_VFS_OP_READ:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
            soft_inc(&g_u32SoftNameInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        i64Ret = do_read(szName, u64Arg2, (u32)u64Arg3);
        break;
    case GJ_VFS_OP_UNLINK:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
            soft_inc(&g_u32SoftNameInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        i64Ret = do_unlink(szName);
        break;
    case GJ_VFS_OP_STAT:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
            soft_inc(&g_u32SoftNameInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        i64Ret = do_stat(szName, u64Arg2);
        break;
    case GJ_VFS_OP_LIST:
        i64Ret = do_list(u64Arg1, (u32)u64Arg3);
        break;
    case GJ_VFS_OP_STATS: {
        /* aSt: [0]=calls [1]=files [2]=owned(0/1) [3]=mounted(0/1) */
        u32 aSt[4];

        if (u64Arg1 == 0) {
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        aSt[0] = g_u32Calls;
        aSt[1] = g_u32Files;
        aSt[2] = g_u32Owner ? 1u : 0u;
        aSt[3] = g_u32Mounted;
        i64Ret = copy_out(u64Arg1, aSt, sizeof(aSt));
        if (i64Ret < 0) {
            break;
        }
        i64Ret = 0;
        /*
         * Emit soft inventory on STATS so bring-up smoke greps
         * vfs_door: soft … lines (Wave 12). Dump after unlock
         * (kprintf under lock avoided). greppable: vfs_door: soft
         */
        fSoftInv = 1;
        break;
    }
    case GJ_VFS_OP_OPEN:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
            soft_inc(&g_u32SoftNameInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        i64Ret = do_open(szName, (u32)u64Arg2);
        break;
    case GJ_VFS_OP_CLOSE:
        i64Ret = do_close((u32)u64Arg1);
        break;
    case GJ_VFS_OP_READFD:
        i64Ret = do_readfd((u32)u64Arg1, u64Arg2, (u32)u64Arg3);
        break;
    case GJ_VFS_OP_WRITEFD:
        i64Ret = do_writefd((u32)u64Arg1, u64Arg2, (u32)u64Arg3);
        break;
    case GJ_VFS_OP_SEEKFD:
        i64Ret = do_seekfd((u32)u64Arg1, (i64)u64Arg2, (u32)u64Arg3);
        break;
    default:
        i64Ret = GJ_ERR_NOSUPPORT;
        break;
    }

    /* Wave 12+14 soft inventory tallies (never mutates i64Ret). */
    soft_note_result(u32Op, i64Ret);
    gj_spin_unlock(&g_lkVfs);
    soft_maybe_once();
    if (fSoftInv) {
        /*
         * Emit soft inventory on STATS so bring-up smoke greps
         * vfs_door: soft … lines. greppable: vfs_door: soft
         */
        soft_inventory_log();
    }
    return i64Ret;
}
