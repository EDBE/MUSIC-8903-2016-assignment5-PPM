# MUSIC-8903-2016
Repo for the class Audio Software Engineering Spring 2016

We split the assignment 5 into two parts. Part 1 is the implementation and tests for the Peak Program Meter class. In part 2, we integrate tested PPM class into vibrato plugin which we created in assignment 4. Due to these two parts are relatively seperate to each other, we have two repos for assignment 5. 

## Assignment 5 Part 1: Peak Program Meter
### 1, Add a peak meter (PPM) to your Vibrato Library

The processor of PPM takes the audio buffer and its length as input, then does the calculation based on the equation shown on page 76 of Audio Content Analysis book. It is able to work for multi-channel buffer. For each process call, the class saves the peak of each channel in a float array (numOfChannel * 1). This array is member variable. Whenever the caller needs the peak values, it uses getPeak function for each channel.

### 2, Add 6 tests to Test_Vibrato to verify correct PPM functionality

Test 1: Zero input

Test 2: DC input

Test 3: Sine input

Test 4: Saw tooth input

Test 5: Audio sample input

Test 6: Impulse input

The PPM passed all the tests.

### 3, Implement a rectangular MeterComponent and add it to your GUI

### 4, Integrate the PPM into your PlugInProcessor

### 5, Add a Timer to your PluginEditor that is called every 10ms