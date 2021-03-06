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
#include <avr/interrupt.h>
//#include <SPI.h>
//#include <MFRC522.h>


#define RST_PIN   9     // Configurable, see typical pin layout above
#define SS_PIN    10    // Configurable, see typical pin layout above


#define FOSC    16000000 // Clock Speed
#define BAUD    9600
#define MYUBRR  FOSC/16/BAUD-1
#define CHAVE   PB0
#define TRIGGER PD3
#define ECHO    PD4
#define MOTOR_1 PD6
#define MOTOR_2 PD3
#define LED_1   PD2
#define LED_2   PD7


//MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
//MFRC522::MIFARE_Key key;

//Ultrassonic

int dt_cycle_1 = 0;
int dt_cycle_2 = 0;
int t = 1;

ISR(TIMER0_OVF_vect){
	cli();

//	OCR0A = dt_cycle_1--;
	if(dt_cycle_1 > 1473) {
		
		dt_cycle_1 = 0;
		t ^= 1;
	}
	dt_cycle_1++;
	//if(dt_cycle_1 <= 1)
	//	dt_cycle_1 = 255;

	sei();
} 

ISR(TIMER2_OVF_vect){
	cli();
	//OCR2B = dt_cycle_2--;
	if(dt_cycle_2 <= 1)
		dt_cycle_2 = 255;
	sei();
} 


void run_motor1(){
	//decrementa o dutycycle 	
	if(t == 1)
		OCR0A = 255;
	else
		OCR0A = 00;

	//dt_cycle_1 = 255;

}


void run_motor2(){}



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
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
void usart_print(char *string,unsigned int len){
	for(unsigned i=0;i< len;i++)
		usart_transmit(string[i]);
}

void usart_println(char *string,unsigned int len){
	usart_print(string,len);
	usart_transmit('\n');
}
/*
int rfid_authentication(){

	if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
		delay(50);
		return -1;//error RFID read
	}


	usart_print("UID: ");
	for (byte i = 0; i < mfrc522.uid.size; i++) {
		usart_transmit(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
		usart_transmit(mfrc522.uid.uidByte[i], HEX);
	} 
}
*/

void setup() {
	//pins RFID pinos: 9-13 PORTB
  	//SPI.begin();         // Init SPI bus
	//mfrc522.PCD_Init();  // Init MFRC522 card

	//======================chave on / off leds======================
	DDRB &= ~(1 << CHAVE);//pb0 input 
	PORTB |= (1 << CHAVE);//pull up	
	DDRD |= (1 << LED_1) | (1 << LED_2);

	//======================sensor Ultrassonico======================
	//Ultrassonic();
	usart_init(MYUBRR);

	//======================contadores pwm===========================
	/* saidas pwm: pino PD6 e PD3, OC0A e OC2B */	
	DDRD |= (1 << MOTOR_1) | (1 << MOTOR_2);

	/*fast pwm*/
	TCCR2A |= (1 << COM2A1) | (3 << WGM20);
	TCCR2B |= (5 << CS20);//prescaler  
	
	TCCR0A |= (1 << COM0A1) | (3 << WGM00);
	TCCR0B |= (3 << CS00);//prescaler  off 1 8 32 64 128 256 1024 
	
	/* interrupcao overflow*/
	TIMSK2 |= 1; 
	TIMSK0 |= 1; 
	
	OCR0A = 0; 
	OCR2B = 0;

	sei();

	//=========================================================

	/* configuracao de interrupcao externa
	DDRD &= ~(1 << DDD2);     // Clear the PD2 pin
    // PD2 (PCINT0 pin) is now an input

    PORTD |= (1 << PORTD2);    // turn On the Pull-up
    // PD2 is now an input with pull-up enabled

	EICRA |= ( (1 << ISC01) | (1 << ISC00) );
	EIMSK |= (1 << INT0); //ENABLE INT0
	*/
} 

int main(){

	setup();

	while(1){
			
		if(PINB & (1 << CHAVE)){ 
			PORTD |= (1 << LED_1);
			
		}
		else{
			run_motor1();
			PORTD &= ~(1 << LED_1);
		}
	}
	return 0;
}
