import numpy as np

arr = np.empty((3, 4), dtype=np.float32)
#shape、strides、dtype、itemsize
print("shape: ", arr.shape)
print("strides: ", arr.strides)
print("dtype: ", arr.dtype)
print("itemsize: ", arr.itemsize)