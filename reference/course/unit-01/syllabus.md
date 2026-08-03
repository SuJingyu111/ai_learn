# Unit 1 Syllabus — Tensor 与 Inference Math Bridge

**Weeks 1–6 · M1 · Gate G1**

## Entry prerequisite

完成 [`START_HERE.md`](../../START_HERE.md) 的诊断并写下 `READY FOR WEEK 01`。本单元
不要求 ML 经历，但要求能运行 Python/NumPy、完成 CPU clean build，并理解基础数组。
PyTorch 可在 Week 1 暂缺，但必须在 Week 2 Exit Ticket 前按
[`getting-started.md`](../../docs/getting-started.md#4-第一次-ml-baseline) 完成
NumPy/PyTorch/ONNX/ORT import/version smoke test。

## Observable outcomes

完成本单元后，学习者能够：

1. 从 storage offset、shape、stride、dtype 手算任意合法 element 的物理地址；
2. 对 slice/transpose/reshape/view/contiguous/copy 预测 storage sharing；
3. 手算 MatMul 元素、Conv/Pool 输出 shape、参数量、activation bytes 和近似 FLOPs；
4. 解释 broadcast、reduction、activation、normalization、softmax 的数据流；
5. 区分 parameter、activation、operator 和 kernel，并说明 training 与 inference 的边界；
6. 用明确 tolerance 验证 CPU normalize/HWC→CHW 与 tiny-model vertical slice。

## Concept inventory

`storage`、`storage_offset`、`shape`、`stride`、`dtype`、`layout`、`contiguous`、
`view`、`copy`、`broadcast`、`reduction`、`MatMul`、`cross-correlation`、`kernel size`、
`stride/padding/dilation`、`activation`、`pooling`、`normalization`、`softmax`、
`parameter`、`activation memory`、`operator`、`kernel`、`FP32`、`ULP`、
`absolute/relative tolerance`、`training/eval/inference`。

## Primary teaching spine

- [D2L: Data Manipulation](https://d2l.ai/chapter_preliminaries/ndarray.html)：array 创建、
  shape、indexing、broadcast、memory。
- [D2L: Linear Algebra](https://d2l.ai/chapter_preliminaries/linear-algebra.html)：scalar、
  vector、matrix、tensor、reduction、dot/MatMul。
- [PyTorch Tensors](https://docs.pytorch.org/tutorials/beginner/basics/tensorqs_tutorial.html)：
  attributes、operations、NumPy bridge。
- [PyTorch Tensor Views](https://docs.pytorch.org/docs/stable/tensor_view.html)：view/copy 和
  contiguous 行为。
- [CS231n: Convolutional Networks](https://cs231n.github.io/convolutional-networks/)：
  convolution layer、spatial arrangement、parameter sharing、pooling。
- [PyTorch Build Model](https://docs.pytorch.org/tutorials/beginner/basics/buildmodel_tutorial.html)：
  只建立 runtime 所需的 module/operator/dataflow 心智模型。
- [D2L: Softmax Regression](https://d2l.ai/chapter_linear-classification/softmax-regression.html)：
  只读 softmax operation、normalization 与输出解释，不展开训练推导。

官方 API 页用于核对行为，不代替上述连续教学材料。

## Six-week lesson sequence

| Week | Theory lesson | Worked problem | Guided/independent lab | Project increment |
|---:|---|---|---|---|
| 1 | array/tensor metadata；诊断复测 | 4D element/byte offset | NumPy metadata explorer | input/intermediate manifest |
| 2 | stride/layout/view/copy | transpose/slice 地址表 | storage sharing + padded stride | HWC→CHW CPU starter |
| 3 | vector/matrix/MatMul/broadcast/reduction | 逐元素 MatMul + bytes | naive MatMul/reference checks | normalize + layout stage |
| 4 | Conv/cross-correlation/pool/operator dataflow | Conv shape、3 个 output、FLOPs | tiny CNN shape tracer | tiny model skeleton |
| 5 | FP/tolerance；parameter/activation；training/inference | tolerance failure cases | CPU vertical slice integration | M1 release candidate |
| 6 | cumulative synthesis | unseen mixed problem set | unseen tensor transform | M1 defense / G1 |

## Required problem set

- P1：10 个 shape/stride/storage offset 题，其中 3 个 non-contiguous。
- P2：预测 8 个 view/copy 操作并用代码验证；错误预测必须解释原因。
- P3：4 个 MatMul 与 6 个 Conv/Pool shape 题；至少 2 个含 dilation/odd size。
- P4：为 tiny CNN 估算 parameters、activation bytes、multiply-adds。
- P5：设计 absolute+relative tolerance，并给出 near-zero 和 large-value 反例。

每题先手算再用代码验证；代码输出不能代替推导。

## Guided lab ladder

1. 元数据 explorer；
2. storage sharing 和 padded-stride fixture；
3. independent normalize + HWC→CHW；
4. tiny model shape hook；
5. synthetic image → CPU preprocessing → PyTorch inference → intermediate dump。

至少在一项 lab 中保留一个受控错误、first mismatch 和 regression test。

## Sidecar prerequisite chain

Week 1–2 先完成 Git/GitHub Flow 与 Actions/SDLC；Week 3–6 再学习 agent vs assistant、
plan→act→evaluate、GitHub control plane、trust boundary、PR governance。此单元不提前
教授 MCP 协议。

主材料：

- [Introduction to Git](https://learn.microsoft.com/en-us/training/modules/intro-to-git/)
- [Introduction to GitHub](https://learn.microsoft.com/en-us/training/modules/introduction-to-github/)
- [Introduction to GitHub Actions](https://learn.microsoft.com/en-us/training/modules/introduction-to-github-actions/)
- [Foundations of Agentic AI in GitHub](https://learn.microsoft.com/en-us/training/modules/foundations-agentic-ai/)

## G1

按 [`mastery-gates.md`](../../docs/mastery-gates.md) 的 25/30/30/15 rubric：

- closed-book unseen stride/MatMul/Conv/tolerance exam；
- independent padded-stride normalize/HWC→CHW；
- M1 clean run；
- 5 分钟解释 operator/kernel、parameter/activation、training/inference。

任何 address/shape/CPU oracle 错误均阻塞 U2。重修只针对失败概念，但使用新 shape、
stride 和 input。
