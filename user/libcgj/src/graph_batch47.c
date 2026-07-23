/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47: fortify/chk surface audit gate.
 *
 * Clean-room fortify wrappers (bound-check then call base libc) were
 * requested for the following symbols. nm -D on product libc.so.6 shows
 * every one already exported from earlier TU's — skip redefinition to
 * avoid multiple-definition link failures:
 *
 *   __readlinkat_chk, __getgroups_chk, __gethostname_chk,
 *   __getdomainname_chk, __getlogin_r_chk, __confstr_chk, __wctomb_chk,
 *   __mbsnrtowcs_chk, __wcsnrtombs_chk, __recv_chk, __recvfrom_chk,
 *   __read_chk, __pread_chk, __getwd_chk
 *
 * Sources (already present): fortify_chk.c, graph_batch15/16/17/20/23.c.
 * Dyn smoke gate (return 64) asserts the surface remains exported.
 */
#include <stddef.h>

const char __libcgj_batch47_marker[] = "libcgj-batch47";
