# Pedestrain-Detection
Pedestrain-Detection-using-Histogram-of-Oriented-Gradients

## OpenCV 工程
- 文件`HOG_SVM_OPENCV`表示在用opencv库函数实现HOG+SVM的算法
- 环境VS2013+OpenCV3.1
- 工程分为训练阶段和测试阶段，修改其文件路径，即可运行
- 图像数据库`Pedestrians64x128`，我在网上下载的正样本ppm格式64*128，负样本jpg格式64*128；测试样本随意；找不到的可以提问给我
- 测试结果如下,论证方案的可行性
![opencv_result](https://github.com/ranjiewwen/Pedestrain-Detection/blob/master/image/opencv_result.png)


## C 工程
- 文件`HOG_SVM_master`表示在纯C环境下实现
- 论证该方案可以向DSP嵌入式平台移植
- 测试图片可以是24位彩色bmp,或者8位bmp图像
- 其中svm超平面的参数用的opencv源代码中的参数，由提供算法的提前训练好的
- 测试结果：
![C_result](https://github.com/ranjiewwen/Pedestrain-Detection/blob/master/image/C_result.png)

## DSP嵌入式平台

- 经测试DSP能成功移植，但是由于HOG特征提取耗时很严重，处理一张640*480需要20s左右；这是在单核dsp上处理的
- opencv内部存在内存优化和并行计算


## 总结

- vgg:例子有交通标志识别，可以借鉴其算法。
- 优化HoG特征，加速实现；运用多核dsp并行计算。
