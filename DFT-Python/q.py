import cv2
import numpy as np
import matplotlib.pyplot as plt

img = cv2.imread("fig1.bmp",0)

lists = [[1,2,3],[4,5,6],[7,8,9]]
arr = np.array(lists,dtype=np.float32)


# np.float32()将图像数据转换成float32 然后进行傅里叶变换cv2.dft()
dft = cv2.dft(np.float32(img),flags=cv2.DFT_COMPLEX_OUTPUT)
# print("arr",arr)
# print("dft",dft)


# 将低频信息转换至图像中心
dft_shift = np.fft.fftshift(dft)
# dft_shift = dft
# print("dft_shift",dft_shift)

# 傅里叶变换后的数据是由实部和虚部构成的，需要进行转换成图像格式才能显示(0,255)
# cv2.magnitude()将实部和虚部转换为实部，乘以20将结果放大
magnitude_spectrum = 20 * np.log(cv2.magnitude(dft_shift[:, :, 0], dft_shift[:, :, 1]))

# 滤波
rows, cols = img.shape
crow,ccol = int(rows/2), int(cols/2) #中心位置
mask = np.zeros((rows, cols, 2), np.uint8)

bor = 100>>1

mask[crow-bor:crow+bor, ccol-bor:ccol+bor] = 1
f = dft_shift * mask
magnitude_f = 20 * np.log(cv2.magnitude(f[:, :, 0], f[:, :, 1]))

ishift = np.fft.ifftshift(f)
iimg = cv2.idft(ishift)
res = cv2.magnitude(iimg[:,:,0], iimg[:,:,1])

print(magnitude_spectrum)

plt.subplot(221)
plt.imshow(img,cmap='gray')
plt.title('Input'),plt.xticks([]),plt.yticks([])
plt.subplot(222)
plt.imshow(magnitude_spectrum,cmap='gray')
plt.title('Output'),plt.xticks([]),plt.yticks([])

plt.subplot(223)
plt.imshow(magnitude_f,cmap='gray')
plt.title('Input2'),plt.xticks([]),plt.yticks([])

plt.subplot(224)
plt.imshow(res,cmap='gray')
plt.title('Output2'),plt.xticks([]),plt.yticks([])
plt.show()