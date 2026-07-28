# Core Module 21 — Orin Capstone Baseline

## 本周目标

冻结一个小而可解释的模型和真实 pipeline workload，建立未经微优化但完整、正确、
可重复的 Orin baseline。

## Pipeline

```text
input frame
  -> CUDA resize / normalize / layout
  -> device tensor
  -> TensorRT inference
  -> minimal postprocess
  -> validation / result
```

可以使用 synthetic frames 或固定 image set；camera integration 是 optional，不能
让驱动/媒体问题阻塞 runtime 主线。

## 每日计划

### Day 1 — 冻结 Capstone Scope

- **10 分钟**：写一句项目问题和目标用户/场景。
- **20 分钟**：冻结 model/version、三类 inputs、fixed/dynamic shapes、FP32/FP16、
  preprocessing/postprocess 和 correctness oracle。
- **50 分钟**：创建 `docs/capstone-architecture.md` 的 scope/contract；列 core 与明确
  non-goals（camera、DLA、plugin 等）。
- **10–30 分钟**：检查每个 feature 是否能在剩余两周产生证据，删去无法完成的扩张。
- **必须留下**：scope freeze、model/input manifest 和 acceptance criteria。

### Day 2 — Clean Build/Deploy/Run

- **10 分钟**：从 README 以外不读取个人 shell history，模拟新环境。
- **20 分钟**：列 host→Orin source identity、configure/build/model generation/deploy。
- **50 分钟**：在 clean build directory 运行完整流程，补 scripts/docs 中隐藏步骤。
- **10–30 分钟**：保存命令 transcript；确认不存在硬编码个人路径/未记录环境变量。
- **必须留下**：可重复 build/run path 和失败时的第一条 blocker。

### Day 3 — End-to-end Correctness Set

- **10 分钟**：定义 normal、boundary、stress/odd-shape 三类 inputs。
- **20 分钟**：保存 framework/CPU expected outputs 或 task-level reference。
- **50 分钟**：运行 full pipeline，比较 intermediate preprocessing 和 final output；
  检查 frame/order/shape metadata。
- **10–30 分钟**：重复运行，判断 deterministic 或合理 bounded variation。
- **必须留下**：三类 input results、max/task error 和 reproducible artifacts。

### Day 4 — Baseline Metrics

- **10 分钟**：冻结 warm-up、samples、sustained duration 和 power state。
- **50 分钟**：采集 first-frame、preprocess、inference、postprocess、end-to-end、
  throughput、memory。
- **20 分钟**：同时保存 power mode、clocks、start/end temperature、`tegrastats`
  代表片段。
- **10–30 分钟**：检查 raw samples 和 units，不先挑最好的数字。
- **必须留下**：baseline CSV/JSON 和 environment manifest。

### Day 5 — Baseline Timeline

- **10 分钟**：写 timeline 要回答的 5 个问题：copy、sync、gap、CPU feed、overlap。
- **15 分钟**：限制 NVTX capture 到稳定的少量 frames。
- **55 分钟**：获取 Nsight Systems trace，标出所有 CPU/GPU/runtime stages。
- **10–30 分钟**：按 end-to-end 时间贡献排序 bottleneck candidates。
- **必须留下**：exact command、annotated evidence 和 candidate ranking。

### Day 6 — Baseline Review

- **15 分钟**：clean rerun 一个代表 case，确认 baseline 不依赖 profiler。
- **45 分钟**：完成 capstone architecture、ownership、baseline results 和 limitations。
- **20 分钟**：选择 Week 22 唯一 primary optimization hypothesis 和 fallback task。
- **10–30 分钟**：weekly review；冻结 baseline，不在 Week 22 偷换 model/workload。
- **必须留下**：capstone baseline v1 和 optimization decision。

## 交付

- 一键或少步骤 reproduction。
- environment + workload manifest。
- baseline raw CSV/JSON 与 timeline。
- `docs/capstone-architecture.md`

## 完成标准

- [ ] clean run 不依赖交互式隐藏步骤。
- [ ] 每个 buffer/device transition 可解释。
- [ ] correctness 先于 timing。
- [ ] baseline 不包含未经标注的 engine build。
