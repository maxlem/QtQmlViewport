// ****************************************************************************
// * $File: //embed/LCA3/Discrete/LCA3_Rev_A2_312lines/code/sources/src/algo/mathematics.c $
// * $Author: samuelg $
// * Created:	2016-01-27
// * $Revision: #2 $
// * $Change: 23378 $
// ****************************************************************************
// ****************************************************************************
/*!
Module:		MOD0018_Mathematics

Platform:	Independent.

\file		  mathematics.c

\brief		Collection of mathematical function implemented for embedded platform.

\author		Samuel Gidel
\author		Vicent Simard Bilodeau
\since		February 25, 2016

\copyright (c) 2016 LeddarTech Inc. All rights reserved.
*/
// ***************************************************************************

//*****************************************************************************
//************** Header Includes **********************************************
//*****************************************************************************
// Standard C libraries include files
#if defined (_LCA3_3DF) || defined (_LCA3_MEMS)
  #include "string.h"
  #include "stdio.h"
  #include "stdlib.h"
  #include "math.h"
  #include <fenv.h>
#endif

// Platform specific include files

// Application include files
#if defined(_GALAXY)
  #include "PlatformGalaxyMathematics.h"
#elif defined(_SINGLECHANNEL)
  #include "PlatformSingleChannelMathematics.h"
#elif defined(_LC16IL32)
  #include "PlatformLC16il32Mathematics.h"
#elif defined(_LC16IL8)
  #include "PlatformLC16il8Mathematics.h"
#elif defined(_LC08L32)
  #include "PlatformLC08l32Mathematics.h"
#elif defined(_MARYANN)
  #include "PlatformMaryAnnMathematics.h"
#elif defined (_GALAXY_LASER)
  #include "PlatformGalaxyLaserMathematics.h"
#elif defined (_LCA3_3DF)
  #include "PlatformLCA3_3DF_Mathematics.h"
#elif defined (_TRACKER)
  #include "PlatformTrackerMathematics.h"
#elif defined (_LCA3_MEMS)
  #include "PlatformLCA3_MEMS_Mathematics.h"
#elif defined (PRODUCT_LCA2_ASIC_32X8)
  #include "PlatformLCA2Asic32x8Mathematics.h"
#endif


//*****************************************************************************
//************** Constants and Macros *****************************************
//*****************************************************************************

//*****************************************************************************
//************** Data Type Definition *****************************************
//*****************************************************************************

//*****************************************************************************
//************** Private Variable Declarations ********************************
//*****************************************************************************
#if defined (_LCA3_3DF) || defined (_LCA3_MEMS)

  static uint16_t gMapMem2Trc[312*64];         // Converts memory index to trace
  static uint16_t gMapMem2TrcInverted[312*64]; // Converts memory index to inverted trace
  static uint16_t gMapTrc2Mem[312*64];         // Converts trace index to memory
  static uint16_t gMapTrc2MemInverted[312*64]; // Converts inverted trace index to memory
  static int16_t  gTrcBuffer[MATH_NB_SAMPLES_PER_CHANNEL_MAX*MATH_NB_CHANNELS*MATH_NB_ACC_BUFFER];
  static uint8_t  gBufferLUT[MATH_NB_ACC_BUFFER][MATH_NB_ACC_BUFFER] = MATH_BUFFER_LUT;
  static int32_t  gTmp2     [MATH_NB_SAMPLES_PER_CHANNEL_MAX*MATH_NB_CHANNELS];
  static int32_t  gTmp4     [MATH_NB_SAMPLES_PER_CHANNEL_MAX*MATH_NB_CHANNELS];
  static uint8_t  gInitBuffer = FALSE;
  static uint8_t  gIdxBuffer = 0;
#endif
//*****************************************************************************
//************** Private Function Definitions *********************************
//*****************************************************************************
static void MATH_Swap (int32_t *aSig, int32_t num1, int32_t num2)
{
  int32_t temp = aSig[num1];
  aSig[num1] = aSig[num2];
  aSig[num2] = temp;
}
// ****************************************************************************
///	Function:	  MATH_Partition

///	\brief      Partition and swap.

/// \param[in] *aSig:        Signal values from 0 to aLen-1.
/// \param[in] aPivot:       Partition aSig[1..aLen] into two subarrays aSig[1..aPivot-1] and aSig[aPivot+1..aLen]
/// \param[in]  aLen:        Length-1 of aSig

///	\return     Return the median value.

/// \author     Samuel Gidel
///	\author     Vincent Simard Bilodeau

///	\since		  March 11, 2016
// ****************************************************************************
static int16_t MATH_Partition ( int32_t *aSig,                               
                                int32_t aLeft,
                                int32_t aRight,
                                int32_t aPivot )
{
  int32_t lLeftPointer = aLeft - 1;
  int32_t lRightPointer = aRight;

  while (1) 
  {
    while (aSig[++lLeftPointer] < aPivot)
    {
    }

    while (lRightPointer > 0 && aSig[--lRightPointer] > aPivot)
    {
    }

    if (lLeftPointer >= lRightPointer)
    {
      break;
    }
    else
    {
      MATH_Swap(aSig, lLeftPointer, lRightPointer);
    }
  }

  MATH_Swap(aSig,lLeftPointer, aRight);
  return lLeftPointer;
}

//*****************************************************************************
//*************** Public Function Definitions *********************************
//*****************************************************************************
// ****************************************************************************
///	Function:	  MATH_LinearInterp

///	\brief      Returns the value of the function f(i-d) at the index i using
///             linear interpolation.

/// \param[in] *aF:          Values of the function from 0 to aLen-1.
/// \param[in]  aLen:        Length of aF
/// \param[in]  aD:          Delay (scaled by DEM_DELAY_SCALE)
/// \param[in]  aIdx:        Desired index between d and d+aLen-1
/// \param[in]  signedFlag:  Signal is signed (1) or not (0)

