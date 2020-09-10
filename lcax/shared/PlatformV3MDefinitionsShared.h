// ****************************************************************************
/*!
Module:     Shared definitions relative to V3M platform.

Platform:   Independent

\file       PlatformV3MDefinitionsShared.h

\brief      Some V3M platform definitions can be shared between different
            system environment (LeddarSoftware, LeddarExplorer, embedded firmware).

\author     Samuel gidel
\since      June 05, 2017

\copyright (c) 2013 LeddarTech Inc. All rights reserved.
*/
// ***************************************************************************

#ifndef __PLATFORM_V3M_DEFINITIONSSHARED_H__
#define __PLATFORM_V3M_DEFINITIONSSHARED_H__

//*****************************************************************************
//*************** Header Includes *********************************************
//*****************************************************************************

#include <stdint.h>

//*****************************************************************************
//*************** Constants and Macros ****************************************
//*****************************************************************************

#ifdef GCC
  #define PACKED    __attribute__((packed))
#else
  #define PACKED
#endif

#define V3M_R_COMMAND_LIST_MAX_LEN         500
#define V3M_R_DATA_PLANES_MAX_LEN          20
#define V3M_R_CL_SIZE                      (16 * 256 * 4)
#define V3M_DBG_GWM                        256
#define V3M_FREQ_MICS                      400

#define V3M_FCT_CLSIZE                     5000 //!< CL maximum length (used for allocation)
#define V3M_FCT_MEM_ALIGN                  64   //Alignment of the memory
#define V3M_FCT_CL_ALIGN                   128
#define V3M_FCT_CLOFS_NONE                 -1
#define V3M_VECTOR_PADDING                 12

#define  V3M_NB_HON_CHANNELS               64
#define  V3M_NB_VER_CHANNELS               256
#define  V3M_NB_VER_CHANNELS_CVE           32
#define  V3M_NB_REF_CHANNELS               0
#define  V3M_NB_CHANNELS                   ((V3M_NB_HON_CHANNELS*V3M_NB_VER_CHANNELS)+V3M_NB_REF_CHANNELS)
#define  V3M_MAX_ECHOES_PER_CHANNEL        2
#define  V3M_NB_USER_LED_POWER_MAX         8
#define  V3M_NB_COEFF_FILTER_MAX           32
#define  V3M_ACCUMULATION_EXPONENT_MAX     10
#define  V3M_OVERSAMPLING_EXPONENT_MAX     0
#define  V3M_SATURATION_AMP_LUT_SIZE_MAX   16
#define  V3M_SATURATION_DIS_LUT_SIZE_MAX   8
#define  V3M_XTALK_NB_TYPE_MAX             8
#define  V3M_NB_SAMPLES_PER_CHANNEL_MAX    512
#define  V3M_BAYES_STD_LUT_SIZE_MAX        5
#define  V3M_NB_BASE_SAMPLE_MAX_CALIB      64
#define  V3M_CPU_LOAD_SCALE                10
#define  V3M_NB_THREAD_MAX                 32
#define  V3M_NB_THREAD_MAX_PER_CORE         8
#define  V3M_NB_CVE_CORE_MAX                2
#define  V3M_NB_IMP_CORE_MAX                4
#define  V3M_NB_CORE_MAX_PER_CVE            4
#define  V3M_NB_CSI_BUFFER_MAX              7
#define  V3M_NB_VIN_MAX                     1
#define  V3M_GWM_SIZE_MAX                   4096
#define  V3M_HEADER_SIZE_MAX                8192
#define  V3M_HEADER_TAG                     0xA5A5A5A5
#define  V3M_NB_USER_FIELD_MAX              22
#define  V3M_OVERSAMPLING_EXPONENT_CALIB    0                                                               ///< Maximum oversampling exponent authorized during a learning.
#define  V3M_OVERSAMPLING_CALIB             (1 << V3M_OVERSAMPLING_EXPONENT_CALIB)
#define  V3M_DEFAULT_ECHOES_OUTPUT_SIZE     32
#define  V3M_DEFAULT_PID_FILE               "/var/run/LCA3.pid"
#define  V3M_DEFAULT_LOG_FILE               "/var/log/messages"
#define  V3M_SHIFT_IMPCORES                 12


//*****************************************************************************
//*************** Data Type Definition ****************************************
//*****************************************************************************

/// \struct sEchoLigth
/// \brief  Light echo structure
typedef struct
{
  uint16_t  mDistance;
  int16_t   mAmplitude;
  uint16_t  mSegment;
  uint16_t  mFlag;
} PACKED sEchoLigth;

/// \struct SEchoFP
/// \brief Element of the echoes list
/// \warning  The size of the structure is used in USB communication.
///           Does not change the structure without changing the
///           definition of gEchoElemStruct in processingData.c. This
///           size dependency should be addressed.
typedef struct
{
  int32_t      mDistance;          ///< Echo distance.
  uint32_t     mAmplitude;         ///< Echo amplitude.
  uint32_t     mBase;              ///< Base value to
  uint16_t     mMaxIndex;          ///< Index of the maximum amplitude.
  uint8_t      mChannelIndex;      ///< Channel index of the echo.
  uint8_t      mValid;             ///< Bit fields validation flags.
  uint32_t     mAmplitudeLowScale; ///< Echo amplitude in low scale factor.
  uint32_t     mSaturationWidth;   ///< Width of the saturation.
} sEcho;

/// \struct SEchoesFP
/// \brief List of Echoes
typedef struct
{
  uint16_t      mEchoCount;                                             ///< Number of echoes in the list.
  sEcho    mEchoes[V3M_MAX_ECHOES_PER_CHANNEL*V3M_NB_CHANNELS];    ///< Echoes structure list.
} sEchoes;

/// \struct SPulse
/// \brief Element of the pulses list. A peak in a trace, not yet an echo.
typedef struct
{
  uint16_t     index;         //<  Index in the trace.
  uint16_t     maxi;          //<  Index of maximum amplitude.
  int32_t      distIntpl;     //<  Distance obtained by interpolation.
  uint16_t     minLefti;      //<  Index of minimum amplitude located on the Left of the pulse.
  uint16_t     minRighti;     //<  Index of minimum amplitude located on the Right of the pulse.
  int32_t      max;           //<  Maximum amplitude.
  int32_t      amp;           //<  Real amplitude of the pulse.
  int32_t      base;          //<  Base level.
  int32_t      minLeft;       //<  Minimum amplitude on the Left of the pulse.
  int32_t      minRight;      //<  Minimum amplitude on the Right of the pulse.
  uint32_t     satWidth;      //<  Saturation width.
  int32_t      ampLowScale;   //<  Real amplitude of the pulse with a small fixed point.
  uint32_t     satLefti;      //<  First point of the pulse saturated.
  uint32_t     satRighti;     //<  Last point of the pulse saturated.
  int32_t      inflec;        //<  Inflection point.
  uint32_t     ch;            //<  Channel Index.
  uint16_t     valid;         //<  Pulse validation.
  uint8_t      satFlag;       //<  Saturation flag.
  uint8_t      bayesFlag;     //<  Bayes detector flag.
} sPulse;

#define V3M_NB_STRUCT_ECHOES_SIZE_MAX           (sizeof(sEchoLigth)/sizeof(uint16_t))

#endif  /* __PLATFORM_V3M_DEFINITIONSSHARED_H__ */
// End of PlatformV3MDefinitionShared.h
