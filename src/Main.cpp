#include <Arduino.h>
#include <SPI.h>
#include "Defs.hpp"

#include <PDM.h>
#include <TFT_eSPI.h>

#include "Util.hpp"
#include "ApproxFFT.hpp"
#include "Draw.hpp"
#include "Light.hpp"

// Audio sample buffer
short sampleBuffer[128];

// Audio sample buffers used for analysis and display
short streamBuffer1[SAMPLES];           // Sample stream buffer (N samples)
short streamBuffer2[SAMPLES];           // Sample stream buffer (N samples)
int approxBuffer1[SAMPLES];             // ApproxFFT sample buffer
int approxBuffer2[SAMPLES];             // ApproxFFT sample buffer
int peakBuffer[SAMPLES_USABLE];         // Amplitude peak buffer
int display_peakBuffer[SAMPLES_USABLE]; // Amplitude peak buffer

int currentBuffer = 1; // Current buffer that has the correct samples

// Divider for the sample value
float divider = DIVIDER_START;
int16_t less_than_divider_decrease_counter = 0;
bool off = false;
uint16_t less_than_off_counter = 0;

uint16_t max_index_float = 5;              // Index of the peak frequency Average
uint16_t max_index_actual = 5;             // Index of the peak frequency Average
uint32_t avg_max_float = FLOATING_AVG_MIN; // Average of the peak frequencys
short stream_buffer_max = 0;               // Maximum value in the streamBuffer

// values for the display
uint32_t display_peak_avg = 0;         // Average of the peak frequencys
uint16_t display_max_index_float = 0;  // Index of the peak frequency Average
uint16_t display_max_index_actual = 0; // Index of the peak frequency Average
uint16_t display_max_index = 0;        // Index of the peak frequency
uint32_t display_avg_max_float = 0;    // Average of the peak frequencys
short display_stream_buffer_max = 0;   // Maximum value in the streamBuffer
int display_peak_value = 0;

int samplesRead = 0; // Samples read from microphone (expected to be 64)
int sampleCount = 0; // Samples put into streamBuffer

// Test varaible to simmulate level
// int increaase = 0;

// Display has progressed old data
bool display_allow_new_data = true;

long startMillis = millis();         // Timer for FPS
long readMillis = millis();          // Timer for read time
double millis_for_one_iteration = 0; // Time it took for last iteration

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

uint16_t *sptr = nullptr;

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
    ;
#endif

  Serial.println("START 0");

  // Initialise the PDM library
  PDM.onReceive([]()
                {
    int bytesAvailable = PDM.available();
    PDM.read(sampleBuffer, bytesAvailable);
    samplesRead = bytesAvailable / 2; });

  PDM.setGain(20);
  // setup the I2S audio input for the sample rate with 32-bits per sample
  if (!PDM.begin(CHANNELS, SAMPLING_FREQUENCY))
  {
    Serial.println("Failed to start PDM!");
    while (1)
    {
    }
  }
  Serial.println("PDM started");

  delay(50);
  Serial.println("Setup complete");

  delay(50);
  initLeds();
  delay(50);
  testLeds();
  delay(50);
  Serial.println("LEDS complete");
}

