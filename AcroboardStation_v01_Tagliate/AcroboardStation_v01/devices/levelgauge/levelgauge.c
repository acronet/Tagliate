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
#include <string.h>
#include "board.h"
//#include "sysclk.h"

#include "conf_board.h"

#include "drivers/usart_interrupt/buffer_usart.h"
#include "config/conf_usart_serial.h"

#include "devices/levelgauge/levelgauge.h"


#define LG_EQUALMEASURES		5 // number of measures with the same value we want to obtain 
#define LG_BUFSIZE				5

static LG_MB7062_STATS g_internal;
uint16_t g_LGmeasureCounter = 0, g_LGresult = 0,g_LGbuf = 0, g_LGadcBuf = 0;
char g_LGszBuf[LG_BUFSIZE],g_LGstring[LG_BUFSIZE];
volatile bool g_LGrecordingData = false;

void levelgauge_init(void)
{
	//USART options.
	static usart_rs232_options_t RS232_SERIAL_OPTIONS = {
		.baudrate = USART_RS232_1_BAUDRATE,
		.charlength = USART_CHAR_LENGTH,
		.paritytype = USART_PARITY,
		.stopbits = USART_STOP_BIT
	};
	sysclk_enable_module(SYSCLK_PORT_C,PR_USART0_bm);
	usart_serial_init(USART_RS232_1, &RS232_SERIAL_OPTIONS);
	usart_set_rx_interrupt_level(USART_RS232_1,USART_INT_LVL_LO);	usart_rx_enable(USART_RS232_1);
	
	//Configure the switch and set at low level
	ioport_configure_pin(LEVELGAUGE_SWITCH, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LEVELGAUGE_SWITCH , LOW);
	
	//Set the USART RX port in inverse mode	//ioport_configure_pin(IOPORT_CREATE_PIN(PORTC, 2), IOPORT_DIR_INPUT | IOPORT_INV_ENABLED);
	
	//Configure the ADC Port
	// ToDo
	
	// DISABLE jtag - it locks the upper 4 pins of PORT B
	CCP       = CCP_IOREG_gc;    // Secret handshake
	MCU.MCUCR = 0b00000001;
	
	sei();
}

