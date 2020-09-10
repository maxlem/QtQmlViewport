// ****************************************************************************
/*!
Module:     Shared definitions relative to LCA3 platform.

Platform:   Independent

\file       PlatformLCA3_MEMS_DefinitionsShared.h

\brief      Some LCA3_MEMS platform definitions can be shared between different
            system environment.

\author     Samuel Gidel
\since      October 19, 2017

\copyright (c) 2017 LeddarTech Inc. All rights reserved.
*/
// ***************************************************************************

#ifndef __PLATFORM_LCA3_MEMS_DEFINITIONSSHARED_H__
#define __PLATFORM_LCA3_MEMS_DEFINITIONSSHARED_H__


//*****************************************************************************
//*************** Header Includes *********************************************
//*****************************************************************************

#include <stdint.h>
#include "shared/PlatformV3MDefinitionsShared.h"

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
#define LCA3_DEFAULT_PART_NAME                "DEFAULT NAME"                    ///< Default device name.
#define LCA3_DEFAULT_PART_NUMBER              "UNKNOWN"                         ///< Default device part number.
#define LCA3_DEFAULT_SOFT_PART_NUMBER         VERSION_FIRMWARE_PART_NUMBER      ///< Default software part number.
#define LCA3_DEFAULT_SERIAL_NUMBER            "LEDDAR12345678-PO"               ///< Default device serial number.
#define LCA3_DEFAULT_GROUP_ID                 "LEDDARTECH"                      ///< Default group id.
#define LCA3_DEFAULT_ASIC_VERSION             "0"                               ///< Default asic version.
#define LCA3_DEFAULT_FPGA_VERSION             0xFFFFFF                          ///< Default FPGA version.
#define LCA3_DEFAULT_MFG_NAME                 "LeddarTech"                      ///< Default manufacturer name.
#define LCA3_DEFAULT_BUILD_DATE               "12 January 2016"                 ///< Default build data
#define LCA3_DEFAULT_BOOTLD_VERSION           "0"                               ///< Bootloard version
#define LCA3_DEFAULT_OPTIONS                  0x2C                              ///< Default device options.
#define LCA3_DEFAULT_MAC_ADDRESS              { 0x40, 0x00, 0x00, 0x8c, 0xC8, 0xf0 }
#define LCA3_DEFAULT_IP_ADDRESS               0x1400A8C0                        ///< Set to static 192.168.0.20 (Network byte order 0xC0A80020)
#define LCA3_DEFAULT_IP_MODE                  LT_IPV4_IP_MODE_STATIC            ///< Activates DHCP or static addressing, in LtComIpv4

// Platform
#define LCA3_DISTANCE_INTERP_SCALE_BITS       16
#define LCA3_DISTANCE_INTERP_SCALE            (1<<LCA3_DISTANCE_INTERP_SCALE_BITS)
#define LCA3_DISTANCE_SCALE_BITS              8
#define LCA3_DISTANCE_SCALE                   (1<<LCA3_DISTANCE_SCALE_BITS)                                 ///< Distance scale factor to a value expressed in fixed-point.
#define LCA3_ADC_SCALE_BITS                   14                                                               ///< ADC scale bits.
#define LCA3_ADC_SCALE                        (1 << LCA3_ADC_SCALE_BITS )                                   ///< ADC scale
#define LCA3_RAW_AMPLITUDE_SCALE_BITS         2u                                                               ///< Raw amplitude is expressed on 10 bits and the fractional part is on 6 bits.
#define LCA3_RAW_AMPLITUDE_SCALE              (1 << LCA3_RAW_AMPLITUDE_SCALE_BITS)                          ///< Raw amplitude scale factor to be expressed in fixed-point.
#define LCA3_TEMP_SENSOR_SCALE_BITS           8                                                                ///< Temperature scale factor
#define LCA3_TEMP_SCALE                       (1 << LCA3_TEMP_SENSOR_SCALE_BITS)
#define LCA3_TEMP_APD_SCALE_BITS              7                                                                ///< Temperature scale factor from APD sensor
#define LCA3_TEMP_APD_SCALE                   (1 << LCA3_TEMP_APD_SCALE_BITS)
#define LCA3_MEASUREMENT_RATE_SCALE_BITS      16
#define LCA3_MEASUREMENT_RATE_SCALE           (1 << LCA3_MEASUREMENT_RATE_SCALE_BITS)                       ///< Measurement rate scale factor to a value expressed in fixed-point Hz.
                                                                                                            ///< Must be same as distance scale for temperature compensation of distance function.
