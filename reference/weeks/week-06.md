# Week 06 — Unit 1 Cumulative Exam 与 Milestone 1 Defense

> **Unit 1 · ML Systems Foundations（第 6/6 周）**  
> **课程角色**：累计复习、闭卷迁移、M1 defense 与第一个 hard gate；sidecar 把 Week 05 架构收敛成可检查的 read-only custom-agent profile/contract。硬门规则见 [mastery gates](../docs/mastery-gates.md)。

## 开始前：硬先修与 remediation

- **硬先修**：Week 05 blocker list 为空；M1 runner 能从 clean process 运行。
- **Blockers**：Knowledge exam 闭卷新题首次正确率低于 80%；任何 fixed hand case 不通过；normalize/layout/operator/model slice 无 first-mismatch；tolerance 无依据；parameter/activation bytes 或 softmax 推导错误；命令/版本缺失；把 agent output 当自我批准；把尚未学习的 Resize 冒充为 M1 已完成。
- **失败时 remediation**：Day 1–5 只修最早失败层（shape→stride→operator→numerics→integration），不得靠删 tests、放宽 tolerance 或跳过 case 进入 defense。

## Observable objectives

- 跨 array/stride/broadcast/reduction/normalize/MatMul/Conv/Pool/softmax/numerics 解释完整 input→operator→output contract。
- 在无提示条件下完成地址、broadcast/reduction、MatMul、Conv/Pool、softmax/bytes、tolerance 手算。
- 从 clean process 复现 M1 并定位一个注入错误。
- 用 evidence 回答限制，拒绝无证据的 performance/kernel claim。
- 解释 custom-agent profile 的 description/tools/instructions 边界，并证明它没有 edit/merge/shell 权限。

## 术语表

累计掌握：**shape、stride、layout、view/copy、broadcast、reduction、normalization、MatMul、Conv、pooling、softmax、parameter/activation bytes、operator、floating tolerance、train/eval/inference、agent architecture、custom-agent profile、tool scope、approval**。

## 必读理论材料

