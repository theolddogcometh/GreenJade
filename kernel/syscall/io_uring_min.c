/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * io_uring rings + SQE I/O + SQ/CQ mmap (userspace fill without inject).
 * Clean-room public Linux ABI shapes. No GPL source.
 *
 * Layout (IORING_FEAT_SINGLE_MMAP-shaped package, page-aligned):
 *   +0x0000  SQ: head,tail,mask,entries,flags,dropped + array[]
 *   +0x1000  CQ: head,tail,mask,entries,overflow,flags + cqes[]
 *   +0x2000  SQEs[entries]
 *
 * Userspace: mmap(fd, …) → package VA; fill SQEs + advance sq.tail; enter.
 * Kernel smoke: gj_io_uring_mmap_package() returns package pointer.
 *
 * Greppable: io_uring: setup …, linux: io_uring min rings PASS,
 *            linux: io_uring SQE I/O PASS, linux: io_uring mmap PASS
 */
#include <gj/error.h>
#include <gj/io_uring.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/linux_dispatch.h>
#include <gj/memobj.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/vfs_ram.h>
#include <gj/vmm.h>

#define GJ_IORING_MAX       4u
#define GJ_IORING_ENTRIES   64u
#define GJ_IORING_SQE_SIZE  64u
#define GJ_IORING_CQE_SIZE  16u
#define GJ_IORING_IO_MAX    4096u
#define GJ_IORING_PAGE      4096u

#define IORING_REGISTER_BUFFERS  0u
#define IORING_REGISTER_FILES    2u
#define IORING_UNREGISTER_FILES  3u
#define IORING_REGISTER_EVENTFD  4u

#define IORING_FEAT_SINGLE_MMAP  (1u << 0)

#define IORING_OP_NOP       0u
#define IORING_OP_READV     1u
#define IORING_OP_WRITEV    2u
#define IORING_OP_FSYNC     3u
#define IORING_OP_CLOSE     19u
#define IORING_OP_READ      22u
#define IORING_OP_WRITE     23u

/* mmap offsets (public Linux uapi) */
#define IORING_OFF_SQ_RING  0ull
#define IORING_OFF_CQ_RING  0x8000000ull
#define IORING_OFF_SQES     0x10000000ull

/* Package page offsets */
#define PKG_OFF_SQ    0x0000u
#define PKG_OFF_CQ    0x1000u
#define PKG_OFF_SQES  0x2000u
#define PKG_PAGES     3u
#define PKG_SIZE      (PKG_PAGES * GJ_IORING_PAGE)

struct gj_io_sqring_offsets {
    u32 head;
    u32 tail;
    u32 ring_mask;
    u32 ring_entries;
    u32 flags;
    u32 dropped;
    u32 array;
    u32 resv1;
    u64 user_addr;
};

struct gj_io_cqring_offsets {
    u32 head;
    u32 tail;
    u32 ring_mask;
    u32 ring_entries;
    u32 overflow;
    u32 cqes;
    u32 flags;
    u32 resv1;
    u64 user_addr;
};

struct gj_io_uring_params {
    u32 sq_entries;
    u32 cq_entries;
    u32 flags;
    u32 sq_thread_cpu;
    u32 sq_thread_idle;
    u32 features;
    u32 wq_fd;
    u32 resv[3];
    struct gj_io_sqring_offsets sq_off;
    struct gj_io_cqring_offsets cq_off;
};

struct gj_io_uring_sqe {
    u8  u8Opcode;
    u8  u8Flags;
    u16 u16Ioprio;
    i32 i32Fd;
    u64 u64Off;
    u64 u64Addr;
    u32 u32Len;
    u32 u32OpFlags;
    u64 u64UserData;
    u16 u16BufIndex;
    u16 u16Personality;
    u32 u32FileIndex;
    u64 u64Pad2[2];
};

struct gj_io_uring_cqe {
    u64 u64UserData;
    i32 i32Res;
    u32 u32Flags;
};

struct gj_iovec {
    u64 u64Base;
    u64 u64Len;
};

/* User-visible SQ control (matches sq_off field order). */
struct gj_io_sq_ctrl {
    u32 head;
    u32 tail;
    u32 ring_mask;
    u32 ring_entries;
    u32 flags;
    u32 dropped;
    u32 resv[2];
    u32 array[GJ_IORING_ENTRIES]; /* at offset 32; params say 64 — pad */
};

