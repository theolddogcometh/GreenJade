let meta = #{
    name: "hwtest-pack",
    description: "Inventory stamp vs sources, pack hwtest image, extract-hash prove, honesty review. Does not flash.",
    when_to_use: "When preparing a GreenJade hardware-test USB image or proving build/greenjade-hwtest.img matches the tree.",
    phases: [
        #{ title: "Inventory", detail: "stamp, dirty sources, image mtime" },
        #{ title: "Build", detail: "drivers-udx, dash, kernel, hwtest-img" },
        #{ title: "Prove", detail: "extract ESP from the image and hash" },
        #{ title: "Honesty", detail: "docs vs stamp; Dual DoD wording" },
        #{ title: "Report", detail: "ready / not-ready; flash stays human" },
    ],
};

let inventory_schema = #{
    "type": "object", "required": ["ready", "stamp", "notes"],
    "properties": #{
        "ready": #{ "type": "boolean" },
        "stamp": #{ "type": "string" },
        "notes": #{ "type": "string" },
    },
};
let pack_schema = #{
    "type": "object", "required": ["ok", "stamp", "notes"],
    "properties": #{
        "ok": #{ "type": "boolean" },
        "stamp": #{ "type": "string" },
        "notes": #{ "type": "string" },
    },
};
let findings_schema = #{
    "type": "object", "required": ["findings"],
    "properties": #{
        "findings": #{ "type": "array", "maxItems": 8, "items": #{
            "type": "object", "required": ["file", "issue"],
            "properties": #{
                "file": #{ "type": "string" },
                "issue": #{ "type": "string" },
            },
        }},
    },
};

fn trimmed(s) {
    if type_of(s) == "string" { s.trim(); s } else { "" }
}

fn output_map(r) {
    if r == () { return (); }
    if r.success != true { return (); }
    if r.output == () { return (); }
    if type_of(r.output) != "map" { return (); }
    r.output
}

let want_stamp = "";
let ask_flash = false;
if args != () {
    if args.stamp != () { want_stamp = trimmed(args.stamp); }
    if args.ask_flash == true { ask_flash = true; }
}

phase("Inventory");
let inv_prompt = "";
inv_prompt += "You are GreenJade hwtest-packer. Read AGENTS.md first. ";
inv_prompt += "Use tools (read_file, grep, run_terminal_command). Do not flash. Do not edit sources. ";
inv_prompt += "Inventory the fly stamp and whether a pack is needed.\n";
inv_prompt += "1. Read kernel/include/gj/config.h for GJ_IMAGE_VERSION.\n";
inv_prompt += "2. Run ./scripts/gj-image-version.sh --report if build/greenjade.elf exists.\n";
inv_prompt += "3. Compare mtimes of kernel/ user/openssh user/drivers user/dash user/udx scripts/make-hwtest-img.sh ";
inv_prompt += "against build/greenjade-hwtest.img (if present).\n";
inv_prompt += "4. Set ready=true only if an image exists AND no product sources are newer AND ";
inv_prompt += "gj-image-version matches config.h.\n";
inv_prompt += "5. notes = short facts (paths, stamps, what is stale). Empty notes only after those checks.\n";
if want_stamp != "" {
    inv_prompt += "Operator expected stamp: ";
    inv_prompt += want_stamp;
    inv_prompt += "\n";
}
let inv = agent(inv_prompt, #{
    label: "inventory",
    agent_type: "hwtest-packer",
    capability_mode: "execute",
    output_schema: inventory_schema,
});
let inv_out = output_map(inv);
let stamp = want_stamp;
if inv_out != () && inv_out.stamp != () {
    let got = trimmed(inv_out.stamp);
    if got != "" { stamp = got; }
}
if stamp == "" { stamp = "unknown"; }
log("inventory stamp=" + stamp);

phase("Build");
let build_prompt = "";
build_prompt += "You are GreenJade hwtest-packer. Read AGENTS.md. Use tools. Do not flash. ";
build_prompt += "Do not bump GJ_IMAGE_VERSION. Do not edit product sources.\n";
build_prompt += "Working stamp: " + stamp + "\n";
build_prompt += "If build/greenjade-hwtest.img already matches this tree and ";
build_prompt += "./scripts/gj-image-version.sh --report equals that stamp, set ok=true and say so.\n";
build_prompt += "Otherwise run, in order, and stop on first hard fail:\n";
build_prompt += "  make drivers-udx\n  make dash\n  make\n  make hwtest-img\n";
build_prompt += "  make assurance-check\n";
build_prompt += "ok=true only if make hwtest-img exits 0. notes = commands + sizes + assurance summary.\n";
let built = agent(build_prompt, #{
    label: "build",
    agent_type: "hwtest-packer",
    capability_mode: "execute",
    output_schema: pack_schema,
});
let built_out = output_map(built);
let build_ok = false;
if built_out != () && built_out.ok == true { build_ok = true; }
if build_ok { log("build ok"); } else { log("build not ok (fail-closed; still attempt prove)"); }

