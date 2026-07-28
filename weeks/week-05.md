# Week 05 — Numerical Representation、Tolerance、Training/Inference Boundary 与 M1 Integration

> **Unit 1 · ML Systems Foundations（第 5/6 周）**  
> **课程角色**：把前四周的“shape 正确”提升为“数值与 inference boundary 正确”，并冻结 Milestone 1 的 `normalize→HWC→CHW→tiny operator/model slice` contract；M1 明确不含 Resize。sidecar 形成 agent architecture artifact。

## 开始前：硬先修与 remediation

- **硬先修**：Week 01–04 的 array/stride/MatMul/normalize/Conv/Pool fixed cases 可重跑；能输出 first mismatch。
- **先修检查**：解释为何浮点结果不应默认 bit-exact。
- **失败时 remediation**：先用十进制 `0.1+0.2` 和 FP32/FP64 同一求和做观察；若仍把 tolerance 当“放宽测试”，先只做 exact integer cases，再恢复 floating comparison。

## Observable objectives

- 解释 FP16/FP32/FP64 的 precision/range 与 rounding，不背位宽细节代替实验。
- 正确使用 absolute/relative tolerance，并报告 max error 与 failing coordinate。
- 从 logits 手算 softmax，并区分 normalization、activation 与 probability conversion。
- 手算 fixed operator slice 的 parameter bytes 与各阶段 activation bytes。
- 概念区分 training/evaluation mode、gradient disabled 与 `inference_mode()`；含 Dropout/BatchNorm 的 stateful 实验留到 Week 07。
- 将 `normalize→HWC→CHW→Conv/ReLU/Pool→logits→softmax` 整合为 M1 clean reproduction，不实现或声称 Resize。
- 画 model/instructions/tools/environment/control 的 agent architecture 与 trust boundaries。

## 术语表

