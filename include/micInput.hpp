//
// Created by enrico on 17.07.22.
//

#ifndef _MICINPUT_HPP
#define _MICINPUT_HPP

#include <Arduino.h>

class micinput
{
private:
    // raw read from mic (after centering to right scale)
    uint16_t micValue;

    // current lvl (calculated by calcLvl)
    uint16_t lvl;

    // current maxLvl in lvls array (only used in calcAvg)
    uint16_t maxLvl;
    // current lvlAvg, gets slowly adjusted to maxLvl
    uint16_t lvlAvg;
    
    // current potAvg + lvlAvg / 2 (average of pot and current avg) 
    uint16_t lvlAvgAdj;

    uint8_t micPin;
    uint8_t avgPin;
    int16_t micOffset;
    uint16_t minAvgLvl;
    uint16_t offDelay;
    uint16_t avgLevelFloor;

    // current count of off cycles so far (gets reset if lvlAvg > minAvgLvl)
    uint16_t offCounter;

    /**
     * reads sound value from microphone and centees to right scale
     */
    void readMic()
    {
        int r = analogRead(micPin) - 512 - micOffset;
        micValue = abs(r);
    }

    /**
     * calculates current level based on previous level and new level
     *
     * multiplies by 3 adds micValue and divides by 2 to make
     * changes smoth and prevent flickering (soundValue makes up 1/4 of new value)
     */
    void calcLvl()
    {
        // fall and rise fast
        lvl = (lvl + micValue) >> 1;
    }

    /**
     * calculates new max level and adjustes lvlAvg
     * adjustst old levels by multiplying by 63 and dividing by >> 6
     */
    void calcAvg()
    {
        // dump down a bit, allows level to rise above and hit end of LED strip
        maxLvl = ((maxLvl * 3) + lvl) >> 2;
        uint16_t cMaxLvl = maxLvl - (maxLvl >> 5);

        // fall faster, rise slower
        if (cMaxLvl > lvlAvg)
            lvlAvg = (lvlAvg * 31 + cMaxLvl) >> 5;
        else
            lvlAvg = (lvlAvg * 15 + cMaxLvl) >> 4;

        lvlAvgAdj = ((lvlAvg * 2) + analogRead(avgPin)) / 3;
    }

    void checkOff()
    {
        // Serial.print(lvl);
        // Serial.print(" - ");
        // Serial.print(maxLvl);
        // Serial.print(" - ");
        // Serial.print(lvlAvg);
        // Serial.print(" - ");
        // Serial.println(lvlAvgAdj);
        
        // check if lvlAvg is smaller that set Threshold
        if (lvlAvg <= minAvgLvl)
        {
            offCounter++;
            // set lvl to 0 if offCounter reached offDelay Threshold
            if (offCounter >= offDelay)
            {
                lvl = 0;
            }
        }
        else
        {
            offCounter = 0;
        }
    }

public:
    /**
     * micPin         pin of mic
     * micOffset      offset to adjust Lvl of mic to be axactly 0 when quiet
     *                arround -100 for 3,3V, as 3,3V mic level cant get centered to 0 with 512, instead ~412
     * offDelay       count of iteratios with AvgLvl < minAvgLvl bevore level gets set to 0
     * minAvgLvl      min level of AvgLvl bevore off counter gets incremented
     * avgLevelFloor  avgLvl cant get lower than this, to prevent flickering if quiet (only gets floored on the output, internal value stays)
     */
    explicit micinput(uint8_t micPin, uint8_t avgPin, int16_t micOffset = 0, uint8_t minAvgLvl = 40, uint16_t offDelay = 30, uint16_t avgLevelFloor = 120)
    {
        this->micPin = micPin;
        this->avgPin = avgPin;
        this->micOffset = micOffset;
        this->minAvgLvl = minAvgLvl;
        this->offDelay = offDelay;
        this->avgLevelFloor = avgLevelFloor;
    }

    /**
     * initialises this Micinput, sets input and default values
     */
    void init()
    {
        pinMode(micPin, INPUT);

        lvl = 10;
        maxLvl = 10;
        lvlAvg = 100;
        offCounter = offDelay;
    }

    /**
     * returns current lvl
     */
    uint16_t getLvl()
    {
        return lvl;
    }

    /**
     * returns current avg value in lvls
     */
    uint16_t getAvg()
    {
        return lvlAvgAdj < avgLevelFloor ? avgLevelFloor : lvlAvgAdj;
    }

    /**
     * returns last maxLvl from lvls
     */
    uint16_t getMax()
    {
        return maxLvl;
    }

    /**
     * returns the last raw mic read (after centering to right scale)
     *
     * value read in readMic()
     */
    int getRaw()
    {
        return micValue;
    }

    /**
     * reads value from mic, calculates the Lvl and AvgLvl and sets lvl to 0 if to quiet
     */
    void read()
    {
        readMic();
        calcLvl();
        calcAvg();
        checkOff();

        // Serial.print(lvl);
        // Serial.print(" - ");
        // Serial.print(maxLvl);
        // Serial.print(" - ");
        // Serial.print(lvlAvg);
        // Serial.print(" - ");
        // Serial.print(analogRead(avgPin));
        // Serial.print(" - ");
        // Serial.print(lvlAvgAdj);
        // Serial.print(" - ");
        // Serial.println(micValue);
    }
};

#endif //_MICINPUT_HPP