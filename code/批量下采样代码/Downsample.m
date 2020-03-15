clear all;
save_dir='C:\Users\406\Desktop\新建文件夹 (2)\nonskydownsample\\'
file_path =  'C:\Users\406\Desktop\新建文件夹 (2)\originalnonsky\\';% 待去雾的图像所在文件夹
img_path_list = dir(strcat(file_path,'*.jpg'));%获取该文件夹中所有png格式的图像
img_num = length(img_path_list);%获取图像总数量
 for j = 1:1:img_num %逐一读取图像
            image_name = img_path_list(j).name;% 图像名
            fprintf('%s\n',strcat(file_path,image_name));% 显示正在处理的图像名
            img=strcat(file_path,image_name); 
            I = imread(img);
            I=imresize(I,[750 1000]);
            fullname=strcat(image_name(1:8),'.jpg');
            imwrite(I,strcat(save_dir,fullname));
end