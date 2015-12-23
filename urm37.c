#include <16f877a.h>
#fuses	HS,NOWDT,PUT,BROWNOUT,NOLVP
#use	delay(CLOCK=20000000)
#use fast_io(d) // RD4 - RD7 for LCD data
#use fast_io(e) // RE0 - RE2 for LCD control bit
#use rs232(BAUD=9600, XMIT=PIN_B0, RCV=PIN_B1)


#define USE_INCT_200
#include "lcd.c"

char READ_MODE[4]={0x33,0x02,0x00,0x35};    // distance measure command
char READ_TEMP[4]={0x11,0x00,0x00,0x11};
char READ_DIST[4]={0x22,0x00,0x00,0x22};

void sendCmd(char *cmd, char *rtn)
{
	int i;
	for (i = 0; i < 4; i++) {
		putc(cmd[i]);
	}
	for (i = 0; i < 4; i++) {
		rtn[i] = getc();
	}
}

main()
{
	long i = 0;
	char rtn[4];
	int temp;

	LCD_cls();
	printf( LCD, "hello!\n" );

	for (;;) {
		sendCmd(READ_DIST, rtn);
		printf( LCD, "\r%02x %02x %02x %02x", rtn[0], rtn[1] rtn[2], rtn[3] );
		delay_ms(1000);	
	}

	for (;;) {
		output_c(0b10000000); // LED 
		delay_ms(10);

		LCD_cls();				
		if (i % 40 < 30) {
			printf( LCD, "#\n" );
		}
		else {
			printf( LCD, " \n" );
		}
		printf( LCD, "count %lu", i );
		i++;

		output_c(0b00000001); // LED 
		delay_ms(10);
	}
}