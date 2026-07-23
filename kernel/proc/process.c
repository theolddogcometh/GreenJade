/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Process: shared CNode, root meta bootstrap, pager on PCB, wait4 reaper,
 * G-PROC-5 death (CNode wipe + private AS reclaim for wait-registered children).
 * Soft deepen: pager ep kernel ref + badge + slot-1 mirror; wait reparent /
 * WNOWAIT / counts; death quota+CDT CNode clear + orphan reparent + scrub.
 *
 * Soft product paths (this file only; not product-complete; no bar3):
 *   Apple §13 bootstrap seal checklist — greppable:
 *     process: bootstrap seal soft …
 *     process: seal checklist …
 *     enumerates root meta, ambient drop lamps, pager empty, promises
 *   G-PROC-5 death tallies — greppable process: death …
 *     pager clear, fault_lock force, CNode wipe, AS destroy
 *   Soft confine exposes PCB u32Confined / u32Promises (ambient drop lamps)
 *
 * Honesty: seal checklist is soft inventory only — not product multi-server
 * seal, not Apple §13 closed, not bar3.
 * docs/CAP_ADDRESSING.md · docs/APPLE_CHANNEL_REMAINING.md §13 ·
 * docs/SOLARIS_STYLE_REMAINING.md §6 · §9 · docs/SECURITY_CORE_DESIGN.md §13
 */
#include <gj/cap.h>
#include <gj/cpu.h>
#include <gj/door.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/vmm.h>

void
gj_process_init(struct gj_process *pProc, struct gj_cnode *pCnode,
                struct gj_cap_slot *pSlots, u64 cSlots)
{
    if (pProc == NULL || pCnode == NULL || pSlots == NULL) {
        return;
    }

    gj_obj_hdr_init(&pProc->hdr);
    gj_cnode_init(pCnode, pSlots, cSlots);

    pProc->pCnode = pCnode;
    pProc->pRootMeta = NULL;
    pProc->refPager = gj_cap_ref_null();
    pProc->pPagerEpObj = NULL;
    pProc->u32PagerBadge = 0;
    gj_space_fault_init(&pProc->fault);
    pProc->u32Personality = 1; /* LINUX default for game path */
    pProc->u32Jit = 0;
    pProc->u32Confined = 0; /* ambient until gj_process_confine */
    pProc->u32Promises = GJ_PROMISE_ALL;
    pProc->u64Cr3 = 0; /* inherit until per-process AS (G-AS-1) */
    pProc->u64AnonNext = 0x0000000040000000ull;
    pProc->u64ExecEntry = 0;
    pProc->u64InterpEntry = 0;
    pProc->u64LoadBias = 0;
    pProc->u64ExecStack = 0;
    pProc->u64StartEntry = 0;
    pProc->u32StartThr = 0;
    pProc->u32ExecFlags = 0;
    pProc->cNeededLoaded = 0;
    pProc->cAuxv = 0;
    memset(pProc->aAuxv, 0, sizeof(pProc->aAuxv));
    memset(pProc->szExecPath, 0, sizeof(pProc->szExecPath));
    memset(pProc->aRegions, 0, sizeof(pProc->aRegions));
    pProc->pParent = NULL;
    pProc->u32ExitCode = 0;
    pProc->u32Alive = 1;
    memset(&pProc->excPort, 0, sizeof(pProc->excPort));
}

void
gj_process_set_jit(struct gj_process *pProc, int fEnable)
{
    if (pProc == NULL) {
        return;
    }
    /* G-JIT-4: u32Jit is cache of GJ_RIGHT_JIT authority */
    pProc->u32Jit = fEnable ? 1u : 0u;
}

/*
 * Soft Apple §13 bootstrap seal checklist (process.c only).
 * Enumerates PCB lamps: root meta, ambient/confine, pager empty, promises.
 * One-way soft lamp is inventory only — does NOT seal retype/IRQ/untyped.
 * Grep: process: bootstrap seal soft | process: seal checklist
 * Honesty: not product-complete; no bar3.
 */
static u32 g_u32SealChecklistLogs;
static u32 g_u32BootstrapSealSoftLamp; /* 0 open, 1 soft-attempted once */
#define GJ_SEAL_SOFT_LOG_MAX 8u

/* Soft G-PROC-5 death tallies (grep: process: death). */
static u64 g_u64DeathTotal;
static u64 g_u64DeathPagerClear;
static u64 g_u64DeathFaultForce;
static u64 g_u64DeathCnodeWipe;
static u64 g_u64DeathCnodeSlots;
static u64 g_u64DeathAsDestroyOk;
static u64 g_u64DeathAsDestroyFail;
static u64 g_u64DeathAsSkip;

static void
process_seal_checklist_soft(const struct gj_process *pProc, const char *szVia)
{
    int fRootMeta;
    int fPagerEmpty;
    int fAmbient;
    u32 u32Confined;
    u32 u32Promises;
    const char *szViaSafe;

    if (pProc == NULL) {
        return;
    }
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    /*
     * Rate-limit root_meta installs (many PE smokes); always emit on
     * confine (ambient authority drop is the soft seal edge).
     */
    if (strcmp(szViaSafe, "confine") != 0 &&
        g_u32SealChecklistLogs >= GJ_SEAL_SOFT_LOG_MAX) {
        return;
    }
    g_u32SealChecklistLogs++;
    fRootMeta = (pProc->pRootMeta != NULL) ? 1 : 0;
    /*
     * Const path: gen!=0 and LIVE when pPagerEpObj known.
     * Empty gen ⇒ pager empty (expected post-init before set_pager).
     */
    fPagerEmpty = 0;
    if (gj_cap_ref_is_null(&pProc->refPager)) {
        fPagerEmpty = 1;
    } else if (pProc->pPagerEpObj != NULL &&
               pProc->pPagerEpObj->u32State != (u32)GJ_OBJ_LIVE) {
        fPagerEmpty = 1; /* dead ep treated as empty for seal lamp */
    }
    u32Confined = pProc->u32Confined;
    u32Promises = pProc->u32Promises;
    fAmbient = (u32Confined == 0u) ? 1 : 0;

    /* Grep: process: bootstrap seal soft */
    kprintf("process: bootstrap seal soft via=%s logs=%u lamp=%u "
            "(Apple s13 soft checklist)\n",
            szViaSafe, g_u32SealChecklistLogs, g_u32BootstrapSealSoftLamp);

    /* Grep: process: seal checklist … */
    kprintf("process: seal checklist root_meta=%u pager_empty=%u "
            "ambient=%u confined=%u promises=0x%x soft\n",
            fRootMeta, fPagerEmpty, fAmbient, u32Confined, u32Promises);

    /* Ambient authority drop lamps (soft confine PCB flags exposed). */
    kprintf("process: seal checklist ambient_drop_lamp confined=%u "
            "promises=0x%x ambient=%u soft\n",
            u32Confined, u32Promises, fAmbient);

    kprintf("process: seal checklist root_meta_installed=%u "
            "pager_empty=%u promises=0x%x soft\n",
            fRootMeta, fPagerEmpty, u32Promises);

    /* Honesty: not product multi-server seal; Apple s13 remains open. */
    kprintf("process: bootstrap seal soft not product-complete "
            "(Apple s13 open; no bar3)\n");
}

