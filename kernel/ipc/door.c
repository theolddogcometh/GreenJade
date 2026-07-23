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
 *
 * Soft door call inventory (Wave 9 exclusive deepen):
 *   - Call path: enter / claim / reply / eio / etimedout / enosys / slot_wait
 *   - Recv path: enter / ok / peer_dead / inval / block
 *   - Reply path: enter / ok / stale / not_ready
 *   - Abort / cancel / thr_exit soft notes + cold product snapshot
 *   greppable: "door: soft …"
 *   Never hard-gates; diagnostics only (wrap OK).
 *
 * Soft ephemeral single-use REPLY (Call path deepen — not full MIG product):
 *   On slot claim, kernel mints a soft REPLY right bound to the door flight.
 *   First door_reply consumes it; second use fails (stale / second_fail).
 *   Timeout / peer death / thr-exit / init invalidates the soft right.
 *   greppable: "door: reply single-use …" / "door: REPLY soft …"
 *   Honesty: not full MIG REPLY until CNode install of GJ_CAP_REPLY; no bar3.
 *
 * Soft badge / cap-transfer deepen (server-authoritative badge path):
 *   grant = door_set_badge, move = last-badge snapshot on completed flight,
 *   fail = null / reject arms. greppable: "door: badge transfer …"
 *   Complements boot smoke "door: badge transfer PASS" (install/mint path).
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
static u8             g_fReplySoftSelfcheck; /* cold-init self-check once */

/*
 * Soft product inventory (Wave 9). Cumulative path tallies across all doors
 * that enter this module. Live/product counters remain per-door (door_stats).
 * greppable: door: soft …
 */
static u64 g_u64SoftCallEnter;     /* door_call_timeout entries */
static u64 g_u64SoftCallClaim;     /* single-flight slot claims */
static u64 g_u64SoftCallReply;     /* terminal via HasReply arm */
static u64 g_u64SoftCallEio;       /* -EIO terminal arms */
static u64 g_u64SoftCallEtimedout; /* -ETIMEDOUT terminal arms */
static u64 g_u64SoftCallEnosys;    /* -ENOSYS terminal arms */
static u64 g_u64SoftCallSlotWait;  /* contender tag-3 block entries */
static u64 g_u64SoftCallClientWait;/* in-flight client tag-2 blocks */
static u64 g_u64SoftRecvEnter;     /* door_recv entries */
static u64 g_u64SoftRecvOk;        /* request delivered to server */
static u64 g_u64SoftRecvPeerDead;  /* PEER_DEAD terminal */
static u64 g_u64SoftRecvInval;     /* INVAL terminal */
static u64 g_u64SoftRecvBlock;     /* server tag-1 block entries */
static u64 g_u64SoftReplyEnter;    /* door_reply entries */
static u64 g_u64SoftReplyOk;       /* reply posted + client woken */
static u64 g_u64SoftReplyStale;    /* pClient==NULL drop */
static u64 g_u64SoftReplyNotReady; /* null / !ready drop */
static u64 g_u64SoftAbort;         /* door_abort_waiters */
static u64 g_u64SoftCancel;        /* door_cancel_inflight */
static u64 g_u64SoftThrExit;       /* door_on_thread_exit entries */
static u64 g_u64SoftThrExitClient; /* thr-exit cleared client slot */
static u64 g_u64SoftThrExitServer; /* thr-exit cleared server role */
static u64 g_u64SoftInstallOk;     /* door_install_endpoint success */
static u64 g_u64SoftInstallFail;   /* install reject (inval/nodev/cap) */
static u64 g_u64SoftLogN;          /* inventory log emissions */
static u8  g_fSoftOnce;            /* one-shot after first call activity */

/*
 * Soft ephemeral single-use REPLY rights (Call path).
 * File-static table — no CNode install, no GJ_CAP_REPLY product binding.
 * greppable: door: reply single-use … / door: REPLY soft …
 */
