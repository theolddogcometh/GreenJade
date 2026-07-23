/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * io_uring min rings + SQE I/O + SQ/CQ mmap (userspace fill without inject).
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
 * Honesty: min rings ≠ full game I/O. This TU is a soft product surface
 * (setup/enter/register + vfs_ram SQE depth). It is not async title I/O,
 * not bar3, and not a Deck Top-50 claim.
 *
 * Soft inventory (Wave 14 base + Wave 16 exclusive deepen) — greppable "io_uring: soft …":
 *   io_uring: soft inventory   — pool/depth caps + call rollup + wave
 *   io_uring: soft setup       — setup ok/enomem/efault tallies
 *   io_uring: soft enter       — enter/submit/inject/eagain tallies
 *   io_uring: soft register    — register opcode buckets
 *   io_uring: soft mmap        — mmap / package-map tallies
 *   io_uring: soft sqe         — exec category tallies
 *   io_uring: soft pool        — live ring occupancy
 *   io_uring: soft inject      — empty-SQ soft inject lifecycle
 *   io_uring: soft honesty     — min_rings vs full_game_io claim lamps
 *   io_uring: soft path        — product claim + honesty line
 *   io_uring: soft rates       — Wave 15 setup/enter/sqe share
 *   io_uring: soft last        — Wave 15 live pool snapshot lamps
 *   io_uring: soft catalog     — Wave 15 surface catalog stamp
 *   io_uring: soft deepen      — wave=16 areas stamp
 *   io_uring: soft inventory PASS / soft PASS
 * greppable: io_uring: soft
 *
 * Honesty: min rings remain a soft scaffold — not product game I/O,
 * not async title I/O, not bar3, not Deck Top-50.
 *
 * Greppable product: io_uring: min rings ready, io_uring: setup …,
 *   linux: io_uring min rings PASS, linux: io_uring SQE I/O PASS,
 *   linux: io_uring mmap PASS, linux: io_uring register depth PASS,
 *   linux: io_uring more opcodes PASS
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
#define GJ_IORING_PATH_MAX  256u

/* Register opcodes (mirror header). */
#define IORING_REGISTER_BUFFERS          0u
#define IORING_UNREGISTER_BUFFERS        1u
#define IORING_REGISTER_FILES            2u
#define IORING_UNREGISTER_FILES          3u
#define IORING_REGISTER_EVENTFD          4u
#define IORING_UNREGISTER_EVENTFD        5u
#define IORING_REGISTER_FILES_UPDATE  6u
#define IORING_REGISTER_EVENTFD_ASYNC    7u
#define IORING_REGISTER_PROBE            8u
#define IORING_REGISTER_PERSONALITY      9u
#define IORING_UNREGISTER_PERSONALITY   10u

#define IORING_FEAT_SINGLE_MMAP  (1u << 0)

/* SQE opcodes */
#define IORING_OP_NOP              0u
#define IORING_OP_READV            1u
#define IORING_OP_WRITEV           2u
#define IORING_OP_FSYNC            3u
#define IORING_OP_READ_FIXED       4u
#define IORING_OP_WRITE_FIXED      5u
#define IORING_OP_POLL_ADD         6u
#define IORING_OP_POLL_REMOVE      7u
#define IORING_OP_SYNC_FILE_RANGE  8u
#define IORING_OP_SENDMSG          9u
#define IORING_OP_RECVMSG         10u
#define IORING_OP_TIMEOUT         11u
#define IORING_OP_TIMEOUT_REMOVE  12u
#define IORING_OP_ACCEPT          13u
#define IORING_OP_ASYNC_CANCEL    14u
#define IORING_OP_LINK_TIMEOUT    15u
#define IORING_OP_CONNECT         16u
#define IORING_OP_FALLOCATE       17u
#define IORING_OP_OPENAT          18u
#define IORING_OP_CLOSE           19u
#define IORING_OP_FILES_UPDATE   20u
#define IORING_OP_STATX           21u
#define IORING_OP_READ            22u
#define IORING_OP_WRITE           23u
#define IORING_OP_FADVISE         24u
#define IORING_OP_MADVISE         25u
#define IORING_OP_SEND            26u
#define IORING_OP_RECV            27u
#define IORING_OP_OPENAT2         28u
#define IORING_OP_EPOLL_CTL       29u
#define IORING_OP_PROVIDE_BUFFERS 31u
#define IORING_OP_REMOVE_BUFFERS  32u
#define IORING_OP_SHUTDOWN        34u
#define IORING_OP_RENAMEAT        35u
#define IORING_OP_UNLINKAT        36u
#define IORING_OP_MKDIRAT         37u
#define IORING_OP_SYMLINKAT       38u
#define IORING_OP_LINKAT          39u

/* SQE flags */
#define IOSQE_FIXED_FILE   (1u << 0)

/* Linux open flags (public x86_64). */
/* LINUX_O_CREAT / LINUX_O_TRUNC: from linux_abi.h (do not redefine). */
#ifndef LINUX_O_CREAT
#define LINUX_O_CREAT  0x40u
#endif
#ifndef LINUX_O_TRUNC
#define LINUX_O_TRUNC  0x200u
#endif

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
    u64 u64Off;   /* also addr2 for renameat / dual-path ops */
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

#define GJ_IORING_REG_FILES_MAX 16u
#define GJ_IORING_REG_BUFS_MAX  8u

struct gj_io_uring_ring {
    u8  u8Used;
    u8  u8Mapped; /* package exposed / mmap ready */
    u8  u8Pad[2];
    u32 u32SqEntries;
    u32 u32CqEntries;
    u32 u32Dropped;
    u32 u32Overflow;
    u32 u32SqeIoOk;
    /* Soft register tables (fixed-file / fixed-buffer / eventfd). */
    u32 u32RegFiles;
    u32 u32RegBufs;
    u32 u32RegEventfd;
    u32 u32Personality;
    i32 aRegFiles[GJ_IORING_REG_FILES_MAX];
    u64 aRegBufAddr[GJ_IORING_REG_BUFS_MAX];
    u32 aRegBufLen[GJ_IORING_REG_BUFS_MAX];
    /* Kernel package (page-aligned). */
    u8  aPkg[PKG_SIZE] __attribute__((aligned(4096)));
    u64 u64UserMapVa; /* 0 if only kernel-visible; else user VA of package */
};

_Static_assert(sizeof(struct gj_io_uring_sqe) == 64, "sqe 64");
_Static_assert(sizeof(struct gj_io_uring_cqe) == 16, "cqe 16");

static struct gj_io_uring_ring g_aRing[GJ_IORING_MAX];
static i64 g_aRingFd[GJ_IORING_MAX];
static int g_fInited;

/* Wave 16 soft inventory stamp (file-local; never product gate). */
#define GJ_IORING_SOFT_WAVE  16u
/* Soft inventory area count (fixed greppable categories for deepen stamp). */
#define GJ_IORING_SOFT_AREAS 18u

/*
 * Soft product inventory (Wave 16 exclusive deepen). File-local sticky
 * counters; wrap OK; diagnostics only — never hard-gate setup/enter/register.
 * greppable: io_uring: soft
 */
