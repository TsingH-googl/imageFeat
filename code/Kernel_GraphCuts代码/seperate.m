function [ sky,ground ] = seperate( im,LL ) 
%UNTITLED 此处显示有关此函数的摘要
%   此处显示详细说明
sz = size(im);
sky=zeros(sz(1),sz(2),3);
ground=zeros(sz(1),sz(2),3);
length=sz(1);
width=sz(2);
for i=1:length
    for j=1:width
        if(LL(i,j)==1)
            ground(i,j,1)=im(i,j,1);
            ground(i,j,2)=im(i,j,2);
            ground(i,j,3)=im(i,j,3);
        else
            sky(i,j,1)=im(i,j,1);
            sky(i,j,2)=im(i,j,2);
            sky(i,j,3)=im(i,j,3);
        end
    end
end
            
end

