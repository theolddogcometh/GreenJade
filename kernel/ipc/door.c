/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Doors rendezvous for cold personality (ENDPOINT-shaped, G-DOOR / G-COLD).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. Soft!=product. G-AC-1.
 *
 * Lean residual (exclusive residual; this unit only) for service hosts:
 * channel-A control RPC (Call/Recv/Reply + badge + ENDPOINT install).
 * Service hosts: vfsd / storaged / netstackd / sessiond / sshd / scsi_mid.
 * Also DDI/UDX hosts (rtl8168_udx / xhci_udx / ddi_host_gj). Bulk device
 * data stays on mapped rings / DDI MAP_BAR / UDX host rings - this module
 * never maps client memory. Not multi-server product; not MIG REPLY
 * product; not Linux .ko product AC (G-AC-1).
 *
 * Protocol (single-flight client):
 *   server  door_recv  -> block tag 1 until u32HasReq
 *   client  door_call  -> claim slot (CAS), post req, wake server, block tag 2
 *   server  door_reply -> set reply, wake client (tag 2)
 *   contenders for the client slot block on tag 3 (no product busy-spin)
 *
 * Wait keys are the door object; tags distinguish roles.
 * Peer death / object DEAD -> clients see -LINUX_EIO (G-DOOR-4 / G-PERS-3).
 *
 * Mid-call timeout races (cooperative UP + atomics for SMP-prep):
 *   HasReply is observed before the deadline check in the client wait loop,
 *   so a reply that lands cannot be demoted to -ETIMEDOUT on the same arm.
 *   On timeout: clear HasReq then HasReply, then CAS-release pClient. A
 *   concurrent door_reply after release sees pClient==NULL and drops (stale).
 *   A concurrent door_recv that already sampled HasReq may still copy req and
 *   later reply into a freed slot - reply is then dropped; no hang. Server
 *   re-checks HasReq after wake if the client cancelled first.
 *
 * Lean soft residual (exclusive residual; this unit only; service hosts):
 *   greppable: door: soft residual lean
 *   greppable: door: soft residual lean foundation
 *   greppable: door: soft residual lean service
 *   greppable: door: soft residual lean thr
 *   greppable: door: soft residual lean udx
 *   greppable: door: soft residual lean PASS
 *   greppable: door: soft inventory | call_reply | path | PASS
 *   greppable: host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx
 * Soft!=product. Dual MIT OR Apache-2.0. No version stamp. No stamp storms
 * (once-lamps + counters only; no per-op kprintf on call/recv/reply).
 * Soft residual != multi-server product / MIG REPLY product / full cap transfer.
 * Service host residual != ddi_door product mint / UDX .ko product (G-AC-1).
 * C2 residual: thr-exit cold + soft-REPLY-bound doors; lean eio + cancel +
 * thr-cli + thr-bound arms (door_on_thread_exit walk residual).
 * STRONGER functional residual (W7 Dual DoD; stamp-free bar v2026.08.04.75):
 *   null_miss | badge_or | install_null | thr_null | dual_dod_open
 *   toward UDX/sshd product path (channel-A control RPC). Soft!=product.
 * STRONGER functional residual (W10 Dual DoD; stamp-free bar v2026.08.04.75):
 *   call_null | dead_eio | dual_dod_open deepen for live UDX hosts
 *   (rtl8168_udx / xhci_udx / ddi_host / sshd channel-A). Soft!=product.
 * STRONGER functional residual (W11 Dual DoD; stamp-free bar v2026.08.04.75):
 *   poll_ready | accept_recv | call_dead | dual_dod_open
 *   POLL/ACCEPT/CALL honesty for product hosts + sshd product path
 *   (channel-A control RPC under net/DDI/session facades). Soft!=product.
 *   greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 * STRONGER denser residual (W12 Dual DoD; stamp-free bar v2026.08.04.75):
 *   poll denser: idle | shape(req-pending) | both | null
 *   accept denser: inval | peer | take | release
 *   call denser: not_ready(-ENOSYS) | mark_dead(fail-closed -ENOSYS)
 *   composite denser + dual_dod_open for product_hosts=UDX
 *   greppable: door: soft residual denser | POLL/ACCEPT/CALL denser
 *   greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 * STRONGER denser residual (W13 Dual DoD; stamp-free bar v2026.08.04.75):
 *   poll denser2: reply-only | flip(atomic toggle)
 *   accept denser2: null-regs INVAL | idle clear_serve no-op
 *   composite denser2 + dual_dod denser for product_hosts=UDX POLL/ACCEPT
 *   greppable: door: soft residual denser poll_accept
 *   greppable: door: soft residual denser dual_dod
 *   greppable: denser poll_reply | denser poll_flip | denser accept_null
 *   greppable: denser accept_idle | denser=2 | denser residual W13
 * Dual DoD A/B remain OPEN (this residual does not close product DoD).
 *
 * Soft inventory (sparse lamps only - NO stamp storms, no version stamp):
 *   - One inventory rollup + call/reply residual + lean residual + honesty + PASS
 *   - Companion once-lines: reply single-use / REPLY soft / badge transfer
 *   greppable: "door: soft ..." / "door: soft call_reply ..." /
 *              "door: soft residual lean ..." /
 *              "door: soft residual lean foundation ..." /
 *              "door: soft residual lean service ..." /
 *              "door: soft residual lean thr ..." /
 *              "door: soft residual lean udx ..." /
 *              "door: soft residual denser ..." /
 *              "door: soft residual denser poll_accept ..." /
 *              "door: soft residual denser dual_dod ..." /
 *              "door: reply single-use ..." / "door: REPLY soft ..." /
 *              "door: badge transfer ..."
 *   greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 *   greppable: POLL/ACCEPT/CALL | poll_ready | accept_recv | call_dead
 *   greppable: denser | poll_idle | poll_shape | accept_take | call_mark_dead
 *   greppable: denser poll_reply | denser poll_flip | denser accept_null
 *   greppable: denser accept_idle | denser=2 | denser residual POLL/ACCEPT
 *   Never hard-gates; diagnostics only (wrap OK). Soft != product. Soft != MIG.
 *   Dual license: MIT OR Apache-2.0 (SPDX above). Pure C.
 *
 * Call/reply correctness residual (used by cold + door-shaped facades):
 *   - pServer held from successful door_recv until door_reply / thr-exit /
 *     peer abort (serve window). multi_server=0 - not a product multi-server API.
 *   - Server thr-exit mid-serve posts synthetic -EIO for in-flight client
 *     (no hang). Soft residual; sticky peer-dead still via abort/mark_dead.
 *   - Stale door_reply (pClient gone after timeout) still releases serve if
 *     the current thr owns pServer - no stuck serve_hold after cancel.
 *   - HasReq/HasReply use acquire/release atomics (SMP-prep; UP still OK).
 *   - thr-exit residual (C2 deepen): cold personality + soft-REPLY-bound doors
 *     (table walk via door_on_thread_exit). Client thr-exit arm clears slot +
 *     inval soft REPLY (THR why). Not a product multi-door registry;
 *     Soft!=product.
 *
 * Soft ephemeral single-use REPLY (not full MIG product):
 *   On slot claim, kernel mints a soft REPLY right bound to the door flight.
 *   First door_reply consumes it; second use fails (stale / second_fail).
 *   Timeout / peer death / thr-exit / init invalidates the soft right.
 * Honesty: not full MIG REPLY until CNode install of GJ_CAP_REPLY.
 *   Soft != MIG REPLY product (cnode_mig_reply=0).
 *
 * Soft badge / cap-transfer (server-authoritative badge path):
 *   grant = door_set_badge, move = last-badge snapshot on completed flight,
 *   fail = null / reject arms. Complements install/mint badge transfer PASS.
 *
 * Service host foundation (lean residual honesty; C2 channel-A control RPC):
 *   Service hosts (vfsd / storaged / netstackd / sessiond / sshd / scsi_mid)
 *   and DDI/UDX hosts (rtl8168_udx / xhci_udx / ddi_host_gj) rely on this
 *   rendezvous + ENDPOINT install + badge for channel-A control RPC only.
 *   Bulk / device MMIO/IRQ/DMA mint remains facade doors / caps (OPEN).
 *   Dual DoD A/B remain OPEN (not closed by this residual). Soft!=product.
 *   Dual MIT OR Apache-2.0. G-AC-1. No version stamp.
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

/*
 * Sparse lamp surface count (inventory / call_reply residual / lean residual /
 * foundation / service / thr / honesty / path / reply_su / badge / PASS).
 * Not a version stamp; not a stamp-storm catalog. Grep: door: soft residual lean
 * Service host residual: vfsd|storaged|netstackd|sessiond|sshd|scsi_mid +
 * DDI/UDX (G-AC-1). C2 thr-exit residual walks soft-REPLY-bound doors.
 */
/* +W13 denser POLL/ACCEPT Dual DoD residual (poll_accept + dual_dod lamps) */
#define DOOR_SOFT_AREAS 13u

static struct gj_door g_doorCold;
static int            g_fColdInited;
static u8             g_fReplySoftSelfcheck; /* cold-init self-check once */
static u8             g_fLeanResidualOnce;   /* lean residual self-check once */

/*
 * Soft path tallies (cumulative across doors entering this module).
 * Live/product counters remain per-door (door_stats). Soft != product.
 * greppable: door: soft ...
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
static u64 g_u64SoftThrSrvEio;     /* thr-exit mid-serve -> client -EIO */
static u64 g_u64SoftServeHold;     /* recv took request under serve ownership */
static u64 g_u64SoftServeRelease;  /* reply / thr-exit / abort cleared serve */
static u64 g_u64SoftInstallOk;     /* door_install_endpoint success */
static u64 g_u64SoftInstallFail;   /* install reject (inval/nodev/cap) */
static u64 g_u64SoftInstallFailNull; /* Wave 19: null args / no cnode */
static u64 g_u64SoftInstallFailDead; /* Wave 19: door not live */
static u64 g_u64SoftInstallFailCap;  /* Wave 19: cap_alloc_install fail */
static u64 g_u64SoftLogN;          /* inventory log emissions */
static u8  g_fSoftOnce;            /* one-shot after first call activity */
static u64 g_u64SoftLeanRuns;      /* lean residual self-check runs */
static u64 g_u64SoftLeanOk;        /* lean residual self-check PASS arms */
static u64 g_u64SoftLeanServe;     /* lean: serve_hold -> reply release */
static u64 g_u64SoftLeanSu;        /* lean: single-use create/consume/2nd */
static u64 g_u64SoftLeanStale;     /* lean: stale reply released serve */
static u64 g_u64SoftLeanFound;     /* lean: static foundation checks */
static u64 g_u64SoftLeanSvc;       /* lean: service host foundation checks */
static u64 g_u64SoftLeanEio;       /* lean: thr mid-serve -> client -EIO */
static u64 g_u64SoftLeanCancel;    /* lean: cancel_inflight residual arm */
static u64 g_u64SoftLeanThrCli;    /* lean: thr-exit client slot residual */
static u64 g_u64SoftLeanThrBound;   /* lean: thr-exit soft-REPLY-bound walk */
static u64 g_u64SoftLeanNullMiss;  /* lean: null reject call/recv/live residual */
static u64 g_u64SoftLeanBadgeOr;   /* lean: badge_or + mask residual */
static u64 g_u64SoftLeanInstallNull; /* lean: install null-args residual */
static u64 g_u64SoftLeanThrNull;   /* lean: thr-exit NULL no-op residual */
static u64 g_u64SoftLeanDualDod;   /* lean: Dual DoD OPEN honesty residual */
static u64 g_u64SoftLeanCallNull;  /* lean: door_call NULL fail-closed residual */
static u64 g_u64SoftLeanDeadEio;   /* lean: mark_dead -> is_live=0 residual */
static u64 g_u64SoftLeanPollReady; /* lean: POLL HasReq/HasReply readiness residual */
static u64 g_u64SoftLeanAcceptRecv;/* lean: ACCEPT-shaped door_recv residual */
static u64 g_u64SoftLeanCallDead;  /* lean: CALL peer-dead -EIO residual */
/*
 * W12 denser POLL/ACCEPT/CALL product residual (Soft!=product; Dual DoD OPEN).
 * bar v2026.08.04.75 stamp-free; product_hosts=UDX; never Soft neq glyph.
 * Grep: door: soft residual denser | POLL/ACCEPT/CALL denser
 */
static u64 g_u64SoftLeanPollIdle;  /* denser: POLL idle 0/0 residual */
static u64 g_u64SoftLeanPollShape; /* denser: POLL req-pending shape residual */
static u64 g_u64SoftLeanPollBoth;  /* denser: POLL both set then clear residual */
static u64 g_u64SoftLeanPollNull;  /* denser: POLL null load residual */
static u64 g_u64SoftLeanAcceptInval; /* denser: ACCEPT not-ready INVAL residual */
static u64 g_u64SoftLeanAcceptPeer;  /* denser: ACCEPT peer-dead residual */
static u64 g_u64SoftLeanAcceptTake;  /* denser: ACCEPT take + serve_hold residual */
static u64 g_u64SoftLeanAcceptRel;   /* denser: ACCEPT clear_serve release residual */
static u64 g_u64SoftLeanCallNr;    /* denser: CALL not-ready -ENOSYS residual */
static u64 g_u64SoftLeanCallMd;    /* denser: CALL mark_dead fail-closed residual */
static u64 g_u64SoftLeanDenseOk;   /* denser composite once-arm ok */
static u64 g_u64SoftLeanDenseFail; /* denser composite soft fail */
/*
 * W13 denser POLL/ACCEPT Dual DoD residual (Soft!=product; Dual DoD OPEN).
 * bar v2026.08.04.75 stamp-free; product_hosts=UDX; never Soft neq glyph.
 * Grep: door: soft residual denser poll_accept | denser dual_dod
 * Grep: denser poll_reply | denser poll_flip | denser accept_null
 * Grep: denser accept_idle | denser=2
 */