#define DOOR_REPLY_SOFT_SLOTS 8u

struct door_reply_soft {
    struct gj_door *pDoor;     /* door flight owner; NULL = free slot */
    u32             u32Gen;    /* non-zero while slot ever used */
    u32             u32Live;   /* 1 = usable single-use right */
    u32             u32Consumed; /* 1 after first successful consume */
};

static struct door_reply_soft g_aReplySoft[DOOR_REPLY_SOFT_SLOTS];
static u32 g_u32ReplySoftGen;      /* monotonic gen mint (wrap OK) */
static u64 g_u64ReplySuCreate;     /* soft REPLY created on claim */
static u64 g_u64ReplySuConsume;    /* first door_reply consume ok */
static u64 g_u64ReplySuSecondFail; /* second use rejected */
static u64 g_u64ReplySuInval;      /* timeout / peer / thr-exit / init */
static u64 g_u64ReplySuDrop;       /* create failed (table full / null) */

/*
 * Soft badge transfer path counters (server badge → client last-badge).
 * greppable: door: badge transfer …
 */
static u64 g_u64BadgeXferGrant; /* door_set_badge success */
static u64 g_u64BadgeXferMove;  /* last-badge snapshot on completed flight */
static u64 g_u64BadgeXferFail;  /* null / reject arms */

static void door_release_client_slot(struct gj_door *pDoor,
                                     struct gj_thread *pCur);
static int  door_live(const struct gj_door *pDoor);
static void door_soft_inc(u64 *pCtr);
static void door_soft_inventory_log(const struct gj_door *pDoor);
static void door_soft_maybe_once(void);
static void door_snapshot_last_badge(struct gj_door *pDoor);
static void door_reply_soft_create(struct gj_door *pDoor);
static int  door_reply_soft_try_consume(struct gj_door *pDoor);
static void door_reply_soft_invalidate(struct gj_door *pDoor);
static u32  door_reply_soft_live_count(void);
static void door_reply_soft_selfcheck(void);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
door_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Soft REPLY: find table slot for door (or NULL).
 * Linear scan — K is tiny (DOOR_REPLY_SOFT_SLOTS).
 */
static struct door_reply_soft *
door_reply_soft_find(struct gj_door *pDoor)
{
    u32 iSlot;

    if (pDoor == NULL) {
        return NULL;
    }
    for (iSlot = 0; iSlot < DOOR_REPLY_SOFT_SLOTS; iSlot++) {
        if (g_aReplySoft[iSlot].pDoor == pDoor) {
            return &g_aReplySoft[iSlot];
        }
    }
    return NULL;
}

/**
 * Soft REPLY create on Call claim (ephemeral single-use right).
 * Re-binds an existing slot for this door, else takes a free slot.
 * Table-full → drop counter only; product Call still proceeds (no hard-break).
 * greppable path: door: reply single-use create
 */
static void
door_reply_soft_create(struct gj_door *pDoor)
{
    struct door_reply_soft *pSlot;
    u32                     iSlot;

    if (pDoor == NULL) {
        door_soft_inc(&g_u64ReplySuDrop);
        return;
    }
    pSlot = door_reply_soft_find(pDoor);
    if (pSlot == NULL) {
        for (iSlot = 0; iSlot < DOOR_REPLY_SOFT_SLOTS; iSlot++) {
            if (g_aReplySoft[iSlot].pDoor == NULL) {
                pSlot = &g_aReplySoft[iSlot];
                break;
            }
        }
    }
    if (pSlot == NULL) {
        door_soft_inc(&g_u64ReplySuDrop);
        return;
    }
    g_u32ReplySoftGen++;
    if (g_u32ReplySoftGen == 0u) {
        g_u32ReplySoftGen = 1u; /* gen 0 reserved = never minted */
    }
    pSlot->pDoor = pDoor;
    pSlot->u32Gen = g_u32ReplySoftGen;
    pSlot->u32Live = 1u;
    pSlot->u32Consumed = 0u;
    door_soft_inc(&g_u64ReplySuCreate);
}

