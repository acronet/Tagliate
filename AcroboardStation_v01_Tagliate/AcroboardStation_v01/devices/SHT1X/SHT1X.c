/*
 * ACRONET Project
 * http://www.acronet.cc
 *
 * Copyright ( C ) 2014 Acrotec srl
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the EUPL v.1.1 license.  See http://ec.europa.eu/idabc/eupl.html for details.
 */ 

#include <asf.h>
#include <stdio.h>
#include <math.h>
#include "board.h"
//#include "sysclk.h"
//#include "led.h"
#include <stdio.h>

#include "conf_board.h"
#include "SHT1X.h"
#include "delay.h"

//SHT75_register g_sht_reg[2];

// Coefficients for HR and T calculating (Sensor Version 4)
const float C1=-2.0468;           // for 12 Bit RH
const float C2=+0.0367;           // for 12 Bit RH
const float C3=-0.0000015955;     // for 12 Bit RH
const float T1=+0.01;             // for 14 Bit T
const float T2=+0.00008;          // for 14 Bit T
//********* SHT75 definitions END *********//

//************ SHT75 functions ************//
char SHT_WriteByte(unsigned char bytte)
{
	unsigned char i,error=0;
	ioport_configure_pin(DATA, IOPORT_DIR_OUTPUT);
	for (i=0x80;i>0;i>>=1) //shift bit for masking
	{
		if (i & bytte)
		ioport_set_pin_level(DATA , HIGH);		//DATA_OUT=1;
		else ioport_set_pin_level(DATA ,LOW);	//DATA_OUT=0;
		ioport_set_pin_level(SCK , HIGH);;
		delay_us(5); //pulswith approx. 5 us
		ioport_set_pin_level(SCK , LOW);
	}
	ioport_set_pin_level(DATA , HIGH);
	ioport_configure_pin(DATA, IOPORT_DIR_INPUT);
	ioport_set_pin_level(SCK , HIGH);;
	delay_us(2);
	error=gpio_pin_is_high(DATA);       //check ack (DATA will be pulled down by SHT75)
	delay_us(2);
	ioport_set_pin_level(SCK , LOW);
	return error;       //error=1 in case of no acknowledge
}

char SHT_ReadByte(unsigned char ack)
{
	unsigned char i,val=0;
	ioport_configure_pin(DATA, IOPORT_DIR_INPUT);
	for (i=0x80;i>0;i>>=1)             //shift bit for masking
	{
		ioport_set_pin_level(SCK , HIGH);;
		delay_us(2);
		if (gpio_pin_is_high(DATA)) val=(val | i);        //read bit
		delay_us(2);
		ioport_set_pin_level(SCK , LOW);
	}
	ioport_configure_pin(DATA, IOPORT_DIR_OUTPUT);
	if(ack==0) {
		ioport_set_pin_level(DATA , HIGH);
		return val;
	}
	
	(ioport_set_pin_level(DATA ,LOW));
	
	ioport_set_pin_level(SCK , HIGH);;
	delay_us(5);          //pulswith approx. 5 us
	ioport_set_pin_level(SCK , LOW);
	ioport_set_pin_level(DATA , HIGH);
	return val;
}

