//
// Created by enrico on 22.06.22.
//

#include "strip.hpp"
#include "micInput.hpp"
// #include <IRremote.hpp>

#define MICPIN A0
#define AVGPIN A1

#define REMOTEPIN 4

#define SUBLEDPIN 2
#define SUBLEDCOUNT 59

#define MIDLEDPIN1 3
#define MIDLEDPIN2 6
#define MIDLEDCOUNT 27

// #define SIDELEDPIN1 5
// #define SIDELEDPIN2 6
// #define SIDELEDCOUNT 31

// IRrecv irrecv(REMOTEPIN);

micinput input = micinput(MICPIN, AVGPIN, -140, 20, 30);
strip<SUBLEDPIN, SUBLEDCOUNT> sub = strip<SUBLEDPIN, SUBLEDCOUNT>(false, 300, 1.5);
strip<MIDLEDPIN1, MIDLEDCOUNT> mid1 = strip<MIDLEDPIN1, MIDLEDCOUNT>(true, 10, 1.5);
strip<MIDLEDPIN2, MIDLEDCOUNT> mid2 = strip<MIDLEDPIN2, MIDLEDCOUNT>(false, 10, 1.5);
// strip<SIDELEDPIN1, SIDELEDCOUNT> side1 = strip<SIDELEDPIN1, SIDELEDCOUNT>(false, 10, 1.5);

int subState = 7;
int midState = 4;
// int sideState = 1;

long lastExec = 0;

long lastMillis = 0;
long loops = 0;

#define chechTime1 long f = millis();
#define chechTime2                \
	Serial.print("time millis:"); \
	Serial.println(millis() - f);

void checkCycles()
{
	loops++;
	long currentMillis = millis();
	if (currentMillis - lastMillis > 1000)
	{
		Serial.print("Loops last second:");
		Serial.println(loops);

		lastMillis = currentMillis;
		loops = 0;
	}
}

void setup()
{
	Serial.begin(9600);
	// irrecv.start();
	Serial.println("\n\n\n");

	input.init();

	sub.init();
	mid1.init();
	mid2.init();
	// side1.init();

	sub.Test2();
	show();
	delay(400);

	mid1.Test();
	show();
	delay(400);

	mid2.Test2();
	show();
	delay(400);

	// sub.Test();
	// delay(400);

	// pinMode(SUBBUTTONPIN, INPUT);
	// pinMode(MIDBUTTONPIN, INPUT);
	// pinMode(SIDEBUTTONPIN, INPUT);
}

