% RGBɫ�ʷֲ�ͼ����ÿ�����ص��R��G��Bֵ����ά��������ʾ������
% Ŀ���Ƿ�����ͼ���RGBͨ��ֵ�ķֲ����

clc
clear all
close all
% ����ͼ��
I  = im2double(imread('C:\Users\406\Desktop\�½��ļ���\�㷨ʵ��\DataBaseB\OriginalImgaes\beijing\05.png'));
% ��ʾͼ���ɫ�ʷֲ�ͼ
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
