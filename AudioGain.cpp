#include "AudioGain.h"
#pragma warning( disable : 4996)
CAudioGain::CAudioGain()
{
	m_nGainStr = 20;
	m_agcMode = AgcModeFixedDigital;
}
CAudioGain::~CAudioGain()
{
}
//-----------------------对音频文件进行增益------------------------//
bool CAudioGain::AudioFileGain(char *filename, char *outfilename, int fs, int gainStrength)
{
	if (fs == 32000)
		return AudioFileGain32K(filename, outfilename, fs, gainStrength);
	if (fs == 16000 || fs ==8000)
		return AudioFileGainNo32K(filename, outfilename, fs, gainStrength);
	return false;
	
}
//------------------------32k采样处理32K采样率需要采用滤波分频-----------------//
bool CAudioGain::AudioFileGain32K(char *filename, char *outfilename, int fs, int gainStrength)
{
	
	bool status = true;

	FILE *infp = NULL;
	FILE *outfp = NULL;

	short *pData = NULL;
	short *pOutData = NULL;
	void *agcHandle = NULL;

	do
	{
		WebRtcAgc_Create(&agcHandle);

		int minLevel = 0;
		int maxLevel = 255;
		int agcMode = m_agcMode;
		WebRtcAgc_Init(agcHandle, minLevel, maxLevel, agcMode, fs);

		WebRtcAgc_config_t agcConfig;
		agcConfig.compressionGaindB = m_nGainStr;
		agcConfig.limiterEnable = 1;
		agcConfig.targetLevelDbfs = 3;
		WebRtcAgc_set_config(agcHandle, agcConfig);

		infp = fopen(filename, "rb");
		if (!infp)
			return false;
		int frameSize = fs / 100;
		pData = new short[frameSize];
		pOutData = new short[frameSize];

		outfp = fopen(outfilename, "wb");
		if (!outfp)
			return false;
		int len = frameSize*sizeof(short);
		int micLevelIn = 0;
		int micLevelOut = 0;
		//设置状态位----------
		int  filter_state1[6], filter_state12[6];
		int  Synthesis_state1[6], Synthesis_state12[6];
		memset(filter_state1, 0, sizeof(filter_state1));
		memset(filter_state12, 0, sizeof(filter_state12));
		memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
		memset(Synthesis_state12, 0, sizeof(Synthesis_state12));


		short* shOutL = new short[160], *shOutH = new short[160];
		short* shInL = new short[160], *shInH = new short[160];
		//--------------
		while (TRUE)
		{
			memset(pData, 0, len);
			len = fread(pData, 1, len, infp);
			int inMicLevel = micLevelOut;
			int outMicLevel = 0;
			if (len>= 640)
			{
				
				
				//首先需要使用滤波函数将音频数据分高低频，以高频和低频的方式传入降噪函数内部
				WebRtcSpl_AnalysisQMF(pData, 320, shInL, shInH, filter_state1, filter_state12);
				uint8_t saturationWarning;
				int nAgcRet = WebRtcAgc_Process(agcHandle, shInL, shOutH, frameSize, shOutL, shOutH, inMicLevel, &outMicLevel, 0, &saturationWarning);
				if (nAgcRet != 0)
				{
					printf("failed in WebRtcAgc_Process\n");
					status = false;
					break;
				}
				micLevelIn = outMicLevel;
				WebRtcSpl_SynthesisQMF(shOutL, shOutH, 160, pOutData, Synthesis_state1, Synthesis_state12);
				fwrite(pOutData, 1, len, outfp);
			}
			else
			{
				break;
			}
		}
		free(shOutL);
		free(shOutH);
		free(shInL);
		free(shInH);

	} while (0);

	fclose(infp);
	fclose(outfp);
	free(pData);
	free(pOutData);
	WebRtcAgc_Free(agcHandle);
	return status;
}
//----------------------非32k采样率------------------------------------------//
bool CAudioGain::AudioFileGainNo32K(char *filename, char *outfilename, int fs, int gainStrength)
{
	bool status = true;
	
	FILE *infp = NULL;
	FILE *outfp = NULL;

	short *pData = NULL;
	short *pOutData = NULL;
	void *agcHandle = NULL;

	do
	{
		WebRtcAgc_Create(&agcHandle);

		int minLevel = 0;
		int maxLevel = 255;
		int agcMode =m_agcMode;
		WebRtcAgc_Init(agcHandle, minLevel, maxLevel, agcMode, fs);

		WebRtcAgc_config_t agcConfig;
		agcConfig.compressionGaindB = m_nGainStr;
		agcConfig.limiterEnable = 1;
		agcConfig.targetLevelDbfs = 3;
		WebRtcAgc_set_config(agcHandle, agcConfig);

		infp = fopen(filename, "rb");
		if (!infp)
			return false;
		int frameSize = fs/100;
		pData = new short[frameSize];
		pOutData = new short[frameSize];

		outfp = fopen(outfilename, "wb");
		if (!outfp)
			return false;
		int len = frameSize*sizeof(short);
		int micLevelIn = 0;
		int micLevelOut = 0;
		while (TRUE)
		{
			memset(pData, 0, len);
			len = fread(pData, 1, len, infp);
			if (len > 0)
			{
				int inMicLevel = micLevelOut;
				int outMicLevel = 0;
				uint8_t saturationWarning;
				int nAgcRet = WebRtcAgc_Process(agcHandle, pData, NULL, frameSize, pOutData, NULL, inMicLevel, &outMicLevel, 0, &saturationWarning);
				if (nAgcRet != 0)
				{
					printf("failed in WebRtcAgc_Process\n");
					status = false;
					break;
				}
				micLevelIn = outMicLevel;
				fwrite(pOutData, 1, len, outfp);
			}
			else
			{
				break;
			}
		}
	} while (0);

	fclose(infp);
	fclose(outfp);
	free(pData);
	free(pOutData);
	WebRtcAgc_Free(agcHandle);
	return status;
}
void CAudioGain::SetGainStr(int gainstr)
{
	m_nGainStr = gainstr;
}
void CAudioGain::SetAgcMode(AgcMode agcmode)
{
	m_agcMode = agcmode;
}
