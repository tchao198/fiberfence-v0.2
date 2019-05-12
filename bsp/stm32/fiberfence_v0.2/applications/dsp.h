#ifndef DSP_H_
#define DSP_H_
#include <arm_math.h>


#define NumStages 2 /* 2阶IIR滤波器个数 */
#define TEST_LENGTH_SAMPLES 1000 /* 采样点数 */

/* 巴特沃斯低通滤波器系数 50Hz*/
const float32_t IIRCoeffs32LP[5*NumStages] = {
	1.0f, -2.0f, 1.0f, 1.9752696348518732f, -0.97624479235943995f,
	1.0f, -2.0f, 1.0f, 1.9426382305401138f, -0.94359727847036723f
};

const float32_t ScaleValue = 0.98787860680282835f * 0.97155887725262025f;

#endif
