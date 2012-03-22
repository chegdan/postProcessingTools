%clear

%% import Vorticity
%must import separately and name Uxyz
%Ufile = './pointData/vorticity4500.dat';
%Uxyz = importdata(Ufile);


%% import y
% yfile = '~/Desktop/researchData/postProcessingDataRep1500/yPoints.dat';
% y = importdata(yfile);

%% Convert to cylindrical coodinates 
% from http://staffweb.cms.gre.ac.uk/~ct02/research/thesis/node26.html
% from http://mathworld.wolfram.com/CylindricalCoordinates.html

uSize = size(Uxyz);
Urtz = ones(uSize(1),7);

for i=1:uSize(1)
    % r = sqrt(x^2+y^2)
    Urtz(i,1) = sqrt(Uxyz(i,1)*Uxyz(i,1) + Uxyz(i,2)*Uxyz(i,2));
    % theta = tan^-1 (y/x)
    Urtz(i,2) = atan(Uxyz(i,2)/Uxyz(i,1));
    % z = z
    Urtz(i,3) = Uxyz(i,3);  
    % U_r 	= U_x cos(\theta) + U_y sin(\theta)
    Urtz(i,4) = Uxyz(i,4)*cos(Urtz(i,2)) + Uxyz(i,5)*sin(Urtz(i,2));
    % U_\theta= -U_x sin(\theta) + U_y cos(\theta)
    Urtz(i,5) = -Uxyz(i,4)*sin(Urtz(i,2)) + Uxyz(i,5)*cos(Urtz(i,2));    
    % U_z = U_z
    Urtz(i,6) = Uxyz(i,6);
    Urtz(i,7) = sqrt(Urtz(i,4)*Urtz(i,4) + Urtz(i,5)*Urtz(i,5) + Urtz(i,6)*Urtz(i,6));  
end

%figure;
%scatter(Urtz(:,1), Urtz(:,7));

%figure;
%scatter(y(:,4), Urtz(:,7))
%Urtz_i = ones(uSize(1),6);

%% Interpolate 1D data to r_interpolate and send to nearest neighbor
cuts = 1000;
% choose 500 points from min to max r values
r_interpolate = linspace(min(Urtz(:,1)), max(Urtz(:,1)), cuts);

% interpoloate the cylindrical Vorticity components to r distance only
Urtz_ri = ones(uSize(1),5);

for i=1:uSize(1)
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
    
    for j=1:uSize(1)
    
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
title('Radial Vorticity vs. Radial Distance')
ylabel('\omega_r [1/s]');
xlabel('r [m]');
print(h,'-dpdf','vorticityrvsr.pdf');

h=figure;
scatter(Urtz_avg(:,1),Urtz_avg(:,3),'.');
title('Azimuthal Vorticity vs. Radial Distance')
ylabel('\omega_\theta [1/s]');
xlabel('r [m]');
print(h,'-dpdf','vorticitythetavsr.pdf');

h=figure;
scatter(Urtz_avg(:,1),Urtz_avg(:,4),'.');
title('Z-Vorticity vs. Radial Distance')
ylabel('\omega_Z [1/s]');
xlabel('r [m]');
print(h,'-dpdf','vorticityzvsr.pdf');


h=figure;
scatter(Urtz_avg(:,1),Urtz_avg(:,5),'.');
title('Vorticity Magnitude vs. Radial Distance')
ylabel('|\omega| [1/s]');
xlabel('r [m]');
print(h,'-dpdf','vorticitymagvsr.pdf');

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
title('Dimensionless Radial Vorticity vs. Radial Distance')
ylabel('\omega_r [-]');
xlabel('r/d [-]');
print(h,'-dpdf','vorticityrvsr_dim.pdf');

h=figure;
scatter(Urtz_dim(:,1),Urtz_dim(:,3),'.');
title('Dimensionless Azimuthal Vorticity vs. Radial Distance')
ylabel('\omega_\theta [-]');
xlabel('r/d [-]');
print(h,'-dpdf','vorticitythetavsr_dim.pdf');


h=figure;
scatter(Urtz_dim(:,1),Urtz_dim(:,4),'.');
title('Dimensionless Z-Vorticity vs. Radial Distance')
ylabel('\omega_Z [-]');
xlabel('r/d [-]');
print(h,'-dpdf','vorticityzvsr_dim.pdf');


h=figure;
scatter(Urtz_dim(:,1),Urtz_dim(:,5),'.');
title('Dimensionless Vorticity Magnitude vs. Radial Distance')
ylabel('|\omega| [-]');
xlabel('r/d [-]');
print(h,'-dpdf','vorticitymagvsr_dim.pdf');

















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



