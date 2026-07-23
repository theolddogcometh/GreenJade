/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Cold-path Linux syscall IPC to personality (libprotonrt).
 * Option C: HOT in kernel; COLD via doors (product) / sync service /
 * legacy queue (GJ_SYS_COLD_DEQUEUE/REPLY). Pure C11 dual-license.
 *
 * Soft deepen surfaces (no redesign):
 *   greppable: GJ_COLD_MODE_ / cold_ipc_register_service / cold_ipc_stats
 *   - Mode flags control doors/service/queue preference (soft product path)
 *   - Service + queue consumer registration with soft generation cookies
 *   - Product counters (wrap OK; diagnostics / smoke)
 */
#pragma once

#include <gj/linux_abi.h>
#include <gj/types.h>

#define GJ_COLD_QUEUE_DEPTH 32

/*
 * Soft doors / path mode flags (product control).
 * greppable: GJ_COLD_MODE_
 *
 * cold_ipc_set_doors_mode(f) remains ABI-stable: toggles DOORS bit only.
 * Full mask via cold_ipc_set_mode_flags / or / and.
 */
#define GJ_COLD_MODE_DOORS           (1u << 0) /* allow door_call path */
#define GJ_COLD_MODE_SERVICE         (1u << 1) /* allow sync service pfn */
#define GJ_COLD_MODE_QUEUE           (1u << 2) /* allow legacy queue */
#define GJ_COLD_MODE_SERVICE_FIRST   (1u << 3) /* bring-up: service before doors */
#define GJ_COLD_MODE_DOORS_FIRST     (1u << 4) /* product: doors before service */
#define GJ_COLD_MODE_REQUIRE_SERVER  (1u << 5) /* doors only if pServer live */

/**
 * Default bring-up soft mask: all paths + service-first + require server.
 * Avoids door_call hang when a stale endpoint has no server.
 */
#define GJ_COLD_MODE_DEFAULT                                                   \
    (GJ_COLD_MODE_DOORS | GJ_COLD_MODE_SERVICE | GJ_COLD_MODE_QUEUE |           \
     GJ_COLD_MODE_SERVICE_FIRST | GJ_COLD_MODE_REQUIRE_SERVER)

enum gj_cold_state {
    GJ_COLD_FREE    = 0,
    GJ_COLD_PENDING = 1,
    GJ_COLD_DONE    = 2,
    /** Soft: dequeued by personality, await COLD_REPLY (not yet DONE). */
    GJ_COLD_CLAIMED = 3,
};

struct gj_cold_request {
    u64                  u64Id;
    u32                  u32State;
    u32                  u32Pad;
    struct gj_linux_regs regs; /* in: args; out: i64Ret when DONE */
};

/**
 * Product counters + soft registration snapshot (wrap OK).
 * greppable: cold_ipc_stats
 */
struct gj_cold_ipc_stats {
    u64 u64Submits;
    u64 u64ServiceHits;
    u64 u64DoorHits;
    u64 u64QueueHits;
    u64 u64Enosys;
    u64 u64Inval;
    u64 u64QueueFull;     /* submit_queue → -EAGAIN */
    u64 u64Dequeues;      /* successful soft claim */
    u64 u64DequeueEmpty;
    u64 u64Replies;       /* successful reply */
    u64 u64ReplyMiss;     /* id not PENDING/CLAIMED */
    u64 u64ServiceLocal;
    u64 u64RegService;    /* soft service register events */
    u64 u64UnregService;
    u64 u64RegQueue;      /* soft queue-consumer register */
    u64 u64UnregQueue;
    u64 u64ModeChanges;
    u32 u32ModeFlags;     /* live soft mode mask */
    u32 u32ServiceGen;    /* live soft service gen (0 = unbound) */
    u32 u32QueueGen;      /* live soft queue-consumer gen (0 = none) */
    u32 u32Pending;       /* PENDING + CLAIMED slot count snapshot */
    u32 u32Attached;      /* 1 if personality attached soft */
    u32 u32ServiceBound;  /* 1 if service pfn non-NULL */
    u32 u32DoorsEnabled;  /* 1 if MODE_DOORS bit set */
    u32 u32Pad;
};

void cold_ipc_init(void);

