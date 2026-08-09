/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Doors as ENDPOINT-shaped objects (G-DOOR / G-COLD-1).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. Soft!=product. G-AC-1.
 *
 * Lean residual foundation for service hosts: channel-A control RPC
 * (Call/Recv/Reply + server-authoritative badge + ENDPOINT install).
 * Service hosts (vfsd / storaged / netstackd / sessiond / sshd / scsi_mid)
 * and DDI/UDX hosts (rtl8168_udx / xhci_udx / ddi_host_gj) use this
 * rendezvous for short control paths; bulk device data stays on mapped
 * rings / DDI MAP_BAR / UDX rings (SECURITY_CORE sec 3). This module never
 * maps client memory.
 * Not multi-server product; not MIG REPLY product; not Linux .ko product (G-AC-1).
 *
 * Heritage: Solaris doors spirit - synchronous Call/Recv/Reply rendezvous
 * for short control RPC. Bulk data stays on mapped rings (SECURITY_CORE sec 3);
 * this module is channel A only (no implicit map of client memory).
 *
 * Installable in a CNode as GJ_CAP_ENDPOINT (G-DOOR-1). Object header is
 * first field so DEAD/gen, pin/ref, and soft CDT edges share the cap path
 * with every other typed object (G-DOOR-2 complete for obj_hdr / DEAD).
 *
 * ---------------------------------------------------------------------------
 * Single-flight client model (product path - no busy-spin)
 * ---------------------------------------------------------------------------
 * One door admits at most one in-flight client call at a time. Contenders
 * block on a wait tag (not spin) until the slot frees. Server is single
 * waiter for the request (pServer); multiple servers on one door is not a
 * product API yet.
 *
 * Protocol (roles / wait tags on the door object as wait key):
 *   tag 1  DOOR_TAG_SERVER  - server blocked in door_recv for a request
 *   tag 2  DOOR_TAG_CLIENT  - client blocked in door_call for a reply
 *   tag 3  DOOR_TAG_SLOT    - contender blocked for the single-flight slot
 *
 * Happy path:
 *   server  door_recv  -> block tag 1 until u32HasReq
 *   client  door_call  -> claim pClient (CAS), post req, wake server, block tag 2
 *   server  door_reply -> set i64Reply / u32HasReply, wake client tag 2
 *   client             -> copy reply, clear flight, release slot, wake tag 3
 *
 * Abort / death (G-DOOR-4 / G-PERS-3):
 *   door_mark_dead / door_abort_waiters / door_on_thread_exit
 *   -> sticky u32PeerDead and/or object DEAD; waiters wake with PEER_DEAD
 *   -> client-visible reply path uses -LINUX_EIO (Linux personality cold ABI)
 *   -> does not itself walk CNode slots; mark_dead kicks sec 1.1 revoke begin
 *
 * Mid-call timeout (door_call_timeout):
 *   Absolute mono-nsec deadline only (SECURITY_CORE sec 5 - never wall clock).
 *   Client wait loop samples HasReply *before* deadline so a landed reply
 *   is never demoted to -ETIMEDOUT on the same arm. On expiry: clear
 *   HasReq then HasReply, release pClient, count u64Timeouts, return
 *   -LINUX_ETIMEDOUT. Late door_reply sees pClient==NULL and drops (stale).
 *
 * ---------------------------------------------------------------------------
 * Badge (client-visible, ABI-stable) - SECURITY_CORE sec 1 rule 3
 * ---------------------------------------------------------------------------
 * Badges are server-authoritative. Clients cannot forge them via this API;
 * door_set_badge is the server-side setter. On a completed flight (real
 * reply or synthetic peer-dead -EIO), the kernel snapshots u32Badge into
 * u32LastBadge so the client can door_get_last_badge() after door_call
 * returns without an out-arg on this ABI.
 *
 *   door_set_badge / door_get_badge     - current server badge
 *   door_get_last_badge                - client flight snapshot (call then get)
 *   door_badge_or / door_get_badge_mask - soft multi-badge OR mask (does not
 *                                        replace u32Badge; observability /
 *                                        soft multi-identity)
 *
 * Unchanged on pure timeout, slot-fail, never-called, or null door.
 *
 * ---------------------------------------------------------------------------
 * Cold personality + install
 * ---------------------------------------------------------------------------
 * door_cold_personality / door_cold_init - global cold-path door used by
 *   libprotonrt / bring-up server (LINUX_ABI_HYBRID cold path).
 * door_install_endpoint - install as GJ_CAP_ENDPOINT into a process CNode;
 *   default rights when u16Rights==0: READ | GRANT | IDENTIFY.
 *
 * Open (not this header's job): G-DOOR-3 full cap transfer on Call (small K,
 * receiver-allocated slots - SOLARIS_STYLE_REMAINING); single-use REPLY
 * caps as separate type remain scaffold elsewhere.
 *
 * Soft residual honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   - multi_server=0 - single server waiter; not a product multi-server API
 *   - soft REPLY rights are ephemeral table slots (cnode_mig_reply=0)
 *   - serve_hold + thr_srv_eio residual in door.c (call/reply correctness)
 *   - Soft inventory is sparse lamps only (no stamp storms / no version stamp)
 *   - Lean residual (exclusive residual unit): once-lamps + counters only
 *   - Service host foundation: channel-A control RPC for vfsd/storaged/
 *     netstackd/sessiond/sshd (+ DDI/UDX hosts); not device mint
 * Soft residual != multi-server product / MIG REPLY product / full cap transfer.
 * Host foundation residual != ddi_door product mint / Linux .ko product (G-AC-1).
 * Service host residual != product multi-server / MIG REPLY / .ko product.
 * Grep markers (door surface / smoke):
 *   G-DOOR-1 G-DOOR-2 G-DOOR-4 G-COLD-1 G-PERS-3 G-AC-1
 *   door: soft ...  door: soft call_reply  door: soft residual lean
 *   door: soft residual lean foundation  door: soft residual lean service
 *   door: soft residual lean PASS
 *   door: reply single-use  door: badge transfer  door: mid-call peer death
 *   host=svc|vfsd|storaged|netstackd|sessiond|sshd|ddi_udx
 * Implementation: kernel/ipc/door.c (do not treat this header as the race bible -
 * file-level comments in door.c own mid-call SMP-prep ordering detail).
 *
 * Companion docs:
 *   docs/DESIGN_SPEC_COMPLETE.md sec 3.2 / 4.1
 *   docs/SECURITY_CORE_DESIGN.md sec 1 (badges), 1.1 (revoke), 3 (IPC)
 *   docs/CAP_ADDRESSING.md (Scheme A handles for endpoint install)
 *   docs/LINUX_ABI_HYBRID.md (cold door_call path)
 *   docs/DDI_SOFT.md / docs/UDX_LINUX_PORTER.md (host foundation consumers)
 */
