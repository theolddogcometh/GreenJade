/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9233: SQLite SQLITE_BUSY result id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sqlite_busy_id_9233(void);
 *     - Return soft SQLITE_BUSY result code id (5).
 *   uint32_t __gj_sqlite_busy_id_9233  (alias)
 *   __libcgj_batch9233_marker = "libcgj-batch9233"
 *
 * Exclusive continuum CREATE-ONLY (9231-9240: sqlite soft id stubs —
 * ok_id, error_id, busy_id, row_id, done_id, open_ready_u,
 * exec_ready_u, stmt_ok_u, ready_u, batch_id_9240). Unique
 * gj_sqlite_busy_id_9233 surface only; no multi-def. Catalog id only;
 * no live SQLite I/O. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9233_marker[] = "libcgj-batch9233";

/* Soft SQLITE_BUSY database file locked result code. */
#define B9233_SQLITE_BUSY  ((uint32_t)5u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9233_sqlite_busy_id(void)
{
	return B9233_SQLITE_BUSY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sqlite_busy_id_9233 - soft SQLITE_BUSY result code id.
 *
 * Always returns 5. Catalog id only; does not wait on locks or invoke
 * busy handlers. No parent wires.
 */
uint32_t
gj_sqlite_busy_id_9233(void)
{
	(void)NULL;
	return b9233_sqlite_busy_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sqlite_busy_id_9233(void)
    __attribute__((alias("gj_sqlite_busy_id_9233")));
