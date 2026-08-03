# 从零写一个推理引擎

你要做的事：用 C++ 从空文件开始，写出一个能加载 ONNX 模型、正确执行、然后在
GPU 和 Jetson Orin 上跑得快的推理引擎。

不是集成 ONNX Runtime，是**写一个它那样的东西**。写完之后，ORT 和 TensorRT
不再是你要学着调用的库，而是你用来对拍和比较的参照物。

## 怎么用这份东西

按编号往下做。每一步都以一条**能跑的命令**和**你应该看到的输出**结束——
对不上就是有 bug，先解决再往下走。

引擎叫 **`tt`**（tiny tensor）：命令行程序名、C++ 命名空间、`.tt` 文件后缀都用它。

没有考试，没有分钟预算，没有周次。做完了就是做完了。

带 🎯 的步骤末尾有一个 **additional goal**：一个可选的延伸实验，形式固定为
假设 → 测什么 → 你可能会发现 → 为什么。跳过它不会让你在后面任何一步卡住。

需要理论时步骤里会直接给链接，指向 [`reference/`](reference/)。那里是按需查阅的
资料库，不定义顺序，不要求通读：

- 卡在某个概念上 → [`reference/README.md`](reference/README.md) 的概念索引
- 想知道某个 Phase 该读什么 → [`reference/resources.md`](reference/resources.md)
- 想持续跟踪这个领域 → [`docs/feed.md`](docs/feed.md)（建议 Phase 04 之后再开始）

## 你需要什么

Phase 00–06 只需要一台能编译 C++17 的机器。**不需要 GPU，不需要 Orin。**
Phase 07 开始需要 NVIDIA GPU，Phase 10 需要 Jetson Orin。

```bash
./scripts/build_and_test.sh cpu     # 现在就该能跑通
```

各 Phase 需要什么环境、什么时候才需要 GPU 和 Orin，见
[`docs/getting-started.md`](docs/getting-started.md)。

---

## Phase 00 — Tensor

> 结束时：你的程序能打印一个张量的 shape、strides、dtype 和实际地址，
> 并且断言每一个 offset 推导都对。

| Step | 做什么 |
|---|---|
| [001](steps/phase-00-tensor/step-001.md) | 一个能编译的空引擎与 `tt` 命令行 |
| [002](steps/phase-00-tensor/step-002.md) | `Shape`：rank、维度、元素总数 |
| [003](steps/phase-00-tensor/step-003.md) | `DType`：字节宽度与类型标签 |
| [004](steps/phase-00-tensor/step-004.md) | `Storage`：一块带引用计数的裸内存 |
| [005](steps/phase-00-tensor/step-005.md) | `Tensor` = storage + shape + dtype + offset |
| [006](steps/phase-00-tensor/step-006.md) | row-major strides 与 flat offset 公式 |
| [007](steps/phase-00-tensor/step-007.md) | `tt dump`：打印元数据和真实地址 |
| [008](steps/phase-00-tensor/step-008.md) | 多维索引 `at(i,j,k)` 与越界检查 |
| [009](steps/phase-00-tensor/step-009.md) | 🎯 `reshape`：什么时候能不复制 |
| [010](steps/phase-00-tensor/step-010.md) | `slice`：共享 storage 的 view |
| [011](steps/phase-00-tensor/step-011.md) | `permute` / `transpose`：只改 strides |
| [012](steps/phase-00-tensor/step-012.md) | `is_contiguous` 与 `contiguous()` |
| [013](steps/phase-00-tensor/step-013.md) | 从文件读写张量（你自己的 `.tt` 格式） |
| [014](steps/phase-00-tensor/step-014.md) | 🎯 测试骨架：手算 fixture 与容差 |

## Phase 01 — 第一批算子

> 结束时：matmul、relu、add 的输出与你手算的 fixture 完全一致，
> 并且你能说出为什么某个循环顺序比另一个快。

| Step | 做什么 |
|---|---|
| [015](steps/phase-01-ops/step-015.md) | 逐元素算子框架与 `relu` |
| [016](steps/phase-01-ops/step-016.md) | `add` 与形状检查 |
| [017](steps/phase-01-ops/step-017.md) | 广播规则：先手算，再实现 |
| [018](steps/phase-01-ops/step-018.md) | 🎯 广播的两种实现：展开 vs 索引映射 |
| [019](steps/phase-01-ops/step-019.md) | `matmul`：最朴素的三重循环 |
| [020](steps/phase-01-ops/step-020.md) | 手算 fixture 锁死 matmul 语义 |
| [021](steps/phase-01-ops/step-021.md) | 🎯 循环顺序 ijk vs ikj：cache 说了算 |
| [022](steps/phase-01-ops/step-022.md) | 浮点不结合：为什么 sum 顺序会改变结果 |
| [023](steps/phase-01-ops/step-023.md) | 🎯 容差怎么定：把它放宽会掩盖什么 |
| [024](steps/phase-01-ops/step-024.md) | `numpy` 对拍脚本：第一个外部 oracle |
| [025](steps/phase-01-ops/step-025.md) | 🎯 `softmax`：数值稳定性不是可选项 |
| [026](steps/phase-01-ops/step-026.md) | 把算子接进统一的注册表 |
| [027](steps/phase-01-ops/step-027.md) | `bench` harness：median/p95 与溯源 |

