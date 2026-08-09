#!/usr/bin/env bash
# SPDX-License-Identifier: MIT OR Apache-2.0
# CREATE-ONLY runner for wave 84000 (graph_batch80001..84000 via O_EXCL).
set -euo pipefail
cd "$(dirname "$0")"
exec python3 _emit_wave_84000_once.py
