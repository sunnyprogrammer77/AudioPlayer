#pragma once
#pragma warning( disable : 4996)
//----------------------˵��--------------------

//���ݱ��뱣֤��8000,16000,32000Ƶ�ʵ�������PCM����

//----------------------------------------------
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "common/WebRtcMoudle/signal_processing_library.h"
#include "common/WebRtcMoudle/noise_suppression_x.h"
#include "common/WebRtcMoudle/noise_suppression.h"
#include "common/WebRtcMoudle/gain_control.h"
class CNoiseSuppression
{
public:
	CNoiseSuppression();
	~CNoiseSuppression();
public:
	bool NoiseSuppressionProcessT(char *szFileIn, char *szFileOut, int nSample, int nMode, bool nStyle);
	bool NoiseSuppressionProcess(char *szFileIn, char *szFileOut, int nSample, int nMode);
	bool NoiseSuppressionProcessX(char *szFileIn, char *szFileOut, int nSample, int nMode);
	bool NoiseSuppressionProcess32K(char *szFileIn, char *szFileOut, int nSample, int nMode);
	bool NoiseSuppressionProcessX32(char *szFileIn, char *szFileOut, int nSample, int nMode);
	bool NoiseSuppressionProcessNo32K(char *szFileIn, char *szFileOut, int nSample, int nMode);
	bool NoiseSuppressionProcessNoX32K(char *szFileIn, char *szFileOut, int nSample, int nMode);
private:

};

