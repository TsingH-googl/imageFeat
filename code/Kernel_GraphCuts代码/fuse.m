function [ fuse_result ] = fuse( seg1,seg2,LL )%seg1Ϊ��գ�seg2Ϊ����
%UNTITLED2 �˴���ʾ�йش˺�����ժҪ
%   �˴���ʾ��ϸ˵��
sz = size(seg1);
fuse_result=zeros(sz(1),sz(2),3);
length=sz(1);
width=sz(2);
for i=1:length
    for j=1:width
        if(LL(i,j)==0) %�鿴LL֪����skyΪ0��groundΪ1.
            fuse_result(i,j,1)=seg1(i,j,1);
            fuse_result(i,j,2)=seg1(i,j,2);
            fuse_result(i,j,3)=seg1(i,j,3);
        else
            fuse_result(i,j,1)=seg2(i,j,1);
            fuse_result(i,j,2)=seg2(i,j,2);
            fuse_result(i,j,3)=seg2(i,j,3);
        end
    end

end

