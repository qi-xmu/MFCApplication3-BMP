close all; clear all; clc
%%

f=imread('tree.png');  %读取图像, 设定图像文件位置
f=rgb2gray(f);         %rgb2gray
x=0:255;               %设定图像长度 (有待确认名称) 灰度值长度?
num=zeros(1,256);      %设定直方图(x,y)=灰度值*像素数
sum_num1=0;
[a,b]=size(f);   %输入图像   长a  *  宽b,
for i=0:255   %像素灰度值
    for j=1:a
        for m=1:b
            if f(j,m)==i     %确定图像的某个像素的灰度值
                num(i+1)=num(i+1)+1;   %i为0到255,统计个灰度值的个数
            end
        end
    end
end
sum_num=a*b;  %计算像素总数
subplot(2,2,1),bar(x,num/sum_num,0.875),title('原始直方图');

%直方图均衡化
num1=zeros(1,256);
num2=zeros(1,256);
for i=1:256
    for j=1:i
        sum_num1=sum_num1+num(j);  %累计直方图
    end
    num1(i)=round(255*sum_num1/sum_num); %取整
    sum_num1=0;
end
for i=1:256
    num2(i)=sum(num(num1==(i-1)));
end
subplot(2,2,3),bar(x,num2/sum_num,0.875),title('均衡化直方图');

%均衡化后的图像
f_new=f;
for i=0:255
    f_new(f==i)=num1(i+1); %找出原图像素的位置，将均衡化后的像素填入对应位置
end
subplot(2,2,2),imshow(f);title('原始图像')
subplot(2,2,4),imshow(f_new);title('均衡化后的图像')


imwrite(f_new, 'tree_new.png')