#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <xbmc_vis_dll.h>
#include <xbmc_vis_types.h>
#include "p8-platform/threads/mutex.h"


extern "C" 
{
  #define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif
  #include <string.h>

  #include "hypnomix.h"  
  #include "dir.h"
  #include "fft.h"
  #include "math.h"
}


static struct hypnomix hyp;
static int hyprunning = 0;
static int pindex = 0;

static P8PLATFORM::CMutex mut;
static bool isLocked = false;


void printLog(int type, const char *fmt, ...)
{
  va_list vl;

  switch(type) {
    case HYP_ERROR:
      printf("[ERROR] ");
      break;
    case HYP_WARNING:
      printf("[WARNING] ");
      break;
    case HYP_INFO:
      printf("[INFO] ");
      break;
    case HYP_DEBUG:
      printf("[DEBUG] ");
      break;
    case HYP_SUCCESS:
      printf("[SUCCESS] ");
      break;
  } 
  
  va_start(vl, fmt);
  vprintf(fmt, vl);
//  CLog::Log(LOGDEBUG, "HYPNOMIX - home: %s", hyp.home);
  va_end(vl);
  
}

extern "C" ADDON_STATUS ADDON_Create(void *hdl, void *props)
{
  P8PLATFORM::CLockObject lock(mut);
  if(!props) {
    return ADDON_STATUS_UNKNOWN;
  }

  VIS_PROPS *visProps = (VIS_PROPS *)props;

  if(hyprunning) {
    return ADDON_STATUS_OK;
  }

  hypnomixRegisterLogFunc(&hyp, printLog);

  hyp.home = (char *)malloc(strlen(visProps->presets)+strlen("/resources/")+1);
  strcpy(hyp.home, visProps->presets);  
  strcat(hyp.home, "/resources/");
  (*hyp.log)(HYP_DEBUG, "home: %s\n", hyp.home);
  hyp.x = visProps->x;
  hyp.y = visProps->y;
  hyp.w = visProps->width;
  hyp.h = visProps->height;
  hyp.nbsamples = 512; /* FIXME: use the real XBMC value */   
  hyp.samples = (float *)malloc(sizeof(float)*hyp.nbsamples);

  hypnomixInit(&hyp);
  hyprunning = 1;
  (*hyp.log)(HYP_DEBUG, "**** XBMC ADDON_Create() ****\n");

  return ADDON_STATUS_OK;
}


extern "C" void Render()
{
  P8PLATFORM::CLockObject lock(mut);
  GLuint prgxbmc;

  glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&prgxbmc);
  glPushMatrix();

  hypnomixDraw(&hyp);

  glPopMatrix();
  glUseProgram(prgxbmc); 
}



extern "C" void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, 
  const char *szSongName)
{
  P8PLATFORM::CLockObject lock(mut);
  if(hyp.songname != NULL) {
    free(hyp.songname);
  }
  hyp.songname = (char *)malloc(strlen(szSongName)+1);
  strcpy(hyp.songname, szSongName); 
  hypnomixLoadPreset(&hyp, pindex);
  
  (*hyp.log)(HYP_DEBUG, "**** XBMC ADDON_Start() ****\n");
}


extern "C" void AudioData(const float *pAudioData, int iAudioDataLength, 
  float *pFreqData, int iFreqDataLength)
{
  P8PLATFORM::CLockObject lock(mut);
  if (!hyprunning)
    return;
  float fftdata[512*2]; /* FIXME: make this dynamic */
  float magn;
  int i;

  for(i = 0; i < std::min(iAudioDataLength,512); i++) {  
    fftdata[i*2] = pAudioData[i];
    fftdata[i*2+1] = 0.0;
  }
  fft(fftdata, std::min(iAudioDataLength,512));

  hyp.nbsamples = std::min(iAudioDataLength,512) / 2; /* fft is symetric, so we only take half */
  for(i = 0; i < hyp.nbsamples; i++) {
    magn = sqrt(fftdata[i*2]*fftdata[i*2]+fftdata[i*2+1]*fftdata[i*2+1]);
    magn /= 256.0;
    magn = 20.0*log(magn);
    if(magn == -INFINITY) {
      magn = -256.0;
    }
    magn += 256.0;
    hyp.samples[i] = magn / 256.0; /* magn is in [0.0, 1.0] */
  }
  hypnomixMorphSamples(&hyp);
  hypnomixComputeVariations(&hyp);
}


