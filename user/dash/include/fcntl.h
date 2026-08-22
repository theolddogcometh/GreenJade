/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Dash glue: F_DUPFD_CLOEXEC (Linux 1030). Dual MIT OR Apache-2.0.
 */
#ifndef DASH_GJ_FCNTL_H
#define DASH_GJ_FCNTL_H

#include_next <fcntl.h>

#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC 1030
#endif

#endif /* DASH_GJ_FCNTL_H */
