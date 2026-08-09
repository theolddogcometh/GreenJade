/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception port delivery (Apple-shaped / SEH-shaped path for Proton A0-A1).
 * User faults: soft single-slot pending record + wake exception handler if
 * registered; else caller kills. PCB port != pager (Apple §12).
 *
 * Soft residual - greppable: EXCEPT_PORT_SOFT
 *   register: thr!=0 bind live; thr==0 unregister (main pre-create thr=0 OK)
 *   deliver:  fail closed if !live / dead proc; overwrite slot; wake HANDLER
 *   take:     soft claim pending; resume path separate (EXCEPT_TAG_FAULT)
 * One-slot port (product: queue / SEH chain). Pure C freestanding.
 *
 * C3 residual (security/isolation - ASSURANCE_LITE C3; Soft!=product):
 *   - deliver: dual-belt u32Alive re-sample (post-install pre-publish + post-
 *     publish unpublish); hang-close wake HANDLER on dead; greppable
 *     del_dead / del_dead_race / EXCEPT_PORT_C3
 *   - register: post-bind alive recheck (unbind if death raced)
 *   - take:    claim-then-discard if !alive (no fault record leak on dead PCB)
 *   - resume:  refuse wake FAULT thr when !alive (H3-adjacent AS isolation)
 *   - wait:    exit on u32Alive==0 as well as !live (no hang; C3 fail-closed)
 *   - clear:   pad overwrite sticky cleared with slot (hygiene)
 *   - smoke:   dead deliver / take / resume fail-closed residual
 * Never hard-gates product SEH; dual MIT OR Apache-2.0; no GPL; G-AC-1.
 *
 * Soft inventory (sparse lamps only - NO stamp storms, no version stamp):
 *   - Rollup + register/deliver/take-drop/wait residual + path + lean + PASS
 *   greppable: "except: soft ..."
 *   Never hard-gates; wrap OK. Soft != product · Soft != SEH product.
 *   Dual license: MIT OR Apache-2.0 (SPDX above).
 */
#include <gj/except.h>
#include <gj/klog.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>

/* ---- lean soft residual inventory (greppable "except: soft ...") ---------- */

/*
 * Sparse lamp surface count (inventory / register / deliver / take_drop /
 * wait / path / residual_lean / PASS / c3_death). Not a version stamp.
 */
#define EXCEPT_SOFT_AREAS 9u
/*
 * Cap full multi-line soft inventory dumps (Soft!=product; no stamp storms).
 * once-path + smoke stay greppable; further calls silent. H2 residual.
 */
#define EXCEPT_SOFT_LOG_CAP 4u
/* C3: soft death residual belt active (greppable EXCEPT_PORT_C3). */
#define EXCEPT_PORT_C3 1

/*
 * Cumulative path tallies (diagnostics only; wrap OK). Not per-PCB.
 * greppable: except: soft ...
 */
static u32 g_u32SoftInit;          /* except_port_init entries */
static u32 g_u32SoftRegEnter;      /* register entries (incl thr0) */
static u32 g_u32SoftRegBind;       /* thr!=0 live bind OK */
static u32 g_u32SoftRegThr0;       /* thr==0 soft unregister */
static u32 g_u32SoftRegDead;       /* bind refused (dead PCB; entry or post-bind) */
static u32 g_u32SoftRegNull;       /* register pProc == NULL */
static u32 g_u32SoftRegRebind;     /* thr!=0 while already live */
static u32 g_u32SoftRegDeadRace;   /* C3: alive fell after bind, before return */
static u32 g_u32SoftUnreg;         /* except_port_unregister entries */
static u32 g_u32SoftDelEnter;      /* deliver entries */
static u32 g_u32SoftDelOk;         /* posted pending + wake */
static u32 g_u32SoftDelNlive;      /* !live fail closed */
static u32 g_u32SoftDelDead;       /* dead PCB fail closed (entry or C3 recheck) */
static u32 g_u32SoftDelNull;       /* deliver pProc == NULL */
static u32 g_u32SoftDelOverwrite;  /* single-slot coalesce (was pending) */
static u32 g_u32SoftDelDeadRace;   /* C3: alive fell after install (pre/post publish) */
static u32 g_u32SoftTakeEnter;     /* take entries */
static u32 g_u32SoftTakeHit;       /* claim pending -> record */
static u32 g_u32SoftTakeEmpty;     /* no pending */
static u32 g_u32SoftTakeInval;     /* null args */
static u32 g_u32SoftTakeDead;      /* C3: claim discarded because !alive */
static u32 g_u32SoftDropEnter;     /* drop entries */
static u32 g_u32SoftDropHit;       /* cleared pending */
static u32 g_u32SoftDropEmpty;     /* no pending */
static u32 g_u32SoftDropInval;     /* null pProc */
static u32 g_u32SoftWaitEnter;     /* wait entries */
static u32 g_u32SoftWaitPending;   /* return because pending */
static u32 g_u32SoftWaitNlive;     /* return because !live */
static u32 g_u32SoftWaitDead;      /* C3: return because u32Alive==0 */
static u32 g_u32SoftWaitBlock;     /* thread_block calls */
static u32 g_u32SoftWaitNull;      /* wait pProc == NULL */
static u32 g_u32SoftWaitRace;      /* post-block re-sample self-wake */
static u32 g_u32SoftResumeEnter;   /* resume_fault entries */
static u32 g_u32SoftResumeWake;    /* sum of thr woken (capped per call) */
static u32 g_u32SoftResumeNull;    /* resume pProc == NULL */
static u32 g_u32SoftResumeDefMax;  /* u32Max was 0 -> defaulted to 1 */
static u32 g_u32SoftResumeDead;    /* C3: resume refused (!alive) */
static u32 g_u32SoftQueryLive;     /* is_live queries */
static u32 g_u32SoftQueryPend;     /* has_pending queries */
static u32 g_u32SoftQueryHandler;  /* except_port_handler queries */
static u32 g_u32SoftQueryCount;    /* except_port_count queries */
static u32 g_u32SoftWakeCall;      /* except_port_soft_wake_handlers */
static u32 g_u32SoftSmokeEnter;    /* except_port_smoke entries */
static u32 g_u32SoftSmokePass;     /* smoke PASS */
static u32 g_u32SoftSmokeFail;     /* smoke FAIL early outs */
static u32 g_u32SoftLogN;          /* soft inventory log emissions */
static u8  g_fSoftInvOnce;         /* one-shot dump after activity */

