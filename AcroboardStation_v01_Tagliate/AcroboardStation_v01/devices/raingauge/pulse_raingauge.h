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


#ifndef PULSE_RAINGAUGE_H_
#define PULSE_RAINGAUGE_H_

#define MAXSLOPE_DATATYPE uint16_t
#define MAXSLOPE_UNDEF_VALUE ((MAXSLOPE_DATATYPE)(-1))
#define CENTS_UNDEF_VALUE MAXSLOPE_UNDEF_VALUE


typedef struct
{
	uint16_t			firstTip_cents;
	uint16_t			lastTip_cents;
	uint16_t			maxSlope_cents;
	uint16_t			maxSlope;
	uint16_t			tips;
} RAINGAUGE_STATS;


void raingauge_init(void);


void raingauge_get_stats(uint8_t id,RAINGAUGE_STATS * const ps);
void raingauge_reset_stats(uint8_t id,RAINGAUGE_STATS * const ps);



#endif /* PULSE_RAINGAUGE_H_ */