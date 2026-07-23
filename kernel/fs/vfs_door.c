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

static int do_format(void);
static int do_mount(u32 *pOut);
static i64 do_create_or_write(const char *szName, const u8 *pData, u32 cb,
                              int fCreateOnly);

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
        return -1;
    }
    if (g_fRam || !virtio_blk_ready()) {
        if (fWrite) {
            memcpy(g_aRam[u32Lba], p, SEC);
        } else {
            memcpy(p, g_aRam[u32Lba], SEC);
        }
        return 0;
    }
    if (fWrite) {
        i64R = store_door_call(GJ_STORE_OP_WRITE, u32Lba, (u64)(gj_vaddr_t)p,
                               SEC);
    } else {
        i64R =
            store_door_call(GJ_STORE_OP_READ, u32Lba, (u64)(gj_vaddr_t)p, SEC);
    }
    return (i64R == (i64)SEC) ? 0 : -1;
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
    gj_spin_init(&g_lkVfs);
    kprintf("vfs_door: init ver=%u ram=%d inodes=%u file_max=%u secs=%u\n",
            VFS_VERSION, g_fRam, INODE_MAX, FILE_MAX_BYTES, VFS_RAM_SECS);
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
        return 0;
    }
    if (sec_rw(0, LBA_FREEMAP, aSec) != 0) {
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
                return 0;
            }
            return LBA_DATA0 + iBit;
        }
    }
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
    if (!g_u32Mounted && do_mount(NULL) != 0) {
        if (do_format() != 0) {
            return GJ_ERR_IO;
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

    if (!g_u32Mounted && do_mount(NULL) != 0) {
        return GJ_ERR_NODEV;
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

    if (!g_u32Mounted && do_mount(NULL) != 0) {
        return GJ_ERR_NODEV;
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

    if (!g_u32Mounted && do_mount(NULL) != 0) {
        return GJ_ERR_NODEV;
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

    if (!g_u32Mounted && do_mount(NULL) != 0) {
        if (fCreat) {
            if (do_format() != 0) {
                return GJ_ERR_IO;
            }
        } else {
            return GJ_ERR_NODEV;
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
    return c;
}

i64
vfs_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    char     szName[NAME_MAX];
    static u8 aTmp[FILE_MAX_BYTES];
    i64      i64Ret;

    if (!g_fInit) {
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
            i64Ret = 0;
            break;
        }
        if ((u64Arg1 >> 32) != 0 || g_u32Owner != (u32)u64Arg1) {
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        kprintf("vfs_door: RELEASE token=0x%x\n", g_u32Owner);
        g_u32Owner = 0;
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
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        i64Ret = do_read(szName, u64Arg2, (u32)u64Arg3);
        break;
    case GJ_VFS_OP_UNLINK:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        i64Ret = do_unlink(szName);
        break;
    case GJ_VFS_OP_STAT:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
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
        break;
    }
    case GJ_VFS_OP_OPEN:
        if (copy_name_from_user(szName, u64Arg1) != 0) {
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

    gj_spin_unlock(&g_lkVfs);
    return i64Ret;
}
