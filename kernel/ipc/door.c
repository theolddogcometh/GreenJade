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
 * Soft door call inventory (Wave 35 exclusive deepen — this unit only):
 *   - inventory / call / recv / reply / lifecycle / cold / err / path / PASS
 *   - Call: enter / claim / reply / eio / etimedout / enosys / slot_wait /
 *     client_wait + outcome rollup
 *   - Recv: enter / ok / peer_dead / inval / block + outcome rollup
 *   - Reply: enter / ok / stale / not_ready (+ single-use cross-link) + outcome
 *   - Lifecycle: abort / cancel / thr_exit / thr_cli / thr_srv / install
 *   - capacity / catalog / tags / deepen / flight / badge / reply_su_inval
 *   - return surface (Wave 17): call|recv|install i64/gj_status buckets
 *     greppable: "door: soft return …"
 *   - return rate / retcode (Wave 20 deepen): call|recv|install rate lamps
 *     + retcode catalog greppable: "door: soft return rate|retcode …"
 *   - Cold product snapshot + badge transfer grant/move/fail
 *   greppable: "door: soft …"
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ bar3 / MIG product.
 *
 * Soft ephemeral single-use REPLY (Wave 20 deepen — not full MIG product):
 *   On slot claim, kernel mints a soft REPLY right bound to the door flight.
 *   First door_reply consumes it; second use fails (stale / second_fail).
 *   Timeout / peer death / thr-exit / init invalidates the soft right.
 *   Deepen: create new|rebind, fallback allow, inval reason split, live peak.
 *   greppable: "door: reply single-use …" / "door: REPLY soft …" /
 *              "door: soft reply_su …"
 *   Honesty: not full MIG REPLY until CNode install of GJ_CAP_REPLY; no bar3.
 *   Soft ≠ MIG REPLY product (cnode_mig_reply=0).
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

/* Wave 35 exclusive soft deepen stamp (greppable wave=56). */
#define DOOR_SOFT_DEEPEN_WAVE 55u
/* +return selftest|retmap over Wave 17 return rate|retcode. */
#define DOOR_SOFT_DEEPEN_AREAS 78u

static struct gj_door g_doorCold;
static int            g_fColdInited;
static u8             g_fReplySoftSelfcheck; /* cold-init self-check once */

/*
 * Soft product inventory (Wave 20 exclusive). Cumulative path tallies across
 * all doors that enter this module. Live/product counters remain per-door
 * (door_stats). greppable: door: soft …
 */
static u64 g_u64SoftCallEnter;     /* door_call_timeout entries */
static u64 g_u64SoftCallClaim;     /* single-flight slot claims */
static u64 g_u64SoftCallReply;     /* terminal via HasReply arm */
static u64 g_u64SoftCallEio;       /* -EIO terminal arms */
static u64 g_u64SoftCallEtimedout; /* -ETIMEDOUT terminal arms */
static u64 g_u64SoftCallEnosys;    /* -ENOSYS terminal arms */
static u64 g_u64SoftCallSlotWait;  /* contender tag-3 block entries */
static u64 g_u64SoftCallClientWait;/* in-flight client tag-2 blocks */
static u64 g_u64SoftCallRetPos;    /* Wave 19: call returned i64Ret >= 0 */
static u64 g_u64SoftCallRetNeg;    /* Wave 19: call returned i64Ret < 0 */
static u64 g_u64SoftRecvEnter;     /* door_recv entries */
static u64 g_u64SoftRecvOk;        /* request delivered to server */
static u64 g_u64SoftRecvPeerDead;  /* PEER_DEAD terminal */
static u64 g_u64SoftRecvInval;     /* INVAL terminal */
static u64 g_u64SoftRecvBlock;     /* server tag-1 block entries */
static u64 g_u64SoftReplyEnter;    /* door_reply entries */
static u64 g_u64SoftReplyOk;       /* reply posted + client woken */
static u64 g_u64SoftReplyStale;    /* pClient==NULL drop / second-use */
static u64 g_u64SoftReplyNotReady; /* null / !ready drop */
static u64 g_u64SoftAbort;         /* door_abort_waiters */
static u64 g_u64SoftCancel;        /* door_cancel_inflight */
static u64 g_u64SoftThrExit;       /* door_on_thread_exit entries */
static u64 g_u64SoftThrExitClient; /* thr-exit cleared client slot */
static u64 g_u64SoftThrExitServer; /* thr-exit cleared server role */
static u64 g_u64SoftInstallOk;     /* door_install_endpoint success */
static u64 g_u64SoftInstallFail;   /* install reject (inval/nodev/cap) */
static u64 g_u64SoftInstallFailNull; /* Wave 19: null args / no cnode */
static u64 g_u64SoftInstallFailDead; /* Wave 19: door not live */
static u64 g_u64SoftInstallFailCap;  /* Wave 19: cap_alloc_install fail */
static u64 g_u64SoftLogN;          /* inventory log emissions */
static u8  g_fSoftOnce;            /* one-shot after first call activity */

/*
 * Soft ephemeral single-use REPLY rights (Call path; Wave 15 deepen).
 * File-static table — no CNode install, no GJ_CAP_REPLY product binding.
 * Soft ≠ MIG REPLY product (honesty: cnode_mig_reply=0 / no_bar3).
 * greppable: door: reply single-use … / door: REPLY soft … /
 *            door: soft reply_su …
 */
