# Unit 6 · Week 36 — Direct-device Integration 与 M6 Hard Gate

> **课程角色**：Unit 6 结课。将正确的 CUDA preprocessing output 直接绑定到 TensorRT
> input，统一 buffer/shape/stream contract，消除未解释 host round-trip。Sidecar 聚焦
> multi-agent conflict、recovery 与 retirement；Day 5–6 执行 M6 两日 hard gate。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：G2 的 model/I/O/preprocessing contract 与 CPU inference oracle、
  G4 的独立 CPU/CUDA resize-normalize-layout preprocessor、G5 的 stream/ownership
  pipeline 均已 PASS；并具备 W31–32 ORT CUDA provider/fallback/I/O Binding evidence、
  W34 runner/lifetime 与 W35 FP16/profile/reference。
- TensorRT engine 与文档必须继续匹配
  `artifacts/week-33/trt-version-pin.md`；任何 JetPack/TensorRT 变化使 engine 与 Gate
  evidence 失效。禁止用 `latest` 文档或其他 GPU 的 engine。
- **Remediation**：direct path 不正确时退回 FP32+fixed shape+separate correct CUDA
  kernels，不做 fusion；无 Orin/无匹配 engine 时允许完成设计与 CPU tests，但 M6 blocked。

## Observable objectives（可观察目标）

完成后能够：

