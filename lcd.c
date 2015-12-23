//
// LCD Control Library
//
//
// 2013-10-18
// added PIC-MDX2-INCT-2.00(4520)
//
// 2011-10-27
// fixed for PIC-MDX2-INCT-2.00(887)
// fixed for PIC-INCT-2.00
//
// 2011-05-31
// initial version
//

//#define USE_INCT_100
//#define USE_INCT_200
//#define USE_MDX2_200_887
//#define USE_MDX2_200_4520

//#define USE_SAMPLE_CODE



#ifdef USE_SAMPLE_CODE
//-------------------------------------------------
#ifdef USE_INCT_100
#include <16f877a.h>
#fuses	HS,NOWDT,NOPROTECT,PUT,BROWNOUT,NOLVP
#use	delay(CLOCK=20000000)
#use fast_io(d) // RD0 - RD7 for LCD data
#endif // USE_INCT_100

//-------------------------------------------------
#ifdef USE_INCT_200
#include <16f877a.h>
#fuses	HS,NOWDT,PUT,BROWNOUT,NOLVP
#use	delay(CLOCK=20000000)
#use fast_io(d) // RD4 - RD7 for LCD data
#use fast_io(e) // RE0 - RE2 for LCD control bit
#endif // USE_INCT_200

//-------------------------------------------------
#ifdef USE_MDX2_200_887
#include <16f887.h>
#fuses	INTRC_IO,NOWDT,PUT,NOLVP
#use	delay(CLOCK=8000000)
#use fast_io(d) // RD4 - RD7 for LCD data
#use fast_io(e) // RE0 - RE2 for LCD control bit
#endif // USE_MDX2_200_887

//-------------------------------------------------
#ifdef USE_MDX2_200_4520
#include <18f4520.h>
#fuses	INTRC_IO,NOWDT,PUT,NOLVP
#use	delay(CLOCK=8000000)
#use fast_io(d) // RD4 - RD7 for LCD data
#use fast_io(e) // RE0 - RE2 for LCD control bit
#endif // USE_MDX2_200_4520
#endif // USE_SAMPLE_CODE


//--------------------
// PIC-INCT-1.00
//--------------------
#ifdef USE_INCT_100
struct {
	int8 unused : 4; 
	int8 data   : 4;
} lcd_data;
#byte lcd_data = 0x08 // PORTD

struct {
	int8 rs     : 1;
	int8 rw     : 1;
	int8 e      : 1;
	int8 unused : 5; 
} lcd_ctrl;
#byte lcd_ctrl = 0x08 // PORTD

#byte portd_tris = 0x88
#define lcd_write_mode() (portd_tris = 0b00000000)
#define lcd_read_mode()  (portd_tris = 0b11110000)
#endif


//-------------------------
// PIC-MDX2-2.00 PIC16F887
//-------------------------
#ifdef USE_MDX2_200_887
struct {
	int8 unused : 4; 
	int8 data   : 4;
} lcd_data;
#byte lcd_data = 0x08 // PORTD

struct {
	int8 e      : 1;
	int8 rw     : 1;
	int8 rs     : 1;
	int8 unused : 5; 
} lcd_ctrl;
#byte lcd_ctrl = 0x09 // PORTE

#byte portd_tris = 0x88
#define lcd_read_mode()  (portd_tris |= 0xf0)
#define lcd_write_mode() (portd_tris &= 0x0f)
#endif

//--------------------------
// PIC-MDX2-2.00 PIC18F4520
//--------------------------
#ifdef USE_MDX2_200_4520
struct {
	int8 unused : 4; 
	int8 data   : 4;
} lcd_data;
#byte lcd_data = 0xF83 // PORTD

struct {
	int8 e      : 1;
	int8 rw     : 1;
	int8 rs     : 1;
	int8 unused : 5; 
} lcd_ctrl;
#byte lcd_ctrl = 0xF8D // LATE

#byte portd_tris = 0xF95
#define lcd_read_mode()  (portd_tris |= 0xf0)
#define lcd_write_mode() (portd_tris &= 0x0f)
#endif

//--------------------
// PIC-INCT-2.00
//--------------------
#ifdef USE_INCT_200
struct {
	int8 unused : 4; 
	int8 data   : 4;
} lcd_data;
#byte lcd_data = 0x08 // PORTD

struct {
	int8 rs     : 1;
	int8 rw     : 1;
	int8 e      : 1;
	int8 unused : 5; 
} lcd_ctrl;
#byte lcd_ctrl = 0x09 // PORTE

#byte portd_tris = 0x88
#define lcd_read_mode()  (portd_tris |= 0xf0)
#define lcd_write_mode() (portd_tris &= 0x0f)
#endif