///	\return     Return the interpolated value (zeros if the desired index
///             is outsize aF).

///	\author     Vincent Simard Bilodeau
/// \author     Samuel Gidel

///	\since		  September 15, 2015
// ****************************************************************************
int32_t MATH_LinearInterp( const void    *aF,
                           uint16_t       aLen,
                           int32_t        aD,
                           uint16_t       aIdx,
                           uint8_t        aSignalScaleBits,
                           const uint8_t  signedFlag )
{

  // Compute interpolation index
  int32_t  lIdx = (aIdx << MATH_DELAY_SCALE_BITS) - aD;
  int16_t  lFlooredIdx = lIdx >> MATH_DELAY_SCALE_BITS;
  uint32_t lDeltaIdx = lIdx - (lFlooredIdx << MATH_DELAY_SCALE_BITS);
  int32_t  lRes;

  // Check bounds
  if (lFlooredIdx < 0 || lFlooredIdx >= aLen - 1)
  {
    return 0;
  }

  // Perform interpolation
  int32_t lY1=0;
  int32_t lY2=0;
  switch (signedFlag)
  {
    case 0:
      lY1 = MATH_SHIFT((int32_t)*(((uint16_t*)aF) + lFlooredIdx), MATH_SIGNAL_SCALE_BITS - aSignalScaleBits);
      lY2 = MATH_SHIFT((int32_t)*(((uint16_t*)aF) + lFlooredIdx + 1), MATH_SIGNAL_SCALE_BITS - aSignalScaleBits);
      break;
    case 1:
      lY1 = MATH_SHIFT((int32_t)*(((int16_t*)aF) + lFlooredIdx), MATH_SIGNAL_SCALE_BITS - aSignalScaleBits);
      lY2 = MATH_SHIFT((int32_t)*(((int16_t*)aF) + lFlooredIdx + 1), MATH_SIGNAL_SCALE_BITS - aSignalScaleBits);
      break;
    case 2:
      lY1 = MATH_SHIFT((int32_t)*(((uint32_t*)aF) + lFlooredIdx), MATH_SIGNAL_SCALE_BITS - aSignalScaleBits);
      lY2 = MATH_SHIFT((int32_t)*(((uint32_t*)aF) + lFlooredIdx + 1), MATH_SIGNAL_SCALE_BITS - aSignalScaleBits);
      break;
    case 3:
      lY1 = MATH_SHIFT((int32_t)*(((int32_t*)aF) + lFlooredIdx), MATH_SIGNAL_SCALE_BITS - aSignalScaleBits);
      lY2 = MATH_SHIFT((int32_t)*(((int32_t*)aF) + lFlooredIdx + 1), MATH_SIGNAL_SCALE_BITS - aSignalScaleBits);
      break;
  }
  lRes = (int32_t)(MATH_ROUNDSHIFTRIGHT((lY2 - lY1)*(int32_t)lDeltaIdx, MATH_DELAY_SCALE_BITS) + lY1);
  return MATH_SHIFT(lRes, aSignalScaleBits - MATH_SIGNAL_SCALE_BITS);
}

// ****************************************************************************
///	Function:	  MATH_QuickSort

///	\brief      Sort an array [0 to aLen-1] into ascending numerical order using Quiksort algorithm.

/// \param[in] *aSig:        Signal values from 0 to aLen-1.
/// \param[in] aPivot:       Partition aSig[1..aLen] into two subarrays aSig[1..aPivot-1] and aSig[aPivot+1..aLen]
/// \param[in]  aLen:        Length-1 of aSig

///	\return     Return the median value.

/// \author     Samuel Gidel
///	\author     Vincent Simard Bilodeau

///	\since		  March 11, 2016
// ****************************************************************************
void MATH_QuickSort ( int32_t *aSig, 
                      int32_t aLeft,
                      int32_t aRight )
{
  int32_t lPivot, lPartitionPoint;
  if (aRight - aLeft <= 0) 
  {
    return;
  }
  else {
    lPivot = aSig[aRight];
    lPartitionPoint = MATH_Partition(aSig, aLeft, aRight, lPivot );
    MATH_QuickSort(aSig, aLeft, lPartitionPoint - 1);
    MATH_QuickSort(aSig, lPartitionPoint + 1, aRight);
  }
}

// ****************************************************************************
///	Function:	  MATH_Sort

///	\brief      Sort an array [0 to aLen-1] into ascending numerical order using Quiksort algorithm.

/// \param[in] *aSig:        Signal values from 0 to aLen-1.
/// \param[in]  aLen:        Length-1 of aSig

///	\return     Return the median value.

/// \author     Samuel Gidel
///	\author     Vincent Simard Bilodeau

///	\since		  June 06, 2016
// ****************************************************************************
void MATH_Sort ( uint32_t *aSig,
                 uint32_t aLen )
{
  uint32_t  i,j, lIdx;
  uint32_t lMin;
  static uint32_t lTmp[MATH_NB_CHANNELS];

  if (aLen > MATH_NB_CHANNELS)
    return;

  for (i = 0; i < aLen; i++)
  {
    lMin = aSig[i]; lIdx = i;
    for (j = 0; j < aLen; j++)
    {
      if (lMin > aSig[j])
      {
        lMin = aSig[j];
        lIdx = j;
      }
    }
    lTmp[i] = lMin;
    aSig[lIdx] = MATH_STUPID_VALUE;
  }

  lMin = aSig[0];
  for (j = 0; j < aLen; j++)
  {
    if (lMin > aSig[j])
    {
      lTmp[aLen-1] = aSig[j];
      break;
    }
  }

  for (i = 0; i < aLen; i++)
    aSig[i] = lTmp[i];
}

