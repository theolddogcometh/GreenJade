/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2193: product shell command budget soft hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_cmd_budget_hint(void);
 *     - Soft recommended max shell command line length in bytes for
 *       product greenjade-shell style tooling (8192). Pure-data budget
 *       hint; not ARG_MAX and not a kernel query.
 *   uint32_t __gj_shell_cmd_budget_hint  (alias)
 *   __libcgj_batch2193_marker = "libcgj-batch2193"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Unique
 * gj_shell_cmd_budget_hint surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2193_marker[] = "libcgj-batch2193";

/* Soft product shell command-line budget (bytes). */
#define B2193_CMD_BUDGET  8192u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2193_budget(void)
{
	return B2193_CMD_BUDGET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_cmd_budget_hint - soft shell command-line byte budget.
 *
 * Always returns 8192. Product-side planning constant for command
 * buffers; does not call getconf or libc.
 */
uint32_t
gj_shell_cmd_budget_hint(void)
{
	(void)NULL;
	return b2193_budget();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_cmd_budget_hint(void)
    __attribute__((alias("gj_shell_cmd_budget_hint")));
