/*
 * ADF_PDM.h
 *
 *  Created on: May 14, 2024
 *      Author: tj925438
 */

#ifndef INC_ADF_PDM_H_
#define INC_ADF_PDM_H_

#define REC_BUFF_SIZE        (192U)		//(1024U)

void ADF_PDM_Init(void);
void ADF_PDM_DeInit(void);
void ADF_SetGain(unsigned long gain);
void ADF_GetToOutBuffer(void);
int  ADF_GetADFState(void);

#endif /* INC_ADF_PDM_H_ */
