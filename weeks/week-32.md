# Week 32 — Unit 6：ORT I/O Binding、User Compute Stream 与 Controlled Tuning

## Unit / 课程角色

- **Unit 6 · Runtime Integration，第二周。** 消除不必要host/device copies，把ORT execution接入caller-managed CUDA stream，同时保持ownership、fallback与correctness可见。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：基于Week 29 frozen dataset、Week 30 taxonomy、Week 31 traces，只做一次single-variable controlled tuning。

## 硬先修与 Remediation

- **硬先修**：Week 31 CPU/CUDA provider evidence通过；能创建/持有device input/output与CUDA stream/event；eval baseline/bundle hashes冻结。
- **开周诊断（15 分钟）**：画普通ORT `Run` 从CPU input到CUDA EP再到CPU output的潜在copies；再画device-bound input/output。
- **未通过时**：先在CPU/CUDA EP普通run上证明correctness/provider assignment；再做仅output binding，不同时引入I/O binding与user stream。

## Observable Objectives

1. 解释I/O Binding解决dynamic output allocation、device placement与implicit copy的方式及限制。
2. 对普通Run、device I/O Binding、I/O Binding+user stream做single-variable分阶段比较。
3. 正确处理OrtValue/device buffer/allocator/session/stream/events的ownership与lifetime。
4. 用profile/timeline证明copy变化、provider assignment与output readiness；不把更少API calls当成性能证据。
5. 在固定20-case dataset/rubric/environment上只改一个instruction或tool description，生成before/after bundles并决定keep/rollback。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| I/O Binding | 显式把model input/output绑定到指定device memory或device allocation policy |
| OrtValue | ORT持有或引用tensor/data的runtime value，lifetime取决于创建/绑定方式 |
| device tensor | storage位于CUDA device、shape/dtype/device metadata明确的tensor |
| user compute stream | caller提供给CUDA EP用于计算/相关操作的stream配置 |
| stream ownership | 创建、使用、同步与销毁stream的责任与session lifetime约束 |
| implicit copy | runtime因CPU input/output或provider boundary自动插入的数据移动 |
| controlled tuning | dataset/rubric/environment固定、仅一个可归因revision的before/after |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [ONNX Runtime — I/O Binding](https://onnxruntime.ai/docs/performance/tune-performance/iobinding.html)，读 device tensor placement、bind input/output、unknown output shape | 25 |
| 1 | [ONNX Runtime CUDA EP — `user_compute_stream`](https://onnxruntime.ai/docs/execution-providers/CUDA-ExecutionProvider.html#user_compute_stream)，读 option、lifetime、external allocator与`do_copy_in_default_stream`相关说明 | 25 |
| 2 | [ONNX Runtime C++ API — IoBinding](https://onnxruntime.ai/docs/api/c/struct_ort_1_1_io_binding.html)，读 BindInput、BindOutput、GetOutputValues、ClearBoundInputs/Outputs | 20 |
| 2 | [NIST/SEMATECH e-Handbook — 5.3.3 How do you select an experimental design?](https://www.itl.nist.gov/div898/handbook/pri/section3/pri33.htm)，读 experimental objective、number of factors 与 comparative objective；把本周单变量before/after定义为受控比较，不声称已证明完整因果 | 20 |

**GH-600 blueprint mapping（0 额外分钟，不作教材）**：[GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) 的 error-analysis→instruction/workflow/memory/tool-access tuning 与 before/after 仅用于认证考纲映射；controlled-comparison设计由 NIST/SEMATECH 与本周 frozen-dataset experiment 教授。

## 带问题阅读

- CPU input/output为什么会让CUDA EP之外仍出现copies？
- known/unknown output shape应选择预分配OrtValue还是device-only MemoryInfo binding？
- user stream创建/销毁顺序如何与session和bound buffers配合？
- provider fallback仍存在时，device binding是否必然消除所有copies？
- controlled tuning中哪些任何变化都会使comparison invalid？
- NIST 的 comparative objective为何要求先声明主要factor与response？本周固定dataset/rubric/runner/environment仍不能排除哪些替代解释？

## Worked Example / 手算

- 普通Run：CPU input 4 MiB、CPU output 1 MiB；画潜在H2D/D2H。device-bound后按同一useful bytes解释预期减少，不假设实际为零。
- 画 `create stream→create session(user stream)→allocate/bind input/output→enqueue producer→event dependency→ORT Run→event→consumer→clear/destroy session→destroy stream` lifetime。
- eval baseline 20 cases中tool-schema failures 6；单改description后5修复、1保持、2 safety regressions：计算net变化并说明为何safety regression可触发rollback。

## 累计项目 Increment

- `labs/ort_runtime/io_binding/`：three stages、device buffers、user stream、correctness/timeline/lifetime tests。
- `ai-app/evals/tuning/week-32/`：baseline/revision manifests、two bundles、per-case diff、decision。
- `notes/curriculum/week-32.md`：copy map、ownership graph、eight-month checkpoint。

## 本周必须产出

- ordinary/I/O-bound/user-stream三阶段correctness、copy/timing/profile对照、lifetime/error evidence。
- one-change tuning proposal、valid before/after bundles、per-case/category regressions与keep/rollback。
- Unit 6阶段Exit ticket与八个月readiness review。

## Day 1 — Theory：I/O Binding 与 User-stream Lifetime（105 分钟）

- **25 分钟｜必修理论**：阅读I/O Binding device placement与known/unknown output shape。
- **25 分钟｜必修理论**：阅读CUDA EP user_compute_stream选项/lifetime/allocator/copy-stream说明。
- **30 分钟｜worked example**：完成ordinary/device-bound copy map与stream/session/buffer lifetime graph。
- **15 分钟｜experiment design**：冻结three stages、model/input/provider/options/correctness/timing。
- **10 分钟｜retrieval**：闭卷解释binding、OrtValue、user stream、implicit copy。
- **必须留下**：copy map、lifetime graph、three-stage contract。

## Day 2 — Theory + Guided：Output Binding 与 Tuning Freeze（105 分钟）

- **20 分钟｜必修理论**：阅读C++ IoBinding methods；写bind/clear/get-output contract。
- **20 分钟｜必修理论**：阅读 NIST experimental-design selection与comparative objective；写primary factor、response、controls和不可作出的因果声明。
- **40 分钟｜guided lab**：先做一个变量：普通CUDA EP→output I/O Binding；同input比较correctness与profile，处理known/unknown output shape。
- **15 分钟｜tuning guided**：冻结dataset/rubric/runner/environment/baseline bundle hashes，选择一个高频failure与单一revision。
- **10 分钟｜检查**：写success、safety regression、rollback criteria。
- **必须留下**：output-binding evidence、immutable eval baseline、one-change proposal。

## Day 3 — Guided → Independent：Device I/O 与 Before/After（105 分钟）

- **55 分钟｜Core lab**：加入device input/output OrtValues；明确allocator/ownership，运行CPU reference比较与profile，证明哪些copies消失/保留。
- **35 分钟｜Sidecar build**：用相同20 cases/rubric/environment跑baseline/revision，生成独立bundles与per-case/category diff。
- **15 分钟｜验证**：manifest diff除目标revision外有变化即comparison invalid并重跑。
- **必须留下**：device I/O correctness/profile、two bundles、comparison-validity report。

## Day 4 — Independent Lab：User Stream、Dependencies、Failures（105 分钟）

- **65 分钟｜Core independent**：配置caller-owned stream；用events连接producer→ORT→consumer，测试wrong/missing dependency、early free、session/stream teardown order；故障隔离且正常path clean。
- **25 分钟｜Sidecar independent**：审查improvements、unchanged、regressions，特别标safety/permission cases；不改dataset/rubric补分。
- **15 分钟｜evidence**：保存provider options、stream identity、timeline、error/recovery与tuning limitations。
- **必须留下**：user-stream runner/timeline、failure tests、per-category tuning analysis。

## Day 5 — Integration：Runtime Pipeline × Tuning Decision（105 分钟）

- **60 分钟｜Core integration**：公平比较ordinary/I/O-bound/user-stream：CPU correctness、provider assignment、copies、setup/first/warm/kernel/end-to-end；结论绑定model/device/version。
- **30 分钟｜Sidecar integration**：按criteria决定keep/rollback/needs-more-data；保存decision rationale与下一轮唯一hypothesis，不执行第二轮。
- **15 分钟｜cross-check**：检查runtime与agent两组before/after都保持单变量、raw evidence与reproducibility。
- **必须留下**：three-stage report、evidence-backed tuning decision、cross-check。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：给unseen input/output placement与provider graph，设计binding/stream/lifetime并诊断一条hidden copy；运行一个unseen shape。
- **35 分钟｜Sidecar assessment**：审阅before/after validity、safety regression与rollback；口述taxonomy→trace→revision→same-dataset decision链。
- **25 分钟｜Exit ticket / checkpoint**：总结Unit 3–6新增能力、仍blocked的设备/runtime条件、三份最强evidence与下一Unit prerequisites。
- **必须留下**：placement assessment、unseen run、tuning defense与readiness note。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M5 为 `PASS`、three-stage runtime evidence 与 frozen eval baseline 无 backlog 时做；若 Gate 落后、comparison invalid 或出现 safety regression，直接取消，不启动第二轮 tuning。
- **来源与选择规则**：读 Eugene Yan 的 [“Product Evals in Three Simple Steps”](https://eugeneyan.com/writing/product-evals/)；只选一个关于 labeled data、evaluator alignment 或每次 config change复用同一 harness 的 claim，并用 NIST comparative-objective原则检查其适用边界。
- **30–45 分钟流程**：5 分钟选 claim → 15–20 分钟读案例并对照 frozen manifests → 10–15 分钟写 `claim → evidence → scope → decision`；个人经验性建议不得升级为通用因果结论。
- **artifact**：`notes/feed/week-32.md`，含 baseline/revision hashes、一个 keep/rollback/needs-more-data 决策。

## Exit Ticket / Unlocks

- **通过条件**：three-stage correctness；copy变化有profile证据；user stream/lifetime无已知错误；tuning仅一变量且dataset/rubric/environment固定，safety regression有rollback。
- **解锁**：后续ORT/TensorRT集成；任何provider fallback、copy或lifetime不明项必须进入明确remediation backlog。
