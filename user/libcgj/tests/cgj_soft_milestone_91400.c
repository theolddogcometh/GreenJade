/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91400 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91400.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91400 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91400 \
 *   user/libcgj/tests/cgj_soft_milestone_91400.c -ldl
 * Direct: -DCGJ_SOFT_M91400_DIRECT + graph_batch91391.c … graph_batch91400.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91400_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91400
#define CGJ_SOFT_B0 91391
#define CGJ_SOFT_B1 91392
#define CGJ_SOFT_B2 91393
#define CGJ_SOFT_B3 91394
#define CGJ_SOFT_B4 91395
#define CGJ_SOFT_B5 91396
#define CGJ_SOFT_B6 91397
#define CGJ_SOFT_B7 91398
#define CGJ_SOFT_B8 91399
#include "_cgj_soft_m_template.inc"
