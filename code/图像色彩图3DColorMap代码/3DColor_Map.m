% RGB色彩分布图，把每个像素点的R、G、B值在三维坐标上显示出来。
% 目的是分析该图像的RGB通道值的分布情况

clc
clear all
close all
% 读入图像
I  = im2double(imread('C:\Users\406\Desktop\新建文件夹\算法实验\DataBaseB\OriginalImgaes\beijing\05.png'));
% 显示图像的色彩分布图
R = I(:,:,1);
G = I(:,:,2);
B = I(:,:,3);
figure()
hold on
     plot3(R,G,B,'.'); 
hold off
grid on
axis([0 1.0  0 1.0  0 1.0]);
xlabel('Red')
ylabel('Green')
zlabel('Blue')
