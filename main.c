/******************************************************************************************************************/
/*                                         AcessoPJ - AKA Cerberus - ver1.0                                       */
/*                                                                                                                */
/*    Descrição: Firmware responsavel pela abertura automatica da porta da PJ.                                    */
/*                                                                                                                */
/*        O microcontrolador recebe do PC (NEO) atraves da porta serial o comando de abertura                     */
/*        da porta, o software no PC fica responsavel por comparar o nº USP recebido do leitor                    */
/*        de codigo de barra com o database e mandar a mensagem coerente com a situação.                          */
/*                                                                                                                */
/*        Por conveniencia, o microcontrolador possui um led bicolor que ira acender dependendo da situação.      */
/*                                                                                                                */
/*                       Situação                  |  Comando  |   Cor do LED                                     */
/*                       Acesso Autorizado         |    #O     |    Verde                                         */
/*                       Acesso Negado             |    #C     |    Vermelho                                      */
/*                       Codigo de Barra Invalido  |    #I     |    Laranja                                       */
/*                                                                                                                */
/*                                                                                                                */
/*    Data: 23/02/10                                                                                              */
/*    Idealizadores: Artur Souza Polizel, Camilo Rodegheri Mendes dos Santos e Marcelo Pesse                      */
/*    Programador: Marcelo Pesse (www.raijuu.net)                                                                 */
/*                                                                                                                */
/******************************************************************************************************************/

#include <stdio.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>

#define FOSC 12000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

#define vermelho 0b00001000
#define verde 0b00010000
#define laranja 0b00011000

void init_usart();
static int uart_putchar(char c, FILE *stream);
uint8_t uart_getchar();
void wait(int time);
void abre_porta();
void liga_led(int led);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void init_usart(){
    DDRD |= 0b00000010;
    DDRD &= ~(0b00000001);

    UBRRH = (8<<MYUBRR);
    UBRRL = MYUBRR;
    UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
    UCSRC = (3<<UCSZ0);

    stdout = &mystdout;
}

static int uart_putchar(char c, FILE *stream){
    if (c == '\n') uart_putchar('\r', stream);
    loop_until_bit_is_set(UCSRA, UDRE);
    UDR = c;
    return 0;
}

uint8_t uart_getchar(){
    while( !(UCSRA & (1<<RXC)) );
    return(UDR);
}

void wait(int time){
   int i;
   for(i = 0; i < time; i++){
      _delay_loop_2(0);
   }
}

void abre_porta(){
    PORTD |= 0b00100000;
    wait(1);
    PORTD &= 0b00000000;
}

void liga_led(int led){
    PORTB |= led;
    wait(30);
    PORTB &= 0b00000000;
}

ISR(USART_RX_vect){
    char op;
    
    if (uart_getchar() == '#'){
        op = uart_getchar();
        if (op == 'O' || op == 'o'){
            abre_porta();
            liga_led(verde);
        }
        if (op == 'C' || op == 'c'){
            liga_led(vermelho);
        }
        if (op == 'I' || op == 'i'){
            liga_led(laranja);
        }
    }
}

int main(){   
    
    char op;
    init_usart();
    DDRB |= 0b00011000;
    DDRD |= 0b00100000;
    
    liga_led(vermelho);
    liga_led(laranja);
    liga_led(verde);

    sei();

    for(;;){
        sleep_mode();
    }
}