/**
 * Submit cold syscall. Product order honours soft mode flags:
 *   SERVICE_FIRST (default bring-up): service → doors → queue
 *   DOORS_FIRST: doors → service → queue
 * Paths gated by MODE_SERVICE / MODE_DOORS / MODE_QUEUE.
 * MODE_REQUIRE_SERVER: doors only when pServer live (default).
 * u64TimeoutNsec reserved (0 = default).
 */
i64 cold_ipc_submit(struct gj_linux_regs *pRegs, u64 u64TimeoutNsec);

/**
 * Personality: soft-claim next PENDING into *pOut (state → CLAIMED).
 * Returns 1 if claimed, 0 if empty / null.
 */
int cold_ipc_dequeue(struct gj_cold_request *pOut);

/**
 * Personality: complete request u64Id (PENDING or CLAIMED) with i64Ret.
 * Soft: wakes submit_queue waiter on DONE.
 */
int cold_ipc_reply(u64 u64Id, i64 i64Ret);

/** True if a consumer has registered (queue gen / attach) or service bound. */
void cold_ipc_set_personality_attached(int fAttached);
int  cold_ipc_personality_attached(void);

/**
 * Register in-process cold service (libprotonrt early / tests).
 * Soft: sets personality attached when pfn non-NULL; bumps service gen.
 * pfn NULL clears the service binding (soft unbind; gen → 0).
 */
void cold_ipc_set_service(i64 (*pfn)(struct gj_linux_regs *pRegs, void *pCtx),
                          void *pCtx);

/**
 * Soft service registration with generation cookie.
 * Returns non-zero gen on bind; 0 if pfn is NULL (clears binding).
 * greppable: cold_ipc_register_service
 */
u32 cold_ipc_register_service(i64 (*pfn)(struct gj_linux_regs *pRegs, void *pCtx),
                              void *pCtx);

/**
 * Soft unbind: clear service only if u32Gen matches live gen.
 * Returns 1 if cleared, 0 on soft miss / unbound.
 */
int cold_ipc_unregister_service(u32 u32Gen);

/** Non-zero when a sync service pfn is bound. */
int cold_ipc_service_registered(void);

/** Soft query: current service ctx (NULL if unbound). */
void *cold_ipc_service_ctx(void);

/** Live soft service generation (0 = unbound). */
u32 cold_ipc_service_gen(void);

/**
 * Soft queue-consumer registration (legacy COLD_DEQUEUE path).
 * Marks personality attached and returns non-zero gen.
 * greppable: cold_ipc_register_queue_consumer
 */
u32 cold_ipc_register_queue_consumer(void);

/**
 * Soft drop queue consumer when gen matches.
 * Returns 1 if cleared, 0 on soft miss.
 * Does not clear an independent sync service binding.
 */
int cold_ipc_unregister_queue_consumer(u32 u32Gen);

/** Live soft queue-consumer gen (0 = none registered via soft path). */
u32 cold_ipc_queue_gen(void);

/** Soft count of PENDING + CLAIMED slots. */
u32 cold_ipc_queue_pending(void);

/** Compile-time queue depth (GJ_COLD_QUEUE_DEPTH). */
u32 cold_ipc_queue_depth(void);

/**
 * Soft doors enable toggle (ABI-stable).
 * fEnable != 0 → OR MODE_DOORS; fEnable == 0 → clear MODE_DOORS.
 * Other soft flags preserved.
 */
void cold_ipc_set_doors_mode(int fEnable);

/** Soft replace full mode mask (0 still legal — all paths off → ENOSYS). */
void cold_ipc_set_mode_flags(u32 u32Flags);

/** Soft OR bits into mode mask. */
void cold_ipc_or_mode_flags(u32 u32Bits);

/** Soft AND mask (clear bits with ~bits via caller). */
void cold_ipc_and_mode_flags(u32 u32Bits);

/** Live soft mode flags. */
u32  cold_ipc_get_mode_flags(void);

/** Non-zero if MODE_DOORS is set. */
int  cold_ipc_doors_enabled(void);

/** Snapshot product counters + soft registration state. NULL pOut is no-op. */
void cold_ipc_stats_get(struct gj_cold_ipc_stats *pOut);

/** Zero counters only; soft registration / mode / queue slots preserved. */
void cold_ipc_stats_reset(void);

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