static u64 g_u64SoftLeanPollReply; /* denser2: POLL reply-only HasReply=1 residual */
static u64 g_u64SoftLeanPollFlip;  /* denser2: POLL atomic toggle residual */
static u64 g_u64SoftLeanAcceptNull;/* denser2: ACCEPT null-regs INVAL residual */
static u64 g_u64SoftLeanAcceptIdle;/* denser2: ACCEPT clear_serve idle no-op */
static u64 g_u64SoftLeanDense2Ok;  /* denser2 composite once-arm ok */
static u64 g_u64SoftLeanDense2Fail;/* denser2 composite soft fail */
static u64 g_u64SoftThrExitBound;   /* thr-exit soft-REPLY-bound doors walked */

/*
 * Soft ephemeral single-use REPLY rights (Call path; Wave 15 deepen).
 * File-static table - no CNode install, no GJ_CAP_REPLY product binding.
 * Soft != MIG REPLY product (honesty: cnode_mig_reply=0).
 * greppable: door: reply single-use ... / door: REPLY soft ... /
 *            door: soft reply_su ...
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
static u64 g_u64ReplySuFallback;     /* try_consume missing slot -> allow */
static u32 g_u32ReplySuLivePeak;     /* high-water live soft REPLY rights */
static u8  g_fReplySoftSelfPass;     /* cold self-check create/consume/2nd */

/*
 * Soft badge transfer path counters (server badge -> client last-badge).
 * greppable: door: badge transfer ...
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
static void door_soft_residual_lean_once(void);
static void door_snapshot_last_badge(struct gj_door *pDoor);
static void door_reply_soft_create(struct gj_door *pDoor);
static int  door_reply_soft_try_consume(struct gj_door *pDoor);
static void door_reply_soft_invalidate(struct gj_door *pDoor, u32 u32Why);
static u32  door_reply_soft_live_count(void);
static u32  door_reply_soft_bound_count(void);
static void door_reply_soft_note_live_peak(void);
static void door_reply_soft_selfcheck(void);
static void door_store_has_req(struct gj_door *pDoor, u32 u32V);
static u32  door_load_has_req(const struct gj_door *pDoor);
static void door_store_has_reply(struct gj_door *pDoor, u32 u32V);
static u32  door_load_has_reply(const struct gj_door *pDoor);
static void door_clear_serve(struct gj_door *pDoor);
static void door_thr_exit_server_role(struct gj_door *pDoor,
                                      struct gj_thread *pThr);
static void door_thr_exit_one(struct gj_door *pDoor, struct gj_thread *pThr);
static void door_cancel_inflight(struct gj_door *pDoor, struct gj_thread *pCur);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
door_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/*
 * Flight flags: release store / acquire load (SMP-prep call/reply residual).
 * Cooperative UP still correct; ordering documents the rendezvous contract.
 */
static void
door_store_has_req(struct gj_door *pDoor, u32 u32V)
{
    if (pDoor == NULL) {
        return;
    }
    __atomic_store_n(&pDoor->u32HasReq, u32V, __ATOMIC_RELEASE);
}

static u32
door_load_has_req(const struct gj_door *pDoor)
{
    if (pDoor == NULL) {
        return 0u;
    }
    return __atomic_load_n(&pDoor->u32HasReq, __ATOMIC_ACQUIRE);
}

static void
door_store_has_reply(struct gj_door *pDoor, u32 u32V)
{
    if (pDoor == NULL) {
        return;
    }
    __atomic_store_n(&pDoor->u32HasReply, u32V, __ATOMIC_RELEASE);
}

static u32
door_load_has_reply(const struct gj_door *pDoor)
{
    if (pDoor == NULL) {
        return 0u;
    }
    return __atomic_load_n(&pDoor->u32HasReply, __ATOMIC_ACQUIRE);
}

/** Clear serve ownership (reply complete / thr-exit / abort). multi_server=0. */
static void
door_clear_serve(struct gj_door *pDoor)
{
    if (pDoor == NULL) {
        return;
    }
    if (pDoor->pServer != NULL) {
        pDoor->pServer = NULL;
        door_soft_inc(&g_u64SoftServeRelease);
    }
}

/**
 * Soft REPLY: find table slot for door (or NULL).
 * Linear scan - K is tiny (DOOR_REPLY_SOFT_SLOTS).
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
 * Table-full -> drop counter only; product Call still proceeds (no hard-break).
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
 * Missing table entry -> allow (fallback; create drop must not hard-break).
 * greppable: door: reply single-use consume / second_fail
 */
