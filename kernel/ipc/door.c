/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Doors rendezvous for cold personality (ENDPOINT-shaped, G-DOOR / G-COLD).
 *
 * Protocol (single-flight client):
 *   server  door_recv  → block tag 1 until u32HasReq
 *   client  door_call  → claim slot (CAS), post req, wake server, block tag 2
 *   server  door_reply → set reply, wake client (tag 2)
 *   contenders for the client slot block on tag 3 (no product busy-spin)
 *
 * Wait keys are the door object; tags distinguish roles.
 * Peer death / object DEAD → clients see -LINUX_EIO (G-DOOR-4 / G-PERS-3).
 *
 * Mid-call timeout races (cooperative UP + atomics for SMP-prep):
 *   HasReply is observed before the deadline check in the client wait loop,
 *   so a reply that lands cannot be demoted to -ETIMEDOUT on the same arm.
 *   On timeout: clear HasReq then HasReply, then CAS-release pClient. A
 *   concurrent door_reply after release sees pClient==NULL and drops (stale).
 *   A concurrent door_recv that already sampled HasReq may still copy req and
 *   later reply into a freed slot — reply is then dropped; no hang. Server
 *   re-checks HasReq after wake if the client cancelled first.
 */
#include <gj/cap.h>
#include <gj/door.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>

/* Block tags on the door wait object (must match wake sites). */
#define DOOR_TAG_SERVER 1u /* server waiting for a request */
#define DOOR_TAG_CLIENT 2u /* client waiting for a reply */
#define DOOR_TAG_SLOT   3u /* contender waiting for single-flight slot */

static struct gj_door g_doorCold;
static int            g_fColdInited;

static void door_release_client_slot(struct gj_door *pDoor,
                                     struct gj_thread *pCur);

static int
door_live(const struct gj_door *pDoor)
{
    if (pDoor == NULL || !pDoor->u32Ready) {
        return 0;
    }
    if (__atomic_load_n(&pDoor->hdr.u32State, __ATOMIC_ACQUIRE) !=
        (u32)GJ_OBJ_LIVE) {
        return 0;
    }
    if (pDoor->u32PeerDead) {
        return 0;
    }
    return 1;
}

/* Snapshot server badge for client get_last_badge after a completed flight. */
static void
door_snapshot_last_badge(struct gj_door *pDoor)
{
    if (pDoor == NULL) {
        return;
    }
    pDoor->u32LastBadge = pDoor->u32Badge;
}

/*
 * Mid-call / peer-path cleanup: drop in-flight flags then release slot.
 * Order matters — see file header race notes. Caller supplies the abort
 * accounting (timeouts vs peer aborts).
 */
static void
door_cancel_inflight(struct gj_door *pDoor, struct gj_thread *pCur)
{
    if (pDoor == NULL) {
        return;
    }
    /*
     * HasReq must be 0 before pClient is cleared so a server woken on the
     * original post does not re-consume a cancelled request after re-check.
     * HasReply cleared so a late reply cannot revive a timed-out client
     * (client already leaving with -ETIMEDOUT / -EIO).
     */
    pDoor->u32HasReq = 0;
    pDoor->u32HasReply = 0;
    door_release_client_slot(pDoor, pCur);
    /* Nudge server so a blocked recv re-evaluates after cancel. */
    (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
}

void
door_init(struct gj_door *pDoor)
{
    if (pDoor == NULL) {
        return;
    }
    memset(pDoor, 0, sizeof(*pDoor));
    gj_obj_hdr_init(&pDoor->hdr);
    pDoor->u32Ready = 1;
}

void
door_cold_init(void)
{
    if (g_fColdInited && g_doorCold.u32Ready &&
        g_doorCold.hdr.u32State == (u32)GJ_OBJ_LIVE) {
        return;
    }
    door_init(&g_doorCold);
    g_fColdInited = 1;
    /*
     * Boot/smoke readability: ready flag + object state. Call/reply/timeout
     * counts live in door_stats (and cold_ipc / linux dispatch layers).
     */
    kprintf("door: cold personality ready=%u state=%u (ENDPOINT)\n",
            g_doorCold.u32Ready, g_doorCold.hdr.u32State);
}

struct gj_door *
door_cold_personality(void)
{
    return &g_doorCold;
}

int
door_is_live(const struct gj_door *pDoor)
{
    return door_live(pDoor);
}

void
door_stats(const struct gj_door *pDoor, u64 *pCalls, u64 *pReplies,
           u64 *pAborts, u64 *pTimeouts)
{
    if (pCalls != NULL) {
        *pCalls = (pDoor != NULL) ? pDoor->u64Calls : 0;
    }
    if (pReplies != NULL) {
        *pReplies = (pDoor != NULL) ? pDoor->u64Replies : 0;
    }
    if (pAborts != NULL) {
        *pAborts = (pDoor != NULL) ? pDoor->u64Aborts : 0;
    }
    if (pTimeouts != NULL) {
        *pTimeouts = (pDoor != NULL) ? pDoor->u64Timeouts : 0;
    }
}

gj_status_t
door_install_endpoint(struct gj_process *pProc, struct gj_door *pDoor,
                      u16 u16Rights, struct gj_cap_ref *pOutRef)
{
    if (pProc == NULL || pDoor == NULL || pOutRef == NULL ||
        pProc->pCnode == NULL) {
        return GJ_ERR_INVAL;
    }
    if (!door_live(pDoor)) {
        return GJ_ERR_NODEV;
    }
    if (u16Rights == 0) {
        u16Rights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_GRANT | GJ_RIGHT_IDENTIFY);
    }
    return gj_cap_alloc_install(pProc->pCnode, (u16)GJ_CAP_ENDPOINT, u16Rights,
                                &pDoor->hdr, pOutRef);
}

