# Core Module 01 — Tensor、Shape、Stride 与 Layout

## 本周目标

建立 AI runtime 的基础数据模型：tensor 不是“带 shape 的数组”这么简单；dtype、
stride、layout、device 和 ownership 都会影响语义与执行。

## 核心问题

- shape、stride、layout、contiguous 分别描述什么？
- NCHW 和 NHWC 的同一逻辑元素地址如何计算？
- view、transpose、reshape、contiguous 何时复制数据？
- image `HWC` 与 model tensor `NCHW` 之间需要哪些转换？

## 每日计划

### Day 1 — 建立 Python 实验环境与 Tensor 词汇

- **10 分钟**：复制 daily log，写下你当前对 shape/stride/layout 的定义。
- **20 分钟**：只读 `docs/resources.md` 中 NumPy fundamentals 的 array basics。
- **50 分钟**：创建 `labs/ml/week01_tensor_layout.py`；构造标量、1D、2D 和
  `1×3×4×5` arrays，打印 `shape/strides/dtype/itemsize/nbytes`。
- **10–30 分钟**：运行
  `python3 labs/ml/week01_tensor_layout.py`，检查 `nbytes` 是否等于元素数乘
  `itemsize`；记录 Python/NumPy 版本。
- **必须留下**：脚本、完整运行命令、5 个术语的一句话定义；环境不可用时留下
  import error 和待安装依赖，不临时换到在线 notebook。

### Day 2 — 从 Stride 手算地址

- **10 分钟**：不运行代码，写出 contiguous NCHW 的四维 offset 公式。
- **20 分钟**：复习 row-major 与 byte stride，只读够回答公式的部分。
- **50 分钟**：在脚本中给 `1×3×4×5` 填入递增整数；选择 5 个坐标，分别用
  公式和 NumPy indexing 取值。
- **10–30 分钟**：加入一个越界坐标的受控失败实验；确认 stride 单位是 bytes，
  并与本项目 `stride_elements` 对比。
- **必须留下**：5 行“坐标→offset→value”表和一次错误假设。

### Day 3 — View、Transpose、Reshape 与 Copy

- **10 分钟**：预测 slice/transpose/reshape 哪些会复制 storage。
- **20 分钟**：阅读 NumPy copies/views 对应小节。
- **50 分钟**：为 base array 创建 slice、transpose、reshape 和 explicit copy；
  打印 strides，并用 `shares_memory`/修改元素验证 storage sharing。
- **10–30 分钟**：尝试 reshape 一个 non-contiguous view；记录成功、复制或失败
  的实际行为，不泛化到所有 framework。
- **必须留下**：一张 operation/view-or-copy/stride/验证方式表。

### Day 4 — PyTorch Tensor 对照

- **10 分钟**：写出 NumPy 与 PyTorch 预计相同和不同的三点。
- **20 分钟**：阅读 PyTorch Tensor Views 的 contiguous/transpose 部分。
- **50 分钟**：创建 `labs/ml/week01_torch_views.py`，重复 Day 2–3；打印
  `shape/stride()/dtype/device/is_contiguous()`。
- **10–30 分钟**：对 transposed tensor 调用 `view`、`reshape`、`contiguous`，
  保存每项结果或异常。
- **必须留下**：脚本和 NumPy/PyTorch 对照表；没有 PyTorch 时记录安装阻塞，
  仍完成 NumPy 部分。

### Day 5 — HWC→CHW CPU Conversion

- **10 分钟**：手画 `H=2,W=3,C=2` 的 HWC 与 CHW physical order。
- **15 分钟**：写输入/输出 contract：dtype、shape、是否支持 padded row。
- **55 分钟**：实现 reference conversion，可用显式 loops；测试 `1×1×1`、
  `3×5×4` 和固定递增值。
- **10–30 分钟**：与 NumPy transpose + contiguous 结果逐元素比较。
- **必须留下**：conversion、至少 3 个 tests、首个 mismatch 的报告逻辑。

### Day 6 — 概念固化与周 Gate

- **15 分钟**：闭卷回答核心问题，不会的标记出来。
- **45 分钟**：写 `notes/concepts/tensor-shape-stride-layout.md`，加入地址公式、
  view/copy 和 HWC/NCHW 图。
- **20 分钟**：阅读当前 `include/cuda_ai/image.h`，列出它与 tensor metadata 的
  相同点和缺项。
- **10–30 分钟**：运行本周所有脚本/tests；填写 weekly review 和下周第一步。
- **必须留下**：概念笔记、可复现命令列表和完成标准逐项证据。

## 最小实验

- 输入：`1×3×4×5`，值为可追踪的递增整数。
- 变体：NCHW、NHWC、non-contiguous slice、explicit contiguous copy。
- 验证：逻辑值一致，physical stride/address 解释正确。

## 交付

- `notes/concepts/tensor-shape-stride-layout.md`
- 一个可运行 notebook 或 Python script。
- HWC→CHW 的固定输入测试及命令记录。

## 完成标准

- [ ] 不看文档能从 shape/stride 计算元素 offset。
- [ ] 能解释 transpose view 与 contiguous copy 的区别。
- [ ] 能指出当前 `ImageView` 与 ML tensor contract 的差异。
- [ ] 至少记录一个错误假设及修正。