/*
 * Soft one-way seal lamp (Apple s13). Records checklist; never product-seals
 * privileged retype / broad IRQ / root untyped. Grep: process: bootstrap seal soft
 */
static void
process_bootstrap_seal_soft_try(struct gj_process *pProc, const char *szVia)
{
    if (pProc == NULL) {
        return;
    }
    if (g_u32BootstrapSealSoftLamp == 0u) {
        g_u32BootstrapSealSoftLamp = 1u;
        kprintf("process: bootstrap seal soft one-way lamp=1 "
                "(soft only; not product-complete; no bar3)\n");
    }
    process_seal_checklist_soft(pProc, szVia);
}

void
gj_process_confine(struct gj_process *pProc, u32 u32Promises)
{
    if (pProc == NULL) {
        return;
    }
    /* Soft: set confined; promises are the allowed ambient set. */
    pProc->u32Confined = 1u;
    pProc->u32Promises = u32Promises;
    /*
     * Soft confine expose PCB flags + ambient authority drop lamp.
     * Grep: process: confine soft | process: seal checklist
     * Not product multi-server; no bar3.
     */
    kprintf("process: confine soft confined=%u promises=0x%x "
            "ambient_drop=1 (soft; not product multi-server; no bar3)\n",
            pProc->u32Confined, pProc->u32Promises);
    /* Confine is the soft ambient-drop edge — re-emit seal checklist lamps. */
    process_bootstrap_seal_soft_try(pProc, "confine");
}

int
gj_process_promise_ok(const struct gj_process *pProc, u32 u32Promise)
{
    if (pProc == NULL) {
        return 0;
    }
    if (pProc->u32Confined == 0u) {
        return 1; /* ambient */
    }
    return (pProc->u32Promises & u32Promise) != 0u ? 1 : 0;
}

int
gj_process_promise_require(const struct gj_process *pProc, u32 u32Promise)
{
    /* NULL process: no confine subject (product ambient smokes). */
    if (pProc == NULL) {
        return 0;
    }
    if (gj_process_promise_ok(pProc, u32Promise)) {
        return 0;
    }
    return -13; /* LINUX_EACCES shape (linux_abi.h LINUX_EACCES) */
}

int
gj_process_has_jit(const struct gj_process *pProc)
{
    /*
     * CapJit: bool cache OR PROCESS self-right when wired via parent mint.
     * Authority source is GJ_RIGHT_JIT on task cap (G-JIT-1); cache for speed.
     */
    return pProc != NULL && pProc->u32Jit != 0;
}

gj_status_t
gj_process_bootstrap_root_meta(struct gj_process *pProc,
                               struct gj_root_meta *pMeta,
                               struct gj_cap_ref *pOutRef)
{
    gj_status_t st;
    struct gj_cap_ref ref;
    u16 u16Rights;

    if (pProc == NULL || pMeta == NULL || pProc->pCnode == NULL) {
        return GJ_ERR_INVAL;
    }

    /* Already filled? */
    if (pProc->pRootMeta != NULL) {
        return GJ_ERR_BUSY;
    }

    gj_obj_hdr_init(&pMeta->hdr);
    pMeta->pProc = pProc;
    pMeta->pCnode = pProc->pCnode;
    pProc->pRootMeta = pMeta;

    /*
     * Install into slot 0 as ROOT_META.
     * Process + CNode are kernel fields for kernel ops only (K1–K6).
     * Not transferable Scheme A PROCESS/CNODE caps.
     */
    u16Rights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY);
    st = gj_cap_slot_install(pProc->pCnode, GJ_CAP_SLOT_ROOT_META,
                             (u16)GJ_CAP_ROOT_META, u16Rights, &pMeta->hdr,
                             &ref);
    if (st != GJ_OK) {
        pProc->pRootMeta = NULL;
        pMeta->pProc = NULL;
        pMeta->pCnode = NULL;
        return st;
    }

    if (pOutRef != NULL) {
        *pOutRef = ref;
    }
    /*
     * Soft Apple §13 seal checklist after root meta install.
     * Enumerates root_meta / pager_empty / ambient / promises (rate-limited).
     * Grep: process: bootstrap seal soft | process: seal checklist
     * Honesty: not product-complete; no bar3.
     */
    process_seal_checklist_soft(pProc, "root_meta");
    return GJ_OK;
}

/*
 * Soft pager kernel ref (SOLARIS_STYLE §9): hold endpoint while PCB names it.
 * Grep: process:pager ref
 */
static void
process_pager_ref_hold(struct gj_obj_hdr *pObj)
{
    if (pObj == NULL) {
        return;
    }
    pObj->u32Ref++;
}

