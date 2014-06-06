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
#include "board.h"
//#include "sysclk.h"
#include "twi_master.h"
//#include "led.h"
#include <stdio.h>

#include "conf_board.h"
#include "SHT21.h"
#include "delay.h"

static status_code_t SHT21_internalWrite(twi_package_t * ppak);
static status_code_t SHT21_internalRead(twi_package_t * ppak);

#define SHT21_TWI_ADDR  0x40

int SHT21_Init(void) 
{
		// TWI master initialization options.
		
		twi_master_options_t opt = {
			.speed = 50000,
			.chip  = TWI_MASTER_ADDRESS,
		};
		
		// Initialize the TWI master driver.
		
		return twi_master_setup(AUX_TWI_PORT, &opt);
}

status_code_t SHT21_TriggerReadRH_hold(SHTVAL * val)
{

	uint8_t cmd_buff = 0b11100101;

	debug_string(VERBOSE,PSTR("[SHT21_TriggerReadRH_Hold] IN\r\n"),true);
	
	twi_package_t pak = {
		.addr_length  = 0,
		.chip         = SHT21_TWI_ADDR,
		.buffer       = (void *)&cmd_buff,
		.length       = 1
	};

	pak.addr_length = 0;
	pak.chip = SHT21_TWI_ADDR;
	pak.buffer = val;
	pak.length = 2;

	return SHT21_internalRead(&pak);
	
}

status_code_t SHT21_TriggerReadTemp_hold(SHTVAL *val)
{
	
	uint8_t cmd_buff = 0b11100011;

	debug_string(VERBOSE,PSTR("[SHT21_TriggerReadTemp_hold] IN\r\n"),true);
	
	twi_package_t pak = {
		.addr[0]	  = cmd_buff,
		.addr_length  = 1,
		.chip         = SHT21_TWI_ADDR,
		.buffer       = (void *)val,
		.length       = 2
	};
	
	return SHT21_internalRead(&pak);
}

status_code_t SHT21_TriggerReadTemp_noHold(SHTVAL * val)
{
	uint8_t cmd_buff = 0b11110011;

	debug_string(VERBOSE,PSTR("[SHT21_TriggerReadTemp_noHold] IN\r\n"),true);

	twi_package_t pak = {
		.addr_length  = 0,
		.chip         = SHT21_TWI_ADDR,
		.buffer       = (void *)&cmd_buff,
		.length       = 1
	};


	SHT21_internalWrite(&pak);
	
	debug_string(VERBOSE,PSTR("[SHT21_TriggerReadTemp_noHold] step 1 ok\r\n"),true);
	//delay_ms(10);
	
	pak.addr_length = 0;
	pak.chip = SHT21_TWI_ADDR;
	pak.buffer = val;
	pak.length = 2;

	delay_ms(2);
	return SHT21_internalRead(&pak);
	
}

status_code_t SHT21_TriggerReadRH_noHold(SHTVAL * val)
{

}

static status_code_t SHT21_internalWrite(twi_package_t * ppak)
{
	
	const status_code_t r = twi_master_write(AUX_TWI_PORT, ppak);
	switch(r) {
		case TWI_SUCCESS:
		debug_string(VERBOSE,PSTR("[SHT21_internalWrite] Write Succeeded\r\n"),true);
		return r;
		break;
		case ERR_IO_ERROR:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_IO_ERROR\r\n"),true);
		break;
		case ERR_FLUSHED:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_FLUSHED\r\n"),true);
		break;
		case ERR_TIMEOUT:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_TIMEOUT\r\n"),true);
		break;
		case ERR_BAD_DATA:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_BAD_DATA\r\n"),true);
		break;
		case ERR_PROTOCOL:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_PROTOCOL\r\n"),true);
		break;
		case ERR_UNSUPPORTED_DEV:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_UNSUPPORTED_DEV\r\n"),true);
		break;
		case ERR_NO_MEMORY:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_NO_MEMORY\r\n"),true);
		break;
		case ERR_INVALID_ARG:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_INVALID_ARG\r\n"),true);
		break;
		case ERR_BAD_ADDRESS:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_BAD_ADDRESS\r\n"),true);
		break;
		case ERR_BUSY:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_BUSY\r\n"),true);
		break;
		case ERR_BAD_FORMAT:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: ERR_BAD_FORMAT\r\n"),true);
		break;
		default:
		debug_string(NORMAL,PSTR("[SHT21_internalWrite] Write Failed: UNKONWN ERROR\r\n"),true);
	}
	return r;
}

