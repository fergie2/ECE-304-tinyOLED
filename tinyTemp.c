/******************************************************************
* megaTemp.c Firmware for a digital thermometer based on ATmega328P,
* TMP-36 sensor and SSD1306 OLED display. Displays TMP-36 sensor voltage (in mV)
* Temp (in F or C depending on switch position). Features a Too-Hot warning
* and LED when temp exceeds a pre-set value (deg F)
* Version 1.0 D. McLaughlin Initial release for ECE-304 2/28/24
**************************************************************/

#include "tinyTemp.h"

#define NUMSAMPLES 25  // #ADC Samples to average
#define VREF 1.1        // ADC reference voltage
#define MAXTEMP 80      // Too-Hot LED turns on at this temp (Deg F)
#define TOO_HOT_LED PB2 // LED indicator pin
#define F_C_SELECT PB3  // F vs C temp switch. C when pressed.

const char tooHot[] PROGMEM = "TOO HOT!";
const char specialBlank[] PROGMEM = "TEMPERATURE:          ";
const char Temperature[] PROGMEM = "TEMPERATURE: "; 
const char decimalPoint[] PROGMEM = ".";
const char degC[] PROGMEM = "DEG C";
const char degF[] PROGMEM = "DEG F";

int main(void)
{
    WDT_OFF(); // Make sure the WDT is off at startup
    float tempC, tempF, analogVoltage;
    // unsigned char switch_state, too_hot;
    unsigned int digitalValue, tempx10, tempMantissa, tempDec;
    unsigned long int totalValue;
    char tempBuffer[3];
    DDRB = 1 << TOO_HOT_LED;    // Set as output
    PORTB |= (1 << F_C_SELECT); // Set input pullup
    adc_init();
    OLED_init();
    OLED_clear();
    
    // Acquire & average NUMSAMPLES temp measurements
    totalValue = 0;
    for (int i = 0; i < NUMSAMPLES; i++)
        totalValue += get_adc(); // Get a sample from temp sensor
    digitalValue = totalValue / NUMSAMPLES;
    
    // Convert to temp & analog voltage in mV
    tempC = digitalValue * 1.0 * VREF / 10.24 - 50.; 
    tempF = tempC * 9. / 5. + 32.;
    // analogVoltage = digitalValue * 1.0 * VREF / 1024; 
    // analogVoltage = analogVoltage * 1000;  

    OLED_cursor(0, 0);
        
    if(tempF > MAXTEMP){
        OLED_printP(Temperature);
        OLED_printP(tooHot);
    }
    else{
        // OLED_printP(Temperature);
        // OLED_printP(blankTooHot);
        OLED_printP(specialBlank);
    }
    OLED_cursor(0, 1);
    
    tempMantissa = tempC;
    itoa(tempMantissa, tempBuffer, 10);

    unsigned char i;
    for (i = 0; i < strlen(tempBuffer); i++)
    {
        OLED_printC(tempBuffer[i]);
    }
    OLED_printP(decimalPoint);

    tempDec = (tempC * 100);
    tempDec = tempDec % 100;
    itoa(tempDec, tempBuffer, 10);

    
    for (i = 0; i < strlen(tempBuffer); i++)
    {
        OLED_printC(tempBuffer[i]);
    }
    OLED_cursor(50, 1);
    OLED_printP(degC);


    //DISPLAY ferenheight

    //Clear the tempBuffer -> solves display quality issues
    for (i=0; i< strlen(tempBuffer); i++){
        tempBuffer[i] = 0;
    }

    OLED_cursor(0, 2); //write on the third line
    tempMantissa = tempF;
    itoa(tempMantissa, tempBuffer, 10); //convert first part of temp reading

    for (i = 0; i < strlen(tempBuffer); i++)
    {
        OLED_printC(tempBuffer[i]);
    }
    OLED_printP(decimalPoint);

    tempDec = (tempF * 100);
    tempDec = tempDec % 100;
    itoa(tempDec, tempBuffer, 10);

    for (i = 0; i < strlen(tempBuffer); i++)
    {
        OLED_printC(tempBuffer[i]);
    }
    OLED_cursor(50, 2);
    OLED_printP(degF);

    for (i=0; i< strlen(tempBuffer); i++){
        tempBuffer[i] = 0;
    }

    wdt_enable(WDTO_8S);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
    while (1)
        ;

    return 0;
}

// Initialize ADC peripheral
void adc_init(void)
{
    ADMUX = 0x82; // Select ADC2; Vref=1.1
    ADCSRA = 0x83; // Enable ADC; divide by 8 for 1 MHZ clock
}

// Read ADC value
unsigned int get_adc()
{
    ADCSRA |= (1 << ADSC); // Start ADC conversion
    while ((ADCSRA & (1 << ADIF)) == 0)
        ;                       // Wait till ADC finishes
    ADCSRA |= (1<<ADIF);        // Clear ADIF flag
    return ADCL | (ADCH << 8);  // Read ADCL first !
}

// Disable WTD and clear reset flag immediately at startup
void WDT_OFF()
{
    MCUSR &= ~(1 << WDRF);
    WDTCR = (1 << WDCE) | (1 << WDE);
    WDTCR = 0x00;
}