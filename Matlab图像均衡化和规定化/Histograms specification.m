yclose all
clear all
clc
f=imread('lena.bmp');  %读取图像, 设定图像文件位置
f1=rgb2gray(imread('tree.png'));  %读取图像, 设定图像文件位置
f3=histeq(f)         %原图均衡化
f2=histeq(f1);       %参考图均衡化
[hgram,x]=imhist(f1);
I=histeq(f,hgram);
subplot(5,2,1);imshow(f),title('原图');
subplot(5,2,2);imhist(f),title('原图直方图')
subplot(5,2,3);imshow(f3),title('原图均衡化');
subplot(5,2,4);imhist(f3),title('原图均衡化直方图')
subplot(5,2,5);imshow(f1),title('参考原图');
subplot(5,2,6);imhist(f1),title('参考原图直方图');
subplot(5,2,7);imshow(f2),title('参考原图均衡化');
subplot(5,2,8);imhist(f2),title('参考原图均衡化直方图');
subplot(5,2,9);imshow(I),title('原图规定化到参考原图');
subplot(5,2,10);imhist(I),title('原图规定化到参考原图直方图');