// ****************************************************************************
///	Function:	  MATH_Median

///	\brief      Compute median, the number separating the higher half of a data sample.

/// \param[in] *aSig:        Signal values from 0 to aLen-1.
/// \param[in]  aLen:        Length of aSig

///	\return     Return the median value.

/// \author     Samuel Gidel
///	\author     Vincent Simard Bilodeau

///	\since		  March 11, 2016
// ****************************************************************************
uint16_t MATH_Median ( uint16_t *aSig,
                       uint16_t aLen )
{
  return (aSig[aLen >> 1]);
}

/// ****************************************************************************
///  function    MATH_ConvFP
///
/// \brief       convolution algorithm
/// \pre         You must declare the space required for the output signal.
/// \pre    
///
/// \param[in]  *aTrc:           Pointer to input signal.
/// \param[out] *aXtalk:         Pointer to output signal, memory allocated. 
///                              Must be a different buffer from input.
///                              Filtering cannot be done in place.
/// \param[in]   aTrcLen:        Number of samples in signal.
/// \param[in]  *aCoeff:         Filter coefficients.
/// \param[in]   aFilterLen:     Length of filter.
///
/// \return      Operation status.
/// \retval      0 on success
/// \retval      Negative on error.
///
/// \authors     Samuel Gidel
/// \authors     Jean-Fran�ois Bernier
///
/// \since       06 march 2017
/// ****************************************************************************
//convolution algorithm
uint8_t MATH_ConvFP(int32_t *aTrc, int32_t *aCoeff, uint16_t aTrcLen, uint16_t aFilterLen, int32_t *aXtalk, uint8_t aFilterSumBits)
{
  uint16_t lConv;
  uint16_t i, j, i1;
  int32_t lTmp;


  if (aTrcLen <= aFilterLen)
    return FALSE;

  //allocated convolution array	
  lConv = aTrcLen + aFilterLen - 1;

  //convolution process
  for (i = 0; i<lConv; i++)
  {
    i1 = i;
    lTmp = 0;
    for (j = 0; j<aFilterLen; j++)
    {
      if (i1<aTrcLen)
        lTmp = lTmp + (aTrc[i1] * aCoeff[j]);

      i1 = i1 - 1;
    }
    aXtalk[i] = lTmp >> aFilterSumBits;
  }
  return TRUE;
}


