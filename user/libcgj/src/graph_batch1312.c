/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1312: Monotonic task-id allocator (caller-owned counter).
 *
 * Surface (unique symbols):
 *   uint64_t gj_task_id_next(uint64_t *id);
 *     — Increment *id by one and return the new value. NULL → 0.
 *       Refuse wrap at UINT64_MAX (leave unchanged, return 0).
 *       Id 0 is reserved as invalid (see batch1313).
 *   uint64_t __gj_task_id_next  (alias)
 *   __libcgj_batch1312_marker = "libcgj-batch1312"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1312_marker[] = "libcgj-batch1312";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_task_id_next — allocate the next task id from a caller-owned counter.
 *
 * Pre-increment style: *id is the last issued id (start at 0 so first
 * issued id is 1). Returns 0 on NULL or when *id is already UINT64_MAX
 * (no wrap to 0, which would collide with the invalid-id convention).
 */
uint64_t
gj_task_id_next(uint64_t *pId)
{
	uint64_t uCur;

	if (pId == NULL) {
		return 0u;
	}
	uCur = *pId;
	if (uCur == 0xffffffffffffffffull) {
		return 0u;
	}
	uCur++;
	*pId = uCur;
	return uCur;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_task_id_next(uint64_t *pId)
    __attribute__((alias("gj_task_id_next")));

