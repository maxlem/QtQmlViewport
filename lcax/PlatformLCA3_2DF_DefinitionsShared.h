// ****************************************************************************
/*!
Module:     Shared definitions relative to LCA3 platform.

Platform:   Independent

\file       PlatformLCA3DefinitionsShared.h

\brief      Some LCA3 platform definitions can be shared between different
            system environment.

\author     Samuel Gidel
\since      June 05, 2017

\copyright (c) 2017 LeddarTech Inc. All rights reserved.
*/
// ***************************************************************************

#ifndef __PLATFORM_LCA3_2DF_DEFINITIONSSHARED_H__
#define __PLATFORM_LCA3_2DF_DEFINITIONSSHARED_H__


//*****************************************************************************
//*************** Header Includes *********************************************
//*****************************************************************************

#include <stdint.h>
#include "PlatformV3MDefinitionsShared.h"

//*****************************************************************************
//*************** Constants and Macros ****************************************
//*****************************************************************************

#ifdef GCC
  #define PACKED    __attribute__((packed))
#else
  #define PACKED
#endif

// Peak detector function selection
#undef _USE_CUSTOM_PULSE
#undef _USE_MAG_CORRECTION
#define _USE_BAYES_DETECTOR
#define _USE_AUTO_LED_POWER
#define _USE_XTALK_REJECTION
#undef  _USE_TIME_PROFILING
#define _USE_TEMP_SLOPE
#undef _NO_64BIT_OPS

// Device                                               
#define LCA3_2DF_DEFAULT_PART_NAME                "DEFAULT NAME"                  ///< Default device name.
#define LCA3_2DF_DEFAULT_PART_NUMBER              "UNKNOWN"                       ///< Default device part number.
#define LCA3_2DF_DEFAULT_SOFT_PART_NUMBER         VERSION_FIRMWARE_PART_NUMBER    ///< Default software part number.
#define LCA3_2DF_DEFAULT_SERIAL_NUMBER            "LEDDAR12345678-PO"             ///< Default device serial number.
#define LCA3_2DF_DEFAULT_GROUP_ID                 "LEDDARTECH"                    ///< Default group id.
#define LCA3_2DF_DEFAULT_ASIC_VERSION             "1234"                          ///< Default asic version.
#define LCA3_2DF_DEFAULT_FPGA_VERSION             "32912782-3"                    ///< Default FPGA version.
#define LCA3_2DF_DEFAULT_MFG_NAME                 "LeddarTech"                    ///< Default manufacturer name.
#define LCA3_2DF_DEFAULT_BUILD_DATE               "12 January 2016"               ///< Default build data
#define LCA3_2DF_DEFAULT_BOOTLD_VERSION           "123dd4"                        ///< Bootloard version
#define LCA3_2DF_DEFAULT_OPTIONS                  0x2C                            ///< Default device options.

