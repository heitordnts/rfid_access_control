#include <avr/io.h>

#define FCPU 16000000
#define BAUD 9600
#define MYUBRR FCPU/16/BAUD-1


int main(){

  usart_init(MYUBRR);

  
  
  
  while(1){
    usart_transmit('a');    
  }


  return 0;
}
void usart_init(unsigned int ubrr){
  //BAUD RATE
  UBRR0H = (unsigned char) (ubrr >> 8);
  UBRR0L = (unsigned char) ubrr;
  //enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  //frame format
  UCSR0C = (1<<USBS0) | (3 << UCSZ00);

}

void usart_transmit( unsigned char data )
{
/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) )
;
/* Put data into buffer, sends the data */
UDR0 = data;
}


unsigned char usart_receive( void )
{
/* Wait for data to be received */
while ( !(UCSR0A & (1<<RXC0)) )
;
/* Get and return received data from buffer */
return UDR0;
}

void usart_flush( void )
{
unsigned char dummy;
while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}

