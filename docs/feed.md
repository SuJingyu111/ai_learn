# 持续跟踪：读什么、怎么读

核对日期：**2026-08-03**。这个领域变化快，下面的链接和活跃度请自行复核——
尤其是社区和 podcast，它们的兴衰比官方文档快得多。

## 原则

输入是为了**发现问题和形成判断**，不是追每日新闻。AI 领域的新闻量足以吞掉你全部
学习时间，而其中与「推理引擎怎么写得对、写得快」相关的比例很低。

三条硬规矩：

1. **主线优先。** 步骤没做完时，跟踪一律让路。读别人的引擎不能代替写自己的。
2. **博客不承担依据。** 涉及 API 保证、版本行为、安全或性能声明时，
   必须回到官方文档或自己的实测。别人的 benchmark 不是你的 benchmark。
3. **不转化就不算读过。** 见文末的四行格式。

节奏建议：每周 1 次、30–60 分钟。Tier 0 优先于其它所有层。

---

## Tier 0 — 你正在造的东西的活参照

**这是最高价值的一层，而且大多数人不做。** 你在写一个推理引擎；世界上有好几个
开源的、真实的、可读的推理引擎。它们不是新闻，是**答案**——当你在 Phase 06 纠结
im2col 还是直接卷积时，去看别人怎么选的。

| 项目 | 为什么对你有用 | 什么时候去读 |
|---|---|---|
| [ggml](https://github.com/ggml-org/ggml) / [llama.cpp](https://github.com/ggml-org/llama.cpp) | 纯 C，从零实现的张量库 + 后端抽象，可读性极好 | Phase 01 起，随时 |
| [tinygrad](https://github.com/tinygrad/tinygrad) | 刻意保持小体量，lazy 求值与 kernel 融合思路清晰 | Phase 02、Phase 08 |
| [TVM](https://tvm.apache.org/) | 图 IR、调度、代码生成的工业级参考 | Phase 06 起 |
| [IREE](https://iree.dev/) | MLIR 路线的编译式 runtime，与 TVM 对照着看 | Phase 08 起 |
| [ONNX Runtime](https://github.com/microsoft/onnxruntime) | 你的对拍对象。**读它的源码**，不只是调它的 API | Phase 05 起 |

读法：**不要通读。** 带一个具体问题去 `grep`。比如
「ORT 的 Conv 在 CPU 上到底走的哪条路径」比「我要读懂 ORT」有用一百倍。

## Tier 1 — 权威与版本相关

涉及 API、兼容性、性能保证时，只有这一层能当依据。

- [NVIDIA Technical Blog](https://developer.nvidia.com/blog/) — 尤其
  [Jetson 标签](https://developer.nvidia.com/blog/tag/jetson/)。只采用与你实际
  JetPack/CUDA 版本匹配的内容。
- **CUDA / TensorRT / ONNX Runtime 的 release notes。** 被严重低估的一类资料：
  它直接告诉你哪些行为变了、哪些 bug 修了、哪些是已知问题。runtime 工程师读
  release notes 的收益远高于读教程。
- [GTC on-demand](https://www.nvidia.com/en-us/on-demand/) — 免费。搜
  CUDA、TensorRT、Jetson、inference optimization，里面有大量深度技术 session。
- [PyTorch dev-discuss](https://dev-discuss.pytorch.org/) — PyTorch 的编译器和
  runtime 设计在这里公开讨论。看真实的设计权衡怎么发生。
- [ONNX](https://onnx.ai/) 算子规范 — Phase 03 起你会反复查。
- [NVIDIA Developer Forums](https://forums.developer.nvidia.com/) — Jetson 具体
  问题最快的去处。**论坛答案必须回版本文档验证。**

## Tier 2 — 深度实践者

数量少而精。这几个人写的东西与「让计算跑得快」直接相关：

- [Lei Mao](https://leimao.github.io/) — CUDA、TensorRT 的深度技术文章，
  与你这个项目的重合度可能是所有博客里最高的。
- [Horace He](https://horace.io/brrr_intro.html) — *Making Deep Learning Go Brrrr
  From First Principles*。讲清楚 memory-bound / compute-bound / overhead-bound
  三种状态怎么区分。**建议 Phase 06 开始前读一遍，Phase 06 结束后再读一遍。**
- [Chip Huyen](https://huyenchip.com/) — ML systems、生产化视角。
- [Simon Willison](https://simonwillison.net/) — LLM 工具、agents、eval、
  prompt injection。主要服务 Phase B。
- [Lilian Weng](https://lilianweng.github.io/) — agent 与研究综述，篇幅长，按需读。

## Tier 3 — 社区与讲座

- **[GPU MODE](https://github.com/gpu-mode)**（原 CUDA MODE）— 讲座录像 + Discord，
  一群人认真写 CUDA kernel 的地方。**对你这个方向，这可能是单个价值最高的社区。**
  讲座在 YouTube，主题覆盖 kernel 优化、量化、Triton、profiling。
- [MLSys](https://mlsys.org/) — ML 与 systems 交叉的顶会。论文和录像都公开。
  这是与你目标岗位最对口的学术场所。
- [Latent Space](https://www.latent.space/podcast) — 选 infra / systems / eval 的期，
  跳过纯产品和融资的。
- [MLOps Community](https://mlops.community/podcast) — 生产化 ML 基础设施。

## Tier 4 — 论文

不建议现在就追。等到 Phase 06 之后，你有了具体问题再去搜，效率高得多。
届时的入口是 MLSys 和 arXiv 的 `cs.DC` / `cs.LG`，配合一个关键词订阅
（FlashAttention、quantization、kernel fusion、inference serving 之类）。

在那之前，**读源码的回报高于读论文**。

---

## 怎么把输入变成产出

任何内容只有转化成下面四行，才算读过。记到 `notes/experiments/` 下：

```text
claim    — 它主张什么
source   — 出处 + 访问日期 + 版本（没有版本的性能声明基本无效）
scope    — 在什么硬件/规模/输入下成立
action   — 我要做的一个最小实验，或一条设计决定，或「仅观点，不采纳」
```

第四行是关键。**没有 action 的笔记等于没读。**

很多结论会和你自己的实测冲突。冲突时以你的实测为准，并把冲突本身记下来——
「某篇文章说 X 快 3 倍，我在 Orin 上只有 1.2 倍，因为 ___」
这种笔记的价值远高于摘抄。

## 一个反模式

不要在 Phase 04 之前花时间跟踪前沿。你现在最缺的不是信息量，是**手上的实现经验**。
在没有自己写过一个 Conv 之前读十篇 kernel 优化文章，吸收率接近零；
写过之后再读同样十篇，每一篇都能对上。

先做到「它能识别一张手写数字」，再开始订阅。
