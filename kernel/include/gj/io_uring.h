/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * io_uring product surface: setup/enter/register + SQE I/O + SQ/CQ mmap.
 * Clean-room public Linux ABI opcode numbers. No GPL source.
 */
#pragma once

#include <gj/types.h>

/* SQE opcodes (Linux uapi subset — public numbers). */
#define GJ_IORING_OP_NOP              0u
#define GJ_IORING_OP_READV            1u
#define GJ_IORING_OP_WRITEV           2u
#define GJ_IORING_OP_FSYNC            3u
#define GJ_IORING_OP_READ_FIXED       4u
#define GJ_IORING_OP_WRITE_FIXED      5u
#define GJ_IORING_OP_POLL_ADD         6u
#define GJ_IORING_OP_POLL_REMOVE      7u
#define GJ_IORING_OP_SYNC_FILE_RANGE  8u
#define GJ_IORING_OP_SENDMSG          9u
#define GJ_IORING_OP_RECVMSG         10u
#define GJ_IORING_OP_TIMEOUT         11u
#define GJ_IORING_OP_TIMEOUT_REMOVE  12u
#define GJ_IORING_OP_ACCEPT          13u
#define GJ_IORING_OP_ASYNC_CANCEL    14u
#define GJ_IORING_OP_LINK_TIMEOUT    15u
#define GJ_IORING_OP_CONNECT         16u
#define GJ_IORING_OP_FALLOCATE       17u
#define GJ_IORING_OP_OPENAT          18u
#define GJ_IORING_OP_CLOSE           19u
#define GJ_IORING_OP_FILES_UPDATE   20u
#define GJ_IORING_OP_STATX           21u
#define GJ_IORING_OP_READ            22u
#define GJ_IORING_OP_WRITE           23u
#define GJ_IORING_OP_FADVISE         24u
#define GJ_IORING_OP_MADVISE         25u
#define GJ_IORING_OP_SEND            26u
#define GJ_IORING_OP_RECV            27u
#define GJ_IORING_OP_OPENAT2         28u
#define GJ_IORING_OP_EPOLL_CTL       29u
#define GJ_IORING_OP_PROVIDE_BUFFERS 31u
#define GJ_IORING_OP_REMOVE_BUFFERS  32u
#define GJ_IORING_OP_SHUTDOWN        34u
#define GJ_IORING_OP_RENAMEAT        35u
#define GJ_IORING_OP_UNLINKAT        36u
#define GJ_IORING_OP_MKDIRAT         37u
#define GJ_IORING_OP_SYMLINKAT       38u
#define GJ_IORING_OP_LINKAT          39u

/* SQE flags (public). */
#define GJ_IOSQE_FIXED_FILE          (1u << 0)
#define GJ_IOSQE_IO_DRAIN            (1u << 1)
#define GJ_IOSQE_IO_LINK             (1u << 2)
#define GJ_IOSQE_IO_HARDLINK         (1u << 3)
#define GJ_IOSQE_ASYNC               (1u << 4)
#define GJ_IOSQE_BUFFER_SELECT       (1u << 5)

/* Register opcodes (Linux uapi subset). */
#define GJ_IORING_REGISTER_BUFFERS          0u
#define GJ_IORING_UNREGISTER_BUFFERS        1u
#define GJ_IORING_REGISTER_FILES            2u
#define GJ_IORING_UNREGISTER_FILES          3u
#define GJ_IORING_REGISTER_EVENTFD          4u
#define GJ_IORING_UNREGISTER_EVENTFD        5u
#define GJ_IORING_REGISTER_FILES_UPDATE  6u
#define GJ_IORING_REGISTER_EVENTFD_ASYNC    7u
#define GJ_IORING_REGISTER_PROBE            8u
#define GJ_IORING_REGISTER_PERSONALITY      9u
#define GJ_IORING_UNREGISTER_PERSONALITY   10u

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