#if defined (_LCA3_3DF) || defined (_LCA3_MEMS)
/// ****************************************************************************
/// \fn     MATH_GenIndexMapToLCA3
///
/// \brief  Generates the tables needed to convert channel indices between
///         memory-order and presentation-order indices.
///
/// \param[in] vSeg: Number of vertical channels. Can be:
///             - 256 for full resolution MEMS
///             - 172 for 2/3 resolution MEMS
///             - 16 for 3D FLASH
///
/// \return     Error code. 0 on success, negative on error.
///
/// \author     Jean-Francois Bernier
/// \since      2017-11-15
/// ****************************************************************************
int32_t MATH_GenIndexMap ( const uint16_t vSeg, uint16_t hSeg )
{
    uint16_t nBank;
    const uint16_t INVALID = 0xFFFF; // Value used to invalidate index arrays
    uint16_t a, b, c, d, idx = 0;

    // Invalidate all values
    memset( gMapMem2Trc, INVALID, sizeof(gMapMem2Trc) );
    memset( gMapMem2TrcInverted, INVALID, sizeof(gMapMem2TrcInverted) );
    memset( gMapTrc2Mem, INVALID, sizeof(gMapTrc2Mem) );
    memset( gMapTrc2MemInverted, INVALID, sizeof(gMapTrc2MemInverted) );

    uint16_t nTrc = vSeg * hSeg;
    const uint16_t muxmap[MATH_ADC_COUNT][MATH_MUX_COUNT] =
    { \
        {1,  3,  5,  7},\
        {9,  11, 13, 15},\
        {17, 19, 21, 23},\
        {25, 27, 29, 31},\
        {33, 35, 37, 39},\
        {41, 43, 45, 47},\
        {49, 51, 53, 55},\
        {57, 59, 61, 63},\
        {6,  4,  2,  0},\
        {14, 12, 10, 8},\
        {22, 20, 18, 16},\
        {30, 28, 26, 24},\
        {38, 36, 34, 32},\
        {46, 44, 42, 40},\
        {54, 52, 50, 48},\
        {62, 60, 58, 56},\
    };

    uint16_t angmap[312];
    uint16_t nAng[3]   = {0,0,0};
    uint16_t angoff[3] = {0,0,0};
    uint16_t offset = 0;

    if (( vSeg == 256 ) || ( vSeg == 172 ))
    {
        if( vSeg == 172 )
        {
          // The 256 angles are divided in 2 banks of 86 + 86 = 172 angles
          // These banks spread the whole fov, so readings have a stride of 2
          nBank   = 2;
          nAng[0] = 86;
          nAng[1] = 86;
        }
        else
        {
          // The 256 angles are divided in 3 banks of 86 + 85 + 85 = 256 angles
          // These banks spread the whole fov, so readings have a stride of 3
          nBank   = 3;
          nAng[0] = 86;
          nAng[1] = 85;
          nAng[2] = 85;
        }
        

        for ( a = 1 ; a < nBank ; ++a )
        {
            angoff[a] = angoff[a-1] + nAng[a-1];
        }

        for( b = 0; b < nBank; ++b )
        {
            angmap[a] = nBank*a + b;
        }

    }
    else if ( ( vSeg == 16) || (vSeg == 64) || (vSeg == 128) || (vSeg == 312) )
    {

        nBank   = 1;
        nAng[0] = vSeg;

        for ( a = 0 ; a < vSeg ; ++a )
        {
            angmap[a] = a;
        }
    }
    else if ( ( vSeg == 8) ) // 
    {

      nBank = 2;

      const uint16_t lines[8] = {0,2,4,6,1,3,5,7};

      for ( b = 0 ; b < 8 ; ++b ) // num vertical banks
      {
        for ( a = 0 ; a < 2 ; ++a ) //num horizontal banks
        {
          // for each line, PIN channel 1,3,5,...31 are sampled first, then, 0,2,4,...,30
          for( c = 0; c < 16; ++c) // LCA2 ASIC sample # 
          {
            // horizontal channel 0 is the one looking
            gMapMem2Trc[idx] = lines[b] * 32 + c*2 + (a+1)%2; 
            gMapTrc2Mem[gMapMem2Trc[idx]] = idx;
            ++idx;
          }
        }
      }

      return 0;
    }
    else
    {
        printf("Error: Number of vertical channel not supported: %d\n", vSeg);
        return -1;
    }

    for ( b = 0 ; b < nBank ; ++b )
    {
        for ( d = 0 ; d < nAng[b] ; ++d )
        {
            //for ( a = MATH_MUX_COUNT ; a > 0 ; --a )
            for ( a = 0 ; a < MATH_MUX_COUNT ; a++ )
            {
                for ( c = 0 ; c < MATH_ADC_COUNT ; ++c )
                {
                    //gMapMem2Trc[idx] = angmap[d+angoff[b]]*hSeg + muxmap[c][a-1];
                    gMapMem2Trc[idx] = angmap[d+angoff[b]]*hSeg + muxmap[c][a];
                    gMapTrc2Mem[gMapMem2Trc[idx]] = idx;
                    //printf("nBank: %d nAng[b]: %d muxmap[c][a]: %d  Mux:%d  Adc: %d\r\n", b, nAng[b], muxmap[c][a-1], a-1, c);
                    ++idx;
                }
            }
        }
    }
    
    // The inverted maps are computed only for the Eagle. They are not used in the other systems.
    for ( a=0 ; a < vSeg ; a++ )
    {
        memcpy((void*) &gMapMem2TrcInverted[a * LCA3_NB_HONRIZONTAL_CHANNELS],
                (void*) &gMapMem2Trc[nTrc - ((a+1) * LCA3_NB_HONRIZONTAL_CHANNELS)],
                LCA3_NB_HONRIZONTAL_CHANNELS * sizeof(uint16_t));

        memcpy((void*) &gMapTrc2MemInverted[a * LCA3_NB_HONRIZONTAL_CHANNELS],
                (void*) &gMapTrc2Mem[nTrc - ((a+1) * LCA3_NB_HONRIZONTAL_CHANNELS)],
                LCA3_NB_HONRIZONTAL_CHANNELS * sizeof(uint16_t));
    }


    



#if defined(DEBUG)
    printf("gMapMem2Trc\n");
    for ( a=0 ; a < vSeg ; a++ )
    {
        for (  b=0 ; b < LCA3_NB_HONRIZONTAL_CHANNELS ; b++ )
        {
            printf("%4d ", gMapMem2Trc[(a*LCA3_NB_HONRIZONTAL_CHANNELS)+b]);
        }
        printf("\n");
    }

    printf("gMapMem2TrcInverted\n");
    for ( a=0 ; a < vSeg ; a++ )
    {
        for ( b=0 ; b < LCA3_NB_HONRIZONTAL_CHANNELS ; b++ )
        {
            printf("%4d ", gMapMem2TrcInverted[(a*LCA3_NB_HONRIZONTAL_CHANNELS)+b]);
        }
        printf("\n");
    }

    printf("gMapTrc2Mem\n");
    for ( a=0 ; a < vSeg ; a++ )
    {
        for (  b=0 ; b < LCA3_NB_HONRIZONTAL_CHANNELS ; b++ )
        {
            printf("%4d ", gMapTrc2Mem[(a*LCA3_NB_HONRIZONTAL_CHANNELS)+b]);
        }
        printf("\n");
    }

    printf("gMapTrc2MemInverted\n");
    for ( a=0 ; a < vSeg ; a++ )
    {
        for ( b=0 ; b < LCA3_NB_HONRIZONTAL_CHANNELS ; b++ )
        {
            printf("%4d ", gMapTrc2MemInverted[(a*LCA3_NB_HONRIZONTAL_CHANNELS)+b]);
        }
        printf("\n");
    }
#endif // DEBUG

    // TODO: @maximel-2018-02-08 : this should be a unit test
    // Validation: check that...
    // - number of generated values is as expected
    // - all table entries have been set (not invalid)
    // - all values are unique

    if ( idx != nTrc ){
        printf("idx: %d != nTrc: %d\r\n", idx, nTrc);
        return -1;
        }


    for ( a = 0 ; a < nTrc ; ++a )
    {
        if ( gMapMem2Trc[a] == INVALID || gMapTrc2Mem[a] == INVALID ){
            printf("__FUNC__: Invalid data at index %d.\n", a);
            return -1;
            }
        // Check all values are unique (no copies)
        for ( b = a+1 ; b < nTrc ; ++b )
        {
            if ( gMapMem2Trc[a] == gMapMem2Trc[b] || gMapTrc2Mem[a] == gMapTrc2Mem[b] ){
                printf("__FUNC__: Duplicate data at index (%d,%d), value (%d,%d) or (%d,%d).\n", a, b, gMapMem2Trc[a], gMapMem2Trc[b], gMapTrc2Mem[a], gMapTrc2Mem[b]);
                return -1;
            }

        }
    }

    return 0;
}