#pragma once

#include <gj/cap.h>
#include <gj/linux_abi.h>
#include <gj/types.h>

struct gj_thread;
struct gj_process;

/*
 * Wait-key tags on the door object (owned by door.c; documented here for
 * cross-module readers). Grep in door.c: DOOR_TAG_SERVER/CLIENT/SLOT
 *   1  server waiting for a request (door_recv)
 *   2  client waiting for a reply (door_call)
 *   3  contender waiting for single-flight client slot
 * Not re-#defined here - door.c owns the macros to avoid dual definition.
 */

/**
 * Kernel door / endpoint object.
 *
 * hdr must remain first: install/resolve/revoke treat the door as a typed
 * cap object through gj_obj_hdr (LIVE/REVOKING/DEAD + gen).
 *
 * Single-flight fields (pClient, HasReq/HasReply, req, i64Reply) form one
 * logical "flight." Contenders never mutate them until they own pClient.
 * Counters wrap OK and are diagnostics only - never used for security.
 */
struct gj_door {
    struct gj_obj_hdr   hdr;         /* first: ENDPOINT object header */
    u32                 u32Ready;    /* non-zero after door_init; cleared on mark_dead */
    u32                 u32PeerDead; /* sticky peer-death; cleared on successful re-init */
    u32                 u32Badge;    /* receiver-side badge (server authority) */
    u32                 u32LastBadge;/* client snapshot after completed call return */
    struct gj_thread   *pServer;     /* blocked in door_recv (tag 1), or NULL */
    struct gj_thread   *pClient;     /* owns single-flight call slot (CAS claim) */
    struct gj_linux_regs req;        /* posted client request regs (kernel copy) */
    i64                 i64Reply;    /* server reply status / value */
    u32                 u32HasReq;   /* non-zero while request posted for server */
    u32                 u32HasReply; /* non-zero while reply ready for client */
    /* Product counters (wrap OK; diagnostics only - door_stats). */
    u64                 u64Calls;    /* successful slot claims that entered call */
    u64                 u64Replies;  /* door_reply completions delivered / counted */
    u64                 u64Aborts;   /* peer death / mark_dead / thr-exit aborts */
    u64                 u64Timeouts; /* door_call_timeout -> -ETIMEDOUT arms */
    u64                 u64BadgeMask;/* soft multi-badge OR bits (door_badge_or) */
};

