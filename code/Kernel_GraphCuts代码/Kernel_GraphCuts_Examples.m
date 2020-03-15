% �˴�����salah���������kernel graph cutsͼ��ָ��㷨����Ҫ���ֵĸ�д��
% ��д��Ŀ����ʹ�ú�������ͼ��ָ�Ϊ�������ͷ���������������򣬲��������������ͼ���������ġ�

clear all; close all; 

% ע�⣬�����һЩͼ����û����յģ�������ͼ������Ի���һ�£�������ܻ���Ϊ2�����򣬳��������
path = 'D:\ͼ��ָ�\Images\�������ݿ�\DJI_0059.JPG';
im = im2double(imread(path)); %ע�⣬����ͼ��Ĵ�С�ᷢ���仯��
im=imresize(im,[750 1000]);
alpha=8;% ���ڵ����������̵�������͹⻬�����ϵ����д����Ҫ˵��������������á�
k =2;   % �ܵķָ�������


sz = size(im);
Hc=ones(sz(1:2));
Vc=Hc;
i_ground = 0; % rank of the bakground for plotting, 0: the darkest; 
%k-1 the brightest; 99: nowhere

diff=10000;
an_energy=999999999;
iter=0;
iter_v=0;
energy_global_min=99999999;

distance = 'sqEuclidean'; % �����ռ����

% Initialization: cluster the data into k regions
tic,
disp('Start kmeans');
data = ToVector(im);
[idx c] = kmeans(data, k, 'distance', distance,'EmptyAction','drop','maxiter',100);
c=c(find(isfinite(c(:,1))),:);
k=size(c,1);
k_max=k;
kmean_time=toc,

Dc = zeros([sz(1:2) k],'single');   
c,


tic
while iter < 5
    iter=iter+1;
    clear Dc
    clear K
    c;
    for ci=1:k
        K=kernel_RBF(im,c(ci,:));
        Dc(:,:,ci)=1-K;
    end   
    clear Sc
    clear K
    %% ƽ����
    Sc = alpha*(ones(k) - eye(k)); 
    gch = GraphCut('open', Dc, Sc, Vc, Hc);
    [gch L] = GraphCut('swap',gch);
    [gch se de] = GraphCut('energy', gch);
    nv_energy=se+de;
    gch = GraphCut('close', gch);
 
    if (nv_energy<=energy_global_min)
        diff=abs(energy_global_min-nv_energy)/energy_global_min;
        energy_global_min=nv_energy;
        L_global_min=L;
        k_max=k;
        
        nv_energy;
        iter_v=0;
        % Calculate region Pl of label l
        if size(im, 3)==3 % Color image
        for l=0:k-1
            Pl=find(L==l);
            card=length(Pl);
            K1=kernel_RBF(im(Pl),c(l+1,1));
            K2=kernel_RBF(im(Pl),c(l+1,2));
            K3=kernel_RBF(im(Pl),c(l+1,3));
            smKI(1)=sum(im(Pl).*K1); 
            smKI(2)=sum(im(Pl+prod(sz(1:2))).*K2); 
            smKI(3)=sum(im(Pl+2*prod(sz(1:2))).*K3);
            smK1=sum(K1);smK2=sum(K2);smK3=sum(K3);
            if (card~=0)
                c(l+1,1)=smKI(1)/smK1;c(l+1,2)=smKI(2)/smK2;c(l+1,3)=smKI(3)/smK3;
            else
                c(l+1,1)=999999999;c(l+1,2)=999999999;c(l+1,3)=999999999;
            end
        end
        end
        
%         if size(im, 1)==1 % Gray-level image
%         for l=0:k-1
%             Pl=find(L==l);
%             card=length(Pl);
%             K=kernel_RBF(im(Pl),c(l+1,1));
%             smKI=sum(im(Pl).*K);
%             smK=sum(K);
%             if (card~=0)
%                 c(l+1,1)=smKI/smK;
%             else
%                 c(l+1,1)=999999999;
%             end
%         end
%         end
        
        
        c=c(find(c(:,1)~=999999999),:);
        c_global_min=c;
        k_global=length(c(:,1));
        k=k_global;

    else
        iter_v=iter_v+1;
        %       Begin updating labels
        % Calculate region Pl of label l
        if size(im, 3)==3 % Color image 
        for l=0:k-1           
            Pl=find(L==l);
            card=length(Pl);
            K1=kernel_RBF(im(Pl),c(l+1,1));
            K2=kernel_RBF(im(Pl),c(l+1,2));
            K3=kernel_RBF(im(Pl),c(l+1,3));
            smKI(1)=sum(im(Pl).*K1); 
            smKI(2)=sum(im(Pl+prod(sz(1:2))).*K2); 
            smKI(3)=sum(im(Pl+2*prod(sz(1:2))).*K3);
            smK1=sum(K1);
            smK2=sum(K2);
            smK3=sum(K3);
            % Calculate contour Cl of region Pl
            if (card~=0)
                c(l+1,1)=smKI(1)/smK1;c(l+1,2)=smKI(2)/smK2;c(l+1,3)=smKI(3)/smK3;
            else
                c(l+1,1)=999999999;c(l+1,2)=999999999;c(l+1,3)=999999999;
                area(l+1)=999999999;
            end
        end
        end
        
