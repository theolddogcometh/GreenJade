/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cold-path Linux syscalls → personality.
 * Product: doors / sync service / legacy queue under soft mode flags.
 * Soft deepen: registration gens, mode mask, CLAIMED queue, stats.
 * Pure C11 — dual MIT OR Apache-2.0.
 *
 * greppable: GJ_COLD_MODE_ cold_ipc_register_service cold_ipc_stats
 */
#include <gj/cold_ipc.h>
#include <gj/door.h>
#include <gj/string.h>
#include <gj/thread.h>

static struct gj_cold_request g_aQ[GJ_COLD_QUEUE_DEPTH];
static u64 g_u64NextId = 1;
static int g_fAttached;
static int g_fInited;

/* Soft mode mask (default = bring-up product). */
static u32 g_u32ModeFlags = GJ_COLD_MODE_DEFAULT;

/* Soft service registration */
static i64 (*g_pfnService)(struct gj_linux_regs *pRegs, void *pCtx);
static void *g_pServiceCtx;
static u32 g_u32ServiceGen;

/* Soft queue-consumer registration */
static u32 g_u32QueueGen;
static u32 g_u32NextServiceGen = 1;
static u32 g_u32NextQueueGen = 1;

/* Product counters (wrap OK). */
static u64 g_u64Submits;
static u64 g_u64ServiceHits;
static u64 g_u64DoorHits;
static u64 g_u64QueueHits;
static u64 g_u64Enosys;
static u64 g_u64Inval;
static u64 g_u64QueueFull;
static u64 g_u64Dequeues;
static u64 g_u64DequeueEmpty;
static u64 g_u64Replies;
static u64 g_u64ReplyMiss;
static u64 g_u64ServiceLocal;
static u64 g_u64RegService;
static u64 g_u64UnregService;
static u64 g_u64RegQueue;
static u64 g_u64UnregQueue;
static u64 g_u64ModeChanges;

static u32
bump_service_gen(void)
{
    u32 u32G = g_u32NextServiceGen++;

    if (g_u32NextServiceGen == 0) {
        g_u32NextServiceGen = 1;
    }
    return u32G;
}

static u32
bump_queue_gen(void)
{
    u32 u32G = g_u32NextQueueGen++;

    if (g_u32NextQueueGen == 0) {
        g_u32NextQueueGen = 1;
    }
    return u32G;
}

static u32
count_pending(void)
{
    u32 iSlot;
    u32 u32N = 0;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        u32 u32St = g_aQ[iSlot].u32State;

        if (u32St == GJ_COLD_PENDING || u32St == GJ_COLD_CLAIMED) {
            u32N++;
        }
    }
    return u32N;
}

/**
 * Soft: doors path usable under current mode flags.
 * REQUIRE_SERVER (default): pServer must be live.
 * Without REQUIRE_SERVER: ready endpoint is enough (may block in door_call).
 */
static int
doors_usable(struct gj_door **ppDoorOut)
{
    struct gj_door *pDoor;

    if ((g_u32ModeFlags & GJ_COLD_MODE_DOORS) == 0) {
        return 0;
    }
    pDoor = door_cold_personality();
    if (pDoor == NULL || !pDoor->u32Ready) {
        return 0;
    }
    if ((g_u32ModeFlags & GJ_COLD_MODE_REQUIRE_SERVER) != 0) {
        if (pDoor->pServer == NULL ||
            pDoor->pServer->u32State == GJ_THR_EXITED) {
            return 0;
        }
    }
    if (ppDoorOut != NULL) {
        *ppDoorOut = pDoor;
    }
    return 1;
}

static int
service_usable(void)
{
    return ((g_u32ModeFlags & GJ_COLD_MODE_SERVICE) != 0) &&
           (g_pfnService != NULL);
}

static int
queue_usable(void)
{
    return ((g_u32ModeFlags & GJ_COLD_MODE_QUEUE) != 0) &&
           (g_fAttached || g_u32QueueGen != 0 || g_pfnService != NULL);
}

