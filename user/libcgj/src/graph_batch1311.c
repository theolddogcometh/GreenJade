/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1311: Work-stealing victim worker index (round-robin skip self).
 *
 * Surface (unique symbols):
 *   size_t gj_work_stealing_victim_idx(size_t thief, size_t nworkers,
 *                                    size_t attempt);
 *     — Victim = (thief + 1 + attempt) mod nworkers. If that equals
 *       thief (only when nworkers < 2), returns nworkers (invalid).
 *       nworkers == 0 → 0. thief >= nworkers → nworkers (invalid).
 *   size_t __gj_work_stealing_victim_idx  (alias)
 *   __libcgj_batch1311_marker = "libcgj-batch1311"
 *
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */
#include <stddef.h>

const char __libcgj_batch1311_marker[] = "libcgj-batch1311";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_work_stealing_victim_idx — pick a victim worker for steal attempt.
 *
 * Starts at the next worker after thief and walks around the ring by
 * attempt. Returns an index in [0, nworkers) on success. Returns nworkers
 * as an invalid sentinel when nworkers < 2 (no distinct victim) or thief
 * is out of range. nworkers == 0 returns 0.
 */
size_t
gj_work_stealing_victim_idx(size_t uThief, size_t cWorkers, size_t uAttempt)
{
	size_t uVic;

	if (cWorkers == 0u) {
		return 0u;
	}
	if (uThief >= cWorkers || cWorkers < 2u) {
		return cWorkers;
	}

	/* Offset by at least one so the first attempt is not self. */
	uVic = uThief + 1u + uAttempt;
	uVic = uVic % cWorkers;
	if (uVic == uThief) {
		/* Only reachable if cWorkers == 1; already guarded, belt-and-braces. */
		return cWorkers;
	}
	return uVic;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_work_stealing_victim_idx(size_t uThief, size_t cWorkers,
                                    size_t uAttempt)
    __attribute__((alias("gj_work_stealing_victim_idx")));

