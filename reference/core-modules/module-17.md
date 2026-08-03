# Core Module 17 — ONNX Runtime CUDA EP 与 I/O Binding

## 本周目标

从 ORT CPU baseline 进入 CUDA Execution Provider，确认 provider assignment、
fallback、device I/O、copy 和 user stream，而不是只看到 GPU provider 名字。

## 每日计划

### Day 1 — ORT/JetPack Compatibility 决策

- **10 分钟**：采集实际 JetPack/L4T/CUDA/cuDNN/Python architecture/version。
- **25 分钟**：只查对应 ORT CUDA EP requirements 和 Jetson 可用 package/build 路径。
- **45 分钟**：写 `labs/runtime/ort-environment.md`，列候选 ORT version、来源、
  CUDA/cuDNN match、是否需 source build。
- **10–30 分钟**：安装前先保存当前 environment；若安装/构建超时，记录精确 blocker。
- **必须留下**：version decision 和可复现 install/build command，不使用不明 wheel。

### Day 2 — CUDA EP Correctness

- **10 分钟**：复用 Week 4 fixed model/input/reference/tolerance。
- **20 分钟**：打印 `get_available_providers()`，明确 request priority。
- **50 分钟**：创建 `labs/runtime/week17_ort_cuda.py`，建立 CUDA session，打印实际
  session providers 并运行 inference。
- **10–30 分钟**：比较 PyTorch/ORT CPU/ORT CUDA；错误或 fallback 时停止性能结论。
- **必须留下**：provider lists、correctness table、环境和 exact command。

### Day 3 — Provider Assignment 与 Fallback

- **10 分钟**：预测每个 graph node 由哪个 EP 执行。
- **20 分钟**：了解 ORT profiling output 和 provider assignment 的观察方式。
- **50 分钟**：开启 profiling，运行少量 iterations，解析 node/provider/duration；
  若全部支持，另外用受控 unsupported model/setting理解 fallback。
- **10–30 分钟**：标出 CPU/GPU boundary 和可能的 copies。
- **必须留下**：node/provider table、fallback evidence 或“未发生”的证据。

### Day 4 — I/O Binding

- **10 分钟**：画普通 `Run()` 的 host input→device→host output path。
- **20 分钟**：阅读 ORT I/O Binding input/output memory contract。
- **50 分钟**：实现 device input/output binding；先只绑定 output 或 input，再两者都
  绑定，避免一次出现多个问题。
- **10–30 分钟**：比较相同 output requirement 下的 correctness/copies/latency。
- **必须留下**：default vs binding timeline假设、代码、raw data。

### Day 5 — User Stream 与 Lifetime

- **10 分钟**：写 ORT session、stream、OrtValue/device buffer 的 lifetime。
- **20 分钟**：阅读 `user_compute_stream` 与 allocator/provider option constraints。
- **50 分钟**：将现有 CUDA stream 传给 ORT，或先构造最小 sample；明确 output-ready
  boundary，不混用错误 stream。
- **10–30 分钟**：用 event/Nsight 验证 ordering；若版本不支持，记录接口差异。
- **必须留下**：stream integration、lifetime diagram 和 version limitation。

### Day 6 — ORT Runtime Report

- **15 分钟**：运行 CPU/CUDA/default I/O/bound I/O 的公平 correctness。
- **45 分钟**：用 Nsight Systems 捕获代表 timeline，标 H2D/compute/D2H/sync。
- **20 分钟**：更新 concept note：provider、I/O Binding、user stream、fallback。
- **10–30 分钟**：weekly review；选择 TensorRT Week 18 的固定 ONNX model。
- **必须留下**：ORT report、raw timeline evidence 和 unresolved compatibility issues。

## Correctness 与测量

- CPU、ORT CUDA 使用相同 fixed input。
- session creation、first run、warm-up、steady state 分开。
- provider fallback 不能静默接受。
- I/O Binding 前后必须比较相同 device/host output requirement。

## 交付

- ORT compatibility/environment note。
- provider assignment/profiling 摘要。
- default I/O vs I/O Binding latency/copy 对照。

## 完成标准

- [ ] 实际 provider 与每个 graph segment 可解释。
- [ ] 输出在 tolerance 内一致。
- [ ] hidden H2D/D2H 已定位。
- [ ] user stream/buffer lifetime contract 明确。