extern "C" void GetInfo(VIS_INFO *pInfo)
{
  pInfo->bWantsFreq = false;
  pInfo->iSyncDelay = 0;
}


extern "C" unsigned int GetSubModules(char ***names)
{
  return 0; /* 0 sub modules */
}


extern "C" bool OnAction(long flags, const void *param)
{
  bool ret = false;
  P8PLATFORM::CLockObject lock(mut);

  if(flags == VIS_ACTION_LOAD_PRESET && param) {
    pindex = *((int *)param);
    hypnomixLoadPreset(&hyp, pindex);
    ret = true;
  }
/*
  if(flags == VIS_ACTION_UPDATE_ALBUMART) {
    printf("ALBUMART=%s", param);
    ret = true;
unknownpleasureStartFade(FADE_IN);  
  }
*/
  return ret;
}


extern "C" unsigned int GetPresets(char ***presets)
{
  int i = 0;
  struct list *li = hyp.prlist.li;

  char **listpreset = (char **)malloc(sizeof(char *) * hyp.prlist.nb);
  //CLog::Log(LOGDEBUG, "HYPNOMIX - GetPresets()");
  while(li != NULL) {
    listpreset[i] = (char *)malloc(strlen(li->filename)+1);
    strcpy(listpreset[i++], li->filename);
//CLog::Log(LOGDEBUG, "HYPNOMIX - PRESETNAME:", li->filename);
    li = li->prev;
  }  
  
  *presets = listpreset;

  return i;
}


extern "C" unsigned int GetPreset()
{
  /* return numpreset; */
  return pindex;
}


extern "C" bool IsLocked()
{
  return isLocked;
}


extern "C" void ADDON_Stop()
{
//  (*hyp.log)(HYP_DEBUG, "**** XBMC ADDON_Stop() ****\n");
  P8PLATFORM::CLockObject lock(mut);
  free(hyp.samples);
  free(hyp.songname);
  free(hyp.home);
  hypnomixDestroy(&hyp);
  hyp.songname = nullptr;
  hyprunning = 0;
}


extern "C" void ADDON_Destroy()
{
//  (*hyp.log)(HYP_DEBUG, "**** XBMC ADDON_Destroy() ****\n");
}


extern "C" bool ADDON_HasSettings()
{
  return false;
}


extern "C" ADDON_STATUS ADDON_GetStatus()
{
  (*hyp.log)(HYP_DEBUG, "**** XBMC ADDON_GetStatus() ****\n");
  return ADDON_STATUS_OK;
}


extern "C" unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
// unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  return 0;
}


extern "C" void ADDON_FreeSettings()
{
}


extern "C" ADDON_STATUS ADDON_SetSetting(const char *id, const void *value)
{
  if (strcmp(id, "###GetSavedSettings") == 0) // We have some settings to be saved in the settings.xml file
  {
    if (strcmp((char*)value, "1") == 0)
    {
      strcpy((char*)id, "lastlockedstatus");
      strcpy((char*)value, isLocked ? "true" : "false");
    }
    if (strcmp((char*)value, "2") == 0)
    {
      strcpy((char*)id, "lastpresetidx");
      sprintf((char*)value,"%i",pindex);
    }

   return ADDON_STATUS_OK;
  }

  if (strcmp(id, "lastpresetidx") == 0)
    pindex = *(int*)value;
  else if (strcmp(id, "lastlockedstatus") == 0)
    isLocked = *(bool*)value;

  return ADDON_STATUS_OK;
}


extern "C" void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
}
