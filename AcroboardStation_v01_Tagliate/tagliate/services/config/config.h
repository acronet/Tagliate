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


#ifndef CONFIG_H_
#define CONFIG_H_

//#define SYSTEM_AWS_ID "TEST2"


//#define SYSTEM_URL_TIME_SERVICE PSTR("http://130.251.104.23/getTime/")
//#define SYSTEM_URL_SENDDATA_SERVICE PSTR("http://130.251.104.23/putData/?AWSID=%s&TIME=%lu&B=%u&P=%u&S=%u")

uint8_t cfg_check(void);

void cfg_get_sim_pin(char szVal[],const uint8_t len);
void cfg_get_gprs_apn(char szVal[],const uint8_t len);

void cfg_get_aws_id(char szVal[],const uint8_t len);
void cfg_get_service_url_time(char szVal[],const uint8_t len);
void cfg_get_service_url_send(char szVal[],const uint8_t len);
void cfg_get_service_url_send_post(char szVal[],const uint8_t len);
void cfg_get_service_url_send(char szVal[],const uint8_t len);


void cfg_get_datalogger_timings(uint32_t * const sendDT,uint32_t * const storeDT,uint32_t * const syncDT,uint32_t * const tickDT);


#endif /* CONFIG_H_ */