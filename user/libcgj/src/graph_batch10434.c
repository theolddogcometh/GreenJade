/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10434: IM soft compose ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_compose_ok_u_10434(void);
 *     - Returns 0 (soft stub: X11/compose sequence table slot is
 *       soft/unprobed; not a runtime Compose file or XIM probe).
 *   uint32_t __gj_compose_ok_u_10434  (alias)
 *   __libcgj_batch10434_marker = "libcgj-batch10434"
 *
 * Exclusive continuum CREATE-ONLY (10431-10440: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→10440). Unique
 * gj_compose_ok_u_10434 surface only; no multi-def. Distinct from
 * sibling IM soft ok_u stubs in this wave. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10434_marker[] = "libcgj-batch10434";

/* Soft-stub compose ok value (unprobed). */
#define B10434_COMPOSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10434_compose_ok(void)
{
	return B10434_COMPOSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_compose_ok_u_10434 - soft compose ok stub.
 *
 * Always returns 0 (soft/unprobed compose slot). Does not call libc or
 * read Compose files. No parent wires.
 */
uint32_t
gj_compose_ok_u_10434(void)
{
	(void)NULL;
	return b10434_compose_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_compose_ok_u_10434(void)
    __attribute__((alias("gj_compose_ok_u_10434")));
