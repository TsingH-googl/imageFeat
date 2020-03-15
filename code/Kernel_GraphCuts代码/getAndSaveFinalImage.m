
skydir='C:\Users\406\Desktop\新建文件夹\\sky\\';%天空
grounddir = 'C:\Users\406\Desktop\新建文件夹\\ground\\';%地面
oridir='C:\Users\406\Desktop\新建文件夹\\有天空的图像\\';%原图像
save_dir='C:\Users\406\Desktop\新建文件夹\\去雾后合并的图像\\';%要保存的目录
img_path_list_sky = dir(strcat(skydir,'*.jpg'));
img_path_list_ground = dir(strcat(grounddir,'*.jpg'));
ori_list=dir(strcat(oridir,'*.JPG'));
img_num = length(img_path_list_sky);%获取图像总数量
if img_num > 0 %有满足条件的图像
        for i = 1:img_num %逐一读取图像
            image_name_sky = strcat(skydir,img_path_list_sky(i).name);% 天空的图像的绝对路径。
            image_name_ground = strcat(grounddir,img_path_list_ground(i).name);% 地面的图像的绝对路径。
            image_name_ori = strcat(oridir,ori_list(i).name);% 原始的图像的绝对路径。
            finalimg= getFinal(image_name_ori ,image_name_sky,image_name_ground );
            shortname=img_path_list_sky(i).name;
            imshow(finalimg);
            imwrite((finalimg),strcat(save_dir,strcat(  strcat(shortname(1:8),'Dehaze')  ,'.jpg')));
        end
end