/* Prefer array at 64 as advertised: pad after dropped. */
struct gj_io_sq_ctrl_pkg {
    u32 head;
    u32 tail;
    u32 ring_mask;
    u32 ring_entries;
    u32 flags;
    u32 dropped;
    u32 pad[10]; /* pad so array starts at offset 64 */
    u32 array[GJ_IORING_ENTRIES];
};

struct gj_io_cq_ctrl_pkg {
    u32 head;
    u32 tail;
    u32 ring_mask;
    u32 ring_entries;
    u32 overflow;
    u32 flags;
    u32 pad[10]; /* cqes at offset 64 */
    struct gj_io_uring_cqe cqes[GJ_IORING_ENTRIES * 2u];
};

struct gj_io_uring_ring {
    u8  u8Used;
    u8  u8Mapped; /* package exposed / mmap ready */
    u8  u8Pad[2];
    u32 u32SqEntries;
    u32 u32CqEntries;
    u32 u32Dropped;
    u32 u32Overflow;
    u32 u32SqeIoOk;
    /* Kernel package (page-aligned). */
    u8  aPkg[PKG_SIZE] __attribute__((aligned(4096)));
    u64 u64UserMapVa; /* 0 if only kernel-visible; else user VA of package */
};

_Static_assert(sizeof(struct gj_io_uring_sqe) == 64, "sqe 64");
_Static_assert(sizeof(struct gj_io_uring_cqe) == 16, "cqe 16");

static struct gj_io_uring_ring g_aRing[GJ_IORING_MAX];
static i64 g_aRingFd[GJ_IORING_MAX];
static int g_fInited;

/* ---- package accessors ------------------------------------------------- */

static struct gj_io_sq_ctrl_pkg *
sq_ctrl(struct gj_io_uring_ring *pR)
{
    return (struct gj_io_sq_ctrl_pkg *)(void *)&pR->aPkg[PKG_OFF_SQ];
}

static struct gj_io_cq_ctrl_pkg *
cq_ctrl(struct gj_io_uring_ring *pR)
{
    return (struct gj_io_cq_ctrl_pkg *)(void *)&pR->aPkg[PKG_OFF_CQ];
}

static struct gj_io_uring_sqe *
sqes(struct gj_io_uring_ring *pR)
{
    return (struct gj_io_uring_sqe *)(void *)&pR->aPkg[PKG_OFF_SQES];
}

static void
pkg_init(struct gj_io_uring_ring *pR, u32 nSq, u32 nCq)
{
    struct gj_io_sq_ctrl_pkg *pSq;
    struct gj_io_cq_ctrl_pkg *pCq;
    u32 i;

    memset(pR->aPkg, 0, sizeof(pR->aPkg));
    pSq = sq_ctrl(pR);
    pCq = cq_ctrl(pR);
    pSq->ring_mask = nSq - 1u;
    pSq->ring_entries = nSq;
    pCq->ring_mask = nCq - 1u;
    pCq->ring_entries = nCq;
    for (i = 0; i < nSq; i++) {
        pSq->array[i] = i;
    }
    pR->u8Mapped = 1; /* package always ready; user mmap optional */
}

void
gj_io_uring_init(void)
{
    u32 i;

    memset(g_aRing, 0, sizeof(g_aRing));
    for (i = 0; i < GJ_IORING_MAX; i++) {
        g_aRingFd[i] = -1;
    }
    g_fInited = 1;
    kprintf("io_uring: rings ready pool=%u depth<=%u SQE+I/O+mmap\n",
            GJ_IORING_MAX, GJ_IORING_ENTRIES);
}

static u32
pow2_clamp(u32 n, u32 max)
{
    u32 p;

    if (n < 1u) {
        n = 1u;
    }
    if (n > max) {
        n = max;
    }
    p = 1u;
    while (p < n) {
        p <<= 1;
    }
    return p;
}

static struct gj_io_uring_ring *
ring_from_fd(i64 i64Fd)
{
    u32 i;

    if (!g_fInited || i64Fd < 0) {
        return NULL;
    }
    for (i = 0; i < GJ_IORING_MAX; i++) {
        if (g_aRing[i].u8Used && g_aRingFd[i] == i64Fd) {
            return &g_aRing[i];
        }
    }
    {
        u32 idx = 0;

        if (vfs_ram_io_uring_fd(i64Fd, &idx) && idx < GJ_IORING_MAX &&
            g_aRing[idx].u8Used) {
            g_aRingFd[idx] = i64Fd;
            return &g_aRing[idx];
        }
    }
    return NULL;
}

int
gj_io_uring_fd_ok(i64 i64Fd)
{
    return ring_from_fd(i64Fd) != NULL;
}