#define CTRL_REG 0
#define DATA_REG 1

union data_t {
	int8 raw;
	struct {
		int8 lower : 4;
		int8 upper : 4;
	} bit4;
} data;


#define strobe() {lcd_ctrl.e = 1; delay_cycles(1); lcd_ctrl.e = 0;}


void busy_wait(void)
{
	lcd_read_mode();
	lcd_ctrl.e  = 0;
	lcd_ctrl.rs = 0; // Control
	lcd_ctrl.rw = 1; // Read

	for (;;) {
		lcd_ctrl.e = 1; // Read Upper
		if ((lcd_data.data & 0b1000) == 0) {
			lcd_ctrl.e = 0;
			strobe(); // Read Lower
			break;
		}
		lcd_ctrl.e = 0;			
		strobe(); // Read Lower
	}
	lcd_ctrl.rw = 0;
	lcd_write_mode();
}


void LCD_write(char c, int rs)
{
	busy_wait();

	data.raw = c;

	lcd_data.data = data.bit4.upper;
	lcd_ctrl.rw   = 0;	// write mode
	lcd_ctrl.rs   = rs;	// select control
	strobe();

	lcd_data.data = data.bit4.lower;
	lcd_ctrl.rw   = 0;	// write mode
	lcd_ctrl.rs   = rs;	// select control
	strobe();
}

void LCD_init(void)
{
	#ifdef USE_MDX2_200_887
	set_tris_e(0b00000000);
	#endif
	#ifdef USE_MDX2_200_4520
	set_tris_e(0b00000000);
	#endif
	#ifdef USE_INCT_200
	set_tris_e(0b00000000);
	#endif
	lcd_write_mode();

	// Initialize
	delay_ms(15);
	lcd_ctrl.rw = 0;
	lcd_ctrl.rs = 0;
	lcd_ctrl.e  = 0;

	lcd_data.data = 0b0011;
	strobe();
	delay_us(5000); // over 4.1ms

	lcd_data.data = 0b0011;
	strobe();
	delay_us(150); // over 100us

	lcd_data.data = 0b0011;
	strobe();
	delay_us(50);  // over 40us

	// 4bit mode
	lcd_data.data = 0b0010;
	strobe();

	// setting
	LCD_write( 0b00101000, CTRL_REG ); // 2 line
	LCD_write( 0b00001000, CTRL_REG ); // display off, cursor off, blink off
	LCD_write( 0b00000001, CTRL_REG ); // clear
	LCD_write( 0b00000110, CTRL_REG );
	LCD_write( 0b00001100, CTRL_REG );
}

static int first_time = 1;
static int cursor = 0;
static char line2[16];
static char lp = 0;

void LCD_cls(void)
{
	int i;
	if (first_time) {
		LCD_init();
		first_time = 0;
		lp = 0;
		for (i = 0; i < 16; i++) {
			line2[i] = ' ';
		}
	}

	LCD_write( 0b00000001, CTRL_REG );
	cursor = 0;
	lp = 0;
}

void LCD_home(void)
{
	LCD_write( 0b00000010, CTRL_REG );
}

void LCD_2line(void)
{
	LCD_write( 0b11000000, CTRL_REG );
}


//===============================================================
// '\n' 改行（スクロール）
// '\r' 復帰（行頭にカーソル移動）
//===============================================================
void LCD(char c)
{
	int i;
	if (first_time) {
		LCD_init();
		first_time = 0;
		lp = 0;
		for (i = 0; i < 16; i++) {
			line2[i] = ' ';
		}
	}

	if (c == '\n') {
		if (cursor == 1) {
			LCD_cls();
			for (i = 0; i < 16; i++) {
				LCD_write(line2[i], DATA_REG);
				line2[i] = ' ';
			}
		}
		LCD_2line();
		cursor = 1;
		lp = 0;
	}
	else 
	if (c == '\r') {
		if (cursor == 1) {
			LCD_2line();
			lp = 0;
		}
		else {
			LCD_home();
			lp = 0;
		}
	}
	else {
		if (lp < 16) {
			LCD_write(c, DATA_REG);
			line2[lp++] = c;
		}
	}
}



//================================================
// SAMPLE CODE
//================================================
#ifdef USE_SAMPLE_CODE
//#include "lcd.c"
void main()
{
	long i = 0;

#ifdef USE_MDX2_200_4520
	setup_oscillator(OSC_INTRC | OSC_8MHZ);
#endif // USE_MDX2_200_4520

	for (;;) {
		output_c(0b10000000); // LED 
		delay_ms(10);

		LCD_cls();				
		if (i % 40 < 30) {
			printf( LCD, "*\n" );
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
#endif // USE_SAMPLE_CODE