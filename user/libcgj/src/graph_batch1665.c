/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1665: OOM score clamp.
 *
 * Surface (unique symbols):
 *   int32_t gj_oom_score_clamp(int32_t score);
 *     — Clamp a process OOM score into [0, 1000], the Linux
 *       /proc/<pid>/oom_score display range. Below 0 → 0; above
 *       1000 → 1000.
 *   int32_t __gj_oom_score_clamp  (alias)
 *   __libcgj_batch1665_marker = "libcgj-batch1665"
 *
 * Pure data — not oom_score_adj (-1000..1000). Does NOT redefine
 * prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1665_marker[] = "libcgj-batch1665";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b1665_clamp(int32_t iScore)
{
	if (iScore < 0) {
		return 0;
	}
	if (iScore > 1000) {
		return 1000;
	}
	return iScore;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oom_score_clamp — clamp OOM score into the 0..1000 display range.
 *
 * Suitable for sanitizing parsed /proc values before UI or policy
 * math. No cgroup or mm_struct access.
 */
int32_t
gj_oom_score_clamp(int32_t iScore)
{
	return b1665_clamp(iScore);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_oom_score_clamp(int32_t iScore)
    __attribute__((alias("gj_oom_score_clamp")));
