# AudioVisualiser

This is curently being rewirtten in the main branch to work on the nano RP2040

### Program to control multiple LED-Strips with an Arduino nano (every) using a microphone as input

**micInput:** Reads microphone and outputs the smoothed current and average Volume level

**strip:** Resembles one LED strip and has different Modes that change the LEDs according to micInputs outputs

Automatically switches into an off Mode if average Volume drops under specific Threshold 

Limits updates/s to ~80 for consistency

TODO: add control via Remote


| Jun. 2022 | Sep. 2023 |
|:-------------:|:-------------:|
|  ![](Videos/02_sub_only.jpg) | ![](Videos/10_all_small.jpg)  |


#### More Videos + Sound in [Videos folder](./Videos)

|  ![](Videos/08_both_cut.gif) | ![](Videos/05_both_olivia_cut.gif)  |
| ------------- | ------------- |
