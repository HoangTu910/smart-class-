// PIC16F877A Configuration Bit Settings

// 'C' source line config statements
#include <xc.h>

#define _XTAL_FREQ 20000000
#define Light1 RD0               // configure pin for Board's LED 
#define Light2 RD1                // configure pin for LED1 (row 1)
#define Light3 RD2                // configure pin for LED2 (row 2)
#define Light4 RD3                // configure pin for LED3 (row 3)
#define Light5 RD4                // configure pin for LED4 (row 4)
#define Fan1 RD5                // configure pin for Air conditioner
#define Fan2 RD6
#define Fan3 RD7

#define but1 RA2                // configure button
#define but2 RA4
#define but3 RA5

#define RS RE0                  // configure LCD pins
#define EN RE1
#define D4 RC0
#define D5 RC1
#define D6 RC2
#define D7 RC3

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//Global Variables
//working state
int countStudent = 0;     
char sensor1State = 0;
char sensor2State = 0;
char autoMode = 1;
char checkState2 = 0;
int temp = 0;
//LCD vars
const unsigned char option1 [] = "1.Count people";
const unsigned char option2 [] = "2.Lights option";
const unsigned char option3 [] = "3.Fans option";
const unsigned char option4 [] = "4.Control mode";

const unsigned char option1_select1 [] = "People:";
const unsigned char option1_select2 [] = "Temp:";

const unsigned char option2_select1 [] = "Light 1:";
const unsigned char option2_select2 [] = "Light 2:";
const unsigned char option2_select3 [] = "Light 3:";
const unsigned char option2_select4 [] = "Light 4:";
const unsigned char option2_select5 [] = "Light B:";

const unsigned char option3_select1 [] = "Fan 1:";
const unsigned char option3_select2 [] = "Fan 2:";
const unsigned char option3_select3 [] = "Fan 3:";

const unsigned char option4_select1 [] = "Auto mode  ";
const unsigned char option4_select2 [] = "Custom mode";

//==================LCDlib=======================
void lcd_cmd(unsigned char cmd){
    RS = 0;
    if (0b10000000 & cmd) D7 = 1; else D7 = 0;
    if (0b01000000 & cmd) D6 = 1; else D6 = 0;
    if (0b00100000 & cmd) D5 = 1; else D5 = 0;
    if (0b00010000 & cmd) D4 = 1; else D4 = 0;
    EN = 1;
    __delay_us(2000);
    EN = 0;
    
    if (0b00001000 & cmd) D7 = 1; else D7 = 0;
    if (0b00000100 & cmd) D6 = 1; else D6 = 0;
    if (0b00000010 & cmd) D5 = 1; else D5 = 0;
    if (0b00000001 & cmd) D4 = 1; else D4 = 0;
    EN = 1;
    __delay_us(2000);
    EN = 0;
}

void lcd_write(unsigned char text){
    RS = 1;
    if (0b10000000 & text) D7 = 1; else D7 = 0;
    if (0b01000000 & text) D6 = 1; else D6 = 0;
    if (0b00100000 & text) D5 = 1; else D5 = 0;
    if (0b00010000 & text) D4 = 1; else D4 = 0;
    EN = 1;
    __delay_us(2000);
    EN = 0;
    
    if (0b00001000 & text) D7 = 1; else D7 = 0;
    if (0b00000100 & text) D6 = 1; else D6 = 0;
    if (0b00000010 & text) D5 = 1; else D5 = 0;
    if (0b00000001 & text) D4 = 1; else D4 = 0;
    EN = 1;
    __delay_us(2000);
    EN = 0;
}

void lcd_print_str(char *p){
    while (*p){
        lcd_write(*p++);
    }
}

void lcd_setCursor(int dong, int cot){ // 
    if (dong == 0) lcd_cmd(0x80 + cot);
    if (dong == 1) lcd_cmd(0xC0 + cot);
}

void lcd_clear(){
    lcd_cmd(0b00000001);
}

