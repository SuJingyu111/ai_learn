# Warp Design（Core Module 13 填写）

## API 与语义

- input/output dtype、layout、channel：TBD
- transform direction（source→destination 或 destination→source）：TBD
- pixel-center convention：TBD
- interpolation / border mode：TBD
- CPU/CUDA ownership 与 stream contract：TBD

## Coordinate pipeline

```text
destination pixel -> inverse homogeneous transform -> source coordinate
                  -> border policy -> interpolation -> output
```

## Correctness oracle

- identity / translation / rotation / perspective hand cases
- fixed-seed randomized CPU vs CUDA tests
- absolute / relative tolerance and rationale
- behavior near homogeneous `w == 0`

## Performance hypotheses

- matrix storage:
- repeated coordinate math:
- source access locality:
- border divergence:
- block geometry:

## Failure modes

- integer overflow / float precision
- invalid dimensions / strides
- out-of-bounds source access
- async lifetime / wrong stream dependency

## Known limitations

TBD during Core Module 13.
