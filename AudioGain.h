#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#include "common/WebRtcMoudle/signal_processing_library.h"
#include "common/WebRtcMoudle/noise_suppression_x.h"
#include "common/WebRtcMoudle/noise_suppression.h"
#include "common/WebRtcMoudle/gain_control.h"
enum AgcMode { AgcModeAdaptiveAnalog ,           //����ģ���������ڵĹ���
	AgcModeAdaptiveDigital,              //�ɱ�����agc�����ǲ�����ϵͳ����
	AgcModeFixedDigital                //�̶������agc
};
class CAudioGain
{
public:
	CAudioGain();
	~CAudioGain();
public:
	bool AudioFileGain(char *filename, char *outfilename, int fs, int gainStrength = 20);
	bool AudioFileGain32K(char *filename, char *outfilename, int fs, int gainStrength = 20);
	bool AudioFileGainNo32K(char *filename, char *outfilename, int fs, int gainStrength = 20);
	void SetGainStr(int gainstr);
	void SetAgcMode(AgcMode agcmode);
private:
	int m_nGainStr;
	AgcMode m_agcMode;
};

