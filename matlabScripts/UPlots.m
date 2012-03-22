clear

%% get some plot import data

[inputFile, z_lower, z_upper, R, cuts] = textread('plotControl.dat','%s %f %f %f %u', 1);

%% import U
Uxyz = importdata(inputFile);


%% import y
% yfile = '~/Desktop/researchData/postProcessingDataRep1500/yPoints.dat';
% y = importdata(yfile);

%% Convert to cylindrical coodinates 
% from http://staffweb.cms.gre.ac.uk/~ct02/research/thesis/node26.html
% from http://mathworld.wolfram.com/CylindricalCoordinates.html

uSize = size(Uxyz);

% only extract data between certain values of z_lower and z_upper
% get a count of the data in this range
zDataCount = 0;
for i=1:uSize(1)
    if( (Uxyz(i,3) <= z_upper) && (Uxyz(i,3) >= z_lower))
        zDataCount = zDataCount + 1;
    end
        
end

%Urtz = ones(uSize(1),7);
Urtz = ones(zDataCount,7);

%traverses new data with this counter
j = 0;

for i=1:uSize(1)
    
    if( (Uxyz(i,3) <= z_upper) && (Uxyz(i,3) >= z_lower))
    
    % r = sqrt(x^2+y^2)
    Urtz(j,1) = sqrt(Uxyz(i,1)*Uxyz(i,1) + Uxyz(i,2)*Uxyz(i,2));
    % theta = tan^-1 (y/x)
    Urtz(j,2) = atan(Uxyz(i,2)/Uxyz(i,1));
    % z = z
    Urtz(j,3) = Uxyz(i,3);  
    % U_r 	= U_x cos(\theta) + U_y sin(\theta)
    Urtz(j,4) = Uxyz(i,4)*cos(Urtz(i,2)) + Uxyz(i,5)*sin(Urtz(i,2));
    % U_\theta= -U_x sin(\theta) + U_y cos(\theta)
    Urtz(j,5) = -Uxyz(i,4)*sin(Urtz(i,2)) + Uxyz(i,5)*cos(Urtz(i,2));    
    % U_z = U_z
    Urtz(j,6) = Uxyz(i,6);
    Urtz(j,7) = sqrt(Urtz(i,4)*Urtz(i,4) + Urtz(i,5)*Urtz(i,5) + Urtz(i,6)*Urtz(i,6));  
    
    j = j +1;
    end
end

%% Interpolate 1D data to r_interpolate and send to nearest neighbor

r_interpolate = linspace(min(Urtz(:,1)), max(Urtz(:,1)), cuts);

% interpoloate the cylindrical velocity components to r distance only
Urtz_ri = ones(uSize(1),5);

for i=1:zDataCount
tempDist = 100;
minPosition = 1;

   for j=1:cuts
       if (abs(Urtz(i,1)-r_interpolate(j))<tempDist)
           minPosition = j;
           tempDist = abs(Urtz(i,1)-r_interpolate(j));
       end
   
   end
   
   Urtz_ri(i,1) = r_interpolate(minPosition);
   Urtz_ri(i,2) = Urtz(i,4);
   Urtz_ri(i,3) = Urtz(i,5);
   Urtz_ri(i,4) = Urtz(i,6);
   Urtz_ri(i,5) = Urtz(i,7);
end

Urtz_avg = ones(cuts,5);

for i=1:cuts
    sumTotalR = 0;
    sumTotalTheta = 0;
    sumTotalZ = 0;
    sumTotalMag = 0;
    count = 0;
    
    for j=1:zDataCount
    
        if(Urtz_ri(j,1) == r_interpolate(i))
           sumTotalR = sumTotalR + Urtz_ri(j,2);
           sumTotalTheta = sumTotalTheta + Urtz_ri(j,3);
           sumTotalZ = sumTotalZ + Urtz_ri(j,4);
           sumTotalMag = sumTotalMag + Urtz_ri(j,5);
           count = count + 1;
        end
    end
    
    Urtz_avg(i,1) = r_interpolate(i);
    Urtz_avg(i,2) = sumTotalR/count;
    Urtz_avg(i,3) = sumTotalTheta/count;
    Urtz_avg(i,4) = sumTotalZ/count;
    Urtz_avg(i,5) = sumTotalMag/count;
end

%% Plot averaged interpolated data

h=figure;
scatter(Urtz_avg(:,1),Urtz_avg(:,2),'.');
title('Radial Velocity vs. Radial Distance')
ylabel('U_r [m/s]');
xlabel('r [m]');
print(h,'-dpdf','Urvsr.pdf');


