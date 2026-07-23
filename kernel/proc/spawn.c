/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * process_spawn + PROCESS task cap mint (G-PROC-*).
 *
 * Product lifecycle (SOLARIS_STYLE_REMAINING §6):
 *   spawn → CNode + root meta + private AS + first thread + parent PROCESS cap
 *   kill  → process_death (pager/regions/CNode/AS for wait-registered children)
 *   wait  → reap exit code, invalidate parent PROCESS cap, recycle spawn slot
 * Failure paths reverse tear down (no orphan AS / stale cap).
 */
#include <gj/cap.h>
#include <gj/cpu.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/process.h>
#include <gj/spawn.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/vmm.h>

struct spawn_slot {
    u8                   u8Used;
    u8                   u8Pad[7];
    struct gj_process    proc;
    struct gj_cnode      cnode;
    struct gj_cap_slot   aSlots[GJ_SPAWN_CNODE_SLOTS];
    struct gj_root_meta  meta;
};

static struct spawn_slot g_aSpawn[GJ_SPAWN_MAX];
static u32               g_cSpawned;

u32
process_spawn_count(void)
{
    return g_cSpawned;
}

/**
 * Locate fixed spawn-table entry for a child process (NULL if not a spawn child).
 */
static struct spawn_slot *
spawn_slot_of(struct gj_process *pProc)
{
    u32 iSlot;

    if (pProc == NULL) {
        return NULL;
    }
    for (iSlot = 0; iSlot < GJ_SPAWN_MAX; iSlot++) {
        if (g_aSpawn[iSlot].u8Used && &g_aSpawn[iSlot].proc == pProc) {
            return &g_aSpawn[iSlot];
        }
    }
    return NULL;
}

/**
 * Destroy private AS under kernel CR3; restore caller CR3.
 * Safe when u64Cr3 is 0 or matches kernel template.
 */
static void
spawn_as_teardown(struct gj_process *pProc)
{
    u64 u64Cr3;
    u64 u64Ker;
    u64 u64Saved;

    if (pProc == NULL || pProc->u64Cr3 == 0) {
        return;
    }
    u64Cr3 = pProc->u64Cr3;
    u64Ker = vmm_kernel_cr3();
    u64Saved = cpu_read_cr3();
    if (u64Ker != 0 &&
        (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        cpu_load_cr3(u64Ker);
        vmm_set_anon_cursor(NULL);
        (void)vmm_as_destroy(u64Cr3);
        pProc->u64Cr3 = 0;
        if (u64Saved != 0) {
            cpu_load_cr3(u64Saved);
        } else if (u64Ker != 0) {
            cpu_load_cr3(u64Ker);
        }
    }
}

/**
 * Reverse spawn install: drop parent PROCESS cap (if any) and private AS, free slot.
 */
static void
spawn_fail_cleanup(struct spawn_slot *pSlot, struct gj_process *pParent,
                   const struct gj_cap_ref *pRef)
{
    if (pSlot == NULL) {
        return;
    }
    if (pParent != NULL && pRef != NULL && pParent->pCnode != NULL) {
        struct gj_cap_resolved res;

        if (gj_cap_resolve_ref(pParent->pCnode, pRef, &res) == GJ_OK &&
            res.pSlot != NULL) {
            gj_cap_slot_invalidate(res.pSlot);
        }
    }
    spawn_as_teardown(&pSlot->proc);
    pSlot->u8Used = 0;
}

gj_status_t
process_kill(struct gj_process *pParent, const struct gj_cap_ref *pRef,
             u32 u32ExitCode)
{
    struct gj_process *pChild;
    struct gj_cap_resolved res;
    gj_status_t st;

    if (pParent == NULL || pRef == NULL) {
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK) {
        return st;
    }
    if (res.u16Type != (u16)GJ_CAP_PROCESS) {
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & (u16)GJ_RIGHT_DESTROY) == 0) {
        return GJ_ERR_PERM;
    }
    pChild = (struct gj_process *)res.pObj;
    if (pChild == NULL) {
        return GJ_ERR_NOENT;
    }
    if (!pChild->u32Alive) {
        /* Already dead — idempotent kill */
        return GJ_OK;
    }
    /*
     * G-PROC-5: full death path (pager, regions, wait-registered CNode wipe,
     * private AS destroy, zombie for wait4 / process_wait).
     */
    process_death(pChild, u32ExitCode);
    kprintf("spawn: kill child exit=%u PASS\n", u32ExitCode);
    return GJ_OK;
}

