/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cold-path Linux syscalls → personality.
 * Product: doors first; sync service for bring-up; legacy queue last.
 * Pure C11 — dual MIT OR Apache-2.0.
 */
#include <gj/cold_ipc.h>
#include <gj/door.h>
#include <gj/string.h>
#include <gj/thread.h>

static struct gj_cold_request g_aQ[GJ_COLD_QUEUE_DEPTH];
static u64 g_u64NextId = 1;
static int g_fAttached;
static int g_fUseDoors = 1;
static int g_fInited;
static i64 (*g_pfnService)(struct gj_linux_regs *pRegs, void *pCtx);
static void *g_pServiceCtx;

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
    g_fUseDoors = 1;
    g_pfnService = NULL;
    g_pServiceCtx = NULL;
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
    /* Doors alone is not enough — need service or explicit attach. */
    return g_fAttached || g_pfnService != NULL;
}

void
cold_ipc_set_service(i64 (*pfn)(struct gj_linux_regs *, void *), void *pCtx)
{
    g_pfnService = pfn;
    g_pServiceCtx = pCtx;
    if (pfn != NULL) {
        g_fAttached = 1;
    }
}

void
cold_ipc_set_doors_mode(int fEnable)
{
    g_fUseDoors = fEnable ? 1 : 0;
}

int
cold_ipc_dequeue(struct gj_cold_request *pOut)
{
    u32 iSlot;

    if (pOut == NULL) {
        return 0;
    }
    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        if (g_aQ[iSlot].u32State == GJ_COLD_PENDING) {
            *pOut = g_aQ[iSlot];
            return 1;
        }
    }
    return 0;
}

int
cold_ipc_reply(u64 u64Id, i64 i64Ret)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < GJ_COLD_QUEUE_DEPTH; iSlot++) {
        if (g_aQ[iSlot].u32State == GJ_COLD_PENDING &&
            g_aQ[iSlot].u64Id == u64Id) {
            g_aQ[iSlot].regs.i64Ret = i64Ret;
            g_aQ[iSlot].u32State = GJ_COLD_DONE;
            (void)thread_wake(&g_aQ[iSlot], 0, 8);
            return 1;
        }
    }
    return 0;
}

/*
 * Legacy queue path for GJ_SYS_COLD_DEQUEUE / COLD_REPLY when doors and
 * sync service are unavailable. Product path prefers doors.
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
            while (pSlot->u32State == GJ_COLD_PENDING) {
                thread_yield();
            }
            pRegs->i64Ret = pSlot->regs.i64Ret;
            pSlot->u32State = GJ_COLD_FREE;
            return pRegs->i64Ret;
        }
    }
    return -LINUX_EAGAIN;
}

i64
cold_ipc_submit(struct gj_linux_regs *pRegs, u64 u64TimeoutNsec)
{
    struct gj_door *pDoor;

    (void)u64TimeoutNsec;

    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }

    pDoor = door_cold_personality();

    /*
     * Product bring-up / hybrid smoke: sync service wins when attached.
     * door_call only when no service (userspace door server path).
     * Avoids kmain hang if a stale pServer is set and never replies.
     */
    if (g_pfnService != NULL) {
        return g_pfnService(pRegs, g_pServiceCtx);
    }

    if (g_fUseDoors && pDoor != NULL && pDoor->u32Ready &&
        pDoor->pServer != NULL &&
        pDoor->pServer->u32State != GJ_THR_EXITED) {
        return door_call(pDoor, pRegs);
    }

    /* Last resort: legacy queue for COLD_DEQUEUE consumers */
    if (g_fAttached) {
        return submit_queue(pRegs);
    }
    return -LINUX_ENOSYS;
}

i64
cold_ipc_service_local(struct gj_linux_regs *pRegs)
{
    if (pRegs == NULL) {
        return -LINUX_EINVAL;
    }
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
