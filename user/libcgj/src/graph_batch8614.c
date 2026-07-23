/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8614: stack grow-down id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_stack_grow_down_id_8614(void);
 *     - Return 1 (soft catalog: process stack grows downward).
 *   uint32_t __gj_stack_grow_down_id_8614  (alias)
 *   __libcgj_batch8614_marker = "libcgj-batch8614"
 *
 * Exclusive continuum CREATE-ONLY (8611-8620: stack/canary id stubs —
 * stack_guard_id, stack_canary_ok_u, stack_align_u, stack_grow_down_id,
 * stack_page_id, ssp_ready_u, ssp_fail_id, frame_ok_u, ra_align_u,
 * batch_id_8620). Unique gj_stack_grow_down_id_8614 surface only; no
 * multi-def. Always 1 on this target family. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8614_marker[] = "libcgj-batch8614";

/* Soft catalog: stack grows toward lower addresses on this target. */
#define B8614_GROW_DOWN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8614_stack_grow_down_id(void)
{
	return B8614_GROW_DOWN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_grow_down_id_8614 - soft stack growth direction id.
 *
 * Always returns 1 (grows down). Catalog id only; does not probe SP.
 * No parent wires.
 */
uint32_t
gj_stack_grow_down_id_8614(void)
{
	(void)NULL;
	return b8614_stack_grow_down_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stack_grow_down_id_8614(void)
    __attribute__((alias("gj_stack_grow_down_id_8614")));
