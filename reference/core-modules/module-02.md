# Core Module 02 — Inference Operators 与 CNN 数据流

## 本周目标

理解 runtime 经常执行的 operators，不追求训练模型能力。重点是输入输出 shape、
计算量、memory traffic、layout 和可验证语义。

## 核心概念

- MatMul/GEMM、Conv2D、bias、ReLU/GELU。
- pooling、batch/layer normalization、softmax。
- parameter、activation、intermediate tensor。
- operator、kernel 和 fusion 不是一一对应。

## 每日计划

### Day 1 — MatMul/GEMM 语义

- **10 分钟**：写出 `M×K` 与 `K×N` 的输出 shape 和一个元素的计算式。
- **20 分钟**：阅读 PyTorch/NumPy MatMul 语义，只关注二维与 batch 区别。
- **50 分钟**：创建 `labs/ml/week02_operators.py`；用三重 loop 实现
  `2×3 @ 3×4` reference，与 NumPy/PyTorch 比较。
- **10–30 分钟**：计算乘加次数、input/output bytes；测试 incompatible shapes。
- **必须留下**：hand calculation、reference、correctness assertion 和 byte/FLOP 表。

### Day 2 — Conv2D 手算

- **10 分钟**：根据 `H,W,K,padding,stride` 写输出尺寸公式。
- **20 分钟**：只读 Conv2D 输入/weight shape 和 padding/stride 文档。
- **50 分钟**：实现单 batch、单 channel 的小 Conv reference；使用 `4×4` input、
  `3×3` kernel，先做 no padding/stride 1。
- **10–30 分钟**：加入 padding 1 和 stride 2；与 framework 输出比较，确认采用
  cross-correlation 还是翻转 kernel。
- **必须留下**：三个手算 output values、shape 公式和 convention 结论。

### Day 3 — 组合一个小型 CNN 数据流

- **10 分钟**：预测 `Conv→ReLU→Pool` 每层 shape。
- **20 分钟**：阅读对应 operator API 的 shape 参数。
- **50 分钟**：创建 `labs/ml/week02_cnn_trace.py`；构造固定 weights/input，
  逐层打印 name、shape、stride、dtype、min/max。
- **10–30 分钟**：用 hooks 或显式 forward stages 验证预测；检查 ReLU 前后负值。
- **必须留下**：完整 shape trace 和一次逐层 correctness comparison。

### Day 4 — Parameter、Activation 与 Memory

- **10 分钟**：区分 parameter、input、activation、output 的 lifetime。
- **20 分钟**：根据 Day 3 shapes 手算每层 tensor bytes。
- **50 分钟**：修改 batch/channel/spatial dimensions，写脚本生成 shape 和 bytes
  表；至少包含 batch 1/4、FP32/FP16 的理论值。
- **10–30 分钟**：解释哪部分随 batch/shape 增长，哪些 weights 可复用。
- **必须留下**：memory worksheet，不需要声称等于 framework peak memory。

### Day 5 — Operator、Kernel 与 Fusion 观察

- **10 分钟**：预测三个 operators 是否对应三个 kernels，注明“待验证”。
- **20 分钟**：阅读 concept map 中 operator/runtime/fusion，并了解 profiler 基本
  输出；不深入 CUDA 指标。
- **50 分钟**：对小/较大输入运行可用的 PyTorch profiler 或简单计时，保存
  operator names 和调用次数。
- **10–30 分钟**：比较 warm-up 前后；只描述观测，不从 CPU profiler推导 Orin。
- **必须留下**：raw profiler/计时摘要和“operator≠kernel”的具体例子。

### Day 6 — CNN 执行链与周复盘

- **15 分钟**：闭卷回答本周核心概念。
- **45 分钟**：写 `notes/concepts/inference-operators.md`，画
  input→operator→activation→operator→output。
- **20 分钟**：录音或口述 90 秒解释 Conv、MatMul、fusion。
- **10–30 分钟**：运行本周 scripts，填写 weekly review；将未理解的
  normalization/softmax 留为 backlog，不扩张本周。
- **必须留下**：概念笔记、90 秒提纲、所有 correctness 命令。

## Correctness

- 固定小整数输入，避免随机数掩盖 indexing 错误。
- 明确 convolution/correlation convention。
- 比较 NumPy reference 与 PyTorch 输出，并记录 tolerance。

## 交付

- `notes/concepts/inference-operators.md`
- 一个 `Conv → ReLU → Pool` shape trace。
- 一张 operator / tensor / kernel 区分图。

## 完成标准

- [ ] 能解释 Conv/MatMul 为什么常成为主要计算。
- [ ] 能由 operator 参数推出输出 shape。
- [ ] 能说明 fusion 可能减少什么，但不声称一定更快。
- [ ] reference 与 framework 输出在明确 tolerance 内一致。