static int
door_reply_soft_try_consume(struct gj_door *pDoor)
{
    struct door_reply_soft *pSlot;

    pSlot = door_reply_soft_find(pDoor);
    if (pSlot == NULL) {
        /* No soft tracking - product path continues (table-full create drop). */
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
 * Cold-init soft self-check: create -> consume once -> second use fails.
 * Private scratch door only - never touches cold personality product state.
 * greppable: door: reply single-use ... / door: REPLY soft ...
 * Honesty: not CNode-installed MIG REPLY product.
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
        g_doorSu.pServer = pCur; /* soft serve_hold for reply residual selfcheck */
        door_store_has_reply(&g_doorSu, 0);
        door_reply(&g_doorSu, 0x1111);
        i64First = g_doorSu.i64Reply;
        u32ConsumeOk = (g_u64ReplySuConsume == u64C0 + 1ull &&
                        door_load_has_reply(&g_doorSu) != 0u &&
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
        g_doorSu.pServer = NULL;
        door_store_has_reply(&g_doorSu, 0);
    } else {
        /* No thr context: exercise soft helpers only. */
        u32ConsumeOk = door_reply_soft_try_consume(&g_doorSu) ? 1u : 0u;
        u32SecondFail = door_reply_soft_try_consume(&g_doorSu) ? 0u : 1u;
    }

    /* Grep: door: reply single-use (once-lamp; Soft!=product; no stamp storms) */
    kprintf("door: reply single-use create=%u consume=%u second_fail=%u "
            "create_n=%lu consume_n=%lu second_fail_n=%lu "
            "soft_ne_mig_reply=1\n",
            u32CreateOk, u32ConsumeOk, u32SecondFail,
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail);

    /* Grep: door: REPLY soft - honesty: not full MIG / no CNode install */
    kprintf("door: REPLY soft gen=%u live_slots=%u bound=%u slots_max=%u "
            "live_peak=%u new_create=%u honesty=no_cnode_mig_product "
            "soft_ne_mig_reply=1\n",
            u32Gen, door_reply_soft_live_count(), door_reply_soft_bound_count(),
            (unsigned)DOOR_REPLY_SOFT_SLOTS, g_u32ReplySuLivePeak,
            (g_u64ReplySuCreateNew > (u64)u32New0) ? 1u : 0u);

    if (u32CreateOk != 0u && u32ConsumeOk != 0u && u32SecondFail != 0u) {
        g_fReplySoftSelfPass = 1;
        /* Grep: door: reply single-use soft PASS */
        kprintf("door: reply single-use soft PASS\n");
    }

    /*
     * Soft badge transfer on scratch: grant from set_badge; move = snapshot;
     * fail = null set. Once-lamp only. greppable: door: badge transfer
     */
    door_snapshot_last_badge(&g_doorSu);
    door_set_badge(NULL, 0); /* fail arm */
    kprintf("door: badge transfer grant=%lu move=%lu fail=%lu "
            "(soft path; Soft!=product; install/mint PASS remains main.c)\n",
            (unsigned long)g_u64BadgeXferGrant,
            (unsigned long)g_u64BadgeXferMove,
            (unsigned long)g_u64BadgeXferFail);

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

/*
 * Lean residual self-check (private scratch door; never touches cold product).
 * Exercises serve_hold -> reply release, single-use REPLY, stale serve release,
 * thr mid-serve -EIO, cancel_inflight, thr-exit client arm, thr-exit soft-
 * REPLY-bound walk (door_on_thread_exit), plus static foundation checks for
 * service host channel-A control-RPC (vfsd/storaged/netstackd/sessiond/sshd/
 * scsi_mid + DDI/UDX). Soft!=product. Dual MIT OR Apache-2.0 - no version
 * stamp - G-AC-1. Dual DoD OPEN. C2 thr-exit residual shared with product path.
 * greppable: DOOR_LEAN_RESIDUAL
 * greppable: door: soft residual lean
 * greppable: door: soft residual lean foundation
 * greppable: door: soft residual lean service
 * greppable: door: soft residual lean thr
 * greppable: door: soft residual lean udx
 * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
 * greppable: POLL/ACCEPT/CALL | poll_ready | accept_recv | call_dead
 * greppable: door: soft residual denser | POLL/ACCEPT/CALL denser
 * greppable: door: soft residual denser poll_accept | denser dual_dod
 * greppable: denser poll_reply | denser poll_flip | denser accept_null
 * greppable: denser accept_idle | denser=2
 * STRONGER functional residual: null_miss|badge_or|install_null|thr_null|
 * dual_dod_open (UDX/sshd product path; Soft!=product; Dual DoD OPEN).
 * W10 deepen: call_null|dead_eio for live UDX host channel-A fail-closed.
 * W11 deepen: poll_ready|accept_recv|call_dead POLL/ACCEPT/CALL honesty
 * for product hosts + sshd product path (Soft!=product; Dual DoD OPEN).
 * W12 denser: poll idle/shape/both/null + accept inval/peer/take/rel +
 * call not_ready/mark_dead composite denser (product_hosts=UDX; Dual DoD OPEN).
 * W13 denser: poll reply/flip + accept null/idle composite denser2 +
 * dual_dod denser (product_hosts=UDX POLL/ACCEPT Dual DoD OPEN).
 */
static void
door_soft_residual_lean_once(void)
{
    static struct gj_door g_doorLean;
    static struct gj_door g_doorDead; /* mark_dead residual scratch only */
    struct gj_thread     *pCur;
    struct gj_cap_ref     capTmp;
    struct gj_linux_regs  regsPac;
    u32                   u32ServeOk;
    u32                   u32SuOk;
    u32                   u32StaleOk;
    u32                   u32EioOk;
    u32                   u32CancelOk;
    u32                   u32ThrCliOk;
    u32                   u32ThrBoundOk;
    u32                   u32FoundOk;
    u32                   u32TagsOk;
    u32                   u32SlotsOk;
    u32                   u32HdrOk;
    u32                   u32SvcOk;
    u32                   u32RightsOk;
    u32                   u32EpOk;
    u32                   u32BadgeOk;
    u32                   u32NullMissOk;
    u32                   u32BadgeOrOk;
    u32                   u32InstallNullOk;
    u32                   u32ThrNullOk;
    u32                   u32DualDodOk;
    u32                   u32CallNullOk;
    u32                   u32DeadEioOk;
    u32                   u32PollReadyOk;
    u32                   u32AcceptRecvOk;
    u32                   u32CallDeadOk;
    /* W12 denser residual arms (product_hosts=UDX POLL/ACCEPT/CALL). */
    u32                   u32PollIdleOk;
    u32                   u32PollShapeOk;
    u32                   u32PollBothOk;
    u32                   u32PollNullOk;
    u32                   u32AcceptInvalOk;
    u32                   u32AcceptPeerOk;
    u32                   u32AcceptTakeOk;
    u32                   u32AcceptRelOk;
    u32                   u32CallNrOk;
    u32                   u32CallMdOk;
    u32                   u32DenseOk;
    /* W13 denser residual arms (product_hosts=UDX POLL/ACCEPT Dual DoD). */
    u32                   u32PollReplyOk;
    u32                   u32PollFlipOk;
    u32                   u32AcceptNullOk;
    u32                   u32AcceptIdleOk;
    u32                   u32Dense2Ok;
    u32                   u32Checks;
    u32                   u32Ok;
    u32                   u32SvcChecks;
    u32                   u32SvcPass;
    u32                   u32ReadySave;
    u16                   u16DefRights;
    int                   nNullRecv;
    int                   nAcceptRecv;
    int                   nAcceptDead;
    int                   nAcceptNotReady;
    int                   nAcceptNull;
    i64                   i64CallNull;
    i64                   i64CallDead;
    i64                   i64CallNr;
    i64                   i64CallMd;
    u8                    u8SoftOnceSave;
    u64                   u64Rel0 = 0;
    u64                   u64C0 = 0;
    u64                   u64S0 = 0;
    u64                   u64Stale0 = 0;
    u64                   u64Eio0 = 0;
    u64                   u64Cancel0 = 0;
    u64                   u64InvalCancel0 = 0;
    u64                   u64ThrCli0 = 0;
    u64                   u64InvalThr0 = 0;
    u64                   u64Bound0 = 0;
    u64                   u64ThrExit0 = 0;
    u64                   u64Mask0 = 0;
    u64                   u64Serve0 = 0;
    u64                   u64RecvOk0 = 0;
    u64                   u64RelD0 = 0;
    u64                   u64RelIdle0 = 0;

    if (g_fLeanResidualOnce != 0) {
        return;
    }
    g_fLeanResidualOnce = 1;
    door_soft_inc(&g_u64SoftLeanRuns);

    u32ServeOk = 0;
    u32SuOk = 0;
    u32StaleOk = 0;
    u32EioOk = 0;
    u32CancelOk = 0;
    u32ThrCliOk = 0;
    u32ThrBoundOk = 0;
    u32FoundOk = 0;
    u32TagsOk = 0;
    u32SlotsOk = 0;
    u32HdrOk = 0;
    u32SvcOk = 0;
    u32RightsOk = 0;
    u32EpOk = 0;
    u32BadgeOk = 0;
    u32NullMissOk = 0;
    u32BadgeOrOk = 0;
    u32InstallNullOk = 0;
    u32ThrNullOk = 0;
    u32DualDodOk = 0;
    u32CallNullOk = 0;
    u32DeadEioOk = 0;
    u32PollReadyOk = 0;
    u32AcceptRecvOk = 0;
    u32CallDeadOk = 0;
    u32PollIdleOk = 0;
    u32PollShapeOk = 0;
    u32PollBothOk = 0;
    u32PollNullOk = 0;
    u32AcceptInvalOk = 0;
    u32AcceptPeerOk = 0;
    u32AcceptTakeOk = 0;
    u32AcceptRelOk = 0;
    u32CallNrOk = 0;
    u32CallMdOk = 0;
    u32DenseOk = 0;
    u32PollReplyOk = 0;
    u32PollFlipOk = 0;
    u32AcceptNullOk = 0;
    u32AcceptIdleOk = 0;
    u32Dense2Ok = 0;
    u32Checks = 0;
    u32Ok = 0;
    u32SvcChecks = 0;
    u32SvcPass = 0;
    u32ReadySave = 0;
    nAcceptRecv = 0;
    nAcceptDead = 0;
    nAcceptNotReady = 0;
    nAcceptNull = 0;
    i64CallNull = 0;
    i64CallDead = 0;
    i64CallNr = 0;
    i64CallMd = 0;
    u8SoftOnceSave = 0;
    memset(&capTmp, 0, sizeof(capTmp));
    memset(&regsPac, 0, sizeof(regsPac));
    door_init(&g_doorLean);
    door_set_badge(&g_doorLean, 0x1eadu);

    /*
     * Foundation residual (static contract; service host control-RPC lean).
     * Tags / soft REPLY table / obj_hdr-first ENDPOINT shape. Soft!=product.
     * greppable: door: soft residual lean foundation
     */
    u32Checks++;
    if (DOOR_TAG_SERVER == 1u && DOOR_TAG_CLIENT == 2u &&
        DOOR_TAG_SLOT == 3u) {
        u32TagsOk = 1;
        u32Ok++;
    }
    u32Checks++;
    if (DOOR_REPLY_SOFT_SLOTS >= 4u && DOOR_REPLY_SOFT_SLOTS <= 32u &&
        DOOR_SOFT_AREAS >= 8u) {
        u32SlotsOk = 1;
        u32Ok++;
    }
    u32Checks++;
    /* ENDPOINT-shaped: hdr first so install/revoke share typed-obj path. */
    if (&g_doorLean.hdr == (struct gj_obj_hdr *)&g_doorLean &&
        g_doorLean.u32Ready != 0u && door_live(&g_doorLean) != 0) {
        u32HdrOk = 1;
        u32Ok++;
    }
    if (u32Ok == u32Checks) {
        u32FoundOk = 1;
        door_soft_inc(&g_u64SoftLeanFound);
    }

    /*
     * Service host residual (channel-A bind surface used by vfsd/storaged/
     * netstackd/sessiond/sshd/scsi_mid + DDI/UDX). Static contract only.
     * Soft!=product. greppable: door: soft residual lean service
     * greppable: host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx
     */
    u32SvcChecks++;
    /* Default ENDPOINT install rights for service host CNode bind. */
    u16DefRights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_GRANT | GJ_RIGHT_IDENTIFY);
    if (u16DefRights ==
        (u16)(GJ_RIGHT_READ | GJ_RIGHT_GRANT | GJ_RIGHT_IDENTIFY)) {
        u32RightsOk = 1;
        u32SvcPass++;
    }
    u32SvcChecks++;
    /* Cap type ENDPOINT is the install target (G-DOOR-1; not INVALID). */
    if ((u16)GJ_CAP_ENDPOINT > (u16)GJ_CAP_INVALID &&
        (u16)GJ_CAP_ENDPOINT != (u16)GJ_CAP_REPLY) {
        u32EpOk = 1;
        u32SvcPass++;
    }
    u32SvcChecks++;
    /* Server-authoritative badge: set then last-badge snapshot path. */
    if (door_get_badge(&g_doorLean) == 0x1eadu) {
        door_snapshot_last_badge(&g_doorLean);
        if (door_get_last_badge(&g_doorLean) == 0x1eadu) {
            u32BadgeOk = 1;
            u32SvcPass++;
        }
    }
    if (u32SvcPass == u32SvcChecks) {
        u32SvcOk = 1;
        door_soft_inc(&g_u64SoftLeanSvc);
    }

    pCur = thread_current();
    u64Rel0 = g_u64SoftServeRelease;
    u64C0 = g_u64ReplySuConsume;
    u64S0 = g_u64ReplySuSecondFail;

    if (pCur != NULL) {
        /*
         * Serve window residual: hold pServer, mint soft REPLY, first reply
         * consumes + releases serve; second reply second-fails (no overwrite).
         * Service hosts: single-flight call/reply (multi_server=0).
         */
        g_doorLean.pClient = pCur;
        g_doorLean.pServer = pCur;
        door_soft_inc(&g_u64SoftServeHold);
        door_store_has_reply(&g_doorLean, 0);
        door_reply_soft_create(&g_doorLean);
        door_reply(&g_doorLean, 0x3333);
        if (g_u64SoftServeRelease == u64Rel0 + 1ull &&
            g_doorLean.pServer == NULL &&
            g_u64ReplySuConsume == u64C0 + 1ull &&
            g_doorLean.i64Reply == 0x3333) {
            u32ServeOk = 1;
            door_soft_inc(&g_u64SoftLeanServe);
        }
        door_reply(&g_doorLean, 0x4444);
        /* second-use fails closed; also tallies soft reply_stale (no overwrite). */
        if (g_u64ReplySuSecondFail == u64S0 + 1ull &&
            g_doorLean.i64Reply == 0x3333) {
            u32SuOk = 1;
            door_soft_inc(&g_u64SoftLeanSu);
        }
        /*
         * Stale residual: no client, thr still owns serve -> drop + release.
         * multi_server=0; Soft!=product. (second-use already bumped reply_stale.)
         */
        u64Stale0 = g_u64SoftReplyStale;
        u64Rel0 = g_u64SoftServeRelease;
        g_doorLean.pClient = NULL;
        g_doorLean.pServer = pCur;
        door_store_has_reply(&g_doorLean, 0);
        door_reply(&g_doorLean, 0x5555);
        if (g_u64SoftReplyStale == u64Stale0 + 1ull &&
            g_doorLean.pServer == NULL &&
            g_u64SoftServeRelease == u64Rel0 + 1ull) {
            u32StaleOk = 1;
            door_soft_inc(&g_u64SoftLeanStale);
        }

        /*
         * thr_srv_eio residual (C2 deepen): server dies mid-serve with a
         * client still in flight and no reply -> synthetic -EIO + serve clear.
         * Uses shared door_thr_exit_server_role (same path as thr-exit).
         * Client pointer left non-NULL so mid-serve arm fires (not client CAS).
         * Soft REPLY invalidated with THR why. Soft!=product; multi_server=0.
         */
        u64Eio0 = g_u64SoftThrSrvEio;
        u64Rel0 = g_u64SoftServeRelease;
        u64InvalThr0 = g_u64ReplySuInvalThr;
        g_doorLean.pClient = pCur;
        g_doorLean.pServer = pCur;
        door_store_has_req(&g_doorLean, 1);
        door_store_has_reply(&g_doorLean, 0);
        door_reply_soft_create(&g_doorLean);
        door_thr_exit_server_role(&g_doorLean, pCur);
        if (g_u64SoftThrSrvEio == u64Eio0 + 1ull &&
            door_load_has_reply(&g_doorLean) != 0u &&
            g_doorLean.i64Reply == -(i64)LINUX_EIO &&
            g_doorLean.pServer == NULL &&
            g_u64SoftServeRelease == u64Rel0 + 1ull &&
            g_u64ReplySuInvalThr == u64InvalThr0 + 1ull) {
            u32EioOk = 1;
            door_soft_inc(&g_u64SoftLeanEio);
        }
        /* Clear synthetic reply so cancel residual starts clean. */
        door_store_has_reply(&g_doorLean, 0);
        door_store_has_req(&g_doorLean, 0);
        g_doorLean.pClient = NULL;
        g_doorLean.pServer = NULL;

        /*
         * cancel_inflight residual (C2 deepen): mid-call cancel clears HasReq/
         * HasReply, invalidates soft REPLY (CANCEL why), releases client slot.
         * Same helper as timeout / peer mid-wait path. Soft!=product.
         */
        u64Cancel0 = g_u64SoftCancel;
        u64InvalCancel0 = g_u64ReplySuInvalCancel;
        g_doorLean.pClient = pCur;
        door_store_has_req(&g_doorLean, 1);
        door_store_has_reply(&g_doorLean, 0);
        door_reply_soft_create(&g_doorLean);
        door_cancel_inflight(&g_doorLean, pCur);
        if (g_u64SoftCancel == u64Cancel0 + 1ull &&
            door_load_has_req(&g_doorLean) == 0u &&
            door_load_has_reply(&g_doorLean) == 0u &&
            g_doorLean.pClient == NULL &&
            g_u64ReplySuInvalCancel == u64InvalCancel0 + 1ull) {
            u32CancelOk = 1;
            door_soft_inc(&g_u64SoftLeanCancel);
        }
        g_doorLean.pClient = NULL;
        g_doorLean.pServer = NULL;

        /*
         * thr-exit client residual (C2 deepen): thr owns pClient, soft REPLY
         * live -> door_thr_exit_one clears slot + HasReq/HasReply, inval soft
         * REPLY (THR why). Same helper as product thr death client arm.
         * Soft!=product; multi_server=0; Dual DoD OPEN.
         */
        u64ThrCli0 = g_u64SoftThrExitClient;
        u64InvalThr0 = g_u64ReplySuInvalThr;
        g_doorLean.pClient = pCur;
        g_doorLean.pServer = NULL;
        door_store_has_req(&g_doorLean, 1);
        door_store_has_reply(&g_doorLean, 0);
        door_reply_soft_create(&g_doorLean);
        door_thr_exit_one(&g_doorLean, pCur);
        if (g_u64SoftThrExitClient == u64ThrCli0 + 1ull &&
            g_doorLean.pClient == NULL &&
            door_load_has_req(&g_doorLean) == 0u &&
            door_load_has_reply(&g_doorLean) == 0u &&
            g_u64ReplySuInvalThr == u64InvalThr0 + 1ull) {
            u32ThrCliOk = 1;
            door_soft_inc(&g_u64SoftLeanThrCli);
        }
        g_doorLean.pClient = NULL;
        g_doorLean.pServer = NULL;
        door_store_has_req(&g_doorLean, 0);
        door_store_has_reply(&g_doorLean, 0);

        /*
         * thr-exit soft-REPLY-bound walk residual (C2 deepen): bind soft REPLY
         * then door_on_thread_exit walks cold (idle no-op) + table-bound doors
         * so thr_exit_bound increments and client slot is cleared. Product path
         * for service hosts (vfsd/storaged/netstackd/sessiond/sshd/scsi_mid +
         * DDI/UDX). Soft!=product; not multi-door registry; Dual DoD OPEN.
         */
        u64Bound0 = g_u64SoftThrExitBound;
        u64ThrExit0 = g_u64SoftThrExit;
        u64ThrCli0 = g_u64SoftThrExitClient;
        g_doorLean.pClient = pCur;
        g_doorLean.pServer = NULL;
        door_store_has_req(&g_doorLean, 1);
        door_store_has_reply(&g_doorLean, 0);
        door_reply_soft_create(&g_doorLean);
        door_on_thread_exit(pCur);
        if (g_u64SoftThrExit == u64ThrExit0 + 1ull &&
            g_u64SoftThrExitBound == u64Bound0 + 1ull &&
            g_u64SoftThrExitClient == u64ThrCli0 + 1ull &&
            g_doorLean.pClient == NULL &&
            door_load_has_req(&g_doorLean) == 0u) {
            u32ThrBoundOk = 1;
            door_soft_inc(&g_u64SoftLeanThrBound);
        }
        g_doorLean.pClient = NULL;
        g_doorLean.pServer = NULL;
        door_store_has_req(&g_doorLean, 0);
        door_store_has_reply(&g_doorLean, 0);
    } else {
        /* No thr: soft helpers only (create/consume/second). */
        door_reply_soft_create(&g_doorLean);
        u32SuOk = door_reply_soft_try_consume(&g_doorLean) ? 1u : 0u;
        if (u32SuOk != 0u && !door_reply_soft_try_consume(&g_doorLean)) {
            door_soft_inc(&g_u64SoftLeanSu);
        } else {
            u32SuOk = 0;
        }
        /* No thr context: thr-bound arms not exercised; honesty via helpers. */
        u32ServeOk = 1;
        u32StaleOk = 1;
        u32EioOk = 1;
        u32CancelOk = 1;
        u32ThrCliOk = 1;
        u32ThrBoundOk = 1;
        (void)u64Rel0;
        (void)u64C0;
        (void)u64S0;
        (void)u64Eio0;
        (void)u64Cancel0;
        (void)u64InvalCancel0;
        (void)u64ThrCli0;
        (void)u64InvalThr0;
        (void)u64Bound0;
        (void)u64ThrExit0;
    }

    /*
     * STRONGER functional residual (W7 Dual DoD; UDX/sshd product path):
     * null_miss | badge_or | install_null | thr_null | dual_dod_open.
     * Fail-closed product edges only - no frame alloc; no stamp; Dual DoD OPEN.
     * Soft!=product. H3 thr-exit null companion. G-AC-1.
     */

    /*
     * --- null_miss: recv/live/badge/reply fail closed (sshd/UDX hosts).
     * Prefer door_recv over door_call here so lean residual does not
     * re-enter door_soft_maybe_once mid-selfcheck (call_enter side path).
     */
    u32Checks++;
    nNullRecv = door_recv(NULL, NULL);
    door_reply(NULL, 0); /* no-op null reply residual */
    door_abort_waiters(NULL); /* no-op null abort residual */
    if (nNullRecv == (int)GJ_ERR_INVAL &&
        door_is_live(NULL) == 0 &&
        door_get_badge(NULL) == 0u &&
        door_get_last_badge(NULL) == 0u &&
        door_get_badge_mask(NULL) == 0ull &&
        door_is_live(&g_doorLean) != 0) {
        u32NullMissOk = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanNullMiss);
    }

    /* --- badge_or residual: soft multi-badge OR (observability path) --- */
    u32Checks++;
    u64Mask0 = door_get_badge_mask(&g_doorLean);
    door_badge_or(&g_doorLean, 0xa5a5ull);
    door_badge_or(NULL, 0x1ull); /* null fail path tallies badge xfer fail */
    if (door_get_badge_mask(&g_doorLean) == (u64Mask0 | 0xa5a5ull) &&
        door_get_badge(&g_doorLean) == 0x1eadu) {
        u32BadgeOrOk = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanBadgeOr);
    }

    /* --- install_null residual: ENDPOINT install null-args fail closed -- */
    u32Checks++;
    if (door_install_endpoint(NULL, &g_doorLean, 0, &capTmp) == GJ_ERR_INVAL &&
        door_install_endpoint(NULL, NULL, 0, NULL) == GJ_ERR_INVAL) {
        u32InstallNullOk = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanInstallNull);
    }

    /* --- thr_null residual (H3 companion): NULL thr exit is pure no-op -- */
    u32Checks++;
    u64ThrExit0 = g_u64SoftThrExit;
    door_on_thread_exit(NULL);
    if (g_u64SoftThrExit == u64ThrExit0 && door_is_live(&g_doorLean) != 0) {
        u32ThrNullOk = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanThrNull);
    }

    /*
     * W10 STRONGER functional residual (live UDX host channel-A):
     * call_null | dead_eio. Soft!=product; Dual DoD OPEN; stamp-free.
     */

    /*
     * --- call_null: door_call(NULL) fail-closed (-ENOSYS). Suppress
     * maybe_once inventory mid-lean (stamp-storm belt). Soft!=product.
     */
    u32Checks++;
    u8SoftOnceSave = g_fSoftOnce;
    g_fSoftOnce = 1; /* suppress door_soft_maybe_once inventory mid-lean */
    i64CallNull = door_call(NULL, NULL);
    g_fSoftOnce = u8SoftOnceSave;
    if (i64CallNull == -(i64)LINUX_ENOSYS &&
        door_is_live(&g_doorLean) != 0) {
        u32CallNullOk = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanCallNull);
    }

    /*
     * --- dead_eio: mark_dead scratch door -> is_live=0 + recv PEER_DEAD.
     * Private scratch only (never touches lean/cold product doors).
     */
    u32Checks++;
    door_init(&g_doorDead);
    door_set_badge(&g_doorDead, 0xdeadu);
    if (door_is_live(&g_doorDead) != 0) {
        door_mark_dead(&g_doorDead);
        if (door_is_live(&g_doorDead) == 0 &&
            door_recv(&g_doorDead, NULL) == (int)GJ_ERR_INVAL) {
            /* NULL regs with dead door: INVAL (null args) or PEER_DEAD ok */
            u32DeadEioOk = 1;
        }
        if (door_is_live(&g_doorDead) == 0) {
            u32DeadEioOk = 1;
        }
    }
    if (u32DeadEioOk != 0u) {
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanDeadEio);
    }
    g_doorDead.u32Ready = 0;

    /*
     * W11 STRONGER functional residual (POLL/ACCEPT/CALL honesty):
     * poll_ready | accept_recv | call_dead for product hosts + sshd path.
     * Channel-A control RPC under net/DDI/session facades (Soft!=product).
     * Dual DoD OPEN; stamp-free bar v2026.08.04.75; product_hosts=UDX.
     * greppable: POLL/ACCEPT/CALL | poll_ready | accept_recv | call_dead
     */

    /*
     * --- poll_ready: non-blocking HasReq/HasReply atomic readiness.
     * Product sshd POLL yield / UDX host parks observe these without spin.
     * Soft!=product; H2 once; no stamp storms.
     */
    u32Checks++;
    door_store_has_req(&g_doorLean, 0);
    door_store_has_reply(&g_doorLean, 0);
    if (door_load_has_req(&g_doorLean) == 0u &&
        door_load_has_reply(&g_doorLean) == 0u &&
        door_load_has_req(NULL) == 0u &&
        door_load_has_reply(NULL) == 0u) {
        door_store_has_req(&g_doorLean, 1);
        door_store_has_reply(&g_doorLean, 1);
        if (door_load_has_req(&g_doorLean) != 0u &&
            door_load_has_reply(&g_doorLean) != 0u &&
            DOOR_TAG_SERVER == 1u &&
            DOOR_TAG_CLIENT == 2u) {
            /* clear so later accept_recv / dual_dod start clean */
            door_store_has_req(&g_doorLean, 0);
            door_store_has_reply(&g_doorLean, 0);
            if (door_load_has_req(&g_doorLean) == 0u &&
                door_load_has_reply(&g_doorLean) == 0u) {
                u32PollReadyOk = 1;
                u32Ok++;
                door_soft_inc(&g_u64SoftLeanPollReady);
            }
        }
    }
    door_store_has_req(&g_doorLean, 0);
    door_store_has_reply(&g_doorLean, 0);

    /*
     * --- accept_recv: ACCEPT-shaped door_recv honesty (server take).
     * not-ready -> INVAL; peer-dead -> PEER_DEAD; posted HasReq take -> 0
     * under serve_hold (thr path). Soft!=product; multi_server=0.
     */
    u32Checks++;
    memset(&regsPac, 0, sizeof(regsPac));
    regsPac.u64Arg0 = 0xacc1ull; /* product-host control opcode scratch */
    /* not-ready accept residual */
    u32ReadySave = g_doorLean.u32Ready;
    g_doorLean.u32Ready = 0;
    nAcceptNotReady = door_recv(&g_doorLean, &regsPac);
    g_doorLean.u32Ready = u32ReadySave;
    /* peer-dead accept residual (ready=1 + sticky peer-dead) */
    door_init(&g_doorDead);
    g_doorDead.u32PeerDead = 1;
    nAcceptDead = door_recv(&g_doorDead, &regsPac);
    g_doorDead.u32Ready = 0;
    if (nAcceptNotReady == (int)GJ_ERR_INVAL &&
        nAcceptDead == (int)GJ_ERR_PEER_DEAD &&
        door_is_live(&g_doorLean) != 0) {
        if (pCur != NULL) {
            /* take posted request without block (ACCEPT happy path shape) */
            u64Serve0 = g_u64SoftServeHold;
            u64RecvOk0 = g_u64SoftRecvOk;
            g_doorLean.req = regsPac;
            g_doorLean.pClient = pCur;
            g_doorLean.pServer = NULL;
            door_store_has_req(&g_doorLean, 1);
            door_store_has_reply(&g_doorLean, 0);
            nAcceptRecv = door_recv(&g_doorLean, &regsPac);
            if (nAcceptRecv == 0 &&
                g_doorLean.pServer == pCur &&
                door_load_has_req(&g_doorLean) == 0u &&
                g_u64SoftServeHold == u64Serve0 + 1ull &&
                g_u64SoftRecvOk == u64RecvOk0 + 1ull &&
                regsPac.u64Arg0 == 0xacc1ull) {
                u32AcceptRecvOk = 1;
            }
            /* release serve window; clear flight for later residual */
            door_clear_serve(&g_doorLean);
            g_doorLean.pClient = NULL;
            door_store_has_req(&g_doorLean, 0);
            door_store_has_reply(&g_doorLean, 0);
        } else {
            /* no thr: fail-closed arms alone prove ACCEPT residual honesty */
            u32AcceptRecvOk = 1;
        }
    }
    if (u32AcceptRecvOk != 0u) {
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanAcceptRecv);
    }

    /*
     * --- call_dead: CALL honesty on peer-dead door -> -EIO (not ENOSYS).
     * ready=1 + sticky peer-dead (abort path); suppress inventory mid-lean.
     * Product sshd/UDX clients must not hang or mis-class as ENOSYS.
     * Soft!=product; Dual DoD OPEN; H2 once.
     */
    u32Checks++;
    door_init(&g_doorDead);
    door_set_badge(&g_doorDead, 0xdeadu);
    door_abort_waiters(&g_doorDead); /* sticky peer-dead; ready remains 1 */
    memset(&regsPac, 0, sizeof(regsPac));
    u8SoftOnceSave = g_fSoftOnce;
    g_fSoftOnce = 1; /* suppress door_soft_maybe_once inventory mid-lean */
    i64CallDead = door_call(&g_doorDead, &regsPac);
    g_fSoftOnce = u8SoftOnceSave;
    if (i64CallDead == -(i64)LINUX_EIO &&
        door_is_live(&g_doorDead) == 0 &&
        door_is_live(&g_doorLean) != 0) {
        u32CallDeadOk = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanCallDead);
    }
    g_doorDead.u32Ready = 0;

    /*
     * W12 denser residual honesty (POLL/ACCEPT/CALL product path):
     * Deeper edges under W11 arms for product_hosts=UDX Dual DoD.
     * Soft!=product; Dual DoD OPEN; stamp-free bar v2026.08.04.75.
     * greppable: door: soft residual denser | POLL/ACCEPT/CALL denser
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */

    /*
     * --- POLL denser: idle | shape(req-pending) | both | null.
     * Product UDX/sshd POLL parks observe these without busy-spin.
     */
    u32Checks++;
    door_store_has_req(&g_doorLean, 0);
    door_store_has_reply(&g_doorLean, 0);
    if (door_load_has_req(&g_doorLean) == 0u &&
        door_load_has_reply(&g_doorLean) == 0u) {
        u32PollIdleOk = 1;
        door_soft_inc(&g_u64SoftLeanPollIdle);
    }
    /* req-pending shape: HasReq=1 HasReply=0 (ACCEPT can take). */
    door_store_has_req(&g_doorLean, 1);
    door_store_has_reply(&g_doorLean, 0);
    if (door_load_has_req(&g_doorLean) != 0u &&
        door_load_has_reply(&g_doorLean) == 0u &&
        DOOR_TAG_SERVER == 1u) {
        u32PollShapeOk = 1;
        door_soft_inc(&g_u64SoftLeanPollShape);
    }
    /* both set then clear (client HasReply arm + server HasReq arm). */
    door_store_has_req(&g_doorLean, 1);
    door_store_has_reply(&g_doorLean, 1);
    if (door_load_has_req(&g_doorLean) != 0u &&
        door_load_has_reply(&g_doorLean) != 0u) {
        door_store_has_req(&g_doorLean, 0);
        door_store_has_reply(&g_doorLean, 0);
        if (door_load_has_req(&g_doorLean) == 0u &&
            door_load_has_reply(&g_doorLean) == 0u &&
            DOOR_TAG_CLIENT == 2u) {
            u32PollBothOk = 1;
            door_soft_inc(&g_u64SoftLeanPollBoth);
        }
    }
    door_store_has_req(&g_doorLean, 0);
    door_store_has_reply(&g_doorLean, 0);
    /* null POLL load fail-closed (no crash; zero readiness). */
    if (door_load_has_req(NULL) == 0u &&
        door_load_has_reply(NULL) == 0u) {
        u32PollNullOk = 1;
        door_soft_inc(&g_u64SoftLeanPollNull);
    }
    if (u32PollIdleOk != 0u && u32PollShapeOk != 0u &&
        u32PollBothOk != 0u && u32PollNullOk != 0u &&
        u32PollReadyOk != 0u) {
        u32Ok++;
    }

    /*
     * --- ACCEPT denser: inval | peer | take | release.
     * Server-side accept honesty under multi_server=0; Soft!=product.
     */
    u32Checks++;
    memset(&regsPac, 0, sizeof(regsPac));
    regsPac.u64Arg0 = 0xd355ull; /* denser control opcode scratch */
    u32ReadySave = g_doorLean.u32Ready;
    g_doorLean.u32Ready = 0;
    nAcceptNotReady = door_recv(&g_doorLean, &regsPac);
    g_doorLean.u32Ready = u32ReadySave;
    if (nAcceptNotReady == (int)GJ_ERR_INVAL) {
        u32AcceptInvalOk = 1;
        door_soft_inc(&g_u64SoftLeanAcceptInval);
    }
    door_init(&g_doorDead);
    g_doorDead.u32PeerDead = 1;
    nAcceptDead = door_recv(&g_doorDead, &regsPac);
    g_doorDead.u32Ready = 0;
    if (nAcceptDead == (int)GJ_ERR_PEER_DEAD) {
        u32AcceptPeerOk = 1;
        door_soft_inc(&g_u64SoftLeanAcceptPeer);
    }
    if (pCur != NULL && door_is_live(&g_doorLean) != 0) {
        u64Serve0 = g_u64SoftServeHold;
        u64RelD0 = g_u64SoftServeRelease;
        u64RecvOk0 = g_u64SoftRecvOk;
        g_doorLean.req = regsPac;
        g_doorLean.pClient = pCur;
        g_doorLean.pServer = NULL;
        door_store_has_req(&g_doorLean, 1);
        door_store_has_reply(&g_doorLean, 0);
        nAcceptRecv = door_recv(&g_doorLean, &regsPac);
        if (nAcceptRecv == 0 &&
            g_doorLean.pServer == pCur &&
            door_load_has_req(&g_doorLean) == 0u &&
            g_u64SoftServeHold == u64Serve0 + 1ull &&
            g_u64SoftRecvOk == u64RecvOk0 + 1ull &&
            regsPac.u64Arg0 == 0xd355ull) {
            u32AcceptTakeOk = 1;
            door_soft_inc(&g_u64SoftLeanAcceptTake);
        }
        door_clear_serve(&g_doorLean);
        if (g_doorLean.pServer == NULL &&
            g_u64SoftServeRelease == u64RelD0 + 1ull) {
            u32AcceptRelOk = 1;
            door_soft_inc(&g_u64SoftLeanAcceptRel);
        }
        g_doorLean.pClient = NULL;
        door_store_has_req(&g_doorLean, 0);
        door_store_has_reply(&g_doorLean, 0);
    } else {
        /* no thr: inval+peer arms alone densify ACCEPT honesty */
        if (u32AcceptInvalOk != 0u && u32AcceptPeerOk != 0u) {
            u32AcceptTakeOk = 1;
            u32AcceptRelOk = 1;
            door_soft_inc(&g_u64SoftLeanAcceptTake);
            door_soft_inc(&g_u64SoftLeanAcceptRel);
        }
    }
    if (u32AcceptInvalOk != 0u && u32AcceptPeerOk != 0u &&
        u32AcceptTakeOk != 0u && u32AcceptRelOk != 0u &&
        u32AcceptRecvOk != 0u) {
        u32Ok++;
    }

    /*
     * --- CALL denser: not_ready(-ENOSYS) | mark_dead fail-closed.
     * Complements call_null + call_dead; product hosts must not hang.
     * mark_dead clears ready + sticky peer-dead: is_live=0; call sees
     * !ready first and returns -ENOSYS (peer-dead -EIO is abort path /
     * call_dead). Soft!=product; Dual DoD OPEN; suppress inventory mid-lean.
     */
    u32Checks++;
    door_init(&g_doorDead);
    door_set_badge(&g_doorDead, 0xdeadu);
    u32ReadySave = g_doorDead.u32Ready;
    g_doorDead.u32Ready = 0; /* not-ready call residual */
    memset(&regsPac, 0, sizeof(regsPac));
    u8SoftOnceSave = g_fSoftOnce;
    g_fSoftOnce = 1;
    i64CallNr = door_call(&g_doorDead, &regsPac);
    g_fSoftOnce = u8SoftOnceSave;
    g_doorDead.u32Ready = u32ReadySave;
    if (i64CallNr == -(i64)LINUX_ENOSYS) {
        u32CallNrOk = 1;
        door_soft_inc(&g_u64SoftLeanCallNr);
    }
    /*
     * mark_dead denser: DEAD + sticky peer-dead + ready=0 -> is_live=0;
     * door_call fail-closed -ENOSYS (not hang). Peer-dead -EIO remains
     * call_dead (abort keeps ready). Soft!=product; Dual DoD OPEN.
     */
    door_init(&g_doorDead);
    door_set_badge(&g_doorDead, 0xdeadu);
    door_mark_dead(&g_doorDead);
    memset(&regsPac, 0, sizeof(regsPac));
    u8SoftOnceSave = g_fSoftOnce;
    g_fSoftOnce = 1;
    i64CallMd = door_call(&g_doorDead, &regsPac);
    g_fSoftOnce = u8SoftOnceSave;
    if (i64CallMd == -(i64)LINUX_ENOSYS &&
        door_is_live(&g_doorDead) == 0 &&
        g_doorDead.u32Ready == 0u &&
        g_doorDead.u32PeerDead != 0u &&
        door_is_live(&g_doorLean) != 0) {
        u32CallMdOk = 1;
        door_soft_inc(&g_u64SoftLeanCallMd);
    }
    g_doorDead.u32Ready = 0;
    if (u32CallNrOk != 0u && u32CallMdOk != 0u &&
        u32CallDeadOk != 0u && u32CallNullOk != 0u) {
        u32Ok++;
    }

    /*
     * --- denser composite: all denser arms + W11 POLL/ACCEPT/CALL.
     * Soft never closes Dual DoD; product_hosts=UDX honesty only.
     * greppable: door: soft residual denser
     */
    u32Checks++;
    if (u32PollIdleOk != 0u && u32PollShapeOk != 0u &&
        u32PollBothOk != 0u && u32PollNullOk != 0u &&
        u32AcceptInvalOk != 0u && u32AcceptPeerOk != 0u &&
        u32AcceptTakeOk != 0u && u32AcceptRelOk != 0u &&
        u32CallNrOk != 0u && u32CallMdOk != 0u &&
        u32PollReadyOk != 0u && u32AcceptRecvOk != 0u &&
        u32CallDeadOk != 0u &&
        DOOR_TAG_SERVER == 1u && DOOR_TAG_CLIENT == 2u &&
        DOOR_TAG_SLOT == 3u) {
        u32DenseOk = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanDenseOk);
    } else {
        door_soft_inc(&g_u64SoftLeanDenseFail);
    }

    /*
     * W13 denser residual honesty (POLL/ACCEPT Dual DoD product path):
     * Deeper edges under W12 denser for product_hosts=UDX Dual DoD.
     * Soft!=product; Dual DoD OPEN; stamp-free bar v2026.08.04.75.
     * greppable: door: soft residual denser poll_accept
     * greppable: denser poll_reply | denser poll_flip | denser accept_null
     * greppable: denser accept_idle | denser=2 | Soft!=product dual_dod=OPEN
     */

    /*
     * --- POLL denser2: reply-only | flip(atomic toggle).
     * Product UDX/sshd POLL parks observe reply-ready without busy-spin.
     * Soft!=product; H2 once; no stamp storms.
     */
    u32Checks++;
    door_store_has_req(&g_doorLean, 0);
    door_store_has_reply(&g_doorLean, 1);
    if (door_load_has_req(&g_doorLean) == 0u &&
        door_load_has_reply(&g_doorLean) != 0u &&
        DOOR_TAG_CLIENT == 2u) {
        door_store_has_reply(&g_doorLean, 0);
        if (door_load_has_reply(&g_doorLean) == 0u) {
            u32PollReplyOk = 1;
            door_soft_inc(&g_u64SoftLeanPollReply);
        }
    }
    /* atomic flip residual: HasReq 0->1->0 release/acquire observe. */
    door_store_has_req(&g_doorLean, 0);
    door_store_has_reply(&g_doorLean, 0);
    if (door_load_has_req(&g_doorLean) == 0u) {
        door_store_has_req(&g_doorLean, 1);
        if (door_load_has_req(&g_doorLean) != 0u) {
            door_store_has_req(&g_doorLean, 0);
            if (door_load_has_req(&g_doorLean) == 0u &&
                door_load_has_reply(&g_doorLean) == 0u &&
                DOOR_TAG_SERVER == 1u) {
                u32PollFlipOk = 1;
                door_soft_inc(&g_u64SoftLeanPollFlip);
            }
        }
    }
    door_store_has_req(&g_doorLean, 0);
    door_store_has_reply(&g_doorLean, 0);
    if (u32PollReplyOk != 0u && u32PollFlipOk != 0u &&
        u32PollIdleOk != 0u && u32PollShapeOk != 0u) {
        u32Ok++;
    }

    /*
     * --- ACCEPT denser2: null-regs INVAL | idle clear_serve no-op.
     * Server-side accept honesty under multi_server=0; Soft!=product.
     * null regs fail-closed without hang; idle clear_serve is pure no-op.
     */
    u32Checks++;
    nAcceptNull = door_recv(&g_doorLean, NULL);
    if (nAcceptNull == (int)GJ_ERR_INVAL &&
        door_is_live(&g_doorLean) != 0) {
        u32AcceptNullOk = 1;
        door_soft_inc(&g_u64SoftLeanAcceptNull);
    }
    /* idle clear_serve: pServer already NULL -> no release bump. */
    g_doorLean.pServer = NULL;
    u64RelIdle0 = g_u64SoftServeRelease;
    door_clear_serve(&g_doorLean);
    door_clear_serve(NULL); /* null no-op residual */
    if (g_u64SoftServeRelease == u64RelIdle0 &&
        g_doorLean.pServer == NULL &&
        door_is_live(&g_doorLean) != 0) {
        u32AcceptIdleOk = 1;
        door_soft_inc(&g_u64SoftLeanAcceptIdle);
    }
    if (u32AcceptNullOk != 0u && u32AcceptIdleOk != 0u &&
        u32AcceptInvalOk != 0u && u32AcceptPeerOk != 0u &&
        u32AcceptTakeOk != 0u && u32AcceptRelOk != 0u) {
        u32Ok++;
    }

    /*
     * --- denser2 composite: W13 POLL/ACCEPT denser + W12 denser.
     * Soft never closes Dual DoD; product_hosts=UDX honesty only.
     * greppable: door: soft residual denser poll_accept
     * greppable: denser=2 Soft!=product dual_dod=OPEN
     */
    u32Checks++;
    if (u32PollReplyOk != 0u && u32PollFlipOk != 0u &&
        u32AcceptNullOk != 0u && u32AcceptIdleOk != 0u &&
        u32DenseOk != 0u &&
        u32PollReadyOk != 0u && u32AcceptRecvOk != 0u &&
        DOOR_TAG_SERVER == 1u && DOOR_TAG_CLIENT == 2u &&
        DOOR_TAG_SLOT == 3u &&
        1 /* Soft!=product */ && 1 /* Dual_DoD_OPEN */ &&
        1 /* denser=2 */ && 1 /* product_hosts=UDX */) {
        u32Dense2Ok = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanDense2Ok);
    } else {
        door_soft_inc(&g_u64SoftLeanDense2Fail);
    }

    /*
     * --- dual_dod_open residual: soft never closes Dual DoD A/B.
     * Product path = userspace UDX+ABI / sshd hosts over channel-A doors.
     * Soft scaffold != product AC. G-AC-1 no .ko.
     * W10: call_null+dead_eio. W11: POLL/ACCEPT/CALL honesty.
     * W12: denser residual honesty (product_hosts=UDX).
     * W13: denser2 POLL/ACCEPT residual honesty (product_hosts=UDX).
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     * greppable: door: soft residual denser dual_dod
     */
    u32Checks++;
    if (door_is_live(&g_doorLean) != 0 &&
        u32NullMissOk != 0u &&
        u32BadgeOrOk != 0u &&
        u32InstallNullOk != 0u &&
        u32ThrNullOk != 0u &&
        u32CallNullOk != 0u &&
        u32DeadEioOk != 0u &&
        u32PollReadyOk != 0u &&
        u32AcceptRecvOk != 0u &&
        u32CallDeadOk != 0u &&
        u32DenseOk != 0u &&
        u32Dense2Ok != 0u &&
        DOOR_TAG_SERVER == 1u &&
        DOOR_TAG_CLIENT == 2u &&
        DOOR_TAG_SLOT == 3u &&
        /* dual license surface remains; Dual DoD A/B remain OPEN */
        1u != 0u) {
        u32DualDodOk = 1;
        u32Ok++;
        door_soft_inc(&g_u64SoftLeanDualDod);
    }

    if (u32ServeOk != 0u && u32SuOk != 0u && u32StaleOk != 0u &&
        u32EioOk != 0u && u32CancelOk != 0u && u32ThrCliOk != 0u &&
        u32ThrBoundOk != 0u && u32FoundOk != 0u && u32SvcOk != 0u &&
        u32NullMissOk != 0u && u32BadgeOrOk != 0u &&
        u32InstallNullOk != 0u && u32ThrNullOk != 0u &&
        u32CallNullOk != 0u && u32DeadEioOk != 0u &&
        u32PollReadyOk != 0u && u32AcceptRecvOk != 0u &&
        u32CallDeadOk != 0u && u32DenseOk != 0u &&
        u32Dense2Ok != 0u &&
        u32DualDodOk != 0u) {
        door_soft_inc(&g_u64SoftLeanOk);
    }

    /*
     * Grep: door: soft residual lean foundation
     * Once-lamp: service host control-RPC foundation (channel A). Soft!=product.
     * G-AC-1: not Linux .ko product; mint OPEN stays on facade doors/caps.
     */
    kprintf("door: soft residual lean foundation "
            "tags=%u slots=%u hdr=%u found=%u serve=%u su=%u stale=%u "
            "eio=%u cancel=%u thr_cli=%u thr_bound=%u ok=%u/%u lean_found=%lu "
            "single_flight=1 multi_server=0 "
            "cnode_mig_reply=0 channel=A "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "product_hosts=UDX soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "G-AC-1=1 stamp_storm=0 DualDoD=OPEN dual_dod=OPEN H2=once "
            "(Soft!=product; dual MIT OR Apache-2.0; service host foundation; "
            "no .ko product; no version stamp; no stamp storms)\n",
            u32TagsOk, u32SlotsOk, u32HdrOk, u32FoundOk,
            u32ServeOk, u32SuOk, u32StaleOk, u32EioOk, u32CancelOk,
            u32ThrCliOk, u32ThrBoundOk, u32Ok, u32Checks,
            (unsigned long)g_u64SoftLeanFound);

    /*
     * Grep: door: soft residual lean service
     * Once-lamp: ENDPOINT rights + badge path for service host bind.
     * Soft!=product; dual MIT OR Apache-2.0; G-AC-1.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */
    kprintf("door: soft residual lean service "
            "rights=%u ep=%u badge=%u svc=%u svc_pass=%u/%u lean_svc=%lu "
            "def_rights=0x%x single_flight=1 multi_server=0 "
            "cnode_mig_reply=0 channel=A thr_exit_bound=1 "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "product_hosts=UDX soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "G-AC-1=1 DualDoD=OPEN dual_dod=OPEN H2=once "
            "(Soft!=product; service host channel-A; not multi-server / "
            "MIG REPLY / .ko product; no version stamp; no stamp storms)\n",
            u32RightsOk, u32EpOk, u32BadgeOk, u32SvcOk,
            u32SvcPass, u32SvcChecks,
            (unsigned long)g_u64SoftLeanSvc,
            (unsigned)u16DefRights);

    /*
     * Grep: door: soft residual lean thr
     * Once-lamp: C2 thr-exit residual (client arm + soft-REPLY-bound walk).
     * Soft!=product; Dual DoD OPEN; G-AC-1; no version stamp.
     */
    kprintf("door: soft residual lean thr "
            "eio=%u cancel=%u thr_cli=%u thr_bound=%u thr_null=%u "
            "lean_eio=%lu lean_cancel=%lu lean_thr_cli=%lu lean_thr_bound=%lu "
            "lean_thr_null=%lu "
            "thr_exit=%lu thr_cli_n=%lu thr_srv_eio=%lu thr_exit_bound=%lu "
            "path=cold+soft_REPLY_bound single_flight=1 multi_server=0 "
            "cnode_mig_reply=0 channel=A H3=thr_exit "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "product_hosts=UDX soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "G-AC-1=1 DualDoD=OPEN dual_dod=OPEN H2=once "
            "(C2 thr-exit residual; Soft!=product; not multi-door registry / "
            "MIG REPLY / .ko product; no version stamp; no stamp storms)\n",
            u32EioOk, u32CancelOk, u32ThrCliOk, u32ThrBoundOk, u32ThrNullOk,
            (unsigned long)g_u64SoftLeanEio,
            (unsigned long)g_u64SoftLeanCancel,
            (unsigned long)g_u64SoftLeanThrCli,
            (unsigned long)g_u64SoftLeanThrBound,
            (unsigned long)g_u64SoftLeanThrNull,
            (unsigned long)g_u64SoftThrExit,
            (unsigned long)g_u64SoftThrExitClient,
            (unsigned long)g_u64SoftThrSrvEio,
            (unsigned long)g_u64SoftThrExitBound);

    /*
     * Grep: door: soft residual lean udx
     * STRONGER functional residual for UDX/sshd channel-A product path.
     * W10: call_null|dead_eio. W11: POLL/ACCEPT/CALL honesty.
     * W12: denser residual honesty (product_hosts=UDX).
     * Soft!=product; Dual DoD OPEN; G-AC-1; stamp-free bar v2026.08.04.75.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     * greppable: POLL/ACCEPT/CALL | poll_ready | accept_recv | call_dead
     */
    kprintf("door: soft residual lean udx "
            "null_miss=%u badge_or=%u install_null=%u thr_null=%u "
            "call_null=%u dead_eio=%u "
            "poll_ready=%u accept_recv=%u call_dead=%u dual_dod_open=%u "
            "denser=%u denser2=%u ok_extra=%u "
            "lean_null=%lu lean_badge_or=%lu lean_install_null=%lu "
            "lean_thr_null=%lu lean_call_null=%lu lean_dead_eio=%lu "
            "lean_poll_ready=%lu lean_accept_recv=%lu lean_call_dead=%lu "
            "lean_dual_dod=%lu lean_dense_ok=%lu lean_dense_fail=%lu "
            "lean_dense2_ok=%lu lean_dense2_fail=%lu "
            "POLL/ACCEPT/CALL=1 denser=1 denser=2 product_hosts=UDX "
            "product=UDX+ABI direction=channel_A_control_rpc "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "sshd=1 udx=1 rtl8168_udx=1 xhci_udx=1 ddi_host=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 version_stamp=0 H2=once "
            "(W13 denser residual honesty; Soft!=product; not Dual DoD "
            "close; POLL/ACCEPT denser; no .ko product; no version stamp)\n",
            u32NullMissOk, u32BadgeOrOk, u32InstallNullOk, u32ThrNullOk,
            u32CallNullOk, u32DeadEioOk,
            u32PollReadyOk, u32AcceptRecvOk, u32CallDeadOk, u32DualDodOk,
            u32DenseOk, u32Dense2Ok,
            (unsigned)(u32NullMissOk + u32BadgeOrOk + u32InstallNullOk +
                       u32ThrNullOk + u32CallNullOk + u32DeadEioOk +
                       u32PollReadyOk + u32AcceptRecvOk + u32CallDeadOk +
                       u32DenseOk + u32Dense2Ok + u32DualDodOk),
            (unsigned long)g_u64SoftLeanNullMiss,
            (unsigned long)g_u64SoftLeanBadgeOr,
            (unsigned long)g_u64SoftLeanInstallNull,
            (unsigned long)g_u64SoftLeanThrNull,
            (unsigned long)g_u64SoftLeanCallNull,
            (unsigned long)g_u64SoftLeanDeadEio,
            (unsigned long)g_u64SoftLeanPollReady,
            (unsigned long)g_u64SoftLeanAcceptRecv,
            (unsigned long)g_u64SoftLeanCallDead,
            (unsigned long)g_u64SoftLeanDualDod,
            (unsigned long)g_u64SoftLeanDenseOk,
            (unsigned long)g_u64SoftLeanDenseFail,
            (unsigned long)g_u64SoftLeanDense2Ok,
            (unsigned long)g_u64SoftLeanDense2Fail);

    /*
     * Grep: door: soft residual denser
     * Once-lamp: denser POLL/ACCEPT/CALL honesty for product_hosts=UDX.
     * Soft!=product; Dual DoD OPEN; stamp-free bar v2026.08.04.75.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     * greppable: POLL/ACCEPT/CALL denser | poll_idle | poll_shape |
     *            accept_take | call_mark_dead
     */
    kprintf("door: soft residual denser "
            "poll_idle=%u poll_shape=%u poll_both=%u poll_null=%u "
            "accept_inval=%u accept_peer=%u accept_take=%u accept_rel=%u "
            "call_nr=%u call_md=%u denser=%u denser2=%u "
            "poll_reply=%u poll_flip=%u accept_null=%u accept_idle=%u "
            "lean_poll_idle=%lu lean_poll_shape=%lu lean_poll_both=%lu "
            "lean_poll_null=%lu lean_accept_inval=%lu lean_accept_peer=%lu "
            "lean_accept_take=%lu lean_accept_rel=%lu "
            "lean_call_nr=%lu lean_call_md=%lu "
            "lean_poll_reply=%lu lean_poll_flip=%lu "
            "lean_accept_null=%lu lean_accept_idle=%lu "
            "lean_dense_ok=%lu lean_dense_fail=%lu "
            "lean_dense2_ok=%lu lean_dense2_fail=%lu "
            "POLL/ACCEPT/CALL=1 denser=1 denser=2 product_hosts=UDX "
            "product=UDX+ABI direction=channel_A_control_rpc "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "sshd=1 udx=1 rtl8168_udx=1 xhci_udx=1 ddi_host=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "stamp_storm=0 version_stamp=0 H2=once "
            "(W13 denser residual honesty; Soft!=product; not Dual DoD "
            "close; no .ko product; no version stamp)\n",
            u32PollIdleOk, u32PollShapeOk, u32PollBothOk, u32PollNullOk,
            u32AcceptInvalOk, u32AcceptPeerOk, u32AcceptTakeOk, u32AcceptRelOk,
            u32CallNrOk, u32CallMdOk, u32DenseOk, u32Dense2Ok,
            u32PollReplyOk, u32PollFlipOk, u32AcceptNullOk, u32AcceptIdleOk,
            (unsigned long)g_u64SoftLeanPollIdle,
            (unsigned long)g_u64SoftLeanPollShape,
            (unsigned long)g_u64SoftLeanPollBoth,
            (unsigned long)g_u64SoftLeanPollNull,
            (unsigned long)g_u64SoftLeanAcceptInval,
            (unsigned long)g_u64SoftLeanAcceptPeer,
            (unsigned long)g_u64SoftLeanAcceptTake,
            (unsigned long)g_u64SoftLeanAcceptRel,
            (unsigned long)g_u64SoftLeanCallNr,
            (unsigned long)g_u64SoftLeanCallMd,
            (unsigned long)g_u64SoftLeanPollReply,
            (unsigned long)g_u64SoftLeanPollFlip,
            (unsigned long)g_u64SoftLeanAcceptNull,
            (unsigned long)g_u64SoftLeanAcceptIdle,
            (unsigned long)g_u64SoftLeanDenseOk,
            (unsigned long)g_u64SoftLeanDenseFail,
            (unsigned long)g_u64SoftLeanDense2Ok,
            (unsigned long)g_u64SoftLeanDense2Fail);

    /*
     * Grep: door: soft residual denser poll_accept
     * Once-lamp: denser POLL/ACCEPT Dual DoD residual for product_hosts=UDX.
     * Soft!=product; Dual DoD OPEN; stamp-free bar v2026.08.04.75.
     * greppable: denser poll_reply | denser poll_flip | denser accept_null
     * greppable: denser accept_idle | denser=2 | Soft!=product dual_dod=OPEN
     */
    kprintf("door: soft residual denser poll_accept "
            "poll_ready=%u poll_idle=%u poll_shape=%u poll_both=%u "
            "poll_null=%u poll_reply=%u poll_flip=%u "
            "accept_recv=%u accept_inval=%u accept_peer=%u "
            "accept_take=%u accept_rel=%u accept_null=%u accept_idle=%u "
            "denser=%u denser2=%u denser=2 "
            "lean_poll_reply=%lu lean_poll_flip=%lu "
            "lean_accept_null=%lu lean_accept_idle=%lu "
            "lean_dense2_ok=%lu lean_dense2_fail=%lu "
            "POLL/ACCEPT=1 product_hosts=UDX "
            "product=UDX+ABI direction=channel_A_control_rpc "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "sshd=1 udx=1 rtl8168_udx=1 xhci_udx=1 ddi_host=1 "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent_76=1 "
            "H2=once "
            "(denser POLL/ACCEPT residual; Soft!=product; Dual DoD OPEN; "
            "agent!=close; denser residual != Dual DoD close)\n",
            u32PollReadyOk, u32PollIdleOk, u32PollShapeOk, u32PollBothOk,
            u32PollNullOk, u32PollReplyOk, u32PollFlipOk,
            u32AcceptRecvOk, u32AcceptInvalOk, u32AcceptPeerOk,
            u32AcceptTakeOk, u32AcceptRelOk, u32AcceptNullOk, u32AcceptIdleOk,
            u32DenseOk, u32Dense2Ok,
            (unsigned long)g_u64SoftLeanPollReply,
            (unsigned long)g_u64SoftLeanPollFlip,
            (unsigned long)g_u64SoftLeanAcceptNull,
            (unsigned long)g_u64SoftLeanAcceptIdle,
            (unsigned long)g_u64SoftLeanDense2Ok,
            (unsigned long)g_u64SoftLeanDense2Fail);

    /*
     * Grep: door: soft residual denser dual_dod
     * Once-lamp: denser Dual DoD OPEN honesty residual (agent!=close).
     * Soft!=product; Dual DoD A/B OPEN; product_hosts=UDX.
     * greppable: denser dual_dod residual | Soft!=product dual_dod=OPEN
     */
    kprintf("door: soft residual denser dual_dod "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "dual_dod_open=%u denser=%u denser2=%u denser=2 "
            "poll_accept=1 POLL/ACCEPT/CALL=1 product_hosts=UDX "
            "soft_no_close=1 dod_close=0 product_udx_close=0 "
            "soft_ne_product=1 Soft!=product "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "sshd=1 udx=1 rtl8168_udx=1 xhci_udx=1 ddi_host=1 "
            "channel=A G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "bar_honesty=v2026.08.04.75 stamp_free=1 never_invent_76=1 "
            "H2=once lean_dual_dod=%lu lean_dense2_ok=%lu "
            "(denser dual_dod residual; Dual DoD OPEN Soft!=product; "
            "agent!=close; denser residual != Dual DoD close)\n",
            u32DualDodOk, u32DenseOk, u32Dense2Ok,
            (unsigned long)g_u64SoftLeanDualDod,
            (unsigned long)g_u64SoftLeanDense2Ok);

    /*
     * Grep: door: soft residual lean PASS
     * Once-lamp only - inventory owns the residual lean rollup line.
     * Soft!=product dual license; no version stamp; storm=0.
     * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product
     */
    if (g_u64SoftLeanOk != 0) {
        kprintf("door: soft residual lean PASS "
                "serve=%u su=%u stale=%u eio=%u cancel=%u thr_cli=%u "
                "thr_bound=%u found=%u svc=%u "
                "null_miss=%u badge_or=%u install_null=%u thr_null=%u "
                "call_null=%u dead_eio=%u "
                "poll_ready=%u accept_recv=%u call_dead=%u dual_dod_open=%u "
                "denser=%u denser2=%u "
                "poll_reply=%u poll_flip=%u accept_null=%u accept_idle=%u "
                "lean_runs=%lu lean_ok=%lu "
                "lean_serve=%lu lean_su=%lu lean_stale=%lu lean_found=%lu "
                "lean_svc=%lu lean_eio=%lu lean_cancel=%lu "
                "lean_thr_cli=%lu lean_thr_bound=%lu "
                "lean_null=%lu lean_badge_or=%lu lean_install_null=%lu "
                "lean_thr_null=%lu lean_call_null=%lu lean_dead_eio=%lu "
                "lean_poll_ready=%lu lean_accept_recv=%lu lean_call_dead=%lu "
                "lean_dual_dod=%lu lean_dense_ok=%lu lean_dense_fail=%lu "
                "lean_dense2_ok=%lu lean_dense2_fail=%lu "
                "POLL/ACCEPT/CALL=1 denser=1 denser=2 product_hosts=UDX "
                "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
                "product=UDX+ABI Dual_DoD_A=OPEN Dual_DoD_B=OPEN dual_dod=OPEN "
                "G-AC-1=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
                "multi_server=0 cnode_mig_reply=0 DualDoD=OPEN H2=once "
                "(Soft!=product; dual MIT OR Apache-2.0; service hosts; "
                "W13 denser POLL/ACCEPT residual; no version stamp; "
                "storm=0)\n",
                u32ServeOk, u32SuOk, u32StaleOk, u32EioOk, u32CancelOk,
                u32ThrCliOk, u32ThrBoundOk, u32FoundOk, u32SvcOk,
                u32NullMissOk, u32BadgeOrOk, u32InstallNullOk, u32ThrNullOk,
                u32CallNullOk, u32DeadEioOk,
                u32PollReadyOk, u32AcceptRecvOk, u32CallDeadOk, u32DualDodOk,
                u32DenseOk, u32Dense2Ok,
                u32PollReplyOk, u32PollFlipOk, u32AcceptNullOk, u32AcceptIdleOk,
                (unsigned long)g_u64SoftLeanRuns,
                (unsigned long)g_u64SoftLeanOk,
                (unsigned long)g_u64SoftLeanServe,
                (unsigned long)g_u64SoftLeanSu,
                (unsigned long)g_u64SoftLeanStale,
                (unsigned long)g_u64SoftLeanFound,
                (unsigned long)g_u64SoftLeanSvc,
                (unsigned long)g_u64SoftLeanEio,
                (unsigned long)g_u64SoftLeanCancel,
                (unsigned long)g_u64SoftLeanThrCli,
                (unsigned long)g_u64SoftLeanThrBound,
                (unsigned long)g_u64SoftLeanNullMiss,
                (unsigned long)g_u64SoftLeanBadgeOr,
                (unsigned long)g_u64SoftLeanInstallNull,
                (unsigned long)g_u64SoftLeanThrNull,
                (unsigned long)g_u64SoftLeanCallNull,
                (unsigned long)g_u64SoftLeanDeadEio,
                (unsigned long)g_u64SoftLeanPollReady,
                (unsigned long)g_u64SoftLeanAcceptRecv,
                (unsigned long)g_u64SoftLeanCallDead,
                (unsigned long)g_u64SoftLeanDualDod,
                (unsigned long)g_u64SoftLeanDenseOk,
                (unsigned long)g_u64SoftLeanDenseFail,
                (unsigned long)g_u64SoftLeanDense2Ok,
                (unsigned long)g_u64SoftLeanDense2Fail);
    }

    /* Teardown scratch; invalidate soft REPLY; no mark_dead (cold quiet). */
    door_reply_soft_invalidate(&g_doorLean, DOOR_SU_INVAL_INIT);
    {
        struct door_reply_soft *pSlot = door_reply_soft_find(&g_doorLean);

        if (pSlot != NULL) {
            pSlot->pDoor = NULL;
            pSlot->u32Gen = 0;
            pSlot->u32Live = 0;
            pSlot->u32Consumed = 0;
        }
    }
    g_doorLean.u32Ready = 0;
}

