# Unit 8 · Week 46 — Case-study Polish 与 GH-600 Six-domain Gap Closure

> **课程角色**：把 resize、async pipeline、runtime integration 三份实验整理成可独立
> 审阅的 case studies；将 GH-600 六域全部映射到真实 repo/MCP/GitHub evidence，
> 优先关闭最大 gap，不用模拟题数量掩盖缺少实践。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W45 clean/failure/lifetime evidence、Domains 1–3 map；W29–44 eval、
  multi-agent、guardrail、GitHub/MCP artifacts。
- 所有 TensorRT/Orin claims 继续引用 actual version/operating-point evidence；图表不能
  从不匹配环境重绘。
- **Remediation**：raw data 缺失的 claim 删除或重跑，不凭回忆补数字；domain 无 artifact
  时优先做一个 30–60 分钟最小实践，不用文字声称掌握。

## Observable objectives（可观察目标）

完成后能够：

1. 让三份 case study 的每个 claim 链接 raw data、environment、correctness 与 limitation。
2. 解释一次 hypothesis 被 profiler/eval 推翻的过程。
3. 为 GH-600 六域建立 evidence/gap/remediation matrix。
4. 演示 Runtime Lab Assistant 的 resources/tools/evals/threat/audit 链。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Case study | 问题→contract→baseline→evidence→change→result→limitation 的报告 |
| Claim traceability | 结论可追到 raw artifact、command、environment |
| Negative result | 无收益/被推翻的 hypothesis，仍是有效 evidence |
| Domain gap | knowledge、artifact 或 explanation 的缺口 |
| Demo defense | 运行演示并回答机制、失败、安全与限制追问 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin “Best Practices / Benchmarking and Profiling” | 15 | 1 |
| T2 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** all-six-domain blueprint/skill mapping | 15 | 3 |
| T3 | [MCP Server Overview, stable revision 2025-11-25](https://modelcontextprotocol.io/specification/2025-11-25/server/index) — primitive control hierarchy | 15 | 4 |
| T4 | [Microsoft Agent Framework — Workflow orchestrations](https://learn.microsoft.com/en-us/agent-framework/workflows/orchestrations/) — sequential、concurrent、handoff、HITL 的可执行机制 | 10 | 3 |

## 带问题阅读

- reviewer 能否从一张图回到 raw samples、units 与 environment？
- 六域中哪个 gap 是“知道定义但无法在真实 artifact 指出机制”？
- MCP demo 若只展示 happy path，遗漏了哪些 production evidence？

## Worked example

```text
claim: direct-device path lowered end-to-end p95 by X%
links: source diff + input hash + correctness JSON + before/after CSV
       + Nsight copy evidence + Orin power/thermal manifest
limitation: one model/profile/JetPack; no generalization beyond tested contract
```

## 累计项目 increment

- `docs/case-studies/resize.md`
- `docs/case-studies/async-pipeline.md`
- `docs/case-studies/runtime-integration.md`
- `artifacts/week-46/gh600-six-domain-gap-map.md`
- `artifacts/week-46/mcp-demo-evidence.md`
- `docs/gh600-checklist.md`

## 本周必须产出

- 三份 traceable case studies 与统一 evidence index。
- 六域 artifact/gap map、每域至少 3 个原创 scenarios、top remediations。
- MCP capstone end-to-end demo、failure/security evidence 和 reviewer questions。

## Day 1 — 理论与 claim audit（105 分钟）

- **15 分钟**：阅读 T1 的实际版本 benchmarking/profiling 部分。
- **30 分钟**：为三份报告建立 claim→artifact→environment→limitation matrix。
- **50 分钟**：删除 unsupported claims、修 units/labels/links；至少保留一个 negative result。
- **10 分钟**：列 missing raw evidence。

**必须留下**：claim matrix、broken-evidence list 和 edit plan。

## Day 2 — Guided lab：三份 case studies（110 分钟）

- **35 分钟**：完善 `resize.md` 的 mapping/border/oracle/kernel/profiler evidence。
- **35 分钟**：完善 `async-pipeline.md` 的 state/lifetime/timeline/throughput evidence。
- **30 分钟**：完善 `runtime-integration.md` 的 device path/precision/power/reliability。
- **10 分钟**：统一 measured/inferred/not-tested 标记。

**必须留下**：三份 case-study v2 和 shared evidence index。

## Day 3 — Independent lab：Six-domain gap map（105 分钟）

- **25 分钟**：阅读 T2+T4；T2 只记录当前六域/skills/访问日期，T4 用来复核
  multi-agent/HITL scenario 的真实执行机制。
- **60 分钟**：逐域映射 artifacts，并每域写 3 个原创 scenarios（共 18）；标
  knowledge/artifact/explanation gaps，不复制真实 exam content。
- **20 分钟**：选 top-3 remediation，指定 artifact、owner、pass condition。

**必须留下**：`gh600-six-domain-gap-map.md`、18 题解析和 remediation plan。

## Day 4 — Integration：MCP capstone demo defense（110 分钟）

- **15 分钟**：阅读 T3。
- **60 分钟**：fresh client 完成 initialize/discovery→read methodology→list→validate→
  compare→profiling prompt；保存 schema、correlation IDs、audit、permission traces。
- **25 分钟**：运行 invalid schema/path/incompatible run/prompt injection/timeout subset。
- **10 分钟**：写五个 reviewer questions 与 honest limitations。

**必须留下**：`mcp-demo-evidence.md`、security regression 和 demo questions。

## Day 5 — Assessment：Peer-style review 与 gap closure（105 分钟）

- **55 分钟**：以陌生 reviewer 视角抽查三份报告；每个数字验证 source/units/
  environment/correctness；修最影响可信度的三项。
- **35 分钟**：闭卷完成六域各一题加最弱域三题（共 9），记录 confidence/error type。
- **15 分钟**：关闭一个 top remediation 并更新 checklist。

**必须留下**：review report、9 题 error log 和 closed-gap evidence。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **35 分钟**：从 README 导航三份报告与 MCP demo，检查 broken links。
- **25 分钟**：做 8 分钟讲解：问题→evidence→negative result→limits。
- **25 分钟**：写周总结与 remaining six-domain gaps。
- **15 分钟**：冻结 W47 unofficial mock blueprint 与 demo inputs。

**必须留下**：Week 46 evidence index、recorded/timed defense notes 和 W47 blueprint。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- 任意 claim 可在两次点击/一个明确路径内回到 raw evidence。
- 六域每域至少一个真实 artifact 和一个 failure/limitation explanation。
- MCP demo 同时展示 functional、failure、security、audit。
- **Unlock W47**：top gaps 有明确 status，mock blueprint 不含真实 exam items。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [NVIDIA Technical Blog · TensorRT](https://developer.nvidia.com/blog/tag/tensorrt/)
  只选 1 篇带 methodology、environment、measurement 或 limitation 的 case study；
  无 raw-condition 线索的营销数字不选。
- **Artifact**：写 `notes/feed/week-46.md`，固定四栏
  `claim → evidence → scope → decision`，并把其证据表达与三份 case studies 比较。
- **取消规则**：claim matrix 落后、存在 broken links 或 top-three remediation 未关闭时
  直接取消。
