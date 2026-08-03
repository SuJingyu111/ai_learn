# Course Pack：按先修顺序学习的资料主线

核对日期：**2026-07-26**。本文件不是“有空看看”的链接收藏，而是课程规定的学习
顺序。每周文件会直接列出当周页面、章节和分钟；这里解释每个资源承担什么教学职责。

## 四种资料角色

| 标签 | 用法 | 能否承担先修 |
|---|---|---|
| **Textbook / Course** | 连续学习概念、做例题和知识检查 | 可以 |
| **Official Reference** | 核对 API、协议、版本和保证 | 不能单独承担 |
| **Guided Lab** | 在提示下验证一个理论命题 | 必须配合理论 |
| **Seminar** | podcast/blog/community 的观点、趋势和案例 | 不可以 |

每次必修阅读必须留下：

1. 5–10 个术语的**自己的定义**；
2. 一个公式、数据流或状态图；
3. 一个容易混淆的邻近概念和反例；
4. 一个能证实/推翻阅读结论的最小实验；
5. 精确 URL、章节、访问日期和版本。

只保存摘要或划线不算完成。

## Course 0 — Python、数学、GitHub

### Python / array / math

- **Textbook** — [NumPy: Absolute Basics for Beginners](https://numpy.org/doc/stable/user/absolute_beginners.html)：
  array creation、indexing、shape、axis、reshape、broadcast。
- **Textbook** — [D2L: Data Manipulation](https://d2l.ai/chapter_preliminaries/ndarray.html)：
  tensor/array operations。
- **Textbook** — [D2L: Linear Algebra](https://d2l.ai/chapter_preliminaries/linear-algebra.html)：
  scalars、vectors、matrices、tensors、reductions、dot products、matrix-vector products。
- **Optional math depth** — [Mathematics for Machine Learning](https://mml-book.github.io/)：
  Chapter 2.1–2.2；只补矩阵和 MatMul 所需部分。

### Git / GitHub / Actions

- **Course** — [Introduction to Git](https://learn.microsoft.com/en-us/training/modules/intro-to-git/)：
  version control、basic commands、branch exercise、knowledge check。
- **Course** — [Introduction to GitHub](https://learn.microsoft.com/en-us/training/modules/introduction-to-github/)：
  GitHub Flow、collaboration、repository、branch、commit、PR。
- **Course** — [Introduction to GitHub Actions](https://learn.microsoft.com/en-us/training/modules/introduction-to-github-actions/)：
  workflow、event、job、runner、action。
- **Reference** — [Workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax)：
  `on`、`permissions`、`jobs`、`needs`、`if`、`timeout-minutes`、artifacts。
- **Security reference** — [GITHUB_TOKEN](https://docs.github.com/en/actions/concepts/security/github_token)、
  [Secure use](https://docs.github.com/en/actions/reference/security/secure-use)、
  [Script injections](https://docs.github.com/en/actions/concepts/security/script-injections)。

## Unit 1 — Tensor、MatMul、Conv 与 Inference Boundary

### W1–W2：storage、stride、view/copy

- **Course** — [PyTorch Tensors tutorial](https://docs.pytorch.org/tutorials/beginner/basics/tensorqs_tutorial.html)：
  Initializing a Tensor、Attributes、Operations、Bridge with NumPy。
- **Reference** — [PyTorch Storage](https://docs.pytorch.org/docs/stable/storage)：
  storage、dtype、shape、stride、storage offset。
- **Reference** — [Tensor Views](https://docs.pytorch.org/docs/stable/tensor_view.html)。
- **Reference** — [`Tensor.view`](https://docs.pytorch.org/docs/stable/generated/torch.Tensor.view.html)：
  contiguity-like condition。
- **Reference** — [`Tensor.contiguous`](https://docs.pytorch.org/docs/stable/generated/torch.Tensor.contiguous.html)。
- **Reference** — [Tensor attributes](https://docs.pytorch.org/docs/stable/tensor_attributes.html)：
  memory format、channels-last。

学完必须能推导：

```text
element_offset = storage_offset + Σ(index[d] × stride[d])
byte_offset    = element_offset × itemsize
```

### W3–W5：MatMul、Conv、operator/dataflow、数值

- **Textbook** — D2L Linear Algebra 的 dot products、matrix-vector、matrix-matrix。
- **Course** — [CS231n: Convolutional Networks](https://cs231n.github.io/convolutional-networks/)：
  Convolutional Layer、Spatial Arrangement、Parameter Sharing、Pooling。
- **Textbook** — [Deep Learning Book, Chapter 9](https://www.deeplearningbook.org/contents/convnets.html)：
  §9.1–9.3 convolution、motivation、pooling；选读。
- **Course** — [PyTorch Build Model](https://docs.pytorch.org/tutorials/beginner/basics/buildmodel_tutorial.html)。
- **Course** — [D2L: Softmax Regression](https://d2l.ai/chapter_linear-classification/softmax-regression.html)：
  softmax operation、normalization、输出解释；训练推导留到 U2 bridge。
- **Reference** — [Torchvision Normalize](https://docs.pytorch.org/vision/stable/generated/torchvision.transforms.Normalize.html)：
  用 `output[c] = (input[c] - mean[c]) / std[c]` 冻结 M1 的 channel-wise transform；
  不把它与 BatchNorm 混为一谈。
- **Guided reference** — [PyTorch Profiler recipe](https://docs.pytorch.org/tutorials/recipes/recipes/profiler_recipe.html)：
  shape、CPU/CUDA time、memory；本单元只用于观察 operator dataflow。

## Unit 2 — Model Lifecycle、ONNX、ORT CPU

### W7–W8：Module/state/autograd

- **Course** — [PyTorch Learn the Basics](https://docs.pytorch.org/tutorials/beginner/basics/)：
  Build Model、Autograd、Optimization Loop。
- **Course** — [Save and Load the Model](https://docs.pytorch.org/tutorials/beginner/basics/saveloadrun_tutorial.html)。
- **Reference** — [Saving and Loading Models](https://docs.pytorch.org/tutorials/beginner/saving_loading_models)。
- **Reference** — [`Module.eval`](https://docs.pytorch.org/docs/stable/generated/torch.nn.Module.html)。
- **Reference** — [Autograd Grad Modes](https://docs.pytorch.org/docs/stable/notes/autograd.html#locally-disable-grad-doc)、
  [`inference_mode`](https://docs.pytorch.org/docs/stable/generated/torch.autograd.grad_mode.inference_mode.html)。

重点问题：`eval()` 为什么不等于禁止 autograd；runtime 学习者为什么仍需理解一次
training loop。

### W9：ONNX IR/export

- **Course** — [Export a PyTorch model to ONNX](https://docs.pytorch.org/tutorials/beginner/onnx/export_simple_model_to_onnx_tutorial.html)。
- **Reference** — [`torch.onnx`](https://docs.pytorch.org/docs/stable/onnx.html)：
  当前 exporter、`dynamo=True`、`dynamic_shapes`。
- **Course** — [ONNX Introduction](https://onnx.ai/onnx/intro/)：
  Input、Output、Node、Initializer、Attribute、opset、shape/type inference。
- **Guided lab** — [ONNX Python API Introduction](https://onnx.ai/onnx/intro/python.html)：
  checker、shape inference、graph inspection。

### W10–W12：ORT CPU/correctness/measurement

- **Course** — [ORT API Basics](https://onnxruntime.ai/docs/tutorials/api-basics.html)。
- **Reference** — [ORT Python API Summary](https://onnxruntime.ai/docs/api/python/api_summary.html)：
  load/run、metadata、SessionOptions、profiling。
- **Method** — [`benchmark-methodology.md`](benchmark-methodology.md)：
  environment、warm-up、raw samples、median/p95、correctness。

## Unit 3 — CUDA Foundations

### 连续教材

- **Textbook** — David B. Kirk & Wen-mei W. Hwu, *Programming Massively Parallel
  Processors*, 4th edition：Chapter 2–5。若暂时无法获得教材，按周文件指定的 CUDA
  Programming Guide 页面学习并补齐同名问题集。
- **Course/Reference** — [CUDA Programming Guide](https://docs.nvidia.com/cuda/cuda-programming-guide/)：
  新版 guided Parts 1–3；不要再用旧 `cuda-c-programming-guide` 作为课程入口。

### W13–W14：programming model/SIMT

- [Programming Model](https://docs.nvidia.com/cuda/cuda-programming-guide/01-introduction/programming-model.html)
- [Introduction to CUDA C++](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/intro-to-cuda-cpp.html)
- [An Even Easier Introduction to CUDA](https://developer.nvidia.com/blog/even-easier-introduction-cuda/)：
  guided first kernel，不承担完整理论。

### W15–W16：memory/coalescing/shared memory

- [CUDA Best Practices](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html)：
  §9.2 Bandwidth、§9.2.2 Effective Bandwidth、§10.2.1 Coalesced Global Memory、
  §10.2.3 Shared Memory/Bank Conflicts、§10.2.3.2 Shared-memory MatMul。
- CUDA Programming Guide：Writing SIMT Kernels、Writing Tile Kernels。

### W17–W18：async errors/timing/sanitizer

- [Asynchronous Execution](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/asynchronous-execution.html)：
  §2.5.1–2.5.3、§2.5.4.2 asynchronous errors。
- CUDA Best Practices §9.1 Timing，重点 §9.1.2 CUDA Events。
- [Compute Sanitizer](https://docs.nvidia.com/cuda/compute-sanitizer/index.html)：
  Overview、memcheck、racecheck、initcheck、synccheck。

## Unit 4 — Profiling 与 Resize Preprocessor

### W19–W20：measurement/profiling

- [Nsight Systems User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html)：
  Basic CUDA Trace、`nsys profile`、CLI Stats、CUDA timeline。
- [Nsight Compute Quickstart](https://docs.nvidia.com/nsight-compute/NsightCompute/)：
  Quickstart §3.1–3.2。
- [Nsight Compute Profiling Guide](https://docs.nvidia.com/nsight-compute/ProfilingGuide/index.html)：
  SpeedOfLight、LaunchStats、Occupancy、WarpStateStats、MemoryWorkloadAnalysis。
- CUDA Best Practices §11.1 Occupancy、§11.3 Thread/Block Heuristics。

课程顺序是 memory/coalescing/shared memory 之后再谈 occupancy；牢记“更高 occupancy
不等于更快”。Systems 先定位阶段，Compute 再分析 candidate kernel。

### W21–W24：Resize semantics→oracle→CUDA→case study

- **Semantic source** — [ONNX Resize specification](https://onnx.ai/onnx/operators/onnx__Resize.html)：
  `half_pixel`、`asymmetric`、`align_corners`、nearest mode、rounding、antialias。
- **API comparison** — [OpenCV Geometric Transformations](https://docs.opencv.org/master/da/d6e/tutorial_py_geometric_transformations.html)、
  [Image Transform API](https://docs.opencv.org/master/da/d54/group__imgproc__transform.html)。
- **Optional platform comparison** — [VPI Rescale](https://docs.nvidia.com/vpi/4.0/algo_rescale.html)。

写 CUDA 前必须冻结 coordinate transform、pixel center、inverse mapping、border、
rounding、dtype 和 tolerance。

## Unit 5 — Streams、Ownership 与 Multi-frame Pipeline

### W25–W30

- CUDA Programming Guide Asynchronous Execution：Streams、Events、Ordering、Default Stream。
- CUDA Best Practices §10.1.1 Pinned Memory、§10.1.2 Async/Overlap、§10.1.3 Zero Copy。
- [CUDA for Tegra](https://docs.nvidia.com/cuda/cuda-for-tegra-appnote/)：
  §3 Memory Management、§4.1 Memory Selection。
- [CUDA `simpleStreams` sample](https://github.com/NVIDIA/cuda-samples/tree/master/Samples/0_Introduction/simpleStreams)
  和 [`asyncAPI`](https://github.com/NVIDIA/cuda-samples/tree/master/Samples/0_Introduction/asyncAPI)：
  仅用于对照，不复制为 summative lab。
- Nsight Systems CUDA timeline。

Affine/GridSample 不再是 Core milestone。若最终 camera/use case 明确需要，再选修：

- [ONNX GridSample](https://onnx.ai/onnx/operators/onnx__GridSample.html)
- OpenCV `warpAffine`/`remap`
- MML §2.7 Linear Mappings、§2.8 Affine Spaces

## Unit 6 — ORT CUDA 与 TensorRT

### W31–W32：ORT CUDA

- [Execution Providers](https://onnxruntime.ai/docs/execution-providers/)：
  provider partition、priority、fallback。
- [CUDA Execution Provider](https://onnxruntime.ai/docs/execution-providers/CUDA-ExecutionProvider.html)：
  compatibility、`user_compute_stream`、copy stream、CUDA Graph options。
- [I/O Binding](https://onnxruntime.ai/docs/performance/tune-performance/iobinding.html)。
- ORT Python API Summary：Data on Device、OrtValue、IOBinding。

### W33–W36：TensorRT 与 direct-device integration

**版本规则**：先记录实际 JetPack/CUDA/cuDNN/TensorRT，再选择匹配 archive。课程链接指向
[TensorRT 10.x documentation](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/)；
不要盲用 `latest`，也不要假定 desktop 生成的 engine 可在 Orin 运行。

- [TensorRT Documentation Hub](https://docs.nvidia.com/tensorrt/)：用于确认当前受支持
  文档与 release note 入口；解释设备行为时仍回到实际安装版本对应的 archive。

- [Quick Start](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/getting-started/quick-start-guide.html)：
  Ecosystem、ONNX deployment。
- [Best Practices](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/performance/best-practices.html)：
  `trtexec`、benchmarking、profiling。
- [C++ API](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/c-api-docs.html)：
  Build Phase、Import ONNX、Serialize/Deserialize、Performing Inference、`enqueueV3`。
- [How TensorRT Works](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/architecture/how-trt-works.html)：
  object lifetime、memory、threading。
- [Accuracy Considerations](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/accuracy-considerations.html)：
  FP32/TF32/FP16、overflow/underflow、mitigation。
- [Dynamic Shapes](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/work-dynamic-shapes.html)：
  runtime dimensions、min/opt/max profile、input shape。

## Unit 7 — Orin、Power/Thermal 与 Reliability

### 平台版本

- [JetPack downloads and release notes](https://developer.nvidia.com/embedded/jetpack/downloads)：
  当前入口只用于确认支持矩阵；实验以实际设备为准。
- [Jetson Linux archives](https://docs.nvidia.com/jetson/archives/)：
  选择与 `cat /etc/nv_tegra_release` 匹配的文档。
- [Jetson Linux r39.2 Guide](https://docs.nvidia.com/jetson/archives/r39.2/DeveloperGuide/)：
  仅当设备确为 r39.2 时使用。
- [Orin Power and Performance](https://docs.nvidia.com/jetson/archives/r39.2/DeveloperGuide/SD/PlatformPowerAndPerformance/JetsonOrinNanoSeriesJetsonOrinNxSeriesAndJetsonAgxOrinSeries.html)：
  supported modes、`nvpmodel`、thermal、power modeling、`jetson_clocks`。
- [Tegrastats](https://docs.nvidia.com/jetson/archives/r39.2/DeveloperGuide/AT/JetsonLinuxDevelopmentTools/TegrastatsUtility.html)：
  Reported Statistics、Running tegrastats。

### Quantization theory

- [TensorRT Quantized Types](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/work-quantized-types.html)：
  explicit Q/DQ、scale、zero-point、granularity、INT8 scheme。

完整 INT8 calibration、DLA、plugin 是 elective；Core 先完成 FP32/FP16/dynamic 和
device-resident correctness。

## Unit 8 — Release、Security、GH-600

### GitHub Agentic AI / GH-600

- **Exam blueprint** — [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600)。
- **Course** — [GH-600T00](https://learn.microsoft.com/en-us/training/courses/gh-600t00)：
  官方页面在 **2026-07-26** 仍写明课程将于 **2026-07-31** 可用。上线前以紧随其后的
  三个已发布 Learn modules 为教学主线；上线后先复核页面再纳入排期。
- **Course 1** — [Foundations of Agentic AI in GitHub](https://learn.microsoft.com/en-us/training/modules/foundations-agentic-ai/)。
- **Course 2** — [Design agent architecture and SDLC integration](https://learn.microsoft.com/en-us/training/modules/design-agent-architecture-integration/)。
- **Course 3** — [Agent tooling, MCP, and execution environments](https://learn.microsoft.com/en-us/training/modules/agent-tooling-mcp-execution-environments/)。
- [Repository custom instructions](https://docs.github.com/en/copilot/how-tos/copilot-on-github/customize-copilot/add-custom-instructions/add-repository-instructions)
- [About custom agents](https://docs.github.com/en/copilot/concepts/agents/copilot-cli/about-custom-agents)
- [Custom agent configuration](https://docs.github.com/en/copilot/reference/custom-agents-configuration)
- [Managing agent sessions](https://docs.github.com/en/copilot/how-tos/copilot-on-github/use-copilot-agents/manage-and-track-agents)
- [Agent risks and mitigations](https://docs.github.com/en/enterprise-cloud@latest/copilot/concepts/agents/cloud-agent/risks-and-mitigations)

GH-600 readiness 每次都重新核对考纲。自编 timed mock 必须标注非官方；exam sandbox
只用于熟悉 UI。

## Agent/MCP 副线完整教学顺序

### U1：GitHub control plane 与 agent fundamentals

Git/GitHub/Actions 课程 → Foundations module → architecture/trust boundaries。没有 repository、
branch、PR、checks 和 governance 基础，不进入 custom agent。

### U2：agent architecture、typed tools 与 safe execution

- [Design agent architecture and SDLC integration](https://learn.microsoft.com/en-us/training/modules/design-agent-architecture-integration/)
- [Agent tooling, MCP, and execution environments](https://learn.microsoft.com/en-us/training/modules/agent-tooling-mcp-execution-environments/)
- GitHub Actions token/security references。

### U3：MCP protocol 与 stdio

统一记录 stable revision **2025-11-25**：

- [Architecture](https://modelcontextprotocol.io/docs/learn/architecture)
- [Lifecycle](https://modelcontextprotocol.io/specification/2025-11-25/basic/lifecycle)
- [Transports](https://modelcontextprotocol.io/specification/2025-11-25/basic/transports)
- [Tools](https://modelcontextprotocol.io/specification/2025-11-25/server/tools)
- [Resources](https://modelcontextprotocol.io/specification/2025-11-25/server/resources)
- [Prompts](https://modelcontextprotocol.io/specification/2025-11-25/server/prompts)

### U4：server、Inspector、HTTP/auth/security

- [Build an MCP server](https://modelcontextprotocol.io/docs/develop/build-server)
- [MCP Inspector](https://modelcontextprotocol.io/docs/tools/inspector)
- [Debugging](https://modelcontextprotocol.io/docs/tools/debugging)
- [Authorization](https://modelcontextprotocol.io/specification/2025-11-25/basic/authorization)
- [Security Best Practices](https://modelcontextprotocol.io/docs/tutorials/security/security_best_practices)

顺序必须是 local read-only server 正确后，再进入 HTTP/auth/deploy。

### U5–U6：memory/eval/multi-agent

以本地、可重复的 artifact store/state machine/eval fixtures 为 mandatory：

- state identity、provenance、TTL、reset、resume、drift；
- versioned eval dataset、failure taxonomy、trace bundle、one-variable tuning；
- sequential/parallel/reviewer/handoff、isolation、conflict recovery。

[Copilot Memory](https://docs.github.com/en/copilot/concepts/agents/copilot-memory) 和
[GitHub Models evaluation](https://docs.github.com/en/github-models/use-github-models/evaluating-ai-models)
若仍为 preview，只作产品案例/elective，不能代替通用机制和本地 harness。

### U7–U8：guardrails、GitHub integration、production

- [Set up GitHub MCP Server](https://docs.github.com/en/copilot/how-tos/provide-context/use-mcp-in-your-ide/set-up-the-github-mcp-server)
- [Configure GitHub MCP toolsets](https://docs.github.com/en/copilot/how-tos/provide-context/use-mcp-in-your-ide/configure-toolsets)
- [Configure MCP registry](https://docs.github.com/en/copilot/how-tos/administer-copilot/manage-mcp-usage/configure-mcp-registry)
- [MCP allowlist limitations](https://docs.github.com/en/copilot/reference/mcp-allowlist-enforcement)
- [GitHub Agentic Workflows](https://docs.github.com/en/copilot/how-tos/github-agentic-workflows)：
  若仍为 preview 则 elective；standard Actions/security 是 mandatory。

Registry/allowlist 不得被描述成可靠 security boundary；实际限制、entitlement 和绕过风险
必须写入 threat model。

## Optional Seminar：Podcast / Community / Blog

Podcast/blog 是可选的，永远不承担先修，也不替代官方文档、代码正确性或 profiler
证据。读到有价值的结论时，用 `claim → source/evidence → scope → experiment` 四段
记在 `notes/experiments/` 下——没有可验证证据的就标注「仅观点」。

- [Chip Huyen](https://huyenchip.com/)：ML systems / AI engineering。
- [Lilian Weng](https://lilianweng.github.io/)：agent/tool/research overview。
- [Simon Willison](https://simonwillison.net/)：LLM apps、agents、eval、security。
- [Latent Space](https://www.latent.space/podcast)
- [MLOps Community Podcast](https://mlops.community/podcast)
- [The Gradient](https://thegradient.pub/)
- [MLSys](https://mlsys.org/)
- [AI Engineer](https://www.ai.engineer/)

复盘只保留 `claim → source/evidence → scope → experiment/decision`。行业观点与官方文档
冲突时，先记录冲突，不擅自把观点升级为 API 或硬件事实。
