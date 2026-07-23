/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5925: shell command registry lookup stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_reg_lookup_stub_5925(void);
 *     - Returns 1 (lookup surface stub present / ready). Soft product
 *       lamp for the shell command surface registry lookup path; does
 *       not perform a name lookup.
 *   uint32_t __gj_shell_cmd_reg_lookup_stub_5925  (alias)
 *   __libcgj_batch5925_marker = "libcgj-batch5925"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_reg_lookup_stub_5925 surface
 * only; no multi-def. Distinct from gj_shell_cmd_reg_slot_ok_5924.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5925_marker[] = "libcgj-batch5925";

/* Lookup path stub ready lamp. */
#define B5925_LOOKUP_STUB_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5925_lookup_stub(void)
{
	return B5925_LOOKUP_STUB_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_reg_lookup_stub_5925 - shell command registry lookup stub.
 *
 * Always returns 1 (lookup stub present). Does not resolve names and
 * does not call libc. No parent wires.
 */
uint32_t
gj_shell_cmd_reg_lookup_stub_5925(void)
{
	(void)NULL;
	return b5925_lookup_stub();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_reg_lookup_stub_5925(void)
    __attribute__((alias("gj_shell_cmd_reg_lookup_stub_5925")));
