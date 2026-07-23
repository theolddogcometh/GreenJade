/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8320: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8320(void);
 *     - Returns the compile-time graph batch number for this TU (8320).
 *   uint32_t __gj_batch_id_8320  (alias)
 *   __libcgj_batch8320_marker = "libcgj-batch8320"
 *
 * Exclusive continuum CREATE-ONLY (8311-8320: atoi/strtol digit parse
 * stubs — digit_val, xdigit_val, parse_sign, parse_base_ok,
 * parse_overflow, atoi_digit, strtol_prefix, strtoul_prefix,
 * parse_ws_skip, batch_id_8320). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8320_marker[] = "libcgj-batch8320";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8320_id(void)
{
	return 8320u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8320 - report this TU's graph batch number.
 *
 * Always returns 8320. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8320(void)
{
	(void)NULL;
	return b8320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8320(void)
    __attribute__((alias("gj_batch_id_8320")));
