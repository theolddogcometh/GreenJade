/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <libgen.h> for OpenSSH-portable (HAVE_LIBGEN_H).
 * Host POSIX compiles (#include_next glibc). Do not redeclare basename
 * on hosted: glibc <string.h> GNU basename is const char * and conflicts.
 * DUT: libcgj has no libgen.h; basename/dirname live in libcgj string.h.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <libgen.h>
#else
char *basename(char *szPath);
char *dirname(char *szPath);
#endif