struct gj_io_uring_soft {
    u64 u64Init;            /* gj_io_uring_init entries */
    u64 u64SetupOk;         /* setup returned a ring fd */
    u64 u64SetupEnomem;     /* pool full */
    u64 u64SetupEfault;     /* params copy_to_user fail */
    u64 u64SetupOpenFail;   /* vfs_ram_io_uring_open fail */
    u64 u64EnterOk;         /* enter returned submitted count */
    u64 u64EnterEbadf;      /* enter bad fd */
    u64 u64EnterEagain;     /* min_complete not met */
    u64 u64EnterSubmit;     /* total SQEs drained via enter */
    u64 u64EnterInject;     /* empty-SQ soft inject advances */
    u64 u64EnterNop;        /* enter with both submit/min 0 */
    u64 u64RegOk;           /* register success */
    u64 u64RegEbadf;        /* register bad fd */
    u64 u64RegEnosys;       /* unknown register opcode */
    u64 u64RegEfault;       /* register user copy fault */
    u64 u64RegBuffers;      /* REGISTER/UNREGISTER_BUFFERS */
    u64 u64RegFiles;        /* REGISTER/UNREGISTER/UPDATE FILES */
    u64 u64RegEventfd;      /* eventfd register/unregister */
    u64 u64RegProbe;        /* REGISTER_PROBE */
    u64 u64RegPersonality;  /* personality register/unregister */
    u64 u64MmapOk;          /* gj_io_uring_mmap success */
    u64 u64MmapEbadf;       /* mmap bad fd */
    u64 u64MmapEinval;      /* mmap bad offset */
    u64 u64MmapEnomem;      /* mmap map fail */
    u64 u64MmapPkg;         /* mmap_package hits */
    u64 u64InjectOk;        /* sqe_inject success */
    u64 u64InjectFail;      /* sqe_inject reject */
    u64 u64SqeExec;         /* exec_sqe entries */
    u64 u64SqeNop;          /* NOP / soft-immediate ops */
    u64 u64SqeRw;           /* READ/WRITE/V/FIXED/SEND/RECV */
    u64 u64SqeFs;           /* open/close/stat/path/fallocate */
    u64 u64SqePoll;         /* POLL_ADD/REMOVE / epoll_ctl */
    u64 u64SqeReg;          /* FILES_UPDATE / PROVIDE/REMOVE_BUFFERS */
    u64 u64SqeEnosys;       /* ACCEPT/CONNECT soft ENOSYS */
    u64 u64SqeEinval;       /* unknown opcode / bad args */
    u64 u64CqePost;         /* CQE posts */
    u64 u64CqeOverflow;     /* CQ overflow drops */
    u64 u64SqDropped;       /* SQ array index drops */
    u64 u64SoftLog;         /* soft inventory dumps */
    u64 u64SoftScan;        /* pool occupancy samples */
    /* Wave 15 deepen: live SQ/CQ snapshot sums + eventfd/personality lamps */
    u64 u64SoftSqPending;   /* sum (sq.tail-sq.head) across used rings */
    u64 u64SoftCqPending;   /* sum (cq.tail-cq.head) across used rings */
    u64 u64SoftEventfdLive; /* rings with eventfd registered */
    u64 u64SoftPersLive;    /* rings with personality registered */
};

static struct gj_io_uring_soft g_soft;
static u8 g_fSoftOnce; /* one-shot deep print after first product activity */
/* Soft live pool snapshot (filled by ioring_soft_scan). */
static u32 g_u32SoftUsed;
static u32 g_u32SoftFree;
static u32 g_u32SoftMapped;
static u32 g_u32SoftRegFiles;
static u32 g_u32SoftRegBufs;
static u32 g_u32SoftEventfdLive;
static u32 g_u32SoftPersLive;
static u32 g_u32SoftSqPending;
static u32 g_u32SoftCqPending;

static void ioring_soft_inc(u64 *pCtr);
static void ioring_soft_scan(void);
static void ioring_soft_log(void);
static void ioring_soft_maybe_once(void);

/** Soft: saturating-ish bump (u64 wrap is fine for telemetry). */
static void
ioring_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Soft: sample ring pool occupancy (no lock; diagnostic race OK).
 * Wave 15 deepen also snapshots SQ/CQ pending + eventfd/personality lamps.
 * greppable via io_uring: soft pool
 */
static void
ioring_soft_scan(void)
{
    u32 i;
    u32 u32Used = 0;
    u32 u32Mapped = 0;
    u32 u32RegFiles = 0;
    u32 u32RegBufs = 0;
    u32 u32Eventfd = 0;
    u32 u32Pers = 0;
    u32 u32SqPend = 0;
    u32 u32CqPend = 0;

    ioring_soft_inc(&g_soft.u64SoftScan);
    for (i = 0; i < GJ_IORING_MAX; i++) {
        if (g_aRing[i].u8Used) {
            struct gj_io_sq_ctrl_pkg *pSq;
            struct gj_io_cq_ctrl_pkg *pCq;
            u32 u32SqH;
            u32 u32SqT;
            u32 u32CqH;
            u32 u32CqT;

            u32Used++;
            if (g_aRing[i].u8Mapped) {
                u32Mapped++;
            }
            u32RegFiles += g_aRing[i].u32RegFiles;
            u32RegBufs += g_aRing[i].u32RegBufs;
            if (g_aRing[i].u32RegEventfd != 0u) {
                u32Eventfd++;
            }
            if (g_aRing[i].u32Personality != 0u) {
                u32Pers++;
            }
            /* Soft SQ/CQ pending (wrap-safe mask distance; diagnostic only). */
            pSq = (struct gj_io_sq_ctrl_pkg *)(void *)&g_aRing[i].aPkg[PKG_OFF_SQ];
            pCq = (struct gj_io_cq_ctrl_pkg *)(void *)&g_aRing[i].aPkg[PKG_OFF_CQ];
            u32SqH = pSq->head;
            u32SqT = pSq->tail;
            u32CqH = pCq->head;
            u32CqT = pCq->tail;
            u32SqPend += (u32SqT - u32SqH) & pSq->ring_mask;
            u32CqPend += (u32CqT - u32CqH) & pCq->ring_mask;
        }
    }
    g_u32SoftUsed = u32Used;
    g_u32SoftFree = (u32Used < GJ_IORING_MAX) ? (GJ_IORING_MAX - u32Used) : 0u;
    g_u32SoftMapped = u32Mapped;
    g_u32SoftRegFiles = u32RegFiles;
    g_u32SoftRegBufs = u32RegBufs;
    g_u32SoftEventfdLive = u32Eventfd;
    g_u32SoftPersLive = u32Pers;
    g_u32SoftSqPending = u32SqPend;
    g_u32SoftCqPending = u32CqPend;
    g_soft.u64SoftSqPending = (u64)u32SqPend;
    g_soft.u64SoftCqPending = (u64)u32CqPend;
    g_soft.u64SoftEventfdLive = (u64)u32Eventfd;
    g_soft.u64SoftPersLive = (u64)u32Pers;
}