static void except_soft_inc(u32 *pCtr);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
except_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Sparse soft exception-port inventory lamps (smoke / once).
 * NO stamp storms, no version stamp: rollup + residual + path + lean + PASS.
 * Never hard-gates; diagnostics only. Soft != product · Soft != SEH product.
 * greppable: except: soft
 */
static void
soft_inventory_log(void)
{
    /*
     * Cap multi-line inventory dumps. Past EXCEPT_SOFT_LOG_CAP stay silent.
     * Soft!=product; no stamp storms under register/deliver churn.
     */
    if (g_u32SoftLogN >= EXCEPT_SOFT_LOG_CAP) {
        return;
    }
    except_soft_inc(&g_u32SoftLogN);

    /* Grep: except: soft inventory - single rollup lamp */
    kprintf("except: soft inventory logs=%u wake_max=%u slot=1 "
            "tags=HANDLER+FAULT soft=EXCEPT_PORT_SOFT areas=%u "
            "log_cap=%u (sparse; Soft!=product)\n",
            g_u32SoftLogN, EXCEPT_SOFT_WAKE_MAX, (unsigned)EXCEPT_SOFT_AREAS,
            (unsigned)EXCEPT_SOFT_LOG_CAP);

    /* Grep: except: soft register */
    kprintf("except: soft register enter=%u bind=%u thr0=%u dead=%u "
            "null=%u unreg=%u init=%u rebind=%u dead_race=%u\n",
            g_u32SoftRegEnter, g_u32SoftRegBind, g_u32SoftRegThr0,
            g_u32SoftRegDead, g_u32SoftRegNull, g_u32SoftUnreg,
            g_u32SoftInit, g_u32SoftRegRebind, g_u32SoftRegDeadRace);

    /* Grep: except: soft deliver */
    kprintf("except: soft deliver enter=%u ok=%u nlive=%u dead=%u "
            "null=%u overwrite=%u dead_race=%u\n",
            g_u32SoftDelEnter, g_u32SoftDelOk, g_u32SoftDelNlive,
            g_u32SoftDelDead, g_u32SoftDelNull, g_u32SoftDelOverwrite,
            g_u32SoftDelDeadRace);

    /* Grep: except: soft take_drop - claim + decline residual (one line) */
    kprintf("except: soft take_drop take_enter=%u take_hit=%u take_empty=%u "
            "take_inval=%u take_dead=%u drop_enter=%u drop_hit=%u "
            "drop_empty=%u drop_inval=%u\n",
            g_u32SoftTakeEnter, g_u32SoftTakeHit, g_u32SoftTakeEmpty,
            g_u32SoftTakeInval, g_u32SoftTakeDead, g_u32SoftDropEnter,
            g_u32SoftDropHit, g_u32SoftDropEmpty, g_u32SoftDropInval);

    /* Grep: except: soft wait - park/resume residual (one line) */
    kprintf("except: soft wait enter=%u pending=%u nlive=%u dead=%u block=%u "
            "null=%u wake_call=%u race=%u resume_enter=%u resume_wake=%u "
            "resume_null=%u resume_dead=%u def_max=%u "
            "query_live=%u query_pend=%u query_handler=%u query_count=%u "
            "smoke_enter=%u smoke_pass=%u smoke_fail=%u\n",
            g_u32SoftWaitEnter, g_u32SoftWaitPending, g_u32SoftWaitNlive,
            g_u32SoftWaitDead, g_u32SoftWaitBlock, g_u32SoftWaitNull,
            g_u32SoftWakeCall, g_u32SoftWaitRace, g_u32SoftResumeEnter,
            g_u32SoftResumeWake, g_u32SoftResumeNull, g_u32SoftResumeDead,
            g_u32SoftResumeDefMax, g_u32SoftQueryLive, g_u32SoftQueryPend,
            g_u32SoftQueryHandler, g_u32SoftQueryCount, g_u32SoftSmokeEnter,
            g_u32SoftSmokePass, g_u32SoftSmokeFail);

    /*
     * Grep: except: soft path
     * Honesty: one-slot soft port != product SEH queue / dual-license product.
     */
    kprintf("except: soft path one_slot=1 coalesce_overwrite=1 "
            "fail_closed=1 thr0_unreg=1 c3_death=1 c3=%u tags=HANDLER+FAULT "
            "wake_max=%u soft_ne_product=1 soft_ne_seh=1 G-AC-1=1 "
            "(soft inventory; not SEH product; dual MIT OR Apache-2.0)\n",
            (unsigned)EXCEPT_PORT_C3, EXCEPT_SOFT_WAKE_MAX);

    /*
     * Grep: except: soft residual lean
     * Lean residual honesty - Soft!=product dual license; no version stamp.
     * C3: del_dead_race + wait_dead + take_dead + resume_dead fail-closed.
     */
    kprintf("except: soft residual lean "
            "bind=%u del_ok=%u take=%u drop=%u overwrite=%u thr0=%u "
            "rebind=%u race=%u wait_dead=%u del_dead_race=%u take_dead=%u "
            "resume_dead=%u reg_dead_race=%u logs=%u "
            "areas=%u soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; "
            "no version stamp; not SEH product; C3 death residual)\n",
            g_u32SoftRegBind, g_u32SoftDelOk, g_u32SoftTakeHit,
            g_u32SoftDropHit, g_u32SoftDelOverwrite, g_u32SoftRegThr0,
            g_u32SoftRegRebind, g_u32SoftWaitRace, g_u32SoftWaitDead,
            g_u32SoftDelDeadRace, g_u32SoftTakeDead, g_u32SoftResumeDead,
            g_u32SoftRegDeadRace, g_u32SoftLogN,
            (unsigned)EXCEPT_SOFT_AREAS);

    /* Grep: except: soft PASS (inventory lamp; never hard-gates product) */
    if (g_u32SoftRegBind != 0u || g_u32SoftDelOk != 0u ||
        g_u32SoftTakeHit != 0u) {
        kprintf("except: soft PASS logs=%u bind=%u del=%u take=%u "
                "areas=%u (sparse; Soft!=product)\n",
                g_u32SoftLogN, g_u32SoftRegBind, g_u32SoftDelOk,
                g_u32SoftTakeHit, (unsigned)EXCEPT_SOFT_AREAS);
    } else {
        kprintf("except: soft PARTIAL logs=%u areas=%u "
                "(no bind/del/take yet; soft inventory only; Soft!=product)\n",
                g_u32SoftLogN, (unsigned)EXCEPT_SOFT_AREAS);
    }
}