static i64
buf_in(u64 u64Addr, void *pKdst, size_t cb)
{
    if (pKdst == NULL || cb == 0u) {
        return 0;
    }
    if (user_range_ok(u64Addr, cb)) {
        if (copy_from_user(pKdst, u64Addr, cb) != GJ_OK) {
            return -LINUX_EFAULT;
        }
        return (i64)cb;
    }
    memcpy(pKdst, (const void *)(gj_vaddr_t)u64Addr, cb);
    return (i64)cb;
}

static i64
buf_out(u64 u64Addr, const void *pKsrc, size_t cb)
{
    if (pKsrc == NULL || cb == 0u) {
        return 0;
    }
    if (user_range_ok(u64Addr, cb)) {
        if (copy_to_user(u64Addr, pKsrc, cb) != GJ_OK) {
            return -LINUX_EFAULT;
        }
        return (i64)cb;
    }
    memcpy((void *)(gj_vaddr_t)u64Addr, pKsrc, cb);
    return (i64)cb;
}

static i32
exec_sqe(struct gj_io_uring_ring *pR, const struct gj_io_uring_sqe *pSqe)
{
    u8 aBounce[GJ_IORING_IO_MAX];
    i64 i64N;
    u32 u32Len;
    i32 i32Fd;
    u64 u64Off;
    u64 u64Addr;

    if (pSqe == NULL) {
        return -LINUX_EINVAL;
    }
    i32Fd = pSqe->i32Fd;
    u64Off = pSqe->u64Off;
    u64Addr = pSqe->u64Addr;
    u32Len = pSqe->u32Len;

    switch (pSqe->u8Opcode) {
    case IORING_OP_NOP:
        return 0;
    case IORING_OP_FSYNC:
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        return 0;
    case IORING_OP_CLOSE:
        return (i32)vfs_ram_close((i64)i32Fd);
    case IORING_OP_READ:
    case IORING_OP_WRITE:
        if (u32Len == 0u) {
            return 0;
        }
        if (u32Len > GJ_IORING_IO_MAX) {
            u32Len = GJ_IORING_IO_MAX;
        }
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        if (pSqe->u8Opcode == IORING_OP_READ) {
            i64N = vfs_ram_pread((i64)i32Fd, aBounce, u32Len, u64Off);
            if (i64N < 0) {
                return (i32)i64N;
            }
            if (buf_out(u64Addr, aBounce, (size_t)i64N) < 0) {
                return -LINUX_EFAULT;
            }
            if (pR != NULL && i64N > 0) {
                pR->u32SqeIoOk++;
            }
            return (i32)i64N;
        }
        i64N = buf_in(u64Addr, aBounce, u32Len);
        if (i64N < 0) {
            return (i32)i64N;
        }
        i64N = vfs_ram_pwrite((i64)i32Fd, aBounce, (size_t)i64N, u64Off);
        if (i64N > 0 && pR != NULL) {
            pR->u32SqeIoOk++;
        }
        return (i32)i64N;
    case IORING_OP_READV:
    case IORING_OP_WRITEV: {
        struct gj_iovec iov;

        if (u32Len < 1u) {
            return -LINUX_EINVAL;
        }
        memset(&iov, 0, sizeof(iov));
        if (user_range_ok(u64Addr, sizeof(iov))) {
            if (copy_from_user(&iov, u64Addr, sizeof(iov)) != GJ_OK) {
                return -LINUX_EFAULT;
            }
        } else {
            memcpy(&iov, (const void *)(gj_vaddr_t)u64Addr, sizeof(iov));
        }
        if (iov.u64Len == 0ull) {
            return 0;
        }
        if (iov.u64Len > (u64)GJ_IORING_IO_MAX) {
            iov.u64Len = GJ_IORING_IO_MAX;
        }
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        if (pSqe->u8Opcode == IORING_OP_READV) {
            i64N = vfs_ram_pread((i64)i32Fd, aBounce, (size_t)iov.u64Len,
                                 u64Off);
            if (i64N < 0) {
                return (i32)i64N;
            }
            if (buf_out(iov.u64Base, aBounce, (size_t)i64N) < 0) {
                return -LINUX_EFAULT;
            }
            if (pR != NULL && i64N > 0) {
                pR->u32SqeIoOk++;
            }
            return (i32)i64N;
        }
        i64N = buf_in(iov.u64Base, aBounce, (size_t)iov.u64Len);
        if (i64N < 0) {
            return (i32)i64N;
        }
        i64N = vfs_ram_pwrite((i64)i32Fd, aBounce, (size_t)i64N, u64Off);
        if (i64N > 0 && pR != NULL) {
            pR->u32SqeIoOk++;
        }
        return (i32)i64N;
    }
    default:
        return -LINUX_EINVAL;
    }
}

