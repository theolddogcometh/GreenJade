/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6866: mount_attr attr_clr soft apply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mount_attr_clr_6866(uint32_t cur, uint32_t clr);
 *     - Return cur & ~clr (soft attr_clr: clear listed attr bits).
 *   uint32_t __gj_mount_attr_clr_6866  (alias)
 *   __libcgj_batch6866_marker = "libcgj-batch6866"
 *
 * Exclusive continuum CREATE-ONLY (6861-6870: mount_setattr attr stubs).
 * Unique gj_mount_attr_clr_6866 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6866_marker[] = "libcgj-batch6866";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6866_clr(uint32_t u32Cur, uint32_t u32Clr)
{
	return u32Cur & ~u32Clr;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mount_attr_clr_6866 - soft attr_clr apply (clear bits).
 *
 * cur: soft current mount attr bitmask
 * clr: bits to clear (mirror of mount_attr.attr_clr field intent)
 *
 * Returns cur & ~clr. Soft pure-data op; does not call mount_setattr.
 */
uint32_t
gj_mount_attr_clr_6866(uint32_t u32Cur, uint32_t u32Clr)
{
	(void)NULL;
	return b6866_clr(u32Cur, u32Clr);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mount_attr_clr_6866(uint32_t u32Cur, uint32_t u32Clr)
    __attribute__((alias("gj_mount_attr_clr_6866")));
