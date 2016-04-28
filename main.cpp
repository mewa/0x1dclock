#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "CyclicBuffer.h"

#define DISP0 PB0
#define DISP1 PB1
#define DISP2 PC0
#define DISP3 PC1
#define DISP4 PC2
#define DISP5 PC3
#define DISP6 PC4
#define DISP7 PC5

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

#define SIZE 128

size_t offset = 0;
unsigned char* disp_buffer[SIZE] = { 0 };
size_t buf_pos = 0;
size_t buf_size = 0;

void add_char(unsigned char* chr) {
  disp_buffer[buf_size++] = chr;
}

void next_char() {
  buf_pos++;
  if (buf_pos >= SIZE || buf_pos >= buf_size) {
    buf_pos = 0;
  }
}

ISR(TIMER1_COMPA_vect) {
  if (offset++ + 1 >= CHAR_WIDTH) {
    offset = 0;
    next_char();
  }
  PORTB ^= (1 << PB2);
}

unsigned char A[CHAR_WIDTH] = { '\xfe', '\x11', '\x11', '\x11', '\x11', '\xfe' };
unsigned char C[CHAR_WIDTH] = { '\xff', '\x81', '\x81', '\x81', '\x81', '\x81' };
unsigned char SPACE[CHAR_WIDTH] = { '\x0', '\x0', '\x0', '\x0', '\x0', '\x0' };

inline void start_timer0() {
  OCR1A = 0xFFF;//15625;
  
  TCCR1B |= (1 << WGM12);
  // Mode 4, CTC on OCR1A
  
  TIMSK |= (1 << OCIE1A);
  //Set interrupt on compare match
  
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // set prescaler to 1024 and start the timer
  
  sei();
}

int main(int argc, char** argv) {
  DDRB = (1 << DDB1) | (1 << DDB0) | (1 << DDB2);
  DDRC = 0x7f;

  start_timer0();

  add_char(A);
  add_char(SPACE);
  add_char(C);
  add_char(SPACE);
  
  unsigned char c = 0;
  unsigned char* p = 0;
  while (1) {
    p = disp_buffer[buf_pos];
    c = p[offset];    
    
    PORTB = 0x3 & c;
    PORTC = (c & 0xfc) >> 2;
  }
  return 0;
}
