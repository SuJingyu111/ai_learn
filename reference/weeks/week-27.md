# Week 27 — Unit 5：Ownership 与 Frame-slot State Machine

## Unit / 课程角色

- **Unit 5 · Asynchronous Pipeline，第三周。** 在增加第二个 buffer 前，先用显式状态机证明每个 host/device buffer 的唯一 owner、可复用条件与错误清理。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：为 Week 25–26 memory/state增加 TTL、pruning、reset 与 audit。

## 硬先修与 Remediation

- **硬先修**：Week 26 transfer/lifetime Exit ticket全过；能用 event表达 H2D/kernel/D2H completion；durable state有合法 transition。
- **开周诊断（15 分钟）**：为一个 frame列 host input、device input/output、host output在 fill/H2D/kernel/D2H/consume时的 owner。
- **未通过时**：回 Week 25 event graph与 Week 26 buffer reuse timeline；先用一帧、一个 slot完成 ownership table，不写 loop。

## Observable Objectives

1. 定义 `free→filling→H2D→compute→D2H→ready→consuming→free` 与 error/draining transitions。
2. 为每个 transition指定 actor、precondition、event、buffer owner与illegal transition。
3. 实现 single-slot multi-frame runner，保持 frame ID/order/correctness，安全处理 0/1/odd frames。
4. 注入 early stop/operator failure，停止新提交并清理已提交 work。
5. 为 memory/state定义 TTL、last-validated、prune dry-run、reset与audit；过期 state不得参与新决策。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| owner | 当前唯一可读写/释放某资源的 actor或stage |
| frame slot | 一组完整 host/device buffers、stream/events与状态 |
| transition guard | 允许状态变化前必须满足的条件 |
| illegal transition | 可能导致复用、乱序、double free或隐藏错误的状态变化 |
| draining | 停止提交新 work，但等待/回收已提交 work |
| TTL | state 在必须重新验证或丢弃前的最长有效期 |
| prune dry-run | 先列出将删除/失效对象，不立即改变持久状态 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Runtime API — Event Management](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__EVENT.html)，复读 event lifecycle、query/synchronize、destroy semantics | 20 |
| 1 | [CUDA Runtime API — Stream Management](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__STREAM.html)，复读 stream query/synchronize/wait-event 与 destruction | 20 |
| 2 | [C++ Core Guidelines — Resource Management](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource)，读 RAII、ownership、lifetime相关规则 | 20 |
| 2 | [MCP Specification 2025-11-25 — Tasks](https://modelcontextprotocol.io/specification/2025-11-25/basic/utilities/tasks)，读 “Task Status Lifecycle”“TTL and Resource Management”“Task Isolation and Access Control”“Audit and Logging”；Tasks 在此版本仍为 experimental | 20 |

**GH-600 blueprint mapping（0 额外分钟，不作教材）**：[GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) 的 expiration、pruning、reset、stale-context 目标仅用于考纲覆盖映射；状态转移、TTL、隔离与审计规则由 MCP 规范和本周实现验证。

## 带问题阅读

- event object destroy 与 event-recorded work completion是什么关系？API contract必须查证什么？
- stream destroy/teardown前如何确保 pending work与buffers安全？
- RAII解决释放路径，为什么不能自动解决GPU异步ownership？
- error后是立即free、device-wide sync还是按slot drain？各自 trade-off是什么？
- TTL到期时应该删除、重新验证、隔离还是请求 human review？
- MCP task 哪些状态是 terminal、TTL 到期允许谁删除什么、缺少 authorization context 时 task ID 与 list 能力应如何收紧？

## Worked Example / 手算

- 对两 frames、一个 slot逐步列 state、owner、event complete条件；计算何时 frame 1 可复用 host input。
- 注入 frame 0 kernel error，推演 stop-submit→observe→drain→error→reset。
- 对 task summary TTL 1h、artifact metadata TTL 24h、human decision无自动TTL但source hash约束，推演 25h 后 resume。

## 累计项目 Increment

- `labs/async_pipeline/frame_slot/`：state enum、transition function、single-slot runner、failure tests。
- `ai-app/state/lifecycle-policy.md` 与 prune/reset fixtures。
- `notes/curriculum/week-27.md`：ownership table、state diagram、illegal transitions。

## 本周必须产出

- frame-slot state machine、owner/invariant表、single-slot multi-frame runner、shutdown/error evidence。
- TTL/prune/reset policy、dry-run与 stale-state tests。
- Exit ticket 与 Week 28 two-slot design。

## Day 1 — Theory：Event/Stream Lifetime 与 Slot Model（105 分钟）

- **20 分钟｜必修理论**：复读 Event API，写 create→record→complete/query→reuse/destroy contract。
- **20 分钟｜必修理论**：复读 Stream API，写 pending work与teardown boundary。
- **30 分钟｜worked example**：完成两frames/一slot ownership timeline与kernel-error drain。
- **25 分钟｜state design**：定义 states、legal transitions、guards、owner、events。
- **10 分钟｜retrieval**：闭卷指出三条 illegal transitions。
- **必须留下**：state diagram、ownership/invariant表、error path。

## Day 2 — Theory + Guided：RAII 与 TTL Policy（105 分钟）

- **20 分钟｜必修理论**：阅读 C++ resource-management rules，标哪些可用于host wrappers。
- **20 分钟｜必修理论**：阅读 MCP Tasks 的状态生命周期、TTL/resource management、task isolation与audit；把 experimental 状态写入scope。
- **40 分钟｜guided lab**：实现 slot资源 RAII与transition checker；非法 transition返回明确错误，不执行GPU work。
- **15 分钟｜state guided**：为 short/durable/external三类状态定义 TTL/last-validated/prune/reset。
- **10 分钟｜检查**：RAII destructor前明确slot必须处于何状态。
- **必须留下**：RAII/transition code、lifecycle policy v1。

## Day 3 — Guided → Independent：Single-slot Runner 与 Prune Dry-run（105 分钟）

- **55 分钟｜Core lab**：实现 0/1/2/odd frames single-slot runner，保存frame IDs并逐帧与synchronous oracle比较。
- **35 分钟｜Sidecar build**：实现prune dry-run：列id/reason/age/source，不直接删；apply需要明确target并写audit。
- **15 分钟｜验证**：expired state在validation前不得影响decision；fresh external artifact需验证hash。
- **必须留下**：runner/per-frame correctness、dry-run/apply audit、stale reject。

## Day 4 — Independent Lab：Shutdown/Error 与 Reset（105 分钟）

- **60 分钟｜Core independent**：测试 empty、early stop、operator/copy failure；停止新提交，observe/drain已提交work，恢复或销毁不产生double-use。
- **30 分钟｜Sidecar independent**：测试TTL expiry、source hash改变、explicit reset、unknown state；分别revalidate/invalidate/reset/reject。
- **15 分钟｜evidence**：用transition log证明没有illegal reuse，记录无法由日志证明的项。
- **必须留下**：shutdown/failure traces、四类 lifecycle result、residual risk。

## Day 5 — Integration：State Machine as Evidence（105 分钟）

- **60 分钟｜Core integration**：clean run正常/early-stop/failure；把frame/state/event/owner写结构化trace，确认trace与output IDs关联。
- **30 分钟｜Sidecar integration**：durable review state引用最新trace artifact；TTL/hash不满足时自动标needs-review，不自动重跑。
- **15 分钟｜cross-check**：比较GPU slot state与agent durable state，写相似点/不可类比点。
- **必须留下**：structured transition trace、state invalidation、cross-track note。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：给一组乱序 transitions找出first illegal step并修复；运行 unseen odd-frame/early-stop case。
- **35 分钟｜Sidecar assessment**：对六个state candidates决定retain/revalidate/prune/reset/reject并解释audit。
- **25 分钟｜Exit ticket**：画两个slots/三frames预期状态，不实现；列 Week 28 no-share invariants。
- **必须留下**：transition assessment、unseen result、TTL decisions 与 two-slot design。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在前序 Gate 为 `PASS`、本周 ownership 与 lifecycle artifacts 全部完成时做；若 Gate 落后、illegal-transition remediation 未清或 Exit Ticket 未过，直接取消，不累计。
- **来源与选择规则**：读 MCP 官方博客 [“One Year of MCP: November 2025 Spec Release”](https://blog.modelcontextprotocol.io/posts/2025-11-25-first-mcp-anniversary/) 中 “Support for Task-based Workflows”；只选一个能回链到 2025-11-25 Tasks 规范的 lifecycle/TTL/security claim，博客展望不得当作规范要求。
- **30–45 分钟流程**：5 分钟选 claim → 15–20 分钟对照博客与规范中的 MUST/SHOULD/MAY → 10–15 分钟写 `claim → evidence → scope → decision`。
- **artifact**：`notes/feed/week-27.md`，含规范版本、experimental 标记、一个 server lifecycle 决策。

## Exit Ticket / Unlocks

- **通过条件**：owner唯一、transition可验证、error能drain、single-slot逐帧正确；TTL/prune/reset有审计且stale state不参与决策。
- **解锁**：Week 28 double-buffer pipeline。
