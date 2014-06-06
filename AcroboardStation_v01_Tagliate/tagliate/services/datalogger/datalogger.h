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


#ifndef DATALOGGER_H_
#define DATALOGGER_H_

extern volatile uint8_t dl_cycle_lock;

void datalogger_run(void);

uint8_t datalogger_init(void);

//TASK functions

uint16_t dl_task_send_data_prepare(void);
uint16_t dl_task_send_data_prepare_RT(void);
uint16_t dl_task_send_data_RT(void);
uint16_t dl_task_sync_time(void);
uint8_t dl_task_store_data(void);




#endif /* DATALOGGER_H_ */