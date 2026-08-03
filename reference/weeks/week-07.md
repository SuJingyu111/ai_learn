# Week 07 — `nn.Module`、State、Checkpoint、Eval 与 Agent Evaluation Design

> **Unit 2 · Model-to-Runtime CPU Vertical Slice（第 1/6 周）**  
> **课程角色**：进入模型对象与持久状态；sidecar 基于 Week 06 已冻结的 read-only custom-agent contract 设计 representative eval cases 与评分规则，暂不根据运行结果调答案。

## 开始前：硬先修与 remediation

- **硬先修**：Unit 1 Hard Gate 通过；能写 PyTorch tensor/operator fixed test。
- **先修检查**：说明 parameter、activation、checkpoint 哪两个会持久化。
- **失败时 remediation**：重看 Week 05 execution-mode 四格；用一个 `Linear(3,2)` 手列 weight/bias shape，再开始 Module。

## Observable objectives

- 定义小型 `nn.Module`，预测并 assert 每层 shape。
- 区分 parameter、buffer、activation 与 `state_dict`。
- 手算 parameter numel/bytes，并与程序一致。
- checkpoint save/load 后对 fixed input 输出一致。
- 用含 Dropout/BatchNorm 的已教学 Module 观察 `train/eval × grad/no-grad/inference`，并区分 mode state 与 gradient tracking。
- 为 custom agent 定义 outcome、case manifest、expected behavior 与 rubric。

## 术语表

需能解释：**module、forward、parameter、buffer、activation、state_dict、checkpoint、serialization、load mismatch、Dropout、BatchNorm、running statistics、training/evaluation mode、grad mode、eval case、expected behavior、rubric、representative dataset**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[PyTorch: Build the Neural Network](https://docs.pytorch.org/tutorials/beginner/basics/buildmodel_tutorial.html) — `nn.Module`、layers、parameters。
- **T2（Day 2，25 分钟）**：[PyTorch: Save, Load and Use Model](https://docs.pytorch.org/tutorials/beginner/basics/saveloadrun_tutorial.html) — `state_dict` recommended workflow。
- **T3（Day 3，10 分钟）**：[PyTorch `Module.eval`](https://docs.pytorch.org/docs/stable/generated/torch.nn.Module.html#torch.nn.Module.eval)。
- **T4（Day 3，15 分钟）**：[PyTorch `Dropout`](https://docs.pytorch.org/docs/stable/generated/torch.nn.Dropout.html) 与 [`BatchNorm2d`](https://docs.pytorch.org/docs/stable/generated/torch.nn.BatchNorm2d.html) — training/eval behavior、running statistics 与 state。
- **S1（Day 1，20 分钟）**：[Designing Agent Architecture and SDLC Integration](https://learn.microsoft.com/en-us/training/modules/design-agent-architecture-integration/) — inputs/outputs/success criteria、observability 与 reliable workflow evidence。

90 分钟已计入 Day 1–3。Podcast/blog 不作必修。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **30–45 分钟；先修或 Gate 落后时取消**：从 [Simon Willison’s Weblog](https://simonwillison.net/) 选择一篇近 12 个月、主题为 agents、evals、tools 或 reproducible artifacts 的文章；优先有可运行 demo 或引用原始来源的文章。
- 在 `notes/feed/week-07.md` 写 `claim → source/evidence → scope → experiment/decision`，并把一个 claim 映射为 frozen eval case；观点不改变已冻结 expected label。不计 unlock。

## 带问题阅读

- `state_dict` 为什么不包含 Python model class？
- buffer 与 parameter 的训练/持久化边界是什么？
- load shape mismatch 为什么应失败而不是静默适配？
- eval expected behavior 如何直接来自 Week 06 contract，而不是看完输出后改答案？

## Worked example / 手算

`Linear(3,2)` weight shape=`(2,3)` 有 6 个参数，bias shape=`(2,)` 有 2 个，共 8 个 FP32=`32 bytes`。若前接 `Conv2d(1,4,kernel_size=3)`，Conv parameters=`4×1×3×3+4=40`；总 parameters=`48`，不包含 activation。

## 累计项目 increment

- `labs/ml/week07_model_state.py`
- `artifacts/models/week07_checkpoint.pt`（可由脚本生成，不要求提交大文件）
- `notes/concepts/model-state-checkpoint.md`
- `ai-app/evals/week-07-eval-design.md`

## Exit ticket / unlocks

**形成性**：parameter 手算一致、checkpoint clean reload assertion 通过、Dropout/BatchNorm 的 train/eval 与 grad-mode evidence 正确、agent eval manifest/rubric 已冻结且覆盖 happy/negative cases。通过 unlock Week 08 baseline run。

## Day 1

**阶段：理论 + module model（100 分钟）**

- **20 分钟**：读 T1。
- **25 分钟**：完成顶部 parameter worked example。
- **35 分钟**：guided 定义 `Conv→ReLU→AdaptivePool→Flatten→Linear`，固定 seed，assert batch 1/2 shape。
- **20 分钟**：读 S1，提取 outcome/operational constraints/qualitative/quantitative signals。
- **必须留下**：model summary、手算与 eval-design field table。

## Day 2

**阶段：worked state/checkpoint（100 分钟）**

- **25 分钟**：读 T2。
- **25 分钟**：手列 named parameters/buffers/state dict expected keys。
- **40 分钟**：打印 name/shape/dtype/numel/bytes，保存/重建/load，对 fixed input assert equality。
- **10 分钟**：从 Week 06 contract 写 eval 的 success/non-goals。
- **必须留下**：state table、checkpoint command/reload assertion 和 eval scope。

## Day 3

**阶段：guided failure lab（100 分钟）**

- **10 分钟**：读 T3。
- **15 分钟**：读 T4，先预测 Dropout output 与 BatchNorm running statistics 在 train/eval 的变化。
- **45 分钟**：用含 Dropout/BatchNorm 的最小 Module 比较 `train/eval × grad/no-grad/inference` outputs/state；故意改 architecture dimension 并保存 load error。
- **20 分钟**：建立 8-case manifest：known defect、benign、injection、missing context、permission request 等，不运行。
- **10 分钟**：记录版本、seed 与 error。
- **必须留下**：mode table、受控 load failure、8-case manifest。

## Day 4

**阶段：independent lab（100 分钟）**

- **70 分钟**：独立实现 clean process `create→load→eval→inference`；测试 missing key、unexpected key 或 corrupted metadata，不吞异常。
- **20 分钟**：创建 `week-07-eval-design.md`，冻结 expected finding/no-finding、severity、residual-risk 与评分规则。
- **10 分钟**：复跑 exact command。
- **必须留下**：clean lifecycle、failure matrix 与 frozen eval design。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：写 object/data lifetime 图并重跑 checkpoint fixed cases。
- **20 分钟**：审计 8 cases 是否覆盖 outcome/tool/context/environment/policy failure；禁止看到输出后改 expected label。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：lifetime 图、coverage audit 和 exit gaps。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：修复 gap，clean-run model/checkpoint/modes。
- **25 分钟**：完成 `model-state-checkpoint.md`。
- **15 分钟**：闭卷解释 frozen contract→eval design→Week 08 baseline 的依赖顺序。
- **15 分钟**：填写 `notes/weekly/week-07.md` 和 Week 08 graph question。
- **必须留下**：形成性 Exit ticket、概念笔记、clean evidence 和 unlock 状态。
