// Mixer.h: interface for the CMixer class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_MIXER_H__7369BAA5_012E_467E_AD1E_A383E4474C58__INCLUDED_)
#define AFX_MIXER_H__7369BAA5_012E_467E_AD1E_A383E4474C58__INCLUDED_
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include "windows.h"
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CMixer
{
public:
   CMixer();
   CMixer(const int VolRange);
   virtual ~CMixer();
   enum MixerDeice{
      SPEAKERS = 0,
      WAVEOUT,
      SYNTHESIZER,
      MICROPHONE
   };
   bool GetMute(MixerDeice dev);   //检查设备是否静音
   bool SetMute(MixerDeice dev, bool vol);     //设置设备静音
   bool SetVolume(MixerDeice dev, long vol);   //设置设备的音量
   unsigned GetVolume(MixerDeice dev);    //得到设备的音量dev=0主音量，1WAVE ,2MIDI ,3 LINE IN
private:
   bool GetVolumeControl(HMIXER hmixer, long componentType, long ctrlType, MIXERCONTROL* mxc);
   bool SetVolumeValue(HMIXER hmixer, MIXERCONTROL *mxc, long volume);
   bool SetMuteValue(HMIXER hmixer, MIXERCONTROL *mxc, bool mute);
   unsigned GetVolumeValue(HMIXER hmixer, MIXERCONTROL *mxc);
   long GetMuteValue(HMIXER hmixer, MIXERCONTROL *mxc);

   long m_VolRange;
};
#endif // !defined(AFX_MIXER_H__7369BAA5_012E_467E_AD1E_A383E4474C58__INCLUDED_)