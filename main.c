/*
	Controle de acesso com RFID - Sistemas digitais - UFRN
	Professor: Sergio Natan
	componentes:
		-	Heitor Dantas
		-	Mateus Rodrigues	
		-	Luiza  Salviano

	FUNCIONAMENTO:
		* leds de sinalizacao
		Se o sensor ultrassonico acusa presenca
			a cancela 1 (motor 1) eh aberta
			espera o carro passar
			fecha cancela 1
			
			*flag pra indicar que existe um carro na gaiola

			verificacao RFID
				caso possua id valido:
					rotina de abrir cancela 2
					guarda num buffer o ID que entrou
			

			interrupcao de contagem para mandar o buffer para arquivo
			de log usando USART; 

	TODO:
		(ok) logica da chave
		()   sensor ultrassonico
		() 	 verificacao rfid - estrutura pronta falta integrar no programa
		()   rotina para acionamento dos motores 
		()   arquivo de log


*/



#include <avr/io.h>

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define CHAVE PB0
#define TRIGGER PD3
#define ECHO PD4
#define MOTOR_1 PD5
#define MOTOR_2 PD6
#define LED_1 PD2
#define LED_2 PD7
#define INPUT 0
#define OUTPUT 1
#define PULL_UP 1


void usart_init( unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void usart_transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRnA & (1<<UDREn)) )
	;
	/* Put data into buffer, sends the data */
	UDRn = data;
}


void setup() {
	//pins RFID pinos: 9-13 PORTB
	//MFRC522();	

	//chave on / off leds
	DDRB &= ~(1 << CHAVE);//pb0 input 
	PORTB |= (1 << CHAVE);//pull up	
	DDRD |= (1 << LED_1) | (1 << LED_2);
	DDRD |= (1 << MOTOR_1) | (1 << MOTOR_2);	

	//sensor Ultrassonico
	//Ultrassonic();
	usart_init(MYUBRR);
} 

int main(){

	setup();

	while(1){
		if(PINB & (1 << CHAVE)){ 
			PORTD |= (1 << LED_1);
			
		}
		else{
			
			PORTD &= ~(1 << LED_1);
		}
	}
	return 0;
}