// generates a transmission start
void SHT_Transstart(void)
{
	ioport_configure_pin(DATA, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(DATA , HIGH);
	ioport_set_pin_level(SCK , LOW);
	delay_us(2);
	ioport_set_pin_level(SCK , HIGH);;
	delay_us(2);
	ioport_set_pin_level(DATA ,LOW);
	delay_us(2);
	ioport_set_pin_level(SCK , LOW);
	delay_us(5);
	ioport_set_pin_level(SCK , HIGH);;
	delay_us(2);
	ioport_set_pin_level(DATA , HIGH);
	delay_us(2);
	ioport_set_pin_level(SCK , LOW);
	ioport_configure_pin(DATA, IOPORT_DIR_INPUT);
}

// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
void SHT_ConnectionRest(void)
{
	unsigned char i;
	ioport_configure_pin(DATA, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(DATA , HIGH); ioport_set_pin_level(SCK , LOW);                   //Initial state
	for(i=0;i<9;i++)                  //9 SCK cycles
	{
		ioport_set_pin_level(SCK , HIGH);;
		delay_us(1);
		ioport_set_pin_level(SCK , LOW);
		delay_us(1);
	}
	SHT_Transstart();                   //transmission start
	ioport_configure_pin(DATA, IOPORT_DIR_INPUT);
}

// resets the sensor by a softreset
char SHT_SoftRst(void)
{
	unsigned char error=0;
	SHT_ConnectionRest();              //reset communication
	error+=SHT_WriteByte(RESET);       //send RESET-command to sensor
	return error;                     //error=1 in case of no response form the sensor
}

// reads the status register with checksum (8-bit)
char SHT_Read_StatusReg(unsigned char *p_value, unsigned char *p_checksum)
{
	unsigned char error=0;
	SHT_Transstart();                   //transmission start
	error=SHT_WriteByte(STATUS_REG_R); //send command to sensor
	*p_value=SHT_ReadByte(ACK);        //read status register (8-bit)
	*p_checksum=SHT_ReadByte(noACK);   //read checksum (8-bit)
	return error;                     //error=1 in case of no response form the sensor
}

// writes the status register with checksum (8-bit)
char SHT_Write_StatusReg(unsigned char p_value)
{
	unsigned char error=0;
	SHT_Transstart();                   //transmission start
	error+=SHT_WriteByte(STATUS_REG_W);//send command to sensor
	error+=SHT_WriteByte(p_value);    //send value of status register
	return error;                     //error>=1 in case of no response form the sensor
}

// ---------------  CRC Checksum  -------------------//
unsigned char CRC_Table[256] =
{
	0,  49,  98,  83, 196, 245, 166, 151, 185, 136, 219, 234, 125,  76,  31,  46,
	67, 114,  33,  16, 135, 182, 229, 212, 250, 203, 152, 169,  62,  15,  92, 109,
	134, 183, 228, 213,  66, 115,  32,  17,  63,  14,  93, 108, 251, 202, 153, 168,
	197, 244, 167, 150,   1,  48,  99,  82, 124,  77,  30,  47, 184, 137, 218, 235,
	61,  12,  95, 110, 249, 200, 155, 170, 132, 181, 230, 215,  64, 113,  34,  19,
	126,  79,  28,  45, 186, 139, 216, 233, 199, 246, 165, 148,   3,  50,  97,  80,
	187, 138, 217, 232, 127,  78,  29,  44,   2,  51,  96,  81, 198, 247, 164, 149,
	248, 201, 154, 171,  60,  13,  94, 111,  65, 112,  35,  18, 133, 180, 231, 214,
	122,  75,  24,  41, 190, 143, 220, 237, 195, 242, 161, 144,   7,  54, 101,  84,
	57,   8,  91, 106, 253, 204, 159, 174, 128, 177, 226, 211,  68, 117,  38,  23,
	252, 205, 158, 175,  56,   9,  90, 107,  69, 116,  39,  22, 129, 176, 227, 210,
	191, 142, 221, 236, 123,  74,  25,  40,   6,  55, 100,  85, 194, 243, 160, 145,
	71, 118,  37,  20, 131, 178, 225, 208, 254, 207, 156, 173,  58,  11,  88, 105,
	4,  53, 102,  87, 192, 241, 162, 147, 189, 140, 223, 238, 121,  72,  27,  42,
	193, 240, 163, 146,   5,  52, 103,  86, 120,  73,  26,  43, 188, 141, 222, 239,
	130, 179, 224, 209,  70, 119,  36,  21,  59,  10,  89, 104, 255, 206, 157, 172
};

//
// Compute CRC of command byte and 2 rcvd bytes, and checks it against CRC stored in global struct
//
unsigned char SHT75_CheckCRC( unsigned char b1, unsigned char b2, unsigned char b3)
{
	unsigned char crc = 0;
	unsigned char tmp = 0;
	unsigned char bits= 8;

	crc = CRC_Table[ crc ^ b1];
	crc = CRC_Table[ crc ^ b2];
	crc = CRC_Table[ crc ^ b3];

	while( bits--)
	{
		tmp >>=1;
		if( crc & 0b10000000)
		tmp |= 0b10000000;
		crc <<=1;
	}
	return tmp;
}
// ---------------  CRC Checksum RND -------------------//

// makes a measurement (humidity/temperature) with checksum
char SHT_Measure( unsigned char *p_checksum, unsigned char mode)
{
	unsigned error=0;
	unsigned int temp=0;
	SHT_Transstart();                   //transmission start
	switch(mode){                     //send command to sensor
		case TEMP        : error+=SHT_WriteByte(MEASURE_TEMP); break;
		case HUMI        : error+=SHT_WriteByte(MEASURE_HUMI); break;
		default     : break;
	}
	ioport_configure_pin(DATA, IOPORT_DIR_INPUT);
	while (1)
	{
		if((gpio_pin_is_high(DATA))) break; //wait until sensor has finished the measurement
	}
	if(gpio_pin_is_high(DATA)) error+=1;                // or timeout (~2 sec.) is reached
	g_sht_reg[mode].b[1]=SHT_ReadByte(ACK);
	g_sht_reg[mode].b[0]=SHT_ReadByte(ACK);

	*p_checksum =SHT_ReadByte(noACK);  //read checksum

	const uint8_t crc = SHT75_CheckCRC((mode==TEMP)?MEASURE_TEMP:MEASURE_HUMI,g_sht_reg[mode].b[1],g_sht_reg[mode].b[0]);
	if(crc!=*p_checksum) {
		//serial_print("\r\nChecksum Error.");
		//serial_nprint(*p_checksum);
		//serial_nprint(crc);
		debug_string(NORMAL,PSTR ("Checksum Error\r\n"),true);
	}
	return error;
}

void Calc_SHT75(unsigned int hum_in ,float *p_hum , unsigned int temp_in ,float *p_temp)
{
	float rh_lin;							// rh_lin:  Humidity linear
	float rh_true;							// rh_true: Temperature compensated humidity

	float t=temp_in;						// t:       Temperature [Ticks] 14 Bit
	float rh=hum_in;						// rh:      Humidity [Ticks] 12 Bit
	float t_C;								// t_C   :  Temperature [?]
	t_C=t*0.01 - 39.7;						//calc. temperature from ticks to [?]
	rh_lin=C3*rh*rh + C2*rh + C1;			//calc. humidity from ticks to [%RH]
	rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;		//calc. temperature compensated humidity [%RH]
	//rh_true = rh_lin;						//Alternativa alla riga sopra: per evitare la compensazione di RH in funzione di T.
	if(rh_true>100)rh_true=100;				//cut if the value is outside of
	if(rh_true<0.1)rh_true=0.1;				//the physical possible range
	*p_hum=rh_true;
	*p_temp=t_C;
}

float Calc_dewpoint(float h,float t)
{
	float logEx,dew_point;
	logEx=0.66077+7.5*t/(237.3+t)+(log10(h)-2);
	dew_point = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
	return dew_point;
}