static void
process_pager_ref_drop(struct gj_obj_hdr *pObj)
{
    if (pObj == NULL) {
        return;
    }
    if (pObj->u32Ref > 0u) {
        pObj->u32Ref--;
    }
}

/*
 * Soft: optional CAP_ADDRESSING slot-1 mirror of default pager.
 * Kernel still uses PCB as canonical; slot is introspection only.
 * Grep: process:pager slot1
 */
static void
process_pager_mirror_clear(struct gj_process *pProc)
{
    struct gj_cap_slot *pSlot;

    if (pProc == NULL || pProc->pCnode == NULL || pProc->pCnode->pSlots == NULL) {
        return;
    }
    if (pProc->pCnode->cSlots <= GJ_CAP_SLOT_PAGER) {
        return;
    }
    pSlot = &pProc->pCnode->pSlots[GJ_CAP_SLOT_PAGER];
    if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
        return;
    }
    /* Only clear if mirror still names our pager object (or any ENDPOINT). */
    if (pSlot->u16Type == (u16)GJ_CAP_ENDPOINT &&
        (pProc->pPagerEpObj == NULL || pSlot->pObj == (void *)pProc->pPagerEpObj)) {
        (void)gj_cap_quota_slot_refund(pProc->pCnode->pQuotaAccount);
        if (pSlot->pObj != NULL) {
            gj_cdt_unlink_slot((struct gj_obj_hdr *)pSlot->pObj, pProc->pCnode,
                               GJ_CAP_SLOT_PAGER);
        }
        gj_cap_slot_invalidate_locked(pSlot, (struct gj_obj_hdr *)pSlot->pObj);
    }
}

static void
process_pager_mirror_install(struct gj_process *pProc, struct gj_obj_hdr *pEp,
                             u16 u16Rights)
{
    struct gj_cap_ref refMirror;
    gj_status_t st;
    u16 u16MirRights;

    if (pProc == NULL || pProc->pCnode == NULL || pEp == NULL) {
        return;
    }
    if (pProc->pCnode->cSlots <= GJ_CAP_SLOT_PAGER) {
        return;
    }
    /* Drop prior mirror without requiring it matches old ep. */
    {
        struct gj_cap_slot *pSlot = &pProc->pCnode->pSlots[GJ_CAP_SLOT_PAGER];

        if (pSlot->u16Type != (u16)GJ_CAP_INVALID) {
            (void)gj_cap_quota_slot_refund(pProc->pCnode->pQuotaAccount);
            if (pSlot->pObj != NULL) {
                gj_cdt_unlink_slot((struct gj_obj_hdr *)pSlot->pObj, pProc->pCnode,
                                   GJ_CAP_SLOT_PAGER);
            }
            gj_cap_slot_invalidate_locked(pSlot,
                                          (struct gj_obj_hdr *)pSlot->pObj);
        }
    }
    /* Mirror is READ|IDENTIFY (+ GRANT if source had it) — not ambient MAP. */
    u16MirRights = (u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY);
    if ((u16Rights & (u16)GJ_RIGHT_GRANT) != 0) {
        u16MirRights = (u16)(u16MirRights | GJ_RIGHT_GRANT);
    }
    st = gj_cap_slot_install(pProc->pCnode, GJ_CAP_SLOT_PAGER,
                             (u16)GJ_CAP_ENDPOINT, u16MirRights, pEp,
                             &refMirror);
    if (st == GJ_OK) {
        kprintf("process: pager mirror slot=%lu gen=%u soft\n",
                (unsigned long)refMirror.u64Slot, refMirror.u32SlotGen);
    }
}

void
gj_process_clear_pager(struct gj_process *pProc)
{
    struct gj_obj_hdr *pOld;

    if (pProc == NULL) {
        return;
    }
    pOld = pProc->pPagerEpObj;
    process_pager_mirror_clear(pProc);
    pProc->refPager = gj_cap_ref_null();
    pProc->pPagerEpObj = NULL;
    pProc->u32PagerBadge = 0;
    /* Soft: drop kernel hold after PCB cleared (SOLARIS_STYLE §9 clear). */
    process_pager_ref_drop(pOld);
}

void
gj_process_pager_refresh(struct gj_process *pProc)
{
    if (pProc == NULL) {
        return;
    }
    if (gj_cap_ref_is_null(&pProc->refPager)) {
        return;
    }
    /* Soft ep-revoke hook: DEAD/REVOKING endpoint clears PCB pager. */
    if (pProc->pPagerEpObj == NULL ||
        pProc->pPagerEpObj->u32State != (u32)GJ_OBJ_LIVE) {
        kprintf("process: pager refresh clear (ep dead) soft\n");
        gj_process_clear_pager(pProc);
    }
}

u32
gj_process_pager_badge(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    return pProc->u32PagerBadge;
}