#define LCA3_DEFAULT_SCAN_TIME                0x7A0                                                            ///< Default duration of one scan in 100 Mhz clock cycles.
#define LCA3_REFRESH_RATE_SCALE               64
#define LCA3_FPGA_FREQUENCY                   320000000
#define LCA3_DEFAULT_SRC_LIGHT_FREQ           (LCA3_FPGA_FREQUENCY/LCA3_DEFAULT_SCAN_TIME)               ///< Infrared source light frequency in Hz.
#define LCA3_LIGHT_SPEED                      299792458
#define LCA3_BASE_SAMPLE_DISTANCE             0.468425715625F                                                      ///< Distance between two base sample points in meters.
#define LCA3_BEAM_RANGE_GAP                   (49 * LCA3_DISTANCE_SCALE / 10)                               ///< (4.9m) Distance between the beam range and the selected base sample distance.
#define LCA3_NB_VERTICAL_CHANNELS             256                                                                ///< Number of Leddar channels along the vertical.
#define LCA3_NB_VERTICAL_CHANNEL_BANK_0       86                                                               ///< 256/3 + 1
#define LCA3_NB_VERTICAL_CHANNEL_BANK_1       85                                                               ///< 256/3
#define LCA3_NB_VERTICAL_CHANNEL_BANK_2       85                                                               ///< 256/3
#define LCA3_NB_REF_CHANNELS                  0                                                                ///< Number of reference channels.
#define LCA3_NB_HONRIZONTAL_CHANNELS          64                                                               ///< Number of Leddar channels along the horizontal.
#define LCA3_NB_CHANNELS                      (LCA3_NB_VERTICAL_CHANNELS * LCA3_NB_HONRIZONTAL_CHANNELS+LCA3_NB_REF_CHANNELS) ///< Total number of channel.
#define LCA3_SIMULTANEOUS_CHANNELS_ACQ        16                                                               ///< Number of channels acquired simultaneously by the FPGA.
#define LCA3_CHANNELS_ACQ_NUMBER              (LCA3_NB_HONRIZONTAL_CHANNELS/LCA3_SIMULTANEOUS_CHANNELS_ACQ)
#define LCA3_DEFAULT_BASE_SAMPLE_COUNT        128                                                               ///< Default base sample count value.
#define LCA3_NB_SAMPLES_PER_CHANNEL_MAX       V3M_NB_SAMPLES_PER_CHANNEL_MAX
#define LCA3_NB_BASE_SAMPLE_MIN               4                                                                ///< Minimum authorized base sample count value.
#define LCA3_NB_BASE_SAMPLE_MAX               LCA3_NB_SAMPLES_PER_CHANNEL_MAX                                                              ///< Maximum authorized base sample count value.
#define LCA3_DEFAULT_ACCUMULATION_EXPONENT    4                                                                ///< Default accumulation exponent value.
#define LCA3_ACCUMULATION_EXPONENT_MIN        0                                                                ///< Minimum authorized accumulation exponent value.
#define LCA3_ACCUMULATION_EXPONENT_MAX        10                                                               ///< Maximum authorized accumulation exponent value.
#define LCA3_DEFAULT_OVERSAMPLING_EXPONENT    0                                                                ///< Default oversampling exponent value.
#define LCA3_OVERSAMPLING_EXPONENT_MIN        0                                                                ///< Minimum authorized oversampling exponent value.
#define LCA3_OVERSAMPLING_EXPONENT_MAX        0                                                                ///< Maximum authorized oversampling exponent value.
#define LCA3_DEFAULT_SEGMENT_ENABLE           0x00FF                                                           ///< Default segment enable flag.
#define LCA3_DEFAULT_START_SCAN_OFFSET        0x0                                                              ///< Default sampling offset
#define LCA3_DEFAULT_TRIGGER_PERIOD           0xffff                                                           ///< Default period of the trigger by steps of 1/8 of sampling period.
#define LCA3_DEFAULT_TRIGGER_CFG              0x8                                                              ///< Default trigger enable setting
#define LCA3_DEFAULT_TRACE_BUFFER_TYPE        0                                                                ///< Default trace buffer state.
#define LCA3_MAX_ECHOES_PER_CHANNEL           2
#define LCA3_MAX_ECHOES                       LCA3_NB_CHANNELS*LCA3_MAX_ECHOES_PER_CHANNEL
#define LCA3_DEFAULT_HOR_FIELD_OF_VIEW        (20)
#define LCA3_DEFAULT_VER_FIELD_OF_VIEW        (60)
#define LCA3_DEFAULT_YAW                      0.0F
#define LCA3_DEFAULT_PITCH                    0.0F
#define LCA3_DEFAULT_ROLL                     0.0F
#define LCA3_DEFAULT_POSITION_X               0.0F
#define LCA3_DEFAULT_POSITION_Y               0.0F
#define LCA3_DEFAULT_POSITION_Z               0.0F
#define LCA3_DEFAULT_FIELD_OF_VIEW_SCALE      (1<<8)
#define LCA3_DEFAULT_ETHERNET_BUFFER_SIZE     2048

