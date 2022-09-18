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

    uint8_t noise;
    uint8_t minLevelDifferenece;
    uint8_t levelDiffereneceOff;
    int micCenter;

    uint8_t micPin;

    uint16_t offDelay;
    uint16_t offCounter;

    /**
     * reads sound value from microphone and removes noise
     */
    void getSoundValue()
    {
        int n = analogRead(micPin);
        // Serial.println(n);
        n = abs(n - 512 - micCenter);
        // Serial.println(n);
        n = (n <= noise) ? 0 : (n - noise);
        // Serial.println(n);
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
        // fall slower, rise faster
        if (soundValue > level)
            level = ((level * 3) + soundValue) >> 2;
        else
            level = ((level * 7) + soundValue) >> 3;
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

        // fall slower, rise faster
        if (maxLvl > maxLvlAvg)
            maxLvlAvg = (maxLvlAvg * 31 + maxLvl) >> 5;
        else
            maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6;
    }

public:
    explicit micinput(uint8_t micPin, uint8_t noise = 15, int micCenter = 0, uint8_t minLevelDifferenece = 40, uint16_t offDelay = 30)
    {
        this->micPin = micPin;
        this->noise = noise;
        this->micCenter = micCenter;
        this->minLevelDifferenece = minLevelDifferenece;
        this->offDelay = offDelay;
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
        return maxLvlAvg;
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