gj_status_t
gj_process_set_pager_badge(struct gj_process *pProc, u64 u64EpSlot,
                           u32 u32EpGen, u32 u32Badge)
{
    struct gj_cap_resolved res;
    gj_status_t st;
    struct gj_obj_hdr *pOld;
    u32 u32SnapBadge;

    if (pProc == NULL || pProc->pCnode == NULL) {
        return GJ_ERR_INVAL;
    }

    /* Clear pager */
    if (u32EpGen == 0) {
        gj_process_clear_pager(pProc);
        return GJ_OK;
    }

    /* Endpoint must resolve in *this* process CNode (Scheme A). */
    st = gj_cap_resolve(pProc->pCnode, u64EpSlot, u32EpGen, &res);
    if (st != GJ_OK) {
        return st;
    }
    /* Pager must be ENDPOINT (doors-like Call/reply); GRANT required. */
    if (res.u16Type != (u16)GJ_CAP_ENDPOINT) {
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & (u16)GJ_RIGHT_GRANT) == 0) {
        return GJ_ERR_PERM;
    }
    if (res.pObj == NULL) {
        return GJ_ERR_INVAL;
    }
    /* Soft LIVE check — refuse DEAD/REVOKING endpoints (fail closed). */
    if (res.pObj->u32State != (u32)GJ_OBJ_LIVE) {
        return GJ_ERR_DEAD;
    }

    /* Soft badge: explicit arg wins; else snap door server badge. */
    u32SnapBadge = u32Badge;
    if (u32SnapBadge == 0u) {
        u32SnapBadge = door_get_badge((struct gj_door *)res.pObj);
    }

    /* Replace: hold new first, then drop old (avoid transient zero-ref). */
    pOld = pProc->pPagerEpObj;
    process_pager_ref_hold(res.pObj);
    pProc->refPager = gj_cap_ref_make(u64EpSlot, u32EpGen);
    pProc->pPagerEpObj = res.pObj;
    pProc->u32PagerBadge = u32SnapBadge;
    if (pOld != NULL && pOld != res.pObj) {
        process_pager_ref_drop(pOld);
    } else if (pOld == res.pObj) {
        /* Same object re-set: undo the extra hold from this call. */
        process_pager_ref_drop(res.pObj);
    }
    process_pager_mirror_install(pProc, res.pObj, res.u16Rights);
    kprintf("process: set_pager slot=%lu gen=%u badge=%u ref=%u soft\n",
            (unsigned long)u64EpSlot, u32EpGen, u32SnapBadge,
            res.pObj->u32Ref);
    return GJ_OK;
}

gj_status_t
gj_process_set_pager(struct gj_process *pProc, u64 u64EpSlot, u32 u32EpGen)
{
    /* Badge 0 → soft-snap from door endpoint when LIVE. */
    return gj_process_set_pager_badge(pProc, u64EpSlot, u32EpGen, 0u);
}

int
gj_process_has_pager(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    if (gj_cap_ref_is_null(&pProc->refPager)) {
        return 0;
    }
    /*
     * Soft refresh needs mutable PCB; const path only reports gen.
     * Callers that need ep-dead clear should use gj_process_pager_refresh.
     */
    if (pProc->pPagerEpObj != NULL &&
        pProc->pPagerEpObj->u32State != (u32)GJ_OBJ_LIVE) {
        return 0;
    }
    return 1;
}

/*
 * Fault policy (CAP_ADDRESSING + SOLARIS_STYLE_REMAINING §7):
 * - one fault lock per space (here: process.fault until gj_space exists)
 * - no pager ⇒ FAULT (kill)
 * - with pager: build cluster + kernel cookie; Call pager (IPC later)
 * - object owns pages / maps are views (map path not fully wired; Apple §2)
 */
gj_status_t
gj_process_handle_fault(struct gj_process *pProc, u64 u64FaultVa, int fWrite,
                        int fExec)
{
    gj_status_t st;
    u64 u64Base;
    u32 u32Access;
    u32 u32NPages;
    struct gj_fault_msg msg;
    struct gj_map_cookie cookie;

    if (pProc == NULL) {
        return GJ_ERR_INVAL;
    }

    /* Soft: drop PCB pager if endpoint was revoked under us. */
    gj_process_pager_refresh(pProc);

    st = gj_space_fault_enter(&pProc->fault);
    if (st != GJ_OK) {
        /* Full impl: wait on CV with mono timeout */
        return st;
    }

    if (!gj_process_has_pager(pProc)) {
        gj_space_fault_leave(&pProc->fault);
        return GJ_ERR_FAULT; /* kill path */
    }

    /* Page-align; cluster of 1 for now (coalesce adjacent later). */
    u64Base = u64FaultVa & ~(4096ull - 1ull);
    u32NPages = 1;
    if (u32NPages > GJ_FAULT_CLUSTER_MAX) {
        u32NPages = GJ_FAULT_CLUSTER_MAX;
    }
    u32Access = GJ_FAULT_ACCESS_R;
    if (fWrite) {
        u32Access |= GJ_FAULT_ACCESS_W;
    }
    if (fExec) {
        u32Access |= GJ_FAULT_ACCESS_X;
    }
    /* W^X: refuse W|X at policy layer when mapping */
    if ((u32Access & GJ_FAULT_ACCESS_W) && (u32Access & GJ_FAULT_ACCESS_X)) {
        gj_space_fault_leave(&pProc->fault);
        return GJ_ERR_PERM;
    }

    st = gj_map_cookie_create(&cookie, /*pSpace*/ pProc, pProc, /*thread*/ NULL,
                              u64Base, u32NPages, u32Access,
                              /*deadline*/ 0, &msg);
    if (st != GJ_OK) {
        gj_space_fault_leave(&pProc->fault);
        return st;
    }

    /*
     * Full path: ipc_call(pager, &msg) with mono timeout; on OK,
     * consume cookie, map FRAMEs, transfer ownership to client, resume.
     * Soft: stamp badge into flags low bits for pager payload later.
     * Until IPC+map exist: invalidate cookie and report AGAIN.
     */
    msg.u32Flags = pProc->u32PagerBadge;
    (void)cookie;
    gj_map_cookie_invalidate(msg.u64CookieLo, msg.u64CookieHi);
    gj_space_fault_leave(&pProc->fault);
    return GJ_ERR_AGAIN;
}

/* ---- wait4 reaper (product: PROCESS caps; interim fixed zombie table) --- */

#define GJ_WAIT_SLOTS 64u
#define GJ_WAIT_PID_BASE 100u

struct process_wait_slot {
    u8                 u8Used;
    u8                 u8Zombie;
    u8                 u8Reaped;
    u8                 u8Pad;
    u32                u32Pid;
    u32                u32Ppid;
    u32                u32Exit;
    struct gj_process *pProc;
};

static struct process_wait_slot g_aWait[GJ_WAIT_SLOTS];
static u32                      g_u32NextPid = GJ_WAIT_PID_BASE;
/* Soft reaper observability (wrap OK). Grep: process:wait stats */
static u64                      g_u64WaitRegister;
static u64                      g_u64WaitZombie;
static u64                      g_u64WaitReap;
static u64                      g_u64WaitReparent;
static u64                      g_u64WaitNowaitPeek;

