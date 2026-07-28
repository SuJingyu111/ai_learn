# 一年执行版：48 个内容周 + 4 个补修周

正式课程定义在 [`CURRICULUM.md`](CURRICULUM.md)，每天执行
[`weeks/`](weeks/)。本文件只解决全年节奏、休息和 Gate 后如何调整，不承担每日映射。

## 推荐 calendar

| Calendar | Content | Unit | Major outcome |
|---:|---:|---|---|
| 1–6 | W1–W6 | U1 Tensor / inference math | M1 + G1 |
| 7–12 | W7–W12 | U2 ONNX / ORT CPU | M2 + G2 |
| 13 | — | Buffer A | G1/G2 remediation、休息、环境偿债 |
| 14–19 | W13–W18 | U3 CUDA foundations | M3 + G3 |
| 20–25 | W19–W24 | U4 GPU performance / preprocess | M4 + G4 |
| 26 | — | Buffer B | G3/G4 remediation、Orin 工具整理 |
| 27–32 | W25–W30 | U5 async pipeline | M5 + G5 |
| 33–38 | W31–W36 | U6 ORT CUDA / TensorRT | M6 + G6 |
| 39 | — | Buffer C | G5/G6 remediation、版本/设备重建 |
| 40–45 | W37–W42 | U7 Orin systems engineering | M7 + G7 |
| 46–51 | W43–W48 | U8 capstone studio | M8 + G8 |
| 52 | — | Buffer D | final retest、release、恢复周 |

若入学诊断需要 Course 0，可将全年扩成 53–54 周，或把前两个 Unit 的 buffer 合并。
Calendar 不应迫使你带着不合格的 prerequisite 进入下一 Unit。

## 每周 9–12 小时怎么分

| 活动 | 时间 |
|---|---:|
| 正式理论课与术语 | 2–2.5 h |
| Worked examples / problem set | 1–1.25 h |
| Guided + independent lab | 3–3.5 h |
| 累计项目 | 1–1.5 h |
| Agent/MCP/GH-600 sidecar | 1.5–2 h |
| retrieval / oral / review | 0.5–0.75 h |

每周 6 天、每天 90–120 分钟。Day 7 完全休息；optional podcast 可以散步时听，但不
制造必须交付。

## Buffer week 的用途

优先级：

1. 处理对应 Gate 的 `CONDITIONAL/RETAKE`；
2. clean reproduction、设备/依赖、测试和 raw evidence 债务；
3. 总结 concept notes 和 oral defense；
4. 休息；
5. 只有前四项完成后做 elective。

Buffer week 不引入新主题，不用于“提前赶下一单元”。

## 8 个月检查点

按上表 Calendar 32 左右应完成 M5/G5，即：

- tensor/graph/CUDA/Resize/async pipeline 的 dependency chain 已通过；
- 第一份 CUDA preprocessor case study 完成；
- 能用 timeline 解释 multi-frame ownership/lifetime；
- Runtime Lab Assistant 已有 MCP server、security、state/eval 基础。

此时可以探索偏 CUDA/edge/runtime 的岗位，但不应声称已掌握 ORT CUDA/TensorRT/Orin
端到端；它们是后四个内容单元。

## 一年终点

- G1–G8 全部 PASS；
- Orin device-resident pipeline + evidence bundle；
- Runtime Lab Assistant v1；
- 三份 case study；
- clean reproduction 和 10 分钟 defense；
- GH-600 六域 artifact + 独立 readiness decision。

## 忙碌周与中断

- 只有 4–6 小时：保留 prerequisite theory、correctness lab、Exit Ticket；先取消 seminar。
- 中断 1–2 周：回来先做上一周 Exit Ticket 的新题，不直接继续。
- 中断超过 3 周：重跑最近 milestone 的 clean test，再做 30 分钟 dependency quiz。
- Gate 未过：进入 remediation，不靠降低 tolerance、删 tests 或跳过 evidence 追进度。
