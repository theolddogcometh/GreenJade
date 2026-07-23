/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception port (SEH/signal-shaped delivery for Proton game tasks).
 * One pending record per process; product path may grow a queue.
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

struct gj_process;

struct gj_except_record {
    u32 u32Vec;
    u32 u32Pad;
    u64 u64Error;
    u64 u64Rip;
    u64 u64Cr2;
};

void except_port_init(struct gj_process *pProc);
gj_status_t except_port_register(struct gj_process *pProc, u32 u32ThrId);
/** Queue fault record + wake handler. Returns 1 if port live, else 0. */
int except_port_deliver(struct gj_process *pProc, u32 u32Vec, u64 u64Err,
                        u64 u64Rip, u64 u64Cr2);
/** Take pending record into *pOut. Returns 1 if taken, 0 if none, -1 on error. */
int except_port_take(struct gj_process *pProc, struct gj_except_record *pOut);
void except_port_wait(struct gj_process *pProc);
/** Smoke: register, deliver #DE-shaped, take record. Returns 0 on PASS. */
int except_port_smoke(struct gj_process *pProc);
