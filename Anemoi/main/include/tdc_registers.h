/*
 * tdc_registers.h
 *
 *  Created on: 11 mar. 2019
 *      Author: cufar
 */

#ifndef MAIN_TDC_REGISTERS_H_
#define MAIN_TDC_REGISTERS_H_

//config4 values
#define CONFIG_4_RECEIVE_MODEv			0x0u //receive single echo
#define CONFIG_4_TRIG_EDGE_POLARITYv	0x0u //rising edge polarity of trigger
#define CONFIG_4_TX_PH_SHIFT_POSv 		0x1Fu //TX 180° pulse shift position

/* TOF Values */
#define TIMING_REGv 			0x300u
#define TOF_1_LNA_FBv 			0x1u //resistive feedback
#define TOF_1_LNA_CTRLv 		0x0u //LNA control active
#define TOF_1_PGA_CTRLv 		0x0u //PGA control active
#define TOF_1_PGA_GAINv 		0x7u //PGA gain of 21dB (max)

//timeout values
#define TIMEOUT_TOF_TIMEOUT_CTRLv		0x3u //1024xT0 of echo listening
#define TIMEOUT_ECHO_TIMEOUTv			0x1u //timeout disabled
#define TIMEOUT_SHORT_TOF_BLANK_PERIODv 0x3u //64xT0 blanking period of short TOD
#define TIMEOUT_FORCE_SHORT_TOFv 		0x0u //short TOF disabled


/* TOF_0 Bit Fields */
#define	TOF_0_TIMING_REG_MASK	0xFFu
#define TOF_0_TIMING_REG_SHIFT	0
#define TOF_0_TIMING_REG_WIDTH	8
#define TOF_0_TIMING_REG(x)		(((uint32_t)(((uint32_t)(x))<<TOF_0_TIMING_REG_SHIFT))&TOF_0_TIMING_REG_MASK)

/* TOF_1 Bit Fields */
#define	TOF_1_TIMING_REG_MASK	0x3u
#define TOF_1_TIMING_REG_SHIFT	0
#define TOF_1_TIMING_REG_WIDTH	2
#define TOF_1_TIMING_REG(x)		(((uint32_t)(((uint32_t)(x))<<TOF_1_TIMING_REG_SHIFT>>8))&TOF_1_TIMING_REG_MASK)

#define	TOF_1_LNA_FB_MASK		0x4u
#define TOF_1_LNA_FB_SHIFT		2
#define TOF_1_LNA_FB_WIDTH		1
#define TOF_1_LNA_FB(x)			(((uint32_t)(((uint32_t)(x))<<TOF_1_LNA_FB_SHIFT))&TOF_1_LNA_FB_MASK)

#define	TOF_1_LNA_CTRL_MASK		0x8u
#define TOF_1_LNA_CTRL_SHIFT	3
#define TOF_1_LNA_CTRL_WIDTH	1
#define TOF_1_LNA_CTRL(x)		(((uint32_t)(((uint32_t)(x))<<TOF_1_LNA_CTRL_SHIFT))&TOF_1_LNA_CTRL_MASK)

#define	TOF_1_PGA_CTRL_MASK		0x10u
#define TOF_1_PGA_CTRL_SHIFT	4
#define TOF_1_PGA_CTRL_WIDTH	1
#define TOF_1_PGA_CTRL(x)		(((uint32_t)(((uint32_t)(x))<<TOF_1_PGA_CTRL_SHIFT))&TOF_1_PGA_CTRL_MASK)

#define	TOF_1_PGA_GAIN_MASK		0xE0u
#define TOF_1_PGA_GAIN_SHIFT	5
#define TOF_1_PGA_GAIN_WIDTH	3
#define TOF_1_PGA_GAIN(x)		(((uint32_t)(((uint32_t)(x))<<TOF_1_PGA_GAIN_SHIFT))&TOF_1_PGA_GAIN_MASK)

#define TOF_1		TOF_1_TIMING_REG(TIMING_REGv)|TOF_1_LNA_FB(TOF_1_LNA_FBv)|TOF_1_LNA_CTRL(TOF_1_LNA_CTRLv)|TOF_1_PGA_CTRL(TOF_1_PGA_CTRLv)|TOF_1_PGA_GAIN(TOF_1_PGA_GAINv)
#define TOF_0		TOF_0_TIMING_REG(TIMING_REGv)

/* TIMEOUT Bit Fields */
#define	TIMEOUT_TOF_TIMEOUT_CTRL_MASK	0x3u
#define TIMEOUT_TOF_TIMEOUT_CTRL_SHIFT	0
#define TIMEOUT_TOF_TIMEOUT_CTRL_WIDTH	2
#define TIMEOUT_TOF_TIMEOUT_CTRL(x)		(((uint32_t)(((uint32_t)(x))<<TIMEOUT_TOF_TIMEOUT_CTRL_SHIFT))&TIMEOUT_TOF_TIMEOUT_CTRL_MASK)

#define	TIMEOUT_ECHO_TIMEOUT_MASK		0x4u
#define TIMEOUT_ECHO_TIMEOUT_SHIFT		2
#define TIMEOUT_ECHO_TIMEOUT_WIDTH		1
#define TIMEOUT_ECHO_TIMEOUT(x)	(((uint32_t)(((uint32_t)(x))<<TIMEOUT_ECHO_TIMEOUT_SHIFT))&TIMEOUT_ECHO_TIMEOUT_MASK)

#define	TIMEOUT_SHORT_TOF_BLANK_PERIOD_MASK		0x38u
#define TIMEOUT_SHORT_TOF_BLANK_PERIOD_SHIFT		3
#define TIMEOUT_SHORT_TOF_BLANK_PERIOD_WIDTH		3
#define TIMEOUT_SHORT_TOF_BLANK_PERIOD(x)	(((uint32_t)(((uint32_t)(x))<<TIMEOUT_SHORT_TOF_BLANK_PERIOD_SHIFT))&TIMEOUT_SHORT_TOF_BLANK_PERIOD_MASK)

#define	TIMEOUT_FORCE_SHORT_TOF_MASK		0x40u
#define TIMEOUT_FORCE_SHORT_TOF_SHIFT		6
#define TIMEOUT_FORCE_SHORT_TOF_WIDTH		1
#define TIMEOUT_FORCE_SHORT_TOF(x)			(((uint32_t)(((uint32_t)(x))<<TIMEOUT_FORCE_SHORT_TOF_SHIFT))&TIMEOUT_FORCE_SHORT_TOF_MASK)

#define TIMEOUT		TIMEOUT_TOF_TIMEOUT_CTRL(TIMEOUT_TOF_TIMEOUT_CTRLv)|TIMEOUT_ECHO_TIMEOUT(TIMEOUT_ECHO_TIMEOUTv)|TIMEOUT_SHORT_TOF_BLANK_PERIOD(TIMEOUT_SHORT_TOF_BLANK_PERIODv)|TIMEOUT_FORCE_SHORT_TOF(TIMEOUT_FORCE_SHORT_TOFv)


#endif /* MAIN_TDC_REGISTERS_H_ */
