//
// Created by enrico on 17.07.22.
//

#ifndef _MICINPUT_HPP
#define _MICINPUT_HPP

#include <Arduino.h>

template <uint16_t sampleCount>
class micinput
{
private:
    int soundValue;

    uint16_t lvl;

    uint16_t maxLvl;
    uint16_t maxLvlAvg;

    uint16_t lvlCount;
    uint16_t lvls[sampleCount]{};

    uint16_t minAvgLvl;
    uint16_t avgLevelFloor;
    uint16_t levelDiffereneceOff;
    int micOffset;

    uint8_t micPin;

    uint16_t offDelay;
    uint16_t offCounter;

    /**
     * reads sound value from microphone
     */
    void readMic()
    {
        int n = analogRead(micPin);
        n = abs(n - 512 - micOffset);
        Serial.println(n);
        soundValue = n;
    }

    /**
     * calculates current level based on previous level and new level
     *
     * multiplies by 3 adds soundValue and divides by 2 to make
     * changes smoth and prevent flickering (soundValue makes up 1/4 of new value)
     */
    void calcLvl(int soundValue)
    {
        // fall and rise fast
        lvl = ((lvl * 3) + soundValue) >> 2;
    }

    /**
     * adds current volume to list of lvls
     */
    void addLvlToLvls(uint16_t volume)
    {
        lvls[lvlCount] = volume;
        if (++lvlCount >= sampleCount)
            lvlCount = 0;
    }

    /**
     * calculates new max level and adjustes maxLvlAvg
     * adjustst old levels by multiplying by 63 and dividing by >> 6
     */
    void calcAvg()
    {
        // calc max of last {sampleCount} levels
        maxLvl = lvls[0];
        for (uint16_t i = 1; i < sampleCount; i++)
        {
            if (lvls[i] > maxLvl)
                maxLvl = lvls[i];
        }

        // dump down a bit, allows level to rise above and hit end of LED strip
        maxLvl -= (maxLvl >> 3);

        // fall faster, rise slower
        if (maxLvl > maxLvlAvg)
            maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6;
        else
            maxLvlAvg = (maxLvlAvg * 31 + maxLvl) >> 5;
    }

    void checkOff()
    {

        // pset to off if difference is to low
        if (maxLvlAvg <= minAvgLvl)
        {

            // Serial.print("min !!");
            // Serial.println(maxLvlAvg);
            maxLvlAvg = minAvgLvl;
            offCounter++;
            if (offCounter >= offDelay)
            {
                // Serial.print("off");
                lvl = 0;
                // offCounter = 0;
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
    explicit micinput(uint8_t micPin, int micOffset = 0, uint8_t minAvgLvl = 40, uint16_t offDelay = 30, uint16_t avgLevelFloor = 120)
    {
        this->micPin = micPin;
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

        lvl = 5;
        maxLvlAvg = 10;
        lvlCount = 0;
        offCounter = 0;
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
        return maxLvlAvg < avgLevelFloor ? avgLevelFloor : maxLvlAvg;
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
        return soundValue;
    }


    /**
     * reads value from mic, calculates the Lvl and AvgLvl and sets lvl to 0 if to quiet
     */ 
    void read()
    {
        readMic();
        calcLvl(soundValue);
        addLvlToLvls(soundValue);
        calcAvg();
        // Serial.println(maxLvlAvg);
        // Serial.println();
        checkOff();
    }
};

#endif //_MICINPUT_HPP