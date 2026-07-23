/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * VFS door — multi-client file namespace over store/LBA mini-FS (product stack).
 * Pure C11 dual-license surface for vfsd / shell / native GJ_SYS_VFS clients.
 *
 * Soft claim path: token 0 is invalid; same non-zero token re-CLAIM is
 * idempotent (reclaim soft); a different token returns BUSY. RELEASE when
 * free is a soft no-op (0). Format/mount are allowed when free (bring-up)
 * or claimed (product vfsd path).
 */
#pragma once

#include <gj/types.h>

/** claim: arg1=token — vfsd ownership for format (32-bit, non-zero) */
#define GJ_VFS_OP_CLAIM    1u
/** release: arg1=token (must match when owned; soft 0 when free) */
#define GJ_VFS_OP_RELEASE  2u
/** format: write super+freemap+empty inodes (requires claim or free) */
#define GJ_VFS_OP_FORMAT   3u
/** mount: verify super; arg1=user u32[4] optional {magic,ver,files,data0} */
#define GJ_VFS_OP_MOUNT    4u
/**
 * create: arg1=name cstr, arg2=user data, arg3=bytes (≤FILE_MAX)
 * → inode index ≥0
 */
#define GJ_VFS_OP_CREATE   5u
/**
 * read: arg1=name, arg2=user buf, arg3=max bytes
 * → bytes read
 */
#define GJ_VFS_OP_READ     6u
/**
 * write: arg1=name, arg2=user data, arg3=bytes — overwrite or create
 * → bytes written
 */
#define GJ_VFS_OP_WRITE    7u
/** unlink: arg1=name → 0; invalidates open door fds on that inode */
#define GJ_VFS_OP_UNLINK   8u
/** stat: arg1=name, arg2=user u32[2] {size, data_lba} → 0 */
#define GJ_VFS_OP_STAT     9u
/** list: arg1=user buf (names separated by \n), arg3=buf size → bytes */
#define GJ_VFS_OP_LIST     10u
/** stats: arg1=user u32[4] {calls, files, owned, mounted} */
#define GJ_VFS_OP_STATS    11u
/**
 * open: arg1=name path (accepts /mnt/foo or foo), arg2=flags
 *   flags: bit0=create, bit1=write (else read-only)
 * → door fd ≥ 0 (not Linux fd)
 */
#define GJ_VFS_OP_OPEN     12u
/** close: arg1=door fd → 0 */
#define GJ_VFS_OP_CLOSE    13u
/** readfd: arg1=door fd, arg2=user buf, arg3=max → bytes (advances offset) */
#define GJ_VFS_OP_READFD   14u
/** writefd: arg1=door fd, arg2=user buf, arg3=len → bytes (write at offset) */
#define GJ_VFS_OP_WRITEFD  15u
/**
 * seekfd: arg1=door fd, arg2=offset (absolute from 0), arg3=whence
 *   whence 0=SET 1=CUR 2=END → new absolute offset
 */
#define GJ_VFS_OP_SEEKFD   16u

#define GJ_VFS_O_CREAT     1u
#define GJ_VFS_O_RDWR      2u

#define GJ_VFS_SEEK_SET    0u
#define GJ_VFS_SEEK_CUR    1u
#define GJ_VFS_SEEK_END    2u

void vfs_door_init(void);
i64  vfs_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);
int  vfs_door_owned(void);
int  vfs_door_mounted(void);
/** Open door-fd count (smoke / diagnostics). */
u32  vfs_door_fd_count(void);
/** Current owner token, or 0 if free (kernel interim / unclaimed). */
u32  vfs_door_owner_token(void);
/** Soft path: first-claim count (excludes idempotent reclaims). */
u32  vfs_door_claim_count(void);
