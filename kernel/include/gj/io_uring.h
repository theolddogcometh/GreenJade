/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * io_uring product surface: setup / enter / register + SQE I/O + SQ/CQ mmap.
 * Clean-room public Linux ABI opcode numbers and mmap offsets. No GPL source.
 *
 * Status (soft ship — vfs_ram / kernel smoke, not game/title I/O):
 *   setup/enter/register  — greppable `linux: io_uring min rings PASS`
 *   package mmap + fill   — greppable `linux: io_uring mmap PASS`
 *   SQE path (RW/fs/path) — greppable `linux: io_uring SQE I/O PASS`
 *   fixed files+bufs      — greppable `linux: io_uring register depth PASS`
 *   more opcodes          — greppable `linux: io_uring more opcodes PASS`
 * Grep: io_uring: setup, kernel/syscall/io_uring_min.c
 *
 * IORING_FEAT_SINGLE_MMAP-shaped package (page-aligned, 3 pages):
 *   +0x0000  SQ control: head,tail,mask,entries,flags,dropped + array[]
 *            (array advertised at offset 64 within SQ page)
 *   +0x1000  CQ control: head,tail,mask,entries,overflow,flags + cqes[]
 *            (cqes advertised at offset 64 within CQ page)
 *   +0x2000  SQEs[entries]  (64-byte SQE × power-of-two entries, max 64)
 *
 * Userspace flow:
 *   1. gj_io_uring_setup → ring fd + params (sq/cq entries, sq_off/cq_off)
 *   2. mmap(fd, OFF_SQ_RING|0) or gj_io_uring_mmap_package (kernel smoke)
 *   3. fill SQE slots, set SQ array[], advance sq.tail
 *   4. gj_io_uring_enter(to_submit, min_complete) → drain SQ, post CQEs
 *   5. peek/advance CQ (or walk cqes[head..tail] after mmap)
 *
 * Soft inject path (kernel smokes without userspace fill):
 *   empty SQ + enter(to_submit>0) synthetic-advances tail over inject slots;
 *   gj_io_uring_sqe_inject writes one SQE and array[idx]=idx.
 *
 * Limits (implementation, not Linux upper bounds):
 *   rings pool ≤ 4; SQ entries power-of-two clamp ≤ 64; CQ = 2×SQ (≤ 128);
 *   reg files ≤ 16; reg buffers ≤ 8; per-op bounce I/O ≤ 4096 bytes.
 *   Accept/connect and true net/title I/O remain open (-ENOSYS soft).
 *
 * docs/IMPLEMENTATION.md · docs/STEAM_HWTEST.md · TODO io_uring soft PASS
 */
#pragma once

#include <gj/types.h>

/* ---- SQE opcodes (Linux uapi subset — public numbers) ------------------- */
/*
 * Soft exec depth (io_uring_min.c): NOP, FSYNC, CLOSE, READ/WRITE/OPENAT
 * path ops, POLL soft-immediate, timeouts/cancel soft-0, fixed-file index,
 * PROVIDE/REMOVE_BUFFERS. ACCEPT/CONNECT -> -ENOSYS. SEND/RECV -> ram fd R/W.
 */