static void
cqe_post(struct gj_io_uring_ring *pR, u64 u64UserData, i32 i32Res)
{
    struct gj_io_cq_ctrl_pkg *pCq;
    u32 mask;
    u32 pending;
    u32 idx;

    if (pR == NULL) {
        return;
    }
    pCq = cq_ctrl(pR);
    mask = pCq->ring_mask;
    pending = pCq->tail - pCq->head;
    if (pending >= pCq->ring_entries) {
        pCq->overflow++;
        pR->u32Overflow++;
        return;
    }
    idx = pCq->tail & mask;
    pCq->cqes[idx].u64UserData = u64UserData;
    pCq->cqes[idx].i32Res = i32Res;
    pCq->cqes[idx].u32Flags = 0;
    pCq->tail++;
}

static u32
drain_sq(struct gj_io_uring_ring *pR, u32 u32ToSubmit)
{
    struct gj_io_sq_ctrl_pkg *pSq;
    struct gj_io_uring_sqe *pSqeBase;
    u32 mask;
    u32 pending;
    u32 limit;
    u32 submitted;

    if (pR == NULL || u32ToSubmit == 0u) {
        return 0u;
    }
    pSq = sq_ctrl(pR);
    pSqeBase = sqes(pR);
    mask = pSq->ring_mask;
    pending = pSq->tail - pSq->head;
    limit = u32ToSubmit;
    if (limit > pending) {
        limit = pending;
    }
    submitted = 0u;
    while (submitted < limit) {
        u32 sqIdx = pSq->head & mask;
        u32 sqeIdx = pSq->array[sqIdx];
        struct gj_io_uring_sqe *pSqe;
        i32 res;

        if (sqeIdx >= pSq->ring_entries) {
            pSq->dropped++;
            pR->u32Dropped++;
            pSq->head++;
            submitted++;
            continue;
        }
        pSqe = &pSqeBase[sqeIdx];
        res = exec_sqe(pR, pSqe);
        cqe_post(pR, pSqe->u64UserData, res);
        pSq->head++;
        submitted++;
    }
    return submitted;
}

i64
gj_io_uring_setup(u32 u32Entries, u64 u64ParamsUser)
{
    u32 i;
    u32 nSq;
    u32 nCq;
    struct gj_io_uring_params params;
    struct gj_io_uring_ring *pR;
    i64 i64Fd;
    u32 u32Slot;

    if (!g_fInited) {
        gj_io_uring_init();
    }
    nSq = pow2_clamp(u32Entries ? u32Entries : 8u, GJ_IORING_ENTRIES);
    nCq = nSq * 2u;
    if (nCq > GJ_IORING_ENTRIES * 2u) {
        nCq = GJ_IORING_ENTRIES * 2u;
    }

    pR = NULL;
    u32Slot = GJ_IORING_MAX;
    for (i = 0; i < GJ_IORING_MAX; i++) {
        if (!g_aRing[i].u8Used) {
            pR = &g_aRing[i];
            u32Slot = i;
            break;
        }
    }
    if (pR == NULL || u32Slot >= GJ_IORING_MAX) {
        return -LINUX_ENOMEM;
    }
    memset(pR, 0, sizeof(*pR));
    pR->u8Used = 1;
    pR->u32SqEntries = nSq;
    pR->u32CqEntries = nCq;
    pkg_init(pR, nSq, nCq);

    i64Fd = vfs_ram_io_uring_open(u32Slot);
    if (i64Fd < 0) {
        pR->u8Used = 0;
        g_aRingFd[u32Slot] = -1;
        return i64Fd;
    }
    g_aRingFd[u32Slot] = i64Fd;

    memset(&params, 0, sizeof(params));
    params.sq_entries = nSq;
    params.cq_entries = nCq;
    params.features = IORING_FEAT_SINGLE_MMAP;
    /* Offsets within the single package (relative to mmap base). */
    params.sq_off.head = PKG_OFF_SQ + 0;
    params.sq_off.tail = PKG_OFF_SQ + 4;
    params.sq_off.ring_mask = PKG_OFF_SQ + 8;
    params.sq_off.ring_entries = PKG_OFF_SQ + 12;
    params.sq_off.flags = PKG_OFF_SQ + 16;
    params.sq_off.dropped = PKG_OFF_SQ + 20;
    params.sq_off.array = PKG_OFF_SQ + 64;
    params.cq_off.head = PKG_OFF_CQ + 0;
    params.cq_off.tail = PKG_OFF_CQ + 4;
    params.cq_off.ring_mask = PKG_OFF_CQ + 8;
    params.cq_off.ring_entries = PKG_OFF_CQ + 12;
    params.cq_off.overflow = PKG_OFF_CQ + 16;
    params.cq_off.flags = PKG_OFF_CQ + 20;
    params.cq_off.cqes = PKG_OFF_CQ + 64;

    if (u64ParamsUser != 0) {
        if (user_range_ok(u64ParamsUser, sizeof(params))) {
            if (copy_to_user(u64ParamsUser, &params, sizeof(params)) !=
                GJ_OK) {
                (void)vfs_ram_close(i64Fd);
                pR->u8Used = 0;
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64ParamsUser, &params, sizeof(params));
        }
    }

    kprintf("io_uring: setup fd=%ld sq=%u cq=%u mmap=1\n", (long)i64Fd, nSq,
            nCq);
    return i64Fd;
}