/**
 * After first product register/deliver/take/drop activity, print soft
 * inventory once (mirrors door/compositor). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftRegBind == 0 && g_u32SoftDelOk == 0 &&
        g_u32SoftTakeHit == 0 && g_u32SoftDropHit == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

/* ---- soft port state helpers (PCB fields; atomics for soft races) ------- */

static int
except_port_live_load(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    return __atomic_load_n(&pProc->excPort.u8Live, __ATOMIC_ACQUIRE) != 0;
}

static int
except_port_pending_load(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    return __atomic_load_n(&pProc->excPort.u8Pending, __ATOMIC_ACQUIRE) != 0;
}

/**
 * C3: sample process liveness (ACQUIRE). process_death stores plain 0;
 * atomic load still orders observations vs pending/live RELEASE stores.
 * greppable: EXCEPT_PORT_C3
 */
static int
except_port_alive_load(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    return __atomic_load_n(&pProc->u32Alive, __ATOMIC_ACQUIRE) != 0;
}

static int
except_port_proc_ok(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    /* Dead PCB: refuse new binds/delivers (process_death clears live too). */
    if (!except_port_alive_load(pProc)) {
        return 0;
    }
    return 1;
}

static void
except_port_clear_slot(struct gj_process *pProc)
{
    pProc->excPort.u32Vec = 0;
    pProc->excPort.u64Error = 0;
    pProc->excPort.u64Rip = 0;
    pProc->excPort.u64Cr2 = 0;
    /* Sticky overwrite pads cleared with slot (C3 hygiene; soft residual). */
    pProc->excPort.u8Pad[0] = 0;
    pProc->excPort.u8Pad[1] = 0;
    __atomic_store_n(&pProc->excPort.u8Pending, 0, __ATOMIC_RELEASE);
}

