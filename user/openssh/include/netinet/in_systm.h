/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <netinet/in_systm.h> for OpenSSH-portable.
 * Host POSIX compiles (#include_next glibc). DUT: libcgj has no in_systm.h.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <netinet/in_systm.h>
#else
# include <stdint.h>
typedef uint16_t n_short;
typedef uint32_t n_long;
typedef uint32_t n_time;
#endif
