# SimpleDyno HS

Here is the continued development of SimpleDyno software, initially done by DamoRC and continued by Lassi Niemistö.

I added the HS suffix to indicate High Speed, which means that it can correctly support higher roller pulse rates.

# History and background

This project started when there was a discussion on Finnish kotiverstas.com forum and on PV-Foorumi.org setting up 
dyno to measure moped engine power. It was discovered that when roller RPM reached a certain limit, a strong 
artifact appeared and made measurement unusable beyond that RPM upwards. The artifact looks like this on SimpleDyno 
analysis view:

![Pulse skip effect](https://github.com/jahonen75/SimpleDynoHS/assets/167219330/ce848e70-4b21-454b-8630-ce7284466ffc)

Various investigations were then done by me, and it took a while to discover the reason. One reason was that it 
was initially suspected that it is the Arduino part which had the problem. Also, this issue only occurs on power run
function, and can't be seen on the realtime display, which works correctly in all situations.

This effect can be pushed upwards using such microcontroller with integrated USB, so serial bit rate limitation 
can be lifted. But eventually, ultimately it is inevitable, and it is better to fix the actual problem.

## Motivation

Thus the main motivation here is to support high pulse rates from Arduino, to get power run function to work properly in 
situation where all the pulses from Arduino can not be sent over serial interface and some pulses are skipped. This is 
especially problematic when using more trigger pulses per roller rotation.

The reason is that the pulses come in irregular intervals, and are spaced like this when seen in linear time axis:

![SD_Real_Data](https://github.com/jahonen75/SimpleDynoHS/assets/167219330/e82bf02b-f386-4117-843c-11a4a95ea0f3)

But Moving Average smoothing does not use time axis, and thus linearly rising, but irregularly spaced data looks like this:

![SD_Seen_By_MA-Smooth](https://github.com/jahonen75/SimpleDynoHS/assets/167219330/42d0ee2a-50ba-45ca-a61e-b96fd7678453)

When Moving average smooths this data, it basically does not consider the time and transfers unevenness to the slope of the 
roller RPM, which is the source of the torque and power calculations, ruining the result.

The fix is that instead of MA smooth, user has now option to disable the fitting altogether and just use the raw data as is, 
or use Moving least squares linear fitting, which tries to fit a straight line to a small section of the input data, and 
reconstruct the data from there.

## Changes done so far

The following main changes that have been done so far after Lassi Niemistö:

1. Provide alternate methods to smooth the raw data in fitting phase which work at high speed
2. Make time stamps of the recorded data to be at the center of the measured pulse interval
3. Shut down the application correctly without dangling thread for simulation runs (for development)

## The Arduino sketch using timer capture

In arduino folder of this repository, there is an improved sketch (SimpleDyno_Capture.ino) to capture roller pulses with as high time resolution
as possible with Arduino UNO. This uses timer capture on pin 8. It outputs time as decimal and has time resolution of 62.5 ns (1/16 MHz). 
So it is 16 times better than with original implementation.
Also because it uses timer capture function on the Arduino UNO AVR microcontroller, it does not suffer from any interrupt latency 
variation whatsoever. 