/// ****************************************************************************
///  function    MATH_GetWhiteNoise
///
/// \brief       Get the Sigma variation of one trace
/// \pre             
///
/// \param[in]  *aTrc:				Pointer to input signal.
/// \param[in]   aColNbr:			Col nbr to analyse 
/// \param[out] *aWhiteNoiseCount:  Return the level of the White Noise in count        
///
/// \return      Operation status.
/// \retval      0 on success
/// \retval      Negative on error.
///
/// \authors     Gabriel St-Pierre
///
/// \since       25 Jan 2018
/// ****************************************************************************
int32_t MATH_GetWhiteNoise(const int16_t *aTrc, uint16_t aColNbr, int32_t *aWhiteNoiseCount, BOOL aScanDirection){
	uint32_t    i, j;
	uint32_t    uTrcPos = 0;
	int32_t     iData[MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE][MATH_NB_HONRIZONTAL_CHANNELS];
	int32_t     iMax[MATH_NB_HONRIZONTAL_CHANNELS];
	int32_t     iOffset = 0;
	int32_t     iDiff = 0;
	uint32_t    iNbrData = 0;
	uint32_t    iIndex = 0;
	uint32_t    iPos = 0;

	if ((aColNbr < 1) || (aColNbr > MATH_NB_VERTICAL_CHANNELS)){
		printf("Error bad line nbr: %d!\r\n", aColNbr);
		return -1;
	}

	if (!aTrc){
		printf("Error data invalid!\r\n");
		return -2;
	}

	//Clear data
	memset(iMax, 0, MATH_NB_HONRIZONTAL_CHANNELS * sizeof(int32_t));
	for (i = 0; i < MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE; i++){
		memset(iData[i], 0, MATH_NB_HONRIZONTAL_CHANNELS * sizeof(int32_t));
	}

	for (i = 0; i < MATH_NB_HONRIZONTAL_CHANNELS; i++){
		uTrcPos = ((aColNbr * MATH_NB_HONRIZONTAL_CHANNELS) + i);
		uTrcPos = ((MATH_GetMapMem2TrcIndex((uint16_t)uTrcPos, aScanDirection) * 512));

		iNbrData = 0;
		iOffset = 0;
		//Get the right data and calculate the means value to get the offset
		for (j = 0; j < MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE; j++){
			iIndex = (uint32_t)(uTrcPos + MATH_WHITE_NOISE_SAMPLES_BEG + j - 1);
			iData[j][i] = (int16_t)aTrc[iIndex];
			iOffset += iData[j][i];
		}
		iOffset = iOffset / MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE;

		//apply the offset to the value and get the max value
		for (j = 0; j < MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE; j++){
			iData[j][i] = iData[j][i] - iOffset;
			if (iData[j][i] > iMax[i]) iMax[i] = iData[j][i];
		}
	}

	for (j = 0; j < MATH_NB_HONRIZONTAL_CHANNELS - 1; j++){
		//Get the Max diff value
		if (abs(iMax[j] - iMax[j + 1]) > iDiff){
			iDiff = abs(iMax[j] - iMax[j + 1]);
			iPos = j;
		}
	}

	*aWhiteNoiseCount = iDiff;
	return 0;
}

/// ****************************************************************************
///  function    MATH_GetSigma
///
/// \brief       Get the varience value for one pixel trace channel
/// \pre             
///
/// \param[in]  *aTrc:				Pointer to input signal.
/// \param[in]   aPixelTrc:			Col nbr to analyse 
/// \param[out] *aSigma:			Return the level of the White Noise in count        
///
/// \return      Operation status.
/// \retval      0 on success
/// \retval      Negative on error.
///
/// \authors     Gabriel St-Pierre
///
/// \since       25 Jan 2018
/// ****************************************************************************
int32_t MATH_GetSigma(const int16_t *aTrc, uint16_t aPixelTrc, int32_t *aSigma, BOOL aScanDirection ){
	uint32_t    j;
	uint32_t    uTrcPos = 0;
	int32_t     iData=0;
	int32_t     imeans = 0;

	int32_t     iSum = 0;
	uint32_t    iIndex = 0;
	uint32_t    iPos = 0;

	if ((aPixelTrc < 1) || (aPixelTrc >(MATH_NB_HONRIZONTAL_CHANNELS * MATH_NB_VERTICAL_CHANNELS))){
		printf("Error bad pixel segment nbr: %d!\r\n", aPixelTrc);
		return -1;
	}

	if (!aTrc){
		printf("Error data invalid!\r\n");
		return -2;
	}

	uTrcPos = ((MATH_GetMapMem2TrcIndex((uint16_t)aPixelTrc, aScanDirection) * 512));
	imeans = 0;
	//Get the right data and calculate the means value
	for (j = 0; j < MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE; j++){
		iIndex		= (uint32_t)(uTrcPos + MATH_WHITE_NOISE_SAMPLES_BEG + j - 1);
		iData	= (int16_t)aTrc[iIndex];
		iSum		= ((iData * iData) + iSum); // Get exponent 2
		imeans		+= iData;
		}
	imeans	= imeans / MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE;
	imeans	= imeans * imeans; //Get exponent 2
	iSum	= iSum / MATH_WHITE_NOISE_NBR_SAMPLES_PER_TRACE;
	
	*aSigma = (int32_t)sqrt(iSum - imeans);
	return 0;
}