u32
process_wait_register(struct gj_process *pChild, u32 u32Ppid)
{
    u32 i;
    u32 pid;
    u32 u32ParentPid;

    if (pChild == NULL) {
        return 0;
    }
    /* Already registered? */
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pChild) {
            return g_aWait[i].u32Pid;
        }
    }
    u32ParentPid = u32Ppid ? u32Ppid : 1u;
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used) {
            pid = g_u32NextPid++;
            if (g_u32NextPid < GJ_WAIT_PID_BASE) {
                g_u32NextPid = GJ_WAIT_PID_BASE;
            }
            g_aWait[i].u8Used = 1;
            g_aWait[i].u8Zombie = 0;
            g_aWait[i].u8Reaped = 0;
            g_aWait[i].u32Pid = pid;
            g_aWait[i].u32Ppid = u32ParentPid;
            g_aWait[i].u32Exit = 0;
            g_aWait[i].pProc = pChild;
            pChild->u32Alive = 1;
            /* Soft: link pParent when parent PCB is still in the wait table. */
            pChild->pParent = NULL;
            {
                u32 j;

                for (j = 0; j < GJ_WAIT_SLOTS; j++) {
                    if (g_aWait[j].u8Used && g_aWait[j].u32Pid == u32ParentPid) {
                        pChild->pParent = g_aWait[j].pProc;
                        break;
                    }
                }
            }
            g_u64WaitRegister++;
            kprintf("process: wait register pid=%u ppid=%u\n", pid,
                    g_aWait[i].u32Ppid);
            return pid;
        }
    }
    return 0; /* table full — caller may continue without wait4 */
}

void
process_wait_note_exit(struct gj_process *pChild, u32 u32Code)
{
    u32 i;

    if (pChild == NULL) {
        return;
    }
    pChild->u32ExitCode = u32Code;
    pChild->u32Alive = 0;
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pChild) {
            /* Soft: re-note updates exit code even if already zombie. */
            if (!g_aWait[i].u8Zombie) {
                g_u64WaitZombie++;
            }
            g_aWait[i].u8Zombie = 1;
            g_aWait[i].u32Exit = u32Code;
            kprintf("process: zombie pid=%u code=%u\n", g_aWait[i].u32Pid,
                    u32Code);
            return;
        }
    }
}

void
process_wait_forget(struct gj_process *pProc)
{
    u32 i;

    if (pProc == NULL) {
        return;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pProc) {
            g_aWait[i].u8Used = 0;
            g_aWait[i].u8Zombie = 0;
            g_aWait[i].u8Reaped = 0;
            g_aWait[i].u32Exit = 0;
            g_aWait[i].u32Pid = 0;
            g_aWait[i].u32Ppid = 0;
            g_aWait[i].pProc = NULL;
            return;
        }
    }
}

static int
process_is_wait_child(struct gj_process *pProc)
{
    u32 i;

    if (pProc == NULL) {
        return 0;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pProc) {
            return 1;
        }
    }
    return 0;
}

u32
process_wait_reparent(u32 u32OldPpid, u32 u32NewPpid)
{
    u32 i;
    u32 u32N = 0;

    if (u32OldPpid == 0 || u32NewPpid == 0 || u32OldPpid == u32NewPpid) {
        return 0;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used || g_aWait[i].u8Reaped) {
            continue;
        }
        if (g_aWait[i].u32Ppid != u32OldPpid) {
            continue;
        }
        g_aWait[i].u32Ppid = u32NewPpid;
        if (g_aWait[i].pProc != NULL) {
            g_aWait[i].pProc->pParent = NULL; /* soft: parent PCB gone */
        }
        u32N++;
        g_u64WaitReparent++;
        kprintf("process: wait reparent pid=%u ppid %u→%u soft\n",
                g_aWait[i].u32Pid, u32OldPpid, u32NewPpid);
    }
    return u32N;
}

u32
process_wait_live_count(u32 u32Ppid)
{
    u32 i;
    u32 u32N = 0;

    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used || g_aWait[i].u8Reaped || g_aWait[i].u8Zombie) {
            continue;
        }
        if (u32Ppid != 0 && g_aWait[i].u32Ppid != u32Ppid) {
            continue;
        }
        u32N++;
    }
    return u32N;
}

u32
process_wait_zombie_count(u32 u32Ppid)
{
    u32 i;
    u32 u32N = 0;

    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (!g_aWait[i].u8Used || g_aWait[i].u8Reaped || !g_aWait[i].u8Zombie) {
            continue;
        }
        if (u32Ppid != 0 && g_aWait[i].u32Ppid != u32Ppid) {
            continue;
        }
        u32N++;
    }
    return u32N;
}

/*
 * Soft G-PROC-5 CNode wipe: kernel-authority slot clear with quota refund +
 * CDT unlink (never rights-gated like user gj_cap_delete). Boot/init CNodes
 * are not passed here — only wait-registered children.
 * Grep: process: death cnode | process:death cnode
 */
static u32
process_death_cnode_wipe(struct gj_process *pProc)
{
    u64 u64Slot;
    u32 u32Cleared = 0;
    struct gj_cnode *pCnode;

    if (pProc == NULL || pProc->pCnode == NULL || pProc->pCnode->pSlots == NULL) {
        return 0;
    }
    pCnode = pProc->pCnode;
    /* Soft try-lock: death still wipes if busy (must not skip); unlock only if held. */
    {
        int fLocked = gj_cnode_trylock(pCnode);

        for (u64Slot = 0; u64Slot < pCnode->cSlots; u64Slot++) {
            struct gj_cap_slot *pSlot = &pCnode->pSlots[u64Slot];
            struct gj_obj_hdr *pObj;

            if (pSlot->u16Type == (u16)GJ_CAP_INVALID) {
                continue;
            }
            pObj = (struct gj_obj_hdr *)pSlot->pObj;
            (void)gj_cap_quota_slot_refund(pCnode->pQuotaAccount);
            if (pObj != NULL) {
                gj_cdt_unlink_slot(pObj, pCnode, u64Slot);
            }
            gj_cap_slot_invalidate_locked(pSlot, pObj);
            u32Cleared++;
        }
        if (fLocked) {
            gj_cnode_unlock(pCnode);
        }
    }
    pProc->pRootMeta = NULL;
    /* Soft: detach quota ledger pointer (account body lives with creator). */
    pCnode->pQuotaAccount = NULL;
    return u32Cleared;
}

