function [ finalimg ] = getFinal( oripath,img1path,img2path ) %1:sky,2:ground

%This code implements multi-region graph cut image segmentation according
%to the kernel-mapping formulation in M. Ben Salah, A. Mitiche, and 
%I. Ben Ayed, Multiregion Image Segmentation by Parametric Kernel Graph
%Cuts, IEEE Transactions on Image Processing, 20(2): 545-557 (2011).
%The code uses Veksler, Boykov, Zabih and Kolmogorov抯 implementation
%of the Graph Cut algorithm. Written in C++, the graph cut algorithm comes
%bundled with a MATLAB wrapper by Shai Bagon (Weizmann). The kernel mapping
%part was implemented in MATLAB by M. Ben Salah (University
%of Alberta). If you use this code, please cite the papers mentioned in the
%accompanying bib file (citations.bib).

%%%%%%%%%%%%%%%%%%%%%%%%%%%Requirements%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%This code was tested with:
% ?MATLAB Version: 7.12.0.635 (R2011a) for 32-bit wrapper
% ?Microsoft Visual C++ 2010 Express

%%%%%%%%%%%%%%%%%%%Generating the mex files in MATLAB%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%>>mex -g GraphCutConstr.cpp graph.cpp GCoptimization.cpp Graph-
%Cut.cpp LinkedBlockList.cpp maxflow.cpp

%>>mex -g GraphCutMex.cpp graph.cpp GCoptimization.cpp GraphCut.cpp
%LinkedBlockList.cpp maxflow.cpp



%%%%%%%%%%%%%%%%%%%%%%%Main inputs and parameters%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%Note: The RBF-kernel parameters are given in function kernel RBF.m%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%Example with a color image%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%path = 'Images\Color_image.jpg';
%im = im2double(imread(path)); 
%alpha=1; %The weight of the smoothness constraint
%k =8; %The number of regions

 


%%%%%%%Example with a SAR image corrupted with a multiplicative noise%%%%%%
%%%%%%%%%%%%%%%%Uncomment the following to run the example)%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

path = oripath;
im = im2double(imread(path)); %注意，这里图像的大小会发生变化。
im=imresize(im,[750 1000]);  %要记得进行resize
alpha=13.0;%可以避免零零散散的分离感觉
k =2;

%%%%%%%%%%%%%%%%%%%%%%%%%%Example with a brain image%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%Uncomment the following to run the example)%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%path = 'Images\Brain_image.tif';
%im = im2double(imread(path));
%alpha=0.1;
%k =4;


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

distance = 'sqEuclidean'; % Feature space distance

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
    %% The smoothness term
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
        %---------------------------------
        %       Begin updating labels
        %---------------------------------
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

%Show the results

if size(im, 3)==3 % Color image 
img=zeros(sz(1),sz(2),3);%用于生成黑白的标记图,都为0
j=1;
% imagesc(im); axis off; hold on; %用于在im图上画出线

for i=0:k_max-1 %k_max也就是要分成几个区域
    %L_global_min，保存了分层的结果，第一层的像素值是0，第二层的像素值是1，第k层是k-1；(此时在show result部分是不变的，且imagesc是一个有轮廓的图来的)
    %LL是逻辑值，循环地对LL进行求逻辑值的目的是使得属于这一层的像素值为1（白色），不是这一层的像素值为0（黑色）
    LL=(L_global_min==i); %%LL,属于这一层的像素值为1（白色），不是这一层的像素值为0（黑色）
    is_zero=sum(sum(LL));%首先对矩阵LL每一列进行求和，得到一个行矩阵。接着对这个行矩阵求和得到一个int数值。目的是看看当前层是否存在。
    %if is_zero  %目的是看看当前层是否存在。
    %下面开始生成一个有k层的图像img，他会因原图像im的颜色而改变。。
         img(:,:,1)=img(:,:,1)+LL*c(j,1); %c矩阵，k行3列 ; 矩阵与常数相乘，矩阵的每一个元素都与这个常数相乘
         img(:,:,2)=img(:,:,2)+LL*c(j,2);
         img(:,:,3)=img(:,:,3)+LL*c(j,3);
         j=j+1;
    %end
    if i~=i_ground
        color=[rand rand rand];
        contour(LL,[1 1],'LineWidth',2.5,'Color',color); hold on;
    end
end
% figure(2);
% imagesc(img); axis off;
end

im1 = im2double(imread(img1path)); 
im2 = im2double(imread(img2path)); 
if(LL(1,1)==1)
    LL=~LL;
end

finalimg=fuse(im1,im2,LL);
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




end

