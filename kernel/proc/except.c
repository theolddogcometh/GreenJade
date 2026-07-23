/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception port delivery (Apple-shaped / SEH-shaped path for Proton A0–A1).
 * User faults: soft single-slot pending record + wake exception handler if
 * registered; else caller kills. PCB port ≠ pager (Apple §12).
 *
 * Soft deepen — greppable: EXCEPT_PORT_SOFT
 *   register: thr!=0 bind live; thr==0 unregister (main pre-create thr=0 OK)
 *   deliver:  fail closed if !live / dead proc; overwrite slot; wake HANDLER
 *   take:     soft claim pending; resume path separate (EXCEPT_TAG_FAULT)
 * One-slot port (product: queue / SEH chain). Pure C freestanding.
 */
#include <gj/except.h>
#include <gj/klog.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>

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

static int
except_port_proc_ok(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    /* Dead PCB: refuse new binds/delivers (process_death clears live too). */
    if (pProc->u32Alive == 0) {
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
    __atomic_store_n(&pProc->excPort.u8Pending, 0, __ATOMIC_RELEASE);
}

static void
except_port_soft_wake_handlers(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_WAKE_HANDLER */
    (void)thread_wake(&pProc->excPort, EXCEPT_TAG_HANDLER, EXCEPT_SOFT_WAKE_MAX);
}

/* ---- public API --------------------------------------------------------- */

void
except_port_init(struct gj_process *pProc)
{
    if (pProc == NULL) {
        return;
    }
    memset(&pProc->excPort, 0, sizeof(pProc->excPort));
}

gj_status_t
except_port_register(struct gj_process *pProc, u32 u32ThrId)
{
    /* greppable: EXCEPT_PORT_REGISTER_SOFT */
    if (pProc == NULL) {
        return GJ_ERR_INVAL;
    }

    /*
     * thr==0: soft unregister. Allows bring-up call sites that pass 0
     * before the handler thr exists (winesrv A0), then rebind later.
     */
    if (u32ThrId == 0) {
        __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
        pProc->excPort.u32HandlerThr = 0;
        except_port_clear_slot(pProc);
        /* Wake any handler waiters so wait() can observe !live. */
        except_port_soft_wake_handlers(pProc);
        kprintf("except: port unregister soft proc=%p\n", (void *)pProc);
        return GJ_OK;
    }

    if (!except_port_proc_ok(pProc)) {
        return GJ_ERR_DEAD;
    }

    pProc->excPort.u32HandlerThr = u32ThrId;
    /*
     * Soft rebind: drop stale pending so a new handler does not consume a
     * fault recorded for a prior sink (SEH identity change).
     */
    except_port_clear_slot(pProc);
    __atomic_store_n(&pProc->excPort.u8Live, 1, __ATOMIC_RELEASE);
    kprintf("except: port register thr=%u proc=%p\n", u32ThrId, (void *)pProc);
    return GJ_OK;
}

gj_status_t
except_port_unregister(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_UNREGISTER_SOFT */
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
    if (pProc == NULL) {
        return 0;
    }
    if (!except_port_live_load(pProc)) {
        return 0;
    }
    /* Fail closed on dead process even if live bit stale. */
    if (pProc->u32Alive == 0) {
        __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
        return 0;
    }

    /*
     * Single-slot soft coalesce: install newest record, then publish pending.
     * Field writes precede pending RELEASE so take never samples a half-slot.
     * Prior pending is overwritten; sticky pad[0] marks drop for smoke/stats.
     */
    u8WasPending =
        __atomic_load_n(&pProc->excPort.u8Pending, __ATOMIC_ACQUIRE);
    pProc->excPort.u32Vec = u32Vec;
    pProc->excPort.u64Error = u64Err;
    pProc->excPort.u64Rip = u64Rip;
    pProc->excPort.u64Cr2 = u64Cr2;
    if (u8WasPending != 0) {
        /* greppable: EXCEPT_PORT_DELIVER_OVERWRITE */
        pProc->excPort.u8Pad[0] = 1;
    }
    u32Count = __atomic_add_fetch(&pProc->excPort.u32Count, 1u,
                                  __ATOMIC_ACQ_REL);
    __atomic_store_n(&pProc->excPort.u8Pending, 1, __ATOMIC_RELEASE);

    kprintf("except: deliver vec=%u rip=0x%lx cr2=0x%lx count=%u%s\n",
            u32Vec, (unsigned long)u64Rip, (unsigned long)u64Cr2, u32Count,
            u8WasPending ? " overwrite" : "");

    except_port_soft_wake_handlers(pProc);
    return 1;
}

int
except_port_take(struct gj_process *pProc, struct gj_except_record *pOut)
{
    u8 u8Expected;

    if (pProc == NULL || pOut == NULL) {
        return -1;
    }

    /* Soft claim: only one take wins if concurrent. */
    u8Expected = 1;
    if (!__atomic_compare_exchange_n(&pProc->excPort.u8Pending, &u8Expected, 0,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        return 0;
    }

    pOut->u32Vec = pProc->excPort.u32Vec;
    pOut->u32Pad = 0;
    pOut->u64Error = pProc->excPort.u64Error;
    pOut->u64Rip = pProc->excPort.u64Rip;
    pOut->u64Cr2 = pProc->excPort.u64Cr2;

    /* Slot fields left as last-written snapshot; pending already 0. */
    return 1;
}

int
except_port_drop(struct gj_process *pProc)
{
    u8 u8Expected;

    if (pProc == NULL) {
        return -1;
    }
    u8Expected = 1;
    if (!__atomic_compare_exchange_n(&pProc->excPort.u8Pending, &u8Expected, 0,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        return 0;
    }
    return 1;
}

void
except_port_wait(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_WAIT_SOFT */
    if (pProc == NULL) {
        return;
    }
    for (;;) {
        if (except_port_pending_load(pProc)) {
            return;
        }
        if (!except_port_live_load(pProc)) {
            /* Unregister / death: do not sleep forever. */
            return;
        }
        thread_block(&pProc->excPort, EXCEPT_TAG_HANDLER);
        /*
         * Soft race: deliver may post pending after the check and before
         * BLOCKED. Re-sample; self-wake so a lost pulse is not stuck.
         */
        if (except_port_pending_load(pProc) || !except_port_live_load(pProc)) {
            except_port_soft_wake_handlers(pProc);
        }
        schedule();
    }
}

u32
except_port_resume_fault(struct gj_process *pProc, u32 u32Max)
{
    /* greppable: EXCEPT_PORT_RESUME_FAULT */
    if (pProc == NULL) {
        return 0;
    }
    if (u32Max == 0) {
        u32Max = 1;
    }
    return thread_wake(&pProc->excPort, EXCEPT_TAG_FAULT, u32Max);
}

int
except_port_is_live(const struct gj_process *pProc)
{
    return except_port_live_load(pProc);
}

int
except_port_has_pending(const struct gj_process *pProc)
{
    return except_port_pending_load(pProc);
}

u32
except_port_handler(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    return pProc->excPort.u32HandlerThr;
}

u32
except_port_count(const struct gj_process *pProc)
{
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

    if (pProc == NULL) {
        return -1;
    }

    /* --- init + null-safe queries --- */
    except_port_init(pProc);
    if (except_port_is_live(pProc) || except_port_has_pending(pProc) ||
        except_port_handler(pProc) != 0 || except_port_count(pProc) != 0) {
        kprintf("except: smoke init state FAIL\n");
        return -1;
    }
    if (except_port_register(NULL, u32Thr) != GJ_ERR_INVAL) {
        kprintf("except: smoke null register FAIL\n");
        return -1;
    }
    if (except_port_deliver(NULL, 0, 0, 0, 0) != 0) {
        kprintf("except: smoke null deliver FAIL\n");
        return -1;
    }
    if (except_port_take(pProc, NULL) != -1 ||
        except_port_take(NULL, &rec) != -1) {
        kprintf("except: smoke take inval FAIL\n");
        return -1;
    }

    /* --- soft unregister thr=0 before bind (winesrv-shaped) --- */
    st = except_port_register(pProc, 0);
    if (st != GJ_OK || except_port_is_live(pProc)) {
        kprintf("except: smoke thr0 unregister FAIL\n");
        return -1;
    }
    if (except_port_deliver(pProc, 0, 0, 0x1ull, 0) != 0) {
        kprintf("except: smoke deliver !live FAIL\n");
        return -1;
    }

    /* --- register soft bind --- */
    if (except_port_register(pProc, u32Thr) != GJ_OK) {
        kprintf("except: smoke register FAIL\n");
        return -1;
    }
    if (!except_port_is_live(pProc) ||
        except_port_handler(pProc) != u32Thr) {
        kprintf("except: smoke live/handler FAIL\n");
        return -1;
    }

    /* --- deliver #DE-shaped --- */
    if (!except_port_deliver(pProc, 0 /* #DE */, 0, 0x400000ull, 0)) {
        kprintf("except: smoke deliver FAIL\n");
        return -1;
    }
    if (!except_port_has_pending(pProc)) {
        kprintf("except: smoke pending FAIL\n");
        return -1;
    }

    /* --- overwrite soft: second deliver wins slot, count advances --- */
    u32Count0 = except_port_count(pProc);
    if (!except_port_deliver(pProc, 14 /* #PF */, 0x3ull, 0x400010ull,
                             0xdeadbeefull)) {
        kprintf("except: smoke overwrite deliver FAIL\n");
        return -1;
    }
    if (except_port_count(pProc) != u32Count0 + 1u) {
        kprintf("except: smoke overwrite count FAIL\n");
        return -1;
    }
    if (pProc->excPort.u8Pad[0] == 0) {
        kprintf("except: smoke overwrite pad FAIL\n");
        return -1;
    }

    memset(&rec, 0, sizeof(rec));
    if (except_port_take(pProc, &rec) != 1 || rec.u32Vec != 14 ||
        rec.u64Rip != 0x400010ull || rec.u64Cr2 != 0xdeadbeefull ||
        rec.u64Error != 0x3ull) {
        kprintf("except: smoke take FAIL\n");
        return -1;
    }
    if (except_port_has_pending(pProc) || except_port_take(pProc, &rec) != 0) {
        kprintf("except: smoke double-take FAIL\n");
        return -1;
    }

    /* --- drop path --- */
    if (!except_port_deliver(pProc, 6 /* #UD */, 0, 0x500000ull, 0)) {
        kprintf("except: smoke drop-setup FAIL\n");
        return -1;
    }
    if (except_port_drop(pProc) != 1 || except_port_drop(pProc) != 0) {
        kprintf("except: smoke drop FAIL\n");
        return -1;
    }

    /* --- rebind soft clears pending --- */
    if (!except_port_deliver(pProc, 3 /* #BP */, 0, 0x600000ull, 0)) {
        kprintf("except: smoke rebind-setup FAIL\n");
        return -1;
    }
    if (except_port_register(pProc, 2) != GJ_OK ||
        except_port_has_pending(pProc) ||
        except_port_handler(pProc) != 2) {
        kprintf("except: smoke rebind FAIL\n");
        return -1;
    }

    /* --- unregister + deliver fail closed --- */
    if (except_port_unregister(pProc) != GJ_OK ||
        except_port_is_live(pProc)) {
        kprintf("except: smoke unregister FAIL\n");
        return -1;
    }
    if (except_port_deliver(pProc, 0, 0, 0, 0) != 0) {
        kprintf("except: smoke post-unreg deliver FAIL\n");
        return -1;
    }

    /* restore a live port so later bring-up can re-use the PCB */
    if (except_port_register(pProc, u32Thr) != GJ_OK) {
        kprintf("except: smoke restore FAIL\n");
        return -1;
    }

    kprintf("except: port smoke PASS count=%u soft=EXCEPT_PORT_SOFT\n",
            except_port_count(pProc));
    return 0;
}
