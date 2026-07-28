# AI Hardware Runtime + Agentic AI：48 周课程与累计项目

这是一个面向已有 C/C++、Linux Kernel、DSP、异构计算和性能调试经验，但
ML/AI 经历较少的工程师设计的长期学习与作品集项目。

推荐周期为 **48 个内容周**，每天 1.5–2 小时、每周 6 天，约 432–576 小时。按完整
一年执行时，在 Unit 2、4、6、8 后各放一个 buffer/remediation week；ML/Python 前置
不足时再加 1–2 周 Course 0。

这不是按日期翻页的任务清单，而是一门有明确 prerequisite、教材主线、problem set、
guided/independent lab、累计项目和 Mastery Gate 的课程。8 个 Unit 各 6 周，只有
通过前一单元的知识考试、陌生实操、项目里程碑和答辩，才解锁下一单元。

[`weeks/`](weeks/) 是唯一日常入口；每个文件直接包含 Runtime 主线、Agent/MCP/GH-600
副线、必修章节、术语、worked problem、Day 1–6、Exit Ticket 和必须产出，不需要在
年度表之间手工换算。旧版 24 个 Core Modules 保存在
[`core-modules/`](core-modules/) 作为补充参考，不再定义正式课程顺序。

项目包含三条互相连接的轨道：

```text
Track A — AI Hardware Runtime（主线）
  model/operator → CUDA → ORT/TensorRT → Jetson Orin

Track B — AI Application（每日副线）
  agents → tools → MCP server → eval/memory/multi-agent/guardrails → GH-600

Track C — Knowledge Feed
  official docs + blog/podcast/community → review → experiment/design decision
```

目标不是转型为模型训练或算法研究工程师，而是成为能连接模型语义、硬件执行、
runtime 和 AI application control plane 的系统工程师。

## 一年后的目标产出

- 能解释常见 inference operators、tensor layout、graph、precision 和量化基础。
- 能独立编写、测试和 profile CUDA 1D/2D kernels。
- 完成 CPU reference、CUDA naive 与 CUDA optimized resize/warp。
- 完成一个 device-resident 的 Orin preprocessing → TensorRT inference pipeline。
- 提交三份完整 case study：
  1. CUDA resize correctness 与优化；
  2. multi-stream / buffer pipeline；
  3. ONNX Runtime / TensorRT 端到端推理。
- 所有性能数字包含环境、输入、warm-up、迭代数、正确性和限制。
- 能用 10 分钟讲清项目设计、一次错误判断、profiler 证据与工程取舍。
- 完成一个有 schema、tests、eval、threat model 和 audit 的 MCP server。
- GH-600 六个 skills domains 均有实际 GitHub/MCP/Agent artifact。
- 建立可持续的 podcast/community/blog 筛选与复盘系统。

## 目标平台与双环境工作流

最终 CUDA、TensorRT、Nsight 和性能结论以 **Jetson Orin** 为准。本地开发机可
用于编辑、CPU reference、文档和静态检查，但不能代替 Orin 验证。

```text
Development host
  ├─ edit / CPU reference / tests / docs
  └─ deploy source or build inputs
                    ↓
Jetson Orin
  ├─ CUDA correctness / Compute Sanitizer
  ├─ Nsight Systems / Nsight Compute
  ├─ ONNX Runtime / TensorRT
  └─ latency / throughput / power / thermal evidence
```

开始前应锁定并记录实际 Orin 型号、JetPack/L4T、CUDA、TensorRT、cuDNN、
compiler、power mode 和 clocks。不要混用桌面 GPU 或其他 JetPack 版本的结论。

## 立即开始

1. 先做 [`START_HERE.md`](START_HERE.md) 的 90 分钟入学诊断。它不假设 ML 经历，
   并会决定是否先做 1–2 周 Course 0。
2. 阅读 [`CURRICULUM.md`](CURRICULUM.md) 和
   [`docs/mastery-gates.md`](docs/mastery-gates.md)，理解先修图、评分、hard blockers
   和重修规则。
3. 按 [`docs/getting-started.md`](docs/getting-started.md) 建立 host/Orin inventory；
   实际 JetPack/CUDA/TensorRT 版本优先于计划示例。
4. 打开 [`weeks/README.md`](weeks/README.md)，只进入当前周。第一次从
   [`weeks/week-01.md`](weeks/week-01.md) 开始。
5. 开始一个 Unit 前可读 [`course/README.md`](course/README.md) 的 syllabus；每天不需要
   再去 `YEAR_PLAN.md`、`LEARNING_PLAN.md` 或 `AI_APP_TRACK.md` 查映射。
6. 使用 [`notes/templates/lecture-note.md`](notes/templates/lecture-note.md)、
   [`problem-set.md`](notes/templates/problem-set.md) 和
   [`gate-report.md`](notes/templates/gate-report.md) 留下可检查证据。