void lcd_init(){
    lcd_cmd(0b00000001);
    lcd_cmd(0b00000010);
    lcd_cmd(0b00000110);
    lcd_cmd(0b00001100);
    lcd_cmd(0b00101000);
    lcd_cmd(0b10000000);
    
    lcd_setCursor(0, 0);
    lcd_write(0b01111110);
    lcd_setCursor(0, 1);
    lcd_print_str(option1);
    lcd_setCursor(1, 1);
    lcd_print_str(option2);
}

/**********************UART BLOCK******************************/
void uartSetup(int baudrate){
    TRISC &= ~(1<<6);
    TRISC |= (1<<7);
    SPBRG = ((_XTAL_FREQ/baudrate)/16) - 1;
    TXSTA = 0B00100100;
    RCSTA = 0B10010000;
}

void uartWrite(char ch){
    while(!TXIF);
    TXREG = ch;
}

void uartSent(char *p){
    while (p){
        uartWrite(*p++);
    }
    uartWrite(0x00);
}
/**********************UART BLOCK******************************/

/**********************FUNCTION BLOCK**************************/
int adcAdjust = 0;
void controlClassroom(int countStudent) {
    if(countStudent == 0){
        PORTD = 0;
        adcAdjust = 0;
    }
    if (countStudent >= 1 && countStudent < 6) {
        PORTD = 0b00010001;
        adcAdjust = 16;

        
    }
    // Turn on lights and fans for row 2 if there are 6-10 students
    if (countStudent >= 6 && countStudent < 11) {
        PORTD = 0b00010011;  // LED2 on  
        adcAdjust = 19;

    }
    // Turn on lights for row 3 if there are 11-15 students
    if (countStudent >= 11 && countStudent < 16) {
        PORTD = 0b00010111;
        adcAdjust = 21;

    }
    if (countStudent >= 16 && countStudent < 21) {
        PORTD = 0b00011111;
        adcAdjust = 21;

    }
    
    
}
//1 Fan = 620
//2 Fan = 624
//3 Fan = 627
//All = 641
void checkState(){
    if(sensor1State == 1){
        countStudent++;
    }
    
    if(sensor2State == 1){
        countStudent--;
        
    }
    
    if(countStudent < 0){
        countStudent = 0;
    }
    if(autoMode == 1) controlClassroom(countStudent);
    sensor1State = 0;
    sensor2State = 0;
}

char andSensor2 = 0;

/**********************FUNCTION BLOCK**************************/

/**********************INTERRUPT BLOCK*************************/
void interruptsSetup(void){
    OPTION_REGbits.INTEDG = 1;  //interrupt on rising edge of INT pin
    INTCONbits.INTE = 1;        //enable the external interrupt
    INTCONbits.GIE = 1;         //set the Global Interrupt Enable
    INTCONbits.RBIE = 1;        // Enable RB interrupt
    INTCONbits.PEIE = 1;        // enable periperal interrupt
    PIE1bits.RCIE = 1;          // 
}

void timer0Setup(){
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 7;
    OPTION_REGbits.T0CS = 0;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    TMR0 = 0;
}

void ADCsetup(){
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    ADCON0bits.ADON = 1;
    ADCON0bits.CHS2 = 0; ADCON0bits.CHS1 = 0; ADCON0bits.CHS0 = 0;
    ADCON0bits.ADCS1 = 0; ADCON0bits.ADCS0 = 1;
    ADCON1 = 0b10000100;
    GO_nDONE = 1;
}

char cmdUart = 0;
char statusDevice = 0;
int adcValue = 0;
int kelvin = 0;
char position = 0;
//char countLightOn (char port) {
//    char count = 0;
//    char buffer = 0;
//    for (int i = 0; i < 5; i++){
//        buffer = port >> i;
//        if (buffer & 1) count++;
//    }
//    return count;
//}
//char countFanOn (char port) {
//    char count = 0;
//    char buffer = 0;
//    for (int i = 0; i < 3; i++){
//        buffer = port >> (i + 5);
//        if (buffer & 1) count++;
//    }
//    return count;
//}

