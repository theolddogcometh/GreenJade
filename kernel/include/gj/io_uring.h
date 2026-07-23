/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * io_uring product surface: setup/enter/register + SQE I/O + SQ/CQ mmap.
 */
#pragma once

#include <gj/types.h>

#define GJ_IORING_OP_NOP              0u
#define GJ_IORING_OP_READV            1u
#define GJ_IORING_OP_WRITEV           2u
#define GJ_IORING_OP_FSYNC            3u
#define GJ_IORING_OP_SYNC_FILE_RANGE  8u
#define GJ_IORING_OP_TIMEOUT         11u
#define GJ_IORING_OP_CLOSE           19u
#define GJ_IORING_OP_READ            22u
#define GJ_IORING_OP_WRITE           23u
#define GJ_IORING_OP_FADVISE         24u
#define GJ_IORING_OP_MADVISE         25u

/* Register opcodes (Linux uapi subset). */
#define GJ_IORING_REGISTER_BUFFERS     0u
#define GJ_IORING_REGISTER_FILES       2u
#define GJ_IORING_UNREGISTER_FILES     3u
#define GJ_IORING_REGISTER_EVENTFD     4u

/* Public mmap offsets (Linux uapi). */
#define GJ_IORING_OFF_SQ_RING  0ull
#define GJ_IORING_OFF_CQ_RING  0x8000000ull
#define GJ_IORING_OFF_SQES     0x10000000ull

i64 gj_io_uring_setup(u32 u32Entries, u64 u64ParamsUser);
i64 gj_io_uring_enter(i64 i64Fd, u32 u32ToSubmit, u32 u32MinComplete,
                      u32 u32Flags);
i64 gj_io_uring_register(i64 i64Fd, u32 u32Opcode, u64 u64Arg, u32 u32NrArgs);

/** Map ring package (offset = GJ_IORING_OFF_* or 0 for base). */
i64 gj_io_uring_mmap(i64 i64Fd, u64 u64Offset, u64 u64Len);

/** Kernel/smoke: full package pointer (userspace fill without inject). */
void *gj_io_uring_mmap_package(i64 i64Fd);

i64 gj_io_uring_sqe_inject(i64 i64Fd, u32 u32Idx, u8 u8Opcode, i32 i32Fd,
                           u64 u64Off, u64 u64Addr, u32 u32Len,
                           u64 u64UserData);
i64 gj_io_uring_cqe_peek(i64 i64Fd, u64 *pUserData, i32 *pRes);
i64 gj_io_uring_cqe_advance(i64 i64Fd);

int gj_io_uring_fd_ok(i64 i64Fd);
void gj_io_uring_init(void);
