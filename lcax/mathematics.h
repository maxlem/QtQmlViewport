// ****************************************************************************
/*!
Module:		MOD0018_Mathematics

Platform:	Independant

\file		  mathematics.h

\brief		Collection of mathematical function implemented for embedded platform.

\author		Samuel Gidel
\author   Vincent Simard Bilodeau

\since		September 15, 2015

\copyright (c) 2015 LeddarTech Inc. All rights reserved.
*/
// ****************************************************************************

#ifndef __MATHEMATICS_H__
#define __MATHEMATICS_H__

#ifdef __cplusplus
extern "C"
{
#endif


  //*****************************************************************************
  //*************** Header Includes *********************************************
  //*****************************************************************************
  // Standard C libraries include files
  #include <stdint.h>	/*uint8_t ...*/

  //*****************************************************************************  
  //************************ Constants and Macros *******************************
  //*****************************************************************************
  // Absolute of a value
  #define MATH_ABS(x)                      (((x)<0)?-(x):(x))
  // Sign of a value
  #define MATH_SGN(x)                      (((x)>=0)?1:-1)
  // Max between two values
  #define MATH_MAX(a,b)                    ((a)>(b)?(a):(b))           
  // Min between two values
  #define MATH_MIN(a,b)                    ((a)>(b)?(b):(a))           
  // Fixed-point ceiled division
  #define MATH_CEILUDIV(a,b)                ((a)/(b) + (((a)%(b)>0)?1:0))
  // Fixed-point rounded division  
  #define MATH_ROUNDDIV(a,b)               ((a)/(b) + (MATH_ABS((a)%(b)) >= MATH_ABS(b)/2? MATH_SGN(a)*MATH_SGN(b):0))
  #define MATH_ROUNDUDIV(a,b)              ((a)/(b) + (((a)%(b)>=b/2)?1:0))
  // Fixed-point rounded left shift
  #define MATH_ROUNDSHIFTRIGHT(v, s)       (MATH_SGN(v)*(((MATH_ABS(v) >> ((s) - 1)) & 0x1) + (MATH_ABS(v) >> (s))))
  // Constructs a mask of a single bit value. "OR" to make a mask: mask = BIT(3) | BIT(6) | BIT(12)
  #define MATH_BIT(x)                      ( 1<<(x) )
  // Set, cleat & obtain single bit at specified position in variable
  #define MATH_SET_TAG(var, field)         ( (var) |=  MATH_BIT(field) )
  #define MATH_CLR_TAG(var, field)         ( (var) &= ~MATH_BIT(field) )
  #define MATH_GET_TAG(var, field)         ( (var) &   MATH_BIT(field) )
  
  #define MATH_SHIFT(a,s)                  ((s) < 0 ? (a)>>-(s):(a)<<(s))

#if defined (WIN32) || defined(_USE_CVI_GUI)
  #define PRINTMATH( dbg, format, arg )     if( dbg==1 ) printf(format, ##arg)
#else
  #define PRINTMATH( dbg, format, arg... )     if( dbg==1 ) printf(format, ##arg)
#endif

  //*****************************************************************************
  //*************** Data Type Definition ****************************************
  //*****************************************************************************

  //*****************************************************************************
  //*************** Public Function Declarations ********************************
  //*****************************************************************************

  DLL void      MATH_SetConfig          ( void );
  DLL int32_t   MATH_LinearInterp       ( const void *aF, uint16_t aLen, int32_t aD, uint16_t aIdx, uint8_t aSignalScaleBits, const uint8_t  signedFlag );
  DLL void      MATH_QuickSort          ( int32_t *aSig, int32_t aLeft, int32_t aRight );
  DLL uint16_t  MATH_Median             ( uint16_t *aSig, uint16_t aLen );
  DLL void      MATH_Sort               ( uint32_t *aSig, uint32_t aLen );
  DLL uint8_t   MATH_ConvFP             ( int32_t *aTrc, int32_t *aCoeff, uint16_t aTrcLen, uint16_t aFilterLen, int32_t *aXtalk, uint8_t aFilterSumBits );
  DLL int32_t   MATH_GenIndexMap        ( const uint16_t vSeg, uint16_t hSeg);
  DLL uint16_t  MATH_GetMapMem2TrcIndex ( uint16_t index, BOOL inverted );
  DLL void      MATH_CopyMapMem2Trc     ( size_t size, uint16_t * dest, BOOL inverted );
  DLL uint16_t  MATH_GetMapTrc2MemIndex ( uint16_t index, BOOL inverted );
  DLL void      MATH_CopyMapTrc2Mem     ( size_t size, uint16_t * dest, BOOL inverted );
  DLL int32_t   MATH_GetWhiteNoise      ( const int16_t *aTrc, uint16_t aColNbr, int32_t *aWhiteNoiseCount, BOOL aScanDirection );
  DLL int32_t	MATH_GetVbiasModel      (int32_t TempApd, uint32_t AmbiantNoise, uint32_t *OptVbias, uint32_t TempCompEn);
  DLL int32_t   MATH_Getpolyval         ( const float polycoef[], uint32_t x_pos, uint32_t nbrCoef, float *Result );
  DLL void      MATH_Accumulation       ( int16_t *aTrc, int16_t aNbSplMax, int16_t aIdxSpl2acc, int16_t aIdxSpl4acc );
  DLL int32_t   MATH_GetSigma           ( const int16_t *aTrc, uint16_t aPixelTrc, int32_t *aSigma, BOOL aScanDirection );
  DLL int16_t*  MATH_GetAccTrcBuffer    ( void );
  DLL int32_t*  MATH_GetAccTmpX         ( uint16_t lNum );
  DLL int32_t	MATH_Smooth             (uint32_t mData, uint32_t *mVal);


#ifdef _USE_CVI_GUI

  /// \struct MATH_Constants
  /// \brief  Used to store all constants related to mathematics module for external use (Matlab).
  typedef struct
  {
    uint32_t DELAY_SCALE_BITS;
    uint32_t RAW_AMPLITUDE_SCALE;
    uint32_t FILTER_SUM_BITS;
    uint32_t NB_CHANNELS;
  } sMATH_Constants;

  DLL uint8_t MATH_Test        ( uint8_t aVal );
  DLL void    MATH_GetConstants( sMATH_Constants *cst );

#endif  //  _USE_CVI_GUI

#ifdef __cplusplus
}
#endif

#endif	// __MATHEMATICS_H__

// End of file