void loop()
{
	// if (irrecv.decode())
	// {
	// 	irrecv.resume();
	// 	Serial.println(irrecv.decodedIRData.command);
	// }

	long currentMillis = millis();
	if (currentMillis - lastExec < 10)
		return;

	lastExec = currentMillis;

	input.read();
	

	// if (digitalRead(SUBBUTTONPIN))
	// {
	// 	subState++;
	// 	// Serial.println(subState);
	// 	while (digitalRead(SUBBUTTONPIN))
	// 	{
	// 	}
	// }

	// if (digitalRead(MIDBUTTONPIN))
	// {
	// 	midState++;
	// 	// Serial.println(midState);
	// 	while (digitalRead(MIDBUTTONPIN))
	// 	{
	// 	}
	// }

	// if (digitalRead(SIDEBUTTONPIN))
	// {
	// 	sideState++;
	// 	// Serial.println(sideState);
	// 	while (digitalRead(SIDEBUTTONPIN))
	// 	{
	// 	}
	// }

	// if (subState >= 7)
	// 	subState = 0;

	// if (midState >= 7)
	// 	midState = 0;

	// if (sideState >= 7)
	// 	sideState = 0;
	// sub.Test();

	// sub.Normal(input.getLvl(), input.getAvg(), true, true);
	// mid1.Normal(input.getLvl(), input.getAvg(), true, true);
	// sub.Normal(input.getMax(), 611, false, false);
	// mid1.Normal(input.getAvg(), 611, false, false);
	// mid1.Normal(input.getRaw(), input.getAvg(), true, true);
	// mid1.Normal(input.getLvl(), 611, false, false);

	// return;

	// subState = 7;
	switch (subState)
	{
	case 0:
		sub.Clear();
		break;
	case 1:
		sub.Normal(input.getLvl(), input.getAvg(), false);
		break;
	case 2:
		sub.CentreOut(input.getLvl(), input.getAvg(), false);
		break;
	case 3:
		sub.Normal(input.getLvl(), input.getAvg());
		break;
	case 4:
		sub.CentreOut(input.getLvl(), input.getAvg());
		break;
	case 5:
		sub.FallingStar(input.getLvl(), input.getAvg(), true, 3);
		break;
	case 6:
		sub.Circle(input.getLvl(), input.getAvg(), 5, 0.05, 3, 2.5, false);
		break;
	case 7:
		sub.Circle(input.getLvl(), input.getAvg(), 5, 0.05, 3, 2.5);
		break;
	case 8:
		sub.Pulse(input.getLvl(), input.getAvg(), 0.90, false);
		break;
	case 9:
		sub.Pulse(input.getLvl(), input.getAvg(), 0.90);
		break;
	}

	// return;

	// midState = 6;

	switch (midState)
	{
	case 0:
		mid1.Clear();
		mid2.Clear();
		break;
	case 1:
		mid1.Normal(input.getLvl(), input.getAvg(), false);
		mid2.Normal(input.getLvl(), input.getAvg(), false);
		break;
	case 2:
		mid1.CentreOut(input.getLvl(), input.getAvg(), false);
		mid2.CentreOut(input.getLvl(), input.getAvg(), false);
		break;
	case 3:
		mid1.Normal(input.getLvl(), input.getAvg());
		mid2.Normal(input.getLvl(), input.getAvg());
		break;
	case 4:
		mid1.CentreOut(input.getLvl(), input.getAvg());
		mid2.CentreOut(input.getLvl(), input.getAvg());
		break;
	case 5:
		mid1.FallingStar(input.getLvl(), input.getAvg(), true, 1);
		mid2.FallingStar(input.getLvl(), input.getAvg(), true, 1);
		break;
	case 6:
		mid1.Circle(input.getLvl(), input.getAvg(), 2, 0.08, 2, 2.0, false);
		mid2.Circle(input.getLvl(), input.getAvg(), 2, 0.08, 2, 2.0, false);
		break;
	case 7:
		mid1.Circle(input.getLvl(), input.getAvg(), 2, 0.08, 2, 2.0);
		mid2.Circle(input.getLvl(), input.getAvg(), 2, 0.08, 2, 2.0);
		break;
	case 8:
		mid1.Pulse(input.getLvl(), input.getAvg(), 0.9, false);
		mid2.Pulse(input.getLvl(), input.getAvg(), 0.9, false);
		break;
	case 9:
		mid1.Pulse(input.getLvl(), input.getAvg(), 0.9);
		mid2.Pulse(input.getLvl(), input.getAvg(), 0.9);
		break;
	}

	show();
	checkCycles();
	return;

	// sideState = 6;

	// switch (sideState)
	// {
	// case 0:
	// 	side1.Clear();
	// 	break;
	// case 1:
	// 	side1.Normal(input.getLvl(), input.getAvg(), false);
	// 	break;
	// case 2:
	// 	side1.CentreOut(input.getLvl(), input.getAvg(), false);
	// 	break;
	// case 3:
	// 	side1.Normal(input.getLvl(), input.getAvg());
	// 	break;
	// case 4:
	// 	side1.CentreOut(input.getLvl(), input.getAvg());
	// 	break;
	// case 5:
	// 	side1.FallingStar(input.getLvl(), input.getAvg(), true, 3);
	// 	break;
	// case 6:
	// 	side1.Circle(input.getLvl(), input.getAvg(), true, 7, 0.05, 2, 2.5);
	// 	break;
	// }

	// show();
	// checkCycles();
}