# 从零写一个推理引擎

用 C++ 从空文件开始，写出一个能加载 ONNX 模型、正确执行、然后在 GPU 和
Jetson Orin 上跑得快的推理引擎。

**不是集成 ONNX Runtime，是写一个它那样的东西。** 写完之后，ORT 和 TensorRT
不再是你要学着调用的库，而是你用来对拍和比较的参照物。

面向已有 C/C++、Linux、DSP、异构计算和性能调试经验，但 ML/AI 经历较少的工程师。

## 现在就开始

```bash
./scripts/build_and_test.sh cpu
```

跑通了就打开 [`STEPS.md`](STEPS.md)，从 Step 001 开始往下做；完成的步骤可以在
[`PROGRESS.md`](PROGRESS.md) 中勾选。

Phase 00–06 在一台普通开发机上就能做完：C++17 编译器与 CMake，
外加 Python（Step 024 起用 numpy 对拍，Phase 05 起用 onnxruntime）。
**不需要 GPU，不需要 Orin。**
各 Phase 分别需要什么环境见 [`docs/getting-started.md`](docs/getting-started.md)。

## 怎么运作

每一步都以一条**能跑的命令**和**你应该看到的输出**结束。对不上就是有 bug，
先解决再往下走。

没有考试，没有分钟预算，没有周次。做完了就是做完了。工件本身就是证据：
如果你的引擎能正确跑通 MNIST，就说明你理解了 tensor layout、图执行和算子语义，
没有什么可再考的。

带 🎯 的步骤末尾有一个 **additional goal**——一个可选的延伸实验，形式固定为
假设 → 测什么 → 你可能会发现 → 为什么。跳过它不会让你在后面任何一步卡住。

## 路线

| Phase | 结束时它能做什么 |
|---|---|
| 00–01 | Tensor、strides、view；relu/add/matmul/softmax 对上手算 fixture |
| 02–03 | 图执行器；解析真实 ONNX 文件 |
| **04** | **正确识别一张真实手写数字** ⭐ |
| 05 | 逐层中间张量与 onnxruntime 一致 |
| 06 | CPU 上快一个数量级，每步加速都有数据 |
| 07 | CUDA 版本与自己的 CPU oracle 一致 |
| 08 | streams、内存 arena、算子融合，无隐藏 host 往返 |
| 09 | 与 ORT CUDA / TensorRT 同台对比并解释差距 |
| 10 | Orin 上持续运行，有 power/thermal 证据 |
| B | 读取真实 benchmark 产物的只读 MCP server |

完整步骤见 [`STEPS.md`](STEPS.md)。

Phase B 同时覆盖 **GH-600** 认证的六个 domain——六域 artifact 是它的副产物。
自查清单见 [`docs/gh600-checklist.md`](docs/gh600-checklist.md)。
考证与引擎能力是两个独立结论，互不解锁。

## 仓库结构

```text
.
├── STEPS.md                 # 唯一入口：所有 phase 与 step
├── PROGRESS.md              # 已有 step 的 checkbox 进度表
├── steps/                   # 步骤文件
├── engine/                  # 你写的引擎：唯一的、持续演进的工件
│   ├── include/ src/ tests/ apps/ bench/
├── oracle/                  # 对拍脚本：numpy → onnxruntime → ORT CUDA → TRT
├── models/                  # ONNX 模型与固定输入
├── benchmark-results/       # 原始 CSV，不把 trace 当结论
├── reference/               # 按需理论库，不定义顺序
├── docs/                    # 架构、测量方法、资源索引
├── notes/                   # 实验记录
└── ai-app/                  # Phase B 的 MCP server
```

## 缺理论的时候去哪

| 你的问题 | 去哪 |
|---|---|
| 「stride 到底是什么」 | [`reference/README.md`](reference/README.md) — 概念索引，直接指到对应模块 |
| 「Phase 07 之前该读什么」 | [`reference/resources.md`](reference/resources.md) — 按 Phase 组织的外部资料 |
| 「我想一直跟踪这个领域」 | [`docs/feed.md`](docs/feed.md) — 渠道与纪律 |

[`reference/`](reference/) 是 48 周课程版本的全部内容（8 个 Unit syllabus、
24 个 Core Module、48 个周文件），降级为**按需查阅**，不定义顺序、不要求通读。

## 工程纪律

这些不是课程要求，是这个项目里每一个性能结论必须满足的条件：

- 正确性先于优化。CPU 参考实现或其他可信 oracle 先于任何 CUDA 优化。
- API 先定义 coordinate、border、layout、dtype、stride 和 ownership。
- CUDA API、launch 和异步执行错误必须可观察。
- 性能报告区分 allocation、H2D、kernel、D2H、engine build 和 steady state。
- 报告 median/p95 和原始数据，不只选择最好的一次。
- Orin 数据记录 power mode、clocks、温度和是否发生 thermal throttling。
- 优化失败或假设被推翻也是有效产出，不修改 workload 制造加速比。

引擎必须遵守的接口契约（ownership、正确性、测量边界）见
[`docs/architecture.md`](docs/architecture.md)；测量方法见
[`docs/benchmark-methodology.md`](docs/benchmark-methodology.md)。

完整规则见 [`AGENTS.md`](AGENTS.md)，它同时是所有 coding agent 的指令来源。

## 检查

```bash
./scripts/build_and_test.sh cpu     # CPU 构建与测试
python3 scripts/validate_steps.py   # 步骤结构与链接
./scripts/check_format.sh           # clang-format
```

CUDA 构建（需要 nvcc）：

```bash
./scripts/build_and_test.sh cuda
```
