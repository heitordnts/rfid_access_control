#include <avr/io.h>

#define CHAVE PB0
#define TRIGGER PD3
#define ECHO PD4
#define MOTOR_1 PD5
#define MOTOR_2 PD6
#define LED_1 PD2
#define LED_2 PD7

void setup() {
	//pins RFID pinos: 9-13 PORTB
	//MFRC522();	

	//chave on / off
	PORTB |= (1 << CHAVE); 

	//sensor Ultrassonico
	//Ultrassonic();

} 
int main(){

	setup();

	while(1){
		


	}

	return 0;
}
