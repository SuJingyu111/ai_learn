# Step 027 — `bench` harness：median/p95 与溯源

**你会做出**：一个测量工具，Phase 06 到 Phase 10 的每一个性能结论都靠它。

**为什么是现在**：前面几个 additional goal 已经在测时间了。把它们用的东西
正式化，并且**从一开始就把溯源信息带上**——否则三个月后你会看着一堆数字
想不起来那是在哪台机器、哪个 commit 上测的。

## 需要知道的（5 分钟）

仓库里已经有一个做对了这些事的例子：`engine/bench/benchmark_resize.cpp`。
它输出 `# utc= commit= host= build_type= compiler=` 的溯源头，
分位数用 nearest-rank，样本数不足 20 时 p95 直接输出 `NA` 而不是编一个数。

规则见 [reference/benchmark-methodology](../../docs/benchmark-methodology.md)。

## 动手

1. `engine/include/tt/bench.h`：`BenchResult run_bench(name, fn, warmup, iters)`，
   返回 median/p95/min/n。
2. 复用 `benchmark_resize.cpp` 里的 nearest-rank 分位数和低样本量保护逻辑——
   **不要重写一个**，把它提取到公共头文件里，两边都用。
3. 输出格式与 `benchmark_resize` 一致，这样 `benchmark-results/` 下的
   CSV 可以用同一个脚本分析。
4. 计时区间里**只放被测的东西**：分配、校验、打印全部挪出去。

## 跑起来

```bash
cmake --build build-cpu --parallel
CUDA_AI_COMMIT=$(git describe --always --dirty) ./build-cpu/tt bench matmul
```

## 你应该看到

```
# utc=2026-07-28T06:12:03Z commit=a1b2c3d-dirty host=Darwin 25.5.0 arm64
# build_type=Release compiler=AppleClang 21.0.0 timer=steady_clock
# warmup=3 iterations=30 timed_region=matmul-only
name,n,median_ms,p95_ms,min_ms
matmul_naive_N512,30,...,...,...
matmul_ikj_N512,30,...,...,...
```

样本数少于 20 时 p95 必须是 `NA`：

```bash
./build-cpu/tt bench matmul --iters 5
```

```
warning: 5 iterations is below the 20 needed for a p95 estimate
matmul_naive_N512,5,...,NA,...
```

## 卡住了

- 数字每次差很多 → 确认是 Release 构建，且计时区间里没有分配。
- p95 等于 min → 你重写了分位数逻辑并且踩了和原来一样的坑
  （`fraction * (n-1)` 在小 n 时退化）。用现成的那份。

---

## Phase 01 完成

你现在有：5 个算子、手算 fixture、numpy 对拍、明确的容差依据、
一个带溯源的测量工具。

**下一步**：Phase 02 把这些算子连成图。它的步骤会在你走到这里时按当时真实的
代码写出来——见 [STEPS.md](../../STEPS.md) 的整体弧线。
