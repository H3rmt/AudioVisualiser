//
// Created by enrico on 22.06.22.
//

#include "strip.hpp"
#include "micInput.hpp"

#define SAMPLES 25

#define LEDPIN 4
#define LEDCOUNT 34
#define BUTTONPIN 3

#define LEDPIN2 3
#define LEDPIN3 2
#define LEDCOUNT2 59
#define LEDCOUNT3 27
#define BUTTONPIN2 9
#define BUTTONPIN3 10

micinput<SAMPLES> input = micinput<SAMPLES>(A0, 10, -100, 40, 130);
strip<LEDPIN2, LEDCOUNT2> strip2 = strip<LEDPIN2, LEDCOUNT2>(false, 50, 2);
strip<LEDPIN3, LEDCOUNT3> strip3 = strip<LEDPIN3, LEDCOUNT3>(false, 50, 2);

int strip2State = 4;
int strip3State = 4;

void setup()
{
	Serial.begin(9600);
	// strip1.init();
	input.init();
	strip2.init();

	// strip2.Test2();
	// delay(400);
	
	strip3.init();
	// strip3.Test2();
	// delay(400);

	pinMode(BUTTONPIN2, INPUT);
	pinMode(BUTTONPIN3, INPUT);

	// strip3.Test();
	// delay(400);
	// strip2.Test();
	// delay(400);
}

void loop()
{
	input.readSound();

	// if(digitalRead(BUTTONPIN))
	// strip1State++;
	// while(digitalRead(BUTTONPIN)) {
	// }

	if (digitalRead(BUTTONPIN2))
	{
		strip2State++;
		// Serial.println(strip2State);
	}
	while (digitalRead(BUTTONPIN2))
	{
	}

	if (digitalRead(BUTTONPIN3))
	{
		strip3State++;
		// Serial.println(strip3State);
	}
	while (digitalRead(BUTTONPIN3))
	{
	}

	// if(strip1State > 7)
	// 	strip1State = 0;

	if (strip2State > 5)
		strip2State = 0;

	if (strip3State > 5)
		strip3State = 0;

	// strip2.Test();
	
	strip2.Normal(input.getRaw(), input.getmaxLvlAvg(), false, false);
	strip3.Normal(input.getLevel(), input.getmaxLvlAvg(), false, false);
	// strip3.Normal(false, input.getAvg(), 1023);
	// strip3.Normal(input.getLevel(), 1023, false, false);
	// strip3.Normal(512 - input.getmaxLvlAvg() + input.getRaw(), 1023, false, false);
	
	return;

	switch (strip2State)
	{
	case 0:
		strip2.Clear();
		break;
	case 1:
		strip2.Normal(input.getLevel(), input.getmaxLvlAvg(), false);
		break;
	case 2:
		strip2.CentreOut(false, input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	case 3:
		strip2.Normal(input.getLevel(), input.getmaxLvlAvg());
		break;
	case 4:
		strip2.CentreOut(true, input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	case 5:
		strip2.FallingStar(input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	case 6:
		strip2.Pulse(input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	case 7:
		strip2.Stream(input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	}

	switch (strip3State)
	{
	case 0:
		strip3.Clear();
		break;
	case 1:
		strip3.Normal(input.getLevel(), input.getmaxLvlAvg(), false);
		break;
	case 2:
		strip3.CentreOut(false, input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	case 3:
		strip3.Normal(input.getLevel(), input.getmaxLvlAvg());
		break;
	case 4:
		strip3.CentreOut(true, input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	case 5:
		strip3.FallingStar(input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	case 6:
		strip3.Pulse(input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	case 7:
		strip3.Stream(input.getLevel(), 0, input.getmaxLvlAvg());
		break;
	}
}