phase("Prove");
let prove_prompt = "";
prove_prompt += "You are GreenJade hwtest-packer. Read AGENTS.md. Use tools. Do not flash.\n";
prove_prompt += "Prove pack of stamp " + stamp + " from the IMAGE, not build/esp.\n";
prove_prompt += "ESP offset 1048576 (GPT p1 sector 2048). MTOOLS_SKIP_CHECK=1.\n";
prove_prompt += "mdir and mcopy from build/greenjade-hwtest.img@@1048576:\n";
prove_prompt += "  ::/EFI/GREENJADE/KERNEL.ELF\n";
prove_prompt += "  ::/EFI/GREENJADE/IMAGE_VERSION\n";
prove_prompt += "  ::/EFI/GREENJADE/drivers/rtl8168_udx\n";
prove_prompt += "  ::/EFI/GREENJADE/drivers/xhci_udx\n";
prove_prompt += "  ::/EFI/GREENJADE/rootfs-full/bin/sh\n";
prove_prompt += "sha256sum those against build/greenjade.elf, ";
prove_prompt += "build/user/drivers/rtl8168_udx, build/user/drivers/xhci_udx, build/user/dash.elf.\n";
prove_prompt += "strings KERNEL for STATUS (static) v" + stamp + " and M0 OK dash SKIP isolate.\n";
prove_prompt += "./scripts/gj-image-version.sh --report on the extracted KERNEL.\n";
prove_prompt += "ok=true only if hashes match AND panel title is STATUS (static) v" + stamp + ".\n";
prove_prompt += "notes = hashes + yes/no for each check. build/esp is not evidence.\n";
let proved = agent(prove_prompt, #{
    label: "prove",
    agent_type: "hwtest-packer",
    capability_mode: "execute",
    output_schema: pack_schema,
});
let proved_out = output_map(proved);
let prove_ok = false;
if proved_out != () && proved_out.ok == true { prove_ok = true; }
if prove_ok { log("prove ok"); } else { log("prove not ok"); }

phase("Honesty");
let hon_docs = "";
hon_docs += "You are GreenJade honesty-auditor (fail-closed, no-densify). Read-only tools only.\n";
hon_docs += "Read AGENTS.md, README.md (lab/STATUS table), docs/HWTEST_TOMORROW.md, docs/TODO.md ";
hon_docs += "(current track only), kernel/include/gj/fb_console.h hold map.\n";
hon_docs += "Stamp under review: " + stamp + "\n";
hon_docs += "Find at most 8 {file, issue} where docs claim a different fly bar, claim Dual DoD close, ";
hon_docs += "claim 0.2.0, or treat L1 assurance as L3. Empty list only after reading those files.\n";
hon_docs += "Do not edit. Do not suggest densify.\n";
let hon_claim = "";
hon_claim += "You are GreenJade honesty-auditor (fail-closed, no-densify). Read-only tools only.\n";
hon_claim += "Grep README.md docs/HWTEST_TOMORROW.md docs/TODO.md docs/ASSURANCE_LITE.md AGENTS.md ";
hon_claim += "for Dual DoD, 0.2.0, interactive login, banner proven, scratchpad.\n";
hon_claim += "Stamp: " + stamp + "\n";
hon_claim += "Report at most 8 {file, issue} over-claims (close from lamps, banner=login, SUCCESS=login, L1=L3). ";
hon_claim += "Empty list only after the greps. Do not edit. Do not densify.\n";
let hon_jobs = [];
hon_jobs.push(#{
    prompt: hon_docs,
    label: "honesty:docs",
    agent_type: "honesty-auditor",
    capability_mode: "read-only",
    output_schema: findings_schema,
});
hon_jobs.push(#{
    prompt: hon_claim,
    label: "honesty:claims",
    agent_type: "honesty-auditor",
    capability_mode: "read-only",
    output_schema: findings_schema,
});
let hon_results = parallel(hon_jobs);
let findings = [];
for r in hon_results {
    let o = output_map(r);
    if o != () && o.findings != () {
        for f in o.findings { findings.push(f); }
    }
}
log(findings.len().to_string() + " honesty findings");

phase("Report");
let ready = false;
if build_ok && prove_ok { ready = true; }
let report = "";
report += "# hwtest-pack\n\n";
report += "- stamp: " + stamp + "\n";
if ready { report += "- ready: yes\n"; } else { report += "- ready: no\n"; }
if build_ok { report += "- build: ok\n"; } else { report += "- build: not-ok\n"; }
if prove_ok { report += "- prove: ok (extracted image)\n"; } else { report += "- prove: not-ok\n"; }
report += "- honesty findings: " + findings.len().to_string() + "\n";
report += "- Dual DoD A/B: OPEN (this workflow never closes them)\n";
report += "- flash: human only — sudo ./scripts/install-hwtest-usb.sh /dev/sdX\n";
if findings.len() > 0 {
    report += "\n## Honesty findings\n";
    for f in findings {
        let fp = "";
        let iss = "";
        if f.file != () { fp = trimmed(f.file); }
        if f.issue != () { iss = trimmed(f.issue); }
        report += "- `" + fp + "`: " + iss + "\n";
    }
}
let path = write_scratch_file("report.md", report);

if ask_flash {
    await_user("user", "Pack report written. Flash is still a human step. Resume only if you will flash from a real shell yourself; this workflow will not run install-hwtest-usb.");
}

complete(#{
    path: path,
    stamp: stamp,
    ready: ready,
    build_ok: build_ok,
    prove_ok: prove_ok,
    honesty_n: findings.len(),
    summary: report,
});
