# Week 04 — Conv/Operator Dataflow 与 Agent Control Plane

> **Unit 1 · ML Systems Foundations（第 4/6 周）**  
> **课程角色**：从 MatMul 扩展到 Conv、activation、pooling 的 runtime dataflow；sidecar 把 agent components 放进 issue→plan→diff→test→review 的可审计控制平面。

## 开始前：硬先修与 remediation

- **硬先修**：MatMul reference/tests 通过；能由参数推出 output shape。
- **先修检查**：对 `4×4` input、`3×3` kernel、stride 1、padding 0 写 output=`2×2`。
- **失败时 remediation**：先用一维 sliding window 画 receptive field，再套 `floor((H+2P-K)/S)+1`；30 分钟后仍失败时只做单 channel/no padding。

## Observable objectives

- 由 kernel/padding/stride/dilation 推导 Conv2D output shape。
- 用小整数手算 cross-correlation output，并识别 convolution convention。
- 跟踪 `Conv→ReLU→Pool` 的 shape、dtype、layout、min/max 和 tensor lifetime。
- 手算 Conv parameters 与 input/intermediate/output activation bytes。
- 区分 operator、module、graph node、kernel 与 fusion。
- 画出 agent SDLC control plane，并标明 approval、evidence 和 recovery。

## 术语表

需能解释：**kernel/filter、channel、stride、padding、dilation、cross-correlation、activation、pooling、parameter、activation bytes、intermediate、operator、fusion、control plane、approval gate、artifact**。

## 必读理论材料

