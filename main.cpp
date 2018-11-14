#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <time.h>
#include "AudioGain.h"
#include "common/common/WavFile.h"
#include "NoiseSuppression.h"
#include<fstream>
#include <windows.h>
#include <mmsystem.h>            
#pragma comment(lib, "winmm.lib")
#ifdef _WIN32_WCE
#include <windows.h>
#define main _tmain
time_t time(time_t *inTT) {
	SYSTEMTIME sysTimeStruct;
	FILETIME fTime;
	ULARGE_INTEGER int64time;
	time_t locTT = 0;

	if (inTT == NULL) {
		inTT = &locTT;
	}

	GetSystemTime(&sysTimeStruct);
	if (SystemTimeToFileTime(&sysTimeStruct, &fTime)) {
		memcpy(&int64time, &fTime, sizeof(FILETIME));
		/* Subtract the value for 1970-01-01 00:00 (UTC) */
		int64time.QuadPart -= 0x19db1ded53e8000;
		/* Convert to seconds. */
		int64time.QuadPart /= 10000000;
		*inTT = int64time.QuadPart;
	}

	return *inTT;
}
#endif
int main(int argc, char **argv)
{
	//test();
	int samples = 16000;
	WavFile wf;
	wf.OpenWavFile("qq.wav");
	wf.SavePCMData("qq.pcm");
	CAudioGain cag;
	cag.AudioFileGain("qq.pcm", "qq1.pcm", samples);
	std::cout << WavFile::PCMTranceWav("qq1.pcm", 1, 16, 16000, "out.wav");
	CNoiseSuppression cns;
	cns.NoiseSuppressionProcessT("qq1.pcm", "qq2.pcm", 16000, 1,false);
	std::cout << WavFile::PCMTranceWav("qq2.pcm", 1, 16,16000, "qq2K.wav");
	//std::cout << WavFile::PCMTranceWav("speaker.pcm", 1, 16, 8000, "speaker.wav");
	//std::cout<<WavFile::PCMTranceWav("micin.pcm", 1, 16, 8000, "micin.wav");
	//std::cout << WavFile::PCMTranceWav("out.pcm", 1, 16, 8000, "out.wav");
	printf("³ÌÐò½áÊø");
	getchar();
	return 0;
}

