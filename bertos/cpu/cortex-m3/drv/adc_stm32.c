/**
 * \file
 * <!--
 * This file is part of BeRTOS.
 *
 * Bertos is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * Copyright 2008 Develer S.r.l. (http://www.develer.com/)
 *
 * -->
 *
 * \brief ADC hardware-specific implementation
 *
 * This ADC module should be use both whit kernel or none.
 * If you are using a kernel, the adc drive does not wait the finish of
 * conversion but use a singal every time a required conversion are
 * ended. This signal wake up a process that return a result of
 * conversion. Otherwise, if you not use a kernl, this module wait
 * whit a loop the finishing of conversion.
 *
 *
 * \author Daniele Basile <asterix@develer.com>
 */


#include "adc_stm32.h"

#include <cpu/irq.h>

#include "cfg/cfg_adc.h"
#include "cfg/cfg_proc.h"
#include "cfg/cfg_signal.h"

#include <cfg/macros.h>
#include <cfg/compiler.h>
#include <cfg/debug.h>

// Define log settings for cfg/log.h.
#define LOG_LEVEL         ADC_LOG_LEVEL
#define LOG_FORMAT        ADC_LOG_FORMAT
#include <cfg/log.h>

#include <drv/adc.h>
#include <drv/clock_stm32.h>
#include <drv/gpio_stm32.h>
#include <drv/irq_cm3.h>

#include <io/stm32.h>


struct stm32_adc *adc = (struct stm32_adc *)ADC1_BASE;

/**
 * Select mux channel \a ch.
 * Generally the stm32 cpu family allow us to program the order
 * of adc channel that we want to read.
 * In this driver implementation we put as fist channel to read the
 * select ones.
 */
void adc_hw_select_ch(uint8_t ch)
{
	/* We sample only from one channel */
	adc->SQR1 |= BV(SQR1_SQ_LEN_SHIFT);
	adc->SQR3 = (ch & SQR3_SQ_MASK);
}

static DECLARE_ISR(adc_redyRead)
{
	kputs("end\n");
}
/**
 * Start an ADC convertion.
 * If a kernel is present, preempt until convertion is complete, otherwise
 * a busy wait on ADC_DRDY bit is done.
 */
uint16_t adc_hw_read(void)
{
	/* Start convertion */
    adc->CR2 |= CR2_EXTTRIG_SWSTRT_SET;

	while (!(adc->SR & BV(SR_EOC)));

	/* Return the last converted data */
	return (adc->DR);
}

/**
 * Init ADC hardware.
 */
void adc_hw_init(void)
{
	/* Enable clocking on AFIO */
	RCC->APB2ENR |= RCC_APB2_AFIO;
	RCC->APB2ENR |= (RCC_APB2_GPIOA | RCC_APB2_GPIOB | RCC_APB2_GPIOC);
	RCC->APB2ENR |= RCC_APB2_ADC1;

	/* Reset registry */
	adc->CR1 = 0;
	adc->CR2 = 0;
	adc->SQR1 = 0;
	adc->SQR2 = 0;
	adc->SQR3 = 0;

	/*
	 * Configure ADC
	 *  - Regular mode
	 *  - Wake up adc
	 *  - Wake up temperature and Vrefint
	 */
	adc->CR2 |= (BV(CR2_ADON) | ADC_EXTERNALTRIGCONV_NONE | BV(CR2_TSVREFE));

	/* Set 17.1usec sampling time on channel 16 and 17 */
	adc->SMPR1 |= ((ADC_SAMPLETIME_239CYCLES5 << ADC_CHANNEL_16) |
		(ADC_SAMPLETIME_239CYCLES5 << ADC_CHANNEL_17));

	/* Register the IRQ handler */
	sysirq_setHandler(ADC_IRQHANDLER, adc_redyRead);
	//adc->CR1 |= BV(CR1_EOCIE);

}
