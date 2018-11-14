#include "NoiseSuppression.h"
CNoiseSuppression::CNoiseSuppression()
{

}
CNoiseSuppression::~CNoiseSuppression()
{

}
bool CNoiseSuppression::NoiseSuppressionProcessT(char *szFileIn, char *szFileOut, int nSample, int nMode,bool nStyle = true)//bStyle:true代表定点降噪，false代表浮点降噪
{
	if (nStyle)
		return NoiseSuppressionProcessX(szFileIn, szFileOut, nSample, nMode);
	else
		return NoiseSuppressionProcess(szFileIn, szFileOut, nSample, nMode);
}
bool CNoiseSuppression::NoiseSuppressionProcess(char *szFileIn, char *szFileOut, int nSample, int nMode)
{
	if (nSample == 16000 || nSample == 8000)
		return NoiseSuppressionProcessNo32K(szFileIn, szFileOut, nSample, nMode);
	if (nSample == 32000)
		return NoiseSuppressionProcess32K(szFileIn, szFileOut, nSample, nMode);
}
bool CNoiseSuppression::NoiseSuppressionProcessX(char *szFileIn, char *szFileOut, int nSample, int nMode)
{
	if (nSample == 16000 || nSample == 8000)
		return NoiseSuppressionProcessNoX32K(szFileIn, szFileOut, nSample, nMode);
	if (nSample == 32000)
		return NoiseSuppressionProcessX32(szFileIn, szFileOut, nSample, nMode);
}
bool CNoiseSuppression::NoiseSuppressionProcess32K(char *szFileIn, char *szFileOut, int nSample, int nMode)
{
	bool bStatus = true;
	int nRet = 0;
	NsHandle *pNS_inst = NULL;

	FILE *fpIn = NULL;
	FILE *fpOut = NULL;

	char *pInBuffer = NULL;
	char *pOutBuffer = NULL;

	//----------每次从文件读取的数据-----------------------//
	int sec = 100;
	int len = 320*sizeof(short);
	int readFileSize = sec * nSample * sizeof(short);
	pInBuffer = new char[readFileSize];
	pOutBuffer = new char[readFileSize];
	//----------------------------------------------------//
	do
	{
		int i = 0;
		int nFileSize = 0;
		int nTime = 0;
		if (0 != WebRtcNs_Create(&pNS_inst))
		{
			printf("Noise_Suppression32K WebRtcNs_Create err! \n");
			bStatus = false;
			break;
		}

		if (0 != WebRtcNs_Init(pNS_inst, nSample))
		{
			printf("Noise_Suppression32K WebRtcNs_Init err! \n");
			bStatus = false;
			break;
		}

		if (0 != WebRtcNs_set_policy(pNS_inst, nMode))
		{
			printf("Noise_Suppression32K WebRtcNs_set_policy err! \n");
			bStatus = false;
			break;
		}

		fpIn = fopen(szFileIn, "rb");
		if (NULL == fpIn)
		{
			printf("open src file err \n");
			bStatus = false;
			break;
		}
		fpOut = fopen(szFileOut, "wb");
		if (NULL == fpOut)
		{
			printf("open out file err! \n");
			bStatus = false;
			break;
		}
		fseek(fpIn, 0, SEEK_END);
		nFileSize = ftell(fpIn);
		fseek(fpIn, 0, SEEK_SET);


		int  filter_state1[6], filter_state12[6];
		int  Synthesis_state1[6], Synthesis_state12[6];

		memset(filter_state1, 0, sizeof(filter_state1));
		memset(filter_state12, 0, sizeof(filter_state12));
		memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
		memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

		nTime = GetTickCount();
		for (i = 0; i < nFileSize; i += readFileSize)
		{
			readFileSize = fread(pInBuffer, sizeof(char), readFileSize, fpIn);
			for (int j = 0; j < readFileSize; j += 640)
			{
				if (readFileSize -j >= 640)
				{
					short shBufferIn[320] = { 0 };
					short shInL[160], shInH[160];
					short shOutL[160] = { 0 }, shOutH[160] = { 0 };

					memcpy(shBufferIn, (char*)(pInBuffer + j), len);
					//首先需要使用滤波函数将音频数据分高低频，以高频和低频的方式传入降噪函数内部
					WebRtcSpl_AnalysisQMF(shBufferIn, 320, shInL, shInH, filter_state1, filter_state12);

					//将需要降噪的数据以高频和低频传入对应接口，同时需要注意返回数据也是分高频和低频
					if (0 == WebRtcNs_Process(pNS_inst, shInL, shInH, shOutL, shOutH))
					{
						short shBufferOut[320];
						//如果降噪成功，则根据降噪后高频和低频数据传入滤波接口，然后用将返回的数据写入文件
						WebRtcSpl_SynthesisQMF(shOutL, shOutH, 160, shBufferOut, Synthesis_state1, Synthesis_state12);
						memcpy(pOutBuffer + j, shBufferOut, len);
					}
				}
			}
			fwrite(pOutBuffer, sizeof(char), nFileSize, fpOut);
		}
		
		nTime = GetTickCount() - nTime;
		printf("n_s user time=%dms\n", nTime);

		
	} while (0);

	WebRtcNs_Free(pNS_inst);
	fclose(fpIn);
	fclose(fpOut);
	free(pInBuffer);
	free(pOutBuffer);
	return bStatus;
}

