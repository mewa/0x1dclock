#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* 	
	wszystkie funkcje opisane w pdf
 	http://www.atmel.com/images/atmel-8159-8-bit-avr-microcontroller-atmega8a_datasheet.pdf
 */

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 6

#define SIZE 128

#define TIMER0_PRESCALER ((1 << CS02))
//  | (1 << CS00)
#define TIMER1_PRESCALER ((1 << CS10))
//  | (1 << CS10)

size_t offset = 0;
unsigned char* disp_buffer[SIZE] = { 0 };
size_t buf_pos = 0;
size_t buf_size = 0;

size_t ovf = 0;
size_t ints = 0;


// dodaje znak 6x6 do bufora wyswietlania
void add_char(unsigned char* chr) {
  disp_buffer[buf_size++] = chr;
}

void next_char() {
  buf_pos++;
  if (buf_pos >= SIZE || buf_pos >= buf_size) {
    buf_pos = 0;
  }
}

void next_offset() {
  if (offset++ + 1 >= CHAR_WIDTH) {
    offset = 0;
    next_char();
  }
  ++ovf;
  DDRD |= (1 << PD0);
  PORTD ^= (1 << PD0);
}

// unused
inline void wait_tim0(unsigned char time) {
  TIMSK |= 1 << TOIE0;
  TCNT0 = 0xFF - time;
  TCCR0 = (1 << CS02) | (1 << CS00);
}

ISR(TIMER1_COMPA_vect) {
  if (offset++ + 1 >= CHAR_WIDTH) {
    offset = 0;
    next_char();
  }
  ++ovf;
  DDRD |= (1 << PD0);
  PORTD ^= (1 << PD0);
}

short icr = 0;

ISR(TIMER1_CAPT_vect) {
  icr = ICR1;
  TCNT1 = 0;
}

/*ISR(TIMER0_OVF_vect) {
  GICR |= 1 << INT0;
  TCCR0 = 0;
  
  if (offset++ + 1 >= CHAR_WIDTH) {
    offset = 0;
    next_char();
  }
  ++ovf;
  DDRD |= (1 << PD0);
  PORTD ^= (1 << PD0);
}*/

unsigned char A[CHAR_WIDTH] = { '\xfe', '\x11', '\x11', '\x11', '\x11', '\xfe' };
unsigned char C[CHAR_WIDTH] = { '\xff', '\x81', '\x81', '\x81', '\x81', '\x81' };
unsigned char SPACE[CHAR_WIDTH] = { '\x0', '\x0', '\x0', '\x0', '\x0', '\x0' };

// start input capture timer
inline void start_timer1() {
  TCCR1B |= (0 << ICES1) | (1 << ICNC1);
  TCCR1B |= TIMER1_PRESCALER;
  
  TIMSK |= (1 << TICIE1);
}

int main(int argc, char** argsv) {
  DDRB = (1 << DDB1) | (0 << DDB0) | (1 << DDB2);
  DDRC = 0x7f;

  // ICP
  PORTB = (1 << PB0);
  
  // setup timer1 for IC
  start_timer1();

  // enable interrupts
  sei();

  add_char(A);
  add_char(SPACE);
  add_char(C);
  add_char(SPACE);

  unsigned short interval = 0, c = 0;
  icr = 0xffff;
  while (1) {
    interval = icr / 360 + 1;
    c = disp_buffer[buf_pos][offset];
  
    TCNT0 = 0xff - interval;
    TCCR0 |= TIMER0_PRESCALER;
    for (int i = 1; i < 0xff; ++i) {
      while (!(TIFR & (1 << TOV0)));
      TIFR &= 1 << TOV0;
      TCNT0 = 0xff - interval;
    }
    TCCR0 &= ~TIMER0_PRESCALER;
    next_offset();

    PORTB = (0x3 & c) << 1;
    PORTC = (c & 0xfc) >> 2;
  }
  return 0;
}
