/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * NFDBITS / howmany / fd_mask for OpenSSH-portable (HAVE_DECL_NFDBITS,
 * HAVE_DECL_HOWMANY, HAVE_FD_MASK). libcgj select is fd_set only.
 */
#pragma once

#include_next <sys/select.h>

#ifndef NFDBITS
# define NFDBITS ((int)(8 * (int)sizeof(unsigned long)))
#endif
#ifndef howmany
# define howmany(x, y) (((x) + ((y) - 1)) / (y))
#endif
