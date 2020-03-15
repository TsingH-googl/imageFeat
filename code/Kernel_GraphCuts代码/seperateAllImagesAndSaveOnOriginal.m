clear all;
skydir='C:\Users\406\Desktop\新建文件夹\算法实验\skydehaze\';%用于保存天空分割后的天空的子文件夹
grounddir ='C:\Users\406\Desktop\新建文件夹\算法实验\grounddehaze\';%用于保存地面分割后的地面的子文件夹
LLdir='C:\Users\406\Desktop\新建文件夹\算法实验\LogicalMatrix\';%为了从原始图像获得LL图像的逻辑矩阵。
dehazedir='C:\Users\406\Desktop\新建文件夹\算法实验\dehaze\'; %去雾后的所有的图像所在位置。
LL_list=dir(strcat(LLdir,'*.mat'));
for block=2:2:2
     skynewdir=strcat(strcat(skydir,strcat('dehaze',num2str(block))),'\\');
     mkdir(skynewdir);
     groundnewdir=strcat(strcat(grounddir,strcat('dehaze',num2str(block))),'\\');
     mkdir(groundnewdir);
     if (block<10)
         temp='dehaze0';
     else
         temp='dehaze';
     end
     blockdir=strcat(strcat(dehazedir,strcat(temp,num2str(block))),'\');
     blockdir_list=dir(strcat(blockdir,'*.jpg'));
     img_num=length(blockdir_list);
        for i = 1:1:img_num %逐一读取图像
            %得到逻辑矩阵LL，用于分割图像。
            imgtoseperate = strcat(blockdir,blockdir_list(i).name);% 待分割的图像的绝对路径。
            im=im2double(imread(imgtoseperate));
            LL=load(strcat(LLdir,LL_list(i).name));
            [sky ground]=seperate(im,LL.LL);
            imwrite((sky),strcat(skynewdir,strcat(  strcat(LL_list(i).name(1:8),'-sky') ,'.jpg')));
            imwrite((ground),strcat(groundnewdir,strcat(  strcat(LL_list(i).name(1:8),'-ground') ,'.jpg')));
        end
end