/**
 * Soft REPLY consume-once for door_reply.
 * Returns 1 if reply may proceed, 0 if single-use already spent / dead.
 * Missing table entry → allow (fallback; create drop must not hard-break).
 */
static int
door_reply_soft_try_consume(struct gj_door *pDoor)
{
    struct door_reply_soft *pSlot;

    pSlot = door_reply_soft_find(pDoor);
    if (pSlot == NULL) {
        return 1; /* no soft tracking — product path continues */
    }
    if (pSlot->u32Live == 0u || pSlot->u32Consumed != 0u) {
        door_soft_inc(&g_u64ReplySuSecondFail);
        return 0;
    }
    pSlot->u32Live = 0u;
    pSlot->u32Consumed = 1u;
    door_soft_inc(&g_u64ReplySuConsume);
    return 1;
}

/**
 * Soft REPLY invalidate (timeout / peer death / thr-exit / door_init).
 * Leaves slot bound so a late second door_reply still second-fails while
 * pClient might race; freed only when create rebinds or door is re-inited
 * after full release (pDoor cleared when fully idle).
 */
static void
door_reply_soft_invalidate(struct gj_door *pDoor)
{
    struct door_reply_soft *pSlot;

    pSlot = door_reply_soft_find(pDoor);
    if (pSlot == NULL) {
        return;
    }
    if (pSlot->u32Live != 0u || pSlot->u32Consumed != 0u ||
        pSlot->u32Gen != 0u) {
        door_soft_inc(&g_u64ReplySuInval);
    }
    pSlot->u32Live = 0u;
    pSlot->u32Consumed = 1u; /* treat as spent so second use fails */
}

/** Count soft REPLY rights currently live (diagnostics). */
static u32
door_reply_soft_live_count(void)
{
    u32 iSlot;
    u32 u32Live;

    u32Live = 0;
    for (iSlot = 0; iSlot < DOOR_REPLY_SOFT_SLOTS; iSlot++) {
        if (g_aReplySoft[iSlot].pDoor != NULL &&
            g_aReplySoft[iSlot].u32Live != 0u) {
            u32Live++;
        }
    }
    return u32Live;
}

/**
 * Cold-init soft self-check: create → consume once → second use fails.
 * Private scratch door only — never touches cold personality product state.
 * greppable: door: reply single-use … / door: REPLY soft …
 * Honesty: not CNode-installed MIG REPLY product; no bar3.
 */
