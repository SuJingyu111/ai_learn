# 环境：现在需要什么，以后需要什么

按 Phase 分。**不要提前配后面才用的东西**——Phase 00 到 06 一台笔记本就够，
在那之前折腾 Orin 是纯粹的拖延。

日常入口是 [`../STEPS.md`](../STEPS.md)。

## 现在（Phase 00–06）

引擎本身只要一个 C++17 编译器和 CMake。对拍脚本要 Python——
Step 024 起用 numpy，Phase 05 起用 onnxruntime。**不需要 GPU，不需要 Orin。**

```bash
./scripts/build_and_test.sh cpu
```

跑通了就可以开始 Step 001。

Phase 01 的 numpy 对拍和 Phase 05 的 onnxruntime 对拍需要 Python：

```bash
python3 -m venv .venv
.venv/bin/python -m pip install --upgrade pip
.venv/bin/python -m pip install numpy
.venv/bin/python -m pip freeze > benchmark-results/host-python-freeze.txt
```

`onnxruntime` 等到 Phase 05 再装——那时装才知道该装哪个版本。
**保留 freeze 文件**：版本快照是后面所有对拍结论的一部分，重建环境时生成新的，
不要覆盖旧的。

## Phase 07 之前：一块 NVIDIA GPU

```bash
nvcc --version
./scripts/build_and_test.sh cuda
```

`ctest` 应该报告 2 个测试，包含 `resize_cuda_parity`。再跑一次 sanitizer：

```bash
compute-sanitizer ./build-cuda/resize_cuda_tests
```

桌面 GPU 可以用来学 CUDA 和验证正确性，**但它的性能数字不能代表 Orin**。

## Phase 10 之前：Jetson Orin

先把设备实际版本记下来，不要用计划里的示例版本：

```bash
uname -a
cat /etc/nv_tegra_release
nvcc --version
cmake --version
c++ --version
dpkg-query -W 'nvidia-jetpack' 'nvidia-l4t-core' 2>/dev/null || true
```

不同 JetPack 绑定不同的 CUDA/TensorRT/cuDNN。**以设备实际版本为准再查对应版本的
文档**，不要为了追新 API 随意升级工作设备。

每轮正式 benchmark 还要记录：

```bash
sudo nvpmodel -q          # power mode
jetson_clocks --show      # clocks
tegrastats                # 运行中的功耗/温度/占用
```

改变 power mode 或 clocks 之后，旧数据不再是公平 baseline。
不了解散热和供电条件时不要长期锁最高频。

### host → Orin 工作流

```text
host 上编辑/测试
  → 小步提交
  → Orin 上同步/checkout
  → clean configure/build/test
  → 跑 benchmark/profile
  → 把原始 CSV 拷回 host
  → 在 host 上写结论
```

用 commit 作为 source identity。`scripts/run_benchmarks.sh` 会自动把
`git describe --dirty` 写进 CSV 的溯源头。

Nsight trace 文件可能很大且含机器路径。提交前检查隐私和体积；
**结论不能只存在截图里**。

## 卡住超过 30 分钟

1. 把输入缩到 1–32 个元素。
2. 打印 shape、stride、dtype、offset、地址。
3. 回到 CPU 参考实现，或一线程一元素的 kernel。
4. 一次只改一个变量。
5. 区分：语义错误、数值误差、内存错误、同步错误、测量错误。这五类的查法完全不同。
6. 保存最小复现、预期、实际和版本。

缺的是**理论**而不是线索时，去
[`../reference/README.md`](../reference/README.md) 的概念索引。

## 东西写在哪

| 内容 | 位置 |
|---|---|
| 引擎代码 | `engine/` |
| 对拍脚本 | `oracle/` |
| 原始 benchmark CSV | `benchmark-results/`（**不 gitignore，刻意的**） |
| additional goal 的分析、实验记录 | `notes/experiments/` |
| 个人日志 | `notes/daily/`（gitignore） |
| 模型文件 | `models/`（gitignore，用 `scripts/fetch_models.sh` 取） |

模板在 [`../notes/templates/`](../notes/templates/)。