void loop()
{
  // Only 64 PDM audio samples are available at a time so accumulate 256 for the FFT
  if (samplesRead != 0)
  {
    short *streamBuffer = (currentBuffer == 1 ? streamBuffer2 : streamBuffer1);
    int *approxBuffer = (currentBuffer == 1 ? approxBuffer2 : approxBuffer1);

    // This is a super weird bug, arround index 66 and 2 are some weird spikes, idk why
#ifdef FIX_SPIKES
    sampleBuffer[1] = sampleBuffer[0];
    sampleBuffer[2] = sampleBuffer[4];
    sampleBuffer[3] = sampleBuffer[4];
#endif

    // Fill the buffers with the samples
    for (int i = 0; i < samplesRead; i++)
    {
      streamBuffer[i + sampleCount] = sampleBuffer[i] / divider;
      approxBuffer[i + sampleCount] = (sampleBuffer[i] / divider) / 2;
    }

    sampleCount += samplesRead;
    samplesRead = 0;

    // enough samples to a FFT iteraton
    if (sampleCount >= SAMPLES)
    {
      millis_for_one_iteration = millis() - readMillis;
      readMillis = millis();

      sampleCount = 0;
      approx_FFT(approxBuffer, SAMPLES, SAMPLING_FREQUENCY);

      short stream_buffer_max = 0;
      int ii = 0;
      for (int i = 0; i < SAMPLES; i++)
      {
        if (streamBuffer[i] > stream_buffer_max)
        {
          ii = i;
          stream_buffer_max = streamBuffer[i];
        }
      }

      if (stream_buffer_max < OFF_THRESHOLD)
      {
        less_than_off_counter++;
        if (less_than_off_counter > OFF_SECONDS * ESTIMATE_SAMPLES_PER_SECOND)
        {
          off = true;
          less_than_off_counter = 0;
        }
      }
      else
      {
        off = false;
        less_than_off_counter = 0;
      }

      if (stream_buffer_max > INCREASE_DIVIDER_PEAK)
      {
        divider += 0.05;
        less_than_divider_decrease_counter = 0;
      }
      else if (stream_buffer_max < DECREASE_DIVIDER_PEAK)
      {
        less_than_divider_decrease_counter++;
        if (less_than_divider_decrease_counter > DECREASE_DIVIDER_SECONDS * ESTIMATE_SAMPLES_PER_SECOND)
        {
          if (divider > DIVIDER_MIN)
          {
            divider -= 0.1;
          }
          less_than_divider_decrease_counter = 0;
        }
      }
      else
      {
        less_than_divider_decrease_counter = 0;
      }

      // Serial.print("Max: ");
      // Serial.print(stream_buffer_max);
      // Serial.print(" at ");
      // Serial.println(ii);

      u_int16_t max_index = 0;
      for (int i = 0; i < SAMPLES_USABLE; i++)
      {
        // adjust the amplitude of the approxBuffer (increase higher frequencies)
        approxBuffer[i] = (0.38 + (i / 11.0)) * approxBuffer[i];

        // update the peakBuffer
        if (approxBuffer[i] > peakBuffer[i])
          peakBuffer[i] = approxBuffer[i];
        else
          peakBuffer[i] = peakBuffer[i] * 0.96 + approxBuffer[i] * 0.04;

        // Find the peak frequency
        if (i < MAXAVGFREQ && peakBuffer[i] > peakBuffer[max_index])
          // if (i < MAXAVGFREQ && approxBuffer[i] > approxBuffer[max_index])
          max_index = i;
      }

      // TODO move the peak a bit faster but stay on the old value until the bar is reached (about 8 near is ok to use the actuall value)

      // move the floating peak frequency to the correct index
      if (max_index > max_index_float)
      {
        max_index_float += max(1, (max_index - max_index_float) / 8); // move /8 of the way
        if (max_index_float > MAXAVGFREQ)
        {
          max_index_float = MAXAVGFREQ;
        }
      }
      else if (max_index < max_index_float)
      {
        if (max_index_float > max(1, (max_index_float - max_index) / 4))
        {
          max_index_float -= max(1, (max_index_float - max_index) / 4); // move /4 of the way
        }
        else
        {
          max_index_float = 0;
        }
      }

      // only change the actual peak frequency if the floating peak frequency is near the actual peak frequency
      if (max_index - max_index_float < 3 && max_index - max_index_float > -3)
      {
        max_index_actual = max_index;
      }

      // get the current peak value
      int peak_value = approxBuffer[max_index_actual];

      // calculate average arround the peak frequency
      int peak_avg = 0;
      uint8_t i = max(0, max_index_actual - 1);
      uint8_t count = 0;
      while (i < max_index_actual + 1)
      {
        peak_avg += approxBuffer[i];
        i++;
        count++;
      }
      peak_avg /= count;

      // adjust the floating average to the peak frequency
      // floating_avg = floating_avg * 0.98 + avg * 0.025;
      avg_max_float = avg_max_float * 0.99 + peak_avg * 0.025;
      // min and max the floating average
      if (avg_max_float < peak_avg)
        avg_max_float = peak_avg;
      if (avg_max_float < FLOATING_AVG_MIN * divider)
        avg_max_float = FLOATING_AVG_MIN * divider;

#ifdef DEBUGG
      Serial.print("peak_avg: ");
      Serial.print(peak_avg);
      Serial.print("   \tmax_index_float: ");
      Serial.print(max_index_float);
      Serial.print(" \tavg_max_float: ");
      Serial.print(avg_max_float);
      Serial.print(" \tstream_buffer_max: ");
      Serial.print(stream_buffer_max);
      Serial.print("    \tpeak_value: ");
      Serial.print(peak_value);
#endif

      if (!off)
      {
        drawLEDs(peak_value / 500, avg_max_float / 500);
      }
      else
      {
        drawLEDsOff();
      }

      if (display_allow_new_data)
      {
#ifdef DEBUGG
        Serial.print("\t\t NEW DATA");
#endif
        display_peak_avg = peak_avg;
        display_max_index_float = max_index_float;
        display_max_index_actual = max_index_actual;
        display_max_index = max_index;
        display_avg_max_float = avg_max_float;
        display_stream_buffer_max = stream_buffer_max;

        // copy peakBuffer to the other buffer with memcp
        memcpy(display_peakBuffer, peakBuffer, sizeof(int) * SAMPLES_USABLE);

        currentBuffer = (currentBuffer == 1 ? 2 : 1);
        display_allow_new_data = false;
      }
#ifdef DEBUGG
      Serial.println();
#endif

      // test led levls
      // drawLEDs(increaase, 1000);
      // increaase += 2;
      // if (increaase > 1000)
      // {
      //   increaase = 0;
      // }
    }
  }
}

