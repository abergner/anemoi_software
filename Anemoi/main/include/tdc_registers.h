/*
 * tdc_registers.h
 *
 *  Created on: 11 mar. 2019
 *      Author: cufar
 */

#ifndef MAIN_TDC_REGISTERS_H_
#define MAIN_TDC_REGISTERS_H_


//config0 values
#define CONFIG_0_NUM_TXv			0xBu //receive single echo
#define CONFIG_0_TX_FREQ_DIVv		0x4u //rising edge polarity of trigger

//config1 values
#define CONFIG_1_NUM_RXv			0x0u //rising edge polarity of trigger
#define CONFIG_1_NUM_AVGv			0x0u //receive single echo

//config2 values
#define CONFIG_2_TOF_MEAS_MODEv		0x0u //receive single echo
#define CONFIG_2_CH_SELv			0x0u //rising edge polarity of trigger
#define CONFIG_2_EXT_CHSELv 		0x1u //TX 180° pulse shift position
#define CONFIG_2_CH_SWPv			0x0u //rising edge polarity of trigger
#define CONFIG_2_DAMPINGv			0x1u //rising edge polarity of trigger
#define CONFIG_2_MEAS_MODEv			0x0u //rising edge polarity of trigger
#define CONFIG_2_VCOM_SELv			0x0u //rising edge polarity of trigger

//config3 values
#define CONFIG_3_ECHO_QUAL_THLDv	0x3u //receive single echo
#define CONFIG_3_BLANKINGv			0x0u //rising edge polarity of trigger
#define CONFIG_3_TEMP_CLK_DIVv 		0x0u //TX 180° pulse shift position
#define CONFIG_3_TEMP_RTD_SELv		0x0u //rising edge polarity of trigger
#define CONFIG_3_TEMP_MODEv			0x0u //rising edge polarity of trigger

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

//clock_rate values
#define CLOCK_RATE_AUTOZERO_PERIODv		0x0u //receive single echo
#define CLOCK_RATE_CLOCKIN_DIVv			0x0u //rising edge polarity of trigger

/* config0 Bit Fields */
#define	CONFIG_0_NUM_TX_MASK			0x1Fu
#define CONFIG_0_NUM_TX_SHIFT			0
#define CONFIG_0_NUM_TX_WIDTH			5
#define CONFIG_0_NUM_TX(x)				(((uint32_t)(((uint32_t)(x))<<CONFIG_0_NUM_TX_SHIFT))&CONFIG_0_NUM_TX_MASK)

#define	CONFIG_0_TX_FREQ_DIV_MASK			0xE0u
#define CONFIG_0_TX_FREQ_DIV_SHIFT			5
#define CONFIG_0_TX_FREQ_DIV_WIDTH			3
#define CONFIG_0_TX_FREQ_DIV(x)				(((uint32_t)(((uint32_t)(x))<<CONFIG_0_TX_FREQ_DIV_SHIFT))&CONFIG_0_TX_FREQ_DIV_MASK)

/* config1 Bit Fields */
#define	CONFIG_1_NUM_RX_MASK			0x7u
#define CONFIG_1_NUM_RX_SHIFT			0
#define CONFIG_1_NUM_RX_WIDTH			3
#define CONFIG_1_NUM_RX(x)				(((uint32_t)(((uint32_t)(x))<<CONFIG_1_NUM_RX_SHIFT))&CONFIG_1_NUM_RX_MASK)

#define	CONFIG_1_NUM_AVG_MASK			0x38u
#define CONFIG_1_NUM_AVG_SHIFT			3
#define CONFIG_1_NUM_AVG_WIDTH			3
#define CONFIG_1_NUM_AVG(x)				(((uint32_t)(((uint32_t)(x))<<CONFIG_1_NUM_AVG_SHIFT))&CONFIG_1_NUM_AVG_MASK)

/* config2 Bit Fields */
#define	CONFIG_2_TOF_MEAS_MODE_MASK		0x3u
#define CONFIG_2_TOF_MEAS_MODE_SHIFT	0
#define CONFIG_2_TOF_MEAS_MODE_WIDTH	2
#define CONFIG_2_TOF_MEAS_MODE(x)		(((uint32_t)(((uint32_t)(x))<<CONFIG_2_TOF_MEAS_MODE_SHIFT))&CONFIG_2_TOF_MEAS_MODE_MASK)

#define	CONFIG_2_CH_SEL_MASK			0x4u
#define CONFIG_2_CH_SEL_SHIFT			2
#define CONFIG_2_CH_SEL_WIDTH			1
#define CONFIG_2_CH_SEL(x)				(((uint32_t)(((uint32_t)(x))<<CONFIG_2_CH_SEL_SHIFT))&CONFIG_2_CH_SEL_MASK)

