# Unit 2 Syllabus — Model Lifecycle、ONNX 与 ORT CPU

**Weeks 7–12 · M2 · Gate G2**

## Entry prerequisite

G1 PASS；能够独立实现 CPU tensor transform，并用 shape/stride/tolerance 解释结果。

## Observable outcomes

1. 区分 parameter、buffer、gradient、optimizer state、checkpoint 和 runtime input；
2. 解释 `train/eval` 与 autograd/no-grad/inference mode 的不同职责；
3. 从 PyTorch program 画 computational graph，并说明 control flow/export 限制；
4. 读懂 ONNX graph 的 input/output/node/initializer/attribute/opset/type/shape；
5. 导出并检查模型，在 ORT CPU 上与 PyTorch 做可复现的 correctness comparison；
6. 分开 session creation、first run、warm-up 和 steady-state latency。

## Concept inventory

`Module`、`state_dict`、`parameter`、`buffer`、`checkpoint`、`autograd`、`forward graph`、
`eval`、`no_grad`、`inference_mode`、`IR`、`node`、`initializer`、`attribute`、`opset`、
`shape inference`、`dynamic shape`、`Execution Provider`、`graph optimization`、
`fallback`、`warm-up`、`median`、`p95`、`experiment contract`。

## Primary teaching spine

- [PyTorch Build Model](https://docs.pytorch.org/tutorials/beginner/basics/buildmodel_tutorial.html)
- [PyTorch Autograd](https://docs.pytorch.org/tutorials/beginner/basics/autogradqs_tutorial.html)
- [Save and Load Model](https://docs.pytorch.org/tutorials/beginner/basics/saveloadrun_tutorial.html)
- [Autograd grad modes](https://docs.pytorch.org/docs/stable/notes/autograd.html#locally-disable-grad-doc)
- [PyTorch ONNX export tutorial](https://docs.pytorch.org/tutorials/beginner/onnx/export_simple_model_to_onnx_tutorial.html)
- [ONNX Concepts](https://onnx.ai/onnx/intro/concepts.html)
- [ONNX Python API introduction](https://onnx.ai/onnx/intro/python.html)
- [ORT API Basics](https://onnxruntime.ai/docs/tutorials/api-basics.html)
- [ORT Python API summary](https://onnxruntime.ai/docs/api/python/api_summary.html)

## Six-week lesson sequence

| Week | Theory lesson | Worked problem | Lab | Project increment |
|---:|---|---|---|---|
| 7 | Module/state/checkpoint/eval | state inventory + dropout/BN cases | save/reload/eval fixture | reproducible tiny model |
| 8 | computational graph/autograd mental model | trace program→graph | forward hooks/graph sketch | model I/O draft |
| 9 | ONNX IR/opset/export | module→nodes mapping | export/checker/shape inference | freeze model/input/normalize/layout |
| 10 | ORT CPU/provider/optimization | partition/fusion reasoning | ORT session + metadata | CPU backend |
| 11 | correctness/tolerance/benchmark statistics | cold/warm sample analysis | cross-backend harness | manifest + raw samples |
| 12 | clean vertical slice | unseen graph/timing questions | clean checkout reproduction | M2 / G2 |

## Required problem set

- 对含 Dropout/BatchNorm 的模型解释 `eval()`、`no_grad()`、`inference_mode()` 的不同影响。
- 为 checkpoint 列出 required state 与不应被序列化的 transient state。
- 从一个 8–12 node graph 标出 initializer、runtime input、constant、attribute。
- 给出 opset mismatch、unsupported node、dynamic shape 的三个 failure traces。
- 从 raw latency samples 计算 median/p95，并指出把 model load 混入 steady-state 的错误。

## Guided lab ladder

1. deterministic save/reload；
2. hook-based tensor shape/dataflow trace；
3. `torch.onnx` export + checker + shape inference；
4. ORT CPU session；
5. PyTorch/ORT comparator；
6. clean script 一次生成 model、运行 correctness 和 timing。

M2 必须在 Week 9–12 冻结最终 capstone model、dataset、runtime input tensor、
normalize/layout、output 与 measurement contract；Week 41 不再首次决定这些内容。
Source-image 到 model tensor 的目标 shape 可以记录，但 Resize coordinate、pixel-center、
border、rounding 与 tolerance 在 U4 正式学习后由 M4 冻结。

## Sidecar prerequisite chain

Week 7–8：custom agent、scope、session log 和 eval baseline。Week 9–12：typed tool、
JSON Schema、structured result/error、least privilege、timeout/retry/rollback/escalation。

主材料：

- [Design agent architecture and SDLC integration](https://learn.microsoft.com/en-us/training/modules/design-agent-architecture-integration/)
- [About custom agents](https://docs.github.com/en/copilot/concepts/agents/copilot-cli/about-custom-agents)
- [Agent tooling, MCP, and execution environments](https://learn.microsoft.com/en-us/training/modules/agent-tooling-mcp-execution-environments/)
- [GITHUB_TOKEN](https://docs.github.com/en/actions/concepts/security/github_token)
- [Script injections](https://docs.github.com/en/actions/concepts/security/script-injections)

## G2

- closed-book lifecycle/ONNX/ORT/measurement exam；
- unseen model export 或 graph diagnosis；
- clean PyTorch→ONNX→ORT CPU M2；
- oral defense：为什么 module 不一定一一对应 node，为什么 first run 不是 steady state。

export contract、cross-backend tolerance、clean reproduction 任一失败均阻塞 U3。
