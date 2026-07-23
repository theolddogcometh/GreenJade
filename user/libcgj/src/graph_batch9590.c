/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9590: git soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9590(void);
 *     - Returns the compile-time graph batch number for this TU (9590).
 *   uint32_t __gj_batch_id_9590  (alias)
 *   __libcgj_batch9590_marker = "libcgj-batch9590"
 *
 * Exclusive continuum CREATE-ONLY (9581-9590: git soft id stubs —
 * git_repo_ok_u_9581, git_index_ok_u_9582, git_commit_ok_u_9583,
 * git_branch_ok_u_9584, git_remote_ok_u_9585, git_diff_ok_u_9586,
 * git_merge_ok_u_9587, git_status_ok_u_9588, git_ready_u_9589,
 * batch_id_9590). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9590_marker[] = "libcgj-batch9590";

#define B9590_BATCH_ID  9590u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9590_id(void)
{
	return B9590_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9590 - report this TU's graph batch number.
 *
 * Always returns 9590.
 */
uint32_t
gj_batch_id_9590(void)
{
	(void)NULL;
	return b9590_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9590(void)
    __attribute__((alias("gj_batch_id_9590")));
