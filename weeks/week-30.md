# Week 30 — Unit 5：Async Pipeline Case Study 与 Hard Gate

## Unit / 课程角色

- **Unit 5 · Asynchronous Pipeline，结课周。** 把streams/events/pinned memory/ownership/double buffer/timeline合成可复现的pipeline case study；CUDA Graphs不在必修范围。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [统一评分规则](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：在冻结的20-case dataset上建立failure taxonomy；本周不做controlled tuning。

## 硬先修与 Remediation

- **硬先修**：Week 29三variant correctness/timeline/dataset baseline全；无已知slot lifetime/race；dataset/config hashes冻结。
- **开周诊断（15 分钟）**：闭卷画two-slot state machine与一段有实际overlap的timeline；对一个agent failure区分symptom/root cause。
- **未通过时**：回 Week 28 state trace或Week 29 timestamp table；修复 evidence缺口后再进case study，不以CUDA Graph补分。

## Observable Objectives

1. 完成同步/single-stream/double-buffer公平case study，报告correctness、first/steady/throughput、setup、thermal与limitations。
2. 从timeline解释实际overlap或明确否定，所有sync/ownership/error boundaries可追溯。
3. 证明0/1/odd以及500–1000 frames sustained run与failure/shutdown无已知race/duplicate/drop。
4. 将agent failures按reasoning/tool/context/environment/policy标注，区分symptom、primary cause、contributing factor与unknown。
5. 通过M5 hard gate；Graphs不参与评分。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| pipeline contract | input/output、ordering、ownership、lifetime、error、shutdown、measurement约定 |
| steady state | setup/teardown之外、重复stage达到稳定节奏的区间 |
| root cause | 最能指向修复位置、由evidence支持的主要原因 |
| symptom | timeout、bad answer等可观察现象，不自动等于root cause |
| contributing factor | 加剧失败但不是primary cause的因素 |
| adjudication | 对模糊标签按rubric与evidence做一致性裁决 |
| controlled tuning | 固定dataset/rubric下的一次单变量修改；留到Week 32 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Best Practices — Performance Metrics](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#performance-metrics)，复读timing、bandwidth、throughput边界 | 20 |
| 1 | [Nsight Systems User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html)，复读CUDA/NVTX trace与report/export | 15 |
| 2 | [Google SRE — Postmortem Culture](https://sre.google/sre-book/postmortem-culture/)，读 “Google’s Postmortem Philosophy”“Avoid Blame and Keep It Constructive” 与 formal review criteria：impact、root/contributing causes、evidence、preventive actions | 25 |
| 2 | [OpenTelemetry — Traces](https://opentelemetry.io/docs/concepts/signals/traces/)，读 trace/span/context与limitations | 15 |

**GH-600 blueprint mapping（0 额外分钟，不作教材）**：[GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) 的 error analysis、failure categories、evaluation evidence、tuning loop 仅用于认证考纲映射；root/contributing-cause分析与可执行follow-up由 Google SRE、OpenTelemetry证据和本周 labeling exercises 教授。

## 带问题阅读

- case study应如何避免只报告best run或只报告steady throughput？
- 何时timeline足以说明dependency，何时需Compute metrics或更多实验？
- failure label怎样指向instruction/tool/state/environment/policy的不同修复位置？
- 一个timeout为何可能是tool、environment、policy或context root cause？
- 在fixed dataset上何时才允许进入controlled tuning？
- 一份可审查 postmortem 为什么必须分开 impact、trigger、root/contributing causes、mitigation与preventive action？reviewer 如何识别“证据不足但过度归因”？

## Worked Example / 手算

- 给setup 5 ms、first frame 9 ms、后续intervals 5/5/6/5 ms，手算first latency、median steady interval、5-frame throughput，并说明短run限制。
- 给“tool call timeout because server permission prompt未获批”：列observation、symptom、primary label、contributing factor、evidence needed、recovery；允许`unknown`。
- 对same failure分别缺trace/有permission log，比较label confidence。

## 累计项目 Increment

- `docs/case-studies/async-pipeline.md` 与 `labs/async_pipeline/results/week-30/`。
- `ai-app/evals/failure-taxonomy.md`、10+ labeled baseline records与adjudication。
- `notes/curriculum/week-30.md`：学习记录；`notes/gates/gate-5.md`：M5 scores、blockers、verdict、next gaps。

## 本周必须产出

- async pipeline case study、raw traces/data、stress/error/reproduction pack。
- failure taxonomy/rubric、至少10 labels、一致性/adjudication evidence。
- M5 hard-gate submission。

## Hard Gate G5 / Milestone M5（统一 100 分）

| 维度 | 分值 | 本周证据 |
|---|---:|---|
| Knowledge | 25 | stream/event/default/pinned/ownership/overlap与failure taxonomy解释 |
| Unseen practical | 30 | 新frame count/error trace、per-frame outputs、slot invariants与safe eval fixtures |
| Cumulative project | 30 | 500–1000 frame pipeline、raw timeline/data/environment、dataset/taxonomy versions |
| Oral defense | 15 | case-study defense、failure diagnosis、limitations/next action |

**通过规则：`PASS` 需总分 ≥80/100、Knowledge 首次 ≥20/25、mandatory tests 100%，且 correctness、security、reproducibility blockers 全过。** 70–79 且无 critical blocker 为 `CONDITIONAL`；其余为 `RETAKE`。Global sync掩盖lifetime、unsafe real action、无法clean reproduce均为blocker；Graphs不加分也不补blocker。

## Day 1 — Theory：Case-study Evidence Contract（105 分钟）

- **20 分钟｜必修理论**：复读 performance metrics；冻结first/steady/throughput/setup/thermal definitions。
- **15 分钟｜必修理论**：复读 Systems trace/report。
- **30 分钟｜worked example**：完成5-frame metrics与short-run limitation。
- **30 分钟｜case-study plan**：冻结variants、cases、environment、correctness、raw fields、questions与acceptance。
- **10 分钟｜retrieval**：闭卷画state/timeline与所有sync理由。
- **必须留下**：measurement contract、worked metrics、case-study outline。

## Day 2 — Theory + Guided：Failure Taxonomy（105 分钟）

- **25 分钟｜必修理论**：阅读 Google SRE postmortem philosophy、blameless causal language与review criteria；建立symptom→evidence→cause→action模板。
- **15 分钟｜必修理论**：阅读OpenTelemetry trace/span/context。
- **35 分钟｜guided labeling**：定义reasoning/tool/context/environment/policy inclusion/exclusion/反例，分离symptom/root/contributor/unknown。
- **20 分钟｜Core guided**：跑三variants correctness/stress preflight，冻结source/config。
- **10 分钟｜检查**：用timeout worked example标注两次，记录evidence变化。
- **必须留下**：taxonomy v1、preflight、worked label。

## Day 3 — Guided → Independent：Case Study 与 10 Labels（105 分钟）

- **55 分钟｜Core lab**：采final raw samples/timelines；完成correctness、first/steady/throughput、setup、thermal、overlap与limitation sections。
- **35 分钟｜Sidecar build**：从Week 29 baseline选择至少10 records，保存case/evidence/symptom/primary/secondary/confidence/recovery。
- **15 分钟｜验证**：加入两个confusable pairs并写adjudication；不改dataset/rubric。
- **必须留下**：case-study draft/raw links、10 labels、adjudication。

## Day 4 — Independent Lab：Failure/Shutdown 与 Label Consistency（105 分钟）

- **60 分钟｜Core independent**：运行0/1/odd以及500/1000 frames、early stop、operator/copy failure；确认stop-submit/drain/cleanup、无duplicate/drop/race。
- **30 分钟｜Sidecar independent**：盲重标5 records，算一致性；分歧先改rubric解释，不改原evidence。
- **15 分钟｜evidence**：把一个runtime failure按同一observation→root方法分析，写不可类比处。
- **必须留下**：stress/error summary、label consistency、cross-domain analysis。

## Day 5 — Integration / Gate Rehearsal（105 分钟）

- **60 分钟｜Core integration**：完成case study与one-command reproduction；验证每个数字/trace/source/environment可追溯。
- **30 分钟｜Sidecar integration**：运行taxonomy checks：missing evidence、multiple plausible causes、unknown、safe recovery；禁止对真实账户/数据执行故障注入。
- **15 分钟｜preflight**：按25/30/30/15自评，冻结submission与blocker状态。
- **必须留下**：final draft、reproduction transcript、taxonomy tests、preflight score。

## Day 6 — Hard Gate G5 / M5（120 分钟 / 100 分）

- **45 分钟 / 25 分｜Knowledge**：闭卷分析unseen stream/event/overlap trace与agent failure evidence。
- **35 分钟 / 30 分｜Unseen practical**：运行unseen odd/stress/error case及safe invalid/permission/stale eval fixtures；false pass/unsafe action为blocker。
- **25 分钟 / 30 分｜Cumulative project**：从clean state重现500–1000 frame pipeline result与taxonomy sample；核对raw timeline/environment/dataset versions；失败为blocker。
- **15 分钟 / 15 分｜Oral defense**：答辩case study、overlap limitation、root-cause confidence与下一验证。
- **必须留下**：`notes/gates/gate-5.md`、逐项分数、blocker checklist、`PASS`/`CONDITIONAL`/`RETAKE` verdict与evidence index。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：只在 M5 preflight 无 blocker、required submission 已冻结且没有 Gate remediation 时做；若 Gate 落后或 verdict 非 `PASS`，直接取消，Feed Card 不参与评分。
- **来源与选择规则**：读 Google SRE Workbook [“Postmortem Culture: Learning from Failure”](https://sre.google/workbook/postmortem-culture/) 的 “Case Study”“Bad Postmortem”“Why Is This Postmortem Bad?” 与 “Good Postmortem”；只选一个能由具体 incident data 支持的 causal/action-item claim。
- **30–45 分钟流程**：5 分钟选 bad→good 对照点 → 15–20 分钟追证据与action ownership → 10–15 分钟写 `claim → evidence → scope → decision`；无法排除替代原因则 decision 为补采证据。
- **artifact**：`notes/feed/week-30.md`，含 bad/good 对照、一个 taxonomy rule 或 postmortem-template 决策。

## Exit Ticket / Unlocks

- **PASS**：≥80且三类blockers全过，解锁Unit 6 ORT CUDA integration。
- **CONDITIONAL / RETAKE**：按统一规则只重做失败objective的新variant；不引入CUDA Graph，不改变dataset来提高分数。
