#include <reg52.h> // Include the 8051 microcontroller register definition

// Function prototypes
void delay(unsigned int count);
void lcd_cmd(unsigned char a);
void lcd_data(unsigned char b);
void lcd_init(void);
void lcd_str(unsigned char *str);
void display_temperature(unsigned int temperature);

// Define special function registers and control pins
sbit rs   = P3^0;   // LCD RS
sbit rw   = P3^1;   // LCD RW
sbit en   = P3^2;   // LCD Enable
sfr  ldata = P2;    // LCD Data Port
sbit rd   = P3^4;   // ADC Read
sbit wr   = P3^5;   // ADC Write
sbit intr = P3^6;   // ADC Interrupt
sfr  adc  = P1;     // ADC Data Port
sbit fan  = P3^7;   // Fan Control

void main() {
    unsigned int adc_value, temperature;

    lcd_init();
    fan = 0;

    lcd_str("TEMP CONTROL FAN");
    delay(100);
    lcd_cmd(0x01);
    lcd_cmd(0x80);
    lcd_str("Temp = ");
    lcd_data(223);
    lcd_str("C");

    while(1) {
        rd = 1;
        wr = 0;
        delay(5);
        wr = 1;

        while(intr == 1); // Wait for conversion
        delay(5);
        rd = 0;

        adc_value = adc;
        temperature = (adc_value * 500) / 255;

        lcd_cmd(0x89);
        display_temperature(temperature);

        if (temperature > 29) {
            lcd_cmd(0xC0);
            lcd_str("TURN FAN ON ");
            fan = 1;
        } else {
            lcd_cmd(0xC0);
            lcd_str("TURN FAN OFF");
            fan = 0;
        }

        intr = 1;
    }
}

void delay(unsigned int count) {
    int i, j;
    for(i = 0; i < count; i++)
        for(j = 0; j < 1275; j++);
}

void display_temperature(unsigned int temperature) {
    unsigned char d1, d2, d3;
    d3 = temperature % 10;
    temperature /= 10;
    d2 = temperature % 10;
    d1 = temperature / 10;
    if (d1) lcd_data(d1 + '0');
    lcd_data(d2 + '0');
    lcd_data(d3 + '0');
}

void lcd_init() {
    lcd_cmd(0x38);
    lcd_cmd(0x0C);
    lcd_cmd(0x01);
    lcd_cmd(0x80);
}

void lcd_cmd(unsigned char a) {
    rs = 0; rw = 0;
    ldata = a;
    en = 1; delay(5); en = 0; delay(5);
}

void lcd_data(unsigned char b) {
    rs = 1; rw = 0;
    ldata = b;
    en = 1; delay(5); en = 0; delay(5);
}

void lcd_str(unsigned char *str) {
    while(*str)
        lcd_data(*str++);
}