static void
except_port_soft_wake_handlers(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_WAKE_HANDLER */
    except_soft_inc(&g_u32SoftWakeCall);
    (void)thread_wake(&pProc->excPort, EXCEPT_TAG_HANDLER, EXCEPT_SOFT_WAKE_MAX);
}

/**
 * C3 hang-close: drop live + slot and wake HANDLER waiters so wait() exits.
 * Used on death residual paths (deliver/register). Soft!=product.
 * greppable: EXCEPT_PORT_C3 / except_port_fail_closed_dead
 */
static void
except_port_fail_closed_dead(struct gj_process *pProc)
{
    if (pProc == NULL) {
        return;
    }
    pProc->excPort.u32HandlerThr = 0;
    except_port_clear_slot(pProc);
    __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
    except_port_soft_wake_handlers(pProc);
}

/* ---- public API --------------------------------------------------------- */

void
except_port_init(struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftInit);
    if (pProc == NULL) {
        return;
    }
    memset(&pProc->excPort, 0, sizeof(pProc->excPort));
}

gj_status_t
except_port_register(struct gj_process *pProc, u32 u32ThrId)
{
    /* greppable: EXCEPT_PORT_REGISTER_SOFT */
    except_soft_inc(&g_u32SoftRegEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftRegNull);
        return GJ_ERR_INVAL;
    }

    /*
     * thr==0: soft unregister. Allows bring-up call sites that pass 0
     * before the handler thr exists (winesrv A0), then rebind later.
     */
    if (u32ThrId == 0) {
        except_soft_inc(&g_u32SoftRegThr0);
        __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
        pProc->excPort.u32HandlerThr = 0;
        except_port_clear_slot(pProc);
        /* Wake any handler waiters so wait() can observe !live. */
        except_port_soft_wake_handlers(pProc);
        kprintf("except: port unregister soft proc=%p\n", (void *)pProc);
        soft_inventory_maybe_once();
        return GJ_OK;
    }

    if (!except_port_proc_ok(pProc)) {
        except_soft_inc(&g_u32SoftRegDead);
        return GJ_ERR_DEAD;
    }

    /* Soft rebind while already live (handler identity change). */
    if (except_port_live_load(pProc)) {
        except_soft_inc(&g_u32SoftRegRebind);
    }

    pProc->excPort.u32HandlerThr = u32ThrId;
    /*
     * Soft rebind: drop stale pending so a new handler does not consume a
     * fault recorded for a prior sink (SEH identity change).
     */
    except_port_clear_slot(pProc);
    __atomic_store_n(&pProc->excPort.u8Live, 1, __ATOMIC_RELEASE);

    /*
     * C3 residual (EXCEPT_PORT_C3): process_death may clear u32Alive between
     * the entry alive check and live publish. Unbind + hang-close wake so a
     * waiter cannot observe a live port on a dead PCB.
     */
    if (!except_port_alive_load(pProc)) {
        except_soft_inc(&g_u32SoftRegDead);
        except_soft_inc(&g_u32SoftRegDeadRace);
        except_port_fail_closed_dead(pProc);
        return GJ_ERR_DEAD;
    }

    except_soft_inc(&g_u32SoftRegBind);
    kprintf("except: port register thr=%u proc=%p\n", u32ThrId, (void *)pProc);
    soft_inventory_maybe_once();
    return GJ_OK;
}

gj_status_t
except_port_unregister(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_UNREGISTER_SOFT */
    except_soft_inc(&g_u32SoftUnreg);
    if (pProc == NULL) {
        return GJ_ERR_INVAL;
    }
    return except_port_register(pProc, 0);
}

