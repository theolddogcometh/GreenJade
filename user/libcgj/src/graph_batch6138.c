/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6138: PipeWire port direction mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pw_port_dir_mask_6138(uint32_t has_input,
 *                                     uint32_t has_output);
 *     - Build a soft port-direction capability mask:
 *         bit0 = input present (has_input nonzero)
 *         bit1 = output present (has_output nonzero)
 *       Other bits are zero. Soft pure-data only.
 *   uint32_t __gj_pw_port_dir_mask_6138  (alias)
 *   __libcgj_batch6138_marker = "libcgj-batch6138"
 *
 * Exclusive continuum CREATE-ONLY (6131-6140: pipewire node graph
 * stubs). Unique gj_pw_port_dir_mask_6138 surface only; no multi-def.
 * Distinct from port pack (batch6132). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6138_marker[] = "libcgj-batch6138";

#define B6138_DIR_IN   1u
#define B6138_DIR_OUT  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6138_port_dir_mask(uint32_t uHasIn, uint32_t uHasOut)
{
	uint32_t uMask;

	uMask = 0u;
	if (uHasIn != 0u) {
		uMask |= B6138_DIR_IN;
	}
	if (uHasOut != 0u) {
		uMask |= B6138_DIR_OUT;
	}
	return uMask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pw_port_dir_mask_6138 - soft port direction capability mask.
 *
 * has_input / has_output: nonzero means that direction is present.
 * Returns mask with bit0=input, bit1=output. Soft pure-data stub.
 * No parent wires.
 */
uint32_t
gj_pw_port_dir_mask_6138(uint32_t uHasInput, uint32_t uHasOutput)
{
	(void)NULL;
	return b6138_port_dir_mask(uHasInput, uHasOutput);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pw_port_dir_mask_6138(uint32_t uHasInput, uint32_t uHasOutput)
    __attribute__((alias("gj_pw_port_dir_mask_6138")));
