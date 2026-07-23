/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * VFS door — multi-client file namespace over store/LBA mini-FS (product stack).
 * Pure C11 freestanding dual MIT OR Apache-2.0 surface for vfsd / shell /
 * native GJ_SYS_VFS clients.
 *
 * Dispatch contract:
 *   vfs_door_call(op, arg1, arg2, arg3) → i64
 *   User pointers: copy_{to,from}_user when in user VA window; kernel
 *   smokes may pass HHDM/static buffers (same pattern as store/session).
 *
 * On-disk layout (v3 — fixed field order; keep ABI/format stable):
 *   LBA0 super (magic 0x31444a47 "GJD1", version 3)
 *   LBA1 freemap
 *   LBA2–3 inode table (INODE_MAX=8, 2 sectors)
 *   LBA4+ data (FILE_MAX_BYTES = 2 sectors = 1 KiB per file)
 *   Soft RAM disk (VFS_RAM_SECS) when virtio-blk is absent.
 *   R/W prefers store_door when virtio-blk ready (product storaged path).
 *
 * Ownership (token policy shared with net/store/session doors):
 *   token 0 invalid for CLAIM; same non-zero token re-CLAIM is idempotent
 *   (reclaim soft); different token → BUSY. RELEASE when free is soft 0.
 *   Format/mount allowed when free (bring-up) or claimed (product vfsd).
 *
 * Door fds (not Linux fds):
 *   VFS_FD_MAX=16 open slots; OPEN normalizes /mnt/foo and bare names.
 *   UNLINK invalidates open door fds on that inode.
 *   READFD/WRITEFD advance offset; SEEKFD absolute/relative/end.
 *
 * Greppable product markers (keep stable):
 *   vfs_door: PASS / ownership / claim soft / multi-client mini-FS
 */
#pragma once

#include <gj/types.h>

/* ---- door opcodes (GJ_SYS_VFS arg0) ---------------------------------- */

/** claim: arg1=token — vfsd ownership for format (32-bit, non-zero) */
#define GJ_VFS_OP_CLAIM    1u
/** release: arg1=token (must match when owned; soft 0 when free) */
#define GJ_VFS_OP_RELEASE  2u
/**
 * format: write super+freemap+empty inodes (requires claim or free).
 * Soft RAM or store-backed sectors depending on virtio-blk readiness.
 */
#define GJ_VFS_OP_FORMAT   3u
/** mount: verify super; arg1=user u32[4] optional {magic,ver,files,data0} */
#define GJ_VFS_OP_MOUNT    4u
/**
 * create: arg1=name cstr, arg2=user data, arg3=bytes (≤FILE_MAX 1 KiB)
 * → inode index ≥0 or -errno
 */
#define GJ_VFS_OP_CREATE   5u
/**
 * read: arg1=name, arg2=user buf, arg3=max bytes
 * → bytes read (path-based; not door-fd offset)
 */
#define GJ_VFS_OP_READ     6u
/**
 * write: arg1=name, arg2=user data, arg3=bytes — overwrite or create
 * → bytes written (clamped to FILE_MAX)
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
 *   flags: bit0=create (GJ_VFS_O_CREAT), bit1=write (GJ_VFS_O_RDWR)
 * → door fd ≥ 0 (not Linux fd; table is door-private)
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

/** OPEN flags (arg2 low bits). */
#define GJ_VFS_O_CREAT     1u
#define GJ_VFS_O_RDWR      2u

/** SEEKFD whence (arg3). */
#define GJ_VFS_SEEK_SET    0u
#define GJ_VFS_SEEK_CUR    1u
#define GJ_VFS_SEEK_END    2u

/**
 * Init door state (ownership free, not mounted). Safe to re-call soft.
 * Call before GJ_SYS_VFS / vfsd spawn.
 */
void vfs_door_init(void);

/**
 * Dispatch VFS opcode. Returns inode/fd/bytes/0 or negative error.
 * Unknown op / not inited → soft fail (-ENOSYS / -ENODEV shaped).
 */
i64  vfs_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);

/** Non-zero when a userspace owner token is held (vfsd claimed). */
int  vfs_door_owned(void);

/** Non-zero after successful MOUNT (super verified). */
int  vfs_door_mounted(void);

/** Open door-fd count (smoke / diagnostics; 0..VFS_FD_MAX). */
u32  vfs_door_fd_count(void);

/** Current owner token, or 0 if free (kernel interim / unclaimed). */
u32  vfs_door_owner_token(void);

/**
 * Soft path: first-claim count (excludes idempotent reclaims).
 * greppable: vfs_door claim soft
 */
u32  vfs_door_claim_count(void);