// Platform
#define LCA3_2DF_DISTANCE_SCALE_BITS              16
#define LCA3_2DF_DISTANCE_SCALE                   (1<<LCA3_2DF_DISTANCE_SCALE_BITS)                                 ///< Distance scale factor to a value expressed in fixed-point.
#define LCA3_2DF_ADC_SCALE_BITS                   10                                                               ///< ADC scale bits.
#define LCA3_2DF_ADC_SCALE                        (1 << LCA3_2DF_ADC_SCALE_BITS )                                   ///< ADC scale
#define LCA3_2DF_RAW_AMPLITUDE_SCALE_BITS         6u                                                               ///< Raw amplitude is expressed on 10 bits and the fractional part is on 6 bits.
#define LCA3_2DF_RAW_AMPLITUDE_SCALE              (1 << LCA3_2DF_RAW_AMPLITUDE_SCALE_BITS)                          ///< Raw amplitude scale factor to be expressed in fixed-point.
#define LCA3_2DF_TEMP_SENSOR_SCALE_BITS           8                                                                ///< Temperature scale factor
#define LCA3_2DF_TEMP_SENSOR_SCALE                LCA3_2DF_TEMP_SENSOR_SCALE_BITS                                   ///< Temperature scale factor bits.
#define LCA3_2DF_TEMP_SCALE_BITS                  LCA3_2DF_DISTANCE_SCALE_BITS                                      ///< Temperature scale factor to a value expressed in fixed-point. Beware,
#define LCA3_2DF_TEMP_SCALE                       LCA3_2DF_DISTANCE_SCALE
#define LCA3_2DF_MEASUREMENT_RATE_SCALE_BITS      16
#define LCA3_2DF_MEASUREMENT_RATE_SCALE           (1 << LCA3_2DF_MEASUREMENT_RATE_SCALE_BITS)                       ///< Measurement rate scale factor to a value expressed in fixed-point Hz. Must be same as distance scale for temperature compensation of distance function.
#define LCA3_2DF_DEFAULT_SCAN_TIME                0x7A0                                                            ///< Default duration of one scan in 100 Mhz clock cycles.
#define LCA3_2DF_REFRESH_RATE_SCALE               64
#define LCA3_2DF_FPGA_FREQUENCY                   310000000
#define LCA3_2DF_DEFAULT_SRC_LIGHT_FREQ           (LCA3_2DF_FPGA_FREQUENCY/LCA3_2DF_DEFAULT_SCAN_TIME)               ///< Infrared source light frequency in Hz.
#define LCA3_2DF_LIGHT_SPEED                      299792458
#define LCA3_2DF_BASE_SAMPLE_DISTANCE             0.386828978064516F                                                      ///< Distance between two base sample points in meters.
#define LCA3_2DF_BEAM_RANGE_GAP                   (49 * LCA3_2DF_DISTANCE_SCALE / 10)                               ///< (4.9m) Distance between the beam range and the selected base sample distance.
#define LCA3_2DF_NB_VERTICAL_CHANNELS             1                                                                ///< Number of Leddar channels along the vertical.
#define LCA3_2DF_NB_REF_CHANNELS                  0                                                                ///< Number of reference channels.
#define LCA3_2DF_NB_HONRIZONTAL_CHANNELS          64                                                               ///< Number of Leddar channels along the horizontal.
#define LCA3_2DF_NB_CHANNELS                      (LCA3_2DF_NB_VERTICAL_CHANNELS * LCA3_2DF_NB_HONRIZONTAL_CHANNELS+LCA3_2DF_NB_REF_CHANNELS) ///< Total number of channel.
#define LCA3_2DF_DEFAULT_BASE_SAMPLE_COUNT        18                                                               ///< Default base sample count value.
#define LCA3_2DF_NB_SAMPLES_PER_CHANNEL_MAX       512
#define LCA3_2DF_NB_BASE_SAMPLE_MIN               4                                                                ///< Minimum authorized base sample count value.
#define LCA3_2DF_NB_BASE_SAMPLE_MAX               LCA3_2DF_NB_SAMPLES_PER_CHANNEL_MAX                                                              ///< Maximum authorized base sample count value.
#define LCA3_2DF_DEFAULT_ACCUMULATION_EXPONENT    8                                                                ///< Default accumulation exponent value.
#define LCA3_2DF_ACCUMULATION_EXPONENT_MIN        0                                                                ///< Minimum authorized accumulation exponent value.
#define LCA3_2DF_ACCUMULATION_EXPONENT_MAX        10                                                               ///< Maximum authorized accumulation exponent value.
#define LCA3_2DF_DEFAULT_OVERSAMPLING_EXPONENT    0                                                                ///< Default oversampling exponent value.
#define LCA3_2DF_OVERSAMPLING_EXPONENT_MIN        0                                                                ///< Minimum authorized oversampling exponent value.
#define LCA3_2DF_OVERSAMPLING_EXPONENT_MAX        0                                                                ///< Maximum authorized oversampling exponent value.
#define LCA3_2DF_DEFAULT_SEGMENT_ENABLE           0x00FF                                                           ///< Default segment enable flag.
#define LCA3_2DF_DEFAULT_START_SCAN_OFFSET        0x0                                                              ///< Default sampling offset
#define LCA3_2DF_DEFAULT_TRIGGER_PERIOD           0xffff                                                           ///< Default period of the trigger by steps of 1/8 of sampling period.
#define LCA3_2DF_DEFAULT_TRIGGER_CFG              0x8                                                              ///< Default trigger enable setting
#define LCA3_2DF_DEFAULT_TRACE_BUFFER_TYPE        0                                                                ///< Default trace buffer state.
#ifdef _USE_CVI_GUI
  #define LCA3_2DF_MAX_ECHOES_PER_CHANNEL           60 
#else
  #define LCA3_2DF_MAX_ECHOES_PER_CHANNEL           2 
#endif
#define LCA3_2DF_MAX_ECHOES                       LCA3_2DF_NB_CHANNELS*LCA3_2DF_MAX_ECHOES_PER_CHANNEL
#define LCA3_2DF_DEFAULT_FIELD_OF_VIEW            (45*LCA3_2DF_DISTANCE_SCALE)
#define LCA3_2DF_DEFAULT_YAW                      0.0F
#define LCA3_2DF_DEFAULT_PITCH                    0.0F
#define LCA3_2DF_DEFAULT_ROLL                     0.0F
#define LCA3_2DF_DEFAULT_POSITION_X               0.0F
#define LCA3_2DF_DEFAULT_POSITION_Y               0.0F
#define LCA3_2DF_DEFAULT_POSITION_Z               0.0F

