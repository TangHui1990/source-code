
#ifndef _XLSD_H_
#define _XLSD_H_



/*
 * XL_iDecodeBar
 *
 * input param £º
 *               nWidth    -- bmp width
 *               nHeight   -- bmp height
 *               pBmpBit   -- bmp buffer
 *               nDataSize -- pData size
 * output param:
 *               nType     -- code type
 *               pData     -- barcode info
 * return :
 *          =-1 is parameter error
 *          =-2 is invalid error
 *          = 0 is decode end
 *          > 0 is barcode length
 */
int  XL_iDecodeBar(int nWidth, int nHeight, char *pBmpBit, int *nType, char *pData, int nDataSize);


#endif

