/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Per-CPU state; GS/KERNEL_GS point here for swapgs SYSCALL path.
 *
 * Layout contract (do not reorder the leading fields — asm offsets):
 *   +0  u64KernelRsp   kernel/syscall stack top (gs:0 in SYSCALL entry)
 *   +8  u64UserRsp
 *   +16 u64UserRip
 *   +24 u64UserRflags
 *   +32 pCurrent       scheduler current thread
 *   +40 u64Cr3
 * Percpu pool: static BSS [0..GJ_CPU_STATIC_MAX), PMM growth [STATIC_MAX..GJ_MAX_CPUS).
 */
#pragma once

#include <gj/config.h>
#include <gj/types.h>

struct gj_thread;
struct gj_process;

/* Per-CPU fallback stack (BSP before thr install; IRQs use TSS.RSP0). */
#define GJ_SYSCALL_STACK_SIZE (16u * 1024u)

struct gj_cpu {
    /* Must stay at start — asm offsets (syscall_entry.S / swapgs path). */
    u64                 u64KernelRsp;   /* +0  top of per-CPU syscall/kernel stack */
    u64                 u64UserRsp;     /* +8 */
    u64                 u64UserRip;     /* +16 */
    u64                 u64UserRflags;  /* +24 */
    struct gj_thread   *pCurrent;       /* +32 */
    u64                 u64Cr3;         /* +40 current CR3 */
    u32                 u32CpuId;
    u32                 u32Online;      /* 1 once GS published for this slot */
    u8                  aSyscallStack[GJ_SYSCALL_STACK_SIZE]
        __attribute__((aligned(16)));
};

void cpu_init_bsp(void);
/** Online secondary CPU slot (after AP reaches long mode). Idempotent. */
void cpu_init_ap(u32 u32CpuId);
struct gj_cpu *cpu_current(void);
u32  cpu_id(void);
u32  cpu_online_count(void);

/**
 * Soft inventory: published percpu for slot, or NULL if OOB / not online.
 * Safe for BSP telemetry; not a substitute for cpu_current() on the hot path.
 */
struct gj_cpu *cpu_for_id(u32 u32CpuId);

/** Soft: 1 if slot has published online percpu (static or PMM-dyn). */
int  cpu_slot_online(u32 u32CpuId);

/** Soft: count of successful PMM-backed percpu allocations (id >= STATIC_MAX). */
u32  cpu_dyn_percpu_count(void);

void cpu_set_current_thread(struct gj_thread *pThr);
struct gj_thread *cpu_current_thread(void);

void cpu_load_cr3(u64 u64Cr3);
u64  cpu_read_cr3(void);

/* MSR GS setup for swapgs */
void cpu_gs_init(struct gj_cpu *pCpu);