课程文档改动后可运行完整性检查：

```bash
python3 scripts/validate_curriculum.py
```

它会检查 48 个周入口、288 个 Day、每日时长、Gate rubric、每周 Feed Card/统一产物
路径、过期关键链接和仓库内 Markdown 链接。

CPU-only 构建：

```bash
cmake -S . -B build-cpu -DCMAKE_BUILD_TYPE=Release -DENABLE_CUDA=OFF
cmake --build build-cpu --parallel
ctest --test-dir build-cpu --output-on-failure
./build-cpu/resize_benchmark
```

Orin/CUDA 构建：

```bash
cmake -S . -B build-cuda -DCMAKE_BUILD_TYPE=Release -DENABLE_CUDA=ON
cmake --build build-cuda --parallel
ctest --test-dir build-cuda --output-on-failure
./build-cuda/resize_demo
```

也可以使用：

```bash
./scripts/build_and_test.sh cpu
./scripts/build_and_test.sh cuda
./scripts/run_benchmarks.sh
./scripts/profile_resize.sh
```

## 8 个 Unit、8 个硬性 Gate

| Unit | Weeks | 主题 | 累计交付 |
|---|---:|---|---|
| U1 | 1–6 | Tensor 与 inference math | CPU reference vertical slice |
| U2 | 7–12 | Model lifecycle、ONNX、ORT CPU、measurement | frozen model/runtime-input + CPU pipeline |
| U3 | 13–18 | CUDA execution、memory、sync、errors | sanitizer-clean CUDA transforms |
| U4 | 19–24 | Profiling 与 CUDA preprocessing | optimized preprocessor case study |
| U5 | 25–30 | Streams、ownership、multi-frame | async pipeline |
| U6 | 31–36 | ORT CUDA、TensorRT、device integration | device-resident RuntimeBackend |
| U7 | 37–42 | Orin power/thermal/reliability | sustained Orin beta + evidence service |
| U8 | 43–48 | Capstone、CI、portfolio、GH-600 | production release + 3 reports |

Week 6、12、18、24、30、36、42、48 进行 Major Gate。总分至少 80，且 correctness、
security、reproducibility hard blockers 全部通过，才继续。

## 学习边界

主线不要求成为 ML 算法工程师。Core Gate 通过前暂不深入：

- 大规模模型训练、数据工程和超参数调优；
- 完整数学推导和复杂反向传播；
- 分布式训练、vLLM/NCCL serving；
- 没有 profiler 证据的微优化；
- affine/perspective warp、INT8、DLA、plugin、DeepStream 等 electives；
- 为覆盖功能列表而同时深入多个 image operators。

需要掌握的是“足以理解和优化推理执行”的 ML：tensor、operator、graph、
layout、precision、quantization、dynamic shape、fallback 和 correctness。

## 仓库结构

```text
.
├── AGENTS.md                # Agent 规则唯一事实源；CLAUDE.md 与 copilot 指向它
├── .github/                 # reviewer、PR 模板、CPU CI 与 curriculum 校验
├── ai-app/                 # 后续 Runtime Lab Assistant MCP server
├── apps/                    # 示例与后续 capstone app
├── artifacts/               # 环境 freeze、contract、model 与 Gate 证据
├── benchmarks/              # 可重复 benchmark
├── benchmark-results/       # 原始数据，不把 trace 当结论
├── course/                  # 8 个 Unit syllabus 与累计项目 contract
├── core-modules/            # 旧版 24 个 Runtime 深挖参考
├── docs/                    # 架构、方法、设计与资源
├── include/cuda_ai/         # 公共 C++/CUDA API
├── labs/                    # 每周独立实现；resize_starter 是 G4 前隔离区
├── notes/                   # 每日、内容、实验、profiling、周复盘
├── scripts/                 # host/Orin build、test、profile 入口
├── src/cpu/                 # CPU correctness oracle
├── src/cuda/                # CUDA implementations
├── tests/                   # CPU/CUDA correctness
└── weeks/                   # 48 个自包含 course weeks（唯一日常入口）
```

## 项目纪律

- CPU reference 或其他可信 oracle 先于 CUDA optimization。
- API 先定义 coordinate、border、layout、dtype、stride 和 ownership。
- CUDA API、launch 和异步 execution error 必须可观察。
- 性能报告区分 allocation、H2D、kernel、D2H、runtime build 和 steady state。
- 报告 median/p95 和原始数据，不只选择最好的一次。
- Orin 数据记录 power mode、clocks、温度和是否发生 thermal throttling。
- 优化失败或 hypothesis 被推翻也是有效产出，不修改 workload 制造加速比。
- Agent/MCP 工具默认 read-only、least privilege；高风险 action 需要显式审批。
- Podcast/blog 结论不能替代官方文档、代码 correctness 或 profiler evidence。
