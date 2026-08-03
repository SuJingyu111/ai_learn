# Core Module 04 — ONNX Graph 与 ONNX Runtime CPU Baseline

## 本周目标

把 Week 3 小模型导出为 ONNX，观察 graph、initializers、shapes 和 operators，并在
ONNX Runtime CPU provider 上建立第一个 runtime correctness/benchmark 闭环。

## 每日计划

### Day 1 — 冻结 Export Contract

- **10 分钟**：记录 Week 3 checkpoint、model input/output shape/dtype/name。
- **20 分钟**：阅读 PyTorch ONNX exporter 的最小导出路径和 opset 概念。
- **50 分钟**：创建 `labs/ml/week04_export_onnx.py`，导出 fixed-shape model；
  使用 fixed seed/input 并保存 reference input/output。
- **10–30 分钟**：运行 ONNX checker；记录 PyTorch/ONNX/opset 版本。
- **必须留下**：export script、model 生成路径、checker 结果和 contract。

### Day 2 — 读取 ONNX Graph

- **10 分钟**：不看 graph，预测 node types 和 initializers。
- **20 分钟**：阅读 ONNX graph/node/initializer/value info 概念。
- **50 分钟**：创建 `labs/ml/week04_inspect_onnx.py`，打印 graph inputs/outputs、
  nodes、initializers、known shapes。
- **10–30 分钟**：统计 node type 和 parameter bytes，与 Week 3 parameter table 比较。
- **必须留下**：graph summary，解释不一致而不强行要求 node 数等于 module 数。

### Day 3 — Module→Graph Mapping

- **10 分钟**：选择两个 PyTorch modules，预测它们如何映射。
- **20 分钟**：查看 exporter/ONNX operator 定义中实际需要的部分。
- **50 分钟**：画 model module、ONNX nodes、initializers 的 mapping；标出
  Flatten/shape-related nodes。
- **10–30 分钟**：用小幅模型改动重新导出一次，观察 graph diff。
- **必须留下**：mapping 表和一个“module 不等于 node”的证据。

### Day 4 — ORT CPU Correctness

- **10 分钟**：写 provider、input/output names 和 tolerance。
- **20 分钟**：阅读 ORT CPU quick start，只关注 session/run/providers。
- **50 分钟**：创建 `labs/ml/week04_ort_cpu.py`；加载 saved input，明确指定 CPU
  provider，打印 session providers，运行 inference。
- **10–30 分钟**：与 PyTorch reference 比较 max abs/relative error；测试错误 dtype
  和 shape。
- **必须留下**：correctness report、provider list、两种 failure messages。

### Day 5 — Runtime Timing Boundary

- **10 分钟**：列出 session creation、first run、warm-up、steady-state 四段。
- **20 分钟**：检查 benchmark methodology。
- **50 分钟**：分别测 session creation、first run、10 次 warm-up、100 次 steady
  state；保存 raw samples。
- **10–30 分钟**：计算 median/p95/min，确认验证和 logging 位于计时外。
- **必须留下**：CSV/JSON 或表格、环境、iterations 和 included/excluded costs。

### Day 6 — Week 4 Gate

- **15 分钟**：闭卷解释 PyTorch model→ONNX graph→ORT provider。
- **35 分钟**：写 `notes/concepts/model-graph-runtime.md`。
- **20 分钟**：从 clean process 执行 export→inspect→ORT correctness。
- **20–40 分钟**：逐项检查 Week 4 Gate；未通过项创建下周前置 backlog，不用
  CUDA 绕过 ML 语义问题。
- **必须留下**：Gate evidence、reproduction commands 和第一个 runtime 心智模型。

## 边界与失败实验

- 输入 name/shape/dtype 错误。
- dynamic batch 尚未声明时传入不同 batch。
- 缺少或不支持 operator 时记录完整错误，不随意修改模型绕过。

## 交付

- ONNX model 或可重复生成脚本。
- graph 摘要：node type、input/output shape、parameter bytes。
- PyTorch/ORT correctness 与 timing 表。

## Week 4 Gate

- [ ] 能解释 `Conv → ReLU → Pool/Resize` graph。
- [ ] PyTorch 与 ORT CPU 输出在 tolerance 内一致。
- [ ] session creation 未混入 steady-state latency。
- [ ] 能解释 graph node 为什么不保证对应一个 kernel。