/*
 * Map package into active process (or return kernel VA for smoke).
 * offset: IORING_OFF_SQ_RING / CQ_RING / SQES (or 0 for full package).
 * Returns VA or -errno.
 */
i64
gj_io_uring_mmap(i64 i64Fd, u64 u64Offset, u64 u64Len)
{
    struct gj_io_uring_ring *pR = ring_from_fd(i64Fd);
    u8 *pBase;
    u64 offInPkg;
    u64 mapLen;
    gj_vaddr_t va;
    u32 i;
    gj_paddr_t pa;

    if (pR == NULL) {
        return -LINUX_EBADF;
    }
    pBase = pR->aPkg;
    if (u64Offset == IORING_OFF_SQ_RING || u64Offset == 0ull) {
        offInPkg = PKG_OFF_SQ;
    } else if (u64Offset == IORING_OFF_CQ_RING) {
        offInPkg = PKG_OFF_CQ;
    } else if (u64Offset == IORING_OFF_SQES) {
        offInPkg = PKG_OFF_SQES;
    } else if (u64Offset < PKG_SIZE) {
        offInPkg = u64Offset;
    } else {
        return -LINUX_EINVAL;
    }
    mapLen = u64Len ? u64Len : (PKG_SIZE - offInPkg);
    if (offInPkg + mapLen > PKG_SIZE) {
        mapLen = PKG_SIZE - offInPkg;
    }

    /*
     * Kernel smoke / no process: return identity VA of package slice.
     * Userspace fill works through kernel pointer when testing without AS.
     */
    if (g_pLinuxProc == NULL) {
        pR->u64UserMapVa = (u64)(gj_vaddr_t)pBase;
        return (i64)((u64)(gj_vaddr_t)(pBase + offInPkg));
    }

    /* Map package pages into process at high band with USER|RW. */
    if (process_as_ensure(g_pLinuxProc) != GJ_OK) {
        return -LINUX_ENOMEM;
    }
    process_as_activate(g_pLinuxProc);
    va = 0x0000000070000000ull + (u64)(pR - g_aRing) * PKG_SIZE + offInPkg;
    for (i = 0; i < PKG_PAGES; i++) {
        void *pPage = (void *)&pR->aPkg[i * GJ_IORING_PAGE];

        pa = vmm_virt_to_phys((gj_vaddr_t)pPage);
        if (pa == 0) {
            /* Identity low for kernel BSS/data */
            pa = (gj_paddr_t)(u64)(gj_vaddr_t)pPage;
        }
        if (vmm_map_page((gj_vaddr_t)(va - offInPkg + i * GJ_IORING_PAGE), pa,
                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                             GJ_VMM_PROT_USER) != GJ_OK) {
            return -LINUX_ENOMEM;
        }
    }
    pR->u64UserMapVa = va - offInPkg;
    (void)mapLen;
    return (i64)va;
}

/** Kernel/smoke helper: pointer to full package + fill params offsets. */
void *
gj_io_uring_mmap_package(i64 i64Fd)
{
    struct gj_io_uring_ring *pR = ring_from_fd(i64Fd);

    if (pR == NULL) {
        return NULL;
    }
    pR->u64UserMapVa = (u64)(gj_vaddr_t)pR->aPkg;
    return (void *)pR->aPkg;
}

