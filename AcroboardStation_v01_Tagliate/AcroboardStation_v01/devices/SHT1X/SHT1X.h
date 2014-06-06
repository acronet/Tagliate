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


#ifndef SHT1X_H_
#define SHT1X_H_

//************ SHT75 definitions ************//
enum {HUMI,TEMP,LAST=HUMI};

#define DATA		IOPORT_CREATE_PIN(PORTC,0)
#define SCK			IOPORT_CREATE_PIN(PORTC,1)
#define noACK 0
#define ACK   1

//adr  command  r/w
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0

typedef union {
	uint8_t b[2];
	uint16_t w;
} SHT75_register;

char SHT_WriteByte(unsigned char bytte);
char SHT_ReadByte(unsigned char ack);
void SHT_Transstart(void);
void SHT_ConnectionRest(void);
char SHT_SoftRst(void);
char SHT_Read_StatusReg(unsigned char *p_value, unsigned char *p_checksum);
char SHT_Write_StatusReg(unsigned char p_value);

SHT75_register g_sht_reg[2];

unsigned char SHT75_CheckCRC( unsigned char b1, unsigned char b2, unsigned char b3);
char SHT_Measure( unsigned char *p_checksum, unsigned char mode);
void Calc_SHT75(unsigned int hum_in ,float *p_hum , unsigned int temp_in ,float *p_temp);
float Calc_dewpoint(float h,float t);
#endif /* SHT1X_H_ */