bool CNoiseSuppression::NoiseSuppressionProcessX32(char *szFileIn, char *szFileOut, int nSample, int nMode)
{
	bool bStatus = true;
	int nRet = 0;
	NsxHandle *pNS_inst = NULL;

	FILE *fpIn = NULL;
	FILE *fpOut = NULL;

	char *pInBuffer = NULL;
	char *pOutBuffer = NULL;

	//-------------------------------------
	int sec = 100;
	int len = 320 * sizeof(short);
	int readFileSize = sec * nSample * 2;
	pInBuffer = new char[readFileSize];
	pOutBuffer = new char[readFileSize];
	//-------------------------------------

	do
	{
		int i = 0;
		int nFileSize = 0;
		int nTime = 0;
		if (0 != WebRtcNsx_Create(&pNS_inst))
		{
			printf("Noise_Suppression WebRtcNs_Create err! \n");
			bStatus = false;
			break;
		}

		if (0 != WebRtcNsx_Init(pNS_inst, nSample))
		{
			printf("Noise_Suppression WebRtcNs_Init err! \n");
			bStatus = false;
			break;
		}

		if (0 != WebRtcNsx_set_policy(pNS_inst, nMode))
		{
			printf("Noise_Suppression WebRtcNs_set_policy err! \n");
			bStatus = false;
			break;
		}

		fpIn = fopen(szFileIn, "rb");
		if (NULL == fpIn)
		{
			printf("open src file err \n");
			bStatus = false;
			break;
		}
		fpOut = fopen(szFileOut, "wb");
		if (NULL == fpOut)
		{
			printf("open out file err! \n");
			bStatus = false;
			break;
		}
		fseek(fpIn, 0, SEEK_END);
		nFileSize = ftell(fpIn);
		fseek(fpIn, 0, SEEK_SET);

		int  filter_state1[6], filter_state12[6];
		int  Synthesis_state1[6], Synthesis_state12[6];

		memset(filter_state1, 0, sizeof(filter_state1));
		memset(filter_state12, 0, sizeof(filter_state12));
		memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
		memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

		nTime = GetTickCount();
		for (int i = 0; i < nFileSize; i += readFileSize)
		{
			readFileSize = fread(pInBuffer, sizeof(char), readFileSize, fpIn);
			for (int j = 0; j < readFileSize; j += 640)
			{
				if (readFileSize - j >= 640)
				{
					short shBufferIn[320] = { 0 };
					short shInL[160], shInH[160];
					short shOutL[160] = { 0 }, shOutH[160] = { 0 };

					memcpy(shBufferIn, (char*)(pInBuffer+j), len);
					//首先需要使用滤波函数将音频数据分高低频，以高频和低频的方式传入降噪函数内部
					WebRtcSpl_AnalysisQMF(shBufferIn, 320, shInL, shInH, filter_state1, filter_state12);

					//将需要降噪的数据以高频和低频传入对应接口，同时需要注意返回数据也是分高频和低频
					if (0 == WebRtcNsx_Process(pNS_inst, shInL, shInH, shOutL, shOutH))
					{
						short shBufferOut[320];
						//如果降噪成功，则根据降噪后高频和低频数据传入滤波接口，然后用将返回的数据写入文件
						WebRtcSpl_SynthesisQMF(shOutL, shOutH, 160, shBufferOut, Synthesis_state1, Synthesis_state12);
						memcpy(pOutBuffer + j, shBufferOut, len);
					}
				}
			}
			fwrite(pOutBuffer, sizeof(char), readFileSize, fpOut);
		}
		nTime = GetTickCount() - nTime;
		printf("n_s user time=%dms\n", nTime);
		
	} while (0);

	WebRtcNsx_Free(pNS_inst);
	fclose(fpIn);
	fclose(fpOut);
	free(pInBuffer);
	free(pOutBuffer);
	return bStatus;
}
bool  CNoiseSuppression::NoiseSuppressionProcessNo32K(char *szFileIn, char *szFileOut, int nSample, int nMode)
{
	bool bStatus = true;
	int nRet = 0;
	NsHandle *pNS_inst = NULL;

	FILE *fpIn = NULL;
	FILE *fpOut = NULL;

	char *pInBuffer = NULL;
	char *pOutBuffer = NULL;

	int frameSize = nSample / 100;
	int len = frameSize*sizeof(short);
	short* shInL = new short[frameSize];
	short* shOutL = new short[frameSize];

	//------------设置分段文件数据读取------------------//
	int sec = 100;
	int readFileSize = sec * nSample * 2;
	pInBuffer = new char[readFileSize];
	pOutBuffer = new char[readFileSize]; 
   //---------------------------------------------------
	 
	do
	{
		int i = 0;
		int nFileSize = 0;
		int nTime = 0;
		if (0 != WebRtcNs_Create(&pNS_inst))
		{
			printf("Noise_SuppressionNo32K WebRtcNs_Create err! \n");
			bStatus = false;
			break;
		}

		if (0 != WebRtcNs_Init(pNS_inst, nSample))
		{
			printf("Noise_SuppressionNo32K WebRtcNs_Init err! \n");
			bStatus = false;
			break;
		}

		if (0 != WebRtcNs_set_policy(pNS_inst, nMode))
		{
			printf("Noise_SuppressionNo32K WebRtcNs_set_policy err! \n");
			bStatus = false;
			break;
		}

		fpIn = fopen(szFileIn, "rb");
		if (NULL == fpIn)
		{
			printf("open src file err \n");
			bStatus = false;
			break;
		}
		fpOut = fopen(szFileOut, "wb");
		if (NULL == fpOut)
		{
			printf("open out file err! \n");
			bStatus = false;
			break;
		}
		fseek(fpIn, 0, SEEK_END);
		nFileSize = ftell(fpIn);
		fseek(fpIn, 0, SEEK_SET);

		int  filter_state1[6], filter_state12[6];
		int  Synthesis_state1[6], Synthesis_state12[6];

		memset(filter_state1, 0, sizeof(filter_state1));
		memset(filter_state12, 0, sizeof(filter_state12));
		memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
		memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

		for (i = 0; i < nFileSize; i += readFileSize)
		{		
			readFileSize = fread(pInBuffer, sizeof(char), readFileSize, fpIn);
	
			for (int j = 0; j < readFileSize; j += len)
			{
				if (readFileSize - j >= len)
				{
					memcpy(shInL, (char*)(pInBuffer + j), len);

					if (0 == WebRtcNs_Process(pNS_inst, shInL, NULL, shOutL, NULL))
					{
						//如果降噪成功，则根据降噪后高频和低频数据传入滤波接口，然后用将返回的数据写入文件

						memcpy(pOutBuffer + j, shOutL, len);

					}
				}
			}
			fwrite(pOutBuffer, sizeof(char), readFileSize, fpOut);
		}
		
		nTime = GetTickCount() - nTime;
		printf("n_s user time=%dms\n", nTime);
	} while (0);
	WebRtcNs_Free(pNS_inst);
	fclose(fpIn);
	fclose(fpOut);
	free(shInL);
	free(pInBuffer);
	free(pOutBuffer);
	return bStatus;
}
bool CNoiseSuppression::NoiseSuppressionProcessNoX32K(char *szFileIn, char *szFileOut, int nSample, int nMode)
{
	bool bStatus = true;
	int nRet = 0;
	NsxHandle *pNS_inst = NULL;

	FILE *fpIn = NULL;
	FILE *fpOut = NULL;

	char *pInBuffer = NULL;
	char *pOutBuffer = NULL;

	int frameSize = nSample / 100;
	int len = frameSize*sizeof(short);
	short* shInL = new short[frameSize];
	short* shOutL = new short[frameSize];

	//------------设置分段文件数据读取------------------//
	int sec = 100;
	int readFileSize = sec * nSample * 2;
	pInBuffer = new char[readFileSize];
	pOutBuffer = new char[readFileSize];
	//---------------------------------------------------

	do
	{
		int i = 0;
		int nFileSize = 0;
		int nTime = 0;
		if (0 != WebRtcNsx_Create(&pNS_inst))
		{
			printf("Noise_Suppression WebRtcNs_Create err! \n");
			bStatus = false;
			break;
		}

		if (0 != WebRtcNsx_Init(pNS_inst, nSample))
		{
			printf("Noise_Suppression WebRtcNs_Init err! \n");
			bStatus = false;
			break;
		}

		if (0 != WebRtcNsx_set_policy(pNS_inst, nMode))
		{
			printf("Noise_Suppression WebRtcNs_set_policy err! \n");
			bStatus = false;
			break;
		}

		fpIn = fopen(szFileIn, "rb");
		if (NULL == fpIn)
		{
			printf("open src file err \n");
			bStatus = false;
			break;
		}
		fpOut = fopen(szFileOut, "wb");
		if (NULL == fpOut)
		{
			printf("open out file err! \n");
			bStatus = false;
			break;
		}
		fseek(fpIn, 0, SEEK_END);
		nFileSize = ftell(fpIn);
		fseek(fpIn, 0, SEEK_SET);

		int  filter_state1[6], filter_state12[6];
		int  Synthesis_state1[6], Synthesis_state12[6];

		memset(filter_state1, 0, sizeof(filter_state1));
		memset(filter_state12, 0, sizeof(filter_state12));
		memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
		memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

		for (i = 0; i < nFileSize; i += readFileSize)
		{

			readFileSize = fread(pInBuffer, sizeof(char), readFileSize, fpIn);

			for (int j = 0; j < readFileSize; j += len)
			{
				if (readFileSize - j >= len)
				{
					memcpy(shInL, (char*)(pInBuffer + j), len);

					if (0 == WebRtcNsx_Process(pNS_inst, shInL, NULL, shOutL, NULL))
					{
						//如果降噪成功，则根据降噪后高频和低频数据传入滤波接口，然后用将返回的数据写入文件

						memcpy(pOutBuffer + j, shOutL, len);

					}
				}
			}
			fwrite(pOutBuffer, sizeof(char), readFileSize, fpOut);
		}

		nTime = GetTickCount() - nTime;
		printf("n_s user time=%dms\n", nTime);
	} while (0);
	WebRtcNsx_Free(pNS_inst);
	fclose(fpIn);
	fclose(fpOut);
	free(shInL);
	free(pInBuffer);
	free(pOutBuffer);
	return bStatus;
}