static void
door_reply_soft_selfcheck(void)
{
    static struct gj_door g_doorSu;
    struct gj_thread     *pCur;
    struct door_reply_soft *pSlot;
    u32                   u32CreateOk;
    u32                   u32ConsumeOk;
    u32                   u32SecondFail;
    u32                   u32Gen;
    i64                   i64First;
    u64                   u64C0;
    u64                   u64S0;

    if (g_fReplySoftSelfcheck != 0) {
        return;
    }
    g_fReplySoftSelfcheck = 1;

    door_init(&g_doorSu);
    door_set_badge(&g_doorSu, 0x5e17u);

    /* Create soft REPLY as Call claim would. */
    door_reply_soft_create(&g_doorSu);
    pSlot = door_reply_soft_find(&g_doorSu);
    u32CreateOk = (pSlot != NULL && pSlot->u32Live != 0u) ? 1u : 0u;
    u32Gen = (pSlot != NULL) ? pSlot->u32Gen : 0u;

    /*
     * Simulate in-flight client so door_reply does not stale-drop on
     * pClient==NULL. Boot always has a current thread on the cold path.
     */
    pCur = thread_current();
    u32ConsumeOk = 0;
    u32SecondFail = 0;
    i64First = 0;
    u64C0 = g_u64ReplySuConsume;
    u64S0 = g_u64ReplySuSecondFail;

    if (pCur != NULL) {
        g_doorSu.pClient = pCur;
        g_doorSu.u32HasReply = 0;
        door_reply(&g_doorSu, 0x1111);
        i64First = g_doorSu.i64Reply;
        u32ConsumeOk = (g_u64ReplySuConsume == u64C0 + 1ull &&
                        g_doorSu.u32HasReply != 0u &&
                        i64First == 0x1111)
                           ? 1u
                           : 0u;
        /* Second use must fail: no overwrite of first reply value. */
        door_reply(&g_doorSu, 0x2222);
        u32SecondFail = (g_u64ReplySuSecondFail == u64S0 + 1ull &&
                         g_doorSu.i64Reply == 0x1111)
                            ? 1u
                            : 0u;
        g_doorSu.pClient = NULL;
        g_doorSu.u32HasReply = 0;
    } else {
        /* No thr context: exercise soft helpers only. */
        u32ConsumeOk = door_reply_soft_try_consume(&g_doorSu) ? 1u : 0u;
        u32SecondFail = door_reply_soft_try_consume(&g_doorSu) ? 0u : 1u;
    }

    /* Grep: door: reply single-use */
    kprintf("door: reply single-use create=%u consume=%u second_fail=%u "
            "create_n=%lu consume_n=%lu second_fail_n=%lu inval_n=%lu "
            "drop_n=%lu\n",
            u32CreateOk, u32ConsumeOk, u32SecondFail,
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned long)g_u64ReplySuInval,
            (unsigned long)g_u64ReplySuDrop);

    /* Grep: door: REPLY soft — honesty: not full MIG / no CNode install */
    kprintf("door: REPLY soft gen=%u live_slots=%u slots_max=%u "
            "honesty=no_cnode_mig_product no_bar3=1\n",
            u32Gen, door_reply_soft_live_count(),
            (unsigned)DOOR_REPLY_SOFT_SLOTS);

    if (u32CreateOk != 0u && u32ConsumeOk != 0u && u32SecondFail != 0u) {
        kprintf("door: reply single-use soft PASS\n");
    }

    /*
     * Soft badge transfer deepen on scratch: grant already from set_badge;
     * move = snapshot last-badge; fail = null set. Complements inventory line.
     * greppable: door: badge transfer
     */
    door_snapshot_last_badge(&g_doorSu);
    door_set_badge(NULL, 0); /* fail arm */
    kprintf("door: badge transfer grant=%lu move=%lu fail=%lu "
            "(soft path; install/mint PASS remains main.c)\n",
            (unsigned long)g_u64BadgeXferGrant,
            (unsigned long)g_u64BadgeXferMove,
            (unsigned long)g_u64BadgeXferFail);

    /* Release scratch; do not mark_dead (avoids abort noise on cold init). */
    door_reply_soft_invalidate(&g_doorSu);
    pSlot = door_reply_soft_find(&g_doorSu);
    if (pSlot != NULL) {
        pSlot->pDoor = NULL;
        pSlot->u32Gen = 0;
        pSlot->u32Live = 0;
        pSlot->u32Consumed = 0;
    }
    g_doorSu.u32Ready = 0;
}

/**
 * Greppable soft door call inventory (product / smoke).
 *   door: soft call enter=… claim=… reply=… eio=… etimedout=… enosys=…
 *        slot_wait=… client_wait=…
 *   door: soft recv enter=… ok=… peer_dead=… inval=… block=…
 *   door: soft reply enter=… ok=… stale=… not_ready=…
 *   door: soft abort=… cancel=… thr_exit=… thr_cli=… thr_srv=…
 *        install_ok=… install_fail=… log_n=…
 *   door: soft cold ready=… live=… peer_dead=… calls=… replies=…
 *        aborts=… timeouts=… badge=0x… last_badge=0x… mask=0x…
 *   door: reply single-use create=… consume=… second_fail=… …
 *   door: REPLY soft live_slots=… honesty=…
 *   door: badge transfer grant=… move=… fail=…
 * greppable: door: soft / door: reply single-use / door: REPLY soft /
 *            door: badge transfer
 */