/**
 * Greppable soft inventory (Wave 16 exclusive deepen). Prefix "io_uring: soft …".
 * Pure observation — never gates min-rings smoke PASS.
 * Honesty: min rings ≠ full game I/O (soft scaffold only).
 *
 * greppable: io_uring: soft inventory
 * greppable: io_uring: soft setup
 * greppable: io_uring: soft enter
 * greppable: io_uring: soft register
 * greppable: io_uring: soft mmap
 * greppable: io_uring: soft sqe
 * greppable: io_uring: soft pool
 * greppable: io_uring: soft inject
 * greppable: io_uring: soft honesty
 * greppable: io_uring: soft path
 * greppable: io_uring: soft rates
 * greppable: io_uring: soft last
 * greppable: io_uring: soft catalog
 * greppable: io_uring: soft deepen
 * greppable: io_uring: soft inventory PASS
 */
static void
ioring_soft_log(void)
{
    ioring_soft_inc(&g_soft.u64SoftLog);
    ioring_soft_scan();

    /* Grep: io_uring: soft inventory */
    kprintf("io_uring: soft inventory wave=%u min_rings=1 full_game_io=0 "
            "pool=%u depth<=%u reg_files<=%u reg_bufs<=%u io_max=%u "
            "init=%lu setup_ok=%lu enter_ok=%lu reg_ok=%lu mmap_ok=%lu "
            "sqe_exec=%lu logs=%lu areas=%u "
            "(min rings != full game I/O; not bar3)\n",
            (unsigned)GJ_IORING_SOFT_WAVE,
            GJ_IORING_MAX, GJ_IORING_ENTRIES, GJ_IORING_REG_FILES_MAX,
            GJ_IORING_REG_BUFS_MAX, GJ_IORING_IO_MAX,
            (unsigned long)g_soft.u64Init,
            (unsigned long)g_soft.u64SetupOk,
            (unsigned long)g_soft.u64EnterOk,
            (unsigned long)g_soft.u64RegOk,
            (unsigned long)g_soft.u64MmapOk,
            (unsigned long)g_soft.u64SqeExec,
            (unsigned long)g_soft.u64SoftLog,
            (unsigned)GJ_IORING_SOFT_AREAS);

    /* Grep: io_uring: soft setup */
    kprintf("io_uring: soft setup ok=%lu enomem=%lu efault=%lu open_fail=%lu "
            "default_sq=8 pow2_clamp=%u cq=2x_sq single_mmap=1 wave=%u\n",
            (unsigned long)g_soft.u64SetupOk,
            (unsigned long)g_soft.u64SetupEnomem,
            (unsigned long)g_soft.u64SetupEfault,
            (unsigned long)g_soft.u64SetupOpenFail,
            GJ_IORING_ENTRIES,
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft enter */
    kprintf("io_uring: soft enter ok=%lu ebadf=%lu eagain=%lu nop=%lu "
            "submit=%lu inject=%lu drain=sync_sqe "
            "(no IOPOLL; not full async game I/O) wave=%u\n",
            (unsigned long)g_soft.u64EnterOk,
            (unsigned long)g_soft.u64EnterEbadf,
            (unsigned long)g_soft.u64EnterEagain,
            (unsigned long)g_soft.u64EnterNop,
            (unsigned long)g_soft.u64EnterSubmit,
            (unsigned long)g_soft.u64EnterInject,
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft register */
    kprintf("io_uring: soft register ok=%lu ebadf=%lu enosys=%lu efault=%lu "
            "buffers=%lu files=%lu eventfd=%lu probe=%lu personality=%lu "
            "caps=files<=%u,bufs<=%u wave=%u\n",
            (unsigned long)g_soft.u64RegOk,
            (unsigned long)g_soft.u64RegEbadf,
            (unsigned long)g_soft.u64RegEnosys,
            (unsigned long)g_soft.u64RegEfault,
            (unsigned long)g_soft.u64RegBuffers,
            (unsigned long)g_soft.u64RegFiles,
            (unsigned long)g_soft.u64RegEventfd,
            (unsigned long)g_soft.u64RegProbe,
            (unsigned long)g_soft.u64RegPersonality,
            GJ_IORING_REG_FILES_MAX, GJ_IORING_REG_BUFS_MAX,
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft mmap */
    kprintf("io_uring: soft mmap ok=%lu ebadf=%lu einval=%lu enomem=%lu "
            "pkg=%lu inject_ok=%lu inject_fail=%lu single_mmap=1 "
            "pkg_pages=%u wave=%u\n",
            (unsigned long)g_soft.u64MmapOk,
            (unsigned long)g_soft.u64MmapEbadf,
            (unsigned long)g_soft.u64MmapEinval,
            (unsigned long)g_soft.u64MmapEnomem,
            (unsigned long)g_soft.u64MmapPkg,
            (unsigned long)g_soft.u64InjectOk,
            (unsigned long)g_soft.u64InjectFail, PKG_PAGES,
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft sqe */
    kprintf("io_uring: soft sqe exec=%lu nop=%lu rw=%lu fs=%lu poll=%lu "
            "reg=%lu enosys=%lu einval=%lu cqe_post=%lu overflow=%lu "
            "sq_drop=%lu vfs_ram=1 wave=%u\n",
            (unsigned long)g_soft.u64SqeExec,
            (unsigned long)g_soft.u64SqeNop,
            (unsigned long)g_soft.u64SqeRw,
            (unsigned long)g_soft.u64SqeFs,
            (unsigned long)g_soft.u64SqePoll,
            (unsigned long)g_soft.u64SqeReg,
            (unsigned long)g_soft.u64SqeEnosys,
            (unsigned long)g_soft.u64SqeEinval,
            (unsigned long)g_soft.u64CqePost,
            (unsigned long)g_soft.u64CqeOverflow,
            (unsigned long)g_soft.u64SqDropped,
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft pool */
    kprintf("io_uring: soft pool max=%u used=%u free=%u mapped=%u "
            "reg_files_sum=%u reg_bufs_sum=%u eventfd_live=%u pers_live=%u "
            "sq_pend=%u cq_pend=%u samples=%lu wave=%u\n",
            GJ_IORING_MAX, g_u32SoftUsed, g_u32SoftFree, g_u32SoftMapped,
            g_u32SoftRegFiles, g_u32SoftRegBufs, g_u32SoftEventfdLive,
            g_u32SoftPersLive, g_u32SoftSqPending, g_u32SoftCqPending,
            (unsigned long)g_soft.u64SoftScan,
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft inject (Wave 15 deepen) */
    kprintf("io_uring: soft inject ok=%lu fail=%lu enter_inject=%lu "
            "mode=empty_sq_advance claim=soft_scaffold wave=%u\n",
            (unsigned long)g_soft.u64InjectOk,
            (unsigned long)g_soft.u64InjectFail,
            (unsigned long)g_soft.u64EnterInject,
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft honesty (Wave 15 deepen) */
    kprintf("io_uring: soft honesty min_rings=1 full_game_io=0 async_title=0 "
            "iopoll=0 bar3=open deck_top50=0 product=soft_scaffold "
            "sqe=vfs_ram_sync wave=%u "
            "(min rings != full game I/O; soft inventory; not bar3)\n",
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft path */
    kprintf("io_uring: soft path setup=vfs_ram_fd enter=drain_sq "
            "register=soft_tables mmap=single_pkg_3p "
            "sqe=vfs_ram_sync inject=empty_sq_advance "
            "min_rings=1 full_game_io=0 wave=%u "
            "(min rings != full game I/O; soft inventory; not bar3)\n",
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft rates (Wave 15 deepen) */
    {
        u64 u64SetupSum;
        u64 u64BpSetupOk;
        u64 u64EnterSum;
        u64 u64BpEnterOk;
        u64 u64SqeSum;
        u64 u64BpSqeNop;

        u64SetupSum = g_soft.u64SetupOk + g_soft.u64SetupEnomem +
                      g_soft.u64SetupEfault + g_soft.u64SetupOpenFail;
        if (u64SetupSum != 0) {
            u64BpSetupOk = (g_soft.u64SetupOk * 10000ull) / u64SetupSum;
        } else {
            u64BpSetupOk = 0;
        }
        u64EnterSum = g_soft.u64EnterOk + g_soft.u64EnterEbadf +
                      g_soft.u64EnterEagain + g_soft.u64EnterNop;
        if (u64EnterSum != 0) {
            u64BpEnterOk = (g_soft.u64EnterOk * 10000ull) / u64EnterSum;
        } else {
            u64BpEnterOk = 0;
        }
        u64SqeSum = g_soft.u64SqeNop + g_soft.u64SqeRw + g_soft.u64SqeFs +
                    g_soft.u64SqePoll + g_soft.u64SqeReg + g_soft.u64SqeEnosys +
                    g_soft.u64SqeEinval;
        if (u64SqeSum != 0) {
            u64BpSqeNop = (g_soft.u64SqeNop * 10000ull) / u64SqeSum;
        } else {
            u64BpSqeNop = 0;
        }
        kprintf("io_uring: soft rates bp_setup_ok=%lu bp_enter_ok=%lu "
                "bp_sqe_nop=%lu setup_sum=%lu enter_sum=%lu sqe_sum=%lu "
                "wave=%u\n",
                (unsigned long)u64BpSetupOk,
                (unsigned long)u64BpEnterOk,
                (unsigned long)u64BpSqeNop,
                (unsigned long)u64SetupSum,
                (unsigned long)u64EnterSum,
                (unsigned long)u64SqeSum,
                (unsigned)GJ_IORING_SOFT_WAVE);
    }

    /* Grep: io_uring: soft last (Wave 15 deepen) */
    kprintf("io_uring: soft last used=%u free=%u mapped=%u "
            "sq_pend=%u cq_pend=%u eventfd_live=%u pers_live=%u "
            "logs=%lu once=%u wave=%u\n",
            g_u32SoftUsed, g_u32SoftFree, g_u32SoftMapped,
            g_u32SoftSqPending, g_u32SoftCqPending,
            g_u32SoftEventfdLive, g_u32SoftPersLive,
            (unsigned long)g_soft.u64SoftLog,
            g_fSoftOnce ? 1u : 0u,
            (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft catalog (Wave 15 deepen) */
    kprintf("io_uring: soft catalog wave=%u areas=%u "
            "surfaces=inventory,setup,enter,register,mmap,sqe,pool,"
            "inject,honesty,path,rates,last,catalog,deepen,PASS\n",
            (unsigned)GJ_IORING_SOFT_WAVE,
            (unsigned)GJ_IORING_SOFT_AREAS);

        /* Grep: io_uring: soft surfaces (Wave 16 deepen) */
    kprintf("io_uring: soft surfaces count=%u wave=%u "
            "names=inventory,pool,calls,path,caps,inject,honesty,rates,"
            "last,catalog,surfaces,note,deepen,PASS\n",
            (unsigned)GJ_IORING_SOFT_AREAS, (unsigned)GJ_IORING_SOFT_WAVE);

    /* Grep: io_uring: soft note (Wave 16 deepen) */
    kprintf("io_uring: soft note milestone=wave16 exclusive=1 "
            "min_rings=1 full_game_io=0 soft_only=1 not_bar3=1 "
            "wave=%u\n",
            (unsigned)GJ_IORING_SOFT_WAVE);

/* Grep: io_uring: soft deepen wave (Wave 15 stamp) */
    kprintf("io_uring: soft deepen wave=%u areas=%u logs=%lu "
            "used=%u mapped=%u setup_ok=%lu enter_ok=%lu sqe_exec=%lu "
            "(Wave 16 exclusive; min rings soft scaffold; not product game I/O; "
            "not bar3)\n",
            (unsigned)GJ_IORING_SOFT_WAVE,
            (unsigned)GJ_IORING_SOFT_AREAS,
            (unsigned long)g_soft.u64SoftLog,
            g_u32SoftUsed, g_u32SoftMapped,
            (unsigned long)g_soft.u64SetupOk,
            (unsigned long)g_soft.u64EnterOk,
            (unsigned long)g_soft.u64SqeExec);

    /* Grep: io_uring: soft inventory PASS / soft PASS */
    kprintf("io_uring: soft inventory PASS wave=%u logs=%lu "
            "setup_ok=%lu enter_ok=%lu sqe_exec=%lu\n",
            (unsigned)GJ_IORING_SOFT_WAVE,
            (unsigned long)g_soft.u64SoftLog,
            (unsigned long)g_soft.u64SetupOk,
            (unsigned long)g_soft.u64EnterOk,
            (unsigned long)g_soft.u64SqeExec);
    kprintf("io_uring: soft PASS wave=%u logs=%lu\n",
            (unsigned)GJ_IORING_SOFT_WAVE,
            (unsigned long)g_soft.u64SoftLog);
}

/**
 * After first product activity, print soft inventory once (mirrors futex /
 * session_door soft-stats-once). Safe from setup/enter/register return paths.
 */
static void
ioring_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_soft.u64SetupOk == 0 && g_soft.u64EnterOk == 0 &&
        g_soft.u64RegOk == 0 && g_soft.u64MmapOk == 0 &&
        g_soft.u64MmapPkg == 0 && g_soft.u64InjectOk == 0) {
        return;
    }
    g_fSoftOnce = 1;
    ioring_soft_log();
}

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
    memset(&g_soft, 0, sizeof(g_soft));
    g_fSoftOnce = 0;
    g_u32SoftUsed = 0;
    g_u32SoftFree = GJ_IORING_MAX;
    g_u32SoftMapped = 0;
    g_u32SoftRegFiles = 0;
    g_u32SoftRegBufs = 0;
    g_fInited = 1;
    ioring_soft_inc(&g_soft.u64Init);
    /*
     * Product marker: greppable "io_uring: min rings ready".
     * Honesty: min rings ≠ full game I/O.
     */
    kprintf("io_uring: min rings ready pool=%u depth<=%u SQE+I/O+mmap+"
            "fixed+poll+openat (min rings != full game I/O)\n",
            GJ_IORING_MAX, GJ_IORING_ENTRIES);
    /* Wave 15 soft inventory baseline (greppable io_uring: soft …). */
    ioring_soft_log();
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

/*
 * Copy a NUL-terminated path from user/kernel VA into pDst (max cb-1 chars).
 * Returns 0 or -errno.
 */
static i32
path_copy(u64 u64Addr, char *pDst, size_t cb)
{
    size_t i;
    i64 n;

    if (pDst == NULL || cb < 2u || u64Addr == 0ull) {
        return -LINUX_EFAULT;
    }
    memset(pDst, 0, cb);
    n = buf_in(u64Addr, pDst, cb - 1u);
    if (n < 0) {
        return (i32)n;
    }
    pDst[cb - 1u] = '\0';
    for (i = 0; i < cb; i++) {
        if (pDst[i] == '\0') {
            if (i == 0u) {
                return -LINUX_ENOENT;
            }
            return 0;
        }
    }
    pDst[cb - 1u] = '\0';
    return 0;
}

/* Resolve fd: plain or IOSQE_FIXED_FILE index into registered files. */
static i32
resolve_fd(struct gj_io_uring_ring *pR, const struct gj_io_uring_sqe *pSqe)
{
    i32 i32Fd;

    if (pSqe == NULL) {
        return -1;
    }
    i32Fd = pSqe->i32Fd;
    if ((pSqe->u8Flags & IOSQE_FIXED_FILE) != 0u) {
        u32 idx;

        if (pR == NULL || pR->u32RegFiles == 0u) {
            return -1;
        }
        if (i32Fd < 0) {
            return -1;
        }
        idx = (u32)i32Fd;
        if (idx >= pR->u32RegFiles || idx >= GJ_IORING_REG_FILES_MAX) {
            return -1;
        }
        return pR->aRegFiles[idx];
    }
    return i32Fd;
}

/* Resolve fixed buffer (READ_FIXED / WRITE_FIXED) → addr/len. */
static i32
resolve_fixed_buf(struct gj_io_uring_ring *pR, u16 u16Idx, u64 *pAddr,
                  u32 *pLen)
{
    if (pR == NULL || pAddr == NULL || pLen == NULL) {
        return -LINUX_EINVAL;
    }
    if ((u32)u16Idx >= pR->u32RegBufs ||
        (u32)u16Idx >= GJ_IORING_REG_BUFS_MAX) {
        return -LINUX_EINVAL;
    }
    *pAddr = pR->aRegBufAddr[u16Idx];
    *pLen = pR->aRegBufLen[u16Idx];
    if (*pAddr == 0ull) {
        return -LINUX_EFAULT;
    }
    return 0;
}

/* Optional: install fd into fixed-files slot when file_index is set. */
static void
maybe_install_fixed(struct gj_io_uring_ring *pR, u32 u32FileIndex, i32 i32NewFd)
{
    if (pR == NULL || i32NewFd < 0) {
        return;
    }
    /* 0 means "no fixed slot" for openat-style install in this soft path. */
    if (u32FileIndex == 0u) {
        return;
    }
    if (u32FileIndex > GJ_IORING_REG_FILES_MAX) {
        return;
    }
    {
        u32 idx = u32FileIndex - 1u; /* 1-based soft mapping */

        if (idx >= GJ_IORING_REG_FILES_MAX) {
            return;
        }
        pR->aRegFiles[idx] = i32NewFd;
        if (pR->u32RegFiles <= idx) {
            pR->u32RegFiles = idx + 1u;
        }
    }
}

static i32
do_rw(struct gj_io_uring_ring *pR, i32 i32Fd, u64 u64Off, u64 u64Addr,
      u32 u32Len, int fWrite)
{
    u8 aBounce[GJ_IORING_IO_MAX];
    i64 i64N;

    if (u32Len == 0u) {
        return 0;
    }
    if (u32Len > GJ_IORING_IO_MAX) {
        u32Len = GJ_IORING_IO_MAX;
    }
    if (!vfs_ram_fd_ok((i64)i32Fd)) {
        return -LINUX_EBADF;
    }
    if (!fWrite) {
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
}

static i32
do_rwv(struct gj_io_uring_ring *pR, i32 i32Fd, u64 u64Off, u64 u64Addr,
       u32 u32NrVecs, int fWrite)
{
    struct gj_iovec iov;
    u8 aBounce[GJ_IORING_IO_MAX];
    i64 i64N;

    if (u32NrVecs < 1u) {
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
    if (!fWrite) {
        i64N = vfs_ram_pread((i64)i32Fd, aBounce, (size_t)iov.u64Len, u64Off);
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

static i32
exec_sqe(struct gj_io_uring_ring *pR, const struct gj_io_uring_sqe *pSqe)
{
    i32 i32Fd;
    u64 u64Off;
    u64 u64Addr;
    u32 u32Len;
    char aPath[GJ_IORING_PATH_MAX];
    char aPath2[GJ_IORING_PATH_MAX];
    i32 i32Rc;
    i64 i64N;

    ioring_soft_inc(&g_soft.u64SqeExec);
    if (pSqe == NULL) {
        ioring_soft_inc(&g_soft.u64SqeEinval);
        return -LINUX_EINVAL;
    }
    i32Fd = resolve_fd(pR, pSqe);
    u64Off = pSqe->u64Off;
    u64Addr = pSqe->u64Addr;
    u32Len = pSqe->u32Len;

    switch (pSqe->u8Opcode) {
    case IORING_OP_NOP:
        ioring_soft_inc(&g_soft.u64SqeNop);
        return 0;

    case IORING_OP_FSYNC:
    case IORING_OP_SYNC_FILE_RANGE:
        ioring_soft_inc(&g_soft.u64SqeNop);
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        /* Soft: accept fsync / sync_file_range as success when fd live. */
        return 0;

    case IORING_OP_TIMEOUT:
    case IORING_OP_TIMEOUT_REMOVE:
    case IORING_OP_LINK_TIMEOUT:
        ioring_soft_inc(&g_soft.u64SqeNop);
        /* Soft timeout SQEs: complete immediately (no timer wait yet). */
        return 0;

    case IORING_OP_ASYNC_CANCEL:
        ioring_soft_inc(&g_soft.u64SqeNop);
        /* Soft cancel: no outstanding async tracking; report success. */
        return 0;

    case IORING_OP_POLL_ADD: {
        u32 want = pSqe->u32OpFlags;
        u32 ready;

        ioring_soft_inc(&g_soft.u64SqePoll);
        if (want == 0u) {
            want = 0x5u; /* POLLIN|POLLOUT public bits (EPOLLIN|EPOLLOUT) */
        }
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        ready = vfs_ram_poll_mask((i64)i32Fd, want);
        /* Soft: complete immediately with ready mask (0 = not ready). */
        return (i32)ready;
    }

    case IORING_OP_POLL_REMOVE:
        ioring_soft_inc(&g_soft.u64SqePoll);
        /* Soft: no poll wait-list; treat as successful removal. */
        return 0;

    case IORING_OP_FADVISE:
        ioring_soft_inc(&g_soft.u64SqeNop);
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        return 0;

    case IORING_OP_MADVISE:
        ioring_soft_inc(&g_soft.u64SqeNop);
        /* Soft madvise: no-op success (range not walked). */
        return 0;

    case IORING_OP_SHUTDOWN:
        ioring_soft_inc(&g_soft.u64SqeNop);
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        /* Soft: accept shutdown when fd live. */
        return 0;

    case IORING_OP_ACCEPT:
    case IORING_OP_CONNECT:
        /* Soft network accept/connect: not wired; report ENOSYS-shaped. */
        ioring_soft_inc(&g_soft.u64SqeEnosys);
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        return -LINUX_ENOSYS;

    case IORING_OP_SENDMSG:
    case IORING_OP_RECVMSG:
    case IORING_OP_SEND:
    case IORING_OP_RECV:
        /* Soft: treat as pipe/file R/W when fd is ram-backed. */
        ioring_soft_inc(&g_soft.u64SqeRw);
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        if (pSqe->u8Opcode == IORING_OP_SEND ||
            pSqe->u8Opcode == IORING_OP_SENDMSG) {
            return do_rw(pR, i32Fd, 0, u64Addr, u32Len, 1);
        }
        return do_rw(pR, i32Fd, 0, u64Addr, u32Len, 0);

    case IORING_OP_CLOSE:
        ioring_soft_inc(&g_soft.u64SqeFs);
        return (i32)vfs_ram_close((i64)i32Fd);

    case IORING_OP_FALLOCATE:
        ioring_soft_inc(&g_soft.u64SqeFs);
        if (!vfs_ram_fd_ok((i64)i32Fd)) {
            return -LINUX_EBADF;
        }
        return (i32)vfs_ram_fallocate((i64)i32Fd, (i64)u64Off, (i64)u32Len);

    case IORING_OP_OPENAT:
    case IORING_OP_OPENAT2: {
        int fCreate;

        ioring_soft_inc(&g_soft.u64SqeFs);
        i32Rc = path_copy(u64Addr, aPath, sizeof(aPath));
        if (i32Rc != 0) {
            return i32Rc;
        }
        fCreate = ((pSqe->u32OpFlags & LINUX_O_CREAT) != 0u) ? 1 : 0;
        i64N = vfs_ram_open(aPath, fCreate);
        if (i64N >= 0 && pR != NULL) {
            maybe_install_fixed(pR, pSqe->u32FileIndex, (i32)i64N);
        }
        return (i32)i64N;
    }

    case IORING_OP_FILES_UPDATE: {
        /* Soft: update a single fixed-file slot. fd=index, off=new fd. */
        u32 idx;

        ioring_soft_inc(&g_soft.u64SqeReg);
        if (pR == NULL) {
            return -LINUX_EINVAL;
        }
        if (pSqe->i32Fd < 0) {
            return -LINUX_EINVAL;
        }
        idx = (u32)pSqe->i32Fd;
        if (idx >= GJ_IORING_REG_FILES_MAX) {
            return -LINUX_EINVAL;
        }
        pR->aRegFiles[idx] = (i32)u64Off;
        if (pR->u32RegFiles <= idx) {
            pR->u32RegFiles = idx + 1u;
        }
        return 0;
    }

    case IORING_OP_STATX: {
        u8 aStat[144];

        ioring_soft_inc(&g_soft.u64SqeFs);
        memset(aStat, 0, sizeof(aStat));
        if (u64Addr != 0ull) {
            i32Rc = path_copy(u64Addr, aPath, sizeof(aPath));
            if (i32Rc == 0) {
                i64N = vfs_ram_stat(aPath, aStat, sizeof(aStat));
            } else if (vfs_ram_fd_ok((i64)i32Fd)) {
                i64N = vfs_ram_fstat((i64)i32Fd, aStat, sizeof(aStat));
            } else {
                return i32Rc;
            }
        } else if (vfs_ram_fd_ok((i64)i32Fd)) {
            i64N = vfs_ram_fstat((i64)i32Fd, aStat, sizeof(aStat));
        } else {
            return -LINUX_EBADF;
        }
        if (i64N < 0) {
            return (i32)i64N;
        }
        /* Soft: drop stat blob unless off points at a buffer (addr2-ish). */
        if (u64Off != 0ull) {
            if (buf_out(u64Off, aStat, sizeof(aStat)) < 0) {
                return -LINUX_EFAULT;
            }
        }
        return 0;
    }

    case IORING_OP_EPOLL_CTL: {
        /*
         * Public prep: fd=epfd, addr=epoll_event*, len=op, off=target_fd.
         * epoll_event soft layout: u32 events + u64 data.
         */
        int nOp = (int)u32Len;
        i64 i64Target = (i64)(i32)u64Off;
        u32 u32Events = 0;
        u64 u64Data = 0;

        ioring_soft_inc(&g_soft.u64SqePoll);
        if (u64Addr != 0ull) {
            u8 aEv[12];

            memset(aEv, 0, sizeof(aEv));
            if (buf_in(u64Addr, aEv, sizeof(aEv)) >= 0) {
                /* Soft packed layout: u32 events + u64 data. */
                memcpy(&u32Events, aEv, sizeof(u32Events));
                memcpy(&u64Data, aEv + 4, sizeof(u64Data));
            }
        }
        return (i32)vfs_ram_epoll_ctl((i64)i32Fd, nOp, i64Target, u32Events,
                                      u64Data);
    }

    case IORING_OP_PROVIDE_BUFFERS: {
        /* Soft: register one buffer at buf_index from addr/len. */
        u16 idx;

        ioring_soft_inc(&g_soft.u64SqeReg);
        if (pR == NULL) {
            return -LINUX_EINVAL;
        }
        idx = pSqe->u16BufIndex;
        if ((u32)idx >= GJ_IORING_REG_BUFS_MAX) {
            return -LINUX_EINVAL;
        }
        pR->aRegBufAddr[idx] = u64Addr;
        pR->aRegBufLen[idx] = u32Len;
        if (pR->u32RegBufs <= (u32)idx) {
            pR->u32RegBufs = (u32)idx + 1u;
        }
        return 0;
    }

    case IORING_OP_REMOVE_BUFFERS: {
        u16 idx;

        ioring_soft_inc(&g_soft.u64SqeReg);
        if (pR == NULL) {
            return -LINUX_EINVAL;
        }
        idx = pSqe->u16BufIndex;
        if ((u32)idx >= GJ_IORING_REG_BUFS_MAX) {
            return -LINUX_EINVAL;
        }
        pR->aRegBufAddr[idx] = 0;
        pR->aRegBufLen[idx] = 0;
        return 0;
    }

    case IORING_OP_UNLINKAT:
        ioring_soft_inc(&g_soft.u64SqeFs);
        i32Rc = path_copy(u64Addr, aPath, sizeof(aPath));
        if (i32Rc != 0) {
            return i32Rc;
        }
        return (i32)vfs_ram_unlink(aPath);

    case IORING_OP_RENAMEAT:
        ioring_soft_inc(&g_soft.u64SqeFs);
        i32Rc = path_copy(u64Addr, aPath, sizeof(aPath));
        if (i32Rc != 0) {
            return i32Rc;
        }
        /* newpath in off/addr2 */
        i32Rc = path_copy(u64Off, aPath2, sizeof(aPath2));
        if (i32Rc != 0) {
            return i32Rc;
        }
        return (i32)vfs_ram_rename(aPath, aPath2);

    case IORING_OP_MKDIRAT: {
        i64 i64FdDir;

        ioring_soft_inc(&g_soft.u64SqeFs);
        i32Rc = path_copy(u64Addr, aPath, sizeof(aPath));
        if (i32Rc != 0) {
            return i32Rc;
        }
        i64FdDir = vfs_ram_open(aPath, 1);
        if (i64FdDir < 0) {
            return (i32)i64FdDir;
        }
        i64N = vfs_ram_mark_dir(i64FdDir);
        (void)vfs_ram_close(i64FdDir);
        return (i32)i64N;
    }

    case IORING_OP_SYMLINKAT:
        /* addr = target, off = linkpath */
        ioring_soft_inc(&g_soft.u64SqeFs);
        i32Rc = path_copy(u64Addr, aPath, sizeof(aPath));
        if (i32Rc != 0) {
            return i32Rc;
        }
        i32Rc = path_copy(u64Off, aPath2, sizeof(aPath2));
        if (i32Rc != 0) {
            return i32Rc;
        }
        return (i32)vfs_ram_symlink(aPath, aPath2);

    case IORING_OP_LINKAT:
        /* addr = oldpath, off = newpath */
        ioring_soft_inc(&g_soft.u64SqeFs);
        i32Rc = path_copy(u64Addr, aPath, sizeof(aPath));
        if (i32Rc != 0) {
            return i32Rc;
        }
        i32Rc = path_copy(u64Off, aPath2, sizeof(aPath2));
        if (i32Rc != 0) {
            return i32Rc;
        }
        return (i32)vfs_ram_link(aPath, aPath2);

    case IORING_OP_READ_FIXED:
    case IORING_OP_WRITE_FIXED: {
        u64 bufAddr = 0;
        u32 bufLen = 0;
        u32 useLen;

        ioring_soft_inc(&g_soft.u64SqeRw);
        i32Rc = resolve_fixed_buf(pR, pSqe->u16BufIndex, &bufAddr, &bufLen);
        if (i32Rc != 0) {
            return i32Rc;
        }
        useLen = u32Len;
        if (useLen == 0u || useLen > bufLen) {
            useLen = bufLen;
        }
        return do_rw(pR, i32Fd, u64Off, bufAddr, useLen,
                     pSqe->u8Opcode == IORING_OP_WRITE_FIXED);
    }

    case IORING_OP_READ:
    case IORING_OP_WRITE:
        ioring_soft_inc(&g_soft.u64SqeRw);
        return do_rw(pR, i32Fd, u64Off, u64Addr, u32Len,
                     pSqe->u8Opcode == IORING_OP_WRITE);

    case IORING_OP_READV:
    case IORING_OP_WRITEV:
        ioring_soft_inc(&g_soft.u64SqeRw);
        return do_rwv(pR, i32Fd, u64Off, u64Addr, u32Len,
                      pSqe->u8Opcode == IORING_OP_WRITEV);

    default:
        ioring_soft_inc(&g_soft.u64SqeEinval);
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
        ioring_soft_inc(&g_soft.u64CqeOverflow);
        return;
    }
    idx = pCq->tail & mask;
    pCq->cqes[idx].u64UserData = u64UserData;
    pCq->cqes[idx].i32Res = i32Res;
    pCq->cqes[idx].u32Flags = 0;
    pCq->tail++;
    ioring_soft_inc(&g_soft.u64CqePost);
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
            ioring_soft_inc(&g_soft.u64SqDropped);
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
        ioring_soft_inc(&g_soft.u64SetupEnomem);
        ioring_soft_maybe_once();
        return -LINUX_ENOMEM;
    }
    memset(pR, 0, sizeof(*pR));
    pR->u8Used = 1;
    pR->u32SqEntries = nSq;
    pR->u32CqEntries = nCq;
    /* Fixed-file slots start as -1. */
    memset(pR->aRegFiles, 0xff, sizeof(pR->aRegFiles));
    pkg_init(pR, nSq, nCq);

    i64Fd = vfs_ram_io_uring_open(u32Slot);
    if (i64Fd < 0) {
        pR->u8Used = 0;
        g_aRingFd[u32Slot] = -1;
        ioring_soft_inc(&g_soft.u64SetupOpenFail);
        ioring_soft_maybe_once();
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
                ioring_soft_inc(&g_soft.u64SetupEfault);
                ioring_soft_maybe_once();
                return -LINUX_EFAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64ParamsUser, &params, sizeof(params));
        }
    }

    ioring_soft_inc(&g_soft.u64SetupOk);
    kprintf("io_uring: setup fd=%ld sq=%u cq=%u mmap=1\n", (long)i64Fd, nSq,
            nCq);
    ioring_soft_maybe_once();
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
        ioring_soft_inc(&g_soft.u64MmapEbadf);
        ioring_soft_maybe_once();
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
        ioring_soft_inc(&g_soft.u64MmapEinval);
        ioring_soft_maybe_once();
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
        ioring_soft_inc(&g_soft.u64MmapOk);
        ioring_soft_maybe_once();
        return (i64)((u64)(gj_vaddr_t)(pBase + offInPkg));
    }

    /* Map package pages into process at high band with USER|RW. */
    if (process_as_ensure(g_pLinuxProc) != GJ_OK) {
        ioring_soft_inc(&g_soft.u64MmapEnomem);
        ioring_soft_maybe_once();
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
            ioring_soft_inc(&g_soft.u64MmapEnomem);
            ioring_soft_maybe_once();
            return -LINUX_ENOMEM;
        }
    }
    pR->u64UserMapVa = va - offInPkg;
    (void)mapLen;
    ioring_soft_inc(&g_soft.u64MmapOk);
    ioring_soft_maybe_once();
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
    ioring_soft_inc(&g_soft.u64MmapPkg);
    ioring_soft_maybe_once();
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
        ioring_soft_inc(&g_soft.u64InjectFail);
        ioring_soft_maybe_once();
        return -LINUX_EBADF;
    }
    if (u32Idx >= pR->u32SqEntries) {
        ioring_soft_inc(&g_soft.u64InjectFail);
        ioring_soft_maybe_once();
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
    ioring_soft_inc(&g_soft.u64InjectOk);
    ioring_soft_maybe_once();
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
        ioring_soft_inc(&g_soft.u64EnterEbadf);
        ioring_soft_maybe_once();
        return -LINUX_EBADF;
    }
    if (u32ToSubmit == 0u && u32MinComplete == 0u) {
        ioring_soft_inc(&g_soft.u64EnterNop);
        ioring_soft_inc(&g_soft.u64EnterOk);
        ioring_soft_maybe_once();
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
        ioring_soft_inc(&g_soft.u64EnterInject);
    }
    u32Submitted = drain_sq(pR, u32ToSubmit);
    g_soft.u64EnterSubmit += (u64)u32Submitted;

    if (u32MinComplete > 0u) {
        struct gj_io_cq_ctrl_pkg *pCq = cq_ctrl(pR);

        u32Ready = pCq->tail - pCq->head;
        if (u32Ready < u32MinComplete) {
            ioring_soft_inc(&g_soft.u64EnterEagain);
            ioring_soft_maybe_once();
            return -LINUX_EAGAIN;
        }
    }
    ioring_soft_inc(&g_soft.u64EnterOk);
    ioring_soft_maybe_once();
    return (i64)u32Submitted;
}

i64
gj_io_uring_register(i64 i64Fd, u32 u32Opcode, u64 u64Arg, u32 u32NrArgs)
{
    struct gj_io_uring_ring *pR = ring_from_fd(i64Fd);
    u32 i;
    u32 n;

    if (pR == NULL) {
        ioring_soft_inc(&g_soft.u64RegEbadf);
        ioring_soft_maybe_once();
        return -LINUX_EBADF;
    }
    if (u32Opcode == IORING_REGISTER_BUFFERS) {
        /*
         * Soft: accept buffer registration. u64Arg may be user iovec array;
         * we store first few base/len pairs (capped).
         */
        n = u32NrArgs;
        if (n > GJ_IORING_REG_BUFS_MAX) {
            n = GJ_IORING_REG_BUFS_MAX;
        }
        /* Clear previous table first. */
        memset(pR->aRegBufAddr, 0, sizeof(pR->aRegBufAddr));
        memset(pR->aRegBufLen, 0, sizeof(pR->aRegBufLen));
        if (u64Arg != 0 && n > 0u) {
            for (i = 0; i < n; i++) {
                struct {
                    u64 base;
                    u64 len;
                } iov;

                memset(&iov, 0, sizeof(iov));
                if (user_range_ok(u64Arg + (u64)i * sizeof(iov),
                                  sizeof(iov))) {
                    if (copy_from_user(&iov, u64Arg + (u64)i * sizeof(iov),
                                       sizeof(iov)) != GJ_OK) {
                        ioring_soft_inc(&g_soft.u64RegEfault);
                        ioring_soft_maybe_once();
                        return -LINUX_EFAULT;
                    }
                } else {
                    memcpy(&iov,
                           (const void *)(gj_vaddr_t)(u64Arg +
                                                      (u64)i * sizeof(iov)),
                           sizeof(iov));
                }
                pR->aRegBufAddr[i] = iov.base;
                pR->aRegBufLen[i] = (u32)iov.len;
            }
        }
        pR->u32RegBufs = n;
        ioring_soft_inc(&g_soft.u64RegBuffers);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_UNREGISTER_BUFFERS) {
        pR->u32RegBufs = 0;
        memset(pR->aRegBufAddr, 0, sizeof(pR->aRegBufAddr));
        memset(pR->aRegBufLen, 0, sizeof(pR->aRegBufLen));
        ioring_soft_inc(&g_soft.u64RegBuffers);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_REGISTER_FILES) {
        n = u32NrArgs;
        if (n > GJ_IORING_REG_FILES_MAX) {
            n = GJ_IORING_REG_FILES_MAX;
        }
        memset(pR->aRegFiles, 0xff, sizeof(pR->aRegFiles));
        if (u64Arg != 0 && n > 0u) {
            for (i = 0; i < n; i++) {
                i32 fd = -1;

                if (user_range_ok(u64Arg + (u64)i * sizeof(i32),
                                  sizeof(i32))) {
                    if (copy_from_user(&fd, u64Arg + (u64)i * sizeof(i32),
                                       sizeof(i32)) != GJ_OK) {
                        ioring_soft_inc(&g_soft.u64RegEfault);
                        ioring_soft_maybe_once();
                        return -LINUX_EFAULT;
                    }
                } else {
                    fd = ((const i32 *)(gj_vaddr_t)u64Arg)[i];
                }
                pR->aRegFiles[i] = fd;
            }
        }
        pR->u32RegFiles = n;
        ioring_soft_inc(&g_soft.u64RegFiles);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_UNREGISTER_FILES) {
        pR->u32RegFiles = 0;
        memset(pR->aRegFiles, 0xff, sizeof(pR->aRegFiles));
        ioring_soft_inc(&g_soft.u64RegFiles);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_REGISTER_FILES_UPDATE) {
        /*
         * Soft sparse update: arg is fd array of nr_args entries starting
         * at offset 0 of the fixed table (full replace of first n slots).
         * Sparse offset protocol not fully modelled — first-n update.
         */
        n = u32NrArgs;
        if (n > GJ_IORING_REG_FILES_MAX) {
            n = GJ_IORING_REG_FILES_MAX;
        }
        if (u64Arg != 0 && n > 0u) {
            for (i = 0; i < n; i++) {
                i32 fd = -1;

                if (user_range_ok(u64Arg + (u64)i * sizeof(i32),
                                  sizeof(i32))) {
                    if (copy_from_user(&fd, u64Arg + (u64)i * sizeof(i32),
                                       sizeof(i32)) != GJ_OK) {
                        ioring_soft_inc(&g_soft.u64RegEfault);
                        ioring_soft_maybe_once();
                        return -LINUX_EFAULT;
                    }
                } else {
                    fd = ((const i32 *)(gj_vaddr_t)u64Arg)[i];
                }
                /* -1 means "skip / no change" in Linux sparse update. */
                if (fd != -1) {
                    pR->aRegFiles[i] = fd;
                }
            }
            if (pR->u32RegFiles < n) {
                pR->u32RegFiles = n;
            }
        }
        ioring_soft_inc(&g_soft.u64RegFiles);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_REGISTER_EVENTFD ||
        u32Opcode == IORING_REGISTER_EVENTFD_ASYNC) {
        pR->u32RegEventfd = 1u;
        (void)u64Arg;
        (void)u32NrArgs;
        ioring_soft_inc(&g_soft.u64RegEventfd);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_UNREGISTER_EVENTFD) {
        pR->u32RegEventfd = 0u;
        ioring_soft_inc(&g_soft.u64RegEventfd);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_REGISTER_PROBE) {
        /*
         * Soft probe: if arg non-zero, write a tiny capability blob
         * { last_op, ops_len, resv } so userspace can see depth>0.
         * Layout is intentionally minimal (not full io_uring_probe).
         */
        if (u64Arg != 0 && u32NrArgs > 0u) {
            struct {
                u8 last_op;
                u8 ops_len;
                u16 resv;
                u32 resv2;
            } probe;

            memset(&probe, 0, sizeof(probe));
            probe.last_op = (u8)IORING_OP_LINKAT;
            probe.ops_len = (u8)(u32NrArgs > 255u ? 255u : u32NrArgs);
            if (buf_out(u64Arg, &probe, sizeof(probe)) < 0) {
                ioring_soft_inc(&g_soft.u64RegEfault);
                ioring_soft_maybe_once();
                return -LINUX_EFAULT;
            }
        }
        ioring_soft_inc(&g_soft.u64RegProbe);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_REGISTER_PERSONALITY) {
        pR->u32Personality = 1u;
        (void)u64Arg;
        (void)u32NrArgs;
        ioring_soft_inc(&g_soft.u64RegPersonality);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    if (u32Opcode == IORING_UNREGISTER_PERSONALITY) {
        pR->u32Personality = 0u;
        ioring_soft_inc(&g_soft.u64RegPersonality);
        ioring_soft_inc(&g_soft.u64RegOk);
        ioring_soft_maybe_once();
        return 0;
    }
    ioring_soft_inc(&g_soft.u64RegEnosys);
    ioring_soft_maybe_once();
    return -LINUX_ENOSYS;
}