void
cold_ipc_init(void)
{
    /* Idempotent: must not wipe protonrt service after attach (boot smoke). */
    if (g_fInited) {
        return;
    }
    memset(g_aQ, 0, sizeof(g_aQ));
    g_u64NextId = 1;
    g_fAttached = 0;
    g_u32ModeFlags = GJ_COLD_MODE_DEFAULT;
    g_pfnService = NULL;
    g_pServiceCtx = NULL;
    g_u32ServiceGen = 0;
    g_u32QueueGen = 0;
    g_u32NextServiceGen = 1;
    g_u32NextQueueGen = 1;
    g_u64Submits = 0;
    g_u64ServiceHits = 0;
    g_u64DoorHits = 0;
    g_u64QueueHits = 0;
    g_u64Enosys = 0;
    g_u64Inval = 0;
    g_u64QueueFull = 0;
    g_u64Dequeues = 0;
    g_u64DequeueEmpty = 0;
    g_u64Replies = 0;
    g_u64ReplyMiss = 0;
    g_u64ServiceLocal = 0;
    g_u64RegService = 0;
    g_u64UnregService = 0;
    g_u64RegQueue = 0;
    g_u64UnregQueue = 0;
    g_u64ModeChanges = 0;
    door_cold_init();
    g_fInited = 1;
}

void
cold_ipc_set_personality_attached(int fAttached)
{
    g_fAttached = fAttached ? 1 : 0;
}

int
cold_ipc_personality_attached(void)
{
    /* Doors alone is not enough — need service, queue gen, or explicit attach. */
    return g_fAttached || g_pfnService != NULL || g_u32QueueGen != 0;
}

void
cold_ipc_set_service(i64 (*pfn)(struct gj_linux_regs *, void *), void *pCtx)
{
    (void)cold_ipc_register_service(pfn, pCtx);
}

u32
cold_ipc_register_service(i64 (*pfn)(struct gj_linux_regs *, void *), void *pCtx)
{
    g_u64RegService++;
    if (pfn == NULL) {
        g_pfnService = NULL;
        g_pServiceCtx = NULL;
        g_u32ServiceGen = 0;
        return 0;
    }
    g_pfnService = pfn;
    g_pServiceCtx = pCtx;
    g_u32ServiceGen = bump_service_gen();
    g_fAttached = 1;
    return g_u32ServiceGen;
}

int
cold_ipc_unregister_service(u32 u32Gen)
{
    if (u32Gen == 0 || u32Gen != g_u32ServiceGen) {
        return 0;
    }
    g_pfnService = NULL;
    g_pServiceCtx = NULL;
    g_u32ServiceGen = 0;
    g_u64UnregService++;
    return 1;
}

int
cold_ipc_service_registered(void)
{
    return g_pfnService != NULL;
}

void *
cold_ipc_service_ctx(void)
{
    return g_pServiceCtx;
}

u32
cold_ipc_service_gen(void)
{
    return g_u32ServiceGen;
}

u32
cold_ipc_register_queue_consumer(void)
{
    g_u32QueueGen = bump_queue_gen();
    g_fAttached = 1;
    g_u64RegQueue++;
    return g_u32QueueGen;
}

int
cold_ipc_unregister_queue_consumer(u32 u32Gen)
{
    if (u32Gen == 0 || u32Gen != g_u32QueueGen) {
        return 0;
    }
    g_u32QueueGen = 0;
    g_u64UnregQueue++;
    /* Soft: drop explicit attach only when no service remains bound. */
    if (g_pfnService == NULL) {
        g_fAttached = 0;
    }
    return 1;
}

u32
cold_ipc_queue_gen(void)
{
    return g_u32QueueGen;
}

u32
cold_ipc_queue_pending(void)
{
    return count_pending();
}

u32
cold_ipc_queue_depth(void)
{
    return (u32)GJ_COLD_QUEUE_DEPTH;
}

void
cold_ipc_set_doors_mode(int fEnable)
{
    u32 u32Prev = g_u32ModeFlags;

    if (fEnable) {
        g_u32ModeFlags |= GJ_COLD_MODE_DOORS;
    } else {
        g_u32ModeFlags &= ~GJ_COLD_MODE_DOORS;
    }
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
    }
}

void
cold_ipc_set_mode_flags(u32 u32Flags)
{
    if (g_u32ModeFlags != u32Flags) {
        g_u64ModeChanges++;
    }
    g_u32ModeFlags = u32Flags;
}

void
cold_ipc_or_mode_flags(u32 u32Bits)
{
    u32 u32Prev = g_u32ModeFlags;

    g_u32ModeFlags |= u32Bits;
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
    }
}