#define LCA3_DEFAULT_BASE_ANGLE_COUNT           1       //< MEMs = 16; Trilumina = 1; 32x1 = 1;
#define LCA3_DEFAULT_ANGLE_OVR                  8       //< MEMs = 1,2,4,8; Trilumina = 8; 32x1 = 1;
#define LCA3_ANGLE_OVR_MIN                      1       //< Angle oversampling min = 1
#define LCA3_ANGLE_OVR_MAX                      8       //< Angle oversampling max (MEMs & Trilumina) = 8; 32x1 = 1;

// Grabber and led power
#define LCA3_DEFAULT_LEDPOW_ENABLE            1
#define LCA3_DEFAULT_TRANS_IMP_GAIN           2         ///< Default transimpedance gain.
#define LCA3_TRANS_IMP_GAIN_MIN               0         ///< Minimum authorized transimpedance gain.
#define LCA3_TRANS_IMP_GAIN_MAX               7         ///< Maximum authorized transimpedance gain.
#define LCA3_DEFAULT_PWM_WIDTH_1              0x5
#define LCA3_DEFAULT_PWM_WIDTH_2              0x5
#define LCA3_DEFAULT_PWM_WIDTH_3              0x5
#define LCA3_DEFAULT_PWM_WIDTH_4              0x5
#define LCA3_DEFAULT_PWM_WIDTH_5              0x5
#define LCA3_DEFAULT_PWM_WIDTH_6              0x5
#define LCA3_DEFAULT_PWM_WIDTH_7              0x5
#define LCA3_DEFAULT_PWM_WIDTH_8              0x5
#define LCA3_DEFAULT_PWM_WIDTH_9              0x00
#define LCA3_DEFAULT_PWM_WIDTH_10             0x00
#define LCA3_DEFAULT_PWM_WIDTH_11             0x00
#define LCA3_DEFAULT_PWM_WIDTH_12             0x00
#define LCA3_DEFAULT_PWM_WIDTH_13             0x00
#define LCA3_DEFAULT_PWM_WIDTH_14             0x00
#define LCA3_DEFAULT_PWM_WIDTH_15             0x00
#define LCA3_DEFAULT_PWM_WIDTH_16             0x00

