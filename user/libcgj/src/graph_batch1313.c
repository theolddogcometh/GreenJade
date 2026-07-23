/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1313: Task-id validity predicate (nonzero).
 *
 * Surface (unique symbols):
 *   int gj_task_id_is_valid(uint64_t id);
 *     — Return 1 if id != 0, else 0. Pair with gj_task_id_next.
 *   int __gj_task_id_is_valid  (alias)
 *   __libcgj_batch1313_marker = "libcgj-batch1313"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stdint.h>

const char __libcgj_batch1313_marker[] = "libcgj-batch1313";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_task_id_is_valid — 1 if id is a live task id (nonzero).
 */
int
gj_task_id_is_valid(uint64_t uId)
{
	return (uId != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_task_id_is_valid(uint64_t uId)
    __attribute__((alias("gj_task_id_is_valid")));

