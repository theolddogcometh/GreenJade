/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/bitypes.h> for OpenSSH-portable (HAVE_SYS_BITYPES_H).
 * Host POSIX compiles (#include_next glibc). DUT: libcgj has no bitypes;
 * u_int8_t aliases live in overlay sys/types.h.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <sys/bitypes.h>
#endif