///< Default number of clock cycles for each PWM pulse
#define LCA3_DEFAULT_PWM_WIDTH                { LCA3_DEFAULT_PWM_WIDTH_1, LCA3_DEFAULT_PWM_WIDTH_2,  LCA3_DEFAULT_PWM_WIDTH_3,  LCA3_DEFAULT_PWM_WIDTH_4,  \
                                                LCA3_DEFAULT_PWM_WIDTH_5, LCA3_DEFAULT_PWM_WIDTH_6,  LCA3_DEFAULT_PWM_WIDTH_7,  LCA3_DEFAULT_PWM_WIDTH_8,  \
                                                LCA3_DEFAULT_PWM_WIDTH_9, LCA3_DEFAULT_PWM_WIDTH_10, LCA3_DEFAULT_PWM_WIDTH_11, LCA3_DEFAULT_PWM_WIDTH_12, \
                                                LCA3_DEFAULT_PWM_WIDTH_13,LCA3_DEFAULT_PWM_WIDTH_14, LCA3_DEFAULT_PWM_WIDTH_15, LCA3_DEFAULT_PWM_WIDTH_16 }
#define LCA3_PWM_WIDTH_MIN                    0                                        ///< Minimum authorized transimpedance gain.
#define LCA3_PWM_WIDTH_MAX                    0xFF
#define LCA3_DEFAULT_PWM_PERIOD               0x10     ///< Default time of one PWM cycle
#define LCA3_PWM_PERIOD_MIN                   0x10     ///< Min allowed time
#define LCA3_PWM_PERIOD_MAX                   0x10     ///< Max allowed
#define LCA3_NB_PWM_PULSES                    16       ///< Number of pwm pulse
#define LCA3_PWM_ENABLE_MIN                   0        ///< PWM enable min
#define LCA3_PWM_ENABLE_MAX                   16
#define LCA3_DEFAULT_MEMS_PHASE               0x1100        ///< MEMS sync signal phase, in 160 MHz clock cycles
#define LCA3_DEFAULT_RIGHT_LEFT_PULSE_ALIGN   0x90          ///< MEMS left-right timing alignment, in 160 MHz clock cycles

#define LCA3_DEFAULT_NB_USER_LED_POWER        4
#define LCA3_DEFAULT_CURRENT_USER_LED_POWER   LCA3_DEFAULT_NB_USER_LED_POWER-1        ///< Current User led power level [in index]
#define LCA3_NB_USER_LED_POWER_MAX            8                                          ///< Maximum authorized user led power.
#define LCA3_NB_USER_FIELD_MAX                22                                          ///< Maximum authorized field led power.
#define LCA3_NB_PWM_PULSES                    16
#define LCA3_DEFAULT_TIME_BASE_DELAY          0xdc
#define LCA3_DEFAULT_RANDOM_CTRL              0
#define LCA3_DEFAULT_ADC                      0
#define LCA3_DEFAULT_REF_PULSE_RATE           (1334*2*85)
#define LCA3_REF_SEG_MASK                     0x100

// Led power
#define LCA3_DEFAULT_CURRENT_USER_LED_PERCENT         100
#define LCA3_USER_LED_PERCENT_MIN                     0
#define LCA3_USER_LED_PERCENT_MAX                     100

// Demerge
#define LCA3_DEFAULT_DEM_ENABLE               0         ///< Default demerge enable state.
#define LCA3_DEM_LUT_MAX_PTS                  200
#define LCA3_OBJECT_DEMERGING_MAX             2
#define LCA3_REF_PULSE_NB_PTS_MAX             400

#define LCA3_DEM_REF_PULSE_MAX_PTS          1       //TODO: clarifier ce define car était inexistant...