i64
gj_io_uring_sqe_inject(i64 i64Fd, u32 u32Idx, u8 u8Opcode, i32 i32Fd,
                       u64 u64Off, u64 u64Addr, u32 u32Len, u64 u64UserData)
{
    struct gj_io_uring_ring *pR = ring_from_fd(i64Fd);
    struct gj_io_uring_sqe *pSqe;
    struct gj_io_sq_ctrl_pkg *pSq;

    if (pR == NULL) {
        return -LINUX_EBADF;
    }
    if (u32Idx >= pR->u32SqEntries) {
        return -LINUX_EINVAL;
    }
    pSqe = &sqes(pR)[u32Idx];
    memset(pSqe, 0, sizeof(*pSqe));
    pSqe->u8Opcode = u8Opcode;
    pSqe->i32Fd = i32Fd;
    pSqe->u64Off = u64Off;
    pSqe->u64Addr = u64Addr;
    pSqe->u32Len = u32Len;
    pSqe->u64UserData = u64UserData;
    pSq = sq_ctrl(pR);
    pSq->array[u32Idx] = u32Idx;
    return 0;
}

i64
gj_io_uring_cqe_peek(i64 i64Fd, u64 *pUserData, i32 *pRes)
{
    struct gj_io_uring_ring *pR = ring_from_fd(i64Fd);
    struct gj_io_cq_ctrl_pkg *pCq;
    u32 idx;

    if (pR == NULL) {
        return -LINUX_EBADF;
    }
    pCq = cq_ctrl(pR);
    if (pCq->tail == pCq->head) {
        return -LINUX_EAGAIN;
    }
    idx = pCq->head & pCq->ring_mask;
    if (pUserData != NULL) {
        *pUserData = pCq->cqes[idx].u64UserData;
    }
    if (pRes != NULL) {
        *pRes = pCq->cqes[idx].i32Res;
    }
    return 0;
}

i64
gj_io_uring_cqe_advance(i64 i64Fd)
{
    struct gj_io_uring_ring *pR = ring_from_fd(i64Fd);
    struct gj_io_cq_ctrl_pkg *pCq;

    if (pR == NULL) {
        return -LINUX_EBADF;
    }
    pCq = cq_ctrl(pR);
    if (pCq->tail == pCq->head) {
        return -LINUX_EAGAIN;
    }
    pCq->head++;
    return 0;
}

i64
gj_io_uring_enter(i64 i64Fd, u32 u32ToSubmit, u32 u32MinComplete, u32 u32Flags)
{
    struct gj_io_uring_ring *pR = ring_from_fd(i64Fd);
    struct gj_io_sq_ctrl_pkg *pSq;
    u32 u32Submitted;
    u32 u32Ready;
    u32 pending;

    (void)u32Flags;
    if (pR == NULL) {
        return -LINUX_EBADF;
    }
    if (u32ToSubmit == 0u && u32MinComplete == 0u) {
        return 0;
    }

    pSq = sq_ctrl(pR);
    pending = pSq->tail - pSq->head;
    /*
     * Userspace fill path: if tail advanced past head, drain those SQEs.
     * Soft inject path: empty SQ + to_submit → queue-queue NOP/inject slots.
     */
    if (pending == 0u && u32ToSubmit > 0u) {
        u32 n = u32ToSubmit;
        if (n > pSq->ring_entries) {
            n = pSq->ring_entries;
        }
        pSq->tail = pSq->head + n;
    }
    u32Submitted = drain_sq(pR, u32ToSubmit);

    if (u32MinComplete > 0u) {
        struct gj_io_cq_ctrl_pkg *pCq = cq_ctrl(pR);

        u32Ready = pCq->tail - pCq->head;
        if (u32Ready < u32MinComplete) {
            return -LINUX_EAGAIN;
        }
    }
    return (i64)u32Submitted;
}

i64
gj_io_uring_register(i64 i64Fd, u32 u32Opcode, u64 u64Arg, u32 u32NrArgs)
{
    struct gj_io_uring_ring *pR = ring_from_fd(i64Fd);

    (void)u64Arg;
    (void)u32NrArgs;
    if (pR == NULL) {
        return -LINUX_EBADF;
    }
    if (u32Opcode == IORING_REGISTER_BUFFERS ||
        u32Opcode == IORING_REGISTER_FILES ||
        u32Opcode == IORING_UNREGISTER_FILES ||
        u32Opcode == IORING_REGISTER_EVENTFD) {
        return 0;
    }
    return -LINUX_ENOSYS;
}