%         if size(im, 3)== 1 % Gray-level image 
%         for l=0:k-1           
%             Pl=find(L==l);
%             card=length(Pl);
%             K=kernel_RBF(im(Pl),c(l+1,1));
%             smKI=sum(im(Pl).*K);
%             smK=sum(K);
%             % Calculate contour Cl of region Pl
%             if (card~=0)
%                 c(l+1,1)=smKI/smK;
%             else
%                 c(l+1,1)=999999999;
%                 area(l+1)=999999999;
%             end
%         end
%         end
              
        c=c(find(c(:,1)~=999999999),:);
        k=length(c(:,1));
    end
end

L=L_global_min;
energy_global_min;
c=c_global_min;

size(c,1)
iter;

% ��ʾ��Ǿ����Լ��ָ�Ľ��
if size(im, 3)==3 % Color image 
img=zeros(sz(1),sz(2),3);%�������ɺڰ׵ı�Ǿ���
j=1;
imshow(im); axis off; hold on; %������imͼ�ϻ�����ɫ�ָ���


for i=0:k_max-1 %k_maxҲ����Ҫ�ֳɼ�������
    %L_glo bal_min�������˷ֲ�Ľ������һ�������ֵ��0���ڶ��������ֵ��1����k����k-1��(��ʱ��show result�����ǲ���ģ���imagesc��һ����������ͼ����)
    %LL���߼�ֵ��ѭ���ض�LL�������߼�ֵ��Ŀ����ʹ��������һ�������ֵΪ1����ɫ����������һ�������ֵΪ0����ɫ��������ָ�Ϊ���������Ӧ�ڴ�����������ɫ�����
    LL=(L_global_min==i); % LL,������һ�������ֵΪ1����ɫ����������һ�������ֵΪ0����ɫ����
    is_zero=sum(sum(LL));
    %if is_zero  %Ŀ���ǿ�����ǰ���Ƿ���ڡ�
    %���濪ʼ����һ����k���ͼ��img��������ԭͼ��im����ɫ���ı䡣��
         img(:,:,1)=img(:,:,1)+LL*c(j,1); %c����k��3�� ; �����볣����ˣ������ÿһ��Ԫ�ض�������������
         img(:,:,2)=img(:,:,2)+LL*c(j,2);
         img(:,:,3)=img(:,:,3)+LL*c(j,3);
         j=j+1;
    %end
    if i~=i_ground
        color=[0.901  0.99 0];%ͼ��ָ��ߵ���ɫ������������ɫ��0<=channel<=1
        contour(LL,[1 1],'LineWidth',6.5,'Color',color); hold on;
    end
end
figure(2);
imagesc(img); axis off;%�ڵڶ�������figure��2������ʾ��Ǿ���ͼimg
end

% ��ʾ�ָ���
[sky ground] = seperate(im,LL);

%����ͼ��ע�����ﱣ���ͼ��Ĵ�С��ԭ���Ĳ�һ���������ǵ����ص��ֵ�Լ��ֱ��ʶ���һ����
% [o1 o2]=seperate(im,LL);
% block= 5;
% [I I_out J T_est T A] = removeHaze( o1, block);
%             temp='-';
%             if(block<10)
%                 temp='-0';
%             end
%             imwrite(uint8(I_out),strcat(save_dir,strcat(strcat(strcat(image_name(1:2),temp),num2str(block)),'.png')));
%%imwrite(im,'C:\Users\406\Desktop\�½��ļ���\1.jpg');


% if size(im, 3)==1 % Gray-level image 
% img=zeros(sz(1),sz(2));
% j=1;
% imagesc(im); axis off; hold on; colormap gray; 
% 
% for i=0:k_max-1
%     LL=(L_global_min==i);
%     is_zero=sum(sum(LL));
%     if is_zero
%          img(:,:,1)=img(:,:,1)+LL*c(j,1);
%          j=j+1;
%     end
%     if i~=i_ground
%         color=[rand rand rand];
%         contour(LL,[1 1],'LineWidth',2.5,'Color',color); hold on;
%     end
% end
% figure(2);
% imagesc(img); axis off;
% end

