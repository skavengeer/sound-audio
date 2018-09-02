//---------------------------------------------------------------------------

#include <vcl.h>
#include <mmsystem.h>
#include <mmeapi.h>
#include <stdlib.h>     /* div, div_t */
#include <math.h>
#include <cmath>
#include <fstream.h>

#include <iostream.h>
#include <Windows.h>
#include <vector>

 #pragma comment(lib, "winmm.lib")

#pragma hdrstop

#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//int TVolumeLevel[177];

TForm1 *Form1;

using namespace std;

double trunc(double d){ return (d>0) ? floor(d) : ceil(d) ; };


//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner) {

}
//---------------------------------------------------------------------------
__fastcall TNoise::~TNoise(void){

 FStream->Free();

}
//---------------------------------------------------------------------------
inline TNoise::TNoise(int Frequency,int Duration,int Volume ) {

  WAVEFORMATEX pFormat;

  int i, TempInt, DataCount, RiffCount;
  Byte SoundValue;
  double w;
  div_t divresult;

  const Word Mono         = 0x0001;
  const int SampleRate    = 11025; // 8000, 11025, 22050, or 44100
  const AnsiString RiffId = "RIFF";
  const AnsiString WaveId = "WAVE";
  const AnsiString FmtId  = "fmt ";
  const AnsiString DataId = "data";

   if (Frequency > (0.6 * SampleRate)) {

	 ShowMessage(Format("Sample rate of %d is too Low to play a tone of %dHz", ARRAYOFCONST(( SampleRate, Frequency))));

	 // exit
   }

  pFormat.wFormatTag      = WAVE_FORMAT_PCM;
  pFormat.nChannels       = Mono;
  pFormat.nSamplesPerSec  = SampleRate;
  pFormat.wBitsPerSample  = 0x0008;
  divresult = div((pFormat.nChannels * pFormat.wBitsPerSample),8);
  pFormat.nBlockAlign     = divresult.quot;
  pFormat.nAvgBytesPerSec = pFormat.nSamplesPerSec * pFormat.nBlockAlign;
  pFormat.cbSize          = 0;

  divresult = div((Duration * SampleRate),1000);
 
	//write out the wave header

	 FStream = new TMemoryStream();

	 DataCount = divresult.quot ;
	 RiffCount =  WaveId.Length()+FmtId.Length() + sizeof(DWORD)+
			 sizeof (WAVEFORMATEX) + DataId.Length()+ sizeof(DWORD) + DataCount  ;  // file data

	 FStream->Write(RiffId.c_str(),4);                          // 'RIFF'
	 FStream->Write((char*)&RiffCount,sizeof(DWORD));              // file data size
	 FStream->Write(WaveId.c_str(),WaveId.Length());            // 'WAVE'
	 FStream->Write(FmtId.c_str(),FmtId.Length());              // 'fmt '
	 TempInt = sizeof(WAVEFORMATEX);

	 FStream->Write((char*)&TempInt,sizeof(DWORD));                 // TWaveFormat data size
	 FStream->Write((char*)&pFormat,sizeof(WAVEFORMATEX));    // WaveFormatEx record
	 FStream->Write(DataId.c_str(),DataId.Length());            // 'data'
	 FStream->Write((char*)&DataCount,sizeof(DWORD));             // sound data size

		 // calculate and write out the tone signal     // now the data values
	 w = 2 * M_PI * Frequency;                            // omega

	for (int i = 0; i < DataCount - 1; i++) {

		SoundValue = 127 + trunc(Volume * sin(i * w / SampleRate)) ;     // wt = w * i / SampleRate
		FStream->Write((char*)&SoundValue, sizeof(Byte));      //
	}


}

//---------------------------------------------------------------------------
void __fastcall TNoise::Play(){

  //sndPlaySound((wchar_t*)MS->Memory , SND_MEMORY || SND_SYNC); || SND_SYNC
   PlaySound((wchar_t*)FStream->Memory ,0, SND_MEMORY );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{

   TNoise *myBeep = new TNoise(3000,300,100);

   myBeep->Play();

}
//---------------------------------------------------------------------------