/// ****************************************************************************
///  function    MATH_GetVbiasModel
///
/// \brief       Get the Vbias value in function of the ambiant noise
/// \pre             
///
/// \param[in]  VbiasIn:			Actual Vbias value.
/// \param[in]  AmbiantNoise:		Ambiant noise value 
/// \param[out] *OptVbias:			Return the optimal Vbias in function of the model        
///
/// \return      Operation status.
/// \retval      0 on success
/// \retval      Negative on error.
///
/// \authors     Gabriel St-Pierre
///
/// \since       27 Fev 2018
/// ****************************************************************************
#define APD_TCOMP_FACTOR       1.1590f
#define APD_TCOMPAPD_FACTOR    0.8628f
#define APD_TREFCOMP_FACTOR    0.1563f
#define APD_VBIAS_FACTOR       13.8044f
#define APD_TCOMP_REF          42.9835f
#define APD_TMAX_VAL           1600
#define APD_TMIN_VAL           -320

#ifdef ALGO_APDVBIAS_WHITENOISE
    #define APD_MAX_MODEL          1022
    #define APD_MIN_VBIAS          1439
    #define APD_MAX_VBIAS          2158
    const uint32_t APDVbias[] = { 2157, 2157, 2157, 2157, 2157, 2157, 2157, 2157, 2157, 2155, 2152, 2149, 2146, 2144, 2141, 2138, 2136, 2133, 2130, 2128, 2125, 2122, 2119, 2117, 2114, 2111, 2102, 2093, 2083, 2074, 2065, 2055, 2046, 2037, 2027, 2018, 2009, 1999, 1990, 1981, 1971, 1962, 1953, 1943, 1941, 1940, 1938, 1936, 1934, 1932, 1930, 1928, 1927, 1925, 1923, 1921, 1919, 1917, 1915, 1914, 1912, 1910, 1908, 1906, 1904, 1902, 1900, 1899, 1897, 1895, 1893, 1891, 1889, 1887, 1886, 1884, 1882, 1880, 1878, 1876, 1874, 1872, 1871, 1869, 1867, 1865, 1863, 1861, 1859, 1858, 1856, 1854, 1852, 1850, 1848, 1846, 1845, 1843, 1841, 1839, 1837, 1835, 1833, 1831, 1830, 1828, 1826, 1824, 1822, 1820, 1818, 1817, 1815, 1813, 1811, 1809, 1807, 1805, 1804, 1802, 1800, 1798, 1796, 1794, 1792, 1790, 1789, 1787, 1785, 1783, 1781, 1779, 1777, 1776, 1774, 1772, 1770, 1768, 1766, 1764, 1762, 1761, 1759, 1757, 1755, 1753, 1751, 1749, 1748, 1746, 1744, 1742, 1740, 1738, 1736, 1735, 1733, 1731, 1729, 1727, 1725, 1723, 1721, 1720, 1718, 1716, 1714, 1712, 1710, 1708, 1707, 1705, 1703, 1701, 1699, 1697, 1695, 1694, 1692, 1690, 1688, 1686, 1684, 1682, 1680, 1679, 1677, 1675, 1673, 1671, 1669, 1667, 1666, 1664, 1662, 1660, 1658, 1656, 1654, 1652, 1651, 1649, 1647, 1645, 1643, 1641, 1639, 1638, 1636, 1634, 1632, 1630, 1628, 1626, 1625, 1623, 1621, 1619, 1617, 1615, 1613, 1611, 1610, 1608, 1606, 1604, 1602, 1600, 1598, 1597, 1595, 1593, 1591, 1589, 1587, 1585, 1583, 1582, 1580, 1578, 1576, 1574, 1572, 1570, 1569, 1567, 1565, 1563, 1561, 1559, 1557, 1556, 1554, 1552, 1550, 1548, 1546, 1544, 1542, 1541, 1539, 1537, 1535, 1533, 1531, 1529, 1528, 1526, 1524, 1522, 1520, 1518, 1516, 1515, 1513, 1511, 1509, 1507, 1505, 1503, 1501, 1500, 1498, 1496, 1494, 1492, 1490, 1488, 1487, 1485, 1483, 1481, 1479, 1477, 1475, 1473, 1472, 1470, 1468, 1466, 1464, 1462, 1460, 1459, 1457, 1455, 1453, 1451, 1449, 1447, 1446, 1444, 1442, 1440, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438 };
#else
    #define APD_MAX_MODEL          232
    #define APD_MIN_VBIAS          1439
    #define APD_MAX_VBIAS          2158
    const uint32_t APDVbias[] = { 2027, 1943, 1934, 1926, 1917, 1908, 1899, 1890, 1881, 1872, 1864, 1855, 1846, 1837, 1828, 1819, 1810, 1802, 1793, 1784, 1775, 1766, 1757, 1748, 1739, 1731, 1722, 1713, 1704, 1695, 1686, 1677, 1669, 1660, 1651, 1642, 1633, 1624, 1615, 1606, 1598, 1589, 1580, 1571, 1562, 1553, 1544, 1536, 1527, 1518, 1509, 1500, 1491, 1482, 1474, 1465, 1456, 1447, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438, 1438 };
#endif

int32_t MATH_GetVbiasModel(int32_t TempApd, uint32_t AmbiantNoise, uint32_t *OptVbias, uint32_t TempCompEn){

	//Get the value in function of the model
	*OptVbias = APDVbias[AmbiantNoise];
	if (AmbiantNoise >= APD_MAX_MODEL) *OptVbias = APD_MIN_VBIAS;
	
	//Calculate the new Vref Vbias Temp compensation
	if (TempCompEn != 0){
		//Verify the Temp limits
		if (TempApd > APD_TMAX_VAL) TempApd = APD_TMAX_VAL;
		if (TempApd < APD_TMIN_VAL) TempApd = APD_TMIN_VAL;

		*OptVbias = (uint32_t)lrint(((float)APD_TCOMPAPD_FACTOR * ((float)(TempApd)+(float)APD_TCOMP_FACTOR * (*OptVbias - (float)APD_VBIAS_FACTOR * ((float)APD_TCOMP_REF - (float)APD_TREFCOMP_FACTOR)))));
	}

	//printf("Out Vbias: %d\r\n", *OptVbias);

	//Verify APD limits
	if (*OptVbias > APD_MAX_VBIAS) *OptVbias = APD_MAX_VBIAS;
	if (*OptVbias < APD_MIN_VBIAS) *OptVbias = APD_MIN_VBIAS;

	return 0;
}

