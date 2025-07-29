#include "Light.hpp"
#include "Defs.hpp"
#include "Strip.hpp"

Strip sub = Strip(SUBPIN, 140, false, 400, true);
Strip midl = Strip(LEFTPIN, 63, true, 500, false);
Strip midr = Strip(RIGHTPIN, 62, false, 500, false);

Strip l1 = Strip(LEFT3PIN, 70, false, 300, true);
Strip l2 = Strip(LEFT2PIN, 70, true, 300, true);

Strip r1 = Strip(RIGHT1PIN, 70, false, 300, true);
Strip r2 = Strip(RIGHT2PIN, 70, true, 300, true);

void drawLEDsOff()
{
    sub.OffAnimiation();
    midl.OffAnimiation();
    midr.OffAnimiation();

    l1.OffAnimiation();
    l2.OffAnimiation();

    r1.OffAnimiation();
    r2.OffAnimiation();
}

void drawLEDs(uint32_t input, uint32_t avg)
{
    sub.Pulse(input, avg, false);
    // sub.CentreOut(input, avg, true);
    midl.Circle(input, avg, 4, 2, 0.12, true);
    midr.Circle(input, avg, 4, 2, 0.12, true);

    l1.CentreOut(input, avg, true);
    l2.CentreIn(input, avg, true);

    r1.CentreOut(input, avg, true);
    r2.CentreIn(input, avg, true);
}

void initLeds()
{
    sub.init();
    midl.init();
    midr.init();

    l1.init();
    l2.init();

    r1.init();
    r2.init();
}

void testLeds()
{
    sub.Test();
    delay(50);
    midl.Test();
    delay(50);
    midr.Test();
    delay(50);

    l1.Test();
    delay(50);
    l2.Test();
    delay(50);

    r1.Test();
    delay(50);
    r2.Test();
    delay(50);
}