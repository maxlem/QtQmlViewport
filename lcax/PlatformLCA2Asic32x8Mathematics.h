/*
 ***************************************************************************************************
  * $File: //depot/software/embed/devLine/D_EMB0036_LCA2_RefDesign/LCA2/src/app/algo/PlatformLCA2AsicMathematics.h $
  * $Author: keavenp $
  * Created:	2017-11-07
  * $Revision: #2 $
  * $Change: 17601 $
  * $DateTime: 2017/11/21 16:01:25 $
****************************************************************************************************
****************************************************************************************************
*
*   MODULE        : MOD0018 Mathematics
*
*   Description   : Defines the LCA2 Asic specific constants for the mathematics.
*
*   Copyright 2017; LeddarTech Inc.
*
*   All Rights Reserved. Protected by International copyright laws.
*
*   Copyright or Distribution Terms
*
****************************************************************************************************
*/

/* DEFINE TO PREVENT RECURSIVE INCLUSION ----------------------------------------------------------*/
#ifndef __PLATFORM_LCA2_ASIC_32x8_MATHEMATICS_H_
#define __PLATFORM_LCA2_ASIC_32x8_MATHEMATICS_H_

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
#include "shared/typeDef.h"
#include "PlatformLCA2AsicDefinitionsShared.h"
  
  //*****************************************************************************  
  //************************ Constants and Macros *******************************
  //*****************************************************************************  
  #define MATH_DELAY_SCALE_BITS         16
  #define MATH_RAW_AMPLITUDE_SCALE      LCA2_ASIC_RAW_AMPLITUDE_SCALE
  #define MATH_SIGNAL_SCALE_BITS        10
  #define MATH_NB_CHANNELS              LCA2_ASIC_NB_CHANNELS
  #define MATH_STUPID_VALUE             INT32_MAX
  #define MATH_DEFAULT_FILTER_SUM_BITS  LCA2_ASIC_DEFAULT_FILTER_SUM_BITS

  //*****************************************************************************  
  //************************** Post Header Includes *****************************
  //*****************************************************************************  
  #include "Mathematics.h"

#ifdef __cplusplus
}
#endif

#endif  // __PLATFORM_LCA2_ASIC_32x8_MATHEMATICS_H_
