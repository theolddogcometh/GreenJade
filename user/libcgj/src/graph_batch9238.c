/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9238: SQLite statement result validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sqlite_stmt_ok_u_9238(uint32_t rc);
 *     - Return 1 if rc is a known soft result code in this continuum
 *       catalog, else 0.
 *   uint32_t __gj_sqlite_stmt_ok_u_9238  (alias)
 *   __libcgj_batch9238_marker = "libcgj-batch9238"
 *
 * Exclusive continuum CREATE-ONLY (9231-9240: sqlite soft id stubs —
 * ok_id, error_id, busy_id, row_id, done_id, open_ready_u,
 * exec_ready_u, stmt_ok_u, ready_u, batch_id_9240). Unique
 * gj_sqlite_stmt_ok_u_9238 surface only; no multi-def. Soft catalog
 * check only; no live SQLite statement I/O. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9238_marker[] = "libcgj-batch9238";

/*
 * Soft SQLite result catalog for this continuum:
 *   SQLITE_OK (0) | SQLITE_ERROR (1) | SQLITE_BUSY (5) |
 *   SQLITE_ROW (100) | SQLITE_DONE (101)
 */
#define B9238_SQLITE_OK     ((uint32_t)0u)
#define B9238_SQLITE_ERROR  ((uint32_t)1u)
#define B9238_SQLITE_BUSY   ((uint32_t)5u)
#define B9238_SQLITE_ROW    ((uint32_t)100u)
#define B9238_SQLITE_DONE   ((uint32_t)101u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9238_sqlite_stmt_ok(uint32_t u32Rc)
{
	if (u32Rc == B9238_SQLITE_OK)
		return 1u;
	if (u32Rc == B9238_SQLITE_ERROR)
		return 1u;
	if (u32Rc == B9238_SQLITE_BUSY)
		return 1u;
	if (u32Rc == B9238_SQLITE_ROW)
		return 1u;
	if (u32Rc == B9238_SQLITE_DONE)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sqlite_stmt_ok_u_9238 - 1 if rc is a known soft catalog result.
 *
 * rc: soft SQLite result / step return code
 *
 * Returns 1 when rc is in {0, 1, 5, 100, 101}, else 0. Soft catalog
 * check only; does not step or finalize statements. No parent wires.
 */
uint32_t
gj_sqlite_stmt_ok_u_9238(uint32_t u32Rc)
{
	(void)NULL;
	return b9238_sqlite_stmt_ok(u32Rc);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sqlite_stmt_ok_u_9238(uint32_t u32Rc)
    __attribute__((alias("gj_sqlite_stmt_ok_u_9238")));