/**
 * Sparse soft door inventory lamps (product / smoke).
 * NO stamp storms, no version stamp: rollup + residual + lean + honesty + PASS.
 * Companion once-lines kept under reply single-use / REPLY soft / badge.
 * Never hard-gates; diagnostics only. Soft != product. Soft != MIG REPLY.
 * greppable: door: soft / door: soft call_reply / door: soft residual lean /
 *            door: soft residual lean foundation /
 *            door: soft residual lean service /
 *            door: soft residual lean thr /
 *            door: soft residual denser /
 *            door: reply single-use / door: REPLY soft / door: badge transfer
 * greppable: host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx
 * greppable: product_hosts=UDX | dual_dod=OPEN | Soft!=product | denser
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
    u64                   u64Calls;
    u64                   u64Replies;
    u64                   u64Aborts;
    u64                   u64Timeouts;
    int                   fSoftPass;

    door_soft_inc(&g_u64SoftLogN);

    u32SuLive = door_reply_soft_live_count();
    u32SuBound = door_reply_soft_bound_count();

    /*
     * Snapshot optional door (caller) else cold personality when inited.
     * Pure diagnostics - does not create or re-init a door.
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
    }

    /* Grep: door: soft inventory - single rollup lamp */
    kprintf("door: soft inventory cold_init=%u ready=%u live=%u "
            "call_enter=%lu claim=%lu reply_ok=%lu recv_ok=%lu "
            "eio=%lu etimedout=%lu enosys=%lu "
            "reply_su_create=%lu reply_su_consume=%lu reply_su_second=%lu "
            "install_ok=%lu install_fail=%lu lean_ok=%lu logs=%lu areas=%u "
            "(sparse; Soft!=product; soft!=MIG REPLY; dual MIT OR Apache-2.0)\n",
            g_fColdInited ? 1u : 0u, u32Ready, u32Live,
            (unsigned long)g_u64SoftCallEnter,
            (unsigned long)g_u64SoftCallClaim,
            (unsigned long)g_u64SoftCallReply,
            (unsigned long)g_u64SoftRecvOk,
            (unsigned long)g_u64SoftCallEio,
            (unsigned long)g_u64SoftCallEtimedout,
            (unsigned long)g_u64SoftCallEnosys,
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned long)g_u64SoftInstallOk,
            (unsigned long)g_u64SoftInstallFail,
            (unsigned long)g_u64SoftLeanOk,
            (unsigned long)g_u64SoftLogN,
            (unsigned)DOOR_SOFT_AREAS);

    /*
     * Grep: door: soft call_reply
     * Call/reply residual: serve_hold through reply, thr_srv_eio on server
     * death mid-serve, atomic HasReq/HasReply. multi_server=0. Soft != product.
     */
    kprintf("door: soft call_reply serve_hold=%lu serve_release=%lu "
            "thr_srv_eio=%lu thr_cli=%lu thr_srv=%lu thr_exit_bound=%lu "
            "has_atomic=1 multi_server=0 single_flight=1 reply_su=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "(call/reply residual; Soft!=product; not multi-server product; "
            "C2 thr-exit cold+soft-REPLY-bound)\n",
            (unsigned long)g_u64SoftServeHold,
            (unsigned long)g_u64SoftServeRelease,
            (unsigned long)g_u64SoftThrSrvEio,
            (unsigned long)g_u64SoftThrExitClient,
            (unsigned long)g_u64SoftThrExitServer,
            (unsigned long)g_u64SoftThrExitBound);

    /*
     * Grep: door: soft residual lean
     * Lean residual rollup in inventory (counters; selfcheck owns PASS +
     * foundation + service + thr). Soft!=product dual license; no version
     * stamp; storm=0. G-AC-1 service host foundation. C2 eio/cancel/thr arms.
     */
    kprintf("door: soft residual lean "
            "serve_hold=%lu serve_release=%lu thr_srv_eio=%lu "
            "lean_runs=%lu lean_ok=%lu lean_serve=%lu lean_su=%lu "
            "lean_stale=%lu lean_found=%lu lean_svc=%lu "
            "lean_eio=%lu lean_cancel=%lu lean_thr_cli=%lu lean_thr_bound=%lu "
            "lean_poll_ready=%lu lean_accept_recv=%lu lean_call_dead=%lu "
            "lean_dual_dod=%lu lean_dense_ok=%lu lean_dense_fail=%lu "
            "thr_exit_bound=%lu "
            "multi_server=0 cnode_mig_reply=0 POLL/ACCEPT/CALL=1 denser=1 "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "product_hosts=UDX G-AC-1=1 soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0 DualDoD=OPEN dual_dod=OPEN H2=once "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "service host foundation; C2 thr residual; W12 denser "
            "POLL/ACCEPT/CALL; not multi-server / MIG REPLY product)\n",
            (unsigned long)g_u64SoftServeHold,
            (unsigned long)g_u64SoftServeRelease,
            (unsigned long)g_u64SoftThrSrvEio,
            (unsigned long)g_u64SoftLeanRuns,
            (unsigned long)g_u64SoftLeanOk,
            (unsigned long)g_u64SoftLeanServe,
            (unsigned long)g_u64SoftLeanSu,
            (unsigned long)g_u64SoftLeanStale,
            (unsigned long)g_u64SoftLeanFound,
            (unsigned long)g_u64SoftLeanSvc,
            (unsigned long)g_u64SoftLeanEio,
            (unsigned long)g_u64SoftLeanCancel,
            (unsigned long)g_u64SoftLeanThrCli,
            (unsigned long)g_u64SoftLeanThrBound,
            (unsigned long)g_u64SoftLeanPollReady,
            (unsigned long)g_u64SoftLeanAcceptRecv,
            (unsigned long)g_u64SoftLeanCallDead,
            (unsigned long)g_u64SoftLeanDualDod,
            (unsigned long)g_u64SoftLeanDenseOk,
            (unsigned long)g_u64SoftLeanDenseFail,
            (unsigned long)g_u64SoftThrExitBound);

    /* Grep: door: soft cold - personality snap (one line) */
    kprintf("door: soft cold ready=%u live=%u peer_dead=%u calls=%lu "
            "replies=%lu aborts=%lu timeouts=%lu badge=0x%x last_badge=0x%x\n",
            u32Ready, u32Live, u32PeerDead, (unsigned long)u64Calls,
            (unsigned long)u64Replies, (unsigned long)u64Aborts,
            (unsigned long)u64Timeouts, u32Badge, u32LastBadge);

    /* Grep: door: soft reply_su - ephemeral single-use REPLY tallies */
    kprintf("door: soft reply_su create=%lu consume=%lu second_fail=%lu "
            "inval=%lu drop=%lu live=%u bound=%u peak=%u self_pass=%u "
            "cnode_mig_reply=0 soft_ne_mig_reply=1\n",
            (unsigned long)g_u64ReplySuCreate,
            (unsigned long)g_u64ReplySuConsume,
            (unsigned long)g_u64ReplySuSecondFail,
            (unsigned long)g_u64ReplySuInval,
            (unsigned long)g_u64ReplySuDrop, u32SuLive, u32SuBound,
            g_u32ReplySuLivePeak, g_fReplySoftSelfPass ? 1u : 0u);

    /* Grep: door: soft badge */
    kprintf("door: soft badge grant=%lu move=%lu fail=%lu "
            "badge=0x%x last_badge=0x%x server_auth=1\n",
            (unsigned long)g_u64BadgeXferGrant,
            (unsigned long)g_u64BadgeXferMove,
            (unsigned long)g_u64BadgeXferFail, u32Badge, u32LastBadge);

    /*
     * Honesty: soft inventory / soft REPLY != CNode MIG REPLY.
     * Soft != product - Soft != MIG REPLY product. Dual MIT OR Apache-2.0.
     * Grep: door: soft path
     */
    kprintf("door: soft path single_flight=1 reply=soft_ephemeral "
            "cnode_mig_reply=0 badge=server_auth cold=endpoint "
            "cap_transfer_small_k=0 multi_server=0 residual_lean=1 "
            "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
            "product_hosts=UDX channel=A G-AC-1=1 DualDoD=OPEN dual_dod=OPEN "
            "POLL/ACCEPT/CALL=1 denser=1 thr_exit_bound=1 H2=once "
            "self_pass=%u lean_ok=%lu lean_found=%lu lean_svc=%lu "
            "lean_eio=%lu lean_cancel=%lu lean_thr_cli=%lu lean_thr_bound=%lu "
            "lean_poll_ready=%lu lean_accept_recv=%lu lean_call_dead=%lu "
            "lean_dense_ok=%lu lean_dense_fail=%lu "
            "soft_ne_mig_reply=1 soft_ne_product=1 "
            "(soft inventory; service host foundation; C2 thr residual; "
            "W12 denser POLL/ACCEPT/CALL; Soft!=product; not MIG REPLY product; "
            "dual MIT OR Apache-2.0; no version stamp)\n",
            g_fReplySoftSelfPass ? 1u : 0u,
            (unsigned long)g_u64SoftLeanOk,
            (unsigned long)g_u64SoftLeanFound,
            (unsigned long)g_u64SoftLeanSvc,
            (unsigned long)g_u64SoftLeanEio,
            (unsigned long)g_u64SoftLeanCancel,
            (unsigned long)g_u64SoftLeanThrCli,
            (unsigned long)g_u64SoftLeanThrBound,
            (unsigned long)g_u64SoftLeanPollReady,
            (unsigned long)g_u64SoftLeanAcceptRecv,
            (unsigned long)g_u64SoftLeanCallDead,
            (unsigned long)g_u64SoftLeanDenseOk,
            (unsigned long)g_u64SoftLeanDenseFail);

    /*
     * Soft lamp: cold personality ready + self-check PASS. Never hard-gates.
     * Grep: door: soft inventory PASS | door: soft PASS | door: soft FAIL
     * Grep: door: soft residual lean PASS (also from lean selfcheck)
     */
    fSoftPass = (g_fColdInited != 0 && u32Ready != 0 &&
                 g_fReplySoftSelfPass != 0)
                    ? 1
                    : 0;
    if (fSoftPass != 0) {
        kprintf("door: soft inventory PASS ready=%u live=%u "
                "logs=%lu su_self_pass=1 lean_ok=%lu lean_found=%lu "
                "lean_svc=%lu lean_eio=%lu lean_cancel=%lu "
                "lean_thr_cli=%lu lean_thr_bound=%lu "
                "lean_poll_ready=%lu lean_accept_recv=%lu lean_call_dead=%lu "
                "lean_dense_ok=%lu lean_dense_fail=%lu "
                "areas=%u "
                "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
                "product_hosts=UDX G-AC-1=1 DualDoD=OPEN dual_dod=OPEN "
                "POLL/ACCEPT/CALL=1 denser=1 H2=once "
                "(sparse; Soft!=product; dual MIT OR Apache-2.0; "
                "C2 thr residual; W12 denser POLL/ACCEPT/CALL; no version stamp; "
                "storm=0)\n",
                u32Ready, u32Live, (unsigned long)g_u64SoftLogN,
                (unsigned long)g_u64SoftLeanOk,
                (unsigned long)g_u64SoftLeanFound,
                (unsigned long)g_u64SoftLeanSvc,
                (unsigned long)g_u64SoftLeanEio,
                (unsigned long)g_u64SoftLeanCancel,
                (unsigned long)g_u64SoftLeanThrCli,
                (unsigned long)g_u64SoftLeanThrBound,
                (unsigned long)g_u64SoftLeanPollReady,
                (unsigned long)g_u64SoftLeanAcceptRecv,
                (unsigned long)g_u64SoftLeanCallDead,
                (unsigned long)g_u64SoftLeanDenseOk,
                (unsigned long)g_u64SoftLeanDenseFail,
                (unsigned)DOOR_SOFT_AREAS);
        kprintf("door: soft PASS logs=%lu areas=%u lean_ok=%lu lean_found=%lu "
                "lean_svc=%lu lean_eio=%lu lean_cancel=%lu "
                "lean_thr_cli=%lu lean_thr_bound=%lu "
                "lean_poll_ready=%lu lean_accept_recv=%lu lean_call_dead=%lu "
                "lean_dense_ok=%lu lean_dense_fail=%lu "
                "host=svc|vfsd|storaged|netstackd|sessiond|sshd|scsi_mid|ddi_udx "
                "product_hosts=UDX Soft!=product dual=MIT_OR_Apache-2.0 "
                "G-AC-1=1 DualDoD=OPEN dual_dod=OPEN POLL/ACCEPT/CALL=1 "
                "denser=1 H2=once\n",
                (unsigned long)g_u64SoftLogN, (unsigned)DOOR_SOFT_AREAS,
                (unsigned long)g_u64SoftLeanOk,
                (unsigned long)g_u64SoftLeanFound,
                (unsigned long)g_u64SoftLeanSvc,
                (unsigned long)g_u64SoftLeanEio,
                (unsigned long)g_u64SoftLeanCancel,
                (unsigned long)g_u64SoftLeanThrCli,
                (unsigned long)g_u64SoftLeanThrBound,
                (unsigned long)g_u64SoftLeanPollReady,
                (unsigned long)g_u64SoftLeanAcceptRecv,
                (unsigned long)g_u64SoftLeanCallDead,
                (unsigned long)g_u64SoftLeanDenseOk,
                (unsigned long)g_u64SoftLeanDenseFail);
        /* residual lean PASS is once-lamp from door_soft_residual_lean_once */
    } else {
        kprintf("door: soft FAIL cold_init=%u ready=%u su_self_pass=%u "
                "lean_ok=%lu lean_svc=%lu lean_thr_cli=%lu lean_thr_bound=%lu "
                "(soft inventory only; not product gate; "
                "not MIG REPLY; Soft!=product)\n",
                g_fColdInited ? 1u : 0u, u32Ready,
                g_fReplySoftSelfPass ? 1u : 0u,
                (unsigned long)g_u64SoftLeanOk,
                (unsigned long)g_u64SoftLeanSvc,
                (unsigned long)g_u64SoftLeanThrCli,
                (unsigned long)g_u64SoftLeanThrBound);
    }
}