- **R1（Day 1，20 分钟）**：[NumPy Copies and Views](https://numpy.org/doc/stable/user/basics.copies.html) — 闭卷前复核，不做新笔记。
- **R2（Day 2，20 分钟）**：[Dive into Deep Learning: Convolutions](https://d2l.ai/chapter_convolutional-neural-networks/conv-layer.html) — shape 与 cross-correlation。
- **R3（Day 3，20 分钟）**：[PyTorch Numerical Accuracy](https://docs.pytorch.org/docs/stable/notes/numerical_accuracy.html) — comparison limitations。
- **S1（Day 4，20 分钟）**：[Custom agents configuration](https://docs.github.com/en/copilot/reference/custom-agents-configuration) — profile fields、tools、prompt body 与 invocation control。

80 分钟均计入 Day 1–4；没有 podcast/blog 必修项。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **仅当全部 Gate blockers 已关闭时做 30–45 分钟，否则取消**：从 [MLOps Community Podcast](https://mlops.community/podcast) 选择一集带 show notes、主题为 reproducibility、evaluation 或 production ML failure 的节目。
- 在 `notes/feed/week-06.md` 写 `claim → source/evidence → scope → experiment/decision`，并说明它是否改变 M1 的一个 test/limitation；不允许用 Feed 替代复习或 Gate evidence，也不计 unlock。

## 带问题阅读

- 哪个错误最早破坏 M1：shape、address、operator convention 还是 tolerance？
- 为什么“输出接近”仍需明确 semantic contract？
- custom-agent instructions 为什么不能替代 host/tool 的实际权限？
- defense 中哪些是 observation，哪些只是 inference？

## Worked example / 手算

综合题：FP32 NCHW `(1,2,3,4)` strides=`(24,12,4,1)`，坐标 `(0,1,2,3)` element offset=`23`、byte offset=`92`；该 tensor 进入 `1×1` Conv（2 input channels、3 output channels）后 output shape=`(1,3,3,4)`。weight+bias parameters=`3×2×1×1+3=9`、FP32 parameter bytes=`36`，output activation bytes=`1×3×3×4×4=144`。若某 output expected=`2.0`、actual=`2.0003`，在 `atol=1e-4, rtol=1e-4` 下允许误差=`0.0003`；边界相等时需由实际 comparison 定义核对，不凭口头判断。

## 累计项目 increment

- `milestones/m1-foundations/defense.md`
- `notes/exams/unit-01.md`
- `notes/ai-app/week-06-custom-agent-defense.md`
- `notes/weekly/week-06.md`

## Exit ticket / unlocks

**Hard Gate：总分 ≥80/100 且所有 blocker 通过，才 unlock Unit 2。下列数字是评分权重，不是分钟；实际计时拆在 Day 5–6：**

- **Knowledge：25 分权重** — 50 分钟闭卷、此前未见的新题；首次正确率必须 ≥80%，否则为 blocker。
- **Unseen practical：30 分权重** — 未见 input/故障的独立手算、实现与定位，不复用演练答案。
- **Cumulative project：30 分权重** — M1 normalize/layout/tiny-slice clean run、boundary correctness、reproduction 与 artifact 完整性；不含 Resize。
- **Oral/design defense：15 分权重** — 10 分钟口头解释设计边界、evidence、limitations 与 custom-agent control。

未过门只重做失败类别；分数高但 blocker 未过仍判未通过。

## Day 1

**阶段：cumulative retrieval I（100 分钟）**

- **20 分钟**：读 R1。
- **35 分钟**：闭卷完成 shape/stride/view-copy/broadcast-reduction 题组，再用代码核对。
- **30 分钟**：向 M1 normalize/layout stage 注入一个 stride/channel error，定位 first mismatch。
- **15 分钟**：从 Week 05 架构提取 custom agent 的 name/description/tools/instructions/non-goals。
- **必须留下**：题组、fault-localization trace 和 custom-agent profile 草案。

## Day 2

**阶段：cumulative retrieval II（100 分钟）**

- **20 分钟**：读 R2。
- **35 分钟**：闭卷做 MatMul、Conv/Pool、softmax 与 parameter/activation bytes 题并解释 convention。
- **30 分钟**：重跑 operator/dataflow/normalization boundary cases。
- **15 分钟**：只读检查 `.github/agents/cuda-reviewer.agent.md`，对照 profile 草案记录差异。
- **必须留下**：手算、clean operator output 和 profile gap table。

## Day 3

**阶段：guided defense rehearsal（100 分钟）**

- **20 分钟**：读 R3。
- **55 分钟**：从空环境说明开始演练 M1：setup→run→first mismatch→tolerance→limitations。
- **15 分钟**：走查 known-defect、benign、missing-context 三种输入时 contract 要求的输出与停止条件。
- **10 分钟**：记录被追问但答不出的三点。
- **必须留下**：rehearsal transcript、raw commands和三种 contract walkthrough。

## Day 4

**阶段：independent mock（100 分钟）**

- **20 分钟**：读 S1。
- **60 分钟**：无笔记完成一套综合 mock：2 概念、3 手算、1 debugging lab。
- **10 分钟**：做 prompt-injection 静态/可用客户端测试，确认 read-only/no self-approval；不扩大权限。
- **10 分钟**：按 rubric 盲评并列 gaps。
- **必须留下**：mock answers、injection result、初评分与 profile gaps。

## Day 5

**阶段：Hard Gate Part 1 / Knowledge（100 分钟）**

- **20 分钟**：只修 Day 4 mock 的最低类别并用新输入复测；到时即停止 remediation。
- **50 分钟**：Knowledge exam：闭卷完成此前未见的 address、broadcast/reduction、operator/bytes、softmax/numerics 与 inference-boundary 题；原样保存首次答案（评分权重 25 分，首次正确率须 ≥80%）。
- **20 分钟**：封存 `defense.md` 的 claim→evidence→limitation、M1 exact commands 与 custom-agent contract；不再修改 summative answers。
- **10 分钟**：记录 Knowledge score/blocker，并为 Day 6 生成只含顺序与 artifact paths 的封条清单。
- **必须留下**：Knowledge 首次答案/分数、最后一次 remediation evidence、sealed defense packet。

## Day 6

**阶段：Hard Gate / M1 Defense（120 分钟）**

- **45 分钟**：Unseen practical：处理未见 normalize/layout/operator input 或故障，独立手算、实现与定位（评分权重 30 分）。
- **45 分钟**：Cumulative project：M1 clean run、boundary correctness、intermediate dump 与 artifact audit；确认 Resize 明确 deferred（评分权重 30 分）。
- **10 分钟**：Oral/design defense：解释设计、evidence、limitations 与 custom-agent contract（评分权重 15 分）。
- **20 分钟**：汇总 Day 5 Knowledge 与本日三项分数，记录 blocker、pass/fail 与 remediation；不得自行上调。
- **必须留下**：四项 rubric 证据、总分、blocker 结果、M1 defense 与 Unit 2 unlock 决定。
