/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SYSCALL MSRs + ring3 helpers (Option C entry).
 */
#pragma once

#include <gj/types.h>

void cpu_syscall_init(void);

/** True after MSRs programmed for SYSCALL/SYSRET. */
int cpu_syscall_ready(void);

/**
 * Enter ring 3 at u64Entry with stack u64Stack (must be mapped user-accessible).
 * Does not return on success. Stub returns if not ready / invalid.
 */
void cpu_enter_user(u64 u64Entry, u64 u64Stack) __attribute__((noreturn));

/**
 * Enter 32-bit compat ring-3 via iretq (CS32 L=0 D=1, SS=user DS).
 * Does not return on success. Used for PE32/WoW64 hardware enter smoke.
 */
void cpu_enter_user32(u64 u64Entry, u64 u64Stack) __attribute__((noreturn));
