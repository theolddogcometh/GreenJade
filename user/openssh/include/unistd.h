/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <unistd.h> for OpenSSH-portable. Host POSIX: glibc.
 * DUT: hide libcgj strmode(mode_t) — HAVE_STRMODE is unset, so
 * openbsd-compat declares strmode(int). Hide 5-arg prctl so OpenSSH
 * can call the glibc-shaped varargs prctl(PR_SET_DUMPABLE, 0).
 */
#pragma once
#if !__STDC_HOSTED__
# define strmode __gj_libcgj_strmode_decl
# define prctl __gj_libcgj_prctl_decl
#endif
#include_next <unistd.h>
#if !__STDC_HOSTED__
# undef strmode
# undef prctl
int prctl(int nOption, ...);
#endif