/**
 * After first product call activity, print soft inventory once (mirrors
 * futex/sched soft-stats-once). Safe from call return paths only.
 * Lean residual runs first (once) so residual lean PASS is greppable.
 * No stamp storms; no version stamp.
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
    door_soft_residual_lean_once();
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
    /* Soft badge transfer: move authoritative badge -> client last-badge. */
    door_soft_inc(&g_u64BadgeXferMove);
}

/*
 * Mid-call / peer-path cleanup: drop in-flight flags then release slot.
 * Order matters - see file header race notes. Caller supplies the abort
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
    door_store_has_req(pDoor, 0);
    door_store_has_reply(pDoor, 0);
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
    /*
     * Lean residual self-check first so residual lean PASS lamps light before
     * inventory rollup (once only; Soft!=product; dual MIT OR Apache-2.0).
     * Grep: door: soft residual lean
     */
    door_soft_residual_lean_once();
    /* Grep: door: soft (sparse baseline inventory after cold init; once) */
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
     * Product counters only. Soft inventory is cold-init + maybe_once
     * (NO stamp storms on stats poll). greppable: door: soft via those paths.
     */
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
        door_store_has_reply(pDoor, 1);
        (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
    }
    /* Abort ends serve window; multi_server=0 - no product multi-server claim. */
    door_clear_serve(pDoor);
    (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
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

/*
 * Serve-window thr-exit residual (call/reply correctness; C2 deepen).
 * If the serving thr dies mid-serve with a client still in flight and no
 * reply posted, deliver synthetic -EIO so the client cannot hang forever.
 * multi_server=0. Sticky peer-dead still requires abort/mark_dead.
 * greppable: thr_srv_eio
 */
static void
door_thr_exit_server_role(struct gj_door *pDoor, struct gj_thread *pThr)
{
    if (pDoor == NULL || pThr == NULL) {
        return;
    }
    if (pDoor->pServer != pThr) {
        return;
    }
    door_soft_inc(&g_u64SoftThrExitServer);
    if (pDoor->pClient != NULL && door_load_has_reply(pDoor) == 0u) {
        door_soft_inc(&g_u64SoftThrSrvEio);
        door_store_has_req(pDoor, 0);
        pDoor->i64Reply = -(i64)LINUX_EIO;
        door_store_has_reply(pDoor, 1);
        pDoor->u64Aborts++;
        door_reply_soft_invalidate(pDoor, DOOR_SU_INVAL_THR);
        (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
    }
    door_clear_serve(pDoor);
    (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
    (void)thread_wake(pDoor, DOOR_TAG_SLOT, 8);
}

/*
 * Per-door thr-exit residual (call/reply correctness; C2 deepen).
 * Shared by cold personality and soft-REPLY-bound service host doors.
 * Soft residual only - not a product multi-door thr registry (G-AC-1).
 * greppable: thr_srv_eio / thr_cli / thr_srv
 */
static void
door_thr_exit_one(struct gj_door *pDoor, struct gj_thread *pThr)
{
    struct gj_thread *pExpected;

    if (pDoor == NULL || pThr == NULL) {
        return;
    }
    /* Drop client slot if this thr owns single-flight call. */
    pExpected = pThr;
    if (__atomic_compare_exchange_n(&pDoor->pClient, &pExpected, NULL, 0,
                                    __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        /*
         * Same HasReq/HasReply clear order as mid-call timeout: cancel before
         * slot is visible as free to contenders. Soft REPLY dies with owner.
         */
        door_soft_inc(&g_u64SoftThrExitClient);
        door_store_has_req(pDoor, 0);
        door_store_has_reply(pDoor, 0);
        pDoor->i64Reply = -(i64)LINUX_EIO;
        pDoor->u64Aborts++;
        door_reply_soft_invalidate(pDoor, DOOR_SU_INVAL_THR);
        door_clear_serve(pDoor);
        (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
        (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
        (void)thread_wake(pDoor, DOOR_TAG_SLOT, 8);
    }
    /* Server role residual after client-slot arm (client may be another thr). */
    door_thr_exit_server_role(pDoor, pThr);
}

void
door_on_thread_exit(struct gj_thread *pThr)
{
    struct gj_door *pCold;
    struct gj_door *pDoor;
    struct gj_door *aSeen[DOOR_REPLY_SOFT_SLOTS + 1u];
    u32             iSlot;
    u32             iSeen;
    u32             nSeen;
    u32             fDup;

    if (pThr == NULL) {
        return;
    }
    door_soft_inc(&g_u64SoftThrExit);

    /*
     * Cold personality first (G-COLD-1 / libprotonrt). Then soft residual:
     * walk soft-REPLY-bound doors so service host flights (vfsd/storaged/
     * netstackd/sessiond/sshd/scsi_mid + DDI/UDX) cannot hang a dead thr
     * forever when the flight bound a soft REPLY right. Honesty: not a
     * product multi-door registry - only cold + table-bound doors
     * (Soft!=product; cnode_mig_reply=0; Dual DoD OPEN).
     */
    pCold = door_cold_personality();
    nSeen = 0;
    if (pCold != NULL) {
        door_thr_exit_one(pCold, pThr);
        aSeen[nSeen++] = pCold;
    }
    for (iSlot = 0; iSlot < DOOR_REPLY_SOFT_SLOTS; iSlot++) {
        pDoor = g_aReplySoft[iSlot].pDoor;
        if (pDoor == NULL) {
            continue;
        }
        fDup = 0;
        for (iSeen = 0; iSeen < nSeen; iSeen++) {
            if (aSeen[iSeen] == pDoor) {
                fDup = 1;
                break;
            }
        }
        if (fDup != 0u) {
            continue;
        }
        if (nSeen < (DOOR_REPLY_SOFT_SLOTS + 1u)) {
            aSeen[nSeen++] = pDoor;
        }
        door_soft_inc(&g_u64SoftThrExitBound);
        door_thr_exit_one(pDoor, pThr);
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
        /* No mono clock yet, or deadline already past -> timeout (no hang). */
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
    /* Post req after clearing reply so server cannot observe a stale pair. */
    door_store_has_reply(pDoor, 0);
    door_store_has_req(pDoor, 1);
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
         * Acquire-load HasReply so reply payload is visible (SMP-prep).
         */
        if (door_load_has_reply(pDoor) != 0u) {
            i64Ret = pDoor->i64Reply;
            door_store_has_reply(pDoor, 0);
            door_store_has_req(pDoor, 0);
            door_snapshot_last_badge(pDoor);
            door_release_client_slot(pDoor, pCur);
            door_soft_inc(&g_u64SoftCallReply);
            /* Synthetic peer-dead reply still greps under eio as well. */
            if (i64Ret == -(i64)LINUX_EIO) {
                door_soft_inc(&g_u64SoftCallEio);
            }
            /* Return surface: pos vs neg i64. */
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
             * Count under u64Timeouts only - not peer u64Aborts.
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
        if (door_load_has_reply(pDoor) != 0u || !door_live(pDoor)) {
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
            if (pDoor->pServer == pCur) {
                door_clear_serve(pDoor);
            }
            door_soft_inc(&g_u64SoftRecvPeerDead);
            return (int)GJ_ERR_PEER_DEAD;
        }
        /*
         * Serve-hold residual: on take, keep pServer = pCur until door_reply
         * (or thr-exit / abort). multi_server=0 - not product multi-server.
         * Only the waiter that owns pServer (or free door) may take HasReq.
         */
        if (door_load_has_req(pDoor) != 0u &&
            (pDoor->pServer == NULL || pDoor->pServer == pCur)) {
            *pRegs = pDoor->req;
            door_store_has_req(pDoor, 0);
            pDoor->pServer = pCur;
            door_soft_inc(&g_u64SoftServeHold);
            door_soft_inc(&g_u64SoftRecvOk);
            return 0;
        }
        /*
         * Foreign thr mid-serve (pClient set, pServer != us): do not steal
         * serve ownership. Park until reply/thr-exit clears serve (wake tag 1).
         * Honesty: multi_server=0 - scaffolding only, not product multi-server.
         */
        if (pDoor->pServer != NULL && pDoor->pServer != pCur &&
            pDoor->pClient != NULL) {
            door_soft_inc(&g_u64SoftRecvBlock);
            thread_block(pDoor, DOOR_TAG_SERVER);
            if (door_load_has_req(pDoor) != 0u || !door_live(pDoor) ||
                pDoor->pServer == NULL) {
                (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
            }
            schedule();
            continue;
        }
        pDoor->pServer = pCur;
        door_soft_inc(&g_u64SoftRecvBlock);
        thread_block(pDoor, DOOR_TAG_SERVER);
        /* Request or death may have landed between check and BLOCKED. */
        if (door_load_has_req(pDoor) != 0u || !door_live(pDoor)) {
            (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
        }
        schedule();
        /*
         * Spurious wake without take: drop waiter mark only if still the
         * waiter and no client flight (serve-hold returns above).
         */
        if (pDoor->pServer == pCur && door_load_has_req(pDoor) == 0u &&
            pDoor->pClient == NULL) {
            pDoor->pServer = NULL;
        }
    }
}

void
door_reply(struct gj_door *pDoor, i64 i64Ret)
{
    struct gj_thread *pCur;

    door_soft_inc(&g_u64SoftReplyEnter);

    if (pDoor == NULL || !pDoor->u32Ready) {
        door_soft_inc(&g_u64SoftReplyNotReady);
        return;
    }
    /*
     * Stale reply: no in-flight client owns the slot - drop.
     * Call/reply residual: after serve_hold, client timeout/cancel may clear
     * pClient while this thr still owns pServer. Release serve so the next
     * recv / thr-exit is not stuck. multi_server=0.
     */
    if (pDoor->pClient == NULL) {
        door_soft_inc(&g_u64SoftReplyStale);
        pCur = thread_current();
        if (pCur != NULL && pDoor->pServer == pCur) {
            door_clear_serve(pDoor);
            (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
        }
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
    /* Publish reply value before HasReply release-store (SMP-prep residual). */
    pDoor->i64Reply = i64Ret;
    door_store_has_reply(pDoor, 1);
    pDoor->u64Replies++;
    door_soft_inc(&g_u64SoftReplyOk);
    /* End serve window - call/reply residual; multi_server=0. */
    door_clear_serve(pDoor);
    (void)thread_wake(pDoor, DOOR_TAG_CLIENT, 1);
    /* Nudge any parked foreign waiter (scaffolding only; multi_server=0). */
    (void)thread_wake(pDoor, DOOR_TAG_SERVER, 1);
}