static status_code_t SHT21_internalRead(twi_package_t * ppak)
{

	const status_code_t r = twi_master_read(AUX_TWI_PORT, ppak);
	switch(r) {
		case TWI_SUCCESS:
		//debug_string(NORMAL,PSTR("READ Succeeded\r\n"));
		return r;
		break;
		case ERR_IO_ERROR:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ Failed: ERR_IO_ERROR\r\n"),true);
		break;
		case ERR_FLUSHED:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_FLUSHED\r\n"),true);
		break;
		case ERR_TIMEOUT:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_TIMEOUT\r\n"),true);
		break;
		case ERR_BAD_DATA:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_BAD_DATA\r\n"),true);
		break;
		case ERR_PROTOCOL:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_PROTOCOL\r\n"),true);
		break;
		case ERR_UNSUPPORTED_DEV:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_UNSUPPORTED_DEV\r\n"),true);
		break;
		case ERR_NO_MEMORY:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_NO_MEMORY\r\n"),true);
		break;
		case ERR_INVALID_ARG:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_INVALID_ARG\r\n"),true);
		break;
		case ERR_BAD_ADDRESS:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_BAD_ADDRESS\r\n"),true);
		break;
		case ERR_BUSY:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_BUSY\r\n"),true);
		break;
		case ERR_BAD_FORMAT:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: ERR_BAD_FORMAT\r\n"),true);
		break;
		default:
		debug_string(NORMAL,PSTR("[SHT21_internalRead] READ  Failed: UNKONWN ERROR\r\n"),true);

	}
	return r;
}

void sht21_read_data(SHT21_STATS * ps)
{
	SHTVAL temp;
	temp.wval = 0;
	
	SHT21_TriggerReadTemp_hold(&temp);
	SHTVAL temp2;
	temp2.bval[1] = temp.bval[0];
	temp2.bval[0] = temp.bval[1];
	
	float valTemp2 = (temp2.wval&0xFFFD);
	valTemp2 = -46.85F + 0.0026812744140625F * valTemp2;

	//sprintf_P(szBuf,PSTR("temperature : %f\r\n"),valTemp2);
	//debug_string_D(NORMAL,szBuf);
	//if(usb_is_present) 	usb_putstring_D(szBuf);
	

	delay_ms(5);

	SHTVAL rh = {0};
	SHT21_TriggerReadRH_hold(&rh);
	
	int8_t t = rh.bval[0];
	rh.bval[0] = rh.bval[1];
	rh.bval[1] = t;

	float valRH2 = rh.wval&0xFFFD;
	valRH2 = -6.0F + (125.0F * valRH2) / 65535.0F;
	//sprintf_P(szBuf,PSTR("rh : %f\r\n"),valRH);
	//debug_string_D(NORMAL,szBuf);
	//if(usb_is_present) 	usb_putstring_D(szBuf);

	delay_ms(5);
	valRH2*=100;
	valTemp2*=100;
	ps->valHR = (uint16_t)valRH2;			// Vanno poi divisi per 100 per ottenere la misura corretta
	ps->valT = (uint16_t)valTemp2;		// Vanno poi divisi per 100 per ottenere la misura corretta
	char szBUF[64];
	sprintf_P(szBUF,PSTR("T: %u\tRH: %u\r\n"),ps->valT,ps->valHR);
	debug_string(NORMAL,szBUF,false);
}	