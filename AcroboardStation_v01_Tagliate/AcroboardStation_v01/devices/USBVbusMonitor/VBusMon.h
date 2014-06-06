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


#ifndef VBUSMON_H_
#define VBUSMON_H_


static inline uint8_t is_USB_cable_plugged(void)
{
	return ioport_pin_is_high(USB_PROBE_PIN);
}

void VBusMon_init(void);
void VBusMon_check(void);


#endif /* VBUSMON_H_ */