int
except_port_deliver(struct gj_process *pProc, u32 u32Vec, u64 u64Err,
                    u64 u64Rip, u64 u64Cr2)
{
    u8  u8WasPending;
    u32 u32Count;

    /* greppable: EXCEPT_PORT_DELIVER_SOFT */
    except_soft_inc(&g_u32SoftDelEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftDelNull);
        return 0;
    }
    if (!except_port_live_load(pProc)) {
        except_soft_inc(&g_u32SoftDelNlive);
        return 0;
    }
    /* Fail closed on dead process even if live bit stale. Hang-close wake. */
    if (!except_port_alive_load(pProc)) {
        except_soft_inc(&g_u32SoftDelDead);
        except_port_fail_closed_dead(pProc);
        return 0;
    }

    /*
     * Single-slot soft coalesce: install newest record, then publish pending.
     * Field writes precede pending RELEASE so take never samples a half-slot.
     * Prior pending is overwritten; sticky pad[0] marks drop for smoke/stats.
     *
     * C3 residual (EXCEPT_PORT_C3) dual-belt vs process_death:
     *   1) re-sample u32Alive after install, before publish (no half-slot)
     *   2) re-sample after publish; unpublish + hang-close if death raced
     * greppable: del_dead / del_dead_race / EXCEPT_PORT_C3
     */
    u8WasPending =
        __atomic_load_n(&pProc->excPort.u8Pending, __ATOMIC_ACQUIRE);
    pProc->excPort.u32Vec = u32Vec;
    pProc->excPort.u64Error = u64Err;
    pProc->excPort.u64Rip = u64Rip;
    pProc->excPort.u64Cr2 = u64Cr2;
    if (u8WasPending != 0) {
        /* greppable: EXCEPT_PORT_DELIVER_OVERWRITE */
        except_soft_inc(&g_u32SoftDelOverwrite);
        pProc->excPort.u8Pad[0] = 1;
    }

    /* greppable: EXCEPT_PORT_C3 / del_dead_race (belt 1: pre-publish) */
    if (!except_port_alive_load(pProc)) {
        except_soft_inc(&g_u32SoftDelDead);
        except_soft_inc(&g_u32SoftDelDeadRace);
        except_port_fail_closed_dead(pProc);
        return 0;
    }

    u32Count = __atomic_add_fetch(&pProc->excPort.u32Count, 1u,
                                  __ATOMIC_ACQ_REL);
    __atomic_store_n(&pProc->excPort.u8Pending, 1, __ATOMIC_RELEASE);

    /* greppable: EXCEPT_PORT_C3 / del_dead_race (belt 2: post-publish) */
    if (!except_port_alive_load(pProc)) {
        except_soft_inc(&g_u32SoftDelDead);
        except_soft_inc(&g_u32SoftDelDeadRace);
        except_port_fail_closed_dead(pProc);
        return 0;
    }

    except_soft_inc(&g_u32SoftDelOk);

    kprintf("except: deliver vec=%u rip=0x%lx cr2=0x%lx count=%u%s\n",
            u32Vec, (unsigned long)u64Rip, (unsigned long)u64Cr2, u32Count,
            u8WasPending ? " overwrite" : "");

    except_port_soft_wake_handlers(pProc);
    soft_inventory_maybe_once();
    return 1;
}

int
except_port_take(struct gj_process *pProc, struct gj_except_record *pOut)
{
    u8 u8Expected;

    except_soft_inc(&g_u32SoftTakeEnter);
    if (pProc == NULL || pOut == NULL) {
        except_soft_inc(&g_u32SoftTakeInval);
        return -1;
    }

    /* Soft claim: only one take wins if concurrent. */
    u8Expected = 1;
    if (!__atomic_compare_exchange_n(&pProc->excPort.u8Pending, &u8Expected, 0,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        except_soft_inc(&g_u32SoftTakeEmpty);
        return 0;
    }

    /*
     * C3 residual (EXCEPT_PORT_C3): claim-then-discard if PCB is dead.
     * Do not leak fault record (RIP/CR2) to a handler after process_death
     * races with pending publish. Pending already 0 from CAS.
     * greppable: take_dead / EXCEPT_PORT_C3
     */
    if (!except_port_alive_load(pProc)) {
        except_soft_inc(&g_u32SoftTakeDead);
        pOut->u32Vec = 0;
        pOut->u32Pad = 0;
        pOut->u64Error = 0;
        pOut->u64Rip = 0;
        pOut->u64Cr2 = 0;
        pProc->excPort.u32Vec = 0;
        pProc->excPort.u64Error = 0;
        pProc->excPort.u64Rip = 0;
        pProc->excPort.u64Cr2 = 0;
        pProc->excPort.u8Pad[0] = 0;
        pProc->excPort.u8Pad[1] = 0;
        __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
        return 0;
    }

    pOut->u32Vec = pProc->excPort.u32Vec;
    pOut->u32Pad = 0;
    pOut->u64Error = pProc->excPort.u64Error;
    pOut->u64Rip = pProc->excPort.u64Rip;
    pOut->u64Cr2 = pProc->excPort.u64Cr2;

    except_soft_inc(&g_u32SoftTakeHit);
    /* Slot fields left as last-written snapshot; pending already 0. */
    soft_inventory_maybe_once();
    return 1;
}

int
except_port_drop(struct gj_process *pProc)
{
    u8 u8Expected;

    except_soft_inc(&g_u32SoftDropEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftDropInval);
        return -1;
    }
    u8Expected = 1;
    if (!__atomic_compare_exchange_n(&pProc->excPort.u8Pending, &u8Expected, 0,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        except_soft_inc(&g_u32SoftDropEmpty);
        return 0;
    }
    except_soft_inc(&g_u32SoftDropHit);
    soft_inventory_maybe_once();
    return 1;
}

void
except_port_wait(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_WAIT_SOFT */
    except_soft_inc(&g_u32SoftWaitEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftWaitNull);
        return;
    }
    for (;;) {
        if (except_port_pending_load(pProc)) {
            /*
             * C3: pending on a dead PCB is not deliverable isolation-safe.
             * Exit wait so handler path can take-discard / exit. Soft!=product.
             */
            if (!except_port_alive_load(pProc)) {
                except_soft_inc(&g_u32SoftWaitDead);
                __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
                return;
            }
            except_soft_inc(&g_u32SoftWaitPending);
            return;
        }
        /*
         * C3 fail-closed: dead PCB must not park forever even if live bit
         * is stale (process_death clears live non-atomically; thr_exit may
         * race). greppable: wait dead / EXCEPT_PORT_C3
         */
        if (!except_port_alive_load(pProc)) {
            except_soft_inc(&g_u32SoftWaitDead);
            __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
            return;
        }
        if (!except_port_live_load(pProc)) {
            /* Unregister / death: do not sleep forever. */
            except_soft_inc(&g_u32SoftWaitNlive);
            return;
        }
        except_soft_inc(&g_u32SoftWaitBlock);
        thread_block(&pProc->excPort, EXCEPT_TAG_HANDLER);
        /*
         * Soft race: deliver may post pending after the check and before
         * BLOCKED. Re-sample; self-wake so a lost pulse is not stuck.
         * C3: also re-sample u32Alive so death cannot leave a stuck waiter.
         */
        if (!except_port_alive_load(pProc)) {
            except_soft_inc(&g_u32SoftWaitRace);
            except_soft_inc(&g_u32SoftWaitDead);
            __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
            except_port_soft_wake_handlers(pProc);
            return;
        }
        if (except_port_pending_load(pProc) || !except_port_live_load(pProc)) {
            except_soft_inc(&g_u32SoftWaitRace);
            except_port_soft_wake_handlers(pProc);
        }
        schedule();
    }
}