void __interrupt() ISR(void){
    if(PIR1bits.RCIF == 1){
        cmdUart = RCREG >> 4;
        if (cmdUart == 1){
            if (autoMode == 0){
                statusDevice = RCREG & 0b00001000;
                position = RCREG & 0b00000111;
                if (statusDevice == 0b00001000) PORTD |= (1 << position);
                if (statusDevice == 0) PORTD &= ~(1 << position);
            }
        }
        if(cmdUart == 2){
            autoMode = 1;
        }
        if(cmdUart == 3){
            autoMode = 0;
        }
        if(cmdUart == 4){
            PORTD = 0xff;
        }
        if(cmdUart == 5){
            PORTD = 0x00;
        }
        PIR1bits.RCIF = 0;   
    }
    
    if(INTCONbits.RBIF && andSensor2 == 1){
        if(PORTBbits.RB4 == 1){
            
            if(checkState2 == 1){
                sensor1State = 0;
            }
            else if(sensor2State == 0){
                sensor1State = 1;
            }
            else{
                sensor1State = 0;
            }
        }
        andSensor2 = 0;
        sensor2State = 0;
        INTCONbits.RBIF = 0;
    }
    
    if(INTCONbits.RBIF && andSensor2 == 0){
        INTCONbits.RBIF = 0;
        
        if(PORTBbits.RB4 == 1){
            if(sensor2State == 0){
                sensor1State = 1;
            }
            else{
                sensor1State = 0;
            }
        }
        checkState2 = 0;
    }
    
    if (INTCONbits.INTF) // Check RB0 change
    {
        andSensor2 = 1;
        if(sensor1State != 1){
            sensor2State = 1;
            checkState2 = 1;
        }
        checkState();
    }
    INTCONbits.INTF = 0; // Clear RB interrupt flag
    
    
    //TIMER AND ADC
    if(INTCONbits.TMR0IF){
        GO_nDONE = 1;
        TMR0 = 0;
        INTCONbits.TMR0IF = 0;
    }
    
    if(PIR1bits.ADIF){
        adcValue = (ADRESH << 8) + ADRESL ; // 0-1024
        adcValue = adcValue - adcAdjust;
        float adcFloat = (float)adcValue;
        kelvin = ((adcFloat/1024.0 * 5040.0) / 10.0);
        temp = kelvin - 273; // 0- 231
        //temp = 29;
        if(autoMode == 1 && countStudent > 0){
            if(temp >= 0 && temp < 25){
                Fan1 = 1;
                Fan2 = 0;
                Fan3 = 0;
            }
            if(temp >= 25 && temp < 30){
                Fan1 = 1;
                Fan2 = 1;
                Fan3 = 0;
            }
            if(temp >= 30){
                Fan1 = 1;
                Fan2 = 1;
                Fan3 = 1;
            }
        }
        if(autoMode == 1 && countStudent < 0){
            Fan1 = 0;
            Fan2 = 0;
            Fan3 = 0;
        }
        PIR1bits.ADIF = 0;
    }
}

