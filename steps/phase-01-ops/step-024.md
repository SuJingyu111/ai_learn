# Step 024 — `numpy` 对拍脚本：第一个外部 oracle

**你会做出**：`oracle/` 下的第一个脚本，用 numpy 验证你的算子。

**为什么是现在**：手算 fixture 只能覆盖很小的输入。现在引入第一个外部参照，
把覆盖面扩大到随机大输入。这也是 Phase 05 对拍 onnxruntime 的预演——
通道（`.tt` 文件）你在 Step 013 已经建好了。

## 动手

1. `oracle/numpy_matmul.py`：读两个 `.tt` 文件，用 numpy 算 matmul，
   写出第三个 `.tt`。
2. `tt` 加一个 `gen` 子命令，用**固定 seed** 生成随机张量存成 `.tt`。
   固定 seed 是硬要求——不可复现的对拍失败无法调试。
3. 写一个 `oracle/check_matmul.sh`：生成 → 你的引擎算 → numpy 算 →
   `tt allclose` 比较。

## 跑起来

```bash
cmake --build build-cpu --parallel
./oracle/check_matmul.sh 128 256 64
```

## 你应该看到

```
gen A(128,256) seed=42 -> /tmp/A.tt
gen B(256,64)  seed=43 -> /tmp/B.tt
tt matmul     -> /tmp/C_mine.tt   (2.14 ms)
numpy matmul  -> /tmp/C_numpy.tt
allclose(rtol=1e-5, atol=1e-6): PASS   max_abs=6.10e-05 max_rel=2.8e-07
```

## 卡住了

- numpy 读不出你的 `.tt` → 回到 Step 013，确认 offset 和小端。
- 差异远大于预期 → 先用 4×4 跑一遍并 dump 两边完整结果。
  大矩阵的失败几乎总能在小矩阵上复现，别在大矩阵上调试。
- PASS 但你不放心 → 把你的 matmul 故意改错一位，确认脚本会 FAIL。
  一个从不失败的对拍脚本比没有更糟。
