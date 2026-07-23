/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8615: stack page id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_stack_page_id_8615(void);
 *     - Return soft default stack page size catalog id (4096).
 *   uint32_t __gj_stack_page_id_8615  (alias)
 *   __libcgj_batch8615_marker = "libcgj-batch8615"
 *
 * Exclusive continuum CREATE-ONLY (8611-8620: stack/canary id stubs —
 * stack_guard_id, stack_canary_ok_u, stack_align_u, stack_grow_down_id,
 * stack_page_id, ssp_ready_u, ssp_fail_id, frame_ok_u, ra_align_u,
 * batch_id_8620). Unique gj_stack_page_id_8615 surface only; no
 * multi-def. Catalog constant only (does not call getpagesize/sysconf).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8615_marker[] = "libcgj-batch8615";

/* Soft default page size used for stack mapping catalog (bytes). */
#define B8615_STACK_PAGE  4096u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8615_stack_page_id(void)
{
	return B8615_STACK_PAGE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_page_id_8615 - soft stack page size catalog id.
 *
 * Always returns 4096. Catalog id only; does not call sysconf(3).
 * No parent wires.
 */
uint32_t
gj_stack_page_id_8615(void)
{
	(void)NULL;
	return b8615_stack_page_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_stack_page_id_8615(void)
    __attribute__((alias("gj_stack_page_id_8615")));
