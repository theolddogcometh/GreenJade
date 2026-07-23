/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cold-path Linux syscall IPC to personality (libprotonrt).
 * Option C: HOT in kernel; COLD via doors (product) / sync service /
 * legacy queue (GJ_SYS_COLD_DEQUEUE/REPLY). Pure C11 dual-license.
 */
#pragma once

#include <gj/linux_abi.h>
#include <gj/types.h>

#define GJ_COLD_QUEUE_DEPTH 32

enum gj_cold_state {
    GJ_COLD_FREE    = 0,
    GJ_COLD_PENDING = 1,
    GJ_COLD_DONE    = 2,
};

struct gj_cold_request {
    u64                  u64Id;
    u32                  u32State;
    u32                  u32Pad;
    struct gj_linux_regs regs; /* in: args; out: i64Ret when DONE */
};

void cold_ipc_init(void);

/**
 * Submit cold syscall. Product order:
 *   1) doors (server present) → door_call
 *   2) registered sync service (libprotonrt bring-up)
 *   3) doors block until server
 *   4) legacy queue if personality attached (COLD_DEQUEUE/REPLY)
 *   else -ENOSYS
 * u64TimeoutNsec reserved (0 = default).
 */
i64 cold_ipc_submit(struct gj_linux_regs *pRegs, u64 u64TimeoutNsec);

/** Personality: copy next PENDING into *pOut; returns 1 if got one, 0 if empty. */
int cold_ipc_dequeue(struct gj_cold_request *pOut);

/** Personality: complete request u64Id with i64Ret. */
int cold_ipc_reply(u64 u64Id, i64 i64Ret);

/** True if a consumer has registered (dequeue expected) or service bound. */
void cold_ipc_set_personality_attached(int fAttached);
int  cold_ipc_personality_attached(void);

/**
 * Register in-process cold service (libprotonrt early / tests).
 * Sets personality attached when pfn non-NULL.
 */
void cold_ipc_set_service(i64 (*pfn)(struct gj_linux_regs *pRegs, void *pCtx),
                          void *pCtx);
void cold_ipc_set_doors_mode(int fEnable);

/** Kernel thread entry: door_recv loop + service + door_reply (bring-up). */
void cold_personality_server(void *pArg);

/**
 * Run registered cold service on *pRegs (sets i64Ret). Used by userspace
 * personality via GJ_SYS_PERSONALITY_SERVE while policy still kernel-side.
 */
i64 cold_ipc_service_local(struct gj_linux_regs *pRegs);

/* Native GJ_SYS numbers for personality (also in syscall.h) */
#define GJ_SYS_COLD_DEQUEUE 80
#define GJ_SYS_COLD_REPLY   81
