/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Storage door — storaged hand-off surface (A1 / product path).
 * Clean-room pure C11 freestanding; dual MIT OR Apache-2.0 (no GPL source).
 * Dispatched via GJ_SYS_STORE (arg0 = opcode).
 *
 * Dispatch contract:
 *   store_door_call(op, arg1, arg2, arg3) → i64
 *   User pointers: copy_{to,from}_user when in user VA window; early
 *   kernel smokes may pass HHDM/static buffers.
 *
 * Backend preference:
 *   Sector R/W prefers virtio-blk; scsi_door (scsi_mid CAP/R/W) is the
 *   fallback when blk is absent so CAP/R/W smokes stay green on scsi-only
 *   hosts. Ring EXPORT/MAP/KICK hand the virtio-blk request virtq to UDX
 *   without changing sector semantics.
 *
 * Ownership (token policy shared with net/session/vfs doors):
 *   token 0  = kernel interim owns policy
 *   non-zero = storaged claimed; CLAIM idempotent for same 32-bit token
 *              (reclaim soft); different token → BUSY
 *   RELEASE when free is soft 0; when owned, token must match
 *
 * Soft paths (bring-up / product smoke):
 *   R/W and ring ops are allowed without claim for smokes; storaged
 *   prefers the owned path.
 *   EXPORT/MAP/KICK soft-skip with NODEV when virtio-blk is absent so
 *   hosts without blk stay green on non-hard markers.
 *   RING_STATE always fills free=0 / ready=0 without hard fail.
 *   MAP records last user VA for diagnostics (store_door_ring_map_va).
 *
 * Transfer ceiling:
 *   GJ_STORE_XFER_MAX (4096) — multi-sector within this; bytes must be a
 *   non-zero multiple of 512.
 *
 * Greppable product markers (keep ABI stable):
 *   store_door: PASS / ownership PASS / rw lba1 … PASS /
 *   virtio queue PASS / ring map PASS
 *   store_door claim soft
 */
#pragma once

#include <gj/types.h>

/** Bounce / single-call transfer ceiling (bytes); multi-sector within this. */
#define GJ_STORE_XFER_MAX 4096u

/* ---- door opcodes (GJ_SYS_STORE arg0) -------------------------------- */

/** STATS: arg1 = user u32[3] {blk_io, scsi_io, door_calls} */
#define GJ_STORE_OP_STATS  1u
/** CAP: arg1 = user u64* capacity in 512-byte sectors */
#define GJ_STORE_OP_CAP    2u
/**
 * READ: arg1=lba arg2=user buf arg3=bytes
 * Bytes must be a non-zero multiple of 512 and ≤ GJ_STORE_XFER_MAX.
 * Prefers virtio-blk; scsi_door READ10 fallback when blk absent.
 */
#define GJ_STORE_OP_READ   3u
/**
 * WRITE: arg1=lba arg2=user buf arg3=bytes
 * Same size rules as READ. scsi WRITE10 fallback when blk absent.
 */
#define GJ_STORE_OP_WRITE  4u
/**
 * CLAIM: arg1 = non-zero 32-bit ownership token (idempotent for same token).
 * token 0 → INVAL; different owner → BUSY.
 */
#define GJ_STORE_OP_CLAIM  5u
/** RELEASE: arg1 = claim token (must match when owned; soft 0 when free) */
#define GJ_STORE_OP_RELEASE 6u
/**
 * QUEUE_INFO: arg1 = user u32[4]
 *   {blk_io, scsi_io, door_rw, owned(0/1)}
 */
#define GJ_STORE_OP_QUEUE_INFO 7u
/**
 * FLUSH: fsync-shaped success when virtio-blk or scsi_mid is ready
 * (no barrier wire yet on either transport — soft success).
 */
#define GJ_STORE_OP_FLUSH      8u
/**
 * EXPORT_RING: arg1 = user ptr to gj_virtq_export (virtio-blk request q).
 * Soft: NODEV when virtio-blk is absent.
 */
#define GJ_STORE_OP_EXPORT_RING 9u
/** KICK: notify device of available descriptors on the request queue */
#define GJ_STORE_OP_KICK        10u
/**
 * RING_STATE: arg1 = user u32[2] {free_descs, ready(0/1)}
 * Soft: ready=0 / free=0 when virtio-blk is absent (no hard fail).
 */
#define GJ_STORE_OP_RING_STATE  11u
/**
 * MAP_RING: arg1 = user VA base (page-aligned)
 *           arg2 = optional user ptr to gj_virtq_export (may be 0)
 * Soft: NODEV when virtio-blk is absent; FAULT on map failure.
 * Records last successful base in store_door_ring_map_va().
 */
#define GJ_STORE_OP_MAP_RING    12u

/**
 * Init ownership/soft counters. Safe to re-call. Backends may probe later.
 */
void store_door_init(void);

/**
 * Dispatch store opcode. Returns 0 / bytes / 1 or negative GJ_ERR_* / errno.
 */
i64  store_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3);

/** Non-zero when a userspace owner token is held. */
int  store_door_owned(void);

/** Current owner token, or 0 if kernel interim owns policy. */
u32  store_door_owner_token(void);

/** Last successful MAP_RING user VA base, or 0 if never mapped. */
u64  store_door_ring_map_va(void);

/** Soft path: EXPORT/MAP/KICK/RING_STATE call count. */
u32  store_door_ring_calls(void);

/**
 * Soft path: first-claim count (excludes idempotent reclaims).
 * greppable: store_door claim soft
 */
u32  store_door_claim_count(void);
