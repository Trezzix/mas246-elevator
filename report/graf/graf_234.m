clc;close all; clear;
data = readtable("move_234.csv",'NumHeaderLines',0);
datarows = 1:1315;
t = table2array(data(datarows,"time"));
height = table2array(data(datarows,"current_height"));
error = table2array(data(datarows,"error"));
pid = table2array(data(datarows,"u"));
acceleration = table2array(data(datarows,"acceleration"));
motor = table2array(data(datarows,"motor"));
height_req(1:443) = 5;
height_req(444:886) = 10;
height_req(887:1315) = 15;
height_req = height_req';

%plotting
figure;
xlabel('time (ms)')
plotyyy(t,error,t,acceleration,t,motor,["Height error (m)" "Acceleration (m/s^2)" "Motor speed (m/s)"])
cfig = get(gcf,'color');
%plot(t,error,'LineWidth', 1)
hold on
%plot(t,acceleration,'LineWidth', 1)
%plot(t,motor,'LineWidth', 1)
%ylabel('specific data unit')
%yyaxis right
%plot(t,pid,'LineWidth', 1)
%ylabel('PID')

grid on
%legend('Height error (m)','Acceleration (m/s^2)','Motor speed(m/s)','PID output');

title('DC Motor movement from increasing floor requests');
hold off

figure;
plot(t,height_req,'LineWidth', 1)
hold on
plot(t,height,'LineWidth', 1)
plot(t,error,'LineWidth', 1)
ylabel('height (m)')
yyaxis right
plot(t,motor,'LineWidth', 1)
ylabel('motor speed (m/s)')
grid on
legend('Requested height (m)','Elevator height (m)', 'Height error (m)', 'motor (m/s)');
xlabel('time (ms)')
title('Movement of the elevator from the first floors with the requested floors: 2,3,4');