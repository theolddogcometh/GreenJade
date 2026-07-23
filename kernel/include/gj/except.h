/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception port (SEH/signal-shaped delivery for Proton game tasks).
 * PCB-owned, separate from pager (Apple §12 / PROTON A0–A1).
 *
 * Soft v1 deepen (register/deliver) — greppable: EXCEPT_PORT_SOFT
 *   - One pending slot per process (overwrite coalesces; u32Count advances).
 *   - register thr=0 soft-unregisters; thr!=0 binds handler + live.
 *   - deliver fails closed when !live / dead process; wakes handler waiters.
 *   - Fault thr blocks on tag EXCEPT_TAG_FAULT (trap.c); handler on HANDLER.
 * Product path may grow a queue / SEH chain; this header stays soft.
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

struct gj_process;

/*
 * thread_block / thread_wake tags on &pProc->excPort.
 * greppable: EXCEPT_TAG_HANDLER · EXCEPT_TAG_FAULT
 *
 * HANDLER — exception-port waiter (except_port_wait).
 * FAULT   — faulting user thr parked after soft deliver (trap uses 1).
 */
#define EXCEPT_TAG_HANDLER 0u
#define EXCEPT_TAG_FAULT   1u

/* Soft wake budget for handler waiters per deliver/abort. */
#define EXCEPT_SOFT_WAKE_MAX 4u

struct gj_except_record {
    u32 u32Vec;
    u32 u32Pad;
    u64 u64Error;
    u64 u64Rip;
    u64 u64Cr2;
};

/** Zero PCB exception port (not live, no pending). */
void except_port_init(struct gj_process *pProc);

/**
 * Soft register / rebind exception sink.
 * greppable: EXCEPT_PORT_REGISTER_SOFT
 *   u32ThrId != 0 → live, handler thr = id (pending cleared on bind).
 *   u32ThrId == 0 → soft unregister (live clear; pending drop).
 * Returns GJ_OK or GJ_ERR_INVAL / GJ_ERR_DEAD.
 */
gj_status_t except_port_register(struct gj_process *pProc, u32 u32ThrId);

/**
 * Soft unregister (alias of register thr=0 when pProc non-NULL).
 * greppable: EXCEPT_PORT_UNREGISTER_SOFT
 */
gj_status_t except_port_unregister(struct gj_process *pProc);

/**
 * Queue fault record + wake handler waiters.
 * greppable: EXCEPT_PORT_DELIVER_SOFT
 * Returns 1 if port live and record posted, else 0 (caller kills).
 * Single-slot: prior pending is overwritten; u32Count still advances.
 */
int except_port_deliver(struct gj_process *pProc, u32 u32Vec, u64 u64Err,
                        u64 u64Rip, u64 u64Cr2);

/**
 * Take pending record into *pOut (soft CAS claim).
 * Returns 1 if taken, 0 if none, -1 on error.
 */
int except_port_take(struct gj_process *pProc, struct gj_except_record *pOut);

/**
 * Drop pending without copying (handler decline / kill path).
 * Returns 1 if a record was dropped, 0 if none, -1 on error.
 */
int except_port_drop(struct gj_process *pProc);

/**
 * Block until pending or port not live.
 * greppable: EXCEPT_PORT_WAIT_SOFT
 */
void except_port_wait(struct gj_process *pProc);

/**
 * Soft-wake fault thr(s) blocked on EXCEPT_TAG_FAULT after policy resolve.
 * greppable: EXCEPT_PORT_RESUME_FAULT
 * Returns wake count.
 */
u32 except_port_resume_fault(struct gj_process *pProc, u32 u32Max);

/** Soft queries (0 / null-safe). */
int  except_port_is_live(const struct gj_process *pProc);
int  except_port_has_pending(const struct gj_process *pProc);
u32  except_port_handler(const struct gj_process *pProc);
u32  except_port_count(const struct gj_process *pProc);

/**
 * Smoke: register/deliver soft deepen.
 * greppable: except: port smoke PASS
 * Returns 0 on PASS, -1 on FAIL.
 */
int except_port_smoke(struct gj_process *pProc);
