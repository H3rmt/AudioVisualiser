# AudioVisualiser

### Program to control multiple LED-Strips with an Arduino nano (every) using a microphone as input

**micInput:** Reads microphone and outputs the smoothed current and average Volume level

**strip:** Resembles one LED strip and has different Modes that change the LEDs according to micInputs outputs

Automatically switches into an off Mode if average Volume drops under specific Threshold 

Limits updates/s to ~80 for consistency

TODO: add control via Remote

---

![](Videos/02_sub_only.jpg)

#### More Videos + Sound in Videos folder 

<div style="display:flex;gap:8px;flex-direction:row;width:100%;overflow:auto;">
  <img src="./Videos/08_both_cut.gif" style="width:calc(50% - 4px);">
  <img src="./Videos/05_both_olivia_cut.gif" style="width:calc(50% - 4px);">
</div>