static void
door_soft_inventory_log(const struct gj_door *pDoor)
{
    const struct gj_door *pSnap;
    u32                   u32Ready;
    u32                   u32Live;
    u32                   u32PeerDead;
    u32                   u32Badge;
    u32                   u32LastBadge;
    u64                   u64Calls;
    u64                   u64Replies;
    u64                   u64Aborts;
    u64                   u64Timeouts;
    u64                   u64Mask;

    door_soft_inc(&g_u64SoftLogN);

    /* Grep: door: soft call */
    kprintf("door: soft call enter=%lu claim=%lu reply=%lu eio=%lu "
            "etimedout=%lu enosys=%lu slot_wait=%lu client_wait=%lu\n",
            (unsigned long)g_u64SoftCallEnter,
            (unsigned long)g_u64SoftCallClaim,
            (unsigned long)g_u64SoftCallReply,
            (unsigned long)g_u64SoftCallEio,
            (unsigned long)g_u64SoftCallEtimedout,
            (unsigned long)g_u64SoftCallEnosys,
            (unsigned long)g_u64SoftCallSlotWait,
            (unsigned long)g_u64SoftCallClientWait);

    /* Grep: door: soft recv */
    kprintf("door: soft recv enter=%lu ok=%lu peer_dead=%lu inval=%lu "
            "block=%lu\n",
            (unsigned long)g_u64SoftRecvEnter,
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)g_u64SoftRecvPeerDead,
            (unsigned long)g_u64SoftRecvInval,
            (unsigned long)g_u64SoftRecvBlock);

    /* Grep: door: soft reply */
    kprintf("door: soft reply enter=%lu ok=%lu stale=%lu not_ready=%lu\n",
            (unsigned long)g_u64SoftReplyEnter,
            (unsigned long)g_u64SoftReplyOk,
            (unsigned long)g_u64SoftReplyStale,
            (unsigned long)g_u64SoftReplyNotReady);

    /* Grep: door: soft abort / cancel / thr_exit / install */
    kprintf("door: soft abort=%lu cancel=%lu thr_exit=%lu thr_cli=%lu "
            "thr_srv=%lu install_ok=%lu install_fail=%lu log_n=%lu\n",
            (unsigned long)g_u64SoftAbort, (unsigned long)g_u64SoftCancel,
            (unsigned long)g_u64SoftThrExit,
            (unsigned long)g_u64SoftThrExitClient,
            (unsigned long)g_u64SoftThrExitServer,
            (unsigned long)g_u64SoftInstallOk,
            (unsigned long)g_u64SoftInstallFail,
            (unsigned long)g_u64SoftLogN);

    /*
     * Snapshot optional door (caller) else cold personality when inited.
     * Pure diagnostics — does not create or re-init a door.
     */
    pSnap = pDoor;
    if (pSnap == NULL && g_fColdInited) {
        pSnap = &g_doorCold;
    }
    if (pSnap != NULL) {
        u32Ready = pSnap->u32Ready;
        u32Live = door_live(pSnap) ? 1u : 0u;
        u32PeerDead = pSnap->u32PeerDead;
        u32Badge = pSnap->u32Badge;
        u32LastBadge = pSnap->u32LastBadge;
        u64Calls = pSnap->u64Calls;
        u64Replies = pSnap->u64Replies;
        u64Aborts = pSnap->u64Aborts;
        u64Timeouts = pSnap->u64Timeouts;
        u64Mask = pSnap->u64BadgeMask;
    } else {
        u32Ready = 0;
        u32Live = 0;
        u32PeerDead = 0;
        u32Badge = 0;
        u32LastBadge = 0;
        u64Calls = 0;
        u64Replies = 0;
        u64Aborts = 0;
        u64Timeouts = 0;
        u64Mask = 0;
    }

    /* Grep: door: soft cold */
    kprintf("door: soft cold ready=%u live=%u peer_dead=%u calls=%lu "
            "replies=%lu aborts=%lu timeouts=%lu badge=0x%x last_badge=0x%x "
            "mask=0x%lx\n",
            u32Ready, u32Live, u32PeerDead, (unsigned long)u64Calls,
            (unsigned long)u64Replies, (unsigned long)u64Aborts,
            (unsigned long)u64Timeouts, u32Badge, u32LastBadge,
            (unsigned long)u64Mask);

    /* Grep: door: reply single-use (soft REPLY tallies) */
    kprintf("door: reply single-use create=%lu consume=%lu second_fail=%lu "
            "inval=%lu drop=%lu\n",
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned long)g_u64ReplySuInval,
            (unsigned long)g_u64ReplySuDrop);

    /* Grep: door: REPLY soft — honesty bounds */
    kprintf("door: REPLY soft live_slots=%u slots_max=%u gen_hi=%u "
            "honesty=no_cnode_mig_product no_bar3=1\n",
            door_reply_soft_live_count(), (unsigned)DOOR_REPLY_SOFT_SLOTS,
            g_u32ReplySoftGen);

    /* Grep: door: badge transfer (grant/move/fail counters) */
    kprintf("door: badge transfer grant=%lu move=%lu fail=%lu\n",
            (unsigned long)g_u64BadgeXferGrant,
            (unsigned long)g_u64BadgeXferMove,
            (unsigned long)g_u64BadgeXferFail);
}

