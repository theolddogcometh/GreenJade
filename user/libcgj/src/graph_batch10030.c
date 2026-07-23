/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10030: mesa/radv soft id stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10030(void);
 *     - Returns the compile-time graph batch number for this TU (10030).
 *   uint32_t __gj_batch_id_10030  (alias)
 *   __libcgj_batch10030_marker = "libcgj-batch10030"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_batch_id_10030 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_batch_id_10000 / prior batch_id_* symbols - avoid
 * multi-def. Closes the 10021-10030 continuum (mesa/radv/amdvlk/anv/
 * lavapipe/zink/iris/v3d soft ok stubs + mesa soft ready). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10030_marker[] = "libcgj-batch10030";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10030_id(void)
{
	return 10030u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10030 - report this TU's graph batch number.
 *
 * Always returns 10030. Soft pure-data identity for the mesa/radv soft
 * id stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_10030(void)
{
	(void)NULL;
	return b10030_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10030(void)
    __attribute__((alias("gj_batch_id_10030")));