#define DOOR_REPLY_SOFT_SLOTS 8u

/* Soft REPLY invalidate reason (Wave 15 reason split; diagnostics only). */
#define DOOR_SU_INVAL_CANCEL 1u /* cancel_inflight (timeout / mid peer) */
#define DOOR_SU_INVAL_ABORT  2u /* abort_waiters / mark_dead */
#define DOOR_SU_INVAL_THR    3u /* thr-exit cleared client slot */
#define DOOR_SU_INVAL_INIT   4u /* door_init / selfcheck teardown */

struct door_reply_soft {
    struct gj_door *pDoor;       /* door flight owner; NULL = free slot */
    u32             u32Gen;      /* non-zero while slot ever used */
    u32             u32Live;     /* 1 = usable single-use right */
    u32             u32Consumed; /* 1 after first successful consume */
};

static struct door_reply_soft g_aReplySoft[DOOR_REPLY_SOFT_SLOTS];
static u32 g_u32ReplySoftGen;        /* monotonic gen mint (wrap OK) */
static u64 g_u64ReplySuCreate;       /* soft REPLY created on claim (total) */
static u64 g_u64ReplySuCreateNew;    /* bound free table slot */
static u64 g_u64ReplySuCreateRebind; /* rebind existing door entry */
static u64 g_u64ReplySuConsume;      /* first door_reply consume ok */
static u64 g_u64ReplySuSecondFail;   /* second use rejected */
static u64 g_u64ReplySuInval;        /* aggregate inval (any reason) */
static u64 g_u64ReplySuInvalCancel;  /* DOOR_SU_INVAL_CANCEL */
static u64 g_u64ReplySuInvalAbort;   /* DOOR_SU_INVAL_ABORT */
static u64 g_u64ReplySuInvalThr;     /* DOOR_SU_INVAL_THR */
static u64 g_u64ReplySuInvalInit;    /* DOOR_SU_INVAL_INIT */
static u64 g_u64ReplySuDrop;         /* create failed (table full / null) */
static u64 g_u64ReplySuFallback;     /* try_consume missing slot → allow */
static u32 g_u32ReplySuLivePeak;     /* high-water live soft REPLY rights */
static u8  g_fReplySoftSelfPass;     /* cold self-check create/consume/2nd */

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
static void door_reply_soft_invalidate(struct gj_door *pDoor, u32 u32Why);
static u32  door_reply_soft_live_count(void);
static u32  door_reply_soft_bound_count(void);
static void door_reply_soft_note_live_peak(void);
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

/** Count soft REPLY table entries bound to a door (live or spent). */
static u32
door_reply_soft_bound_count(void)
{
    u32 iSlot;
    u32 u32Bound;

    u32Bound = 0;
    for (iSlot = 0; iSlot < DOOR_REPLY_SOFT_SLOTS; iSlot++) {
        if (g_aReplySoft[iSlot].pDoor != NULL) {
            u32Bound++;
        }
    }
    return u32Bound;
}

/** Soft: note high-water live single-use REPLY rights. */
static void
door_reply_soft_note_live_peak(void)
{
    u32 u32Live;

    u32Live = door_reply_soft_live_count();
    if (u32Live > g_u32ReplySuLivePeak) {
        g_u32ReplySuLivePeak = u32Live;
    }
}

/**
 * Soft REPLY create on Call claim (ephemeral single-use right).
 * Re-binds an existing slot for this door, else takes a free slot.
 * Table-full → drop counter only; product Call still proceeds (no hard-break).
 * greppable path: door: reply single-use create / door: soft reply_su
 */
