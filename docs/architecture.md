# Architecture

引擎的目标形态与它必须遵守的契约。步骤怎么走见 [`../STEPS.md`](../STEPS.md)；
这里只写**结构**和**规则**，不写顺序。

## 最终形态

```text
ONNX 文件
  → 解析（你写的 protobuf reader）
  → Graph IR：节点、初始值、拓扑序
  → 内存规划：中间张量的复用与生命周期
  → 执行
      ├─ CPU backend（你写的算子，也是 CUDA 的 oracle）
      └─ CUDA backend（device-resident，无隐藏 host 往返）
  → 输出
       │
       └→ 对拍：numpy / onnxruntime / ORT CUDA / TensorRT
       └→ 测量：latency 分位数 + 环境溯源 + power/thermal
```

ORT 和 TensorRT 在右下角，**是参照物不是依赖**。引擎不调用它们；
它只需要在同样的输入上给出同样的结果，并解释性能差距。

## 分层

```text
CLI / 驱动      tt inspect / run / bench / dump
图              Graph IR、拓扑序、内存规划
算子            elementwise / matmul / conv / pool / softmax
后端            CPU 参考实现；CUDA kernel
Tensor          storage、shape、dtype、stride、offset
平台            主机 CPU；NVIDIA GPU；Jetson Orin 的功耗/时钟/散热约束
```

下层不知道上层存在。Tensor 不知道有图，算子不知道有 ONNX。
每加一层都问一次：这层能不能单独测。

## Ownership

- Tensor 是**非拥有视图 + 引用计数存储**：多个 Tensor 可指向同一块内存，
  最后一个引用释放它。
- CPU 算子同步返回。
- CUDA 算子接收 stream，**不同步它**；调用方拥有 stream 和 device buffer。
- async enqueue 成功**不表示** work 已完成。
- event/stream 完成之前不得释放或复用相关 buffer。
- 任何第三方 runtime 的隐式同步都不得靠猜测——查文档或实测。

## Correctness

- CPU 参考实现应简单、可读，且**独立于**优化版本与 CUDA 版本。
  它慢没关系，它的职责是「显然正确」。
- 手算固定用例锁定 coordinate、layout 和算子语义。
- fixed-seed 随机测试覆盖 odd、单像素、padded stride 和边界。
- precision、layout 或 backend 改变时，重跑全部正确性测试。
- tolerance 必须给出**数值来源**（累加次数 × eps、FMA 合并等），
  不能是拍脑袋的 `1e-5`。见 [`../steps/phase-01-ops/step-023.md`](../steps/phase-01-ops/step-023.md)。

## Measurement boundaries

报告时按适用范围分开，不要混成一个数：

- 模型解析 / engine build；
- allocation / registration；
- H2D / 输入准备；
- 单个算子 kernel；
- 完整推理；
- D2H / 后处理；
- first-frame、steady-state、end-to-end；
- throughput、memory 峰值、power/thermal。

CUDA event、host clock、Nsight Systems 和 Nsight Compute 回答的是**不同问题**。
优化不得改变 API 语义、workload 语义，或绕过正确性测试。

方法见 [`benchmark-methodology.md`](benchmark-methodology.md)。

## Evidence

每个值得保留的结论都要能追回到原始数据：

```text
commit + 环境 + workload
  → 原始 samples / trace
  → 解释
  → 有范围限定的结论
```

原始 CSV 进 `benchmark-results/`（**不被 gitignore，这是刻意的**）。
分析写在 `notes/experiments/`。截图和 trace 不是结论。

Phase B 的 MCP server 消费的就是这条链的产物——见 [`../ai-app/README.md`](../ai-app/README.md)。

## 已存在的参考实现

`engine/src/cpu/resize_cpu.cpp` 和 `engine/src/cuda/resize_cuda.cu` 是一个
**已经写完**的前处理算子，带 CPU/CUDA parity 测试。它演示了这份文档要求的
全部契约：非拥有视图、stream 契约、手算用例、padded stride 覆盖、tolerance 依据。

写自己的算子时可以拿它当形状参照。Phase 07 会让你重写它的 CUDA 侧再做对比。