/**
 * Initialize a door object to LIVE + ready, zero flight/badge/counters.
 * Safe to re-init after teardown only when no waiters remain (caller duty).
 * Does not install into any CNode - use door_install_endpoint.
 */
void door_init(struct gj_door *pDoor);

/**
 * Client: claim single-flight slot, post req, wake server, block until reply.
 *
 * Returns reply status (Linux-style negative errno on error path used by
 * cold personality / libprotonrt). Positive/zero values are server-defined
 * success payloads on the cold ABI.
 *
 *   -LINUX_ENOSYS   null door / not ready / no current thread context
 *   -LINUX_EIO      object DEAD / peer dead (G-PERS-3 / G-DOOR-4)
 *   -LINUX_ETIMEDOUT mono deadline expired (door_call_timeout only)
 *
 * On a completed flight (real reply or synthetic -EIO), u32LastBadge is
 * updated from u32Badge so clients may door_get_last_badge() after return
 * without an out-arg on this ABI. Pure timeout / never-entered does not
 * refresh last badge.
 *
 * Contenders: if pClient is held, this thread blocks on DOOR_TAG_SLOT until
 * the owner releases (or peer death aborts). No product busy-spin (G-COLD-3).
 *
 * pRegs: kernel-side copy of the client request register block; must be
 * non-NULL on success path. Implementation copies into pDoor->req.
 */
i64 door_call(struct gj_door *pDoor, struct gj_linux_regs *pRegs);

/**
 * door_call with absolute mono deadline (nanoseconds since mono epoch).
 *
 * u64DeadlineMonoNsec == 0 -> no timeout (same as door_call).
 * Security timeouts use mono only - never wall clock (SECURITY_CORE sec 5).
 *
 * On mid-call expiry: clear HasReq then HasReply, drop client slot, count
 * u64Timeouts (not u64Aborts), return -LINUX_ETIMEDOUT. A reply that was
 * already visible before the deadline sample wins (no demotion to timeout).
 */
i64 door_call_timeout(struct gj_door *pDoor, struct gj_linux_regs *pRegs,
                      u64 u64DeadlineMonoNsec);

/**
 * Set server-authoritative badge observed by clients (G-DOOR badge path).
 * Only the receiver side should call this; clients read via get / last_badge.
 */
void door_set_badge(struct gj_door *pDoor, u32 u32Badge);

/** Current server badge; 0 if pDoor NULL. */
u32  door_get_badge(const struct gj_door *pDoor);

/**
 * Last badge snapshotted on a completed door_call flight (reply or
 * synthetic peer-dead path). Unchanged on pure timeout / slot-fail /
 * never-called / null. ABI-stable alternative to an optional out-arg on
 * door_call (keeps the cold Call signature small).
 */
u32  door_get_last_badge(const struct gj_door *pDoor);

/**
 * Soft multi-badge: OR bits into u64BadgeMask without changing u32Badge.
 * Useful for soft multi-identity / diagnostic aggregation; not a substitute
 * for the authoritative u32Badge on the Call snapshot path.
 * door_get_badge_mask returns the accumulated mask (0 if pDoor NULL).
 */
void door_badge_or(struct gj_door *pDoor, u64 u64Bits);
u64  door_get_badge_mask(const struct gj_door *pDoor);

/**
 * Server: block until a request is posted, copy into *pRegs.
 *
 * Returns 0 on success with *pRegs filled from the posted flight.
 * Returns negative gj_status_t on failure:
 *   GJ_ERR_INVAL      null args / not ready
 *   GJ_ERR_PEER_DEAD  object DEAD or sticky peer-dead while waiting
 *
 * Only one server waiter is tracked (pServer). Blocks on DOOR_TAG_SERVER.
 * After wake, implementation re-checks HasReq / live so cancelled clients
 * (timeout) do not deliver a stale request as a new call.
 *
 * Serve-hold residual (multi_server=0): on successful take, pServer stays set
 * until door_reply / thr-exit / abort (serve window). Soft!=product multi-server.
 * Soft residual only - not multi-server product dual-path.
 * Lean residual greps (door.c once-lamps; Soft!=product dual MIT OR Apache-2.0):
 *   door: soft residual lean / door: soft residual lean foundation
 *   door: soft residual lean service / door: soft residual lean PASS
 * Service host foundation: vfsd|storaged|netstackd|sessiond|sshd + DDI/UDX
 * (channel A control-RPC; G-AC-1; Soft!=product).
 */
