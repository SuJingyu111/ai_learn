# Week 12 — Milestone 2：Clean CPU Vertical Slice 与 Hard Gate

> **Unit 2 · Model-to-Runtime CPU Vertical Slice（第 6/6 周）**  
> **课程角色**：从 model/checkpoint 到 ONNX/ORT correctness/benchmark 的完整 CPU vertical slice defense；M2 只覆盖 frozen runtime tensor/normalize/layout scope，Resize semantics 留到 W21/M4；sidecar 交付 read-only tool reliability contract。

## 开始前：硬先修与 remediation

- **硬先修**：Week 09 checker、Week 10 provider correctness、Week 11 benchmark/tolerance 全通过。
- **Blockers**：Knowledge exam 闭卷新题首次正确率低于 80%；无法 clean export→ORT；provider 未记录；reference mismatch；raw samples/计时边界缺失；silent fallback；把 G2 误报成已有 CPU/CUDA Resize oracle；tool 接收 arbitrary path/shell 或无限 retry。
- **失败时 remediation**：按 source model→export contract→ONNX checker→ORT input/provider→correctness→timing 的顺序修最早失败层；不得以放宽 tolerance 或删除 case 过门。

## Observable objectives

- 从新进程重建 checkpoint→export→check→inspect→ORT CPU inference。
- 对固定与 boundary inputs 证明 PyTorch/ORT correctness。
- 分开 build/load/session/first/warm/steady costs并保留 raw data。
- 复述 frozen model/runtime input tensor/dataset/normalize/layout/planned source shape，并明确 Resize coordinate/pixel-center/border/rounding/tolerance deferred。
- defense 中区分 facts、inference、limitations 与 unresolved risk。
- 实现/说明 timeout、bounded retry、idempotency、structured error 与 escalation；对 read-only operation 明确“无状态回滚”，不把它伪装成写操作 rollback。

## 术语表

累计掌握：**Module/state/checkpoint、computational graph、ONNX IR/opset、export contract、runtime input tensor、dataset manifest、normalize/layout、planned source shape、deferred Resize contract、ORT session/provider/optimization、oracle/tolerance、raw statistics、typed tool、validation、permission、timeout、retry、idempotency、rollback/compensation boundary、escalation**。

## 必读理论材料

