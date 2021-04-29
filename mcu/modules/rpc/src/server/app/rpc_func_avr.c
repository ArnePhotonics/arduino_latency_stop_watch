/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include "../../../include/rpc_transmission/server/app/qt2mcu.h"
#include "board.h"
#include "globals.h"
#include "vc.h"

#include "Arduino.h"

void reset_all();

void start_timer(arduino_dig_pin_trigger_t triggered_by, arduino_dig_edge_t edge);
void define_roundstop_pin(arduino_dig_pin_roundstop_t pin, arduino_dig_edge_t edge);


static uint16_t timer_overflow_counter = 0;

ISR(TIMER1_OVF_vect)        
{
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1); // LED ein und aus
    timer_overflow_counter++;
}

void timer_init(){
    //Timer1 is used normally by servo library
    TCCR1B = 0; //Timer1 is a 16bit timer
    TCCR1B |= (1 << CS10) | (1 << CS12); //1024 prescaler --> 64us/tick 
    TCNT1 = 0;
    TIMSK1 |= (1 << TOIE0);
    timer_overflow_counter = 0;
}

round_times_t get_round_times(void){
    round_times_t result;
    result.ain0 = analogRead(0);
    result.ain1 = analogRead(1);
    result.ain2 = analogRead(2);
    result.ain3 = analogRead(3);
    result.ain4 = analogRead(4);
    result.ain5 = analogRead(5);
    return result;
}

uint8_t get_digital_value(uint8_t pin_number){
    uint8_t result = digitalRead(pin_number) == HIGH;
    return result;
}

void set_digital_value(uint8_t pin_number, uint8_t value){
    if (value){
      digitalWrite(pin_number, HIGH);
    }else{
      digitalWrite(pin_number, LOW);
    }
}

void set_digital_direction(uint8_t pin_number, uint8_t output_direction){
    if (output_direction){
      pinMode(pin_number, OUTPUT);
    }else{
      pinMode(pin_number, INPUT);
    }
}

device_descriptor_v1_t get_device_descriptor(void) {
    device_descriptor_v1_t descriptor = {
        .githash = GITHASH,
        .gitDate_unix = GITUNIX,
        .deviceID = 0,
        .guid = {0},
        .boardRevision = 0,
        .name = "ArduWatch",
        .version = "-",
    };
    return descriptor;
}
