%function [I J T A L] = removeHaze( imageName, patch_size )
function [I I_out J T_est T A] = removeHaze( imageName, patch_size )
%REMOVEHAZE Summary of this function goes here
%   Detailed explanation goes here

    imageName = strcat(imageName);

    % Used to make image more realistic due to seeming unbelievable
    % when there is no sense of depth
    aerialPerspective = 0.95;

    I = imread(imageName);
%     if max(max(max(size(I)))) > 768
%         scale = 768 / max(max(max(size(I))));
%         I = imresize(I,scale);
%     end

%¶ÔÍ¼Ïñ½øÐÐresize
%I=imresize(I,[750 1000]);

    I = double(I) ./ 255;
    
    % Make grayscales to color
    if numel(size(I)) == 2
        [x y] = size(I);
        tmpI = zeros(x,y,3);
        for c = 1:3
            tmpI(:,:,c) = I;
        end
        I = tmpI;
    end
    
    
    J = mymakeDarkChannel(I);
    
    % Section 4.4
    %   Estimate Atmosphere
    
    %  We first pick the top 0.1% bright- est pixels in the dark channel.
    %  These pixels are most haze- opaque (bounded by yellow lines in 
    %  Figure 6(b)). Among these pixels, the pixels with highest intensity 
    %  in the input image I is selected as the atmospheric light. 
    
    
    %
    % TL;DR  TAKE .1% of the brightest pixels
    dimJ = size(J);
    numBrightestPixels = ceil(0.001 * dimJ(1) * dimJ(2)); % Use the cieling to overestimate number needed
    
    A = estimateA(I,J,numBrightestPixels);
    
    
    % Section 4.1
    %   Estimate the Transmission
    %   Equation 12
    T_est = 1 - aerialPerspective*makeDarkChannel(I./A,patch_size);
    
    [T] = generateLaplacian(I,T_est);
    %[L T] = generateLaplacian(I,3, T_est);
    
    dehazed = zeros(size(I));

    % Equation 16
    for c = 1:3
        dehazed(:,:,c) = (I(:,:,c) - A(:,:,c))./(max(T, .1)) + A(:,:,c);
    end
    
     I_out = dehazed;
      mi= prctile(I_out(:),1);
  ma= prctile(I_out(:),99);

  I_out(:,:,1)=(I_out(:,:,1)-mi)*255/(ma-mi);
          
  I_out(:,:,2)=(I_out(:,:,2)-mi)*255/(ma-mi);
        
  I_out(:,:,3)=(I_out(:,:,3)-mi)*255/(ma-mi);

    beep
end

