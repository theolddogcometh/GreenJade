/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9581: git repo ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_git_repo_ok_u_9581(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       git repository / .git validity probe.
 *   uint32_t __gj_git_repo_ok_u_9581  (alias)
 *   __libcgj_batch9581_marker = "libcgj-batch9581"
 *
 * Exclusive continuum CREATE-ONLY (9581-9590: git soft id stubs —
 * git_repo_ok_u_9581, git_index_ok_u_9582, git_commit_ok_u_9583,
 * git_branch_ok_u_9584, git_remote_ok_u_9585, git_diff_ok_u_9586,
 * git_merge_ok_u_9587, git_status_ok_u_9588, git_ready_u_9589,
 * batch_id_9590). Unique surface only; no multi-def. No parent wires.
 * No __int128. No git implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9581_marker[] = "libcgj-batch9581";

/* Soft git repo-ok lamp: always off (not a real repository probe). */
#define B9581_GIT_REPO_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9581_git_repo_ok(void)
{
	return B9581_GIT_REPO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_git_repo_ok_u_9581 - git repository ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not touch a git repo or
 * call libc/git. No parent wires.
 */
uint32_t
gj_git_repo_ok_u_9581(void)
{
	(void)NULL;
	return b9581_git_repo_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_git_repo_ok_u_9581(void)
    __attribute__((alias("gj_git_repo_ok_u_9581")));
