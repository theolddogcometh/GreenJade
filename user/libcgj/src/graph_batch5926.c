/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5926: shell command registry drop/unregister stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_reg_drop_stub_5926(void);
 *     - Returns 1 (drop/unregister surface stub present / ready). Soft
 *       product lamp for the shell command surface registry drop path;
 *       does not clear a slot.
 *   uint32_t __gj_shell_cmd_reg_drop_stub_5926  (alias)
 *   __libcgj_batch5926_marker = "libcgj-batch5926"
 *
 * Exclusive continuum CREATE-ONLY (5921-5930: shell command surface
 * registry stubs). Unique gj_shell_cmd_reg_drop_stub_5926 surface only;
 * no multi-def. Distinct from gj_shell_cmd_reg_lookup_stub_5925.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5926_marker[] = "libcgj-batch5926";

/* Drop/unregister path stub ready lamp. */
#define B5926_DROP_STUB_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5926_drop_stub(void)
{
	return B5926_DROP_STUB_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_reg_drop_stub_5926 - shell command registry drop stub.
 *
 * Always returns 1 (drop stub present). Does not mutate a registry and
 * does not call libc. No parent wires.
 */
uint32_t
gj_shell_cmd_reg_drop_stub_5926(void)
{
	(void)NULL;
	return b5926_drop_stub();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_reg_drop_stub_5926(void)
    __attribute__((alias("gj_shell_cmd_reg_drop_stub_5926")));
