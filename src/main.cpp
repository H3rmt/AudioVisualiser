#include "glob.hpp"

// IRrecv irrecv(REMOTEPIN);

micinput input = micinput(MICPIN, AVGPIN, -140, 42, UPS * SUO);
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

void checkCycles()
{
	loops++;
	long currentMillis = millis();
	if (currentMillis - lastMillis > 1000)
	{
		print("Loops last second:");
		println(loops);

		lastMillis = currentMillis;
		loops = 0;
	}
}

void setup()
{
	Serial.begin(9600);
	// irrecv.start();
	println("\n\n\n");

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
	// 	println(irrecv.decodedIRData.command);
	// }

	long currentMillis = millis();
	if (currentMillis - lastExec < (1000 / UPS))
		return;

	lastExec = currentMillis;

	input.read();

	// if (digitalRead(SUBBUTTONPIN))
	// {
	// 	subState++;
	// 	// println(subState);
	// 	while (digitalRead(SUBBUTTONPIN))
	// 	{
	// 	}
	// }

	// if (digitalRead(MIDBUTTONPIN))
	// {
	// 	midState++;
	// 	// println(midState);
	// 	while (digitalRead(MIDBUTTONPIN))
	// 	{
	// 	}
	// }

	// if (digitalRead(SIDEBUTTONPIN))
	// {
	// 	sideState++;
	// 	// println(sideState);
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

	if (input.getOff())
	{
		mid1.OffAnimiation();
		mid2.OffAnimiation();
		sub.OffAnimiation();
		show();
		checkCycles();
		return;
	}

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