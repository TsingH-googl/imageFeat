
skydir='C:\Users\406\Desktop\�½��ļ���\\sky\\';%���
grounddir = 'C:\Users\406\Desktop\�½��ļ���\\ground\\';%����
oridir='C:\Users\406\Desktop\�½��ļ���\\����յ�ͼ��\\';%ԭͼ��
save_dir='C:\Users\406\Desktop\�½��ļ���\\ȥ���ϲ���ͼ��\\';%Ҫ�����Ŀ¼
img_path_list_sky = dir(strcat(skydir,'*.jpg'));
img_path_list_ground = dir(strcat(grounddir,'*.jpg'));
ori_list=dir(strcat(oridir,'*.JPG'));
img_num = length(img_path_list_sky);%��ȡͼ��������
if img_num > 0 %������������ͼ��
        for i = 1:img_num %��һ��ȡͼ��
            image_name_sky = strcat(skydir,img_path_list_sky(i).name);% ��յ�ͼ��ľ���·����
            image_name_ground = strcat(grounddir,img_path_list_ground(i).name);% �����ͼ��ľ���·����
            image_name_ori = strcat(oridir,ori_list(i).name);% ԭʼ��ͼ��ľ���·����
            finalimg= getFinal(image_name_ori ,image_name_sky,image_name_ground );
            shortname=img_path_list_sky(i).name;
            imshow(finalimg);
            imwrite((finalimg),strcat(save_dir,strcat(  strcat(shortname(1:8),'Dehaze')  ,'.jpg')));
        end
end

