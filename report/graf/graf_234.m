clc;close all; clear;

% Export variables and lists

data = readtable("move_1232.csv",'NumHeaderLines',0);
datarows = 1:height(data)-1;
t = table2array(data(datarows,"time"));
height = table2array(data(datarows,"current_height"));
error = table2array(data(datarows,"error"));
pid = table2array(data(datarows,"u"));
acceleration = table2array(data(datarows,"acceleration"));
motor = table2array(data(datarows,"current_speed"));
height_req = table2array(data(datarows,"req_height"));

% Plotting

plotyyy(t,error,t,acceleration,t,motor,["Height error (m)" "Acceleration (m/s^2)" "Motor speed (m/s)"])
cfig = get(gcf,'color');
hold on
grid on
title('Servo motor movement with requested floors: 1,2,3,2');


figure
plot(t,height_req,'LineWidth', 1)
hold on
plot(t,height,'LineWidth', 1)
plot(t,error,'LineWidth', 1)
ylabel('height (m)')
ylim([-7 12.5])

yyaxis right
ylim([-1.4 2.5])
plot(t,motor,'LineWidth', 1)
ylabel('motor speed (m/s)')
grid on
legend('Requested height (m)','Elevator height (m)', 'Height error (m)', ...
       'motor (m/s)', 'Location','southwest');
xlabel('time (ms)')
title('Elevator movement from first floors with requested floors: 1,2,3,2');