h=figure;
scatter(Urtz_avg(:,1),Urtz_avg(:,3),'.');
title('Azimuthal Velocity vs. Radial Distance')
ylabel('U_\theta [m/s]');
xlabel('r [m]');
print(h,'-dpdf','Uthetavsr.pdf');

h=figure;
scatter(Urtz_avg(:,1),Urtz_avg(:,4),'.');
title('Z-Velocity vs. Radial Distance')
ylabel('U_Z [m/s]');
xlabel('r [m]');
print(h,'-dpdf','Uzvsr.pdf');


h=figure;
scatter(Urtz_avg(:,1),Urtz_avg(:,5),'.');
title('Velocity Magnitude vs. Radial Distance')
ylabel('|U| [m/s]');
xlabel('r [m]');
print(h,'-dpdf','Umagvsr.pdf');

%% Dimensionless Plots
meanUr= mean(Urtz_avg(:,2));
meanUtheta= mean(Urtz_avg(:,3));
meanUz= mean(Urtz_avg(:,4));
meanUmag= mean(Urtz_avg(:,5));

Urtz_dim = Urtz_avg;

Urtz_dim(:,1) = Urtz_avg(:,1)./0.03;
Urtz_dim(:,2) = Urtz_avg(:,2)./abs(meanUr);
Urtz_dim(:,3) = Urtz_avg(:,3)./abs(meanUtheta);
Urtz_dim(:,4) = Urtz_avg(:,4)./abs(meanUz);
Urtz_dim(:,5) = Urtz_avg(:,5)./abs(meanUmag);


h=figure;
scatter(Urtz_dim(:,1),Urtz_dim(:,2),'.');
title('Dimensionless Radial Velocity vs. Radial Distance')
ylabel('U_r [-]');
xlabel('r/d [-]');
print(h,'-dpdf','Urvsr_dim.pdf');

h=figure;
scatter(Urtz_dim(:,1),Urtz_dim(:,3),'.');
title('Dimensionless Azimuthal Velocity vs. Radial Distance')
ylabel('U_\theta [-]');
xlabel('r/d [-]');
print(h,'-dpdf','Uthetavsr_dim.pdf');

h=figure;
scatter(Urtz_dim(:,1),Urtz_dim(:,4),'.');
title('Dimensionless Z-Velocity vs. Radial Distance')
ylabel('U_Z [-]');
xlabel('r/d [-]');
print(h,'-dpdf','Uzvsr_dim.pdf');

h=figure;
scatter(Urtz_dim(:,1),Urtz_dim(:,5),'.');
title('Dimensionless Velocity Magnitude vs. Radial Distance')
ylabel('|U| [-]');
xlabel('r/d [-]');
print(h,'-dpdf','Umagvsr_dim.pdf');















%Urtz_ri2 = Urtz_ri;

% Interpolate U_r
%Urtz_ri2(:,2) = interp1(Urtz(:,1),Urtz(:,4),Urtz_ri2(:,1));

% Interpolate U_\theta
%Urtz_ri2(:,3) = interp1(Urtz(:,1),Urtz(:,5),Urtz_ri2(:,1));

% Interpolate U_z
%Urtz_ri2(:,4) = interp1(Urtz(:,1),Urtz(:,6),Urtz_ri2(:,1));

% Interpolate |U|
%Urtz_ri2(:,5) = interp1(Urtz(:,1),Urtz(:,7),Urtz_ri2(:,1));

% theta_i = linespace();
% r_i
% z_i
%% plot at specific z-value

%zplaneCount = 0;

%for i=1:uSize(1)
%    if and(U(i,3)>0.1, U(i,3)<0.11 )
%        zplaneCount = zplaneCount + 1;
%    end
%end

%Urz = ones(zplaneCount, 2);
%counter = 0;
%for i=1:uSize(1)
%    if and(U(i,3)>0.1, U(i,3)<0.11 )
%        counter = counter + 1;
%        Urz(counter,1) = (U(i,1)*U(i,1) + U(i,2)*U(i,2))^(0.5);
%        Urz(counter,2) = (U(i,4)*U(i,4) + U(i,5)*U(i,5) + U(i,6)*U(i,6))^0.5;
        
%    end
%end




%% Umag vs r value plot
%figure;
%scatter(magUvsr(:,1),magUvsr(:,2));
%xlabel('r [m]');
%ylabel('|U| [m/s]');
%title('Umag vs. distance from bed center');


%% Umag vs y value plot
%figure;
%scatter(y(:,4),magUvsr(:,2));
%xlabel('y [m]');
%ylabel('|U| [m/s]');
%title('Umag vs. perpendicular distance from nearest no-slip');

%%