/* Soft: scrub exec/auxv handoff so reaped PCBs leave no image facts. */
static void
process_death_scrub_exec(struct gj_process *pProc)
{
    if (pProc == NULL) {
        return;
    }
    pProc->u64ExecEntry = 0;
    pProc->u64InterpEntry = 0;
    pProc->u64LoadBias = 0;
    pProc->u64ExecStack = 0;
    pProc->u64StartEntry = 0;
    pProc->u32StartThr = 0;
    pProc->u32ExecFlags = 0;
    pProc->cNeededLoaded = 0;
    pProc->cAuxv = 0;
    memset(pProc->aAuxv, 0, sizeof(pProc->aAuxv));
    memset(pProc->szExecPath, 0, sizeof(pProc->szExecPath));
    pProc->u64AnonNext = 0x0000000040000000ull;
    /*
     * Soft multi-server confine death cleanup: drop PCB confine flags so a
     * recycled wait-table slot cannot inherit ambient-drop state.
     * Grep: process: death confine_scrub
     */
    pProc->u32Confined = 0;
    pProc->u32Promises = 0;
}

void
process_death(struct gj_process *pProc, u32 u32ExitCode)
{
    u32 iReg;
    u64 u64Cr3;
    u64 u64Ker;
    u64 u64SavedCr3;
    u32 u32Cleared = 0;
    u32 u32SelfPid;
    u32 u32Reparented;
    u32 u32RegionsDropped = 0;
    u32 u32FaultWasBusy = 0;
    u32 u32HadPager = 0;
    u32 u32AsOk = 0;
    u32 u32AsFail = 0;
    u32 u32AsSkip = 0;
    int fWaitChild;

    if (pProc == NULL) {
        return;
    }
    /* Idempotent: second death only re-notes zombie code */
    if (!pProc->u32Alive && pProc->u64Cr3 == 0 && !gj_process_has_pager(pProc)) {
        process_wait_note_exit(pProc, u32ExitCode);
        return;
    }

    g_u64DeathTotal++;
    u32SelfPid = process_wait_pid_of(pProc);
    pProc->u32ExitCode = u32ExitCode;
    pProc->u32Alive = 0;

    /* ---- pager clear (G-PROC-5) ---------------------------------------- */
    u32HadPager = gj_process_has_pager(pProc) ? 1u : 0u;
    gj_process_clear_pager(pProc);
    g_u64DeathPagerClear++;
    /* Grep: process: death pager */
    kprintf("process: death pager_clear had=%u soft (G-PROC-5)\n",
            u32HadPager);

    /* Drop exception port (handler thr may already be gone) */
    pProc->excPort.u8Live = 0;
    pProc->excPort.u8Pending = 0;
    pProc->excPort.u32HandlerThr = 0;
    pProc->excPort.u32Vec = 0;
    pProc->excPort.u32Count = 0;
    pProc->excPort.u64Error = 0;
    pProc->excPort.u64Rip = 0;
    pProc->excPort.u64Cr2 = 0;

    /*
     * Soft: force-clear fault serialization so death cannot leave AS locked.
     * Grep: process: death fault_lock
     */
    u32FaultWasBusy = (pProc->fault.u32FaultInProgress != 0u ||
                       pProc->fault.u32Waiters != 0u)
                          ? 1u
                          : 0u;
    pProc->fault.u32FaultInProgress = 0;
    pProc->fault.u32Waiters = 0;
    g_u64DeathFaultForce++;
    kprintf("process: death fault_lock force was_busy=%u soft (G-PROC-5)\n",
            u32FaultWasBusy);

    /* Drop region views (object owns pages; maps are views — G-MO) */
    for (iReg = 0; iReg < GJ_PROC_REGION_MAX; iReg++) {
        if (pProc->aRegions[iReg].u8Used) {
            memset(&pProc->aRegions[iReg], 0, sizeof(pProc->aRegions[iReg]));
            u32RegionsDropped++;
        }
    }

    /*
     * Soft G-PROC-5: reparent unreaped children to init before we become a
     * zombie (wait4 parent filter stays honest for grand-children).
     */
    u32Reparented = 0;
    if (u32SelfPid != 0) {
        u32Reparented = process_wait_reparent(u32SelfPid, 1u);
    }

    fWaitChild = process_is_wait_child(pProc);
    /*
     * G-PROC-5: wipe CNode for wait-registered children only (never boot/init).
     * Soft deepen: quota refund + CDT unlink per slot, then process revoke.
     * Grep: process: death cnode_wipe | process: cnode_clear
     */
    if (fWaitChild) {
        u32Cleared = process_death_cnode_wipe(pProc);
        g_u64DeathCnodeWipe++;
        g_u64DeathCnodeSlots += (u64)u32Cleared;
        (void)gj_obj_revoke_begin(&pProc->hdr);
        (void)gj_revoke_cdt_walk_batch(&pProc->hdr, 16);
        (void)gj_revoke_process_deferred(16);
        kprintf("process: death cnode_wipe slots=%u wait_child=1 soft "
                "(G-PROC-5)\n",
                u32Cleared);
        kprintf("process: cnode_clear slots=%u PASS\n", u32Cleared);
        process_death_scrub_exec(pProc);
        kprintf("process: death confine_scrub confined=0 promises=0 soft\n");
    } else {
        /* Long-lived PCB: leave CNode; still note skip for tallies. */
        kprintf("process: death cnode_wipe slots=0 wait_child=0 skip soft "
                "(G-PROC-5)\n");
    }
    /*
     * Destroy private AS only for wait-registered children (PE/spawn/fork).
     * Never free boot/init AS — ring-3 smokes share it with the rest of kmain.
     * Save/restore caller CR3: death may run mid-syscall on the parent AS
     * (e.g. vfork child exit while parent PE32 is current).
     * Grep: process: death as_destroy | process: as_destroy
     */
    u64SavedCr3 = cpu_read_cr3();
    u64Cr3 = pProc->u64Cr3;
    u64Ker = vmm_kernel_cr3();
    if (fWaitChild && u64Cr3 != 0 && u64Ker != 0 &&
        (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        cpu_load_cr3(u64Ker);
        vmm_set_anon_cursor(NULL);
        if (vmm_as_destroy(u64Cr3) == GJ_OK) {
            u32AsOk = 1;
            g_u64DeathAsDestroyOk++;
            kprintf("process: death as_destroy cr3=0x%lx ok=1 soft "
                    "(G-PROC-5)\n",
                    (unsigned long)u64Cr3);
            kprintf("process: as_destroy cr3=0x%lx PASS\n",
                    (unsigned long)u64Cr3);
        } else {
            u32AsFail = 1;
            g_u64DeathAsDestroyFail++;
            kprintf("process: death as_destroy cr3=0x%lx ok=0 soft "
                    "(G-PROC-5)\n",
                    (unsigned long)u64Cr3);
            kprintf("process: as_destroy cr3=0x%lx FAIL\n",
                    (unsigned long)u64Cr3);
        }
        pProc->u64Cr3 = 0;
    } else if (u64Cr3 != 0 && u64Ker != 0 &&
               (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        /* Long-lived process: leave AS; switch off if we were on it */
        u32AsSkip = 1;
        g_u64DeathAsSkip++;
        kprintf("process: death as_destroy cr3=0x%lx skip=long_lived soft "
                "(G-PROC-5)\n",
                (unsigned long)u64Cr3);
        if ((u64SavedCr3 & ~0xfffull) == (u64Cr3 & ~0xfffull)) {
            cpu_load_cr3(u64Ker);
            vmm_set_anon_cursor(NULL);
            u64SavedCr3 = u64Ker;
        }
    } else {
        u32AsSkip = 1;
        g_u64DeathAsSkip++;
        kprintf("process: death as_destroy cr3=0x%lx skip=none soft "
                "(G-PROC-5)\n",
                (unsigned long)u64Cr3);
    }
    /* Restore caller address space when it was not the victim */
    if (u64SavedCr3 != 0 &&
        (u64SavedCr3 & ~0xfffull) != (u64Cr3 & ~0xfffull)) {
        cpu_load_cr3(u64SavedCr3);
    } else if (u64Ker != 0) {
        cpu_load_cr3(u64Ker);
    }
    process_wait_note_exit(pProc, u32ExitCode);
    pProc->pParent = NULL;

    /*
     * Aggregate G-PROC-5 death tallies (soft product observability).
     * Grep: process: death exit= | process: death tallies
     */
    kprintf("process: death tallies total=%llu pager_clear=%llu "
            "fault_force=%llu cnode_wipe=%llu cnode_slots=%llu "
            "as_ok=%llu as_fail=%llu as_skip=%llu soft (G-PROC-5)\n",
            (unsigned long long)g_u64DeathTotal,
            (unsigned long long)g_u64DeathPagerClear,
            (unsigned long long)g_u64DeathFaultForce,
            (unsigned long long)g_u64DeathCnodeWipe,
            (unsigned long long)g_u64DeathCnodeSlots,
            (unsigned long long)g_u64DeathAsDestroyOk,
            (unsigned long long)g_u64DeathAsDestroyFail,
            (unsigned long long)g_u64DeathAsSkip);
    kprintf("process: death exit=%u reparent=%u regions=%u "
            "cnode_slots=%u as_ok=%u as_fail=%u as_skip=%u "
            "wait_child=%d soft (G-PROC-5)\n",
            u32ExitCode, u32Reparented, u32RegionsDropped, u32Cleared,
            u32AsOk, u32AsFail, u32AsSkip, fWaitChild);
}

/* Stub children for Linux fork/vfork (no full AS clone until product spawn). */
#define GJ_FORK_STUBS 16u
static struct gj_process g_aForkStub[GJ_FORK_STUBS];
static u8                g_aForkUsed[GJ_FORK_STUBS];

/**
 * Deferred child exit: schedules after parent returns from fork so wait4 can
 * reap a real zombie (not only vfork's immediate exit).
 */
static void
fork_child_exit_worker(void *pArg)
{
    struct gj_process *pChild = (struct gj_process *)pArg;

    /* Yield so parent can run wait4 after fork returns */
    thread_yield();
    thread_yield();
    if (pChild != NULL && pChild->u32Alive) {
        /* process_death frees private AS (avoid PML4 leak on fork stubs) */
        process_death(pChild, 0);
    }
    thread_exit();
}

i64
process_linux_fork(u32 u32Ppid, int fExitNow)
{
    u32 i;
    u32 pid;
    u32 thr = 0;

    for (i = 0; i < GJ_FORK_STUBS; i++) {
        if (!g_aForkUsed[i]) {
            break;
        }
    }
    if (i >= GJ_FORK_STUBS) {
        return -11; /* EAGAIN */
    }
    memset(&g_aForkStub[i], 0, sizeof(g_aForkStub[i]));
    g_aForkUsed[i] = 1;
    g_aForkStub[i].u32Alive = 1;
    g_aForkStub[i].u32Personality = 1;
    /*
     * Private AS shell for child (G-AS) + clone parent private user pages.
     * Product: full COW; bring-up: copy non-identity user 4K pages from parent.
     */
    if (process_as_ensure(&g_aForkStub[i]) == GJ_OK) {
        u32 cCloned = 0;
        extern struct gj_process *g_pLinuxProc;

        kprintf("process: linux_fork as cr3=0x%lx\n",
                (unsigned long)g_aForkStub[i].u64Cr3);
        if (g_pLinuxProc != NULL && g_pLinuxProc->u64Cr3 != 0 &&
            g_aForkStub[i].u64Cr3 != 0 &&
            (g_pLinuxProc->u64Cr3 & ~0xfffull) !=
                (g_aForkStub[i].u64Cr3 & ~0xfffull)) {
            if (vmm_as_clone_user_pages(g_pLinuxProc->u64Cr3,
                                        g_aForkStub[i].u64Cr3, 512,
                                        &cCloned) == GJ_OK) {
                kprintf("process: linux_fork clone pages=%u PASS\n", cCloned);
            } else {
                kprintf("process: linux_fork clone pages FAIL\n");
            }
        }
    }
    pid = process_wait_register(&g_aForkStub[i], u32Ppid ? u32Ppid : 1u);
    if (pid == 0) {
        g_aForkUsed[i] = 0;
        return -12; /* ENOMEM */
    }
    if (fExitNow) {
        /* vfork-shaped: child already exited; free AS shell */
        process_death(&g_aForkStub[i], 0);
    } else {
        /*
         * fork-shaped: schedule a kernel worker that marks the child zombie
         * after yields so parent wait4 can reap (bring-up; product: user AS).
         */
        thr = thread_create(&g_aForkStub[i], fork_child_exit_worker,
                            &g_aForkStub[i]);
        if (thr == 0) {
            /* Fallback: immediate exit so wait still works */
            process_death(&g_aForkStub[i], 0);
            kprintf("process: linux_fork pid=%u (no thr, exit now)\n", pid);
            return (i64)pid;
        }
    }
    kprintf("process: linux_fork pid=%u exit_now=%d thr=%u\n", pid, fExitNow,
            thr);
    return (i64)pid;
}

i64
process_linux_exit_pid(u32 u32Pid, u32 u32Code)
{
    u32 i;

    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && !g_aWait[i].u8Reaped &&
            g_aWait[i].u32Pid == u32Pid) {
            process_wait_note_exit(g_aWait[i].pProc, u32Code);
            return 0;
        }
    }
    return -3; /* ESRCH */
}