/// ****************************************************************************
///  function    MATH_Smooth
///
/// \brief       Conditionne the signal with a smooth effect
/// \pre             
///
/// \param[in]  mData:			Get the data to confditionne
/// \param[out] *mVal:			return the signal conditionned         
///
/// \return      Operation status.
/// \retval      0 on success
/// \retval      Negative on error.
///
/// \authors     Gabriel St-Pierre
///
/// \since       27 Fev 2018
/// ****************************************************************************
#ifdef ALGO_APDVBIAS_WHITENOISE
    #define MATH_VALUE_SMOOTH      50
#else
    #define MATH_VALUE_SMOOTH      100
#endif
int32_t MATH_Smooth(uint32_t mData, uint32_t *mVal){
	static uint32_t mDatabuf[MATH_VALUE_SMOOTH];
	static bool mInitialised = FALSE;
	static uint32_t mIndex = 0;
	uint32_t mMean = 0;
	int i = 0;

	if (mInitialised == FALSE){
		mInitialised = TRUE;
		memset(mDatabuf, 0, MATH_VALUE_SMOOTH);
	}

	mDatabuf[mIndex] = mData;
	mIndex++;
	if ((mIndex % MATH_VALUE_SMOOTH) == 0)
		mIndex = 0;

	for (i = 0; i < MATH_VALUE_SMOOTH; i++){
		mMean = mMean + mDatabuf[i];
	}
	fesetround(FE_UPWARD);
	*mVal = (uint32_t)lrint((float)((float)mMean / (float)MATH_VALUE_SMOOTH));
	return 0;
}
/// ****************************************************************************
///  function    MATH_Getpolyval
///
/// \brief       Get the polynome value at x position 
/// \pre             
///
/// \param[in]  VbiasIn:			Actual Vbias value.
/// \param[in]  AmbiantNoise:		Ambiant noise value 
/// \param[out] *OptVbias:			Return the optimal Vbias in function of the model        
///
/// \return      Operation status.
/// \retval      0 on success
/// \retval      Negative on error.
///
/// \authors     Gabriel St-Pierre
///
/// \since       27 Fev 2018
/// ****************************************************************************
int32_t MATH_Getpolyval(const float polycoef[], uint32_t x_pos, uint32_t nbrCoef, float *Result){
	uint32_t i = 0;
	float ResVal = polycoef[nbrCoef-1];

	if ((polycoef == NULL) | (x_pos > 225.0) | (nbrCoef > 10)){
		return -1;
	}
	for (i = 1; i < nbrCoef ; i++){
		ResVal = ((float)(polycoef[nbrCoef - i - 1] * powf(x_pos, i))) + ResVal;
	}

	*Result = ResVal;

	return 0;
}

// ****************************************************************************
///	Function:   MATH_SetConfig

///	\brief      Set fundamental parameters in connection with a trace.

/// \param[in]  None.

///	\return     None.

///	\author     Samuel Gidel
///	\since      March 05, 2018
// ****************************************************************************
void MATH_SetConfig( void )
{
  gInitBuffer = FALSE;
  gIdxBuffer = 0;
}

/// ****************************************************************************
///  function    MATH_Accumulation
///
/// \brief       Accumulate trace up to four times.
///
/// \param[in/out]   aTrc : trace to accumulate then accumulate
///
/// \return      gInitBuffer: 0 then accumulator is ready otherwise no.
///
/// \authors     Samuel gidel
///
/// \since       19 February 2018
/// ****************************************************************************
void
MATH_Accumulation ( int16_t *aTrc,
                    int16_t aNbSplMax,
                    int16_t aIdxSpl2acc,
                    int16_t aIdxSpl4acc )
{
  int32_t i, j;
  static int32_t  lTmp[MATH_NB_SAMPLES_PER_CHANNEL_MAX*MATH_NB_CHANNELS];

  // update the accumulator
  for (i = 0; i < MATH_NB_CHANNELS; i++)
  {
    for (j = aIdxSpl2acc; j < aNbSplMax; j++)
      gTrcBuffer[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX) + (gIdxBuffer*MATH_NB_SAMPLES_PER_CHANNEL_MAX*MATH_NB_CHANNELS)] = 
                aTrc[(i*MATH_NB_SAMPLES_PER_CHANNEL_MAX) + j];
  }

  if (gInitBuffer == TRUE)
  {
    // average different part of traces
    for (i = 0; i < MATH_NB_CHANNELS; i++)
    {
      for (j = aIdxSpl2acc; j < aNbSplMax; j++)
      {
        gTmp2[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)] = 
                      gTrcBuffer[ j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX) + (gBufferLUT[gIdxBuffer][0] * MATH_NB_SAMPLES_PER_CHANNEL_MAX * MATH_NB_CHANNELS)] +
                      gTrcBuffer[ j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX) + (gBufferLUT[gIdxBuffer][1] * MATH_NB_SAMPLES_PER_CHANNEL_MAX * MATH_NB_CHANNELS)];
        gTmp4[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)] = 
                      gTrcBuffer[ j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX) + (gBufferLUT[gIdxBuffer][2] * MATH_NB_SAMPLES_PER_CHANNEL_MAX * MATH_NB_CHANNELS)] +
                      gTrcBuffer[ j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX) + (gBufferLUT[gIdxBuffer][3] * MATH_NB_SAMPLES_PER_CHANNEL_MAX * MATH_NB_CHANNELS)];
      }
    }

    for (i = 0; i < MATH_NB_CHANNELS; i++)
    {
      for (j = aIdxSpl2acc; j < aIdxSpl4acc; j++)
      {
        lTmp[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)] = gTmp2[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)] >> 1;
        aTrc[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)] = lTmp[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)];
      }
      for (j = aIdxSpl4acc; j < aNbSplMax; j++)
      {
        lTmp[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)] = (gTmp2[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)] + gTmp4[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)]) >> 2;
        aTrc[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)] = lTmp[j + (i*MATH_NB_SAMPLES_PER_CHANNEL_MAX)];
      }
    }
  }
  gIdxBuffer++;
  if (gIdxBuffer == MATH_NB_ACC_BUFFER)
  {
    gIdxBuffer = 0;
    gInitBuffer = TRUE;
  }

}

