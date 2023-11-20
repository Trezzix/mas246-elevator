clc;close all; clear;
data = readtable("zieglers_output.csv",'NumHeaderLines',0);
datarows = 1:(height(data)-1);
t = table2array(data(datarows,"time"));
v = table2array(data(datarows,"speed"));
sample_rate = 500; %500[Hz] = 1/0.002[s] <--2ms
filtering_freq = 0.000000000001;
v_filter = lowpass(v,filtering_freq,sample_rate); %v=v

figure
plot(t,v_filter)