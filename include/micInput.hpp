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
    uint16_t maxLvlAvg;
    int soundValue;
    uint16_t level;

    uint16_t volumeCount;
    uint16_t volumes[sampleCount]{};
    uint16_t maxLvl;

    uint8_t minLevelDifferenece;
    uint8_t minAvgLevel;
    uint8_t levelDiffereneceOff;
    int micCenter;

    uint8_t micPin;

    uint16_t offDelay;
    uint16_t offCounter;

    /**
     * reads sound value from microphone
     */
    void getSoundValue()
    {
        int n = analogRead(micPin);
        n = abs(n - 512 - micCenter);
        Serial.println(n);
        soundValue = n;
    }

    /**
     * calculates current level based on previous level
     *
     * multiplies by 3 adds soundValue and divides by 2 to make
     * changes smoth
     */
    void calcSoundLevel(int soundValue)
    {
        if (false)
        {
            // fall slower, rise faster
            if (soundValue > level)
                level = ((level * 3) + soundValue) >> 2;
            else
                level = ((level * 7) + soundValue) >> 3;
        }
        else if (true)
        {
            // fall and rise fast
            // level = ((level * 1) + soundValue) >> 1;
            level = ((level * 3) + soundValue) >> 2;
        }
        else
        {
            level = ((level * 7) + soundValue) >> 3;
        }
    }

    /**
     * adds current volume to list of volumes
     */
    void addCurrentVolumeToSamples(uint16_t volume)
    {
        volumes[volumeCount] = volume;
        if (++volumeCount >= sampleCount)
            volumeCount = 0;
    }

    /**
     * calculates new max and min levels and
     * adjustst old levels by multiplying by 63 and dividing by >> 6
     */
    void calcAvgMinMax()
    {
        maxLvl = volumes[0];
        for (uint16_t i = 1; i < sampleCount; i++)
        {
            if (volumes[i] > maxLvl)
                maxLvl = volumes[i];
        }

        // dump down a bit
        maxLvl = maxLvl - (maxLvl >> 3);

        if (false)
        {
            // fall slower, rise faster
            if (maxLvl > maxLvlAvg)
                maxLvlAvg = (maxLvlAvg * 31 + maxLvl) >> 5;
            else
                maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6;
        }
        else if (false)
        {
            // fall and rise fast
            maxLvlAvg = (maxLvlAvg * 15 + maxLvl) >> 4;
            // maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6;
        }
        else
        {
            // fall faster, rise slower
            if (maxLvl > maxLvlAvg)
                maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6;
            else
                maxLvlAvg = (maxLvlAvg * 31 + maxLvl) >> 5;
        }
    }

public:
    explicit micinput(uint8_t micPin, int micCenter = 0, uint8_t minLevelDifferenece = 40, uint16_t offDelay = 30, uint16_t minAvgLevel = 120)
    {
        this->micPin = micPin;
        this->micCenter = micCenter;
        this->minLevelDifferenece = minLevelDifferenece;
        this->offDelay = offDelay;
        this->minAvgLevel = minAvgLevel;
    }

    void init()
    {
        pinMode(micPin, INPUT);

        level = 10;
        maxLvlAvg = 10;
        volumeCount = 0;
        offCounter = 0;
    }

    uint16_t getLevel()
    {
        return level;
    }

    uint16_t getmaxLvlAvg()
    {
        return maxLvlAvg < minAvgLevel ? minAvgLevel : maxLvlAvg;
    }

    uint16_t getAvg()
    {
        return maxLvl;
    }

    int getRaw()
    {
        return soundValue;
    }

    void readSound()
    {
        getSoundValue();
        calcSoundLevel(soundValue);
        addCurrentVolumeToSamples(soundValue);
        calcAvgMinMax();
        Serial.println(maxLvlAvg);
        Serial.println();

        // pset to off if difference is to low
        if (maxLvlAvg <= minLevelDifferenece)
        {

            // Serial.print("min !!");
            // Serial.println(maxLvlAvg);
            maxLvlAvg = minLevelDifferenece;
            offCounter++;
            if (offCounter >= offDelay)
            {
                // Serial.print("off");
                level = 0;
                // offCounter = 0;
            }
        }
        else
        {
            offCounter = 0;
        }
    }
};

#endif //_MICINPUT_HPP