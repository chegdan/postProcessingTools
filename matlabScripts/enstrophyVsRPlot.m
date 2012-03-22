%clear

%% import Q
% Q is a scalar, so we don't need as much information as before
% got lazy and didnt change much from the helicity m file

Hfile = './pointData/enstrophy4500.dat';
Hxyz = importdata(Hfile);


%% import y
% yfile = '~/Desktop/researchData/postProcessingDataRep1500/yPoints.dat';
% y = importdata(yfile);

%% Convert to cylindrical coodinates 
% from http://staffweb.cms.gre.ac.uk/~ct02/research/thesis/node26.html
% from http://mathworld.wolfram.com/CylindricalCoordinates.html

hSize = size(Hxyz);
Hrtz = ones(hSize(1),5);

for i=1:hSize(1)
    % r = sqrt(x^2+y^2)
    Hrtz(i,1) = sqrt(Hxyz(i,1)*Hxyz(i,1) + Hxyz(i,2)*Hxyz(i,2));
    % theta = tan^-1 (y/x)
    Hrtz(i,2) = atan(Hxyz(i,2)/Hxyz(i,1));
    % z = z
    Hrtz(i,3) = Hxyz(i,3);  
    % H = H
    Hrtz(i,4) = Hxyz(i,4);
    
end


%% Interpolate 1D data to r_interpolate and send to nearest neighbor
cuts = 1000;
% choose 500 points from min to max r values
r_interpolate = linspace(min(Hrtz(:,1)), max(Hrtz(:,1)), cuts);

% interpoloate the cylindrical Vorticity components to r distance only, 
% drop theta and z
Hrtz_ri = ones(hSize(1),2);

for i=1:hSize(1)
tempDist = 100;
minPosition = 1;

   for j=1:cuts
       if (abs(Hrtz(i,1)-r_interpolate(j))<tempDist)
           minPosition = j;
           tempDist = abs(Hrtz(i,1)-r_interpolate(j));
       end
   
   end
   
   Hrtz_ri(i,1) = r_interpolate(minPosition);
   Hrtz_ri(i,2) = Hrtz(i,4);
end

Hrtz_avg = ones(cuts,2);

for i=1:cuts
    sumTotalH = 0;
    %sumTotalTheta = 0;
    %sumTotalZ = 0;
    %sumTotalMag = 0;
    count = 0;
    
    for j=1:hSize(1)
    
        if(Hrtz_ri(j,1) == r_interpolate(i))
           sumTotalH = sumTotalH + Hrtz_ri(j,2);
           count = count + 1;
        end
    end
    
    Hrtz_avg(i,1) = r_interpolate(i);
    Hrtz_avg(i,2) = sumTotalH/count;
end

%% Plot averaged interpolated data

h=figure;
scatter(Hrtz_avg(:,1),Hrtz_avg(:,2),'.');
title('Enstrophy vs. Radial Distance')
ylabel('Enstrophy [1/s^2]');
xlabel('r [m]');
print(h,'-dpdf','enstrophyvsr.pdf');


%% Dimensionless Plots
meanH= mean(Hrtz_avg(:,2));

Hrtz_dim = Hrtz_avg;

Hrtz_dim(:,1) = Hrtz_avg(:,1)./0.03;
Hrtz_dim(:,2) = Hrtz_avg(:,2)./abs(meanH);


h=figure;
scatter(Hrtz_dim(:,1),Hrtz_dim(:,2),'.');
title('Dimensionless Enstrophy vs. Dimensionless Radial Distance')
ylabel('Enstrophy [-]');
xlabel('r/d [-]');
print(h,'-dpdf','enstrophyvsr_dim.pdf');

