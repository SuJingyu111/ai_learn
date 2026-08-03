# Unit 7 · Week 42 — Sustained Beta、MCP Evidence Assistant 与 M7 Hard Gate

> **课程角色**：Unit 7 结课。把 beta correctness、timeline、power/thermal、reliability
> artifacts 暴露给 read-only Runtime Lab Assistant；明确 registry/allowlist 的现实限制，
> 再于 Day 5–6 执行 M7 两日 hard gate。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W37–41 beta contract、correctness matrix、device path、timeline/security、
  sustained/reliability evidence；MCP server v0 schema/tests。
- Jetson/TensorRT evidence 必须保留 actual-version pins；环境变化使 sustained comparison
  invalid。MCP registry policy 不视为 sandbox，必须叠加 OS/tool/repo least privilege。
- **Remediation**：缺 sustained evidence 时 Day 1–2 补测；MCP 不可运行时保留 protocol
  fixtures/blocked trace，但 M7 evidence-assistant blocker 不通过；不得扩展 write tool。

## Observable objectives（可观察目标）

完成后能够：

1. 复现一段 sustained beta 并把 raw/summary/provenance 串成 evidence bundle。
2. 通过 MCP resources/tools 安全 list、validate、compare allowlisted runs。
3. 解释 registry discovery、allowlist enforcement 与其 current limitations。
4. 依据统一 rubric 通过 M7，而不是用高总分抵消 blocker。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Evidence bundle | contract、environment、raw data、summary、hash 与 limitations 的集合 |
| Resource | application-controlled 的可读 MCP context |
| Read-only tool | 输入受 schema/allowlist 约束且不修改外部状态的 operation |
| Registry | 可发现 MCP server manifests 的 catalog，不天然是 execution sandbox |
| Allowlist | 允许的 server/tool 身份集合；具体 enforcement 有 surface/版本限制 |
| Hard gate | ≥80 且全部 blockers pass 的里程碑 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [MCP Server Overview, stable revision 2025-11-25](https://modelcontextprotocol.io/specification/2025-11-25/server/index) — prompts/resources/tools control hierarchy | 15 | 1 |
| T2 | [MCP Resources, stable revision 2025-11-25](https://modelcontextprotocol.io/specification/2025-11-25/server/resources) — URI/error/security considerations | 15 | 2 |
| T3 | [MCP Tools, stable revision 2025-11-25](https://modelcontextprotocol.io/specification/2025-11-25/server/tools) — schemas、list/call、human control | 15 | 3 |
| T4 | [GitHub MCP allowlist enforcement](https://docs.github.com/en/copilot/reference/mcp-allowlist-enforcement) — “Current enforcement limitations / local servers” | 20 | 4 |

## 带问题阅读

- 什么时候用 resource，什么时候用 tool；谁控制它们？
- path allowlist 如何抵抗 traversal/symlink/absolute-path tricks？
- registry-only policy 的哪些限制意味着仍需 host sandbox 和 least privilege？

## Worked example

```text
resource: runtime://methodology
tool list_runs() -> IDs only from artifacts/allowlisted-runs
tool validate_run(id) -> schema/provenance/correctness fields
tool compare_runs(a,b) -> structured delta + incompatible-condition warnings
```

工具不接受任意 path/shell；summary 与 raw environment 不兼容时返回“不可比较”，
而不是生成更漂亮结论。

## 累计项目 increment

- `ai-app/deployment/mcp-inventory.md`
- `ai-app/evals/evidence-assistant-cases.md`
- `artifacts/week-42/evidence-bundle/`
- `artifacts/week-42/m7-scorecard.md`
- `notes/gates/gate-7.md`
- `notes/ai-app/week-42.md`

## 本周必须产出

- sustained beta evidence bundle 与 clean reproduction。
- Runtime Lab Assistant read-only resources/list/validate/compare path 及 security tests。
- registry/allowlist limitation matrix、M7 scorecard 与 go/remediate decision。

## Day 1 — 理论与 evidence bundle（105 分钟）

- **15 分钟**：阅读 T1，选 resource/tool primitive。
- **60 分钟**：复跑 representative sustained window，收集 contract、environment、
  raw samples、timeline、security/reliability results 与 hashes。
- **20 分钟**：生成 bundle index，区分 measured/inferred/unavailable。
- **10 分钟**：完整性检查。

**必须留下**：`evidence-bundle/`、index/hash 与 sustained rerun。

## Day 2 — Guided lab：Resources 与 validation（110 分钟）

- **15 分钟**：阅读 T2。
- **70 分钟**：实现/配置 architecture、methodology、environment resources 与
  `list_runs`/`validate_run`；URI/path 只解析 allowlisted IDs，invalid/not-found 返回
  structured error。
- **15 分钟**：运行 valid、unknown ID、traversal、missing metadata tests。
- **10 分钟**：保存 protocol trace。

**必须留下**：resources/validation path、四个 tests 和 structured errors。

## Day 3 — Independent lab：Compare evidence（105 分钟）

- **15 分钟**：阅读 T3。
- **65 分钟**：实现/配置 `compare_runs` 与 profiling-review prompt；比较前检查 model/
  input/environment/power/methodology compatibility，输出 structured before/after。
- **15 分钟**：测试 incompatible run、malicious metadata、oversized result。
- **10 分钟**：更新 `evidence-assistant-cases.md`。

**必须留下**：compare trace、compatibility warnings 和 security results。

## Day 4 — Integration：Registry/allowlist limitations（110 分钟）

- **20 分钟**：阅读 T4，记录访问日期与 supported surfaces。
- **50 分钟**：完成 `mcp-inventory.md`：server ID/version/owner/transport/tools/scopes/
  risk/review/disable；配置只读 local fixture，组织级设置只做设计或由 owner 操作。
- **30 分钟**：写 registry discovery、allowlist、host sandbox、tool schema 四层 control
  matrix；明确 current name/ID matching/strict-enforcement limitations。
- **10 分钟**：测试 unlisted server fixture fail/alert behavior。

**必须留下**：inventory、limitations matrix 和 unlisted-server evidence。

## Day 5 — M7 Gate A：Knowledge 与 submission audit（105 分钟）

- **55 分钟**：闭卷完成预先冻结、首次作答留痕的 scored Knowledge assessment，
  计 **25 分**；覆盖 beta contracts、precision/shape、streams、Orin reliability、
  MCP primitives、registry/allowlist 与 security。
- **25 分钟**：执行 submission audit：核对 commit/environment/variant、sustained raw
  data、artifact hashes、MCP schemas/tests 与四项 rubric evidence index。
- **25 分钟**：执行 blocker audit：逐条判定 correctness、thermal/reliability、
  device path、MCP functional/security 与 rollback blocker，冻结 Day 6 submission
  和 unseen variant seed。

**必须留下**：Knowledge 原始答卷与 25 分得分、submission manifest、blocker audit 和
Day 6 frozen variant ID。

## Day 6 — M7 Gate B：Practical、project、oral 与 report（110 分钟）

- **45 分钟**：执行考官从冻结池抽取的 unseen practical，计 **30 分**；覆盖 runtime
  或 MCP failure、diagnostic、rollback、audit 与 next-valid-run。
- **30 分钟**：从 clean process 复现 cumulative sustained beta 与 MCP read-only
  evidence assistant，计 **30 分**；验证 operating point、raw evidence 和 security。
- **20 分钟**：完成 oral defense，计 **15 分**；解释 registry/allowlist 边界、
  human conclusion ownership 与一个 residual risk。
- **15 分钟**：汇总两日 **Knowledge 25 / Unseen practical 30 / Cumulative project 30 /
  Oral defense 15**，写 go/remediate decision 与复测条件。

**必须留下**：`m7-scorecard.md`、`notes/gates/gate-7.md`、unseen/clean transcripts、
oral prompts/notes 和 gate decision。

## M7 blockers、Exit ticket 与 unlocks

**通过条件：总分 ≥80/100，且 blockers 全部通过。**

- [ ] 冻结 beta contract 的 declared inputs/precision/shapes 全部 correctness pass。
- [ ] sustained data 绑定实际 Orin operating point，无未解释 thermal/reliability drift。
- [ ] device-resident/stream timeline 无未解释 host round-trip 或 readiness gap。
- [ ] stress/early-stop/runtime/MCP failure、rollback 与 next-valid-run 均通过。
- [ ] MCP evidence assistant 只读、schema/path allowlisted，functional/security tests 通过。
- [ ] registry/allowlist limitations、audit、disable/rollback 与 residual risk 可解释。

Gate administration：Knowledge 首次 ≥80%、订正后 100%；mandatory correctness/security
tests 100%；report 记录 date、commit、environment、variant ID、四项得分与 remediation。

Exit ticket：用 5 分钟从 raw run 经 MCP compare 到 human conclusion，指出一项不能自动化
的判断。**Unlock Unit 8 / W43**：M7 pass；否则进入 remediation。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [Model Context Protocol Blog](https://blog.modelcontextprotocol.io/)
  只选 1 篇与 stable spec、registry 或 production server operation 直接相关的文章；
  release candidate/Preview 只能标 maturity，不能改写本周稳定版 contract。
- **Artifact**：写 `notes/feed/week-42.md`，固定四栏
  `claim → evidence → scope → decision`；不得作为 M7 submission 或补分材料。
- **取消规则**：Gate submission/blocker audit 落后或未冻结时直接取消，不补做。