// Peak detector
// Bits field used in pulse struct (valid field)
#define LCA3_PULSE_INVALID                    0     // Reset "valid" field in Pulse struct
#define LCA3_PULSE_VALID                      0
#define LCA3_PULSE_MULTOBJ                    1
#define LCA3_PULSE_OVSHOOT                    2
#define LCA3_PULSE_SATURAT                    3
#define LCA3_PULSE_LOWAMP                     4
#define LCA3_PULSE_MULTOBJ2                   5
#define LCA3_PULSE_XTALK                      6
#define LCA3_PULSE_WPB                        7
#define LCA3_PULSE_REF                        8
#define LCA3_PULSE_OVSHOOT1                   9
#define LCA3_PULSE_OVSHOOT2                   10
#define LCA3_PULSE_OVSHOOT3                   11
#define LCA3_PULSE_OVSHOOT4                   12

#define LCA3_DEFAULT_FILTER_SUM_BITS                8                                                          ///< Number of bits added by the trace filter
#define LCA3_FILTER_SUM_BITS_MIN                    1
#define LCA3_FILTER_SUM_BITS_MAX                    16
#define LCA3_DEFAULT_FILTER_SUM                     (1 <<LCA3_DEFAULT_FILTER_SUM_BITS )                        ///< The sum of all element in the filter matrix.
#define LCA3_DEFAULT_FILTERED_SCALE                 (LCA3_RAW_AMPLITUDE_SCALE * LCA3_DEFAULT_FILTER_SUM)   ///< Filter scale factor to be expressed in fixed-point.
#define LCA3_DEFAULT_FILTERED_SCALE_BITS            (LCA3_DEFAULT_FILTER_SUM_BITS+LCA3_RAW_AMPLITUDE_SCALE_BITS)
#define LCA3_DEFAULT_RAW_AMPLITUDE_MAX              (((LCA3_ADC_SCALE/2)-2)*LCA3_RAW_AMPLITUDE_SCALE)
#define LCA3_DEFAULT_REAL_DISTANCE_OFFSET           ((int32_t)(2.6*LCA3_DISTANCE_SCALE))                       ///< Difference in distance between the signal peak and the real distance: 2.6 m
#define LCA3_REAL_DISTANCE_OFFSET_MIN               ((int32_t)(0*LCA3_DISTANCE_SCALE))
#define LCA3_REAL_DISTANCE_OFFSET_MAX               ((int32_t)(10*LCA3_DISTANCE_SCALE))
#define LCA3_BAYES_STD_LUT_SIZE_MAX                 5
#define LCA3_XTALK_NB_TYPE_MAX                      8
#define LCA3_SATURATION_DIS_LUT_SIZE_MAX            8
#define LCA3_SATURATION_AMP_LUT_SIZE_MAX            16
#define LCA3_ACCUMULATION_DEM_EXPONENT_MIN          6
#define LCA3_OVERSAMPLING_DEM_EXPONENT_MIN          3
#define LCA3_REF_PULSE_FACTOR_SCALE_BITS            6

// Crosstalk removal
#define LCA3_DIFF_EQ_MAX                            8
#define LCA3_TEMPLATE_MAX                           8
#define LCA3_DIFF_EQ_COEFF_MAX                      21
#define LCA3_TEMPLATE_COUNT_MAX                     200
#define LCA3_DEFAULT_XTALK_OPTIC_SEG_ENABLE         0
#define LCA3_DEFAULT_XTALK_OPTIC_LINE_ENABLE        0