/**
 * After first product call activity, print soft inventory once (mirrors
 * futex/sched soft-stats-once). Safe from call return paths only.
 */
static void
door_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u64SoftCallEnter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    door_soft_inventory_log(NULL);
}

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
        door_soft_inc(&g_u64BadgeXferFail);
        return;
    }
    pDoor->u32LastBadge = pDoor->u32Badge;
    /* Soft badge transfer: move authoritative badge → client last-badge. */
    door_soft_inc(&g_u64BadgeXferMove);
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
    door_soft_inc(&g_u64SoftCancel);
    /*
     * HasReq must be 0 before pClient is cleared so a server woken on the
     * original post does not re-consume a cancelled request after re-check.
     * HasReply cleared so a late reply cannot revive a timed-out client
     * (client already leaving with -ETIMEDOUT / -EIO).
     * Soft REPLY right dies with the flight (single-use end).
     */
    pDoor->u32HasReq = 0;
    pDoor->u32HasReply = 0;
    door_reply_soft_invalidate(pDoor);
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
    /* Drop any soft REPLY bound to a prior life of this object address. */
    door_reply_soft_invalidate(pDoor);
    {
        struct door_reply_soft *pSlot = door_reply_soft_find(pDoor);

        if (pSlot != NULL) {
            pSlot->pDoor = NULL;
            pSlot->u32Gen = 0;
            pSlot->u32Live = 0;
            pSlot->u32Consumed = 0;
        }
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
    /* Soft REPLY single-use self-check (private scratch door; honesty only). */
    door_reply_soft_selfcheck();
    /* Grep: door: soft (baseline inventory after cold init) */
    door_soft_inventory_log(&g_doorCold);
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
    /*
     * Emit soft inventory on stats read so bring-up smoke also greps
     * door: soft call/recv/reply lines (mirrors file_lock_count).
     * greppable: door: soft
     */
    door_soft_inventory_log(pDoor);
}