- **T1（Day 1，25 分钟）**：[Dive into Deep Learning: Convolutions for Images](https://d2l.ai/chapter_convolutional-neural-networks/conv-layer.html) — cross-correlation 与 feature maps。
- **T2（Day 2，15 分钟）**：[PyTorch `Conv2d`](https://docs.pytorch.org/docs/stable/generated/torch.nn.Conv2d.html) — input/weight/output shape、stride/padding/dilation。
- **T3（Day 2，15 分钟）**：[Dive into Deep Learning: Pooling](https://d2l.ai/chapter_convolutional-neural-networks/pooling.html) — max/average pooling、window、stride 与 output。
- **S1（Day 1，20 分钟）**：[Designing Agent Architecture and SDLC Integration](https://learn.microsoft.com/en-us/training/modules/design-agent-architecture-integration/) — structured task、planning/execution separation 与 PR governance。
- **S2（Day 3，20 分钟）**：[GitHub: About pull request reviews](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/reviewing-changes-in-pull-requests/about-pull-request-reviews) — review/approval/change requests。

95 分钟计入 Day 1–3；Podcast/blog 不作为教材。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **30–45 分钟；先修或 Gate 落后时取消**：从 [GitHub Blog: AI & ML](https://github.blog/ai-and-ml/) 选择一篇近 12 个月、讨论 coding agent workflow、review 或 human-in-the-loop 的文章；必须区分产品案例、建议和可验证保证。
- 在 `notes/feed/week-04.md` 写 `claim → source/evidence → scope → experiment/decision`，并提出一个可由本周 control-plane fixture 验证的反例。不计 unlock。

## 带问题阅读

- 深度学习库通常实现 convolution 还是 cross-correlation？
- pooling 的 window/stride 如何改变 shape，它为什么不是“免费缩小 tensor”？
- operator 为什么不保证对应一个 GPU kernel？
- 哪些 activation bytes 随 batch/spatial 增长，哪些 parameter bytes 可复用？
- agent 何时必须停在 approval gate，而不是自己继续？

## Worked example / 手算

Input `[[1,2,3],[4,5,6],[7,8,9]]` 与 kernel `[[1,0],[0,-1]]` 做 valid cross-correlation：左上=`1-5=-4`，右上=`2-6=-4`，左下=`4-8=-4`，右下=`5-9=-4`，output=`2×2` 且全为 `-4`。ReLU 后全为 `0`，`2×2` max-pool 后 shape=`1×1`、value=`0`。若 1 input/1 output channel 且有 bias，parameter numel=`2×2+1=5`、FP32 bytes=`20`；input/Conv-output/pool-output activation bytes 分别为 `36/16/4`。

## 累计项目 increment

- `labs/ml/week04_conv_dataflow.py`
- `notes/concepts/inference-operator-dataflow.md`
- `notes/ai-app/week-04-control-plane.md`
- `notes/ai-app/week-04-control-plane-cases.md`

## Exit ticket / unlocks

**形成性**：正确推 4 个 Conv/Pool shapes、手算与 framework/reference 一致、parameter/activation bytes 可复算、完整 trace `Conv→ReLU→Pool`，并能指出 agent/human 的每个 control-plane gate。完成 unlock Week 05。

## Day 1

**阶段：理论 + shape model（100 分钟）**

- **25 分钟**：读 T1，回答 cross-correlation convention。
- **25 分钟**：逐步复现顶部 worked example，再改变 kernel 手算一例。
- **30 分钟**：做 4 组 Conv output-shape 题并为其中两组计算 parameter/activation bytes，覆盖 padding/stride/odd size。
- **20 分钟**：读 S1，为“实现 Conv reference”写 structured plan 与 approval boundary。
- **必须留下**：两组手算、4 个 shape answers 和 structured plan。

## Day 2

**阶段：worked example → guided lab（100 分钟）**

- **15 分钟**：读 T2，记录 NCHW input/weight/output contract。
- **15 分钟**：读 T3，手算一个 max-pool 与 average-pool output。
- **20 分钟**：展开一个 Conv output cell 的 channel/kernel loops。
- **35 分钟**：guided 实现 single-batch/single-channel Conv reference 与 `2×2` pool reference，再加一个 padding/stride case。
- **15 分钟**：把 plan 的每步映射为 issue/branch/diff/test/review artifact。
- **必须留下**：Conv/Pool loop trace、references、三个 hand outputs 与 artifact map。

## Day 3

**阶段：guided dataflow lab（100 分钟）**

- **60 分钟**：构造 fixed weights/input 的 `Conv→ReLU→Pool`，逐层打印 name/shape/stride/dtype/min/max/numel/bytes，与预测核对。
- **20 分钟**：读 S2，区分 comment、approval、request changes 与 merge authority。
- **10 分钟**：给 control plane 加 CI fail 与 reviewer unavailable recovery。
- **10 分钟**：保存 exact commands 和 tolerance。
- **必须留下**：完整 shape trace、review responsibility 表和两条 recovery path。

## Day 4

**阶段：independent lab（100 分钟）**

- **70 分钟**：独立扩展 multi-channel 或明确 unsupported；先预测 parameter/activation bytes，再测试 `1×1`、odd spatial、negative input、bad channel，比较 NumPy/PyTorch。
- **20 分钟**：在 `week-04-control-plane.md` 完成 `issue→scoped plan→diff→tests→read-only review→human merge` 图。
- **10 分钟**：运行 first-mismatch reporter。
- **必须留下**：independent cases、control-plane 图和 mismatch evidence。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：写 operator/tensor/lifetime/bytes 表；对小/较大 input 或 batch 做非性能性的 activation-memory 比较，不从 CPU 推导 Orin kernel/fusion。
- **20 分钟**：做 5 个 control-plane cases：模糊 issue、越界 edit、缺 tests、自批自合、失败恢复。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：lifetime 表、5 个场景题与 exit 错因。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：clean-run Conv/dataflow cases，闭卷重做 shape 与 output。
- **25 分钟**：完成 `inference-operator-dataflow.md`，标明 operator≠kernel 的证据边界。
- **15 分钟**：闭卷重画 control plane。
- **15 分钟**：填写 `notes/weekly/week-04.md`，写 Week 05 numerical risk。
- **必须留下**：形成性 Exit ticket、概念笔记、clean evidence 与 unlock 状态。