u32
except_port_resume_fault(struct gj_process *pProc, u32 u32Max)
{
    u32 u32Woke;

    /* greppable: EXCEPT_PORT_RESUME_FAULT */
    except_soft_inc(&g_u32SoftResumeEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftResumeNull);
        return 0;
    }
    /*
     * C3 residual (EXCEPT_PORT_C3 / H3-adjacent): do not wake FAULT thr into
     * a dead process AS. process_death thr_exit is primary; this is a soft
     * belt so resume cannot re-arm USER entry after u32Alive=0.
     * greppable: resume_dead / EXCEPT_PORT_C3
     */
    if (!except_port_alive_load(pProc)) {
        except_soft_inc(&g_u32SoftResumeDead);
        __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
        return 0;
    }
    if (u32Max == 0) {
        u32Max = 1;
        except_soft_inc(&g_u32SoftResumeDefMax);
    }
    u32Woke = thread_wake(&pProc->excPort, EXCEPT_TAG_FAULT, u32Max);
    /* Soft: accumulate wake count without saturating diagnostics. */
    if (u32Woke > 0u) {
        if (g_u32SoftResumeWake < 0xffffffffu - u32Woke) {
            g_u32SoftResumeWake += u32Woke;
        } else {
            g_u32SoftResumeWake = 0xffffffffu;
        }
    }
    return u32Woke;
}

int
except_port_is_live(const struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftQueryLive);
    return except_port_live_load(pProc);
}

int
except_port_has_pending(const struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftQueryPend);
    return except_port_pending_load(pProc);
}

u32
except_port_handler(const struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftQueryHandler);
    if (pProc == NULL) {
        return 0;
    }
    return pProc->excPort.u32HandlerThr;
}

u32
except_port_count(const struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftQueryCount);
    if (pProc == NULL) {
        return 0;
    }
    return __atomic_load_n(&pProc->excPort.u32Count, __ATOMIC_ACQUIRE);
}

