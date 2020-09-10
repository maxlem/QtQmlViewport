// *****************************************************************************
///  module     MOD0018_Mathematics
///
/// \file       PlatformLCA3_MEMS_Mathematics.h
///
/// \brief      Defines the LCA3_MEMS specific constants for
///             the mathematics.
///
/// \author     Samuel Gidel
///
/// \since      October 19, 2017
///
/// \copyright (c) 2016 LeddarTech Inc. All rights reserved.
// *****************************************************************************

#ifndef _PLATFORM_LCA3_MEMS_MATHEMATICS_H_
#define _PLATFORM_LCA3_MEMS_MATHEMATICS_H_

#undef DLL
#ifdef _USE_CVI_GUI
  #if _DLL_EXPORT
    #define DLL	__declspec(dllexport)
  #else
    #define DLL __declspec(dllimport)
  #endif
#else
  #define DLL
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

  //*****************************************************************************  
  //************************** Pre Header Includes ******************************
  //*****************************************************************************  
  #include "shared/typedef.h"
  #include <PlatformLCA3_MEMS_DefinitionsShared.h>
  
  //*****************************************************************************  
  //************************ Constants and Macros *******************************
  //*****************************************************************************  
  #define MATH_DELAY_SCALE_BITS         16
  #define MATH_RAW_AMPLITUDE_SCALE      LCA3_RAW_AMPLITUDE_SCALE
  #define MATH_SIGNAL_SCALE_BITS        10
  #define MATH_NB_CHANNELS              LCA3_NB_CHANNELS
  #define MATH_STUPID_VALUE             INT32_MAX
  #define MATH_DEFAULT_FILTER_SUM_BITS  LCA3_DEFAULT_FILTER_SUM_BITS

  #define MATH_NB_HONRIZONTAL_CHANNELS    LCA3_NB_HONRIZONTAL_CHANNELS
  #define MATH_NB_VERTICAL_CHANNELS	      LCA3_NB_VERTICAL_CHANNELS

  #define MATH_ADC_COUNT   16 // nb of parallel acquisitions, for GenIndexMap
  #define MATH_MUX_COUNT   4  // nb of channel multiplexing phases, for GenIndexMap
  
  // Constantes to get the noise samples
  #define MATH_WHITE_NOISE_SAMPLES_BEG           13
  #define MATH_WHITE_NOISE_SAMPLES_END           21
  #define MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE (MATH_WHITE_NOISE_SAMPLES_END - MATH_WHITE_NOISE_SAMPLES_BEG + 1)
  #define MATH_NB_SAMPLES_PER_CHANNEL_MAX        LCA3_NB_SAMPLES_PER_CHANNEL_MAX
  #define MATH_NB_ACC_BUFFER                     4
  #define MATH_BUFFER_LUT   \
  {                         \
    {  0, 3, 2, 1, },       \
    {  1, 0, 3, 2, },       \
    {  2, 1, 0, 3, },       \
    {  3, 2, 1, 0, },       \
  }

  //*****************************************************************************  
  //************************** Post Header Includes *****************************
  //*****************************************************************************  
  #include "mathematics.h"

#ifdef __cplusplus
}
#endif

#endif  // _PLATFORM_LCA3_MEMS_MATHEMATICS_H_
