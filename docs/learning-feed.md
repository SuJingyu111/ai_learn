# Podcast、Community 与 Blog 学习系统

## 目标

内容输入用于发现问题和形成判断，不用于追逐每日新闻。每周最多：

- 一篇 primary/technical article；
- 一篇 practitioner analysis，或一集 podcast 的一半到一集；
- 一次 30–45 分钟复盘。

任何内容只有转化为 concept note、experiment hypothesis、design decision 或
backlog item，才算完成。

这是 **optional seminar**，不是课程教材。它不能定义 prerequisite、替代 Unit
syllabus 的必修章节，或成为 Major Gate 答案的唯一依据。Core theory/lab/Exit Ticket
未完成时，当周 seminar 自动取消。

每周细分文件末尾提供一个 30–45 分钟 Feed Card。它可在通勤/散步时完成；若只能严格
投入每周 9–12 小时，就用它替换一段非 Gate review，不额外叠加时间，也不补做被取消
的旧 Feed Card。所有复盘统一保存到 `notes/feed/week-NN.md`；首次可运行
`mkdir -p notes/feed`。

## Source Ladder

### Tier 1 — Primary Sources

遇到 API、兼容性、安全或性能声明，优先：

- NVIDIA CUDA / Jetson / TensorRT / Nsight 官方文档和 Technical Blog；
- PyTorch、ONNX、ONNX Runtime、Hugging Face 官方文档；
- GitHub Docs、Microsoft Learn modules；GH-600 Study Guide 仅用于 exam-domain mapping；
- Model Context Protocol specification；
- MLSys conference papers/talks。

### Tier 2 — 高信号工程实践

- [Chip Huyen](https://huyenchip.com/)：ML systems、production AI、AI engineering。
- [Lilian Weng / Lil'Log](https://lilianweng.github.io/)：agent、tool use 和研究综述；
  篇幅较长，按问题选读。
- [Simon Willison](https://simonwillison.net/)：LLM tools、coding agents、eval、
  prompt injection 和快速变化的产品生态。
- [NVIDIA Technical Blog — Jetson](https://developer.nvidia.com/blog/tag/jetson/)：
  只采用与实际 Orin/JetPack 匹配的内容。
- [Hugging Face optimization docs](https://huggingface.co/docs/transformers/optimization_overview)：
  用于后期 transformer inference 扩展，不提前抢占主线。

### Tier 3 — Podcast / Talks

- [Latent Space](https://www.latent.space/podcast)：AI Engineer、agents、infra；
  选择 systems/evals/tooling episodes。
- [MLOps Community Podcast](https://mlops.community/podcast)：production ML/AI
  infrastructure 和组织实践。
- [The Gradient](https://thegradient.pub/)：研究与工程背景，适合补概念但不追全量。
- [AI Engineer talks](https://www.ai.engineer/)：MCP、agents、evals、production apps。
- [MLSys](https://mlsys.org/)：ML 与 systems 交叉的深度内容。

### Community

- NVIDIA Developer Forums：Jetson/JetPack 具体问题；论坛答案必须回到版本文档验证。
- GitHub Community：Copilot、Actions、MCP/GitHub workflow 使用问题。
- MLOps Community：production AI 的经验交流。
- MLSys / AI Engineer talks 与公开讨论：用于寻找论文、工具和反例。

Reddit/Hacker News/X 可用作“发现线索”，不能作为版本/API/benchmark 的最终证据。

每 4 周可选做一次 community practicum：把一个已最小化、已脱敏且含环境/复现步骤的
问题发到 NVIDIA Developer Forums、GitHub Community 或 MLOps Community，或对已有
讨论补充可复现实验。回复仍需回到官方版本文档与本地验证，不能把点赞数当证据。

## 每周内容轮换

| 周内时段 | 内容 |
|---|---|
| Day 3 或 5 | 从当周 Feed Card 选 20–30 分钟 primary/practitioner source |
| Day 6 | 用剩余 10–15 分钟写复盘；Core 落后则整张卡取消 |

每四周轮换：

1. CUDA/Orin/runtime；
2. ML inference/operator；
3. Agent/MCP/GH-600；
4. MLSys/AI engineering broad view。

## 选择一篇内容前

必须回答：

- 它与本周哪个问题相关？
- 作者是 primary source、实践者还是评论者？
- 是否依赖特定版本/硬件/业务条件？
- 读完后准备修改什么 experiment 或 design？

回答不了就放入候选列表，不立即阅读。

## 复盘标准

使用 [`notes/templates/content-review.md`](../notes/templates/content-review.md)。
复盘不写长摘要，只保留：

1. 作者的核心 claim。
2. 支持 claim 的 evidence。
3. 假设和适用条件。
4. 与当前项目一致/冲突之处。
5. 一个 30–90 分钟可验证动作。
6. 何时重新核对或丢弃这条结论。
