clear all;
save_dir='C:\Users\406\Desktop\�½��ļ��� (2)\nonskydownsample\\'
file_path =  'C:\Users\406\Desktop\�½��ļ��� (2)\originalnonsky\\';% ��ȥ���ͼ�������ļ���
img_path_list = dir(strcat(file_path,'*.jpg'));%��ȡ���ļ���������png��ʽ��ͼ��
img_num = length(img_path_list);%��ȡͼ��������
 for j = 1:1:img_num %��һ��ȡͼ��
            image_name = img_path_list(j).name;% ͼ����
            fprintf('%s\n',strcat(file_path,image_name));% ��ʾ���ڴ����ͼ����
            img=strcat(file_path,image_name); 
            I = imread(img);
            I=imresize(I,[750 1000]);
            fullname=strcat(image_name(1:8),'.jpg');
            imwrite(I,strcat(save_dir,fullname));
end