gj_status_t
door_install_endpoint(struct gj_process *pProc, struct gj_door *pDoor,
                      u16 u16Rights, struct gj_cap_ref *pOutRef)
{
    gj_status_t st;

    if (pProc == NULL || pDoor == NULL || pOutRef == NULL ||
        pProc->pCnode == NULL) {
        door_soft_inc(&g_u64SoftInstallFail);
        return GJ_ERR_INVAL;
    }
    if (!door_live(pDoor)) {
        door_soft_inc(&g_u64SoftInstallFail);
        return GJ_ERR_NODEV;
    }
    if (u16Rights == 0) {
        u16Rights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_GRANT | GJ_RIGHT_IDENTIFY);
    }
    st = gj_cap_alloc_install(pProc->pCnode, (u16)GJ_CAP_ENDPOINT, u16Rights,
                              &pDoor->hdr, pOutRef);
    if (st == GJ_OK) {
        door_soft_inc(&g_u64SoftInstallOk);
    } else {
        door_soft_inc(&g_u64SoftInstallFail);
    }
    return st;
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
    door_soft_inc(&g_u64SoftAbort);
    pDoor->u32PeerDead = 1;
    pDoor->u64Aborts++;
    /*
     * Deliver a synthetic reply so a blocked client exits door_call with -EIO
     * rather than hanging. Server loops re-check door_live after wake.
     * HasReq left as-is: client path clears it on the -EIO return arm after
     * observing HasReply (or !door_live).
     * Invalidate soft REPLY so a late door_reply cannot double-complete.
     */
    door_reply_soft_invalidate(pDoor);
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
    door_soft_inc(&g_u64SoftThrExit);
    /* Drop client slot if this thr owns single-flight call. */
    pExpected = pThr;
    if (__atomic_compare_exchange_n(&pDoor->pClient, &pExpected, NULL, 0,
                                    __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        /*
         * Same HasReq/HasReply clear order as mid-call timeout: cancel before
         * slot is visible as free to contenders. Soft REPLY dies with owner.
         */
        door_soft_inc(&g_u64SoftThrExitClient);
        pDoor->u32HasReq = 0;
        pDoor->u32HasReply = 0;
        pDoor->i64Reply = -(i64)LINUX_EIO;
        pDoor->u64Aborts++;
        door_reply_soft_invalidate(pDoor);
        (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
        (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
        (void)thread_wake(pDoor, DOOR_TAG_SLOT, 8);
    }
    /* Drop server role so cold_ipc falls back to sync service. */
    if (pDoor->pServer == pThr) {
        door_soft_inc(&g_u64SoftThrExitServer);
        pDoor->pServer = NULL;
        (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
        (void)thread_wake(pDoor, DOOR_TAG_SLOT, 8);
    }
}

void
door_set_badge(struct gj_door *pDoor, u32 u32Badge)
{
    if (pDoor == NULL) {
        door_soft_inc(&g_u64BadgeXferFail);
        return;
    }
    pDoor->u32Badge = u32Badge;
    /* Soft badge transfer: server grant of authoritative badge. */
    door_soft_inc(&g_u64BadgeXferGrant);
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
        door_soft_inc(&g_u64BadgeXferFail);
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

    door_soft_inc(&g_u64SoftCallEnter);

    if (pDoor == NULL || pRegs == NULL || !pDoor->u32Ready) {
        door_soft_inc(&g_u64SoftCallEnosys);
        door_soft_maybe_once();
        return -LINUX_ENOSYS;
    }
    if (!door_live(pDoor)) {
        door_soft_inc(&g_u64SoftCallEio);
        door_soft_maybe_once();
        return -LINUX_EIO; /* peer/object not live (PEER_DEAD) */
    }
    pCur = thread_current();
    if (pCur == NULL) {
        door_soft_inc(&g_u64SoftCallEnosys);
        door_soft_maybe_once();
        return -LINUX_ENOSYS;
    }

    /* Single-flight: CAS-claim client slot; contenders block (G-COLD-3). */
    for (;;) {
        if (!door_live(pDoor)) {
            door_soft_inc(&g_u64SoftCallEio);
            door_soft_maybe_once();
            return -LINUX_EIO;
        }
        /* No mono clock yet, or deadline already past → timeout (no hang). */
        if (u64DeadlineMonoNsec != 0 &&
            (!timer_ready() ||
             timer_mono_nsec() >= u64DeadlineMonoNsec)) {
            pDoor->u64Timeouts++;
            door_soft_inc(&g_u64SoftCallEtimedout);
            door_soft_maybe_once();
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
        door_soft_inc(&g_u64SoftCallSlotWait);
        thread_block(pDoor, DOOR_TAG_SLOT);
        if (pDoor->pClient == NULL || !door_live(pDoor)) {
            (void)thread_wake(pDoor, DOOR_TAG_SLOT, 1);
        }
        schedule();
    }

    door_soft_inc(&g_u64SoftCallClaim);
    pDoor->req = *pRegs;
    pDoor->u32HasReq = 1;
    pDoor->u32HasReply = 0;
    pDoor->u64Calls++;
    /*
     * Soft ephemeral single-use REPLY right for this flight (Call path).
     * First door_reply consumes; second fails. Not CNode MIG product.
     * greppable: door: reply single-use create
     */
    door_reply_soft_create(pDoor);

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
            door_soft_inc(&g_u64SoftCallReply);
            /* Synthetic peer-dead reply still greps under eio as well. */
            if (i64Ret == -(i64)LINUX_EIO) {
                door_soft_inc(&g_u64SoftCallEio);
            }
            door_soft_maybe_once();
            return i64Ret;
        }
        if (!door_live(pDoor)) {
            /* Peer death mid-wait: drop flight; last badge still useful. */
            door_snapshot_last_badge(pDoor);
            door_cancel_inflight(pDoor, pCur);
            door_soft_inc(&g_u64SoftCallEio);
            door_soft_maybe_once();
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
            door_soft_inc(&g_u64SoftCallEtimedout);
            door_soft_maybe_once();
            return -LINUX_ETIMEDOUT;
        }
        door_soft_inc(&g_u64SoftCallClientWait);
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

    door_soft_inc(&g_u64SoftRecvEnter);

    if (pDoor == NULL || pRegs == NULL || !pDoor->u32Ready) {
        door_soft_inc(&g_u64SoftRecvInval);
        return (int)GJ_ERR_INVAL;
    }
    if (!door_live(pDoor)) {
        door_soft_inc(&g_u64SoftRecvPeerDead);
        return (int)GJ_ERR_PEER_DEAD;
    }
    pCur = thread_current();
    if (pCur == NULL) {
        door_soft_inc(&g_u64SoftRecvInval);
        return (int)GJ_ERR_INVAL;
    }

    for (;;) {
        if (!door_live(pDoor)) {
            pDoor->pServer = NULL;
            door_soft_inc(&g_u64SoftRecvPeerDead);
            return (int)GJ_ERR_PEER_DEAD;
        }
        if (pDoor->u32HasReq) {
            *pRegs = pDoor->req;
            pDoor->u32HasReq = 0;
            pDoor->pServer = NULL;
            door_soft_inc(&g_u64SoftRecvOk);
            return 0;
        }
        pDoor->pServer = pCur;
        door_soft_inc(&g_u64SoftRecvBlock);
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
    door_soft_inc(&g_u64SoftReplyEnter);

    if (pDoor == NULL || !pDoor->u32Ready) {
        door_soft_inc(&g_u64SoftReplyNotReady);
        return;
    }
    /* Stale reply: no in-flight client owns the slot — drop. */
    if (pDoor->pClient == NULL) {
        door_soft_inc(&g_u64SoftReplyStale);
        return;
    }
    /*
     * Ephemeral single-use REPLY soft: consume once.
     * Second door_reply on the same flight fails closed (no overwrite).
     * greppable: door: reply single-use consume / second_fail
     */
    if (!door_reply_soft_try_consume(pDoor)) {
        door_soft_inc(&g_u64SoftReplyStale);
        return;
    }
    pDoor->i64Reply = i64Ret;
    pDoor->u32HasReply = 1;
    pDoor->u64Replies++;
    door_soft_inc(&g_u64SoftReplyOk);
    (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
}
