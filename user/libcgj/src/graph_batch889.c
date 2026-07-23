/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch889: sequence-counter end (writer side).
 *
 * Completes a seqlock write section: odd seq → even. Pair with
 * gj_seqcnt_begin (batch888).
 *
 * Surface (unique symbols):
 *   void gj_seqcnt_end(uint32_t *seq);
 *     — If *seq is odd, increment to even. Already even → leave alone.
 *       NULL → no-op.
 *   void __gj_seqcnt_end  (alias)
 *   __libcgj_batch889_marker = "libcgj-batch889"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch889_marker[] = "libcgj-batch889";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_seqcnt_end — leave a write section (odd → even).
 *
 * Makes the sequence counter even again so readers observing an even
 * value may treat the protected data as stable. Plain sequential.
 */
void
gj_seqcnt_end(uint32_t *seq)
{
	uint32_t s;

	if (seq == NULL) {
		return;
	}

	s = *seq;
	if ((s & 1u) != 0u) {
		*seq = s + 1u;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_seqcnt_end(uint32_t *seq)
    __attribute__((alias("gj_seqcnt_end")));
