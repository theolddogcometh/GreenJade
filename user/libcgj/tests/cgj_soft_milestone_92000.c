/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 92000 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_92000.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_92000 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_92000 \
 *   user/libcgj/tests/cgj_soft_milestone_92000.c -ldl
 * Direct: -DCGJ_SOFT_M92000_DIRECT + graph_batch91991.c … graph_batch92000.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M92000_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 92000
#define CGJ_SOFT_B0 91991
#define CGJ_SOFT_B1 91992
#define CGJ_SOFT_B2 91993
#define CGJ_SOFT_B3 91994
#define CGJ_SOFT_B4 91995
#define CGJ_SOFT_B5 91996
#define CGJ_SOFT_B6 91997
#define CGJ_SOFT_B7 91998
#define CGJ_SOFT_B8 91999
#include "_cgj_soft_m_template.inc"
