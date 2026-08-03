# Core Module 10 — CPU Resize 语义与 Correctness Oracle

## 本周目标

把现有 CPU nearest/bilinear 从 starter code 提升为可信 oracle，锁定 coordinate、
border、layout、channel、stride、invalid input 和 numerical tolerance。

## 每日计划

### Day 1 — API/Coordinate Contract 审阅

- **10 分钟**：阅读 `image.h`、`resize.h`、CPU implementation，不修改代码。
- **20 分钟**：手算 half-pixel mapping 的 `2→1`、`2→4`、identity coordinates。
- **50 分钟**：创建 `docs/resize-design.md`，写 dtype/layout/stride unit/channel、
  mapping/nearest rounding/bilinear/border/invalid input。
- **10–30 分钟**：检查现有代码与每条 contract 是否一致，列出 ambiguity/bug candidates。
- **必须留下**：坐标表、contract 草稿和审阅问题列表。

### Day 2 — Known-value Tests

- **10 分钟**：选择能够分别暴露 rounding、weight、border 错误的 expected values。
- **15 分钟**：确认 expected 手算独立于被测实现。
- **55 分钟**：扩充 `tests/test_resize.cpp`：identity、`2×2→1×1`、`2×2→4×4`
  关键像素、nearest known values。
- **10–30 分钟**：运行 CPU tests；故意改变一个 expected 确认 test 会失败后恢复。
- **必须留下**：tests、手算依据和 exact test command。

### Day 3 — One-pixel 与 Odd Sizes

- **10 分钟**：预测 `1×N`、`N×1`、`1×1` bilinear 的 border 行为。
- **15 分钟**：列 case matrix：one-pixel、odd upscale/downscale、non-uniform scale。
- **55 分钟**：增加 cases；constant image 必须保持 constant，identity 保持 input。
- **10–30 分钟**：用 sanitizer/ASan（host 可用时）或 guard data 检查 boundaries。
- **必须留下**：case matrix、pass/fail 和任何语义修正。

### Day 4 — Channels 与 Padded Stride

- **10 分钟**：画 packed 与 padded row 的 memory layout，定义 padding sentinel。
- **15 分钟**：选择 1/3/4 channels 和 source/destination 不同 padding。
- **55 分钟**：增加 view helper 支持 explicit stride；验证 output active region，
  padding sentinel 不被改写。
- **10–30 分钟**：测试 multi-channel 各 channel 不同 pattern，避免 constant 掩盖错位。
- **必须留下**：padded stride tests、sentinel assertion 和 layout 图。

### Day 5 — Invalid Inputs 与 API Decision

- **10 分钟**：列 null、zero/negative dims、channel mismatch、short/negative stride、
  alias/overflow risk。
- **20 分钟**：决定每项 throw/error/unsupported，更新 design doc。
- **50 分钟**：为当前 contract 增加 tests；修复明确的 CPU validation 缺口。
- **10–30 分钟**：检查异常不会部分写 destination；记录尚未处理的 huge-dimension
  overflow/alias 限制。
- **必须留下**：invalid table、tests 和 known limitations。

### Day 6 — Randomized Oracle Gate

- **10 分钟**：选择 fixed seed、尺寸范围、channels、padding 范围。
- **20 分钟**：定义 randomized failure output：seed/case/pixel/channel/expected/actual。
- **50 分钟**：实现至少数十个 cases；将 known hand cases 和 random tests 分开。
- **10–30 分钟**：clean build/test，完成 Week 10 weekly review 和 design doc。
- **必须留下**：seeded test、full reproduction、CPU oracle Gate evidence。

## 需要明确的 contract

- float、interleaved HWC、channels 范围。
- stride 单位是 elements 还是 bytes。
- source/destination 是否允许 alias。
- pixel-center mapping 与 nearest rounding。
- border clamp 和 invalid input 行为。
- CPU exception 与 CUDA error 的对应关系。

## 交付

- 扩充后的 CPU tests。
- `docs/resize-design.md`
- reference cases 与 tolerance rationale。

## 完成标准

- [ ] API 语义无 TBD。
- [ ] padded stride 和 one-pixel cases 通过。
- [ ] randomized test 可由 seed 复现。
- [ ] benchmark 运行前先通过 correctness gate。
