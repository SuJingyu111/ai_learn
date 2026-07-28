# Week 02 — Stride、Layout、View/Copy 与 Actions/SDLC

> **Unit 1 · ML Systems Foundations（第 2/6 周）**  
> **课程角色**：从逻辑 tensor 进入物理布局；sidecar 把 GitHub Flow 扩展为可观察的 Actions/SDLC。课程依赖见 [CURRICULUM](../CURRICULUM.md)。

## 开始前：硬先修与 remediation

- **硬先修**：Week 01 能从 shape/dtype 算 elements/bytes，`week01_array_tensor.py` assertions 通过。
- **先修检查（10 分钟）**：手算 `3×4` row-major 的 `(2,1)` offset；答案不是 `9` 时不得直接做 transpose。
- **失败时 remediation**：重做 Week 01 worked example，并用一维 flat array 验证公式；30 分钟后仍失败则把本周 lab 限定为二维 contiguous，non-contiguous 任务标 blocked。

## Observable objectives

- 由 shape 与 element/byte strides 手算二维和 NCHW offset。
- 用 `shares_memory`/修改元素区分 view 与 copy。
- 解释 transpose、reshape、contiguous 的实际行为而不跨 framework 泛化。
- 实现并验证 HWC→CHW reference conversion。
- 读懂 GitHub Actions workflow 的 trigger、job、step、permissions 和 artifact。

## 术语表

需能解释：**stride、layout、contiguous、row-major、offset、view、copy、reshape、transpose、NCHW、NHWC、workflow、job、step、runner、artifact**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[NumPy Indexing on ndarrays](https://numpy.org/doc/stable/user/basics.indexing.html) — basic indexing 与 slicing。
- **T2（Day 2，20 分钟）**：[NumPy Copies and Views](https://numpy.org/doc/stable/user/basics.copies.html) — indexing、reshape、transpose。
- **T3（Day 3，20 分钟）**：[PyTorch Tensor Views](https://docs.pytorch.org/docs/stable/tensor_view.html) — view ops、contiguity。
- **S1（Day 1，20 分钟）**：[Understanding GitHub Actions](https://docs.github.com/en/actions/about-github-actions/understanding-github-actions) — workflows、events、jobs、actions、runners。
- **S2（Day 2，15 分钟）**：[Workflow syntax: permissions](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax#permissions) — least-privilege permissions。

95 分钟全部计入 Day 1–3。Podcast/blog 仅可选 seminar。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **30–45 分钟；先修或 Gate 落后时取消**：从 [GitHub Changelog](https://github.blog/changelog/) 选择一篇近 12 个月的 Actions、workflow permissions 或 artifact 更新；必须能指出它改变的是产品行为、默认值还是仅新增功能。
- 在 `notes/feed/week-02.md` 写 `claim → source/evidence → scope → experiment/decision`；若无法在本地 fixture 验证，就把 decision 写成“等待验证”。可散步听相关视频/音频，不计 unlock。

## 带问题阅读

- stride 为什么必须说明单位是 elements 还是 bytes？
- transpose 为什么能改变逻辑 axes 而不搬数据？
- reshape 成功是否等价于一定共享 storage？
- CI workflow 的 YAML 声明和实际权限保证分别由谁提供？

## Worked example / 手算

NCHW tensor shape=`(1,3,4,5)` 的 contiguous element strides=`(60,20,5,1)`；坐标 `(0,2,3,4)` offset=`0×60+2×20+3×5+4=59`。FP32 byte offset=`59×4=236`。若转 NHWC `(1,4,5,3)`，需重新根据对应 strides 计算，不能沿用 59 的公式。

## 累计项目 increment

- `labs/ml/week02_stride_layout.py`
- `labs/ml/week02_hwc_chw.py`
- `notes/concepts/stride-layout-view-copy.md`
- `notes/ai-app/week-02-actions-sdlc.md`

## Exit ticket / unlocks

**形成性**：手算 5 个 offset、正确判定 4 个 view/copy cases、HWC→CHW 3 个 tests
通过，并能解释 `.github/workflows/ci.yml` 的权限/产物；按
[getting started](../docs/getting-started.md#4-第一次-ml-baseline) 完成
NumPy/PyTorch/ONNX/ORT import/version smoke test 与 freeze。全部完成才 unlock
Week 03；失败项用下周首 30 分钟补救。

## Day 1

**阶段：理论 + 地址模型（100 分钟）**

- **20 分钟**：读 T1，回答 basic slicing 是否复制。
- **25 分钟**：逐步重做顶部 NCHW worked example，再做 NHWC 对应坐标。
- **35 分钟**：guided lab：打印 `shape/strides/itemsize`，用 5 个坐标验证手算。
- **20 分钟**：读 S1 并只读标注 `.github/workflows/ci.yml` 的 trigger/job/step/runner/artifact。
- **必须留下**：两套地址公式、5 行验证表和 workflow anatomy。

## Day 2

**阶段：worked example + view/copy（100 分钟）**

- **20 分钟**：读 T2，先预测再记录 slice/advanced indexing/reshape/transpose 行为。
- **30 分钟**：手画 base、slice、transpose 的 shape/stride/storage relationship。
- **35 分钟**：用 `np.shares_memory`、元素 mutation 和 data pointer 验证预测。
- **15 分钟**：读 S2，记录现有 CI permissions 能做/不能做什么。
- **必须留下**：view/copy 表、一次预测修正和 permissions boundary。

## Day 3

**阶段：guided lab（100 分钟）**

- **20 分钟**：读 T3，区分 PyTorch `view/reshape/contiguous`。
- **55 分钟**：创建 `week02_stride_layout.py`，对 contiguous 与 transposed tensor 调用三者，保存 result/exception/storage sharing。
- **15 分钟**：为 CI 设计一个只读 test artifact 流程；不触发远端 workflow。
- **10 分钟**：记录 NumPy/PyTorch 行为差异与版本。
- **必须留下**：可运行脚本、framework 对照和 CI artifact 纸面流程。

## Day 4

**阶段：independent lab（100 分钟）**

- **70 分钟**：独立实现 HWC→CHW CPU conversion，支持固定 contract；测试 `1×1×1`、odd dimensions、RGBA、padded row（若 contract 支持），和 NumPy transpose+copy 比较。
- **20 分钟**：给 Actions/SDLC 增加 failure path：test fail 时禁止 artifact 被解释为通过、禁止自动 merge。
- **10 分钟**：输出 first mismatch 坐标/expected/actual。
- **必须留下**：conversion、case matrix、failure gate 和首错报告。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：整合 offset、view/copy、HWC→CHW；从 clean process 重跑全部 tests，并检查 empty/odd/non-contiguous。
- **20 分钟**：写 4 个 Actions 场景题，覆盖 untrusted PR、permissions、artifact、failed job。
- **20 分钟**：完成 Exit ticket 初测并分类概念/计算/实验错误。
- **必须留下**：clean run、4 个 SDLC cases 和 exit 错误分类。

## Day 6

**阶段：review / unlock（100 分钟）**

- **40 分钟**：修复 exit 错项并闭卷重做 5 个 offsets。
- **25 分钟**：完成 `stride-layout-view-copy.md`，加入 worked address 与 storage 图。
- **20 分钟**：用自己的话解释 workflow YAML、runner execution、artifact evidence 的边界。
- **15 分钟**：填写 `notes/weekly/week-02.md` 和 Week 03 MatMul 第一题。
- **必须留下**：形成性 Exit ticket、概念笔记、可复现命令和 unlock 状态。