void
cold_ipc_and_mode_flags(u32 u32Bits)
{
    u32 u32Prev = g_u32ModeFlags;

    g_u32ModeFlags &= u32Bits;
    if (g_u32ModeFlags != u32Prev) {
        g_u64ModeChanges++;
    }
}

u32
cold_ipc_get_mode_flags(void)
{
    return g_u32ModeFlags;
}

int
cold_ipc_doors_enabled(void)
{
    return (g_u32ModeFlags & GJ_COLD_MODE_DOORS) != 0;
}

void
cold_ipc_stats_get(struct gj_cold_ipc_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    pOut->u64Submits = g_u64Submits;
    pOut->u64ServiceHits = g_u64ServiceHits;
    pOut->u64DoorHits = g_u64DoorHits;
    pOut->u64QueueHits = g_u64QueueHits;
    pOut->u64Enosys = g_u64Enosys;
    pOut->u64Inval = g_u64Inval;
    pOut->u64QueueFull = g_u64QueueFull;
    pOut->u64Dequeues = g_u64Dequeues;
    pOut->u64DequeueEmpty = g_u64DequeueEmpty;
    pOut->u64Replies = g_u64Replies;
    pOut->u64ReplyMiss = g_u64ReplyMiss;
    pOut->u64ServiceLocal = g_u64ServiceLocal;
    pOut->u64RegService = g_u64RegService;
    pOut->u64UnregService = g_u64UnregService;
    pOut->u64RegQueue = g_u64RegQueue;
    pOut->u64UnregQueue = g_u64UnregQueue;
    pOut->u64ModeChanges = g_u64ModeChanges;
    pOut->u32ModeFlags = g_u32ModeFlags;
    pOut->u32ServiceGen = g_u32ServiceGen;
    pOut->u32QueueGen = g_u32QueueGen;
    pOut->u32Pending = count_pending();
    pOut->u32Attached = cold_ipc_personality_attached() ? 1u : 0u;
    pOut->u32ServiceBound = (g_pfnService != NULL) ? 1u : 0u;
    pOut->u32DoorsEnabled = cold_ipc_doors_enabled() ? 1u : 0u;
    pOut->u32Pad = 0;
}

void
cold_ipc_stats_reset(void)
{
    /* Counters only — preserve soft registration, mode, and queue slots. */
    g_u64Submits = 0;
    g_u64ServiceHits = 0;
    g_u64DoorHits = 0;
    g_u64QueueHits = 0;
    g_u64Enosys = 0;
    g_u64Inval = 0;
    g_u64QueueFull = 0;
    g_u64Dequeues = 0;
    g_u64DequeueEmpty = 0;
    g_u64Replies = 0;
    g_u64ReplyMiss = 0;
    g_u64ServiceLocal = 0;
    g_u64RegService = 0;
    g_u64UnregService = 0;
    g_u64RegQueue = 0;
    g_u64UnregQueue = 0;
    g_u64ModeChanges = 0;
}

int
cold_ipc_dequeue(struct gj_cold_request *pOut)
{
    u32 iSlot;

    if (pOut == NULL) {
        g_u64DequeueEmpty++;
        return 0;
    }
    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        if (g_aQ[iSlot].u32State == GJ_COLD_PENDING) {
            /* Soft claim: personality owns slot until reply. */
            g_aQ[iSlot].u32State = GJ_COLD_CLAIMED;
            *pOut = g_aQ[iSlot];
            g_u64Dequeues++;
            return 1;
        }
    }
    g_u64DequeueEmpty++;
    return 0;
}

int
cold_ipc_reply(u64 u64Id, i64 i64Ret)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        u32 u32St = g_aQ[iSlot].u32State;

        if ((u32St == GJ_COLD_PENDING || u32St == GJ_COLD_CLAIMED) &&
            g_aQ[iSlot].u64Id == u64Id) {
            g_aQ[iSlot].regs.i64Ret = i64Ret;
            g_aQ[iSlot].u32State = GJ_COLD_DONE;
            (void)thread_wake(&g_aQ[iSlot], 0, 8);
            g_u64Replies++;
            return 1;
        }
    }
    g_u64ReplyMiss++;
    return 0;
}

/*
 * Legacy queue path for GJ_SYS_COLD_DEQUEUE / COLD_REPLY when doors and
 * sync service are unavailable. Product path prefers doors/service.
 * Soft: waits for DONE (covers PENDING and CLAIMED reclaim).
 */