- **R1（Day 1，20 分钟）**：[PyTorch Save/Load](https://docs.pytorch.org/tutorials/beginner/basics/saveloadrun_tutorial.html) — state_dict workflow。
- **R2（Day 2，20 分钟）**：[ONNX IR Specification](https://onnx.ai/onnx/repo-docs/IR.html) — graph/node/initializer。
- **R3（Day 3，20 分钟）**：[ORT Get Started with Python](https://onnxruntime.ai/docs/get-started/with-python.html) — session/run。
- **S1（Day 4，20 分钟）**：[Designing Agent Architecture and SDLC Integration](https://learn.microsoft.com/en-us/training/modules/design-agent-architecture-integration/) — reliable workflows、observability、tool governance、secrets boundaries 与 reliability patterns。

80 分钟计入 Day 1–4；Podcast/blog 不参与 hard gate。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **仅当全部 Gate blockers 已关闭时做，否则取消**：从 [GitHub Blog: AI & ML](https://github.blog/ai-and-ml/) 选择一篇近 12 个月、主题为 agent reliability、evaluation、governance 或 production failure 的文章。
- 在 `notes/feed/week-12.md` 写 `claim → source/evidence → scope → experiment/decision`，并指出一个可加入 failure-aware read tool 的 negative case；Feed 不得替代 Gate preparation/evidence，不计 unlock。

## 带问题阅读

- vertical slice 中最早的 semantic contract 在哪里冻结？
- checker 通过为什么不等于 runtime output 正确？
- M2 冻结了哪些 preprocess facts，哪些 Resize facts必须等 W21/M4？
- retry 何时安全，何时会重复副作用？
- read-only operation 的 idempotency 仍需记录哪些 identity/state，为什么“无写入所以无状态 rollback”也必须明说？

## Worked example / 手算

若 session creation=`42 ms`、first inference=`7 ms`、warm-up 后 samples median=`1.2 ms`、p95=`1.5 ms`，报告必须分列这四项；不能写“模型延迟 1.2 ms”而忽略首次/端到端边界，更不能在未实现 Resize 时命名为 source-image end-to-end latency。若 tool 第一次 read 超时且 outcome unknown，只能用同一 request ID 对确定性 read 做 bounded retry；该 read 无状态变更，所以 rollback=`N/A`，不能把此规则套给任意写操作。

## 累计项目 increment

- `labs/ml/week12_cpu_vertical_slice.py`
- `milestones/m2-cpu-runtime/report.md`
- `notes/exams/unit-02.md`
- `ai-app/server/failure_aware_read.py` 或明确 blocked reproduction
- `notes/ai-app/week-12-reliability.md`

## Exit ticket / unlocks

**Hard Gate：总分 ≥80/100 且所有 blocker 通过，才 unlock Unit 3。下列数字是评分权重，不是分钟；实际计时拆在 Day 5–6：**

- **Knowledge：25 分权重** — 50 分钟闭卷、此前未见的新题；首次正确率必须 ≥80%，否则为 blocker。
- **Unseen practical：30 分权重** — 未见 model/runtime-input/provider/tool failure 的独立运行与定位。
- **Cumulative project：30 分权重** — M2 clean vertical slice、runtime-tensor correctness、raw benchmark 与 artifact 完整性；不包含 Resize oracle。
- **Oral/design defense：15 分权重** — 10 分钟口头解释设计、scope、evidence、limitations、permissions 与 reliability。

高分不能覆盖 blocker；失败只重做对应类别并使用新 input/case。

## Day 1

**阶段：cumulative rebuild I（100 分钟）**

- **20 分钟**：读 R1。
- **55 分钟**：从 model code+checkpoint 进入 eval/inference，复核 frozen model/runtime input tensor/dataset/normalize/layout/planned source→model shape；reference 只覆盖 runtime tensor，Resize semantics 标记 W21/M4 deferred。
- **15 分钟**：冻结 reliability cases：validation、not-found、transient、timeout、permission。
- **10 分钟**：记录 clean environment。
- **必须留下**：source model/reference、contract 和 frozen reliability set。

## Day 2

**阶段：cumulative rebuild II（100 分钟）**

- **20 分钟**：读 R2。
- **55 分钟**：export→checker→inspect；核对 inputs/outputs/initializers/opset。
- **15 分钟**：为 reliability cases 写 retry/no-retry/idempotency/rollback-or-N/A decisions。
- **10 分钟**：保存 versions/commands。
- **必须留下**：ONNX evidence 与 failure-policy table。

## Day 3

**阶段：guided vertical slice rehearsal（100 分钟）**

- **20 分钟**：读 R3。
- **60 分钟**：运行 ORT CPU correctness、wrong inputs、session/first/warm/steady timing。
- **10 分钟**：实现/验证固定 allowlist 的 read-only mini tool，不接受 path/shell。
- **10 分钟**：保存 raw results。
- **必须留下**：vertical slice rehearsal、raw samples 和 tool scope。

## Day 4

**阶段：independent mock（100 分钟）**

- **20 分钟**：读 S1。
- **60 分钟**：无笔记用新 fixed runtime tensor 独立 clean-run；注入一处 dtype/shape 错误并定位，禁止加入未学 Resize。
- **10 分钟**：测试 bounded retry/timeout/duplicate request ID。
- **10 分钟**：按 rubric 盲评并列 gaps。
- **必须留下**：independent run、fault trace、reliability traces 与初评分。

## Day 5

**阶段：Hard Gate Part 1 / Knowledge（100 分钟）**

- **20 分钟**：只修 Day 4 mock 的最低类别并用新 case 复测；到时即停止 remediation。
- **50 分钟**：Knowledge exam：闭卷完成此前未见的 lifecycle/ONNX/ORT/measurement/tool-reliability 与 M2 scope 题；原样保存首次答案（评分权重 25 分，首次正确率须 ≥80%）。
- **20 分钟**：封存 M2 report 的 claim→scope→evidence→limitation→reproduction、exact commands 与 reliability contract；不再修改 summative answers。
- **10 分钟**：记录 Knowledge score/blocker，并为 Day 6 生成只含顺序与 artifact paths 的封条清单。
- **必须留下**：Knowledge 首次答案/分数、最后一次 remediation evidence、sealed M2 defense packet。

## Day 6

**阶段：Hard Gate / M2 Defense（120 分钟）**

- **45 分钟**：Unseen practical：处理未见 model/runtime-input/provider/tool failure（评分权重 30 分）。
- **45 分钟**：Cumulative project：M2 clean vertical slice、runtime-tensor correctness、raw benchmark 与 artifact audit；确认 Resize deferred（评分权重 30 分）。
- **10 分钟**：Oral/design defense：解释设计、scope、evidence、limitations、permissions 与 reliability（评分权重 15 分）。
- **20 分钟**：汇总 Day 5 Knowledge 与本日三项分数，记录 blockers、pass/fail/remediation，不得自行上调。
- **必须留下**：四类 rubric evidence、M2 defense、分数/blocker 与 Unit 3 unlock 决定。
