# Core Module 05 — CUDA Execution、错误模型与可靠计时

## 本周目标

在 Orin 上建立 CUDA 编程和错误观察基线：host launch、grid/block/thread、
异步执行、Runtime API error 和 CUDA event timing。

## 每日计划

### Day 1 — Orin Baseline 与第一个 Kernel

- **10 分钟**：在 Orin 创建 daily log，写明设备是否为 AGX Orin/NX/Nano。
- **20 分钟**：运行 `docs/getting-started.md` 的环境命令，额外记录
  `sudo nvpmodel -q`、`jetson_clocks --show` 和空载 `tegrastats` 片段。
- **50 分钟**：创建 `labs/cuda/week05_vector_add.cu`，实现一线程一元素 vector add
  和 CPU comparison；先使用 `N=32`。
- **10–30 分钟**：用 `nvcc` 或 CMake 编译运行，记录 exact command、GPU arch 和
  首个错误。
- **必须留下**：environment snapshot、可运行 kernel、CPU/GPU max error。

### Day 2 — Grid-stride SAXPY 与边界尺寸

- **10 分钟**：手算普通 indexing 与 grid-stride loop 的覆盖范围。
- **20 分钟**：阅读 CUDA Programming Guide 中 thread hierarchy/grid-stride 所需段落。
- **50 分钟**：实现 SAXPY 和 CPU oracle；测试 `N=0,1,31,32,33,255,256,257,large`。
- **10–30 分钟**：对每个 N 填充 guard/canary 或检查完整 output，防止只验证首元素。
- **必须留下**：case matrix、launch config 和所有 case 的 correctness summary。

### Day 3 — 统一 Error-checking

- **10 分钟**：列出 allocation/copy/launch/execution/free 可能的错误观察点。
- **20 分钟**：阅读 CUDA Runtime error handling 和 `cudaGetLastError` 语义。
- **50 分钟**：为 lab 加入统一 check helper；检查 `cudaMalloc/cudaMemcpy/event`，
  launch 后立即检查 launch error。
- **10–30 分钟**：使用受控 invalid allocation/argument 测试 helper 输出文件和行号。
- **必须留下**：error helper、failure output 和为何不直接忽略 return code 的结论。

### Day 4 — Async Execution Error

- **10 分钟**：预测一个非法 device access 何时会被 host 看到。
- **20 分钟**：复习 launch 异步性；故障代码与正常 benchmark 隔离。
- **50 分钟**：创建独立 `week05_async_error.cu`，故意触发安全可控的 out-of-bounds；
  分别在 launch check、event/stream synchronize 观察错误。
- **10–30 分钟**：运行 Compute Sanitizer，并在故障后重启独立进程验证正常 lab。
- **必须留下**：错误首次出现位置、sanitizer command、launch vs execution 解释。

### Day 5 — Timing Boundary

- **10 分钟**：画 allocation→H2D→kernel→D2H→validation timeline。
- **20 分钟**：阅读 benchmark methodology 中 host clock/CUDA event 区别。
- **50 分钟**：为 SAXPY 分别测 allocation、H2D、kernel event、D2H 和 end-to-end；
  warm-up 后采集至少 30 samples。
- **10–30 分钟**：计算 median/p95，验证 event 与 kernel 位于同一 stream。
- **必须留下**：raw CSV、included/excluded stages 和 checksum/max error。

### Day 6 — Execution Model 周复盘

- **15 分钟**：闭卷画 host→runtime→stream→grid/block/thread。
- **40 分钟**：写 `notes/concepts/cuda-execution-error-model.md`。
- **20 分钟**：clean rebuild/run 正常 labs；确认故障样例不会被默认 test 执行。
- **15–30 分钟**：填写 weekly review，列出 Week 6 要控制的 block-size 变量。
- **必须留下**：latency breakdown、概念笔记和复现命令。

## 实验纪律

- 先验证 output，再计时。
- event 必须放在正确 stream。
- 不用全局 `cudaDeviceSynchronize()` 掩盖依赖设计。
- 故意错误实验必须独立、可恢复，不污染后续数据。

## 交付

- vector add/SAXPY 或等价最小实验。
- H2D/kernel/D2H/end-to-end latency 表。
- `notes/concepts/cuda-execution-error-model.md`

## 完成标准

- [ ] 任意 N 无越界。
- [ ] API、launch 和 async execution error 都能观察。
- [ ] 能解释 kernel launch 返回不代表 work 完成。
- [ ] timing 明确包含和排除的阶段。
