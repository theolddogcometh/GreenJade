/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception port delivery (Apple-shaped / SEH-shaped path for Proton A0–A1).
 * User faults: queue a single pending record + wake exception thread if
 * registered; else caller kills. One-slot port (product: queue / SEH chain).
 */
#include <gj/except.h>
#include <gj/klog.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>

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
    if (pProc == NULL || u32ThrId == 0) {
        return GJ_ERR_INVAL;
    }
    pProc->excPort.u32HandlerThr = u32ThrId;
    pProc->excPort.u8Live = 1;
    pProc->excPort.u8Pending = 0;
    kprintf("except: port register thr=%u proc=%p\n", u32ThrId, (void *)pProc);
    return GJ_OK;
}

int
except_port_deliver(struct gj_process *pProc, u32 u32Vec, u64 u64Err,
                    u64 u64Rip, u64 u64Cr2)
{
    if (pProc == NULL || !pProc->excPort.u8Live) {
        return 0;
    }
    /* Overwrite prior pending (single-slot); count still advances for smoke. */
    pProc->excPort.u32Vec = u32Vec;
    pProc->excPort.u64Error = u64Err;
    pProc->excPort.u64Rip = u64Rip;
    pProc->excPort.u64Cr2 = u64Cr2;
    pProc->excPort.u8Pending = 1;
    pProc->excPort.u32Count++;
    kprintf("except: deliver vec=%u rip=0x%lx cr2=0x%lx count=%u\n",
            u32Vec, (unsigned long)u64Rip, (unsigned long)u64Cr2,
            pProc->excPort.u32Count);
    /* Wake any thread blocked on the port object */
    (void)thread_wake(&pProc->excPort, 0, 1);
    return 1;
}

int
except_port_take(struct gj_process *pProc, struct gj_except_record *pOut)
{
    if (pProc == NULL || pOut == NULL) {
        return -1;
    }
    if (!pProc->excPort.u8Pending) {
        return 0;
    }
    pOut->u32Vec = pProc->excPort.u32Vec;
    pOut->u64Error = pProc->excPort.u64Error;
    pOut->u64Rip = pProc->excPort.u64Rip;
    pOut->u64Cr2 = pProc->excPort.u64Cr2;
    pProc->excPort.u8Pending = 0;
    return 1;
}

void
except_port_wait(struct gj_process *pProc)
{
    if (pProc == NULL) {
        return;
    }
    while (!pProc->excPort.u8Pending) {
        thread_block(&pProc->excPort, 0);
        schedule();
    }
}

int
except_port_smoke(struct gj_process *pProc)
{
    struct gj_except_record rec;
    u32 u32Thr = 1; /* synthetic handler thr id for smoke */

    if (pProc == NULL) {
        return -1;
    }
    except_port_init(pProc);
    if (except_port_register(pProc, u32Thr) != GJ_OK) {
        kprintf("except: smoke register FAIL\n");
        return -1;
    }
    if (!except_port_deliver(pProc, 0 /* #DE */, 0, 0x400000ull, 0)) {
        kprintf("except: smoke deliver FAIL\n");
        return -1;
    }
    memset(&rec, 0, sizeof(rec));
    if (except_port_take(pProc, &rec) != 1 || rec.u32Vec != 0 ||
        rec.u64Rip != 0x400000ull) {
        kprintf("except: smoke take FAIL\n");
        return -1;
    }
    if (except_port_take(pProc, &rec) != 0) {
        kprintf("except: smoke double-take FAIL\n");
        return -1;
    }
    kprintf("except: port smoke PASS count=%u\n", pProc->excPort.u32Count);
    return 0;
}
