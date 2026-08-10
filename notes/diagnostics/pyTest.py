import numpy as np

arr = np.zeros((3, 4), dtype=np.float32)
#shape、strides、dtype、itemsize
print("shape: ", arr.shape)
print("strides: ", arr.strides)
print("dtype: ", arr.dtype)
print("itemsize: ", arr.itemsize)

transposed_arr = arr.T
print(transposed_arr)

sliced_arr = arr[0:2, 0:3]
print(sliced_arr)

sliced_arr[0][1] = 1
print(sliced_arr)
print(arr)
print(transposed_arr)

# 原始图像：(H=224, W=224, C=3)
# 轴索引：   0      1      2
image_hwc = np.zeros((1920, 1080, 3)) 

# 进行转置，目标排序：(2, 0, 1)
image_chw = image_hwc.transpose(2, 0, 1)

print(image_chw.shape)
