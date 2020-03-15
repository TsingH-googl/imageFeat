function [ finalimg ] = getFinal( oripath,img1path,img2path ) %1:sky,2:ground

%This code implements multi-region graph cut image segmentation according
%to the kernel-mapping formulation in M. Ben Salah, A. Mitiche, and 
%I. Ben Ayed, Multiregion Image Segmentation by Parametric Kernel Graph
%Cuts, IEEE Transactions on Image Processing, 20(2): 545-557 (2011).
%The code uses Veksler, Boykov, Zabih and Kolmogorov�s implementation
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
im = im2double(imread(path)); %ע�⣬����ͼ��Ĵ�С�ᷢ���仯��
im=imresize(im,[750 1000]);  %Ҫ�ǵý���resize
alpha=13.0;%���Ա�������ɢɢ�ķ���о�
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
img=zeros(sz(1),sz(2),3);%�������ɺڰ׵ı��ͼ,��Ϊ0
j=1;
% imagesc(im); axis off; hold on; %������imͼ�ϻ�����

for i=0:k_max-1 %k_maxҲ����Ҫ�ֳɼ�������
    %L_global_min�������˷ֲ�Ľ������һ�������ֵ��0���ڶ��������ֵ��1����k����k-1��(��ʱ��show result�����ǲ���ģ���imagesc��һ����������ͼ����)
    %LL���߼�ֵ��ѭ���ض�LL�������߼�ֵ��Ŀ����ʹ��������һ�������ֵΪ1����ɫ����������һ�������ֵΪ0����ɫ��
    LL=(L_global_min==i); %%LL,������һ�������ֵΪ1����ɫ����������һ�������ֵΪ0����ɫ��
    is_zero=sum(sum(LL));%���ȶԾ���LLÿһ�н�����ͣ��õ�һ���о��󡣽��Ŷ�����о�����͵õ�һ��int��ֵ��Ŀ���ǿ�����ǰ���Ƿ���ڡ�
    %if is_zero  %Ŀ���ǿ�����ǰ���Ƿ���ڡ�
    %���濪ʼ����һ����k���ͼ��img��������ԭͼ��im����ɫ���ı䡣��
         img(:,:,1)=img(:,:,1)+LL*c(j,1); %c����k��3�� ; �����볣����ˣ������ÿһ��Ԫ�ض�������������
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




end