#define GJ_IORING_OP_NOP              0u
#define GJ_IORING_OP_READV            1u
#define GJ_IORING_OP_WRITEV           2u
#define GJ_IORING_OP_FSYNC            3u
#define GJ_IORING_OP_READ_FIXED       4u  /* uses registered buffer index */
#define GJ_IORING_OP_WRITE_FIXED      5u
#define GJ_IORING_OP_POLL_ADD         6u  /* soft: immediate ready mask */
#define GJ_IORING_OP_POLL_REMOVE      7u  /* soft: no wait-list */
#define GJ_IORING_OP_SYNC_FILE_RANGE  8u
#define GJ_IORING_OP_SENDMSG          9u  /* soft: ram fd write path */
#define GJ_IORING_OP_RECVMSG         10u  /* soft: ram fd read path */
#define GJ_IORING_OP_TIMEOUT         11u  /* soft: complete immediately */
#define GJ_IORING_OP_TIMEOUT_REMOVE  12u
#define GJ_IORING_OP_ACCEPT          13u  /* soft: -ENOSYS when fd ok */
#define GJ_IORING_OP_ASYNC_CANCEL    14u  /* soft: no async table; success */
#define GJ_IORING_OP_LINK_TIMEOUT    15u
#define GJ_IORING_OP_CONNECT         16u  /* soft: -ENOSYS when fd ok */
#define GJ_IORING_OP_FALLOCATE       17u
#define GJ_IORING_OP_OPENAT          18u
#define GJ_IORING_OP_CLOSE           19u
#define GJ_IORING_OP_FILES_UPDATE   20u  /* soft: one fixed-file slot */
#define GJ_IORING_OP_STATX           21u
#define GJ_IORING_OP_READ            22u
#define GJ_IORING_OP_WRITE           23u
#define GJ_IORING_OP_FADVISE         24u  /* soft: live-fd success */
#define GJ_IORING_OP_MADVISE         25u  /* soft: no-op success */
#define GJ_IORING_OP_SEND            26u
#define GJ_IORING_OP_RECV            27u
#define GJ_IORING_OP_OPENAT2         28u
#define GJ_IORING_OP_EPOLL_CTL       29u
/* 30 reserved in public Linux numbering */
#define GJ_IORING_OP_PROVIDE_BUFFERS 31u
#define GJ_IORING_OP_REMOVE_BUFFERS  32u
/* 33 reserved */
#define GJ_IORING_OP_SHUTDOWN        34u
#define GJ_IORING_OP_RENAMEAT        35u
#define GJ_IORING_OP_UNLINKAT        36u
#define GJ_IORING_OP_MKDIRAT         37u
#define GJ_IORING_OP_SYMLINKAT       38u
#define GJ_IORING_OP_LINKAT          39u

/* ---- SQE flags (public Linux bits; FIXED_FILE wired) -------------------- */
#define GJ_IOSQE_FIXED_FILE          (1u << 0) /* fd = index into reg files */
#define GJ_IOSQE_IO_DRAIN            (1u << 1) /* catalog; not ordered yet */
#define GJ_IOSQE_IO_LINK             (1u << 2)
#define GJ_IOSQE_IO_HARDLINK         (1u << 3)
#define GJ_IOSQE_ASYNC               (1u << 4)
#define GJ_IOSQE_BUFFER_SELECT       (1u << 5)

/* ---- Register opcodes (Linux uapi subset) ------------------------------- */
/*
 * Soft tables live on the ring object (not a full io_uring_register kernel
 * identity). PROBE writes a tiny capability blob when arg non-zero.
 * PERSONALITY accept-only (no multi-personality dispatch yet).
 */
#define GJ_IORING_REGISTER_BUFFERS          0u /* iovec[nr]; cap 8 */
#define GJ_IORING_UNREGISTER_BUFFERS        1u
#define GJ_IORING_REGISTER_FILES            2u /* i32[nr]; cap 16; -1 empty */
#define GJ_IORING_UNREGISTER_FILES          3u
#define GJ_IORING_REGISTER_EVENTFD          4u /* soft flag only */
#define GJ_IORING_UNREGISTER_EVENTFD        5u
#define GJ_IORING_REGISTER_FILES_UPDATE  6u /* first-n sparse; -1 = skip */
#define GJ_IORING_REGISTER_EVENTFD_ASYNC    7u /* soft: same as EVENTFD */
#define GJ_IORING_REGISTER_PROBE            8u
#define GJ_IORING_REGISTER_PERSONALITY      9u
#define GJ_IORING_UNREGISTER_PERSONALITY   10u

/* ---- Public mmap offsets (Linux uapi) ----------------------------------- */
/*
 * With SINGLE_MMAP, all three offsets resolve into one package:
 *   OFF_SQ_RING / 0  → SQ page base
 *   OFF_CQ_RING      → CQ page base (+0x1000 in package)
 *   OFF_SQES         → SQE array page (+0x2000 in package)
 * Params returned by setup describe field offsets relative to package base.
 */
