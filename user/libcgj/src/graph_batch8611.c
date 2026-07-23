/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8611: stack guard id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_stack_guard_id_8611(void);
 *     - Return soft stack-protector guard catalog id (0xdeadbeef).
 *   uint32_t __gj_stack_guard_id_8611  (alias)
 *   __libcgj_batch8611_marker = "libcgj-batch8611"
 *
 * Exclusive continuum CREATE-ONLY (8611-8620: stack/canary id stubs —
 * stack_guard_id, stack_canary_ok_u, stack_align_u, stack_grow_down_id,
 * stack_page_id, ssp_ready_u, ssp_fail_id, frame_ok_u, ra_align_u,
 * batch_id_8620). Unique gj_stack_guard_id_8611 surface only; no
 * multi-def. Catalog id only; does not touch __stack_chk_guard.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8611_marker[] = "libcgj-batch8611";

/* Soft classic stack-guard canary catalog constant (32-bit). */
#define B8611_STACK_GUARD  ((uint32_t)0xdeadbeefu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8611_stack_guard_id(void)
{
	return B8611_STACK_GUARD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_guard_id_8611 - soft stack-protector guard catalog id.
 *
 * Always returns 0xdeadbeef. Catalog id only; does not read or write
 * __stack_chk_guard. No parent wires.
 */
uint32_t
gj_stack_guard_id_8611(void)
{
	(void)NULL;
	return b8611_stack_guard_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stack_guard_id_8611(void)
    __attribute__((alias("gj_stack_guard_id_8611")));
