# Core Module 20 — Custom CUDA Preprocessing Runtime Integration

## 本周目标

将 resize、normalize、layout conversion 与 runtime 串接，避免不必要的 device→host
→device round-trip，并统一 stream、buffer 和 shape contract。

## 每日计划

### Day 1 — Capstone Preprocessing Contract

- **10 分钟**：选择固定 model/input source，不再更换模型。
- **20 分钟**：从 model metadata 写 source image→model tensor 的 shape/layout/dtype/
  color order/range/normalization/resize contract。
- **50 分钟**：创建 CPU/PyTorch reference，保存 3 类 fixed inputs 和 expected tensor。
- **10–30 分钟**：对 corner pixels/channels 手算，避免 RGB/BGR、scale、layout 错误。
- **必须留下**：preprocessing contract、reference tensors 和 tolerance。

### Day 2 — CUDA Preprocessing Stages

- **10 分钟**：画 resize→normalize→HWC→CHW 的 intermediates 和 bytes。
- **20 分钟**：决定先串联现有正确 kernels，不立刻 fusion。
- **50 分钟**：实现/组合 CUDA stages，复用 stream 和 preallocated buffers。
- **10–30 分钟**：每个 stage 可独立拷回验证，再做 end-stage comparison。
- **必须留下**：stage pipeline、per-stage/full correctness 和 buffer sizes。

### Day 3 — Direct Runtime Input

- **10 分钟**：画当前 CUDA output→runtime input 是否发生 host round-trip。
- **20 分钟**：选择 ORT I/O Binding 或 TensorRT direct device address 主路径。
- **50 分钟**：将 preprocessing output buffer 绑定为 inference input，保持 shape/dtype
  contract。
- **10–30 分钟**：与 CPU preprocessing + runtime reference 比较 final output。
- **必须留下**：device-resident path、end-to-end correctness 和 eliminated copy。

### Day 4 — Stream/Dependency Integration

- **10 分钟**：列 preprocessing、runtime、postprocess 分别使用的 stream。
- **20 分钟**：设计 same-stream 或 event dependency，明确谁拥有 stream。
- **50 分钟**：实现 ordering，移除仅为“确保正确”加入的全局同步。
- **10–30 分钟**：用 event/Nsight 验证 runtime 只在 input-ready 后执行、output-ready
  后才读取。
- **必须留下**：stream/lifetime diagram 和 synchronization reasons。

### Day 5 — Baseline vs CUDA vs Fused 决策

- **10 分钟**：冻结 CPU preprocessing、separate CUDA kernels 两个 baselines。
- **45 分钟**：采集 stage/end-to-end latency 和 memory traffic。
- **25 分钟**：只有 profiler 显示 intermediates/launch 重要才实现小型 fused variant；
  否则补强 separate path。
- **10–30 分钟**：所有 variants 运行相同 final correctness。
- **必须留下**：decision/evidence/raw data；不做 fusion 是允许的。

### Day 6 — Runtime Integration Gate

- **15 分钟**：clean run fixed/dynamic（适用时）和 FP32/FP16 paths。
- **45 分钟**：Nsight Systems 捕获 preprocessing→inference→postprocess timeline。
- **20 分钟**：标出所有 copy、sync、allocation 和 unexplained gaps。
- **10–30 分钟**：完成 Week 20 Gate、weekly review 和 capstone backlog。
- **必须留下**：device-resident integration Gate 和 baseline timeline。

## Optional

只有当 graph/runtime 确实不能表达所需 operator，或 integration case 能体现能力时，
才实现小型 TensorRT plugin。Plugin 不是为了增加关键词。

## 交付

- preprocessing→runtime integration API。
- end-to-end correctness case。
- device buffer ownership/lifetime diagram。
- before/after copy/timeline 数据。

## Week 20 Gate

- [ ] 无未解释的 host round-trip。
- [ ] custom preprocessing 与 framework reference 一致。
- [ ] stream 和 output readiness contract 明确。
- [ ] ORT/TensorRT fixed/dynamic input 行为可解释。
