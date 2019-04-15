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
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>


//#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   9     // Configurable, see typical pin layout above
#define SS_PIN    10    // Configurable, see typical pin layout above


#define FOSC    16000000 // Clock Speed
#define BAUD    9600
#define MYUBRR  FOSC/16/BAUD-1
#define CHAVE   PB0
#define TRIGGER 7
#define ECHO    4
#define MOTOR_1 PD6
#define MOTOR_2 PD3
#define LED_1   PD2
#define BOTAO   PD7



//MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
//MFRC522::MIFARE_Key key;

//==========================GLOBALS ============================

uint8_t dt_cycle_1 = 255;
uint8_t dt_cycle_2 = 0;
int t = 1;


//==============================================================

void print_int(char n);
void run_motor1();
void run_motor2();
void usart_init( unsigned int ubrr);
void usart_transmit( unsigned char data );
void usart_print(char *string,unsigned int len);
void usart_println(char *string,unsigned int len);
/*int rfid_authentication();*/

void setup() {
	//pins RFID pinos: 9-13 PORTB
  	//SPI.begin();         // Init SPI bus
	//mfrc522.PCD_Init();  // Init MFRC522 card

	//======================chave on / off leds======================
	DDRB &= ~(1 << CHAVE);//pb0 input 
	PORTB |= (1 << CHAVE);//pull up	
	DDRD |= (1 << LED_1);
 
  DDRD &= ~(1 << BOTAO);
  PORTD |= (1 << BOTAO);//pull up

  //======================USART======================

  usart_init(MYUBRR);

	//======================sensor Ultrassonico======================
	


	//======================contadores pwm===========================
	/* saidas pwm: pino PD6 e PD3, OC0A e OC2B */	
	DDRD |= (1 << MOTOR_1) | (1 << MOTOR_2);
  
	/*fast pwm*/
	TCCR2A |= (1 << COM2A1) | (3 << WGM20);
	TCCR2B |= (5 << CS20);//prescaler  
	
	TCCR0A |= (1 << COM0A1) | (3 << WGM00);
	
	TCCR0B &= ~(7);//prescaler  off 1 8 32 64 128 256 1024 
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
  int botao_apertado = 0;
  int autorizado = 0;
  int i=0;
  
	while(1){
		if(i%100==0)
      print_int(dt_cycle_1);
    i++;
		if(PINB & (1 << CHAVE)){
			
			if( !(PIND & (1 << BOTAO)) && botao_apertado == 0 ){
          botao_apertado = 1;
          run_motor1();
          botao_apertado = 0;
          autorizado = 1;
			}
     if(autorizado){
        

      
     }
		}
		else{
			//run_motor1();
			PORTD &= ~(1 << LED_1);
		}
	}
	return 0;
}


//debug functions
void print_int(uint8_t n){
  uint8_t r;
  uint8_t buf[16];
  int i=14;
  buf[15] = 0;
  while(n > 0){
    r = n % 10;
    buf[i] = r+48;
    i--;
    n /= 10;
  }
  //for(i++;buf[
  usart_println((uint8_t*)buf+i+1);  
}


ISR(TIMER2_OVF_vect){
  cli();
  OCR0A = dt_cycle_1--;
  

   if(dt_cycle_1 <= 0) dt_cycle_1 = 255;

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
  TCCR0B |= (5 << CS00);//prescaler  off 1 8 32 64 128 256 1024 
  _delay_ms(4000);
  OCR0A = 255;
  dt_cycle_1 = 255;
  TCCR0B &= ~(7);//prescaler  off 1 8 32 64 128 256 1024
}

void run_motor2(){
  
}



void usart_init( unsigned int ubrr)
{
  /*Set baud rate */
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
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
void usart_print(uint8_t *string){
  for(unsigned i=0;string[i]!=0;i++)
    usart_transmit(string[i]);
}

void usart_println(uint8_t *string){
  usart_print(string);
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
