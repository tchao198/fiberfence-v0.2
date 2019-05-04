#ifndef __AD7924_H
#define __AD7924_H
#include "sys.h"

//ADC转换指令 cmd
/*--------------------------------------
  |  15 | 14 |  13 |  12 | 11 | 10 | 9 | 8 |  7 |  6  |  5  |   4  | 3 | 2 | 1 | 0 |
	|WRITE|SEQ1|DONTC|DONTC|ADD1|ADD0|PM1|PM0|SEQ0|DONTC|RANGE|CODING| 0 | 0 | 0 | 0 |
	
	RANGE:  0 2*ref
				  1 ref
	CODING: 0 补码
				  1 标准二进制
*---------------------------------------*/
#define CHAN1_CONVERSION    0x8330      //0x8330-ref=2.5v   0x8310-2*ref=5v
#define CHAN2_CONVERSION    0x8730      //0x8730-ref=2.5v   0x8710-2*ref=5v
#define CHAN3_CONVERSION    0x8B30      //0x8B30-ref=2.5v   0x8B10-2*ref=5v
#define CHAN4_CONVERSION    0x8F30      //0x8F30-ref=2.5v   0x8F10-2*ref=5v

#define	AD7924_CS 		PGout(15)  
void AD7924_Init(void);
u16 ADC_Conversion(u16 cmd);
#endif
