#include "tinyOLED.h"


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>


void WDT_OFF(void);
void adc_init(void);
unsigned int get_adc(void);