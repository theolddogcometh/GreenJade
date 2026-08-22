/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/prctl.h> for OpenSSH-portable (HAVE_SYS_PRCTL_H,
 * PR_SET_DUMPABLE). Host POSIX: glibc. DUT: libcgj has no sys/prctl.h.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <sys/prctl.h>
#else
#ifndef PR_SET_PDEATHSIG
# define PR_SET_PDEATHSIG 1
#endif
#ifndef PR_GET_DUMPABLE
# define PR_GET_DUMPABLE 3
#endif
#ifndef PR_SET_DUMPABLE
# define PR_SET_DUMPABLE 4
#endif
#ifndef PR_SET_NAME
# define PR_SET_NAME 15
#endif
#ifndef PR_GET_NAME
# define PR_GET_NAME 16
#endif
int prctl(int nOption, ...);
#endif