/// ****************************************************************************
///  function    MATH_GetAccTrcBuffer
///
/// \brief       Gets the frame at lIdxBuffer
///
/// \param[in]   lIdxBuffer: frames desired
///
/// \return      Pointer to the Map.
///
/// \authors     Samuel gidel
///
/// \since       05 March 2017
/// ****************************************************************************
int16_t*
MATH_GetAccTrcBuffer ( void )
{
  return &gTrcBuffer[0];
}

/// ****************************************************************************
///  function    MATH_GetAccTmpX
///
/// \brief       Gets internal variable for debug
///
/// \param[in]   lNum: temporary buffer desired
///
/// \return      Pointer to the Map.
///
/// \authors     Samuel gidel
///
/// \since       05 March 2017
/// ****************************************************************************
int32_t *
MATH_GetAccTmpX(uint16_t lNum)
{
  if (lNum == 2)
    return &gTmp2[0];
  else
    return &gTmp4[0];
}

/// ****************************************************************************
///  function    MATH_GetMapMem2TrcIndex
///
/// \brief       Gets the trace index from the memory index
///
/// \param[in]   index: The memory index.
/// \param[in]   inverted: If true, ask for the index of an inverted trace.
///                  
/// \return      The trace index of the given memory index.
///
/// \authors     Jonathan Poulin
///
/// \since       07 September 2018
/// ****************************************************************************
uint16_t
MATH_GetMapMem2TrcIndex ( uint16_t index, BOOL inverted )
{
	if ( inverted )
	{
		return gMapMem2TrcInverted[index];
	}
	else
	{
		return gMapMem2Trc[index];
	}
}

/// ****************************************************************************
///  function    MATH_CopyMapMem2TrcAt
///
/// \brief       copies gMapMem2Trc to user provided memory
///
/// \param[in]   size (the number of channels, i.e. 256 * 64 for the LCA3_MEMS, 16 * 64 for the LCA3_FLASH
/// \param[out]  dest pointer to destination dest
///
/// \authors     Maxime Lemonnier
///
/// \since       8 Febbruary 2018
/// ****************************************************************************
void
MATH_CopyMapMem2Trc (size_t size, uint16_t * dest, BOOL inverted)
{
  memcpy(dest, inverted ? gMapMem2TrcInverted : gMapMem2Trc, size * sizeof(uint16_t));
}

/// ****************************************************************************
///  function    MATH_GetMapTrc2MemIndex
///
/// \brief       Gets the memory index from the trace index
///
/// \param[in]   index: The trace index.
/// \param[in]   inverted: If true, get the index for an inverted trace.
///                  
/// \return      The memory index of the given trace index.
///
/// \authors     Jonathan Poulin
///
/// \since       07 September 2018
/// ****************************************************************************
uint16_t
MATH_GetMapTrc2MemIndex ( uint16_t index, BOOL inverted )
{
	if ( inverted )
	{
		return gMapTrc2MemInverted[index];
	}
	else
	{
		return gMapTrc2Mem[index];
	}
}

/// ****************************************************************************
///  function    MATH_CopyMapTrc2Mem
///
/// \brief       copies gMapTrc2Mem to user provided memory
///
/// \param[in]   size (the number of channels, i.e. 256 * 64 for the LCA3_MEMS, 16 * 64 for the LCA3_FLASH
/// \param[out]  dest pointer to destination dest
///
/// \authors     Maxime Lemonnier
///
/// \since       8 Febbruary 2018
/// ****************************************************************************
void
MATH_CopyMapTrc2Mem ( size_t size, uint16_t * dest, BOOL inverted)
{
  memcpy(dest, inverted ? gMapTrc2MemInverted : gMapTrc2Mem, size * sizeof(uint16_t));
}

#endif

#ifdef _USE_CVI_GUI
/// ****************************************************************************
///  function       MATH_Test

/// \brief          dummy function.

/// \param[in]      aVal:  dummy value.

/// \return         Returns aVal*2.

/// \author         Samuel Gidel 
/// \author         Vincent Simard Bilodeau

/// \since          25 February 2016
/// ****************************************************************************
uint8_t MATH_Test(uint8_t aVal)
{
  return (aVal * 2);
}
/// ****************************************************************************
///  function    MATH_GetConstants
///
/// \brief       Gets mathematics constants.
///
/// \param[out]  *cst: Struct filled with the module's constants.
///
/// \authors     Samuel Gidel
///
/// \since       25 February 2016
/// ****************************************************************************
void MATH_GetConstants(sMATH_Constants *cst)
{
  cst->DELAY_SCALE_BITS     = MATH_DELAY_SCALE_BITS;
  cst->RAW_AMPLITUDE_SCALE  = MATH_RAW_AMPLITUDE_SCALE;
  cst->FILTER_SUM_BITS      = MATH_DEFAULT_FILTER_SUM_BITS;
  cst->NB_CHANNELS          = MATH_NB_CHANNELS;
}
#endif // _USE_CVI_GUI