## Phase 02 — 手搭的图

> 结束时：一个你手工连出来的两层 MLP 能完成前向计算。

| Step | 做什么 |
|---|---|
| 028–036 | `Node` / `Graph` 数据结构、拓扑排序、执行器、中间张量生命周期、<br>手工构造 MLP、逐层 dump、第一个"图跑通了"的时刻 |

## Phase 03 — 加载真实模型

> 结束时：`tt inspect mnist.onnx` 打印出真实模型的完整图结构。

| Step | 做什么 |
|---|---|
| 037–048 | 最小 protobuf wire-format 读取器（varint、长度前缀、字段跳过）、<br>ONNX 的 `ModelProto`/`GraphProto`/`NodeProto` 字段映射、initializer、<br>opset、shape inference 基础、`tt inspect` |

> **Scope guard**：手写约 200 行 wire-format 读取器就够了（和手写 FAT16 同性质），
> **不要**去实现完整的 protobuf 编解码器。

## Phase 04 — ⭐ 跑通真网络

> 结束时：**你的引擎正确识别出一张真实的手写数字。**
> 到这一步为止，你一行 CUDA 都没写，也没碰过 Orin。

| Step | 做什么 |
|---|---|
| 049–060 | MNIST 需要的算子：`Conv`、`MaxPool`、`Gemm`、`Relu`、`Reshape`；<br>im2col 的第一版；权重绑定；端到端推理；输出 argmax |

## Phase 05 — 对拍 oracle

> 结束时：你的每一个中间张量都与 onnxruntime 在容差内一致。

| Step | 做什么 |
|---|---|
| 061–070 | onnxruntime 参照脚本、逐层中间张量导出与 diff、<br>第一个真 bug 的定位过程、回归测试固化 |

## Phase 06 — CPU 上做快

> 结束时：比 Phase 04 的版本快一个数量级，每一步加速都有数据支撑。

| Step | 做什么 |
|---|---|
| 071–086 | 🎯 密集：先测再改、分块 matmul、im2col 的内存代价、<br>直接卷积 vs im2col、layout 选择、多线程、SIMD、<br>换上真实的 ImageNet 模型 |

## Phase 07 — 上 GPU

> 结束时：CUDA 版本与你自己的 CPU 实现结果一致。**这里开始需要 NVIDIA GPU。**

| Step | 做什么 |
|---|---|
| 087–102 | 🎯 第一个 kernel、launch 配置、错误检查、event 计时、<br>coalescing、shared memory、Compute Sanitizer、<br>逐算子搬到 GPU 并与 CPU oracle 对拍 |

## Phase 08 — 真正的 runtime 关注点

> 结束时：device-resident 执行链，没有隐藏的 host 往返。

| Step | 做什么 |
|---|---|
| 103–118 | 🎯 streams 与异步、内存 arena/复用规划、算子融合、<br>buffer 生命周期与 ownership、多帧流水线 |

## Phase 09 — 与专业选手比

> 结束时：你能指着数据说清楚自己在哪里输、输多少、为什么。

| Step | 做什么 |
|---|---|
| 119–130 | 🎯 ORT CUDA 与 TensorRT 的公平对比方法、<br>partition/fallback 观察、FP16、engine build 与 steady state 分离、<br>差距归因 |

## Phase 10 — Orin

> 结束时：在 Jetson Orin 上持续稳定运行，有 power/thermal 证据。

| Step | 做什么 |
|---|---|
| 131–142 | 部署、power mode 与 clocks、`tegrastats`、<br>sustained vs 冷启动、thermal throttling、失效注入与回滚 |

## Phase B — Agent / MCP 第二序列

> 前置：Phase 09 结束（那时才有真实的 benchmark 产物值得读）。

| Step | 做什么 |
|---|---|
| B01–B20 | 只读 MCP server：schema、resources、tools、<br>`list/validate/compare runs`、threat model、eval、audit |

主题索引见 [`reference/AI_APP_TRACK.md`](reference/AI_APP_TRACK.md)。

---

## 里程碑

不是考试，是"它现在能做到 X 了"：

| | 里程碑 |
|---|---|
| Phase 02 结束 | 一个图跑通了前向 |
| **Phase 04 结束** | **正确识别一张真实手写数字** ⭐ |
| Phase 05 结束 | 逐层与 onnxruntime 一致 |
| Phase 06 结束 | 比第一版快一个数量级，有数据 |
| Phase 07 结束 | GPU 上结果一致 |
| Phase 09 结束 | 与 TensorRT 同台对比并解释差距 |
| Phase 10 结束 | Orin 上持续运行，有 power/thermal 证据 |
