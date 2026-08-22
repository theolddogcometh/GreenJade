/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Hide libcgj memfd_create so dash's HAVE_MEMFD_CREATE=0 stub can exist.
 * Dual MIT OR Apache-2.0.
 */
#ifndef DASH_GJ_SYS_MMAN_H
#define DASH_GJ_SYS_MMAN_H

#define memfd_create dash_gj_hidden_memfd_create
#include_next <sys/mman.h>
#undef memfd_create

#endif /* DASH_GJ_SYS_MMAN_H */
