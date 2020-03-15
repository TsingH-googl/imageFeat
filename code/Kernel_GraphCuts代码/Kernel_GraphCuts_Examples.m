% 此代码是salah等人提出的kernel graph cuts图像分割算法的主要部分的改写。
% 改写的目的是使得航拍有雾图像分割为天空区域和非天空区域两个区域，并且两个区域的子图像是连续的。

clear all; close all; 

% 注意，如果有一些图像是没有天空的，即整幅图像的属性基本一致，则程序不能划分为2个区域，程序会会出错。
path = 'D:\图像分割\Images\雾霾数据库\DJI_0059.JPG';
im = im2double(imread(path)); %注意，这里图像的大小会发生变化。
im=imresize(im,[750 1000]);
alpha=8;% 用于调节能量方程的数据项和光滑项的正系数，写论文要说明这个参数的作用。
k =2;   % 总的分割区域数


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

distance = 'sqEuclidean'; % 特征空间距离

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
    %% 平滑项
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

% 显示标记矩阵以及分割的结果
if size(im, 3)==3 % Color image 
img=zeros(sz(1),sz(2),3);%用于生成黑白的标记矩阵
j=1;
imshow(im); axis off; hold on; %用于在im图上画出颜色分割线


for i=0:k_max-1 %k_max也就是要分成几个区域
    %L_glo bal_min，保存了分层的结果，第一层的像素值是0，第二层的像素值是1，第k层是k-1；(此时在show result部分是不变的，且imagesc是一个有轮廓的图来的)
    %LL是逻辑值，循环地对LL进行求逻辑值的目的是使得属于这一层的像素值为1（白色），不是这一层的像素值为0（黑色）（如果分割为多个区域则应在代码中设置颜色随机）
    LL=(L_global_min==i); % LL,属于这一层的像素值为1（白色），不是这一层的像素值为0（黑色）。
    is_zero=sum(sum(LL));
    %if is_zero  %目的是看看当前层是否存在。
    %下面开始生成一个有k层的图像img，他会因原图像im的颜色而改变。。
         img(:,:,1)=img(:,:,1)+LL*c(j,1); %c矩阵，k行3列 ; 矩阵与常数相乘，矩阵的每一个元素都与这个常数相乘
         img(:,:,2)=img(:,:,2)+LL*c(j,2);
         img(:,:,3)=img(:,:,3)+LL*c(j,3);
         j=j+1;
    %end
    if i~=i_ground
        color=[0.901  0.99 0];%图像分割线的颜色，这里是亮黄色。0<=channel<=1
        contour(LL,[1 1],'LineWidth',6.5,'Color',color); hold on;
    end
end
figure(2);
imagesc(img); axis off;%在第二个窗口figure（2）中显示标记矩阵图img
end

% 显示分割结果
[sky ground] = seperate(im,LL);

%保存图像，注意这里保存的图像的大小与原来的不一样但是他们的像素点的值以及分辨率都是一样的
% [o1 o2]=seperate(im,LL);
% block= 5;
% [I I_out J T_est T A] = removeHaze( o1, block);
%             temp='-';
%             if(block<10)
%                 temp='-0';
%             end
%             imwrite(uint8(I_out),strcat(save_dir,strcat(strcat(strcat(image_name(1:2),temp),num2str(block)),'.png')));
%%imwrite(im,'C:\Users\406\Desktop\新建文件夹\1.jpg');


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

