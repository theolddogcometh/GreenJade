# dash glue (product `/bin/sh`)

Vendored **dash 0.5.13.5** is [`third_party/bsd/dash/`](../../third_party/bsd/dash/)
(**BSD-3-Clause**). This directory is GreenJade **MIT OR Apache-2.0** glue
only: `crt0.S`, include overlays, and `signames.c`.

`signames.c` replaces omitted GPL `third_party/bsd/dash/src/mksignames.c`
(GNU bash host generator). The table is original GJ; it is **not** derived
from bash or from dash’s generated output.

| Field | Value |
|-------|--------|
| **Vendor** | `third_party/bsd/dash/` · `COPYING` BSD-3-Clause |
| **Glue** | this directory · MIT OR Apache-2.0 |
| **Build** | `make dash` → `build/user/dash.elf` |
| **Packed license** | `/usr/share/licenses/dash/COPYING` |

Canonical provenance: [`third_party/PROVENANCE.md`](../../third_party/PROVENANCE.md).
