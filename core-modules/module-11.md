# Core Module 11 — CUDA Resize Correctness

## 本周目标

让 CUDA resize 与 Week 10 CPU oracle 共享语义与测试数据，建立真正的 GPU test
target，而不是只依赖 demo 的一个固定输入。

## 每日计划

### Day 1 — CUDA Kernel 静态审阅

- **10 分钟**：从 Week 10 contract 列出 CUDA 必须匹配的语义。
- **25 分钟**：逐行审阅 `resize_cuda.cu` 的 x/y/channel index、grid rounding、
  stride、clamp、floor 和 error return。
- **45 分钟**：为 `1×1`、`33×9`、3 channels 手算首/末 thread 地址；列 integer
  overflow/alias/lifetime residual risks。
- **10–30 分钟**：写审阅记录，先不做性能修改。
- **必须留下**：symbol/line-based checklist 和最小修复清单。

### Day 2 — GPU Test Harness

- **10 分钟**：设计 host/device buffer RAII、copy、sync 和 error observation。
- **20 分钟**：决定 CUDA tests 在无 GPU/无 CUDA 时如何明确 skip/not build。
- **50 分钟**：建立 CUDA test target/helper，把一个 Week 10 known case 送到 GPU，
  与 CPU output 比较。
- **10–30 分钟**：故意制造 mismatch，确认报告包含 index/expected/actual/max error。
- **必须留下**：test target、helper、一个 passing 与一个验证过的 failing path。

### Day 3 — 共享 Case Matrix

- **10 分钟**：将 Week 10 cases 标记为 CPU-only 或 CPU+CUDA，目标全部可复用。
- **15 分钟**：确定 device allocation size 与 explicit padded stride。
- **55 分钟**：覆盖 odd、non-block-multiple、one-pixel、1/3/4 channel、padded
  source/destination；检查 padding sentinel。
- **10–30 分钟**：至少运行两种 block-edge 尺寸，保存 max error。
- **必须留下**：CPU/CUDA matrix 和每类代表 case 证据。

### Day 4 — Invalid/Async Error Path

- **10 分钟**：映射 CPU invalid exception 与 CUDA `cudaErrorInvalidValue`。
- **20 分钟**：区分 API validation、launch error、async execution error。
- **50 分钟**：增加 invalid metadata tests；确保调用者在正确 stream/event 边界
  检查 execution；不让 production API变成同步。
- **10–30 分钟**：运行独立错误 test，确认后续 test process 不被 sticky error 污染。
- **必须留下**：error table、tests 和 API lifetime说明。

### Day 5 — Randomized + Compute Sanitizer

- **10 分钟**：固定 seed 和运行 case 数，确保失败可在单 case 重现。
- **15 分钟**：选择 sanitizer tools 和 timeout，避免 profile 整个大型 suite。
- **55 分钟**：运行 randomized CPU/GPU comparison；对小而覆盖性的 subset 运行
  Compute Sanitizer。
- **10–30 分钟**：保存 sanitizer summary、首个失败或 clean result。
- **必须留下**：seed、max error distribution、sanitizer command/version。

### Day 6 — Orin CUDA Gate

- **15 分钟**：在 clean build directory configure/build。
- **30 分钟**：运行完整 CPU/CUDA `ctest --output-on-failure`。
- **20 分钟**：运行 `resize_demo` 和一个非默认 test seed。
- **15–30 分钟**：记录 Orin environment、pass count、max error 和 unavailable
  coverage；weekly review。
- **必须留下**：clean build transcript、Gate evidence；无 Orin 时明确 blocked，
  不把静态审阅标记为 GPU pass。

## API 与 Lifetime

- device pointers 与 stream 由调用者拥有。
- enqueue 成功不表示执行成功或结果已可读。
- event/stream 完成前不得释放或复用 buffers。
- test helper 的同步只用于 correctness/error observation，不代表生产 API 同步。

## 交付

- CUDA correctness test target。
- CPU/CUDA max error report。
- sanitizer command/result。

## 完成标准

- [ ] CPU/CUDA 使用相同 case matrix。
- [ ] 非 `32×8` 整数倍输出安全。
- [ ] launch 和 async errors 均被检查。
- [ ] 无 GPU 环境时明确 skip，而不是假通过。
