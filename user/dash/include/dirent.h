/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Dash uses struct dirent64 / readdir64; libcgj is LP64 (dirent is enough).
 * Dual MIT OR Apache-2.0.
 */
#ifndef DASH_GJ_DIRENT_H
#define DASH_GJ_DIRENT_H

#include_next <dirent.h>

#ifndef dirent64
#define dirent64 dirent
#endif
#ifndef readdir64
#define readdir64 readdir
#endif

#endif /* DASH_GJ_DIRENT_H */