gj_status_t
process_wait(struct gj_process *pParent, const struct gj_cap_ref *pRef,
             u32 *pOutExit)
{
    struct gj_process *pChild;
    struct gj_cap_resolved res;
    struct spawn_slot *pSlot;
    gj_status_t st;
    u32 u32Exit;

    if (pParent == NULL || pRef == NULL) {
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK) {
        return st;
    }
    if (res.u16Type != (u16)GJ_CAP_PROCESS) {
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & (u16)GJ_RIGHT_WAIT) == 0) {
        return GJ_ERR_PERM;
    }
    pChild = (struct gj_process *)res.pObj;
    if (pChild == NULL) {
        return GJ_ERR_NOENT;
    }
    if (pChild->u32Alive) {
        return GJ_ERR_AGAIN;
    }
    u32Exit = pChild->u32ExitCode;
    if (pOutExit != NULL) {
        *pOutExit = u32Exit;
    }
    /*
     * Reap: drop wait-table entry, invalidate parent PROCESS cap, recycle
     * spawn slot. Cap gen bump prevents reuse of a stale handle.
     */
    process_wait_forget(pChild);
    if (res.pSlot != NULL) {
        gj_cap_slot_invalidate(res.pSlot);
    }
    pSlot = spawn_slot_of(pChild);
    if (pSlot != NULL) {
        pSlot->u8Used = 0;
    }
    kprintf("spawn: wait reaped exit=%u PASS\n", u32Exit);
    return GJ_OK;
}

struct gj_process *
process_from_cap(struct gj_process *pParent, const struct gj_cap_ref *pRef)
{
    struct gj_cap_resolved res;
    gj_status_t st;

    if (pParent == NULL || pRef == NULL || pParent->pCnode == NULL) {
        return NULL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK || res.u16Type != (u16)GJ_CAP_PROCESS) {
        return NULL;
    }
    return (struct gj_process *)res.pObj;
}

gj_status_t
process_spawn(struct gj_process *pParent, const struct gj_spawn_args *pArgs,
              struct gj_process **ppOutChild, struct gj_cap_ref *pOutCap)
{
    u32 iSlot;
    struct spawn_slot *pSlot;
    struct gj_process *pChild;
    struct gj_cap_ref ref;
    gj_status_t st;
    u16 u16Rights;
    u32 u32Thr;
    u32 u32Ppid;

    if (pParent == NULL || pArgs == NULL || pArgs->pfnEntry == NULL) {
        return GJ_ERR_INVAL;
    }
    if (pParent->pCnode == NULL) {
        return GJ_ERR_INVAL;
    }

    pSlot = NULL;
    for (iSlot = 0; iSlot < GJ_SPAWN_MAX; iSlot++) {
        if (!g_aSpawn[iSlot].u8Used) {
            pSlot = &g_aSpawn[iSlot];
            break;
        }
    }
    if (pSlot == NULL) {
        return GJ_ERR_NOMEM;
    }
    memset(pSlot, 0, sizeof(*pSlot));
    pSlot->u8Used = 1;
    pChild = &pSlot->proc;
    memset(&ref, 0, sizeof(ref));

    gj_process_init(pChild, &pSlot->cnode, pSlot->aSlots, GJ_SPAWN_CNODE_SLOTS);
    pChild->pParent = pParent;
    pChild->u32Alive = 1;
    pChild->u32Personality = pArgs->u32Personality;
    gj_process_set_jit(pChild, pArgs->u32Jit ? 1 : 0);

    st = gj_process_bootstrap_root_meta(pChild, &pSlot->meta, NULL);
    if (st != GJ_OK) {
        pSlot->u8Used = 0;
        return st;
    }

    st = process_as_ensure(pChild);
    if (st != GJ_OK) {
        pSlot->u8Used = 0;
        return st;
    }

    /* Mint PROCESS task port into parent CNode (G-PROC-2) */
    u16Rights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY | GJ_RIGHT_DESTROY |
                      GJ_RIGHT_WAIT | GJ_RIGHT_VM | GJ_RIGHT_SPAWN);
    if (pArgs->u32Jit) {
        u16Rights |= (u16)GJ_RIGHT_JIT;
    }
    st = gj_cap_alloc_install(pParent->pCnode, (u16)GJ_CAP_PROCESS, u16Rights,
                              &pChild->hdr, &ref);
    if (st != GJ_OK) {
        spawn_fail_cleanup(pSlot, NULL, NULL);
        return st;
    }

    u32Thr = thread_create(pChild, pArgs->pfnEntry, pArgs->pArg);
    if (u32Thr == 0) {
        spawn_fail_cleanup(pSlot, pParent, &ref);
        return GJ_ERR_NOMEM;
    }

    /*
     * Wait-register after first thr is live so process_death reclaims CNode +
     * private AS (G-PROC-5). Parent pid from wait table when known; else 1.
     */
    u32Ppid = process_wait_pid_of(pParent);
    (void)process_wait_register(pChild, u32Ppid != 0 ? u32Ppid : 1u);

    g_cSpawned++;
    kprintf("spawn: child thr=%u cr3=0x%lx cap slot=%lu gen=%u PASS\n",
            u32Thr, (unsigned long)pChild->u64Cr3,
            (unsigned long)ref.u64Slot, ref.u32SlotGen);

    if (ppOutChild != NULL) {
        *ppOutChild = pChild;
    }
    if (pOutCap != NULL) {
        *pOutCap = ref;
    }
    return GJ_OK;
}
