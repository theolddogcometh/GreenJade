/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8613: stack align stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_stack_align_u_8613(void);
 *     - Return soft System V AMD64 stack alignment requirement (16).
 *   uint32_t __gj_stack_align_u_8613  (alias)
 *   __libcgj_batch8613_marker = "libcgj-batch8613"
 *
 * Exclusive continuum CREATE-ONLY (8611-8620: stack/canary id stubs —
 * stack_guard_id, stack_canary_ok_u, stack_align_u, stack_grow_down_id,
 * stack_page_id, ssp_ready_u, ssp_fail_id, frame_ok_u, ra_align_u,
 * batch_id_8620). Unique gj_stack_align_u_8613 surface only; no
 * multi-def. Catalog constant only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8613_marker[] = "libcgj-batch8613";

/* Soft SysV AMD64 / x86-64 ABI stack alignment (bytes). */
#define B8613_STACK_ALIGN  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8613_stack_align(void)
{
	return B8613_STACK_ALIGN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_align_u_8613 - soft stack alignment catalog constant.
 *
 * Always returns 16. Catalog value only; does not inspect SP. No parent
 * wires.
 */
uint32_t
gj_stack_align_u_8613(void)
{
	(void)NULL;
	return b8613_stack_align();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stack_align_u_8613(void)
    __attribute__((alias("gj_stack_align_u_8613")));
