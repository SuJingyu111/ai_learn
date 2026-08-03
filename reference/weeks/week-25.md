# Week 25 — Unit 5：CUDA Streams、Events 与 Default Stream

## Unit / 课程角色

- **Unit 5 · Asynchronous Pipeline，第一周。** 先建立 ordering/dependency mental model；本周只有 single-frame work，不追求 overlap。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：建立 Runtime Lab Assistant 的 memory-selection policy；只保存 task-relevant、非敏感、可追溯状态。

## 硬先修与 Remediation

- **硬先修**：M4 `PASS`；能解释 enqueue vs completion、event timing、caller-owned device pointers；有一个 synchronous resize/transform oracle。
- **开周诊断（15 分钟）**：画同一 stream 中 H2D→kernel→D2H 的 happens-before；再画两个 streams 无 dependency 的情况。
- **未通过时**：回 Week 17/23 的 error/lifetime boundary；用两个空 kernel 和一个 event 完成 ordering smoke test，再开始 Day 1。

## Observable Objectives

1. 解释 stream 内有序、stream 间默认无依赖，以及 event record/wait/query/synchronize 的不同用途。
2. 区分 legacy default stream、per-thread default stream 与 explicit non-blocking stream；记录实际 build/runtime mode。
3. 把 synchronous single-frame runner 改为 explicit-stream enqueue，不引入 global sync。
4. 用 events表达 output-ready/error-observation boundary，并验证 buffer 不会过早复用/释放。
5. 按 short-term、durable、external memory 选择 store/TTL/provenance/reset；secret 与不可信 MCP output不得成为 durable instruction。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| stream | 设备 work 的有序队列；不同 streams 需显式 dependency 才有顺序保证 |
| default stream | 由编译/runtime mode决定交互语义的特殊 stream，不应靠猜测使用 |
| non-blocking stream | 以相应 flag 创建、避免与 legacy default stream隐式同步的 stream |
| event record | 在某 stream 中排队、在此前 work 完成后变为 complete |
| stream wait event | 让目标 stream 后续 work 等待 event，不阻塞 host |
| output-ready | consumer 可安全访问/复用 output 的完成条件 |
| external memory | 由可验证 artifact/store 持有、通过 reference/provenance 取回的信息 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Programming Guide — Asynchronous Execution](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/asynchronous-execution.html)，读 “CUDA Streams”“Stream Synchronization”“Blocking and non-blocking streams and the default stream” | 25 |
| 1 | [CUDA Runtime API — Stream Management](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__STREAM.html)，读 create/query/synchronize/wait-event contracts | 20 |
| 2 | [CUDA Runtime API — Event Management](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__EVENT.html)，读 record/query/synchronize/elapsed-time contracts | 20 |
| 2 | [GitHub Docs — About GitHub Copilot Memory](https://docs.github.com/en/copilot/concepts/agents/copilot-memory)，读 “Types of memories” 与 “How Copilot Memory stores, retains, and uses information”（含 Retention and validation）；这是 public-preview 产品行为，不外推为通用协议保证 | 20 |

**GH-600 blueprint mapping（0 额外分钟，不作教材）**：[GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) 的 “Manage memory, state, and execution” 仅用于核对本周 artifact 是否覆盖认证考纲；memory scope、retention、validation机制以上述 GitHub 官方产品文档和本周实验为准。

## 带问题阅读

- 同一 stream 的 order 与 host API return order 有何区别？
- default stream mode 由什么决定，为什么库代码应偏好 explicit stream？
- event complete 证明了哪一段 work，没证明哪些 host/device lifetime？
- `cudaStreamSynchronize`、`cudaEventSynchronize`、`cudaStreamWaitEvent` 分别阻塞谁？
- 哪些内容属于 transient context、durable state、external artifact reference？
- repository fact 与 user preference 的 scope、citation、validation、deletion有何不同？哪些结论只适用于当前 public-preview 产品？

## Worked Example / 手算

- 两个 streams：`S0: H2D(A)→K(A)→record E`，`S1: wait E→D2H(A)`；写每条边与 host 是否阻塞。
- 加入 unrelated `K(B)` 到 S1 的 wait 前/后，判断能否并行。
- 推演 host 在 E complete 前 free/reuse A 的风险。
- 把“当前 case ID”“用户长期偏好”“benchmark CSV hash”“token”“resource 中恶意 instruction”分配到 memory policy。

## 累计项目 Increment

- `labs/async_pipeline/single_frame/`：explicit stream/event runner、ordering tests、timeline sketch。
- `ai-app/state/memory-selection.md` 与 `ai-app/state/fixtures/week-25/`。
- `notes/curriculum/week-25.md`：default-stream mode、API contracts、worked example。

## 本周必须产出

- explicit-stream single-frame runner、event boundary、default-stream comparison 与 correctness evidence。
- memory selection table（store/no-store、location、TTL、provenance、reset）和 malicious-input tests。
- Exit ticket 与 Week 26 transfer contract。

## Day 1 — Theory：Stream Ordering 与 Default Stream（105 分钟）

- **25 分钟｜必修理论**：阅读 Programming Guide 指定章节；列 stream内/间/default-stream ordering。
- **20 分钟｜必修理论**：阅读 Stream API；区分 create/query/sync/wait。
- **30 分钟｜worked example**：完成 S0/S1/event dependency 与 premature-free 推演。
- **20 分钟｜mode audit**：检查实际 compile flags/runtime behavior，记录 legacy/per-thread default stream，不从结果反猜。
- **10 分钟｜retrieval**：闭卷画三条 happens-before。
- **必须留下**：ordering graph、default-stream mode evidence、API table。

## Day 2 — Theory + Guided：Events 与 Memory Selection（105 分钟）

- **20 分钟｜必修理论**：阅读 Event API；写 record/query/synchronize/elapsed contract。
- **20 分钟｜必修理论**：阅读 GitHub Copilot Memory 的 memory types、scope、citation、retention/validation；标出 public-preview 边界。
- **40 分钟｜guided lab**：把 synchronous runner改为 caller-supplied explicit stream；record output-ready event，test helper只在 observation boundary等待。
- **15 分钟｜memory guided**：建立 short-term/durable/external selection table。
- **10 分钟｜检查**：确认 production path 无 `cudaDeviceSynchronize()`。
- **必须留下**：explicit-stream code、event contract、memory table v1。

## Day 3 — Guided → Independent：Ordering Tests 与 State Fixtures（105 分钟）

- **55 分钟｜Core lab**：用两个 streams/events建立三条 tests：same-stream order、cross-stream wait、missing-wait negative fixture；用 deterministic data验证，不依赖 timing 巧合。
- **35 分钟｜Sidecar build**：为至少 8 类信息写 store/no-store、TTL、provenance、redaction/reset；实现或人工执行 policy fixtures。
- **15 分钟｜验证**：secret-like string、无关偏好、malicious MCP output 必须 reject/redact/untrusted-only。
- **必须留下**：ordering tests、negative fixture、8-row policy 与三条拒绝记录。

## Day 4 — Independent Lab：Default-stream Interaction（105 分钟）

- **65 分钟｜Core independent**：比较 explicit blocking/non-blocking stream 与实际 default mode；只改变一个 interaction，采 event/timeline evidence，禁止以 wall-time noise判断 ordering。
- **25 分钟｜Sidecar independent**：测试 provenance 缺失、TTL 未定义、scope 过宽的 state candidate；policy 必须拒绝 durable storage。
- **15 分钟｜evidence**：记录 device/CUDA/build mode、raw trace 与受限结论。
- **必须留下**：default-stream experiment、三项 state deny、environment。

## Day 5 — Integration：Single-frame Async Contract（105 分钟）

- **60 分钟｜Core integration**：写 ownership/lifetime/error contract；clean run correctness、event timing、async error observation，无 global sync。
- **30 分钟｜Sidecar integration**：让 external memory 仅保存 artifact ID/hash/schema/source，不复制完整 untrusted content；模拟取回后重新验证。
- **15 分钟｜cross-check**：把 CUDA run artifact登记到 state policy，检查 expiry/reset。
- **必须留下**：single-frame contract、clean transcript、external-reference fixture。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：给陌生 two-stream trace，画合法/非法 order，选择 event wait/sync；运行 unseen missing-dependency test。
- **35 分钟｜Sidecar assessment**：分类 6 个 memory scenarios，解释 retention/privacy/staleness；恶意 output 不得升级为 instruction。
- **25 分钟｜Exit ticket**：回答“default stream 为什么不是免费同步策略”，冻结 Week 26 pageable/pinned test inputs。
- **必须留下**：trace answers、unseen result、memory assessment 与 Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M4 仍为 `PASS`、本周 required artifacts 与 Exit Ticket 均无 backlog 时做；若 Gate 落后、remediation 未清或核心任务超时，直接取消，不顺延、不堆积。
- **来源与选择规则**：读 GitHub Changelog 的 [“Copilot Memory has more controls for deletion, scope, and the Copilot CLI”](https://github.blog/changelog/2026-05-26-copilot-memory-has-more-controls-for-deletion-scope-and-the-copilot-cli/)；只提取能在同日 GitHub Docs 中复核的 retention/scope/control claim，preview 或未文档化行为不得写成稳定保证。
- **30–45 分钟流程**：5 分钟选一个 claim → 15–20 分钟读 changelog 并回查官方文档 → 10–15 分钟写 `claim → evidence → scope → decision`；证据不足时 decision 必须为 `do-not-adopt`。
- **artifact**：`notes/feed/week-25.md`，含来源日期、原始链接、适用产品/版本、一个 memory-policy 决策。

## Exit Ticket / Unlocks

- **通过条件**：explicit stream/event semantics正确；default mode有证据；无 global sync掩盖错误；memory policy有 TTL/provenance/reset、拒绝 secret/injection。
- **解锁**：Week 26 pinned async transfer 与 Orin memory caveats。