// Static noise removal
#define LCA3_DEFAULT_STNOIRMV_ENABLE                1                                                               ///< Static noise removal for standard segment
#define LCA3_DEFAULT_STNOIRMV_ENABLE_REFPULSE       1                                                               ///< Static noise removal for reference segment
#define LCA3_OVERSAMPLING_EXPONENT_CALIB            0                                                               ///< Maximum oversampling exponent authorized during a learning.
#define LCA3_OVERSAMPLING_CALIB                     (1 << LCA3_OVERSAMPLING_EXPONENT_CALIB)                      ///< Maximum oversampling authorized during a learning.
#define LCA3_DEFAULT_NB_BASE_SAMPLE_CALIB           64
#define LCA3_NB_BASE_SAMPLE_MIN_CALIB               1                                                               ///< Minimum authorized number base sample.
#define LCA3_NB_BASE_SAMPLE_MAX_CALIB               64                                                              ///< Maximum authorized number base sample.

// Mathematics
#define LCA3_DELAY_SCALE_BITS                       16
// Mems
#define  LCA3_FPGA_DEFAULT_MEMS_CURRENT             0x200
#define  LCA3_FPGA_DEFAULT_MAX_MEMS_CURRENT         0x240
#define  LCA3_DEFAULT_PULSE_WIDTH                   0x03
#define  LCA3_DEFAULT_CYCLE_PER_SCAN                0x0

#define  LCA3_DEFAULT_APD_BIAS                      170.7f // 0x800
#define  LCA3_DEFAULT_APD_TEMP0                     25.0f
#define  LCA3_DEFAULT_APD_TFACTOR                   1.1517f
#define  LCA3_DEFAULT_APD_COMP_EN                   TRUE

#define LCA3_PID_INTEGRAL_LIMIT                     30000
#define LCA3_PID_PROPORTIONAL_GAIN                  0.25
#define LCA3_PID_INTEGRAL_GAIN                      0.17
#define LCA3_PID_DERIVATIVE_GAIN                    0.00

#define LCA3_ALGO_THROTTLE_FPGA_UPDATE              5
#define LCA3_ALGO_THROTTLE_FPGA_TIMEBASE            85000  //us

#define LCA3_WHITE_NOISE_SKIP_FRAMES                30
#define LCA3_WHITE_NOISE_DIV_FACTOR_SENSOR          4
#define LCA3_WHITE_NOISE_NBR_LINES                  ((int32_t)( LCA3_NB_VERTICAL_CHANNELS / LCA3_WHITE_NOISE_DIV_FACTOR_SENSOR ))

#if ( LCA3_NB_VERTICAL_CHANNELS > V3M_NB_VER_CHANNELS )
    #error LCA3_NB_VERTICAL_CHANNELS > V3M_NB_VER_CHANNELS
#endif

#if ( LCA3_NB_HONRIZONTAL_CHANNELS > V3M_NB_HON_CHANNELS )
    #error LCA3_NB_HONRIZONTAL_CHANNELS > V3M_NB_HON_CHANNELS
#endif

#if ( LCA3_NB_SAMPLES_PER_CHANNEL_MAX > V3M_NB_SAMPLES_PER_CHANNEL_MAX )
    #error LCA3_NB_SAMPLES_PER_CHANNEL_MAX > V3M_NB_SAMPLES_PER_CHANNEL_MAX
#endif

#if ( LCA3_DEFAULT_NB_BASE_SAMPLE_CALIB > V3M_NB_BASE_SAMPLE_MAX_CALIB )
    #error LCA3_DEFAULT_NB_BASE_SAMPLE_CALIB > V3M_NB_BASE_SAMPLE_MAX_CALIB
#endif

#if ( LCA3_OVERSAMPLING_EXPONENT_CALIB > V3M_OVERSAMPLING_EXPONENT_CALIB )
    #error LCA3_OVERSAMPLING_EXPONENT_CALIB > V3M_OVERSAMPLING_EXPONENT_CALIB
#endif

#if ( LCA3_NB_USER_FIELD_MAX > V3M_NB_USER_FIELD_MAX )
    #error LCA3_NB_USER_FIELD_MAX > V3M_NB_USER_FIELD_MAX
#endif


#endif  /* __PLATFORM_LCA3_MEMS_DEFINITIONSSHARED_H__ */

// End of PlatformLCA3_MEMS_DefinitionShared.h