#define GJ_IORING_OFF_SQ_RING  0ull
#define GJ_IORING_OFF_CQ_RING  0x8000000ull
#define GJ_IORING_OFF_SQES     0x10000000ull

/* ---- Lifecycle / syscalls ----------------------------------------------- */

/**
 * Create a ring: clamp entries to power-of-two ≤ 64 (default 8 if 0).
 * CQ entries = 2×SQ (capped). Allocates a fixed pool slot (max 4 rings).
 * Opens a vfs_ram io_uring fd. When u64ParamsUser != 0, writes Linux-shaped
 * io_uring_params (sq/cq entries, IORING_FEAT_SINGLE_MMAP, sq_off/cq_off).
 *
 * Returns ring fd (≥0) or -errno: -ENOMEM pool full, -EFAULT bad user params.
 * Grep: io_uring: setup
 */
i64 gj_io_uring_setup(u32 u32Entries, u64 u64ParamsUser);

/**
 * Submit and optionally wait for completions.
 * Drains up to u32ToSubmit pending SQ entries (sq.tail − sq.head). Soft inject
 * path: if SQ empty and u32ToSubmit>0, synthetic-advances tail so inject slots
 * run. u32Flags currently ignored (no IOPOLL / GETEVENTS product bits).
 *
 * If u32MinComplete>0 and CQ pending < min, returns -EAGAIN after submit.
 * Returns number of SQEs submitted (≥0) or -EBADF.
 */
i64 gj_io_uring_enter(i64 i64Fd, u32 u32ToSubmit, u32 u32MinComplete,
                      u32 u32Flags);

/**
 * Register / unregister buffers, files, eventfd, probe, personality.
 * See GJ_IORING_REGISTER_* above for soft semantics and caps.
 * Returns 0 or -errno (-EBADF, -EFAULT, -EINVAL for unknown opcode).
 */
i64 gj_io_uring_register(i64 i64Fd, u32 u32Opcode, u64 u64Arg, u32 u32NrArgs);

/**
 * Map ring package (or a slice) for userspace fill.
 * u64Offset = GJ_IORING_OFF_* or 0 for SQ/full base; also accepts raw
 * package-relative offsets < package size. u64Len 0 ⇒ remainder of package.
 *
 * With active linux process AS: maps 3 package pages USER|RW at a high-band
 * VA and returns the requested slice VA. Without process (kernel smoke):
 * returns identity VA of the package slice.
 * Returns VA (≥0) or -errno (-EBADF, -EINVAL, -ENOMEM).
 */
i64 gj_io_uring_mmap(i64 i64Fd, u64 u64Offset, u64 u64Len);

/**
 * Kernel/smoke: pointer to full package base (no AS map).
 * Lets smokes fill SQEs/array/tail without inject. NULL if bad fd.
 */
void *gj_io_uring_mmap_package(i64 i64Fd);

/**
 * Kernel inject helper: write one SQE at index u32Idx and set array[idx]=idx.
 * Does not advance tail — enter(to_submit) does (or soft empty-SQ advance).
 * Returns 0 or -errno (-EBADF, -EINVAL if idx ≥ sq_entries).
 */
i64 gj_io_uring_sqe_inject(i64 i64Fd, u32 u32Idx, u8 u8Opcode, i32 i32Fd,
                           u64 u64Off, u64 u64Addr, u32 u32Len,
                           u64 u64UserData);

/**
 * Non-destructive peek at CQ head. Fills *pUserData / *pRes when non-NULL.
 * Returns 0, -EAGAIN if CQ empty, or -EBADF.
 */
i64 gj_io_uring_cqe_peek(i64 i64Fd, u64 *pUserData, i32 *pRes);

/**
 * Consume one CQE (advance cq.head). Returns 0, -EAGAIN empty, or -EBADF.
 */
i64 gj_io_uring_cqe_advance(i64 i64Fd);

/** Non-zero if i64Fd names a live ring in the fixed pool. */
int gj_io_uring_fd_ok(i64 i64Fd);

/**
 * One-shot init: clear ring pool + fd map. Idempotent; setup auto-calls.
 * Grep: io_uring: min rings ready
 */
void gj_io_uring_init(void);
