# reference/ — 按需查阅的理论库

这里是 48 周课程版本的全部内容。**它不定义顺序，也不要求通读。**
做步骤时卡在某个概念上，从这里查；不卡就不用打开。

三个入口，按你此刻的问题选：

| 你的问题 | 去哪 |
|---|---|
| 「做到 Step 019 卡住了，该读什么」 | [`resources.md`](resources.md) — 按 Phase 组织的外部资料 |
| 「stride 到底是什么」 | 下面的**概念索引** |
| 「我想持续跟踪这个领域」 | [`../docs/feed.md`](../docs/feed.md) |

## 卡住时的顺序

先花 10 分钟自己定位，再去读——带着具体问题读的效率是漫无目的读的几倍：

1. 把输入缩到最小（1–32 个元素），打印 shape、strides、dtype、地址。
2. 问自己：是**语义**不懂（这个算子该算什么），还是**机制**不懂
   （字节现在在哪、谁拥有），还是**测量**不懂（这个数字凭什么可信）？
3. 按下面的表找到对应的 Core Module，只读相关那一节。
4. 需要权威出处（API 保证、版本行为、安全声明）时，直接查
   [`resources.md`](resources.md) 里的 Tier 1 官方文档，不要停在博客。

[`../docs/concept-map.md`](../docs/concept-map.md) 有一张概念依赖 DAG。
某个概念解释不清时，沿箭头**向上**找真正缺失的前置，不要搜当前这个高级术语的速成教程。

## 概念索引

| 概念 | 最先在哪一步需要 | 读这里 |
|---|---|---|
| shape / rank / dtype / element vs byte | Step 002–003 | [module-01](core-modules/module-01.md) |
| storage、stride、view vs copy、地址推导 | Step 004–012 | [module-01](core-modules/module-01.md) |
| layout（HWC/CHW）、contiguous、permute 代价 | Step 011–012 | [module-01](core-modules/module-01.md) |
| 广播规则 | Step 017 | [module-01](core-modules/module-01.md) |
| MatMul 的 shape/dataflow/FLOPs 直觉 | Step 019 | [module-02](core-modules/module-02.md) |
| 浮点误差、tolerance、绝对 vs 相对 | Step 022–023 | [module-02](core-modules/module-02.md) |
| operator vs kernel、参数 vs 激活 | Phase 02 | [module-02](core-modules/module-02.md) |
| 模型生命周期、training/inference 边界 | Phase 03 | [module-03](core-modules/module-03.md) |
| ONNX IR、opset、graph、shape inference | Phase 03 | [module-04](core-modules/module-04.md) |
| Conv 的输出尺寸、padding、stride、dilation | Phase 04 | [module-02](core-modules/module-02.md) |
| ORT provider、图优化、CPU baseline | Phase 05 | [module-04](core-modules/module-04.md) |
| benchmark 方法论、warm-up、median/p95 | Step 027 | [../docs/benchmark-methodology.md](../docs/benchmark-methodology.md) |
| Nsight Systems vs Nsight Compute 的分工 | Phase 06 | [module-09](core-modules/module-09.md) |
| CUDA execution model、grid/block/thread | Phase 07 | [module-05](core-modules/module-05.md) |
| CUDA 错误模型、异步错误、event 计时 | Phase 07 | [module-05](core-modules/module-05.md) |
| warp、divergence、SM、occupancy | Phase 07 | [module-06](core-modules/module-06.md) |
| global memory、coalescing、访问模式 | Phase 07 | [module-07](core-modules/module-07.md) |
| shared memory、reduction、Compute Sanitizer | Phase 07 | [module-08](core-modules/module-08.md) |
| 采样算子语义（坐标、border、rounding） | Phase 07 | [module-10](core-modules/module-10.md)、[module-11](core-modules/module-11.md) |
| 受控优化与 case study 写法 | Phase 06 / 09 | [module-12](core-modules/module-12.md) |
| streams、events、pinned memory | Phase 08 | [module-15](core-modules/module-15.md) |
| 多帧流水线、ownership、buffer lifetime | Phase 08 | [module-16](core-modules/module-16.md) |
| ORT CUDA EP、I/O Binding、fallback | Phase 09 | [module-17](core-modules/module-17.md) |
| TensorRT engine/context/enqueue/lifetime | Phase 09 | [module-18](core-modules/module-18.md) |
| precision、quantization、dynamic shape | Phase 09 | [module-19](core-modules/module-19.md) |
| Orin power/clock/thermal、sustained 测量 | Phase 10 | [module-22](core-modules/module-22.md) |
| reliability、失效注入、回滚 | Phase 10 | [module-23](core-modules/module-23.md) |
| Agent/MCP、typed tools、threat model | Phase B | [AI_APP_TRACK.md](AI_APP_TRACK.md) |

## 这里还有什么

- [`core-modules/`](core-modules/) — 24 个深挖模块，上表指向的就是它们。
- [`weeks/`](weeks/) — 48 周课程版本的每周文件。里面的 worked example、术语表和
  带问题阅读仍然有用；Day 安排和 Exit Ticket 已经不适用。
- [`course/`](course/) — 8 个 Unit 的 syllabus 与旧的累计项目 contract。
- [`CURRICULUM.md`](CURRICULUM.md)、[`LEARNING_PLAN.md`](LEARNING_PLAN.md)、
  [`mastery-gates.md`](mastery-gates.md) — 旧结构的总纲、边界与评分规则。
  评分机制已废弃，但 `mastery-gates.md` 里的 **hard blockers 清单**仍然是一份
  很好的自查表：正确性、证据链、可复现性。

## 一句话提醒

这些材料写于旧的课程结构，措辞里会出现 Unit、Week、Gate、Exit Ticket，
**文件路径也大多过时**（`include/cuda_ai/`、`labs/`、`notes/feed/`、`artifacts/`
都已经不存在或改名）。

忽略那些框架词和路径，只取其中的技术内容。当前的路径以
[`../STEPS.md`](../STEPS.md) 和 [`../README.md`](../README.md) 为准。