#define	CONFIG_2_EXT_CHSEL_MASK			0x8u
#define CONFIG_2_EXT_CHSEL_SHIFT		3
#define CONFIG_2_EXT_CHSEL_WIDTH		1
#define CONFIG_2_EXT_CHSEL(x)			(((uint32_t)(((uint32_t)(x))<<CONFIG_2_EXT_CHSEL_SHIFT))&CONFIG_2_EXT_CHSEL_MASK)

#define	CONFIG_2_CH_SWP_MASK			0x10u
#define CONFIG_2_CH_SWP_SHIFT			4
#define CONFIG_2_CH_SWP_WIDTH			1
#define CONFIG_2_CH_SWP(x)				(((uint32_t)(((uint32_t)(x))<<CONFIG_2_CH_SWP_SHIFT))&CONFIG_2_CH_SWP_MASK)

#define	CONFIG_2_DAMPING_MASK			0x20u
#define CONFIG_2_DAMPING_SHIFT			5
#define CONFIG_2_DAMPING_WIDTH			1
#define CONFIG_2_DAMPING(x)				(((uint32_t)(((uint32_t)(x))<<CONFIG_2_DAMPING_SHIFT))&CONFIG_2_DAMPING_MASK)

#define	CONFIG_2_MEAS_MODE_MASK			0x40u
#define CONFIG_2_MEAS_MODE_SHIFT		6
#define CONFIG_2_MEAS_MODE_WIDTH		1
#define CONFIG_2_MEAS_MODE(x)			(((uint32_t)(((uint32_t)(x))<<CONFIG_2_MEAS_MODE_SHIFT))&CONFIG_2_MEAS_MODE_MASK)

#define	CONFIG_2_VCOM_SEL_MASK			0x80u
#define CONFIG_2_VCOM_SEL_SHIFT			7
#define CONFIG_2_VCOM_SEL_WIDTH			1
#define CONFIG_2_VCOM_SEL(x)			(((uint32_t)(((uint32_t)(x))<<CONFIG_2_VCOM_SEL_SHIFT))&CONFIG_2_VCOM_SEL_MASK)

/* config3 Bit Fields */
#define	CONFIG_3_ECHO_QUAL_THLD_MASK	0x7u
#define CONFIG_3_ECHO_QUAL_THLD_SHIFT	0
#define CONFIG_3_ECHO_QUAL_THLD_WIDTH	3
#define CONFIG_3_ECHO_QUAL_THLD(x)		(((uint32_t)(((uint32_t)(x))<<CONFIG_3_ECHO_QUAL_THLD_SHIFT))&CONFIG_3_ECHO_QUAL_THLD_MASK)

#define	CONFIG_3_BLANKING_MASK			0x8u
#define CONFIG_3_BLANKING_SHIFT			3
#define CONFIG_3_BLANKING_WIDTH			1
#define CONFIG_3_BLANKING(x)			(((uint32_t)(((uint32_t)(x))<<CONFIG_3_BLANKING_SHIFT))&CONFIG_3_BLANKING_MASK)

#define	CONFIG_3_TEMP_CLK_DIV_MASK		0x10u
#define CONFIG_3_TEMP_CLK_DIV_SHIFT		4
#define CONFIG_3_TEMP_CLK_DIV_WIDTH		1
#define CONFIG_3_TEMP_CLK_DIV(x)		(((uint32_t)(((uint32_t)(x))<<CONFIG_3_TEMP_CLK_DIV_SHIFT))&CONFIG_3_TEMP_CLK_DIV_MASK)

#define	CONFIG_3_TEMP_RTD_SEL_MASK		0x20u
#define CONFIG_3_TEMP_RTD_SEL_SHIFT		5
#define CONFIG_3_TEMP_RTD_SEL_WIDTH		1
#define CONFIG_3_TEMP_RTD_SEL(x)		(((uint32_t)(((uint32_t)(x))<<CONFIG_3_TEMP_RTD_SEL_SHIFT))&CONFIG_3_TEMP_RTD_SEL_MASK)

#define	CONFIG_3_TEMP_MODE_MASK			0x40u
#define CONFIG_3_TEMP_MODE_SHIFT		6
#define CONFIG_3_TEMP_MODE_WIDTH		1
#define CONFIG_3_TEMP_MODE(x)			(((uint32_t)(((uint32_t)(x))<<CONFIG_3_TEMP_MODE_SHIFT))&CONFIG_3_TEMP_MODE_MASK)