static i64
submit_queue(struct gj_linux_regs *pRegs)
{
    u32 iSlot;
    struct gj_cold_request *pSlot;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        if (g_aQ[iSlot].u32State == GJ_COLD_FREE ||
            g_aQ[iSlot].u32State == GJ_COLD_DONE) {
            pSlot = &g_aQ[iSlot];
            pSlot->u64Id = g_u64NextId++;
            if (g_u64NextId == 0) {
                g_u64NextId = 1;
            }
            pSlot->regs = *pRegs;
            pSlot->u32State = GJ_COLD_PENDING;
            /* Soft: spin-yield until reply sets DONE (CLAIMED interim OK). */
            while (pSlot->u32State != GJ_COLD_DONE) {
                thread_yield();
            }
            pRegs->i64Ret = pSlot->regs.i64Ret;
            pSlot->u32State = GJ_COLD_FREE;
            return pRegs->i64Ret;
        }
    }
    g_u64QueueFull++;
    return -LINUX_EAGAIN;
}

static int
path_service(struct gj_linux_regs *pRegs, i64 *pOut)
{
    if (!service_usable()) {
        return 0;
    }
    g_u64ServiceHits++;
    *pOut = g_pfnService(pRegs, g_pServiceCtx);
    return 1;
}

static int
path_doors(struct gj_linux_regs *pRegs, i64 *pOut)
{
    struct gj_door *pDoor = NULL;

    if (!doors_usable(&pDoor)) {
        return 0;
    }
    g_u64DoorHits++;
    *pOut = door_call(pDoor, pRegs);
    return 1;
}

static int
path_queue(struct gj_linux_regs *pRegs, i64 *pOut)
{
    if (!queue_usable()) {
        return 0;
    }
    g_u64QueueHits++;
    *pOut = submit_queue(pRegs);
    return 1;
}

i64
cold_ipc_submit(struct gj_linux_regs *pRegs, u64 u64TimeoutNsec)
{
    i64 i64R = 0;
    int fDoorsFirst;

    (void)u64TimeoutNsec;

    g_u64Submits++;

    if (pRegs == NULL) {
        g_u64Inval++;
        return -LINUX_EINVAL;
    }

    /*
     * Soft product order:
     *   DOORS_FIRST (and not SERVICE_FIRST): doors → service → queue
     *   default / SERVICE_FIRST: service → doors → queue
     * Avoids kmain hang if a stale pServer is set and never replies when
     * REQUIRE_SERVER is clear and service is bound (service still wins under
     * SERVICE_FIRST).
     */
    fDoorsFirst = ((g_u32ModeFlags & GJ_COLD_MODE_DOORS_FIRST) != 0) &&
                  ((g_u32ModeFlags & GJ_COLD_MODE_SERVICE_FIRST) == 0);

    if (fDoorsFirst) {
        if (path_doors(pRegs, &i64R)) {
            return i64R;
        }
        if (path_service(pRegs, &i64R)) {
            return i64R;
        }
    } else {
        if (path_service(pRegs, &i64R)) {
            return i64R;
        }
        if (path_doors(pRegs, &i64R)) {
            return i64R;
        }
    }

    if (path_queue(pRegs, &i64R)) {
        return i64R;
    }

    g_u64Enosys++;
    return -LINUX_ENOSYS;
}

i64
cold_ipc_service_local(struct gj_linux_regs *pRegs)
{
    if (pRegs == NULL) {
        g_u64Inval++;
        return -LINUX_EINVAL;
    }
    g_u64ServiceLocal++;
    if (g_pfnService != NULL) {
        pRegs->i64Ret = g_pfnService(pRegs, g_pServiceCtx);
        return pRegs->i64Ret;
    }
    pRegs->i64Ret = -LINUX_ENOSYS;
    return pRegs->i64Ret;
}

/* Personality server body — run as kernel thread (bring-up fallback) */
void
cold_personality_server(void *pArg)
{
    struct gj_door *pDoor = door_cold_personality();
    struct gj_linux_regs regsReq;
    i64 i64R;

    (void)pArg;
    cold_ipc_set_personality_attached(1);
    for (;;) {
        if (door_recv(pDoor, &regsReq) != 0) {
            thread_yield();
            continue;
        }
        i64R = cold_ipc_service_local(&regsReq);
        door_reply(pDoor, i64R);
    }
}
