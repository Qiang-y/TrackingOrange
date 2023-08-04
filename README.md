# TrackingOrange
## tdt进阶题

***

./v_a_t 文件夹里使用的是速度-加速度的直接预测，目前可以预测下一帧以及判断是否转向

***
./kalman 文件夹里使用的是kalman滤波，目前只可以预测下一帧的位置。

opencv自带的kalman滤波器暂时没能弄明白是如何工作的，我参考了 https://github.com/radikz/Ball-Prediction 
的写法进行修改。