需能解释：**floating point、rounding、precision、range、absolute error、relative error、atol、rtol、determinism、logit、softmax、probability、normalization、parameter bytes、activation bytes、training mode、evaluation mode、gradient、inference mode、trust boundary、least privilege**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[PyTorch Numerical Accuracy](https://docs.pytorch.org/docs/stable/notes/numerical_accuracy.html) — floating-point non-associativity、extremal values。
- **T2（Day 2，20 分钟）**：[PyTorch Autograd Mechanics: Locally disabling gradient computation](https://docs.pytorch.org/docs/stable/notes/autograd.html#locally-disabling-gradient-computation) — no-grad 与 inference mode。
- **T3（Day 2，15 分钟）**：[Dive into Deep Learning: Softmax Regression](https://d2l.ai/chapter_linear-classification/softmax-regression.html) — 只读 logits→softmax probabilities 与 normalization，不进入训练推导。
- **S1（Day 1，15 分钟）**：[Designing Agent Architecture and SDLC Integration](https://learn.microsoft.com/en-us/training/modules/design-agent-architecture-integration/) — responsibilities、inputs/outputs、planning/execution 与 environment boundary。
- **S2（Day 3，20 分钟）**：[GitHub Copilot custom agents](https://docs.github.com/en/copilot/concepts/agents/copilot-cli/about-custom-agents) — specialization 与 tools。

90 分钟计入 Day 1–3。Podcast/blog 可选但不计必修。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **30–45 分钟；先修或 Gate 落后时取消**：从 [Chip Huyen](https://huyenchip.com/) 选择一篇讨论 AI engineering、inference/evaluation 或 production evidence 的文章；优先有明确发布日期、示例和 limitation 的文章。
- 在 `notes/feed/week-05.md` 写 `claim → source/evidence → scope → experiment/decision`，并把能由本周 M1 fixture 验证的 claim 转成一个 test idea。可散步听访谈，不计 unlock。

## 带问题阅读

- 运算顺序改变为何可能改变末位？
- `eval()` 与 `no_grad()` 为什么不能互相替代？
- softmax 与 channel-wise input normalization 各自归一化什么，为什么不能互换？
- parameter bytes 与 activation bytes 哪一个随 batch/spatial shape 增长？
- tolerance 应来自 dtype/operation/scale 的什么依据？
- instructions 与实际 tool permission 分别约束什么？

## Worked example / 手算

Expected=`1000.0`、actual=`1000.1`：absolute error=`0.1`，relative error=`0.1/1000=1e-4`。若 `atol=1e-3, rtol=2e-4`，允许误差=`1e-3+2e-4×1000=0.201`，因此通过；对 expected 接近 0 时必须主要审视 `atol`，不能只除以极小数。

若 logits=`[0, ln(2)]`，`exp(logits)=[1,2]`，softmax=`[1/3,2/3]`，和为 1；这是 output probability conversion，不是 Week 03 的 input channel normalize。若 fixed Conv 有 5 个 FP32 parameters、阶段 activations 共 14 个 FP32 elements，则两者分别为 `20 bytes` 与 `56 bytes`；batch 翻倍只使后者翻倍。

## 累计项目 increment

- `labs/ml/week05_numerics_lifecycle.py`
- `notes/concepts/numerical-and-inference-contract.md`
- `milestones/m1-foundations/report.md`
- `notes/ai-app/week-05-agent-architecture.md`

## Exit ticket / unlocks

**形成性**：解释 execution-mode 边界、手算两组 atol/rtol 与一组 softmax、parameter/activation bytes 可复算、M1 fixed cases clean-run 且明确无 Resize claim、architecture 图含两个 trust boundaries。完成后 unlock Week 06 defense；任何 correctness/tolerance 缺口是 Week 06 blocker。

## Day 1

**阶段：理论 + numerical model（100 分钟）**

- **20 分钟**：读 T1，记录 non-associativity 与 overflow/underflow 风险。
- **30 分钟**：逐步做顶部 tolerance worked example，再做 expected≈0 与 negative value 两题。
- **35 分钟**：guided lab：比较 FP16/FP32/FP64 的固定 reduction/MatMul，输出 abs/rel/max error。
- **15 分钟**：读 S1，列 agent architecture 五类组件与责任。
- **必须留下**：三道手算、dtype error table 和组件清单。

## Day 2

**阶段：worked inference boundary（100 分钟）**

- **20 分钟**：读 T2。
- **15 分钟**：读 T3，手算顶部 softmax 并说明数值稳定实现为何会减去 max logit。
- **20 分钟**：手画 `training/evaluation purpose × grad enabled/disabled` 四格；明确 stateful module 行为需到 Week 07 实验。
- **35 分钟**：guided functional slice：fixed HWC input 做 normalize/HWC→CHW，再走 fixed-weight Conv/ReLU/Pool/logits/softmax；逐阶段记录 shape/numel/bytes。
- **10 分钟**：列出 `eval()`、no-grad、inference mode 各自保证与不保证；不创建 Dropout/BatchNorm module。
- **必须留下**：四格边界、softmax 手算、functional slice outputs/bytes 和 deferred-to-W7 标记。

## Day 3

**阶段：guided M1 integration（100 分钟）**

- **60 分钟**：把 Week 01–04 fixed cases汇成 `synthetic HWC→normalize→HWC→CHW→fixed Conv/ReLU/Pool→logits→softmax→intermediate dump` clean runner；统一 first mismatch、dtype/shape/tolerance report，明确没有 Resize。
- **20 分钟**：读 S2，只读检查 `.github/agents/cuda-reviewer.agent.md` 的 description/tools/instructions。
- **10 分钟**：在 architecture 图标明 untrusted issue/PR/log input。
- **10 分钟**：保存命令与环境版本。
- **必须留下**：M1 runner 首版、custom-agent contract 表和 trust-boundary 标注。

## Day 4

**阶段：independent integration（100 分钟）**

- **70 分钟**：独立加入 boundary cases：empty/one/odd/non-contiguous/bad dtype/channel mismatch/zero std；每项定义 exact 或 tolerance comparison，且不得悄悄插入 Resize。
- **20 分钟**：完成 `week-05-agent-architecture.md`，含 model/instructions/tools/environment/control、approval 与 least privilege。
- **10 分钟**：clean-run 并冻结 raw output。
- **必须留下**：boundary matrix、architecture 图与 clean output。

## Day 5

**阶段：M1 rehearsal + formative review（100 分钟）**

- **60 分钟**：写 `milestones/m1-foundations/report.md`：normalize/layout/operator/model-slice contracts、softmax 与 bytes worksheet、correctness evidence、limitations、reproduction；显式列出 `Resize deferred to W21–W24 / M4`。
- **20 分钟**：用 read-only reviewer contract 检查一个已知错误 fixture；不授权 edit/merge/shell。
- **20 分钟**：完成 Exit ticket 初测，列 Week 06 blockers。
- **必须留下**：M1 report、review fixture/result 和 blocker list。

## Day 6

**阶段：review / defense unlock（100 分钟）**

- **45 分钟**：修复所有 correctness/tolerance blockers，clean-run M1。
- **25 分钟**：闭卷解释 numerical contract、normalization/softmax、parameter/activation bytes 与 training/inference boundary。
- **15 分钟**：闭卷重画 agent architecture。
- **15 分钟**：填写 `notes/weekly/week-05.md` 与 Week 06 defense order。
- **必须留下**：形成性 Exit ticket、M1 candidate、defense agenda 和 unlock 状态。
