# Core Module 13 — Affine Warp 设计与 CPU Reference

## 本周目标

把隐含的图像几何知识写成严格 API contract，并完成 inverse-mapping affine warp
的 CPU correctness oracle。Perspective warp 暂时是 optional。

## 每日计划

### Day 1 — 冻结 Warp API Contract

- **10 分钟**：列出 matrix direction、storage order、coordinate、border、dtype、
  layout、stride、ownership 的所有待决项。
- **25 分钟**：阅读现有 `docs/warp-design.md` 和 resize contract。
- **45 分钟**：完成 API 草案：调用者传 source→destination 还是 inverse matrix；
  singular matrix、alias、invalid metadata 如何处理。
- **10–30 分钟**：写两个伪调用样例并让 contract 能唯一解释结果。
- **必须留下**：无歧义的 design v1 和仍需实验决定的项目。

### Day 2 — Hand Cases 与 Matrix Convention

- **10 分钟**：手写 homogeneous affine matrix 乘法。
- **20 分钟**：选择 identity、translation、scale、90° rotation 的小 image。
- **50 分钟**：对目标像素手算 destination→source 坐标、nearest source index；
  至少覆盖 border。
- **10–30 分钟**：写 table-driven expected outputs，不从未来 implementation 生成。
- **必须留下**：matrix/layout convention 图和四组 hand cases。

### Day 3 — CPU Affine Nearest

- **10 分钟**：把 Day 2 mapping 写成伪代码。
- **15 分钟**：决定内部是否求逆；若求逆，先实现/使用可验证的小矩阵 inverse。
- **55 分钟**：实现 CPU affine nearest 和 validation，先只跑 identity/translation。
- **10–30 分钟**：逐像素比较 hand expected，打印首个 coordinate mismatch。
- **必须留下**：implementation、tests 和 matrix direction 证据。

### Day 4 — CPU Affine Bilinear

- **10 分钟**：确认能否复用 resize bilinear sample，而不复用 resize scale mapping。
- **20 分钟**：定义 border clamp/constant 与 fractional coordinate 行为。
- **50 分钟**：实现 bilinear sampling，加入 identity、subpixel translation、scale。
- **10–30 分钟**：与独立 NumPy/Python oracle 或手算关键像素比较。
- **必须留下**：bilinear implementation、oracle 和 tolerance。

### Day 5 — Boundary/Failure Cases

- **10 分钟**：列 rotation、all-outside、padded stride、singular/extreme matrix。
- **15 分钟**：决定每项 expected behavior。
- **55 分钟**：增加 tests；用 padding sentinel 检查 writes，不允许 NaN/Inf 静默传播。
- **10–30 分钟**：运行 host sanitizer（可用时）和完整 CPU tests。
- **必须留下**：boundary matrix、failure behavior 和 known limits。

### Day 6 — Randomized CPU Gate

- **10 分钟**：固定 seed 和 safe transform ranges，避免所有 random cases 都出界。
- **20 分钟**：设计 independent oracle/round-trip property 的限制。
- **50 分钟**：运行数十个 randomized cases，保存失败 transform/input。
- **10–30 分钟**：更新 `docs/warp-design.md`，weekly review；明确 perspective optional。
- **必须留下**：CPU affine Gate、random seed 和完整 design。

## 必须决定

- 输入矩阵是 source→destination 还是 destination→source。
- row-major/column-major 与 homogeneous coordinate convention。
- 是否由 API 内部求逆，singular matrix 如何报告。
- border clamp/constant，以及 constant value。
- `w≈0` 只有进入 perspective extension 时才需要定义。

## 交付

- 完整 `docs/warp-design.md`。
- CPU affine nearest/bilinear。
- hand cases + randomized tests。

## 完成标准

- [ ] identity 和 translation 可手工解释。
- [ ] matrix direction 没有依赖调用者猜测。
- [ ] border/stride/invalid transform 行为明确。
- [ ] perspective 未做时明确标为 non-goal，而非半实现。