void levelgauge_interrupt_start(void){	g_LGszBuf[0]= 'R';	g_LGszBuf[1]= '9';	g_LGszBuf[2]= '9';	g_LGszBuf[3]= '9';	g_LGszBuf[4]= 0;	g_LGstring[0]= 'R';	g_LGstring[1]= '9';	g_LGstring[2]= '9';	g_LGstring[3]= '9';	g_LGstring[4]= 0;	//g_LGmeasureCounter = 0;	//g_LGresult = 0;	//g_LGbuf = 0;	//g_LGadcBuf = 0;	//Enable interrupt	usart_set_rx_interrupt_level(USART_RS232_1,USART_INT_LVL_LO);  //Occhio alla priorità	usart_rx_enable(USART_RS232_1);	//Enable switch	ioport_set_pin_level(LEVELGAUGE_SWITCH , HIGH);}
void levelgauge_interrupt_stop(void){	//Disable switch	ioport_set_pin_level(LEVELGAUGE_SWITCH , LOW);	//Disable interrupt	usart_set_rx_interrupt_level(USART_RS232_1,USART_INT_LVL_OFF);	usart_rx_disable(USART_RS232_1);}

static void levelgauge_saveSample(void)
{
	uint16_t bufVal = 0;
	g_LGrecordingData = true;
	bufVal += (uint16_t)(g_LGstring[2] - 48);
	bufVal += (uint16_t)(g_LGstring[1]-48) * 10 ;
	bufVal += (uint16_t)(g_LGstring[0]-48) * 100 ;
	
	char szBuf[128];
	//sprintf_P(szBuf,PSTR("S: %s - C: %u - L: %u - R: %u\r\n"),g_LGstring,g_LGmeasureCounter,g_LGbuf,g_LGresult);
	//debug_string(NORMAL,szBuf,false);
	
	if (bufVal == g_LGbuf){
		g_LGmeasureCounter++;
		if(g_LGmeasureCounter >= LG_EQUALMEASURES)
		{
			//sprintf_P(szBuf,PSTR("S: %s - L: %u - R: %u\r\n"),g_LGstring,g_LGbuf,g_LGresult);
			//debug_string(NORMAL,szBuf,false);
			g_LGresult = g_LGbuf;
			levelgauge_interrupt_stop();
		}
	}
	else
	{
		g_LGmeasureCounter = 0;
		g_LGbuf = bufVal;
	}
	g_internal.lastVal = bufVal;
	g_LGrecordingData = false;
}


//void levelgauge_getValue(LG_MB7062_STATS * const ps)
//{
	//while (g_LGrecordingData);
	//if(g_LGmeasureCounter >= LG_EQUALMEASURES)
		//g_internal.val = g_LGresult;
	//else
	//{
		//g_LGmeasureCounter = 0;
		//g_internal.val = 1000;
	//}
	//levelgauge_adcGetValue();
	//memcpy_ram2ram(ps,&g_internal,sizeof(LG_MB7062_STATS));
//}

void levelgauge_getValue(LG_MB7062_STATS * const ps)
{
	while (g_LGrecordingData);
		g_internal.val = g_LGresult;
	levelgauge_adcGetValue();
	memcpy_ram2ram(ps,&g_internal,sizeof(LG_MB7062_STATS));
}

void levelgauge_adcGetValue( void )
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	uint8_t inputgain = 1;
	char szBUF[32];
	
	////Acroboard R08
	//LG_ADC_PORT.PIN0CTRL = PORT_OPC_TOTEM_gc; // Voltage reference
	//LG_ADC_PORT.PIN4CTRL = PORT_OPC_TOTEM_gc; // Analog output
	//LG_ADC_PORT.PIN5CTRL = PORT_OPC_TOTEM_gc; // GND x offset
	
	////Acroboard R09
	LG_ADC_PORT.PIN0CTRL = PORT_OPC_TOTEM_gc; // Voltage reference
	LG_ADC_PORT.PIN2CTRL = PORT_OPC_TOTEM_gc; // Analog output
	LG_ADC_PORT.PIN3CTRL = PORT_OPC_TOTEM_gc; // GND x offset
	
	delay_ms(10);
	adc_read_configuration(&LG_ADC, &adc_conf);
	adcch_read_configuration(&LG_ADC, LG_ADC_CH, &adcch_conf);
	
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12, ADC_REF_AREFB);
	//adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12, ADC_REF_VCC);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 64000UL);
	adc_write_configuration(&LG_ADC, &adc_conf);
	
	/////////////////////////////////////////////////////////////////////////
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN3, ADCCH_NEG_NONE, inputgain);
	adcch_write_configuration(&LG_ADC, LG_ADC_CH, &adcch_conf);
	adc_enable(&LG_ADC);
	adc_start_conversion(&LG_ADC, LG_ADC_CH);
	adc_wait_for_interrupt_flag(&LG_ADC, LG_ADC_CH);
	adc_start_conversion(&LG_ADC, LG_ADC_CH);
	adc_wait_for_interrupt_flag(&LG_ADC, LG_ADC_CH);
	const int16_t off = adc_get_result(&LG_ADC, LG_ADC_CH);
	adc_disable(&LG_ADC);
	
	///////////////////////////////////////////////////////////////////////
	adcch_set_input(&adcch_conf, ADCCH_POS_BANDGAP, ADCCH_NEG_NONE, inputgain);
	adcch_write_configuration(&LG_ADC, LG_ADC_CH, &adcch_conf);
	adc_enable(&LG_ADC);
	adc_start_conversion(&LG_ADC, LG_ADC_CH);
	adc_wait_for_interrupt_flag(&LG_ADC, LG_ADC_CH);
	adc_start_conversion(&LG_ADC, LG_ADC_CH);
	adc_wait_for_interrupt_flag(&LG_ADC, LG_ADC_CH);
	const int16_t gain = adc_get_result(&LG_ADC, LG_ADC_CH)-off;
	adc_disable(&LG_ADC);
	
	///////////////////////////////////////////////////////////////////////
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN2, ADCCH_NEG_NONE, inputgain);
	adcch_write_configuration(&LG_ADC, LG_ADC_CH, &adcch_conf);
	adc_enable(&LG_ADC);
	adc_start_conversion(&LG_ADC, LG_ADC_CH);
	adc_wait_for_interrupt_flag(&LG_ADC, LG_ADC_CH);
	adc_start_conversion(&LG_ADC, LG_ADC_CH);
	adc_wait_for_interrupt_flag(&LG_ADC, LG_ADC_CH);
	const int16_t adc_res = adc_get_result(&LG_ADC, LG_ADC_CH)-off;
	adc_disable(&LG_ADC);
	
	sprintf_P(szBUF,PSTR("off: %d\t gain: %d\t adc: %d\r\n"),off,gain,adc_res);
	debug_string(NORMAL,szBUF,false);
	
	////Acroboard R08
	//LG_ADC_PORT.PIN0CTRL = PORT_OPC_PULLUP_gc; // Voltage reference
	//LG_ADC_PORT.PIN4CTRL = PORT_OPC_PULLUP_gc; // Analog output
	//LG_ADC_PORT.PIN5CTRL = PORT_OPC_PULLUP_gc; // GND x offset
	
	////Acroboard R09
	LG_ADC_PORT.PIN0CTRL = PORT_OPC_PULLUP_gc; // Voltage reference
	LG_ADC_PORT.PIN2CTRL = PORT_OPC_PULLUP_gc; // Analog output
	LG_ADC_PORT.PIN3CTRL = PORT_OPC_PULLUP_gc; // GND x offset
	
	//g_internal.adcVal = adc_res;
	//g_internal.adcVal = (float)adc_res/4096.0F*2065.0F / .54789;// --> Vref (mV)
	g_internal.adcVal = adc_res;
	g_internal.aVal = (float)adc_res/4096.0F*1808.0F / .54789;// --> Vref (mV) - .54789 Voltage divider factor
}

static bool usartc_USARTRS232LG_1_RX_CBuffer_Complete(void)
{
	USART_t * const ad = USART_RS232_1;
	static uint8_t idx=0;	const uint8_t dt = ad->DATA;
	
	//char szBUF[2];
	//szBUF[0]=dt;
	//szBUF[1]=0;
	//debug_string(NORMAL,szBUF,false);
		/* Advance buffer head. */	if (idx==0)	{		if (dt == 'R')			g_LGszBuf[idx++]=dt;		else			idx=0;	}	else		g_LGszBuf[idx++]=dt;			if ( idx == LG_BUFSIZE)	{		idx=0;		g_LGstring[0]= g_LGszBuf[1];		g_LGstring[1]= g_LGszBuf[2];		g_LGstring[2]= g_LGszBuf[3];		g_LGstring[3]= 0;		levelgauge_saveSample();	}	return true;
}

ISR(USARTC0_RXC_vect)
{
	usartc_USARTRS232LG_1_RX_CBuffer_Complete();
}