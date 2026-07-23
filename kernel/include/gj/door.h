/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Doors as ENDPOINT-shaped objects (G-DOOR / G-COLD-1).
 *
 * Synchronous Call/Recv/Reply rendezvous (Solaris doors spirit). Installable
 * in a CNode as GJ_CAP_ENDPOINT. Single-flight client slot; contenders block
 * (no product busy-spin). Object DEAD / gen failure ⇒ PEER_DEAD (-EIO).
 *
 * Protocol (single-flight client):
 *   server  door_recv  → block tag 1 until request posted
 *   client  door_call  → claim slot, post req, wake server, block tag 2
 *   server  door_reply → set reply, wake client
 *   abort   door_mark_dead / door_abort_waiters → PEER_DEAD (G-DOOR-4)
 */
#pragma once

#include <gj/cap.h>
#include <gj/linux_abi.h>
#include <gj/types.h>

struct gj_thread;
struct gj_process;

struct gj_door {
    struct gj_obj_hdr   hdr;       /* first: ENDPOINT object header */
    u32                 u32Ready;
    u32                 u32PeerDead; /* sticky; cleared on successful re-init */
    u32                 u32Badge;    /* receiver-side badge (server authority) */
    u32                 u32Pad0;
    struct gj_thread   *pServer;   /* blocked in door_recv */
    struct gj_thread   *pClient;   /* owns single-flight call slot */
    struct gj_linux_regs req;
    i64                 i64Reply;
    u32                 u32HasReq;
    u32                 u32HasReply;
    /* Product counters (wrap OK; diagnostics only). */
    u64                 u64Calls;
    u64                 u64Replies;
    u64                 u64Aborts;
};

void door_init(struct gj_door *pDoor);

/**
 * Client: claim single-flight slot, post req, wake server, block until reply.
 * Returns reply status (Linux-style negative errno on error).
 *   -LINUX_ENOSYS  null / not ready / no thread context
 *   -LINUX_EIO     object DEAD / peer dead (G-PERS-3 / G-DOOR-4)
 *   -LINUX_ETIMEDOUT only if aborted without sticky peer-dead (rare)
 */
i64 door_call(struct gj_door *pDoor, struct gj_linux_regs *pRegs);

/**
 * Server: block until request, copy into *pRegs.
 * Returns 0 on success, or negative gj_status_t (GJ_ERR_*).
 */
int door_recv(struct gj_door *pDoor, struct gj_linux_regs *pRegs);

/**
 * Server: complete in-flight call with i64Ret, wake client.
 * No-op if no client owns the slot (stale reply drop).
 */
void door_reply(struct gj_door *pDoor, i64 i64Ret);

/**
 * Wake all door waiters with PEER_DEAD semantics (does not revoke caps).
 * Safe to call from server teardown paths.
 */
void door_abort_waiters(struct gj_door *pDoor);

/**
 * Clear single-flight client/server roles held by an exiting thr.
 * Must run from thr kill / thread_exit so cold door_call cannot hang
 * forever on a dead pClient CAS claim (smoke thr INTERP #PF path).
 */
void door_on_thread_exit(struct gj_thread *pThr);

/**
 * Mark door DEAD (obj_hdr + ready clear) and abort waiters (G-DOOR-4).
 * Uses gj_obj_revoke_begin when still LIVE so CNode hygiene can follow.
 */
void door_mark_dead(struct gj_door *pDoor);

/** Non-zero if ready and LIVE. */
int door_is_live(const struct gj_door *pDoor);

/** Snapshot product counters into optional outs (NULL skips). */
void door_stats(const struct gj_door *pDoor, u64 *pCalls, u64 *pReplies,
                u64 *pAborts);

/** Global cold-path personality door (libprotonrt / bring-up server). */
struct gj_door *door_cold_personality(void);
void door_cold_init(void);

/**
 * Install door as GJ_CAP_ENDPOINT into process CNode (G-DOOR-1).
 * Returns handle in *pOutRef. Default rights if u16Rights == 0:
 * READ | GRANT | IDENTIFY.
 */
gj_status_t door_install_endpoint(struct gj_process *pProc, struct gj_door *pDoor,
                                  u16 u16Rights, struct gj_cap_ref *pOutRef);
