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

#define SIZE 128;

size_t offset = 0;


ISR(TIMER1_COMPA_vect) {
  if (offset++ + 1 >= CHAR_WIDTH) {
    offset = 0;
  }
  PORTB ^= (1 << PB2);
}

char A[CHAR_WIDTH] = { '\xfe', '\x11', '\x11', '\x11', '\x11', '\xfe' };
char SPACE[CHAR_WIDTH] = { '\x0', '\x0', '\x0', '\x0', '\x0', '\x0' };
  
inline void start_timer0() {
  OCR1A = 15625;
  
  TCCR1B |= (1 << WGM12);
  // Mode 4, CTC on OCR1A
  
  TIMSK |= (1 << OCIE1A);
  //Set interrupt on compare match
  
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // set prescaler to 1024 and start the timer
  
  sei();
}

void display_decoded(char* decoded) {
  
}

int main(int argc, char** argv) {
  DDRB = (1 << DDB1) | (1 << DDB0) | (1 << DDB2);
  DDRC = 0x7f;

  start_timer0();

  display_decoded(A);
  display_decoded(SPACE);
  
  unsigned char c = 0;
  while (1) {
    //c = display_queue.current()[offset];    
    
    PORTB = 0x3 & c;
    PORTC = c >> 2;
  }
  return 0;
}