int door_recv(struct gj_door *pDoor, struct gj_linux_regs *pRegs);

/**
 * Server: complete in-flight call with i64Ret, wake client (DOOR_TAG_CLIENT).
 * No-op if no client owns the slot (stale reply drop after timeout/abort);
 * stale path still releases serve if this thr owns pServer (call/reply residual).
 * Soft single-use REPLY: second reply on same flight fails closed (lean residual).
 * Does not clear peer-dead or revive a DEAD object.
 */
void door_reply(struct gj_door *pDoor, i64 i64Ret);

/**
 * Wake all door waiters with PEER_DEAD semantics (does not revoke caps).
 * Safe to call from server teardown paths. Sets sticky peer-dead, posts a
 * synthetic -EIO-style completion for an in-flight client when needed,
 * wakes tags 1/2/3, increments u64Aborts.
 *
 * Cap-table hygiene is separate: pair with door_mark_dead / gj_obj_revoke_begin
 * when the endpoint itself must fail-closed for resolve (S1/S2).
 */
void door_abort_waiters(struct gj_door *pDoor);

/**
 * Clear single-flight client/server roles held by an exiting thread.
 * Must run from thr kill / thread_exit so cold door_call cannot hang
 * forever on a dead pClient CAS claim (smoke thr INTERP #PF path) and so
 * a dead server does not leave clients blocked without PEER_DEAD.
 *
 * Thr-exit EIO residual: if pServer dies mid-serve with a client in flight
 * and no reply posted, posts synthetic -LINUX_EIO and wakes the client
 * (no hang). Soft residual; sticky peer-dead still via abort/mark_dead.
 */
void door_on_thread_exit(struct gj_thread *pThr);

/**
 * Mark door DEAD (obj_hdr via revoke begin when still LIVE + ready clear)
 * and abort waiters (G-DOOR-4).
 *
 * Security: after DEAD/gen bump, resolve/use fails closed (S1/S2) even if
 * CNode slots are not yet zeroed (Phase A' deferred hygiene). This call
 * does not walk derived slots itself - revoke deferred path owns S4/S7.
 */
void door_mark_dead(struct gj_door *pDoor);

/**
 * Non-zero if ready, LIVE, and not sticky peer-dead.
 * Diagnostic / pre-check only; racing death can still fail a subsequent op.
 */
int door_is_live(const struct gj_door *pDoor);

/**
 * Snapshot product counters into optional outs (NULL skips that field).
 * pCalls     - flights that claimed the client slot
 * pReplies   - reply completions counted on the product path
 * pAborts    - peer death / mark_dead / thr-exit aborts
 * pTimeouts  - mid-call or pre-claim -ETIMEDOUT count (clear abort path;
 *              not mixed into pAborts)
 */
void door_stats(const struct gj_door *pDoor, u64 *pCalls, u64 *pReplies,
                u64 *pAborts, u64 *pTimeouts);

/**
 * Global cold-path personality door (libprotonrt / bring-up server).
 * door_cold_init is idempotent; door_cold_personality returns NULL until
 * init has run. Used by kernel/syscall cold path (G-COLD-1).
 */
struct gj_door *door_cold_personality(void);
void door_cold_init(void);

/**
 * Install door as GJ_CAP_ENDPOINT into process CNode (G-DOOR-1).
 *
 * Returns handle in *pOutRef (Scheme A: slot + slot_gen). Soft-charges
 * process slot quota when the CNode has pQuotaAccount attached; bumps
 * object u32SlotsLeft. CDT edge link is mint-path responsibility when
 * edges are allocated (install alone is root binding for this object).
 *
 * Default rights if u16Rights == 0: READ | GRANT | IDENTIFY.
 * Requires a valid process with an initialized CNode.
 */
gj_status_t door_install_endpoint(struct gj_process *pProc, struct gj_door *pDoor,
                                  u16 u16Rights, struct gj_cap_ref *pOutRef);