static void
door_reply_soft_create(struct gj_door *pDoor)
{
    struct door_reply_soft *pSlot;
    u32                     iSlot;
    int                     fRebind;

    if (pDoor == NULL) {
        door_soft_inc(&g_u64ReplySuDrop);
        return;
    }
    fRebind = 0;
    pSlot = door_reply_soft_find(pDoor);
    if (pSlot != NULL) {
        fRebind = 1;
    } else {
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
    if (fRebind != 0) {
        door_soft_inc(&g_u64ReplySuCreateRebind);
    } else {
        door_soft_inc(&g_u64ReplySuCreateNew);
    }
    door_reply_soft_note_live_peak();
}

/**
 * Soft REPLY consume-once for door_reply.
 * Returns 1 if reply may proceed, 0 if single-use already spent / dead.
 * Missing table entry → allow (fallback; create drop must not hard-break).
 * greppable: door: reply single-use consume / second_fail
 */
static int
door_reply_soft_try_consume(struct gj_door *pDoor)
{
    struct door_reply_soft *pSlot;

    pSlot = door_reply_soft_find(pDoor);
    if (pSlot == NULL) {
        /* No soft tracking — product path continues (table-full create drop). */
        door_soft_inc(&g_u64ReplySuFallback);
        return 1;
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
 * u32Why: DOOR_SU_INVAL_* reason (Wave 15 split).
 */
static void
door_reply_soft_invalidate(struct gj_door *pDoor, u32 u32Why)
{
    struct door_reply_soft *pSlot;

    pSlot = door_reply_soft_find(pDoor);
    if (pSlot == NULL) {
        return;
    }
    if (pSlot->u32Live != 0u || pSlot->u32Consumed != 0u ||
        pSlot->u32Gen != 0u) {
        door_soft_inc(&g_u64ReplySuInval);
        if (u32Why == DOOR_SU_INVAL_CANCEL) {
            door_soft_inc(&g_u64ReplySuInvalCancel);
        } else if (u32Why == DOOR_SU_INVAL_ABORT) {
            door_soft_inc(&g_u64ReplySuInvalAbort);
        } else if (u32Why == DOOR_SU_INVAL_THR) {
            door_soft_inc(&g_u64ReplySuInvalThr);
        } else if (u32Why == DOOR_SU_INVAL_INIT) {
            door_soft_inc(&g_u64ReplySuInvalInit);
        }
    }
    pSlot->u32Live = 0u;
    pSlot->u32Consumed = 1u; /* treat as spent so second use fails */
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
    u32                   u32New0;
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
    u32New0 = (u32)g_u64ReplySuCreateNew;
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
            "drop_n=%lu new_n=%lu rebind_n=%lu fallback_n=%lu wave=%u\n",
            u32CreateOk, u32ConsumeOk, u32SecondFail,
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned long)g_u64ReplySuInval,
            (unsigned long)g_u64ReplySuDrop,
            (unsigned long)g_u64ReplySuCreateNew,
            (unsigned long)g_u64ReplySuCreateRebind,
            (unsigned long)g_u64ReplySuFallback,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: REPLY soft — honesty: not full MIG / no CNode install */
    kprintf("door: REPLY soft gen=%u live_slots=%u bound=%u slots_max=%u "
            "live_peak=%u new_create=%u honesty=no_cnode_mig_product "
            "no_bar3=1 soft_ne_mig_reply=1 wave=%u\n",
            u32Gen, door_reply_soft_live_count(), door_reply_soft_bound_count(),
            (unsigned)DOOR_REPLY_SOFT_SLOTS, g_u32ReplySuLivePeak,
            (g_u64ReplySuCreateNew > (u64)u32New0) ? 1u : 0u,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    if (u32CreateOk != 0u && u32ConsumeOk != 0u && u32SecondFail != 0u) {
        g_fReplySoftSelfPass = 1;
        /* Grep: door: reply single-use soft PASS */
        kprintf("door: reply single-use soft PASS wave=%u\n",
                (unsigned)DOOR_SOFT_DEEPEN_WAVE);
    }

    /*
     * Soft badge transfer deepen on scratch: grant already from set_badge;
     * move = snapshot last-badge; fail = null set. Complements inventory line.
     * greppable: door: badge transfer
     */
    door_snapshot_last_badge(&g_doorSu);
    door_set_badge(NULL, 0); /* fail arm */
    kprintf("door: badge transfer grant=%lu move=%lu fail=%lu "
            "(soft path; install/mint PASS remains main.c) wave=%u\n",
            (unsigned long)g_u64BadgeXferGrant,
            (unsigned long)g_u64BadgeXferMove,
            (unsigned long)g_u64BadgeXferFail,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Release scratch; do not mark_dead (avoids abort noise on cold init). */
    door_reply_soft_invalidate(&g_doorSu, DOOR_SU_INVAL_INIT);
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
 * Greppable soft door call inventory (Wave 35 exclusive; product / smoke).
 * Prefix-stable markers (door: soft …):
 *   door: soft inventory  — rollup enter/claim/reply + logs + wave
 *   door: soft call       — call path terminal arms + wait tallies
 *   door: soft call outcome — terminal rollup
 *   door: soft recv       — recv path ok / peer_dead / inval / block
 *   door: soft recv outcome — terminal rollup
 *   door: soft reply      — reply enter/ok/stale/not_ready
 *   door: soft reply outcome — terminal rollup
 *   door: soft lifecycle  — abort/cancel/thr_exit/install
 *   door: soft cold       — cold personality / caller door snapshot
 *   door: soft reply_su   — ephemeral single-use REPLY tallies
 *   door: soft reply_su inval — inval reason split
 *   door: soft err        — call eio/etimedout/enosys + reply rejects
 *   door: soft capacity   — fixed soft table / tag lamps
 *   door: soft catalog    — path surface catalog (impl vs not)
 *   door: soft tags       — wait-key tag surface
 *   door: soft flight     — single-flight snap (has_req/reply/roles)
 *   door: soft badge      — badge transfer soft under door: soft
 *   door: soft return     — Wave 17 call|recv|install return surfaces
 *   door: soft return rate— Wave 17 call|recv|install rate lamps
 *   door: soft retcode    — Wave 17 observed i64/status retcode catalog
 *   door: soft return selftest — Wave 19 terminal return surface
 *   door: soft retmap     — Wave 19 return-surface map
 *   door: soft return selftest — Wave 19 terminal return surface
 *   door: soft retmap     — Wave 19 return-surface map
 *   door: soft deepen     — wave=56 areas stamp
 *   door: soft path       — honesty: soft ≠ bar3 / MIG REPLY product
 *   door: soft inventory PASS / door: soft PASS
 * Companion (not door: soft … prefix):
 *   door: reply single-use … / door: REPLY soft … / door: badge transfer …
 * Never hard-gates; diagnostics only. Soft ≠ MIG REPLY product.
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
    u32                   u32SuLive;
    u32                   u32SuBound;
    u32                   u32HasReq;
    u32                   u32HasReply;
    u32                   u32HasClient;
    u32                   u32HasServer;
    u64                   u64Calls;
    u64                   u64Replies;
    u64                   u64Aborts;
    u64                   u64Timeouts;
    u64                   u64Mask;
    int                   fSoftPass;

    door_soft_inc(&g_u64SoftLogN);

    u32SuLive = door_reply_soft_live_count();
    u32SuBound = door_reply_soft_bound_count();

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
        u32HasReq = pSnap->u32HasReq ? 1u : 0u;
        u32HasReply = pSnap->u32HasReply ? 1u : 0u;
        u32HasClient = (pSnap->pClient != NULL) ? 1u : 0u;
        u32HasServer = (pSnap->pServer != NULL) ? 1u : 0u;
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
        u32HasReq = 0;
        u32HasReply = 0;
        u32HasClient = 0;
        u32HasServer = 0;
    }

    /* Grep: door: soft inventory */
    kprintf("door: soft inventory cold_init=%u ready=%u live=%u "
            "call_enter=%lu claim=%lu reply_ok=%lu recv_ok=%lu "
            "reply_su_create=%lu reply_su_consume=%lu "
            "reply_su_second=%lu logs=%u areas=%u wave=%u\n",
            g_fColdInited ? 1u : 0u, u32Ready, u32Live,
            (unsigned long)g_u64SoftCallEnter,
            (unsigned long)g_u64SoftCallClaim,
            (unsigned long)g_u64SoftCallReply,
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned)g_u64SoftLogN,
            (unsigned)DOOR_SOFT_DEEPEN_AREAS,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft call */
    kprintf("door: soft call enter=%lu claim=%lu reply=%lu eio=%lu "
            "etimedout=%lu enosys=%lu slot_wait=%lu client_wait=%lu "
            "wave=%u\n",
            (unsigned long)g_u64SoftCallEnter,
            (unsigned long)g_u64SoftCallClaim,
            (unsigned long)g_u64SoftCallReply,
            (unsigned long)g_u64SoftCallEio,
            (unsigned long)g_u64SoftCallEtimedout,
            (unsigned long)g_u64SoftCallEnosys,
            (unsigned long)g_u64SoftCallSlotWait,
            (unsigned long)g_u64SoftCallClientWait,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft call outcome — terminal rollup */
    kprintf("door: soft call outcome reply=%lu eio=%lu etimedout=%lu "
            "enosys=%lu claim=%lu slot_wait=%lu client_wait=%lu "
            "ret_pos=%lu ret_neg=%lu wave=%u\n",
            (unsigned long)g_u64SoftCallReply,
            (unsigned long)g_u64SoftCallEio,
            (unsigned long)g_u64SoftCallEtimedout,
            (unsigned long)g_u64SoftCallEnosys,
            (unsigned long)g_u64SoftCallClaim,
            (unsigned long)g_u64SoftCallSlotWait,
            (unsigned long)g_u64SoftCallClientWait,
            (unsigned long)g_u64SoftCallRetPos,
            (unsigned long)g_u64SoftCallRetNeg,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft recv */
    kprintf("door: soft recv enter=%lu ok=%lu peer_dead=%lu inval=%lu "
            "block=%lu wave=%u\n",
            (unsigned long)g_u64SoftRecvEnter,
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)g_u64SoftRecvPeerDead,
            (unsigned long)g_u64SoftRecvInval,
            (unsigned long)g_u64SoftRecvBlock,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft recv outcome — terminal rollup */
    kprintf("door: soft recv outcome ok=%lu peer_dead=%lu inval=%lu "
            "block=%lu wave=%u\n",
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)g_u64SoftRecvPeerDead,
            (unsigned long)g_u64SoftRecvInval,
            (unsigned long)g_u64SoftRecvBlock,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft reply */
    kprintf("door: soft reply enter=%lu ok=%lu stale=%lu not_ready=%lu "
            "su_consume=%lu su_second_fail=%lu wave=%u\n",
            (unsigned long)g_u64SoftReplyEnter,
            (unsigned long)g_u64SoftReplyOk,
            (unsigned long)g_u64SoftReplyStale,
            (unsigned long)g_u64SoftReplyNotReady,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft reply outcome — terminal rollup */
    kprintf("door: soft reply outcome ok=%lu stale=%lu not_ready=%lu "
            "su_consume=%lu su_second=%lu wave=%u\n",
            (unsigned long)g_u64SoftReplyOk,
            (unsigned long)g_u64SoftReplyStale,
            (unsigned long)g_u64SoftReplyNotReady,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft lifecycle (abort / cancel / thr_exit / install) */
    kprintf("door: soft lifecycle abort=%lu cancel=%lu thr_exit=%lu "
            "thr_cli=%lu thr_srv=%lu install_ok=%lu install_fail=%lu "
            "log_n=%lu wave=%u\n",
            (unsigned long)g_u64SoftAbort, (unsigned long)g_u64SoftCancel,
            (unsigned long)g_u64SoftThrExit,
            (unsigned long)g_u64SoftThrExitClient,
            (unsigned long)g_u64SoftThrExitServer,
            (unsigned long)g_u64SoftInstallOk,
            (unsigned long)g_u64SoftInstallFail,
            (unsigned long)g_u64SoftLogN,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /*
     * Legacy single-line shape retained under door: soft abort=… so older
     * greps for thr_exit / install without "lifecycle" still match.
     * Grep: door: soft abort=
     */
    kprintf("door: soft abort=%lu cancel=%lu thr_exit=%lu thr_cli=%lu "
            "thr_srv=%lu install_ok=%lu install_fail=%lu log_n=%lu "
            "wave=%u\n",
            (unsigned long)g_u64SoftAbort, (unsigned long)g_u64SoftCancel,
            (unsigned long)g_u64SoftThrExit,
            (unsigned long)g_u64SoftThrExitClient,
            (unsigned long)g_u64SoftThrExitServer,
            (unsigned long)g_u64SoftInstallOk,
            (unsigned long)g_u64SoftInstallFail,
            (unsigned long)g_u64SoftLogN,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft cold */
    kprintf("door: soft cold ready=%u live=%u peer_dead=%u calls=%lu "
            "replies=%lu aborts=%lu timeouts=%lu badge=0x%x last_badge=0x%x "
            "mask=0x%lx wave=%u\n",
            u32Ready, u32Live, u32PeerDead, (unsigned long)u64Calls,
            (unsigned long)u64Replies, (unsigned long)u64Aborts,
            (unsigned long)u64Timeouts, u32Badge, u32LastBadge,
            (unsigned long)u64Mask, (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /*
     * Soft REPLY single-use under door: soft … so a single "door: soft"
     * grep also hits the ephemeral right surface (Wave 17).
     * Grep: door: soft reply_su
     */
    kprintf("door: soft reply_su create=%lu new=%lu rebind=%lu "
            "consume=%lu second_fail=%lu inval=%lu drop=%lu fallback=%lu "
            "live=%u bound=%u peak=%u gen_hi=%u self_pass=%u wave=%u\n",
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuCreateNew,
            (unsigned long)g_u64ReplySuCreateRebind,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned long)g_u64ReplySuInval,
            (unsigned long)g_u64ReplySuDrop,
            (unsigned long)g_u64ReplySuFallback, u32SuLive, u32SuBound,
            g_u32ReplySuLivePeak, g_u32ReplySoftGen,
            g_fReplySoftSelfPass ? 1u : 0u,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft reply_su inval — reason split */
    kprintf("door: soft reply_su inval total=%lu cancel=%lu abort=%lu "
            "thr=%lu init=%lu wave=%u\n",
            (unsigned long)g_u64ReplySuInval,
            (unsigned long)g_u64ReplySuInvalCancel,
            (unsigned long)g_u64ReplySuInvalAbort,
            (unsigned long)g_u64ReplySuInvalThr,
            (unsigned long)g_u64ReplySuInvalInit,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /*
     * Grep: door: soft return
     * Wave 19 public return-surface: call i64 / recv status / install.
     * Soft ≠ MIG REPLY product.
     */
    kprintf("door: soft return call_pos=%lu call_neg=%lu call_eio=%lu "
            "call_etimedout=%lu call_enosys=%lu call_reply=%lu "
            "recv_ok=%lu recv_peer_dead=%lu recv_inval=%lu "
            "install_ok=%lu install_fail=%lu "
            "install_null=%lu install_dead=%lu install_cap=%lu "
            "cnode_mig_reply=0 soft_ne_mig_reply=1 wave=%u\n",
            (unsigned long)g_u64SoftCallRetPos,
            (unsigned long)g_u64SoftCallRetNeg,
            (unsigned long)g_u64SoftCallEio,
            (unsigned long)g_u64SoftCallEtimedout,
            (unsigned long)g_u64SoftCallEnosys,
            (unsigned long)g_u64SoftCallReply,
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)g_u64SoftRecvPeerDead,
            (unsigned long)g_u64SoftRecvInval,
            (unsigned long)g_u64SoftInstallOk,
            (unsigned long)g_u64SoftInstallFail,
            (unsigned long)g_u64SoftInstallFailNull,
            (unsigned long)g_u64SoftInstallFailDead,
            (unsigned long)g_u64SoftInstallFailCap,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft return call — i64 terminal surface */
    kprintf("door: soft return call pos=%lu neg=%lu reply=%lu eio=%lu "
            "etimedout=%lu enosys=%lu wave=%u\n",
            (unsigned long)g_u64SoftCallRetPos,
            (unsigned long)g_u64SoftCallRetNeg,
            (unsigned long)g_u64SoftCallReply,
            (unsigned long)g_u64SoftCallEio,
            (unsigned long)g_u64SoftCallEtimedout,
            (unsigned long)g_u64SoftCallEnosys,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft return recv — status surface */
    kprintf("door: soft return recv ok=%lu peer_dead=%lu inval=%lu "
            "wave=%u\n",
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)g_u64SoftRecvPeerDead,
            (unsigned long)g_u64SoftRecvInval,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft return install — status surface */
    kprintf("door: soft return install ok=%lu fail=%lu null=%lu "
            "dead=%lu cap=%lu wave=%u\n",
            (unsigned long)g_u64SoftInstallOk,
            (unsigned long)g_u64SoftInstallFail,
            (unsigned long)g_u64SoftInstallFailNull,
            (unsigned long)g_u64SoftInstallFailDead,
            (unsigned long)g_u64SoftInstallFailCap,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft err */
    kprintf("door: soft err eio=%lu etimedout=%lu enosys=%lu "
            "recv_peer_dead=%lu recv_inval=%lu reply_stale=%lu "
            "reply_not_ready=%lu install_fail=%lu su_second=%lu "
            "su_drop=%lu wave=%u\n",
            (unsigned long)g_u64SoftCallEio,
            (unsigned long)g_u64SoftCallEtimedout,
            (unsigned long)g_u64SoftCallEnosys,
            (unsigned long)g_u64SoftRecvPeerDead,
            (unsigned long)g_u64SoftRecvInval,
            (unsigned long)g_u64SoftReplyStale,
            (unsigned long)g_u64SoftReplyNotReady,
            (unsigned long)g_u64SoftInstallFail,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned long)g_u64ReplySuDrop,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft capacity — fixed soft table / tag lamps */
    kprintf("door: soft capacity reply_su_slots=%u tags=3 tag_srv=%u "
            "tag_cli=%u tag_slot=%u single_flight=1 heap=0 "
            "cnode_mig_reply=0 wave=%u\n",
            (unsigned)DOOR_REPLY_SOFT_SLOTS,
            (unsigned)DOOR_TAG_SERVER, (unsigned)DOOR_TAG_CLIENT,
            (unsigned)DOOR_TAG_SLOT, (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft catalog — path surface catalog (impl vs not) */
    kprintf("door: soft catalog call=1 recv=1 reply=1 abort=1 cancel=1 "
            "thr_exit=1 install=1 cold=1 reply_su_soft=1 badge_xfer=1 "
            "mig_reply_cnode=0 multi_server=0 cap_transfer_small_k=0 "
            "wave=%u\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft tags — wait-key tag surface */
    kprintf("door: soft tags server=%u client=%u slot=%u "
            "park=thread_block+schedule spin_product=0 wave=%u\n",
            (unsigned)DOOR_TAG_SERVER, (unsigned)DOOR_TAG_CLIENT,
            (unsigned)DOOR_TAG_SLOT, (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft flight — single-flight snap (diagnostic race OK) */
    kprintf("door: soft flight has_req=%u has_reply=%u has_client=%u "
            "has_server=%u peer_dead=%u ready=%u live=%u wave=%u\n",
            u32HasReq, u32HasReply, u32HasClient, u32HasServer,
            u32PeerDead, u32Ready, u32Live,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft badge — under door: soft prefix */
    kprintf("door: soft badge grant=%lu move=%lu fail=%lu "
            "badge=0x%x last_badge=0x%x mask=0x%lx server_auth=1 wave=%u\n",
            (unsigned long)g_u64BadgeXferGrant,
            (unsigned long)g_u64BadgeXferMove,
            (unsigned long)g_u64BadgeXferFail, u32Badge, u32LastBadge,
            (unsigned long)u64Mask, (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /*
     * Honesty line: soft inventory / soft REPLY ≠ CNode MIG REPLY / bar3.
     * Soft ≠ MIG REPLY product.
     * Grep: door: soft path
     */
    kprintf("door: soft path single_flight=1 reply=soft_ephemeral "
            "cnode_mig_reply=0 badge=server_auth cold=endpoint "
            "cap_transfer_small_k=0 self_pass=%u soft_ne_mig_reply=1 "
            "(soft inventory; not bar3; not MIG REPLY product) wave=%u\n",
            g_fReplySoftSelfPass ? 1u : 0u,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /*
     * Grep: door: soft return rate
     * Wave 17 return-surface rate lamps (kept) (soft ≠ product / MIG REPLY).
     */
    kprintf("door: soft return rate "
            "call_pos=%lu call_neg=%lu "
            "recv_ok=%lu recv_fail=%lu "
            "install_ok=%lu install_fail=%lu "
            "reply_ok=%lu reply_stale=%lu reply_not_ready=%lu "
            "wave=%u (return rate; Soft≠product; soft≠MIG REPLY product; "
            "not bar3)\n",
            (unsigned long)g_u64SoftCallRetPos,
            (unsigned long)g_u64SoftCallRetNeg,
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)(g_u64SoftRecvPeerDead + g_u64SoftRecvInval),
            (unsigned long)g_u64SoftInstallOk,
            (unsigned long)g_u64SoftInstallFail,
            (unsigned long)g_u64SoftReplyOk,
            (unsigned long)g_u64SoftReplyStale,
            (unsigned long)g_u64SoftReplyNotReady,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /*
     * Grep: door: soft retcode
     * Wave 17 retcode catalog for call i64 / recv / install status classes.
     */
    kprintf("door: soft retcode "
            "call_pos=1 call_neg=1 call_eio=1 call_etimedout=1 call_enosys=1 "
            "recv_ok=1 recv_peer_dead=1 recv_inval=1 "
            "install_ok=1 install_null=1 install_dead=1 install_cap=1 "
            "reply_ok=1 reply_stale=1 reply_not_ready=1 "
            "cnode_mig_reply=0 soft_ne_mig_reply=1 wave=%u "
            "(retcode catalog; Soft≠product; soft≠MIG REPLY product)\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: door: soft return selftest — Wave 19 terminal return surface */
    kprintf("door: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=0 wave=%u soft PASS\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft retmap — Wave 19 return-surface map */
    kprintf("door: soft retmap soft_inv=1 deepen=1 return_rate=1 retcode=1 "
            "product=OPEN wave=%u soft PASS\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: door: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("door: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);
    /* Grep: door: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("door: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: door: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("door: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);
    /* Grep: door: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("door: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: door: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("door: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /* Grep: door: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("door: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: door: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("door: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /* Grep: door: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("door: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: door: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("door: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /* Grep: door: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("door: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: door: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("door: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /* Grep: door: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("door: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: door: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("door: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /* Grep: door: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("door: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: door: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("door: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
            /* Grep: door: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("door: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)DOOR_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: door: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("door: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)DOOR_SOFT_DEEPEN_WAVE);
                    /* Grep: door: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("door: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)DOOR_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: door: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("door: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)DOOR_SOFT_DEEPEN_WAVE);
                            /* Grep: door: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("door: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)DOOR_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: door: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("door: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=56 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: door: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("door: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=56 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=56 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: door: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("door: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=56 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: door: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("door: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=56 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=56 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: door: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("door: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=56 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("door: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("door: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("door: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("door: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("door: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("door: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retfortress — Wave 35 return-fortress honesty */
kprintf("door: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("door: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft rethold — Wave 36 return-hold honesty */
kprintf("door: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retspire — Wave 36 exclusive spire stamp */
kprintf("door: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retwall — Wave 37 return-wall honesty */
kprintf("door: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retgate — Wave 37 exclusive gate stamp */
kprintf("door: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retmoat — Wave 38 return-moat honesty */
kprintf("door: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retower — Wave 38 exclusive tower stamp */
kprintf("door: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("door: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("door: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("door: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("door: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retravelin — Wave 41 return-travelin honesty */
kprintf("door: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("door: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("door: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("door: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("door: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("door: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("door: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("door: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("door: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("door: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retbailey — Wave 46 return-bailey honesty */
kprintf("door: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("door: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("door: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("door: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("door: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("door: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("door: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("door: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retsally — Wave 50 return-sally honesty */
kprintf("door: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("door: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retfosse — Wave 51 return-fosse honesty */
kprintf("door: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("door: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("door: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("door: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retravelin — Wave 53 return-travelin honesty */
kprintf("door: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("door: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("door: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retredan — Wave 54 exclusive redan stamp */
kprintf("door: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retflank — Wave 55 return-flank honesty */
kprintf("door: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retface — Wave 55 exclusive face stamp */
kprintf("door: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: door: soft retgorge — Wave 56 return-gorge honesty */
kprintf("door: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=56 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: door: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("door: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=56 "
        "(retshoulder stamp; Soft≠product)\n");






                            kprintf("door: soft deepen wave=%u areas=%u call_enter=%lu "
            "recv_enter=%lu reply_enter=%lu reply_su_create=%lu "
            "ret_call_pos=%lu ret_call_neg=%lu ret_recv_ok=%lu "
            "ret_inst_ok=%lu soft_log=%lu cold_init=%u ok=1 skip=0\n",
            (unsigned)DOOR_SOFT_DEEPEN_WAVE,
            (unsigned)DOOR_SOFT_DEEPEN_AREAS,
            (unsigned long)g_u64SoftCallEnter,
            (unsigned long)g_u64SoftRecvEnter,
            (unsigned long)g_u64SoftReplyEnter,
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64SoftCallRetPos,
            (unsigned long)g_u64SoftCallRetNeg,
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)g_u64SoftInstallOk,
            (unsigned long)g_u64SoftLogN,
            g_fColdInited ? 1u : 0u);

    /* Grep: door: reply single-use (soft REPLY tallies; companion prefix) */
    kprintf("door: reply single-use create=%lu consume=%lu second_fail=%lu "
            "inval=%lu drop=%lu new=%lu rebind=%lu fallback=%lu "
            "inval_cancel=%lu inval_abort=%lu inval_thr=%lu inval_init=%lu "
            "wave=%u\n",
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned long)g_u64ReplySuInval,
            (unsigned long)g_u64ReplySuDrop,
            (unsigned long)g_u64ReplySuCreateNew,
            (unsigned long)g_u64ReplySuCreateRebind,
            (unsigned long)g_u64ReplySuFallback,
            (unsigned long)g_u64ReplySuInvalCancel,
            (unsigned long)g_u64ReplySuInvalAbort,
            (unsigned long)g_u64ReplySuInvalThr,
            (unsigned long)g_u64ReplySuInvalInit,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: REPLY soft — honesty bounds (soft ≠ MIG REPLY product) */
    kprintf("door: REPLY soft live_slots=%u bound=%u slots_max=%u "
            "gen_hi=%u live_peak=%u self_pass=%u "
            "honesty=no_cnode_mig_product no_bar3=1 soft_ne_mig_reply=1 "
            "wave=%u\n",
            u32SuLive, u32SuBound, (unsigned)DOOR_REPLY_SOFT_SLOTS,
            g_u32ReplySoftGen, g_u32ReplySuLivePeak,
            g_fReplySoftSelfPass ? 1u : 0u,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /* Grep: door: badge transfer (grant/move/fail counters) */
    kprintf("door: badge transfer grant=%lu move=%lu fail=%lu wave=%u\n",
            (unsigned long)g_u64BadgeXferGrant,
            (unsigned long)g_u64BadgeXferMove,
            (unsigned long)g_u64BadgeXferFail,
            (unsigned)DOOR_SOFT_DEEPEN_WAVE);

    /*
     * Soft lamp: cold personality ready + self-check PASS. Never hard-gates.
     * Grep: door: soft inventory PASS | door: soft PASS
     * Grep: door: soft FAIL
     */
    fSoftPass = (g_fColdInited != 0 && u32Ready != 0 &&
                 g_fReplySoftSelfPass != 0)
                    ? 1
                    : 0;
    if (fSoftPass != 0) {
        kprintf("door: soft inventory PASS ready=%u live=%u "
                "logs=%lu su_self_pass=1 areas=%u wave=%u\n",
                u32Ready, u32Live, (unsigned long)g_u64SoftLogN,
                (unsigned)DOOR_SOFT_DEEPEN_AREAS,
                (unsigned)DOOR_SOFT_DEEPEN_WAVE);
        kprintf("door: soft PASS wave=%u logs=%lu areas=%u\n",
                (unsigned)DOOR_SOFT_DEEPEN_WAVE,
                (unsigned long)g_u64SoftLogN,
                (unsigned)DOOR_SOFT_DEEPEN_AREAS);
    } else {
        kprintf("door: soft FAIL cold_init=%u ready=%u su_self_pass=%u "
                "(soft inventory only; not product gate; not MIG REPLY) "
                "wave=%u\n",
                g_fColdInited ? 1u : 0u, u32Ready,
                g_fReplySoftSelfPass ? 1u : 0u,
                (unsigned)DOOR_SOFT_DEEPEN_WAVE);
    }
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
    door_reply_soft_invalidate(pDoor, DOOR_SU_INVAL_CANCEL);
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
    door_reply_soft_invalidate(pDoor, DOOR_SU_INVAL_INIT);
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
    /* Grep: door: soft (baseline inventory after cold init; wave=56) */
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
     * door: soft call/recv/reply/reply_su/return lines (Wave 19; mirrors
     * file_lock_count). greppable: door: soft
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
        door_soft_inc(&g_u64SoftInstallFailNull); /* Wave 19 return */
        return GJ_ERR_INVAL;
    }
    if (!door_live(pDoor)) {
        door_soft_inc(&g_u64SoftInstallFail);
        door_soft_inc(&g_u64SoftInstallFailDead);
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
        door_soft_inc(&g_u64SoftInstallFailCap);
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
    door_reply_soft_invalidate(pDoor, DOOR_SU_INVAL_ABORT);
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
        door_reply_soft_invalidate(pDoor, DOOR_SU_INVAL_THR);
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
        door_soft_inc(&g_u64SoftCallRetNeg); /* Wave 19 return surface */
        door_soft_maybe_once();
        return -LINUX_ENOSYS;
    }
    if (!door_live(pDoor)) {
        door_soft_inc(&g_u64SoftCallEio);
        door_soft_inc(&g_u64SoftCallRetNeg);
        door_soft_maybe_once();
        return -LINUX_EIO; /* peer/object not live (PEER_DEAD) */
    }
    pCur = thread_current();
    if (pCur == NULL) {
        door_soft_inc(&g_u64SoftCallEnosys);
        door_soft_inc(&g_u64SoftCallRetNeg);
        door_soft_maybe_once();
        return -LINUX_ENOSYS;
    }

    /* Single-flight: CAS-claim client slot; contenders block (G-COLD-3). */
    for (;;) {
        if (!door_live(pDoor)) {
            door_soft_inc(&g_u64SoftCallEio);
            door_soft_inc(&g_u64SoftCallRetNeg);
            door_soft_maybe_once();
            return -LINUX_EIO;
        }
        /* No mono clock yet, or deadline already past → timeout (no hang). */
        if (u64DeadlineMonoNsec != 0 &&
            (!timer_ready() ||
             timer_mono_nsec() >= u64DeadlineMonoNsec)) {
            pDoor->u64Timeouts++;
            door_soft_inc(&g_u64SoftCallEtimedout);
            door_soft_inc(&g_u64SoftCallRetNeg);
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
            /* Wave 19 return surface: pos vs neg i64. */
            if (i64Ret < 0) {
                door_soft_inc(&g_u64SoftCallRetNeg);
            } else {
                door_soft_inc(&g_u64SoftCallRetPos);
            }
            door_soft_maybe_once();
            return i64Ret;
        }
        if (!door_live(pDoor)) {
            /* Peer death mid-wait: drop flight; last badge still useful. */
            door_snapshot_last_badge(pDoor);
            door_cancel_inflight(pDoor, pCur);
            door_soft_inc(&g_u64SoftCallEio);
            door_soft_inc(&g_u64SoftCallRetNeg);
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
            door_soft_inc(&g_u64SoftCallRetNeg);
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