// Grabber and led power
#define LCA3_2DF_DEFAULT_LEDPOW_ENABLE            1
#define LCA3_2DF_DEFAULT_TRANS_IMP_GAIN           2         ///< Default transimpedance gain.
#define LCA3_2DF_TRANS_IMP_GAIN_MIN               0         ///< Minimum authorized transimpedance gain.
#define LCA3_2DF_TRANS_IMP_GAIN_MAX               7         ///< Maximum authorized transimpedance gain.
#define LCA3_2DF_DEFAULT_PWM_WIDTH_1              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_2              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_3              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_4              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_5              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_6              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_7              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_8              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_9              0x5
#define LCA3_2DF_DEFAULT_PWM_WIDTH_10             0x6
#define LCA3_2DF_DEFAULT_PWM_WIDTH_11             0x7
#define LCA3_2DF_DEFAULT_PWM_WIDTH_12             0x9
#define LCA3_2DF_DEFAULT_PWM_WIDTH_13             0xb
#define LCA3_2DF_DEFAULT_PWM_WIDTH_14             0xe
#define LCA3_2DF_DEFAULT_PWM_WIDTH_15             0x14
#define LCA3_2DF_DEFAULT_PWM_WIDTH_16             0x20
#define LCA3_2DF_DEFAULT_PWM_WIDTH                { LCA3_2DF_DEFAULT_PWM_WIDTH_1, LCA3_2DF_DEFAULT_PWM_WIDTH_2,  LCA3_2DF_DEFAULT_PWM_WIDTH_3,  LCA3_2DF_DEFAULT_PWM_WIDTH_4,  \
                                                    LCA3_2DF_DEFAULT_PWM_WIDTH_5, LCA3_2DF_DEFAULT_PWM_WIDTH_6,  LCA3_2DF_DEFAULT_PWM_WIDTH_7,  LCA3_2DF_DEFAULT_PWM_WIDTH_8,  \
                                                    LCA3_2DF_DEFAULT_PWM_WIDTH_9, LCA3_2DF_DEFAULT_PWM_WIDTH_10, LCA3_2DF_DEFAULT_PWM_WIDTH_11, LCA3_2DF_DEFAULT_PWM_WIDTH_12,  \
                                                    LCA3_2DF_DEFAULT_PWM_WIDTH_13,LCA3_2DF_DEFAULT_PWM_WIDTH_14, LCA3_2DF_DEFAULT_PWM_WIDTH_15, LCA3_2DF_DEFAULT_PWM_WIDTH_16 }               ///< Default pwm width.                                                                           ///< Default number of clock cycles for each PWM pulse.
#define LCA3_2DF_PWM_WIDTH_MIN                    0                                        ///< Minimum authorized transimpedance gain.
#define LCA3_2DF_PWM_WIDTH_MAX                    0xFF  
#define LCA3_2DF_DEFAULT_PWM_PERIOD               0x32                                     ///< Default time of one PWM cycle.
#define LCA3_2DF_PWM_PERIOD_MIN                   0                                        ///< Default time of one PWM cycle.
#define LCA3_2DF_PWM_PERIOD_MAX                   0xFF                                     ///< Default time of one PWM cycle.
#define LCA3_2DF_NB_PWM_PULSES                    16                                       ///< Number of pwm pulse.
#define LCA3_2DF_PWM_ENABLE_MIN                   0                                        ///< PWM enable min.
#define LCA3_2DF_PWM_ENABLE_MAX                   16

#define LCA3_2DF_DEFAULT_NB_USER_LED_POWER        5
#define LCA3_2DF_DEFAULT_CURRENT_USER_LED_POWER   LCA3_2DF_DEFAULT_NB_USER_LED_POWER-1        ///< Current User led power level [in index]
#define LCA3_2DF_NB_USER_LED_POWER_MAX            16                                          ///< Maximum authorized user led power.
#define LCA3_2DF_NB_USER_FIELD_MAX                22                                          ///< Maximum authorized field led power.
#define LCA3_2DF_NB_PWM_PULSES                    16
#define LCA3_2DF_DEFAULT_TIME_BASE_DELAY          0xc
#define LCA3_2DF_DEFAULT_RANDOM_CTRL              0
#define LCA3_2DF_DEFAULT_ADC                      0
#define LCA3_2DF_DEFAULT_REF_PULSE_RATE           1
#define LCA3_2DF_REF_SEG_MASK                     0x100

// Demerge
#define LCA3_2DF_DEFAULT_DEM_ENABLE               0         ///< Default demerge enable state.
#define LCA3_2DF_DEM_LUT_MAX_PTS                  200    
#define LCA3_2DF_OBJECT_DEMERGING_MAX             2
#define LCA3_2DF_REF_PULSE_NB_PTS_MAX             400

