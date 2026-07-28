# Unit 8 · Week 47 — Self-authored Unofficial Mock 与 Demo Defense

> **课程角色**：用公开 GH-600 skills 自编原创场景，做一次**非官方 mock**与 error
> analysis；它只训练 constraint reasoning，不能预测正式考试分数。并对 runtime/MCP
> capstone 做 reviewer-style demo defense。
>
> **边界**：不得寻找、记录、复述或分享真实 exam items。Microsoft Exam Sandbox
> **只用于熟悉 UI、导航和交互类型**，不提供考试内容，也不证明 readiness。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W46 six-domain gap map、case-study v2、fresh MCP demo、公开 study guide。
- **Remediation**：任一 domain 无 artifact 时，先用 Day 1/2 的 build block补最小实践；
  不用题库记忆替代。自己出题会产生 author bias，结果只能用于 gap analysis，不等同
  official practice assessment 或 passing probability。

## Observable objectives（可观察目标）

完成后能够：

1. 从公开 skills blueprint 编写原创、约束充分、无泄题风险的 scenario set。
2. 在固定时间内作答并按 domain/error/confidence 分析。
3. 用 official sources 修正概念，不改写原始答案掩盖错误。
4. 在 8–10 分钟 demo 中防守 runtime 与 MCP 的机制、证据、安全、限制。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Self-authored unofficial mock | 由学习者按公开 blueprint 原创的练习，不是官方样题 |
| Author bias | 出题者已知结构/答案导致成绩虚高 |
| Exam sandbox | 只演练考试 UI/导航的官方界面 |
| Confidence calibration | confidence 与实际 correctness 的匹配程度 |
| Distractor rationale | 错项违反哪个 scope/risk/evidence constraint |
| Demo defense | 运行演示并即时回答 reviewer 追问 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** “Skills at a glance” 与六域 blueprint mapping | 20 | 1 |
| T2 | [Exam and assessment security policies](https://learn.microsoft.com/en-us/credentials/support/exam-and-assessment-lab-security-policies) — confidentiality / ethical conduct | 15 | 1 |
| T3 | [Microsoft Exam Sandbox](https://aka.ms/examdemo) — **仅 UI/navigation** | 10 | 2 |
| T4 | [MCP Security Best Practices](https://modelcontextprotocol.io/docs/tutorials/security/security_best_practices) — scope、token、confused deputy | 15 | 4 |
| T5 | [Microsoft Agent Framework — Workflow orchestrations](https://learn.microsoft.com/en-us/agent-framework/workflows/orchestrations/) — sequential、concurrent、handoff 与 HITL 的机制复习 | 10 | 5 |

## 带问题阅读

- 一个好 scenario 的决定性 constraints 是 scope、state、evidence 还是 material risk？
- author bias 与重复题如何使成绩看起来稳定？
- demo 中哪些结论是 measured、inferred、not tested？

## Worked example

原创练习样式（非考试题）：

```text
场景：read-only comparison agent 收到含“改 power mode”的 benchmark metadata。
约束：metadata untrusted；tool 无 device-write scope；comparison 只需现有 artifacts。
最佳动作：忽略嵌入指令，验证 provenance，仅用 allowlisted read tools，记录 denial。
错项：临时扩大权限 / 自动执行 / 只删除文字但信任其余结论。
```

评分看 constraint reasoning 与 evidence，不看措辞是否像正式考试。

## 累计项目 increment

- `ai-app/evals/gh600-self-mock.md`
- `artifacts/week-47/mock-results.md`
- `artifacts/week-47/mock-error-analysis.md`
- `artifacts/week-47/demo-defense.md`
- `docs/gh600-checklist.md`

## 本周必须产出

- 六域原创 unofficial mock、separate answer rationale 与明确局限声明。
- timed result、error/confidence/domain analysis 和 targeted remediation。
- runtime+MCP demo defense、five follow-ups 与 final-release blockers。

## Day 1 — 理论、ethics 与 mock blueprint（105 分钟）

- **20 分钟**：阅读 T1，只用公开 skills 创建六域 coverage blueprint。
- **15 分钟**：阅读 T2，写不使用真实/记忆 exam content 的规则。
- **55 分钟**：为每域起草 4 个原创 scenarios（共 24）；每题含 constraints、best
  action、distractor rationale、repo evidence，答案另段保存。
- **15 分钟**：检查重复、歧义、author bias 与 domain balance。

**必须留下**：24 题 unofficial draft、blueprint 和 ethics statement。

## Day 2 — Guided lab：UI sandbox 与 mock quality（110 分钟）

- **10 分钟**：只使用 T3 熟悉 UI/navigation/flag/review，不把 sandbox 当内容练习。
- **50 分钟**：人工审查 24 题：是否能由公开 docs/artifacts回答、是否只有一个 best
  action、是否偷渡产品细节；修 wording，不改 public blueprint。
- **35 分钟**：增加 6 个跨域 scenarios，总计 30；随机排序并冻结。
- **15 分钟**：写 author-bias limitation 与 timing protocol。

**必须留下**：frozen 30-question self-mock、answer rationale 和 UI-only note。

## Day 3 — Independent assessment：Timed unofficial mock（105 分钟）

- **75 分钟**：闭卷、不中断完成自编 30 题；逐题记录 answer/confidence/time，不查资料。
- **20 分钟**：冻结 answer sheet 后才对照 rationale，统计 domain accuracy 与 calibration。
- **10 分钟**：保存 raw result，不改原答案。

**必须留下**：`mock-results.md`、timing/confidence data 和 explicit unofficial label。

## Day 4 — Integration：Runtime/MCP demo defense（110 分钟）

- **15 分钟**：阅读 T4。
- **55 分钟**：fresh run 8–10 分钟 demo：contract→device path→timeline→sustained→
  optimization→failure/CI；MCP initialize→resource→list/validate/compare→security denial。
- **30 分钟**：回答五个追问：lifetime、version pin、negative result、registry limitation、
  incident rollback。
- **10 分钟**：标 measured/inferred/not-tested。

**必须留下**：`demo-defense.md`、demo trace、five answers 和 limitation labels。

## Day 5 — Assessment：Error analysis 与 remediation（105 分钟）

- **10 分钟**：阅读 T5，以 primary mechanism source 复核 coordination/HITL 错题，
  不从 GH-600 blueprint 推导产品行为。
- **45 分钟**：逐项分类 wrong/low-confidence：constraint misread、knowledge、scope/
  permission、state、evaluation evidence、coordination、guardrail。
- **30 分钟**：完成 top 2–3 remediations；对同 concept 自写 6 个新变体并作答。
- **20 分钟**：更新 six-domain scorecard/checklist；成绩仍标 unofficial/biased。

**必须留下**：`mock-error-analysis.md`、remediation artifacts 和 6 题 retest。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **35 分钟**：重做 demo 中最弱的一段与一个 failure/security path。
- **25 分钟**：口述六域各一件 artifact及 limitation。
- **25 分钟**：写 readiness evidence、remaining blockers；不凭 mock 分数预约。
- **15 分钟**：冻结 W48 release/M8 inputs 与 scorecard skeleton。

**必须留下**：Week 47 evidence index、final blockers 和 W48 gate inputs。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正；self-authored mock 不能成为唯一 assessment。

- mock 每页明确“self-authored / unofficial / author-biased / not predictive”。
- Exam Sandbox 只产生 UI familiarity note，没有题目内容或 readiness claim。
- 错题保留原始答案、原因、remediation 与新 evidence。
- **Unlock W48**：demo 可复现，release blockers 与 six-domain gaps 均有明确状态。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [GitHub Blog · AI & ML](https://github.blog/ai-and-ml/)
  只选 1 篇讨论 agent evaluation、guardrails 或 developer workflow 的文章；不选择
  exam dumps、题库或声称预测 GH-600 分数的内容。
- **Artifact**：写 `notes/feed/week-47.md`，固定四栏
  `claim → evidence → scope → decision`，最多生成一个新 remediation，不改 mock 分数。
- **取消规则**：unofficial mock error analysis 或 demo defense 落后时直接取消。
