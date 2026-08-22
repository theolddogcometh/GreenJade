/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Tiny <sys/param.h> for vendored dash. libcgj has no sys/param.h.
 * Dual MIT OR Apache-2.0. Not a dash source.
 */
#ifndef DASH_GJ_SYS_PARAM_H
#define DASH_GJ_SYS_PARAM_H

#ifndef MAXPATHLEN
#define MAXPATHLEN 4096
#endif

#ifndef PIPE_BUF
#define PIPE_BUF 4096
#endif

#ifndef BSD4_4
#define BSD4_4 1
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#ifndef MAXNAMLEN
#define MAXNAMLEN 255
#endif

#endif /* DASH_GJ_SYS_PARAM_H */