/*
 * Release single-flight client ownership and wake one contender for the slot.
 * Caller still holds no locks (cooperative UP + atomics for SMP-prep).
 */
static void
door_release_client_slot(struct gj_door *pDoor, struct gj_thread *pCur)
{
    struct gj_thread *pExpected = pCur;

    if (pDoor == NULL) {
        return;
    }
    (void)__atomic_compare_exchange_n(&pDoor->pClient, &pExpected, NULL, 0,
                                      __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
    (void)thread_wake(pDoor, DOOR_TAG_SLOT, 1);
}

void
door_abort_waiters(struct gj_door *pDoor)
{
    if (pDoor == NULL) {
        return;
    }
    pDoor->u32PeerDead = 1;
    pDoor->u64Aborts++;
    /*
     * Deliver a synthetic reply so a blocked client exits door_call with -EIO
     * rather than hanging. Server loops re-check door_live after wake.
     * HasReq left as-is: client path clears it on the -EIO return arm after
     * observing HasReply (or !door_live).
     */
    if (pDoor->pClient != NULL) {
        pDoor->i64Reply = -(i64)LINUX_EIO;
        pDoor->u32HasReply = 1;
        (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
    }
    if (pDoor->pServer != NULL) {
        (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
    }
    (void)thread_wake(pDoor, DOOR_TAG_SLOT, 8);
}

void
door_mark_dead(struct gj_door *pDoor)
{
    if (pDoor == NULL) {
        return;
    }
    pDoor->u32Ready = 0;
    pDoor->u32PeerDead = 1;
    if (pDoor->hdr.u32State == (u32)GJ_OBJ_LIVE) {
        (void)gj_obj_revoke_begin(&pDoor->hdr);
    } else {
        __atomic_store_n(&pDoor->hdr.u32State, (u32)GJ_OBJ_DEAD,
                         __ATOMIC_RELEASE);
    }
    door_abort_waiters(pDoor);
}

void
door_on_thread_exit(struct gj_thread *pThr)
{
    struct gj_door *pDoor = door_cold_personality();
    struct gj_thread *pExpected;

    if (pThr == NULL || pDoor == NULL) {
        return;
    }
    /* Drop client slot if this thr owns single-flight call. */
    pExpected = pThr;
    if (__atomic_compare_exchange_n(&pDoor->pClient, &pExpected, NULL, 0,
                                    __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        /*
         * Same HasReq/HasReply clear order as mid-call timeout: cancel before
         * slot is visible as free to contenders.
         */
        pDoor->u32HasReq = 0;
        pDoor->u32HasReply = 0;
        pDoor->i64Reply = -(i64)LINUX_EIO;
        pDoor->u64Aborts++;
        (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
        (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
        (void)thread_wake(pDoor, DOOR_TAG_SLOT, 8);
    }
    /* Drop server role so cold_ipc falls back to sync service. */
    if (pDoor->pServer == pThr) {
        pDoor->pServer = NULL;
        (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
        (void)thread_wake(pDoor, DOOR_TAG_SLOT, 8);
    }
}

void
door_set_badge(struct gj_door *pDoor, u32 u32Badge)
{
    if (pDoor == NULL) {
        return;
    }
    pDoor->u32Badge = u32Badge;
}

u32
door_get_badge(const struct gj_door *pDoor)
{
    return pDoor != NULL ? pDoor->u32Badge : 0u;
}

u32
door_get_last_badge(const struct gj_door *pDoor)
{
    return pDoor != NULL ? pDoor->u32LastBadge : 0u;
}

void
door_badge_or(struct gj_door *pDoor, u64 u64Bits)
{
    if (pDoor == NULL || u64Bits == 0) {
        return;
    }
    pDoor->u64BadgeMask |= u64Bits;
}

u64
door_get_badge_mask(const struct gj_door *pDoor)
{
    return pDoor != NULL ? pDoor->u64BadgeMask : 0ull;
}

i64
door_call(struct gj_door *pDoor, struct gj_linux_regs *pRegs)
{
    return door_call_timeout(pDoor, pRegs, 0);
}

i64
door_call_timeout(struct gj_door *pDoor, struct gj_linux_regs *pRegs,
                  u64 u64DeadlineMonoNsec)
{
    struct gj_thread *pCur;
    struct gj_thread *pExpected;
    i64               i64Ret;

    if (pDoor == NULL || pRegs == NULL || !pDoor->u32Ready) {
        return -LINUX_ENOSYS;
    }
    if (!door_live(pDoor)) {
        return -LINUX_EIO; /* peer/object not live (PEER_DEAD) */
    }
    pCur = thread_current();
    if (pCur == NULL) {
        return -LINUX_ENOSYS;
    }

    /* Single-flight: CAS-claim client slot; contenders block (G-COLD-3). */
    for (;;) {
        if (!door_live(pDoor)) {
            return -LINUX_EIO;
        }
        /* No mono clock yet, or deadline already past → timeout (no hang). */
        if (u64DeadlineMonoNsec != 0 &&
            (!timer_ready() ||
             timer_mono_nsec() >= u64DeadlineMonoNsec)) {
            pDoor->u64Timeouts++;
            return -LINUX_ETIMEDOUT;
        }
        pExpected = NULL;
        if (__atomic_compare_exchange_n(&pDoor->pClient, &pExpected, pCur, 0,
                                        __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
            break;
        }
        /* Stale owner: dead thr left slot held after #PF kill. */
        {
            struct gj_thread *pHold = pDoor->pClient;

            if (pHold != NULL &&
                (pHold->u32State == GJ_THR_EXITED || pHold->u32Id == 0)) {
                door_on_thread_exit(pHold);
                continue;
            }
        }
        thread_block(pDoor, DOOR_TAG_SLOT);
        if (pDoor->pClient == NULL || !door_live(pDoor)) {
            (void)thread_wake(pDoor, DOOR_TAG_SLOT, 1);
        }
        schedule();
    }

    pDoor->req = *pRegs;
    pDoor->u32HasReq = 1;
    pDoor->u32HasReply = 0;
    pDoor->u64Calls++;

    if (pDoor->pServer != NULL) {
        (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
    }

    for (;;) {
        /*
         * Reply arm first: if server completed before our timeout sample,
         * return the real reply (never demote a landed reply to ETIMEDOUT).
         */
        if (pDoor->u32HasReply) {
            i64Ret = pDoor->i64Reply;
            pDoor->u32HasReply = 0;
            pDoor->u32HasReq = 0;
            door_snapshot_last_badge(pDoor);
            door_release_client_slot(pDoor, pCur);
            return i64Ret;
        }
        if (!door_live(pDoor)) {
            /* Peer death mid-wait: drop flight; last badge still useful. */
            door_snapshot_last_badge(pDoor);
            door_cancel_inflight(pDoor, pCur);
            return -LINUX_EIO;
        }
        if (u64DeadlineMonoNsec != 0 &&
            (!timer_ready() ||
             timer_mono_nsec() >= u64DeadlineMonoNsec)) {
            /*
             * Mid-call timeout cleanup: HasReq/HasReply cleared inside
             * door_cancel_inflight before slot release (see file header).
             * Count under u64Timeouts only — not peer u64Aborts.
             */
            door_cancel_inflight(pDoor, pCur);
            pDoor->u64Timeouts++;
            return -LINUX_ETIMEDOUT;
        }
        thread_block(pDoor, DOOR_TAG_CLIENT);
        if (pDoor->u32HasReply || !door_live(pDoor)) {
            (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
        }
        schedule();
    }
}

int
door_recv(struct gj_door *pDoor, struct gj_linux_regs *pRegs)
{
    struct gj_thread *pCur;

    if (pDoor == NULL || pRegs == NULL || !pDoor->u32Ready) {
        return (int)GJ_ERR_INVAL;
    }
    if (!door_live(pDoor)) {
        return (int)GJ_ERR_PEER_DEAD;
    }
    pCur = thread_current();
    if (pCur == NULL) {
        return (int)GJ_ERR_INVAL;
    }

    for (;;) {
        if (!door_live(pDoor)) {
            pDoor->pServer = NULL;
            return (int)GJ_ERR_PEER_DEAD;
        }
        if (pDoor->u32HasReq) {
            *pRegs = pDoor->req;
            pDoor->u32HasReq = 0;
            pDoor->pServer = NULL;
            return 0;
        }
        pDoor->pServer = pCur;
        thread_block(pDoor, DOOR_TAG_SERVER);
        /* Request or death may have landed between check and BLOCKED. */
        if (pDoor->u32HasReq || !door_live(pDoor)) {
            (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
        }
        schedule();
        pDoor->pServer = NULL;
    }
}

void
door_reply(struct gj_door *pDoor, i64 i64Ret)
{
    if (pDoor == NULL || !pDoor->u32Ready) {
        return;
    }
    /* Stale reply: no in-flight client owns the slot — drop. */
    if (pDoor->pClient == NULL) {
        return;
    }
    pDoor->i64Reply = i64Ret;
    pDoor->u32HasReply = 1;
    pDoor->u64Replies++;
    (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
}