void main(void) {
    TRISB = 0b00010001;
    TRISD = 0;
    PORTD = 0;
    TRISC = 0x80;
    PORTC = 0;
    TRISA = 0b00000001;
    PORTA = 0;
    TRISE = 0;
    ADCON1 |= (0 << 3) | (1 << 2) | (0 << 1) | (0 << 0);
    lcd_init();
    interruptsSetup();
    uartSetup(9600);
    timer0Setup();
    ADCsetup();
    char line_homePage = 1, homepage = 1, inside_option = 0;
    while(1){
        if (but1 == 1){
            while (but1 == 1);
            line_homePage++;
            
            if (line_homePage == 3) { // neu line = 3 thi chuyen qua page 2 luc nay se xoa page 1
                lcd_clear();
                homepage = 2;
            }
            if (line_homePage > 4) { // reset lai neu tran
                line_homePage = 1;
                homepage = 1;
            }
            
            // lenh dat vi tri mui ten
            lcd_setCursor(0, 0); lcd_write(' ');
            lcd_setCursor(1, 0); lcd_write(' ');
            if (line_homePage == 1 || line_homePage == 3) lcd_setCursor(0, 0);
            if (line_homePage == 2 || line_homePage == 4) lcd_setCursor(1, 0);
            lcd_write(0b01111110);
            
            // lenh dat noi dung page
            if (homepage == 1){
                lcd_setCursor(0, 1); lcd_print_str(option1);
                lcd_setCursor(1, 1); lcd_print_str(option2);
            }
            if (homepage == 2){
                lcd_setCursor(0, 1); lcd_print_str(option3);
                lcd_setCursor(1, 1); lcd_print_str(option4);
            }
        }
        if (but2 == 1){
            while (but2 == 1);
            //=====================option1_inside===================
            if (line_homePage == 1){
                inside_option = 1;
                //code here
                lcd_clear();
                lcd_setCursor(0, 0);
                lcd_print_str(option1_select1);
                lcd_setCursor(1, 0);
                lcd_print_str(option1_select2);
                //=========
                while (inside_option == 1){  
                    //code here
                    //countStudent = adcValue;
                    char tram_count = countStudent / 100;
                    char chuc_count = (countStudent / 10) % 10;
                    char donvi_count = countStudent % 10;
                    lcd_setCursor(0, 10);
                    if (tram_count == 0) lcd_write(' ');
                    else lcd_write(tram_count + 48);
                    if (chuc_count == 0 && tram_count == 0) lcd_write(' ');
                    else lcd_write(chuc_count + 48);
                    lcd_write(donvi_count + 48);
                    
                    //temp = 0;
                    char tram_temp = temp / 10;
                    char chuc_temp = temp % 10;
                    lcd_setCursor(1, 11);
                    lcd_write(tram_temp + 48);
                    lcd_write(chuc_temp + 48);
                    lcd_write(0b11011111);
                    lcd_write('C');
                    //=========
                    if (but3 == 1){
                        while(but3 == 1);
                        inside_option = 0;
                    }
                }
                lcd_clear();
                lcd_setCursor(0, 0);
                lcd_write(0b01111110);
                lcd_setCursor(0, 1);
                lcd_print_str(option1);
                lcd_setCursor(1, 1);
                lcd_print_str(option2);
            }
            
            //=====================option2_inside===================
            if (line_homePage == 2){
                inside_option = 1;
                //code here
                char line_option2Page = 1, option2page = 1;
                lcd_clear();
                lcd_setCursor(0, 0); lcd_write(0b01111110); 
                lcd_setCursor(0, 1); lcd_print_str(option2_select1);
                lcd_setCursor(1, 1); lcd_print_str(option2_select2);
                if (Light1 == 1) { lcd_setCursor(0, 11); lcd_print_str("on "); }
                if (Light1 == 0) { lcd_setCursor(0, 11); lcd_print_str("off"); }
                if (Light2 == 1) { lcd_setCursor(1, 11); lcd_print_str("on "); }
                if (Light2 == 0) { lcd_setCursor(1, 11); lcd_print_str("off"); }
                
                if (autoMode == 1){
                    lcd_clear();
                    lcd_setCursor(0, 3); 
                    lcd_print_str(option4_select1);
                }
                //=========
                while (inside_option == 1){
                    //code here
                    if (autoMode == 0){
                        if (but1 == 1){
                            while (but1 == 1);
                            line_option2Page++;

                            if (line_option2Page == 3) { // neu line = 3 thi chuyen qua page 2 luc nay se xoa page 1
                                lcd_clear();
                                option2page = 2;
                            }
                            if (line_option2Page == 5) { // neu line = 3 thi chuyen qua page 2 luc nay se xoa page 1
                                lcd_clear();
                                option2page = 3;
                            }
                            if (line_option2Page > 5) { // reset lai neu tran
                                line_option2Page = 1;
                                option2page = 1;
                            }

                            // lenh dat vi tri mui ten
                            lcd_setCursor(0, 0); lcd_write(' ');
                            lcd_setCursor(1, 0); lcd_write(' ');
                            if (line_option2Page == 1 || line_option2Page == 3 || line_option2Page == 5) lcd_setCursor(0, 0);
                            if (line_option2Page == 2 || line_option2Page == 4) lcd_setCursor(1, 0);
                            lcd_write(0b01111110);

                            // lenh dat noi dung page
                            if (option2page == 1){
                                lcd_setCursor(0, 1); lcd_print_str(option2_select1);
                                lcd_setCursor(1, 1); lcd_print_str(option2_select2);
                                lcd_setCursor(0, 11);
                                if (Light1 == 1) lcd_print_str("on "); else lcd_print_str("off");
                                lcd_setCursor(1, 11);
                                if (Light2 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (option2page == 2){
                                lcd_setCursor(0, 1); lcd_print_str(option2_select3);
                                lcd_setCursor(1, 1); lcd_print_str(option2_select4);
                                lcd_setCursor(0, 11);
                                if (Light3 == 1) lcd_print_str("on "); else lcd_print_str("off");
                                lcd_setCursor(1, 11);
                                if (Light4 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (option2page == 3){
                                lcd_setCursor(0, 1); lcd_print_str(option2_select5);
                                lcd_setCursor(0, 11);
                                if (Light5 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                        }
                        if (but2 == 1){
                            while (but2 == 1);
                            if (line_option2Page == 1) {
                                Light1 = ~Light1;
                                lcd_setCursor(0, 11);
                                if (Light1 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (line_option2Page == 2) {
                                Light2 = ~Light2;
                                lcd_setCursor(1, 11);
                                if (Light2 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (line_option2Page == 3) {
                                Light3 = ~Light3;
                                lcd_setCursor(0, 11);
                                if (Light3 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (line_option2Page == 4) {
                                Light4 = ~Light4;
                                lcd_setCursor(1, 11);
                                if (Light4 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (line_option2Page == 5) {
                                Light5 = ~Light5;
                                lcd_setCursor(0, 11);
                                if (Light5 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                        }
                    }
                    //=========
                    if (but3 == 1){
                        while(but3 == 1);
                        inside_option = 0;
                    }
                }
                lcd_clear();
                lcd_setCursor(1, 0);
                lcd_write(0b01111110);
                lcd_setCursor(0, 1);
                lcd_print_str(option1);
                lcd_setCursor(1, 1);
                lcd_print_str(option2);
            }
            
            //=====================option3_inside===================
            if (line_homePage == 3){
                inside_option = 1;
                //code here
                char line_option3Page = 1, option3page = 1;
                lcd_clear();
                lcd_setCursor(0, 0); lcd_write(0b01111110);
                lcd_setCursor(0, 1); lcd_print_str(option3_select1);
                lcd_setCursor(1, 1); lcd_print_str(option3_select2);
                if (Fan1 == 1) { lcd_setCursor(0, 11); lcd_print_str("on "); }
                if (Fan1 == 0) { lcd_setCursor(0, 11); lcd_print_str("off"); }
                if (Fan2 == 1) { lcd_setCursor(1, 11); lcd_print_str("on "); }
                if (Fan2 == 0) { lcd_setCursor(1, 11); lcd_print_str("off"); }
                
                if (autoMode == 1){
                    lcd_clear();
                    lcd_setCursor(0, 3); 
                    lcd_print_str(option4_select1);
                }
                //=========
                while (inside_option == 1){
                    //code here
                    if (autoMode == 0){
                        if (but1 == 1){
                            while (but1 == 1);
                            line_option3Page++;

                            if (line_option3Page == 3) { // neu line = 3 thi chuyen qua page 2 luc nay se xoa page 1
                                lcd_clear();
                                option3page = 2;
                            }
                            if (line_option3Page > 3) { // reset lai neu tran
                                line_option3Page = 1;
                                option3page = 1;
                            }

                            // lenh dat vi tri mui ten
                            lcd_setCursor(0, 0); lcd_write(' ');
                            lcd_setCursor(1, 0); lcd_write(' ');
                            if (line_option3Page == 1 || line_option3Page == 3) lcd_setCursor(0, 0);
                            if (line_option3Page == 2) lcd_setCursor(1, 0);
                            lcd_write(0b01111110);

                            // lenh dat noi dung page
                            if (option3page == 1){
                                lcd_setCursor(0, 1); lcd_print_str(option3_select1);
                                lcd_setCursor(1, 1); lcd_print_str(option3_select2);
                                lcd_setCursor(0, 11);
                                if (Fan1 == 1) lcd_print_str("on "); else lcd_print_str("off");
                                lcd_setCursor(1, 11);
                                if (Fan2 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (option3page == 2){
                                lcd_setCursor(0, 1); lcd_print_str(option3_select3);
                                lcd_setCursor(0, 11);
                                if (Fan3 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                        }
                        if (but2 == 1){
                            while (but2 == 1);
                            if (line_option3Page == 1) {
                                Fan1 = ~Fan1;
                                lcd_setCursor(0, 11);
                                if (Fan1 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (line_option3Page == 2) {
                                Fan2 = ~Fan2;
                                lcd_setCursor(1, 11);
                                if (Fan2 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                            if (line_option3Page == 3) {
                                Fan3 = ~Fan3;
                                lcd_setCursor(0, 11);
                                if (Fan3 == 1) lcd_print_str("on "); else lcd_print_str("off");
                            }
                        }
                    }
                    //=========
                    if (but3 == 1){
                        while(but3 == 1);
                        inside_option = 0;
                    }
                }
                lcd_clear();
                lcd_setCursor(0, 0);
                lcd_write(0b01111110);
                lcd_setCursor(0, 1);
                lcd_print_str(option3);
                lcd_setCursor(1, 1);
                lcd_print_str(option4);
            }
            
            //=====================option4_inside===================
            if (line_homePage == 4){
                inside_option = 1;
                //code here
                char line_option4Page = 1;
                lcd_clear();
                lcd_setCursor(0, 0); lcd_write(0b01111110);
                lcd_setCursor(0, 1); lcd_print_str(option4_select1);
                lcd_setCursor(1, 1); lcd_print_str(option4_select2);
                if (autoMode == 1) { lcd_setCursor(0, 13); lcd_write(0b11111111); }
                if (autoMode == 0) { lcd_setCursor(1, 13); lcd_write(0b11111111); }
                //=========
                while (inside_option == 1){
                    //code here
                    if(autoMode == 1){
                            lcd_setCursor(0, 13); lcd_write(0b11111111);
                            lcd_setCursor(1, 13); lcd_write(' ');
                        }
                    if(autoMode == 0){
                        lcd_setCursor(0, 13); lcd_write(' ');
                        lcd_setCursor(1, 13); lcd_write(0b11111111);
                    }
                    if (but1 == 1){
                        while (but1 == 1);
                        line_option4Page++;
                        
                        if (line_option4Page > 2) line_option4Page = 1;
            
                        // lenh dat vi tri mui ten
                        lcd_setCursor(0, 0); lcd_write(' ');
                        lcd_setCursor(1, 0); lcd_write(' ');
                        if (line_option4Page == 1) lcd_setCursor(0, 0);
                        if (line_option4Page == 2) lcd_setCursor(1, 0);
                        lcd_write(0b01111110);
                    }
                    if (but2 == 1){
                        while (but2 == 1);
                        if (line_option4Page == 1) {
                            autoMode = 1;
                        }
                        if (line_option4Page == 2) {
                            autoMode = 0;
                        }
                    }
                    //=========
                    if (but3 == 1){
                        while(but3 == 1);
                        inside_option = 0;
                    }
                }
                lcd_clear();
                lcd_setCursor(1, 0);
                lcd_write(0b01111110);
                lcd_setCursor(0, 1);
                lcd_print_str(option3);
                lcd_setCursor(1, 1);
                lcd_print_str(option4);
            }
            //=======================================================
        }
    }
    return;
}