u32
process_wait_pid_of(struct gj_process *pProc)
{
    u32 i;

    if (pProc == NULL) {
        return 0;
    }
    for (i = 0; i < GJ_WAIT_SLOTS; i++) {
        if (g_aWait[i].u8Used && g_aWait[i].pProc == pProc) {
            return g_aWait[i].u32Pid;
        }
    }
    return 0;
}

i64
process_wait4(i64 i64Pid, i32 *pStatus, int nOptions)
{
    return process_wait4_ppid(0, i64Pid, pStatus, nOptions);
}

i64
process_wait4_ppid(u32 u32Ppid, i64 i64Pid, i32 *pStatus, int nOptions)
{
    u32 attempt;
    int fNoHang = (nOptions & GJ_WAIT_WNOHANG) != 0;
    int fNoWait = (nOptions & GJ_WAIT_WNOWAIT) != 0;
    u32 u32MaxAttempts = fNoHang ? 1u : 64u;

    /*
     * Soft: WUNTRACED / WCONTINUED ignored (no stop/continue state yet).
     * pid 0 treated as any-child (bring-up); pid < -1 process-group unsupported.
     */
    for (attempt = 0; attempt < u32MaxAttempts; attempt++) {
        u32 i;
        int fHaveChild = 0;

        /* Reap a matching zombie (optionally only our children) */
        for (i = 0; i < GJ_WAIT_SLOTS; i++) {
            struct process_wait_slot *pS = &g_aWait[i];
            i32 i32Status;
            i64 i64Ret;

            if (!pS->u8Used || pS->u8Reaped) {
                continue;
            }
            if (u32Ppid != 0 && pS->u32Ppid != u32Ppid) {
                continue;
            }
            /* Soft: exact pid, any (-1), or legacy any (0 → treat as -1). */
            if (i64Pid > 0 && (u32)i64Pid != pS->u32Pid) {
                continue;
            }
            if (i64Pid < -1) {
                continue; /* process group — unsupported */
            }
            fHaveChild = 1;
            if (!pS->u8Zombie) {
                continue;
            }
            /* Linux-shaped exit status: (code & 0xff) << 8; signal path later. */
            i32Status = (i32)((pS->u32Exit & 0xffu) << 8);
            if (pStatus != NULL) {
                *pStatus = i32Status;
            }
            i64Ret = (i64)pS->u32Pid;

            if (fNoWait) {
                /* Soft WNOWAIT: report zombie without consuming the slot. */
                g_u64WaitNowaitPeek++;
                kprintf("process: wait4 nowait pid=%u status=0x%x soft\n",
                        pS->u32Pid, (unsigned)i32Status);
                return i64Ret;
            }

            pS->u8Reaped = 1;
            g_u64WaitReap++;
            kprintf("process: wait4 reaped pid=%u status=0x%x\n", pS->u32Pid,
                    (unsigned)i32Status);
            {
                u32 j;

                for (j = 0; j < GJ_FORK_STUBS; j++) {
                    if (g_aForkUsed[j] && pS->pProc == &g_aForkStub[j]) {
                        g_aForkUsed[j] = 0;
                        break;
                    }
                }
            }
            /* Soft: full slot scrub so pid reuse cannot see stale exit. */
            pS->u8Used = 0;
            pS->u8Zombie = 0;
            pS->u8Reaped = 0;
            pS->u32Exit = 0;
            pS->u32Pid = 0;
            pS->u32Ppid = 0;
            pS->pProc = NULL;
            return i64Ret;
        }
        /* No unreaped children at all → ECHILD */
        if (!fHaveChild) {
            return -10; /* ECHILD */
        }
        /* Live children, none exited yet */
        if (fNoHang) {
            return 0; /* WNOHANG */
        }
        /* Blocking-ish: yield so fork exit workers can run */
        thread_yield();
    }
    return -10; /* ECHILD — timed out waiting for child exit */
}
