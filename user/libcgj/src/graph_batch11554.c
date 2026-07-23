/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11554: controller soft Steam Input ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_input_ok_u_11554(void);
 *     - Returns 0 (soft stub: Steam Input / controller API slot is
 *       soft/unprobed; not a runtime Steamworks or steamclient probe).
 *   uint32_t __gj_steam_input_ok_u_11554  (alias)
 *   __libcgj_batch11554_marker = "libcgj-batch11554"
 *
 * Exclusive continuum CREATE-ONLY (11551-11560: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11560). Unique
 * gj_steam_input_ok_u_11554 surface only; no multi-def. Distinct from
 * sibling controller soft ok_u stubs in this wave. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11554_marker[] = "libcgj-batch11554";

/* Soft-stub Steam Input ok value (unprobed). */
#define B11554_STEAM_INPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11554_steam_input_ok(void)
{
	return B11554_STEAM_INPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_input_ok_u_11554 - soft Steam Input ok stub.
 *
 * Always returns 0 (soft/unprobed Steam Input slot). Does not call libc
 * or probe Steam/Steamworks. No parent wires.
 */
uint32_t
gj_steam_input_ok_u_11554(void)
{
	(void)NULL;
	return b11554_steam_input_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_input_ok_u_11554(void)
    __attribute__((alias("gj_steam_input_ok_u_11554")));
