clear all;
skydir='C:\Users\406\Desktop\�½��ļ���\�㷨ʵ��\skydehaze\';%���ڱ�����շָ�����յ����ļ���
grounddir ='C:\Users\406\Desktop\�½��ļ���\�㷨ʵ��\grounddehaze\';%���ڱ������ָ��ĵ�������ļ���
LLdir='C:\Users\406\Desktop\�½��ļ���\�㷨ʵ��\LogicalMatrix\';%Ϊ�˴�ԭʼͼ����LLͼ����߼�����
dehazedir='C:\Users\406\Desktop\�½��ļ���\�㷨ʵ��\dehaze\'; %ȥ�������е�ͼ������λ�á�
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
        for i = 1:1:img_num %��һ��ȡͼ��
            %�õ��߼�����LL�����ڷָ�ͼ��
            imgtoseperate = strcat(blockdir,blockdir_list(i).name);% ���ָ��ͼ��ľ���·����
            im=im2double(imread(imgtoseperate));
            LL=load(strcat(LLdir,LL_list(i).name));
            [sky ground]=seperate(im,LL.LL);
            imwrite((sky),strcat(skynewdir,strcat(  strcat(LL_list(i).name(1:8),'-sky') ,'.jpg')));
            imwrite((ground),strcat(groundnewdir,strcat(  strcat(LL_list(i).name(1:8),'-ground') ,'.jpg')));
        end
end