bool display_ready = false;
void setup1()
{
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial)
    ;
#endif

  Serial.println("START 1");
  delay(500);

  tft.init();
  tft.setRotation(3);
  // tft.setSwapBytes(false);
  tft.initDMA();

  Serial.println("TFT started");
  tft.fillScreen(ILI9341_BLACK);
  // tft.startWrite();
  drawTopBar(tft);
  drawSideBar(tft);

  sptr = (uint16_t *)spr.createSprite(WIDTH, HEIGHT);
  delay(200);
  display_ready = true;
}

uint16_t counter = 0;   // Frame counter
uint16_t interval = 40; // FPS calculated over 40 frames
void loop1()
{
  if (!display_ready)
  {
    return;
  }
  // wait for next FFT to swap buffers
  if (display_allow_new_data)
  {
    return;
  }

  tft.dmaWait();
  drawSprite(spr,
             (currentBuffer == 1 ? approxBuffer1 : approxBuffer2),
             (currentBuffer == 1 ? streamBuffer1 : streamBuffer2),
             display_peakBuffer, display_max_index_float, display_max_index_actual,
             display_max_index, display_stream_buffer_max, display_peak_avg, display_avg_max_float, off);

  tft.startWrite();
  tft.pushImageDMA(0, TOPBARHEIGHT, WIDTH, HEIGHT, sptr);
  tft.endWrite();
  // tft.pushImage(0, TOPBARHEIGHT, WIDTH, HEIGHT, sptr);

  // only calculate the fps every <interval> iterations.
  counter++;
  if (counter >= interval)
  {
    double fps = interval * 1000.0 / (millis() - startMillis); // 60 frames over 12_000 - 10_000 = 2_000 => 60 * 1000 / 2_000 = 30
    startMillis = millis();

    drawFPS(tft, fps, 1000.0 / millis_for_one_iteration, divider);
    counter = 0;
  }

  // Allow for new data to be loaded into the buffers
  display_allow_new_data = true;
}