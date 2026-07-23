/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Ring-3 trampoline + freestanding personality server map (G-PERS).
 */
#pragma once

#include <gj/process.h>

/**
 * Map Linux-shaped ring-3 smoke trampoline into pProc AS.
 * Code @ 0x0100_0000, stack top @ 0x0110_0000. Returns 0 on success.
 */
int  user_task_map_ring3(struct gj_process *pProc);
void user_task_enter_ring3(void); /* does not return on success */

/**
 * Map freestanding protonrt-user door server into process AS (G-PERS).
 * Entry @ 0x0120_0000, stack top @ 0x0130_0000.
 * Sets NATIVE personality (0) for IPC_* syscalls. Returns 0 on success.
 */
int user_personality_map(struct gj_process *pProc);

/** VA of mapped personality entry / stack top (0 if not mapped). */
u64 user_personality_entry(void);
u64 user_personality_stack(void);
