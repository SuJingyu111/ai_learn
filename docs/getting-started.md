# 开始前：Host、Jetson Orin 与实验基线

先完成 [`../START_HERE.md`](../START_HERE.md) 的入学诊断。本文件建立设备与实验
环境，不替代 Python/math/GitHub prerequisite 检查。

## 1. 两个环境的职责

### Development host

用于编辑、CPU reference、文档、格式检查和可在本机执行的 tests。若 host 不是
NVIDIA Linux，不要求运行 CUDA，也不能用 host timing 代表 Orin。

### Jetson Orin

是 CUDA correctness、Compute Sanitizer、Nsight、ONNX Runtime/TensorRT 和最终
性能结论的 source of truth。

先记录：

```bash
uname -a
cat /etc/nv_tegra_release
nvcc --version
nvidia-smi || true
cmake --version
c++ --version
dpkg-query -W 'nvidia-jetpack' 'nvidia-l4t-core' 2>/dev/null || true
```

不同 JetPack 版本绑定不同 CUDA/TensorRT/cuDNN。先以设备实际版本为准，再查
对应版本文档；不要为了追最新 API 随意升级工作设备。

## 2. Orin measurement baseline

每轮正式 benchmark 记录：

- Orin 型号与内存；
- JetPack/L4T、CUDA、TensorRT、cuDNN；
- power mode、clock policy；
- 温度、thermal throttling 和后台负载；
- compiler、build type 和 flags；
- 输入 shape/layout/dtype；
- warm-up、iterations、同步位置；
- allocation/H2D/kernel/D2H/runtime build 是否计入。

常用观察工具以设备版本为准：

```bash
sudo nvpmodel -q
tegrastats
jetson_clocks --show
```

不要在不了解散热和供电条件时长期锁定最高 clocks。改变 power mode 或 clocks
后，旧数据不再是公平 baseline。

## 3. 第一次构建

Host CPU-only：

```bash
./scripts/build_and_test.sh cpu
```

Orin CUDA：

```bash
./scripts/build_and_test.sh cuda
compute-sanitizer ./build-cuda/resize_demo
```

若失败，先区分 configure、compile、link、CUDA API、launch 和 async execution
error。保存第一条真实错误以及完整环境，不只复制最后一行。

## 4. 第一次 ML baseline

Calendar Week 1–12（Unit 1–2）使用
Python/NumPy/PyTorch/ONNX/ONNX Runtime。环境应可重建，并保存：

```bash
python3 -m venv .venv-learning
source .venv-learning/bin/activate
python -m pip install --upgrade pip
python -m pip install numpy torch onnx onnxruntime
python -c "import numpy, torch, onnx, onnxruntime; print(numpy.__version__, torch.__version__, onnx.__version__, onnxruntime.__version__)"
mkdir -p artifacts/environment
python -m pip freeze > artifacts/environment/host-learning-freeze.txt
```

这不是要求永远使用安装时的最新版，而是建立**本次课程实际解析出的版本快照**。后续
若重建或升级，保留旧 freeze、生成新文件并重跑相应 Gate；不要覆盖历史 evidence。
公司环境、Apple Silicon、代理或 Python 版本导致安装失败时，保存 exact command/error，
按各项目官方安装说明选择兼容 wheel。禁止为绕过依赖问题改用不可复现的在线 notebook。

先在 CPU 上完成小型 graph，不在第一周解决所有 Jetson Python wheel 兼容问题。
Week 1 允许暂时缺 PyTorch，**Week 2 Exit Ticket 前必须完成上述四包 import/version
smoke test**，否则不解锁 Week 3。ONNX/ORT 在 Week 9 才正式使用，但提前验证安装可为
兼容性问题留出 remediation 时间。

进入 Unit 3 前准备 CUDA 环境；进入 Unit 6 的 ORT CUDA/TensorRT 前，再按实际
JetPack support matrix 选择匹配版本，不能盲用 `latest`。

## 5. Host → Orin 工作流

推荐使用 Git commit 作为 source identity。若暂时没有 remote，也要让 benchmark
记录 commit 或明确记录 `dirty/unversioned`。

建议流程：

```text
host edit/test
  -> small commit
  -> sync/checkout on Orin
  -> clean configure/build/test
  -> run benchmark/profile
  -> copy raw CSV/JSON/report metadata back
  -> write conclusion on host
```

Nsight trace 文件可能很大且包含机器路径。保留必要报告，提交前检查隐私和仓库
大小；结论不能只存在截图里。

## 6. 卡住超过 30 分钟

1. 把 tensor/image/kernel 输入缩到 1–32 个元素。
2. 打印 shape、stride、dtype、layout、provider 和 device。
3. 回到 CPU reference 或一线程一元素 kernel。
4. 一次只改变一个变量。
5. 区分语义错误、数值误差、内存错误、同步错误和 measurement error。
6. 保存最小复现、预期、实际结果和版本。

## 7. 每周留下什么

首次执行：

```bash
mkdir -p notes/{diagnostics,problems,weekly,gates,curriculum,feed} artifacts/environment milestones labs
```

- `notes/daily/`：问题、命令、失败和短结论。
- `notes/concepts/`：可复用的心智模型。
- `notes/problems/`：手算、首次答案、订正与新题复测。
- `notes/gates/`：Major Gate 分数、hard blockers、remediation。
- `notes/feed/`：每周可选 Feed Card 的四段式复盘。
- `benchmark-results/`：原始 CSV/JSON。
- `docs/`：稳定下来的 API、架构和 case study。
- `weeks/`：周任务和 Gate，不在这里伪造完成状态。
