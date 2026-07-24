/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 80000 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_80000.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_80000 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_80000 \
 *   user/libcgj/tests/cgj_soft_milestone_80000.c -ldl
 * Direct: -DCGJ_SOFT_M80000_DIRECT + graph_batch79991.c … graph_batch80000.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M80000_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 80000
#define CGJ_SOFT_B0 79991
#define CGJ_SOFT_B1 79992
#define CGJ_SOFT_B2 79993
#define CGJ_SOFT_B3 79994
#define CGJ_SOFT_B4 79995
#define CGJ_SOFT_B5 79996
#define CGJ_SOFT_B6 79997
#define CGJ_SOFT_B7 79998
#define CGJ_SOFT_B8 79999
#include "_cgj_soft_m_template.inc"
