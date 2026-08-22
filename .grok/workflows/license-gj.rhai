let meta = #{
    name: "license-gj",
    description: "Enforce vendor-license folder law: leaks outside third_party/<license>/, omitted GPL, make license. Not Dual DoD close.",
    when_to_use: "When importing vendor code, checking third_party layout, or after license/provenance changes.",
    phases: [
        #{ title: "Scan", detail: "honesty: folder law + leaks" },
        #{ title: "Gate", detail: "run check-license.sh and check-third-party-license.sh" },
        #{ title: "Report", detail: "PASS/FAIL; Dual DoD still OPEN" },
    ],
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
let gate_schema = #{
    "type": "object", "required": ["ok", "notes"],
    "properties": #{
        "ok": #{ "type": "boolean" },
        "notes": #{ "type": "string" },
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

phase("Scan");
let sp = "";
sp += "You are GreenJade honesty-auditor under fail-closed, no-densify, and vendor-license. ";
sp += "Use tools. Do not answer from memory. Do not edit. Do not flash.\n";
sp += "Read AGENTS.md Vendor license (hard) and third_party/PROVENANCE.md.\n";
sp += "Check: (1) vendor trees exist under third_party/apache-2.0/openssl, ";
sp += "third_party/bsd/{dash,openssh,tcsh}, third_party/licenseref-zsh/zsh, ";
sp += "third_party/public-domain/ed25519. (2) find kernel user abandoned scripts ";
sp += "for ed25519.c or vendor COPYING/LICENCE — those are leaks. ";
sp += "(3) omitted GPL paths from check-third-party-license.sh stay absent.\n";
sp += "Report at most 8 {file, issue}. Empty findings is valid only after you inspected. ";
sp += "A license PASS is not Dual DoD close.\n";
let scan = agent(sp, #{
    label: "license-scan",
    agent_type: "honesty-auditor",
    capability_mode: "read-only",
    output_schema: findings_schema,
});

phase("Gate");
let gp = "";
gp += "Run, do not skip: ./scripts/check-license.sh and ";
gp += "./scripts/check-third-party-license.sh (or make license). ";
gp += "Do not flash. Do not edit sources. Do not bump GJ_IMAGE_VERSION.\n";
gp += "Set ok=true only if both scripts exit 0. notes = last OK/FAILED lines. ";
gp += "ok=true is not Dual DoD close.\n";
let gate = agent(gp, #{
    label: "license-gate",
    agent_type: "hwtest-packer",
    capability_mode: "execute",
    output_schema: gate_schema,
});

phase("Report");
let findings = [];
let so = output_map(scan);
if so != () && so.findings != () {
    for f in so.findings {
        let fp = "";
        if f.file != () { fp = trimmed(f.file); }
        if fp != "" && findings.len() < 8 {
            findings.push(#{ file: fp, issue: trimmed(f.issue) });
        }
    }
}
let gate_ok = false;
let gate_notes = "gate agent produced no usable output";
let gate_out = output_map(gate);
if gate_out != () {
    if gate_out.ok == true { gate_ok = true; }
    if gate_out.notes != () { gate_notes = trimmed(gate_out.notes); }
}

let report = "# license-gj\n\n";
report += "Vendor-license folder law. Dual DoD A/B stay OPEN. Stamp not bumped.\n\n";
if gate_ok {
    report += "**Gate:** PASS (`make license`).\n\n";
} else {
    report += "**Gate:** FAIL — " + gate_notes + "\n\n";
}
if findings.len() == 0 {
    report += "**Scan:** no in-scope leaks reported after inspection.\n";
} else {
    report += "**Scan findings:**\n";
    for f in findings {
        report += "- `" + f.file + "`: " + f.issue + "\n";
    }
}
let path = write_scratch_file("report.md", report);
let summary = "license-gj gate=";
if gate_ok { summary += "PASS"; } else { summary += "FAIL"; }
summary += " findings=" + findings.len().to_string();
complete(#{ path: path, summary: summary, gate_ok: gate_ok, findings: findings });
