/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91000 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91000.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91000 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91000 \
 *   user/libcgj/tests/cgj_soft_milestone_91000.c -ldl
 * Direct: -DCGJ_SOFT_M91000_DIRECT + graph_batch90991.c … graph_batch91000.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91000_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91000
#define CGJ_SOFT_B0 90991
#define CGJ_SOFT_B1 90992
#define CGJ_SOFT_B2 90993
#define CGJ_SOFT_B3 90994
#define CGJ_SOFT_B4 90995
#define CGJ_SOFT_B5 90996
#define CGJ_SOFT_B6 90997
#define CGJ_SOFT_B7 90998
#define CGJ_SOFT_B8 90999
#include "_cgj_soft_m_template.inc"
