/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * <sys/poll.h> alias of <poll.h> for OpenSSH-portable (HAVE_SYS_POLL_H).
 * Host POSIX: glibc sys/poll.h. DUT: libcgj has <poll.h> only.
 */
#pragma once
#if __STDC_HOSTED__
# include_next <sys/poll.h>
#else
# include <poll.h>
#endif
