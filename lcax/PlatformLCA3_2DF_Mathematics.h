// *****************************************************************************
///  module     MOD0018_Mathematics
///
/// \file       PlatformLCA3_2DF_Mathematics.h
///
/// \brief      Defines the LCA3 specific constants for 
///             the mathematics.
///
/// \author     Samuel Gidel
///
/// \since      June 05, 2017
///
/// \copyright (c) 2016 LeddarTech Inc. All rights reserved.
// *****************************************************************************

#ifndef _PLATFORM_LCA3_2DF_MATHEMATICS_H_
#define _PLATFORM_LCA3_2DF_MATHEMATICS_H_

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
  #include "PlatformLCA3_2DF_DefinitionsShared.h"
  
  //*****************************************************************************  
  //************************ Constants and Macros *******************************
  //*****************************************************************************  
  #define MATH_DELAY_SCALE_BITS         16
  #define MATH_RAW_AMPLITUDE_SCALE      LCA3_2DF_RAW_AMPLITUDE_SCALE
  #define MATH_SIGNAL_SCALE_BITS        10
  #define MATH_NB_CHANNELS              V3M_NB_CHANNELS
  #define MATH_STUPID_VALUE             INT32_MAX
  #define MATH_DEFAULT_FILTER_SUM_BITS  LCA3_2DF_DEFAULT_FILTER_SUM_BITS


  //*****************************************************************************  
  //************************** Post Header Includes *****************************
  //*****************************************************************************  
  #include "mathematics.h"

#ifdef __cplusplus
}
#endif

#endif  // _PLATFORM_LCA3_2DF_MATHEMATICS_H_
