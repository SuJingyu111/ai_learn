#!/usr/bin/env bash
# Fetch the ONNX models the steps use. Models are not committed: they are large
# and their licences are their own. Phase 03 onwards needs mnist.onnx.
set -euo pipefail

mkdir -p models
cd models

fetch() {
  local name="$1" url="$2"
  if [[ -f "${name}" ]]; then
    echo "${name} already present"
    return
  fi
  echo "fetching ${name}"
  curl -fsSL -o "${name}" "${url}"
}

# ONNX model zoo, MNIST (opset 8, ~26 KB).
fetch mnist.onnx \
  https://github.com/onnx/models/raw/main/validated/vision/classification/mnist/model/mnist-8.onnx

echo
echo "models/:"
ls -lh
