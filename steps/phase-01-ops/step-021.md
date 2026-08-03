# Step 021 — 循环顺序 ijk vs ikj

**你会做出**：matmul 的第二种循环顺序，以及它为什么更快的解释。

**为什么是现在**：这是整个课程里第一次"改了代码它变快了"。
在只有三重循环、没有任何其他复杂度的情况下理解 cache，
比在 Conv 里理解要容易一个数量级。

## 动手

1. 加 `matmul_ikj`：把中间和最内层循环交换，**其他一切不变**。
2. 用 Step 020 的全部 fixture 验证它和 `matmul_naive` 结果完全一致
   （逐位相同，不是"在容差内"——同样的加法顺序应该给出同样的结果）。
3. 先不要接进 `matmul`。这一步只是把它放在那里。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt op-demo matmul-order
```

## 你应该看到

小矩阵结果一致，大矩阵时间明显不同：

```
N=64    ijk=0.31ms  ikj=0.29ms   results identical: yes
N=512   ijk=...ms   ikj=...ms    results identical: yes
```

具体数字取决于你的机器——**重点是 N 大时两者的比值**。

## 卡住了

- 两者时间一样 → 矩阵太小，全放得进 L1。把 N 加到 512 或 1024。
- 结果不完全一致 → 交换循环时改动了累加变量的作用域。
  `ikj` 的最内层是 `C[i][j] += A[i][k] * B[k][j]`，
  `C[i][j]` 不能提到寄存器里累加（那是 `ijk` 的写法）。

---

### 🎯 Additional goal — 为什么 ikj 更快（可选）

**假设**：N ≥ 512 时 `ikj` 比 `ijk` 快 2× 以上；N ≤ 64 时几乎没有差别。

**做什么**：用 `engine/bench` 的 harness，对 N = 64、128、256、512、1024
各测两个版本，至少 30 次迭代。

**测什么**：median/p95/min。有条件的话加上 cache miss：
Linux 用 `perf stat -e L1-dcache-load-misses,LLC-load-misses`，
macOS 用 Instruments 或先只看时间。

**你可能会发现**：交叉点大约在"三个 N×N 的 float32 矩阵刚好放不进 L2"的地方。
如果 N=1024 时两者仍然一样快，先确认编译器没有自动做循环交换——
用 `-O0` 再测一次对比（`-O0` 的绝对数字没有意义，但比值有）。

**为什么会这样**：`ijk` 的最内层循环变量是 k，访问 `B[k][j]`——
k 增加时地址跳 `N * 4` 字节，每次都是新的 cache line，一条 line 里 16 个 float
只用了 1 个。`ikj` 的最内层是 j，`B[k][j]` 和 `C[i][j]` 都沿行连续走，
一条 cache line 的 16 个 float 全部用上。

**这个结论什么时候不成立**：矩阵小到全部放进 L1 时（访问模式无所谓，
都不会 miss）；或者编译器已经做了循环变换。真实的高性能 matmul
两种顺序都不用——它做分块，那是 Phase 06 的事。

**写下来**：假设 → 五个 N 的实测表 → 交叉点在哪 → 为什么在那里 → 边界条件。
