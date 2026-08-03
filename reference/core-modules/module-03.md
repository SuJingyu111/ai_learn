# Core Module 03 — PyTorch Inference 与模型生命周期

## 本周目标

理解一个模型从定义、parameter 初始化、checkpoint 到 inference 的生命周期，以及
training mode 与 evaluation mode 对执行语义的影响。

## 核心问题

- module、parameter、buffer、state dict 是什么？
- `train()`、`eval()`、gradient disabled 分别改变什么？
- model load、warm-up、first run 和 steady state 为什么要分开？
- CPU/GPU tensor device 不一致会发生什么？

## 每日计划

### Day 1 — 定义可解释的小模型

- **10 分钟**：写 model input/output contract 和逐层 expected shape。
- **20 分钟**：阅读 PyTorch `nn.Module` 与 forward 的最小示例。
- **50 分钟**：创建 `labs/ml/week03_model.py`，实现
  `Conv→ReLU→AdaptivePool→Flatten→Linear`，使用固定 seed。
- **10–30 分钟**：对 batch 1/2 运行 forward，assert output shape 和 finite values。
- **必须留下**：model summary、shape assertions 和运行命令。

### Day 2 — Parameters、Buffers 与 State

- **10 分钟**：预测模型 parameter 数量和 bytes。
- **20 分钟**：阅读 `named_parameters`、`named_buffers`、`state_dict`。
- **50 分钟**：打印 name/shape/dtype/numel/bytes；手算 Conv/Linear 参数并比较。
- **10–30 分钟**：加入或识别一个 buffer，解释它与 trainable parameter 的区别。
- **必须留下**：parameter table、总 bytes 和手算核对。

### Day 3 — 最小 Training Loop，只为理解 Lifecycle

- **10 分钟**：写出 forward→loss→backward→optimizer step 的顺序。
- **20 分钟**：阅读 loss、optimizer、zero_grad 的最小解释。
- **50 分钟**：用 synthetic data 训练 20–50 steps，记录首尾 loss；不调参追求
  accuracy。
- **10–30 分钟**：确认一次 step 前后至少一个 parameter 变化；然后停止训练主题。
- **必须留下**：最小 loop、loss log 和“training 非本项目主线”的边界说明。

### Day 4 — Checkpoint Save/Load 与确定性

- **10 分钟**：预测保存 state dict 是否包含 model code。
- **20 分钟**：阅读 save/load state dict 的官方示例。
- **50 分钟**：保存 checkpoint，重新创建 model 并加载；对 fixed input 比较输出。
- **10–30 分钟**：故意修改一个 architecture dimension，观察 load error。
- **必须留下**：checkpoint 生成命令、reload assertion、版本/seed；大二进制不必
  提交，但生成步骤必须可重复。

### Day 5 — Eval、Inference Mode 与可靠计时

- **10 分钟**：列出 `train/eval/no_grad/inference_mode` 的预期差异。
- **20 分钟**：只读对应 API 和计时注意事项。
- **50 分钟**：分别测 model creation、checkpoint load、first forward、warm-up 后
  50 次 forwards；同步/计时方法写清。
- **10–30 分钟**：将 output validation 和打印移出 timed region，计算
  median/p95。
- **必须留下**：cold/warm table 和计时代码；不把 host Python 数据称为 Orin 性能。

### Day 6 — Model Lifecycle 固化

- **15 分钟**：闭卷画 model definition→state→load→eval→warm-up→inference。
- **45 分钟**：写 `notes/concepts/model-lifecycle.md`，加入 object/data lifetime。
- **20 分钟**：运行 clean checkpoint→inference 流程，确认新进程可重现。
- **10–30 分钟**：weekly review；列出导出 ONNX 前需要锁定的 input name/shape。
- **必须留下**：lifecycle 图、clean reproduction 和 Week 4 第一项任务。

## 最小实验

对相同输入分别运行：

- model 刚创建；
- checkpoint reload；
- training mode；
- evaluation mode + no-grad/inference mode；
- warm-up 后 steady state。

记录语义差异和 timing 限制，不把 Python microbenchmark 当成 Orin 结论。

## 交付

- 小模型定义与可重建 checkpoint 步骤。
- `notes/concepts/model-lifecycle.md`
- cold/warm timing 表。

## 完成标准

- [ ] 能解释 parameter、activation 和 checkpoint。
- [ ] inference 使用正确的 evaluation/gradient 设置。
- [ ] build/load/warm-up/steady-state timing 已分开。
- [ ] 能指出训练知识中哪些不是当前主线。