/* config4 Bit Fields */
#define	CONFIG_4_TX_PH_SHIFT_POS_MASK	0x1Fu
#define CONFIG_4_TX_PH_SHIFT_POS_SHIFT	0
#define CONFIG_4_TX_PH_SHIFT_POS_WIDTH	5
#define CONFIG_4_TX_PH_SHIFT_POS(x)		(((uint32_t)(((uint32_t)(x))<<CONFIG_4_TX_PH_SHIFT_POS_SHIFT))&CONFIG_4_TX_PH_SHIFT_POS_MASK)

#define	CONFIG_4_TRIG_EDGE_POLARITY_MASK	0x20u
#define CONFIG_4_TRIG_EDGE_POLARITY_SHIFT	5
#define CONFIG_4_TRIG_EDGE_POLARITY_WIDTH	1
#define CONFIG_4_TRIG_EDGE_POLARITY(x)		(((uint32_t)(((uint32_t)(x))<<CONFIG_4_TRIG_EDGE_POLARITY_SHIFT))&CONFIG_4_TRIG_EDGE_POLARITY_MASK)

#define	CONFIG_4_RECEIVE_MODE_MASK		0x40u
#define CONFIG_4_RECEIVE_MODE_SHIFT		6
#define CONFIG_4_RECEIVE_MODE_WIDTH		1
#define CONFIG_4_RECEIVE_MODE(x)		(((uint32_t)(((uint32_t)(x))<<CONFIG_4_RECEIVE_MODE_SHIFT))&CONFIG_4_RECEIVE_MODE_MASK)

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




#define CONFIG_0	CONFIG_0_NUM_TX(CONFIG_0_NUM_TXv)|CONFIG_0_TX_FREQ_DIV(CONFIG_0_TX_FREQ_DIVv)
#define CONFIG_1	0x40u|CONFIG_1_NUM_RX(CONFIG_1_NUM_RXv)|CONFIG_1_NUM_AVG(CONFIG_1_NUM_AVGv)
#define CONFIG_2	CONFIG_2_TOF_MEAS_MODE(CONFIG_2_TOF_MEAS_MODEv)|CONFIG_2_CH_SEL(CONFIG_2_CH_SELv)|CONFIG_2_EXT_CHSEL(CONFIG_2_EXT_CHSELv)|CONFIG_2_CH_SWP(CONFIG_2_CH_SWPv)|CONFIG_2_DAMPING(CONFIG_2_DAMPINGv)|CONFIG_2_MEAS_MODE(CONFIG_2_MEAS_MODEv)|CONFIG_2_VCOM_SEL(CONFIG_2_VCOM_SELv)
#define CONFIG_3	CONFIG_3_ECHO_QUAL_THLD(CONFIG_3_ECHO_QUAL_THLDv)|CONFIG_3_BLANKING(CONFIG_3_BLANKINGv)|CONFIG_3_TEMP_CLK_DIV(CONFIG_3_TEMP_CLK_DIVv)|CONFIG_3_TEMP_RTD_SEL(CONFIG_3_TEMP_RTD_SELv)|CONFIG_3_TEMP_MODE(CONFIG_3_TEMP_MODEv)
#define CONFIG_4	CONFIG_4_RECEIVE_MODE(CONFIG_4_RECEIVE_MODEv)|CONFIG_4_TRIG_EDGE_POLARITY(CONFIG_4_TRIG_EDGE_POLARITYv)|CONFIG_4_TX_PH_SHIFT_POS(CONFIG_4_TX_PH_SHIFT_POSv)
#define TOF_1		TOF_1_TIMING_REG(TIMING_REGv)|TOF_1_LNA_FB(TOF_1_LNA_FBv)|TOF_1_LNA_CTRL(TOF_1_LNA_CTRLv)|TOF_1_PGA_CTRL(TOF_1_PGA_CTRLv)|TOF_1_PGA_GAIN(TOF_1_PGA_GAINv)
#define TOF_0		TOF_0_TIMING_REG(TIMING_REGv)
#define TIMEOUT		TIMEOUT_TOF_TIMEOUT_CTRL(TIMEOUT_TOF_TIMEOUT_CTRLv)|TIMEOUT_ECHO_TIMEOUT(TIMEOUT_ECHO_TIMEOUTv)|TIMEOUT_SHORT_TOF_BLANK_PERIOD(TIMEOUT_SHORT_TOF_BLANK_PERIODv)|TIMEOUT_FORCE_SHORT_TOF(TIMEOUT_FORCE_SHORT_TOFv)

#endif /* MAIN_TDC_REGISTERS_H_ */