int
except_port_smoke(struct gj_process *pProc)
{
    struct gj_except_record rec;
    u32                     u32Thr = 1; /* synthetic handler thr id */
    u32                     u32Count0;
    gj_status_t             st;

    except_soft_inc(&g_u32SoftSmokeEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftSmokeFail);
        return -1;
    }

    /* --- init + null-safe queries --- */
    except_port_init(pProc);
    if (except_port_is_live(pProc) || except_port_has_pending(pProc) ||
        except_port_handler(pProc) != 0 || except_port_count(pProc) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke init state FAIL\n");
        return -1;
    }
    if (except_port_register(NULL, u32Thr) != GJ_ERR_INVAL) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke null register FAIL\n");
        return -1;
    }
    if (except_port_deliver(NULL, 0, 0, 0, 0) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke null deliver FAIL\n");
        return -1;
    }
    if (except_port_take(pProc, NULL) != -1 ||
        except_port_take(NULL, &rec) != -1) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke take inval FAIL\n");
        return -1;
    }

    /* --- soft unregister thr=0 before bind (winesrv-shaped) --- */
    st = except_port_register(pProc, 0);
    if (st != GJ_OK || except_port_is_live(pProc)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke thr0 unregister FAIL\n");
        return -1;
    }
    if (except_port_deliver(pProc, 0, 0, 0x1ull, 0) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke deliver !live FAIL\n");
        return -1;
    }

    /* --- register soft bind --- */
    if (except_port_register(pProc, u32Thr) != GJ_OK) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke register FAIL\n");
        return -1;
    }
    if (!except_port_is_live(pProc) ||
        except_port_handler(pProc) != u32Thr) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke live/handler FAIL\n");
        return -1;
    }

    /* --- deliver #DE-shaped --- */
    if (!except_port_deliver(pProc, 0 /* #DE */, 0, 0x400000ull, 0)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke deliver FAIL\n");
        return -1;
    }
    if (!except_port_has_pending(pProc)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke pending FAIL\n");
        return -1;
    }

    /* --- overwrite soft: second deliver wins slot, count advances --- */
    u32Count0 = except_port_count(pProc);
    if (!except_port_deliver(pProc, 14 /* #PF */, 0x3ull, 0x400010ull,
                             0xdeadbeefull)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke overwrite deliver FAIL\n");
        return -1;
    }
    if (except_port_count(pProc) != u32Count0 + 1u) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke overwrite count FAIL\n");
        return -1;
    }
    if (pProc->excPort.u8Pad[0] == 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke overwrite pad FAIL\n");
        return -1;
    }

    memset(&rec, 0, sizeof(rec));
    if (except_port_take(pProc, &rec) != 1 || rec.u32Vec != 14 ||
        rec.u64Rip != 0x400010ull || rec.u64Cr2 != 0xdeadbeefull ||
        rec.u64Error != 0x3ull) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke take FAIL\n");
        return -1;
    }
    if (except_port_has_pending(pProc) || except_port_take(pProc, &rec) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke double-take FAIL\n");
        return -1;
    }

    /* --- drop path --- */
    if (!except_port_deliver(pProc, 6 /* #UD */, 0, 0x500000ull, 0)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke drop-setup FAIL\n");
        return -1;
    }
    if (except_port_drop(pProc) != 1 || except_port_drop(pProc) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke drop FAIL\n");
        return -1;
    }

    /* --- rebind soft clears pending --- */
    if (!except_port_deliver(pProc, 3 /* #BP */, 0, 0x600000ull, 0)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke rebind-setup FAIL\n");
        return -1;
    }
    if (except_port_register(pProc, 2) != GJ_OK ||
        except_port_has_pending(pProc) ||
        except_port_handler(pProc) != 2) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke rebind FAIL\n");
        return -1;
    }

    /* --- unregister + deliver fail closed --- */
    if (except_port_unregister(pProc) != GJ_OK ||
        except_port_is_live(pProc)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke unregister FAIL\n");
        return -1;
    }
    if (except_port_deliver(pProc, 0, 0, 0, 0) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke post-unreg deliver FAIL\n");
        return -1;
    }

    /*
     * --- C3 residual: dead PCB deliver / take / resume fail closed ---
     * Soft smoke only: flip u32Alive to 0 while port is re-bound live.
     * deliver must return 0 and leave no pending (EXCEPT_PORT_C3).
     * take must discard claimed record on dead PCB (no RIP/CR2 leak).
     * resume_fault must refuse FAULT wake on dead PCB (H3-adjacent).
     * Restores u32Alive after. Soft!=product; not process_death product.
     */
    if (except_port_register(pProc, u32Thr) != GJ_OK) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke c3 rebind FAIL\n");
        return -1;
    }
    {
        u32 u32AliveSave = pProc->u32Alive;
        u32 u32TakeDead0 = g_u32SoftTakeDead;
        u32 u32ResumeDead0 = g_u32SoftResumeDead;

        /* C3 belt: entry-path dead deliver fail-closed + hang-close. */
        pProc->u32Alive = 0;
        if (except_port_deliver(pProc, 0 /* #DE */, 0, 0x700000ull, 0) != 0 ||
            except_port_has_pending(pProc) ||
            except_port_is_live(pProc)) {
            pProc->u32Alive = u32AliveSave;
            except_soft_inc(&g_u32SoftSmokeFail);
            kprintf("except: smoke c3 dead deliver FAIL\n");
            return -1;
        }
        /* Entry-path dead (alive==0 before install) still tallies del_dead. */
        if (g_u32SoftDelDead == 0) {
            pProc->u32Alive = u32AliveSave;
            except_soft_inc(&g_u32SoftSmokeFail);
            kprintf("except: smoke c3 del_dead tally FAIL\n");
            return -1;
        }

        /* C3: resume_fault must refuse on dead PCB (no FAULT thr re-arm). */
        if (except_port_resume_fault(pProc, 1) != 0u ||
            g_u32SoftResumeDead <= u32ResumeDead0) {
            pProc->u32Alive = u32AliveSave;
            except_soft_inc(&g_u32SoftSmokeFail);
            kprintf("except: smoke c3 dead resume FAIL\n");
            return -1;
        }

        /* Re-alive + rebind, post a pending, then death + take-discard. */
        pProc->u32Alive = u32AliveSave;
        if (except_port_register(pProc, u32Thr) != GJ_OK) {
            except_soft_inc(&g_u32SoftSmokeFail);
            kprintf("except: smoke c3 take rebind FAIL\n");
            return -1;
        }
        if (!except_port_deliver(pProc, 14 /* #PF */, 0x5ull, 0x800000ull,
                                 0xcafebabeull) ||
            !except_port_has_pending(pProc)) {
            except_soft_inc(&g_u32SoftSmokeFail);
            kprintf("except: smoke c3 take setup FAIL\n");
            return -1;
        }
        pProc->u32Alive = 0;
        memset(&rec, 0, sizeof(rec));
        rec.u32Vec = 0xdeadu; /* poison: must not survive take-discard */
        rec.u64Rip = 0xbadull;
        if (except_port_take(pProc, &rec) != 0 || rec.u32Vec != 0 ||
            rec.u64Rip != 0 || except_port_has_pending(pProc) ||
            g_u32SoftTakeDead <= u32TakeDead0) {
            pProc->u32Alive = u32AliveSave;
            except_soft_inc(&g_u32SoftSmokeFail);
            kprintf("except: smoke c3 dead take FAIL\n");
            return -1;
        }

        /* C3 wait residual: dead + no pending exits without hang. */
        except_port_wait(pProc);
        if (g_u32SoftWaitDead == 0) {
            pProc->u32Alive = u32AliveSave;
            except_soft_inc(&g_u32SoftSmokeFail);
            kprintf("except: smoke c3 wait dead FAIL\n");
            return -1;
        }

        pProc->u32Alive = u32AliveSave;
    }

    /* restore a live port so later bring-up can re-use the PCB */
    if (except_port_register(pProc, u32Thr) != GJ_OK) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke restore FAIL\n");
        return -1;
    }

    /*
     * Soft inventory rollup (greppable "except: soft ...").
     * Always emit full dump at smoke end so boot logs carry the catalog.
     * Sparse lamps only - Soft!=product; no version stamp; no stamp storms.
     */
    soft_inventory_log();
    if (g_u32SoftRegBind == 0 || g_u32SoftDelOk == 0 ||
        g_u32SoftTakeHit == 0 || g_u32SoftDropHit == 0 ||
        g_u32SoftDelOverwrite == 0 || g_u32SoftRegThr0 == 0 ||
        g_u32SoftDelDead == 0 || g_u32SoftTakeDead == 0 ||
        g_u32SoftResumeDead == 0 || g_u32SoftWaitDead == 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: soft inventory tallies FAIL "
                "bind=%u del=%u take=%u drop=%u ow=%u thr0=%u dead=%u "
                "take_dead=%u resume_dead=%u wait_dead=%u\n",
                g_u32SoftRegBind, g_u32SoftDelOk, g_u32SoftTakeHit,
                g_u32SoftDropHit, g_u32SoftDelOverwrite, g_u32SoftRegThr0,
                g_u32SoftDelDead, g_u32SoftTakeDead, g_u32SoftResumeDead,
                g_u32SoftWaitDead);
        return -1;
    }
    except_soft_inc(&g_u32SoftSmokePass);
    /* Grep: except: soft smoke */
    kprintf("except: soft smoke PASS bind=%u del=%u take=%u drop=%u "
            "overwrite=%u thr0=%u rebind=%u del_dead=%u take_dead=%u "
            "resume_dead=%u wait_dead=%u logs=%u areas=%u "
            "(Soft!=product; dual MIT OR Apache-2.0; C3 death residual)\n",
            g_u32SoftRegBind, g_u32SoftDelOk, g_u32SoftTakeHit,
            g_u32SoftDropHit, g_u32SoftDelOverwrite, g_u32SoftRegThr0,
            g_u32SoftRegRebind, g_u32SoftDelDead, g_u32SoftTakeDead,
            g_u32SoftResumeDead, g_u32SoftWaitDead, g_u32SoftLogN,
            (unsigned)EXCEPT_SOFT_AREAS);

    kprintf("except: port smoke PASS count=%u soft=EXCEPT_PORT_SOFT "
            "areas=%u c3=EXCEPT_PORT_C3\n",
            except_port_count(pProc), (unsigned)EXCEPT_SOFT_AREAS);
    return 0;
}