1. 将 CUDA output device pointer 作为 TensorRT input address，不经过隐式 host round-trip。
2. 用 event/stream evidence 证明 input-ready 与 output-ready ordering。
3. 对 ORT CUDA 与 TensorRT 的 fixed/FP16/dynamic（适用时）运行 end-to-end correctness。
4. 用 version/owner/artifact evidence 处理 duplicate/conflicting agents 并安全 retire。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Direct-device path | preprocessing output device buffer 直接成为 runtime input |
| Producer-consumer dependency | producer 完成写入后 consumer 才能读取的 happens-before |
| Hidden round-trip | 未在 contract 中声明的 D2H→H2D 数据路径 |
| Canonical artifact | 经 owner/reviewer 接受的唯一版本化结果 |
| Recovery ledger | 记录 task/owner/version/failure/action/evidence 的恢复账本 |
| Hard gate | 分数达标且所有 blockers 均通过，缺一不可 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin C++ guide 的 tensor addresses、execution context、enqueue | 20 | 1 |
| T2 | [CUDA Runtime API](https://docs.nvidia.com/cuda/cuda-runtime-api/) — “Stream Management” 与 “Event Management” | 20 | 2 |
| T3 | [Managing agent sessions](https://docs.github.com/en/copilot/how-tos/copilot-on-github/use-copilot-agents/manage-and-track-agents) — monitor、session logs、steer、stop/archive 与 traceability | 10 | 4 |
| T4 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** blueprint/domain/skill mapping，不作为 recovery/retirement 教材 | 5 | 4 |

## 带问题阅读

- 同一 device pointer 能否直接复用，取决于哪些 dtype/layout/shape/lifetime 条件？
- same-stream ordering 与 cross-stream event dependency 各证明什么？
- 两个 agents 给出冲突结论时，为什么不能简单采用“最后一条”？

## Worked example

```text
CUDA preprocess writes d_input on stream S
TensorRT context binds input_name -> d_input
enqueue(context, S)
record output_ready on S
consumer reads d_output only after output_ready
```

若 runtime 使用另一 stream T，则必须由 event 建立 `S producer → T wait → enqueue`；
全局同步不是默认修复。冲突 artifacts 用 source/version/hash/correctness evidence 仲裁。

## 累计项目 increment

- `artifacts/week-36/direct-device-correctness.json`
- `artifacts/week-36/backend-comparison.md`
- `artifacts/week-36/runtime-timeline.md`
- `artifacts/week-36/m6-scorecard.md`
- `notes/gates/gate-6.md`
- `notes/ai-app/week-36.md`（recovery ledger 与 retirement checklist）
- `docs/capstone-architecture.md`（device path 草案）

## 本周必须产出

- CUDA preprocessing→TensorRT direct-device path、correctness 与 timeline。
- no-hidden-round-trip evidence、buffer/stream lifetime contract。
- duplicate/conflict/recovery fixtures、M6 scorecard 与 go/remediate decision。

## Day 1 — 理论与 integration contract（105 分钟）

- **20 分钟**：阅读 T1，核对实际版本的 tensor-address/enqueue API。
- **25 分钟**：从 W9–12 contract 写 source image→model tensor 的 shape/layout/dtype/
  color/range/normalization 与 tolerance。
- **50 分钟**：画 CPU baseline、CUDA-staged、direct-device 三条 paths，列每个 buffer
  bytes/owner/lifetime；选 FP32 fixed 作为 first integration。
- **10 分钟**：预注册 correctness 与 no-round-trip criteria。

**必须留下**：integration contract、三条 path diagram 和 acceptance criteria。

## Day 2 — Guided lab：direct address 与 ordering（110 分钟）

- **20 分钟**：阅读 T2，回答 same-stream/event 的 dependency 差异。
- **70 分钟**：组合已正确 CUDA stages，reuse explicit stream/preallocated buffer；
  将 final device buffer 绑定为 TensorRT input，运行 fixed case，对比 CPU preprocessing+
  runtime reference。
- **10 分钟**：用 event 标 input-ready/output-ready。
- **10 分钟**：保存 first mismatch 或 max error。

**必须留下**：direct-device code path、correctness JSON 和 stream/event timeline。

## Day 3 — Independent lab：shape/precision matrix（105 分钟）

- **15 分钟**：冻结 FP32/FP16 与 fixed/dynamic test matrix。
- **70 分钟**：运行 ORT CUDA 与 TensorRT 代表 inputs；逐 stage/full/final output
  compare，检查 provider/fallback、I/O Binding 或 tensor address、buffer size、profile
  range 与 runtime shape；任何 correctness regression 先停 timing。
- **20 分钟**：用 Nsight Systems 或明确 copy logging 查 D2H/H2D、sync、allocation。

**必须留下**：matrix results、copy inventory 和 unexplained-gap list。

## Day 4 — Integration：Recovery/retire drill（110 分钟）

- **15 分钟**：阅读 T3+T4；从 T3 写 monitor/steer/stop/archive 的 evidence boundary，
  用 T4 只定位 blueprint/domain。
- **60 分钟**：修复 Day 3 最高优先级 integration gap并重跑；不做无证据 fusion。
- **25 分钟**：在本地 fixtures 模拟 duplicate work、contradictory artifacts、worker
  timeout；按 owner/version/hash/evidence 决定 deduplicate、retry、escalate。
- **10 分钟**：完成 agent scope/lease retirement，不删除 audit history。

**必须留下**：修复证据、recovery ledger 和 retirement trace。

## Day 5 — M6 Gate A：Knowledge 与 submission audit（105 分钟）

- **55 分钟**：闭卷完成预先冻结、首次作答留痕的 scored Knowledge assessment，
  计 **25 分**；覆盖 runtime object/lifetime、shape/profile、direct-device ordering、
  fallback 与 agent recovery，不在本段订正。
- **25 分钟**：执行 submission audit：核对 commit、environment、version pin、variant
  ID、artifact hashes、commands 与四项 rubric 的 evidence index；缺项不得临时补造。
- **25 分钟**：执行 blocker audit：逐条判定 correctness/security/Orin/engine/
  direct-device blocker，冻结 Day 6 submission 与 unseen variant seed。

**必须留下**：Knowledge 原始答卷与 25 分得分、submission manifest、blocker audit 和
Day 6 frozen variant ID。

## Day 6 — M6 Gate B：Practical、project、oral 与 report（110 分钟）

- **45 分钟**：执行考官从冻结池抽取的 unseen practical，计 **30 分**；保存 command、
  raw output、diagnostic、cleanup 与 next-valid-run。
- **30 分钟**：从 clean process 复现 cumulative direct-device project，计 **30 分**；
  验证 version/engine、correctness、copy 与 stream/event evidence。
- **20 分钟**：完成 oral defense，计 **15 分**；现场解释 object/buffer lifetime、
  fallback、冲突 artifact 仲裁与一个 residual risk。
- **15 分钟**：汇总两日 **Knowledge 25 / Unseen practical 30 / Cumulative project 30 /
  Oral defense 15**，写 go/remediate decision 与复测条件。

**必须留下**：`m6-scorecard.md`、`notes/gates/gate-6.md`、unseen/clean transcripts、
oral prompts/notes 和 gate decision。

## M6 blockers、Exit ticket 与 unlocks

**通过条件：总分 ≥80/100，且以下 blockers 全部通过。高分不能抵消 blocker。**

- [ ] JetPack/TensorRT/version/archive/engine identity 可复现。
- [ ] ORT CUDA actual provider/partition 可解释，无 silent fallback，device I/O contract 明确。
- [ ] CPU/framework 与 TensorRT final output 在冻结 tolerance 内。
- [ ] 无未解释 host round-trip；input/output readiness 有 stream/event 证据。
- [ ] fixed 与声明支持的 precision/profile cases 无 buffer/lifetime failure。
- [ ] multi-agent artifact owner、handoff/recovery/retirement 可审计。

Gate administration：Knowledge 首次 ≥80%、订正后 100%；mandatory correctness/security
tests 100%；report 记录 date、commit、environment、variant ID、四项得分与 remediation。

Exit ticket：口述 engine/context/device pointer/stream 的完整 lifetime，并解释一个被
拒绝的冲突 artifact。**Unlock Unit 7 / W37**：M6 pass；否则只进入 remediation。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [NVIDIA Technical Blog · Jetson](https://developer.nvidia.com/blog/tag/jetson/)
  只选 1 篇涉及 edge inference integration、device memory 或 deployment reliability
  的文章；与实机 JetPack/L4T 不匹配的 claim 只能标为 out-of-scope。
- **Artifact**：写 `notes/feed/week-36.md`，固定四栏
  `claim → evidence → scope → decision`；不得作为 M6 submission 或补分材料。
- **取消规则**：Gate submission/blocker audit 落后或未冻结时直接取消，不补做。