// Peak detector
// Bits field used in pulse struct (valid field)
#define LCA3_2DF_PULSE_INVALID                    0     // Reset "valid" field in Pulse struct
#define LCA3_2DF_PULSE_VALID                      0
#define LCA3_2DF_PULSE_MULTOBJ                    1
#define LCA3_2DF_PULSE_OVSHOOT                    2
#define LCA3_2DF_PULSE_SATURAT                    3
#define LCA3_2DF_PULSE_LOWAMP                     4
#define LCA3_2DF_PULSE_MULTOBJ2                   5
#define LCA3_2DF_PULSE_XTALK                      6
#define LCA3_2DF_PULSE_WPB                        7
#define LCA3_2DF_PULSE_REF                        8
#define LCA3_2DF_PULSE_OVSHOOT1                   9
#define LCA3_2DF_PULSE_OVSHOOT2                   10
#define LCA3_2DF_PULSE_OVSHOOT3                   11
#define LCA3_2DF_PULSE_OVSHOOT4                   12

#define LCA3_2DF_DEFAULT_FILTER_SUM_BITS                13                                                             ///< Number of bits added by the trace filter
#define LCA3_2DF_FILTER_SUM_BITS_MIN                    1
#define LCA3_2DF_FILTER_SUM_BITS_MAX                    16
#define LCA3_2DF_DEFAULT_FILTER_SUM                     (1 <<LCA3_2DF_DEFAULT_FILTER_SUM_BITS )                        ///< The sum of all element in the filter matrix.
#define LCA3_2DF_DEFAULT_FILTERED_SCALE                 (LCA3_2DF_RAW_AMPLITUDE_SCALE * LCA3_2DF_DEFAULT_FILTER_SUM)   ///< Filter scale factor to be expressed in fixed-point.
#define LCA3_2DF_DEFAULT_FILTERED_SCALE_BITS            (LCA3_2DF_DEFAULT_FILTER_SUM_BITS+LCA3_2DF_RAW_AMPLITUDE_SCALE_BITS)
#define LCA3_2DF_DEFAULT_RAW_AMPLITUDE_MAX              (1020*LCA3_2DF_RAW_AMPLITUDE_SCALE)
#define LCA3_2DF_DEFAULT_REAL_DISTANCE_OFFSET           ((int32_t)(2.6*LCA3_2DF_DISTANCE_SCALE))                       ///< Difference in distance between the signal peak and the real distance: 2.6 m
#define LCA3_2DF_REAL_DISTANCE_OFFSET_MIN               ((int32_t)(0*LCA3_2DF_DISTANCE_SCALE))            
#define LCA3_2DF_REAL_DISTANCE_OFFSET_MAX               ((int32_t)(10*LCA3_2DF_DISTANCE_SCALE)) 
#define LCA3_2DF_BAYES_STD_LUT_SIZE_MAX                 5
#define LCA3_2DF_MAG_CORRECTION_LUT_SIZE_MAX            50
#define LCA3_2DF_XTALK_NB_TYPE_MAX                      8
#define LCA3_2DF_SATURATION_DIS_LUT_SIZE_MAX            8
#define LCA3_2DF_SATURATION_AMP_LUT_SIZE_MAX            16
#define LCA3_2DF_ACCUMULATION_DEM_EXPONENT_MIN           6
#define LCA3_2DF_OVERSAMPLING_DEM_EXPONENT_MIN           3
#define LCA3_2DF_REF_PULSE_FACTOR_SCALE_BITS             6

// Crosstalk removal
#define LCA3_2DF_DIFF_EQ_MAX                            8
#define LCA3_2DF_TEMPLATE_MAX                           8
#define LCA3_2DF_DIFF_EQ_COEFF_MAX                      21
#define LCA3_2DF_TEMPLATE_COUNT_MAX                     200

// Static noise removal
#define LCA3_2DF_DEFAULT_STNOIRMV_ENABLE                1                                                               ///< Static noise removal for standard segment
#define LCA3_2DF_DEFAULT_STNOIRMV_ENABLE_REFPULSE       1                                                               ///< Static noise removal for reference segment
#define LCA3_2DF_OVERSAMPLING_EXPONENT_CALIB            3                                                               ///< Maximum oversampling exponent authorized during a learning.
#define LCA3_2DF_OVERSAMPLING_CALIB                     (1 << LCA3_2DF_OVERSAMPLING_EXPONENT_CALIB)                      ///< Maximum oversampling authorized during a learning.
#define LCA3_2DF_NB_BASE_SAMPLE_MIN_CALIB               1                                                               ///< Minimum authorized number base sample.
#define LCA3_2DF_NB_BASE_SAMPLE_MAX_CALIB               64                                                              ///< Maximum authorized number base sample.

// Mathematics
#define LCA3_2DF_DELAY_SCALE_BITS                       16


#endif  /* __PLATFORM_LCA3_2DF_DEFINITIONSSHARED_H__ */

// End of PlatformLCA3DefinitionShared.h
