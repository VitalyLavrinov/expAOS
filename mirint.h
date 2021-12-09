#ifndef mirintH
#define mirintH
#include "loadinter.h"

#include "mirint_s.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct MIRINTTAG {
  INTERFACE_HEADER Header;
  int *NChannels, *NGroups;
  double **MaxVol, **MinVol, **CurrVol, *EarthVol, ***EWeights, **AddVol, **GVol;
  int **ChainStatus, *EarthStatus;
  int *IsBoard;
  int *HV;
  int *IsError;
  void (__fastcall *MirrorControlShow)(void);
  void (__fastcall *MirrorControlHide)(void);
  int (__fastcall *MirrorSetVoltage)(int i, double v, bool bd);
  void (__fastcall *MirrorSetAllVoltage)(double v, bool bd);
  bool (__fastcall *MirrorSetHighVoltage)(bool b);
  long (__fastcall *MirrorSetDelay)(long d);
  void (__fastcall *MirrorDegauss)(double v);
  void (__fastcall *MirrorAddChannelToGroup)(int en, int gn, bool sv);
  void (__fastcall *MirrorSetGroupVoltage)(int gn, double v, bool bd);
  int (__fastcall *MirrorEnumGroups)(void);
  int  (__fastcall *MirrorFirstElecInGroup)(int gn);
  double  (__fastcall *MirrorGetGroupVoltage)(int gn);
  void (__fastcall *MirrorSetSynchr)(void (__fastcall *f)(void));
  void (__fastcall *MirrorSetEarth)(double v);
  int (__fastcall *MirrorCheckError)(void);
  void (__fastcall *MirrorReset)(void);
  void (__fastcall *MirrorSwitch)(bool b);
} MIRROR_INTERFACE;

typedef struct MIRINTTAG2: public MIRROR_INTERFACE {
  int *SleepMode;
  void (__fastcall *MirrorSetSleep)(unsigned short sd);   // in sec, 0 means sleep mode off
  void (__fastcall *MirrorGroupShow)(void);
} MIRROR_INTERFACE2;

typedef struct MIRINTTAG_S {
  INTERFACE_HEADER Header;
  int *NChannels, *NGroups;
  double **MaxVol, **MinVol, **CurrVol, *EarthVol, ***EWeights, **AddVol, **GVol;
  int **ChainStatus, *EarthStatus;
  int *IsBoard;
  int *HV;
  int *IsError;
  int Dummy;

  void (__stdcall *MirrorControlShow)(void);
  void (__stdcall *MirrorControlHide)(void);
  int (__stdcall *MirrorSetVoltage)(int i, double v, bool bd);
  void (__stdcall *MirrorSetAllVoltage)(double v, bool bd);
  bool (__stdcall *MirrorSetHighVoltage)(bool b);
  long (__stdcall *MirrorSetDelay)(long d);
  void (__stdcall *MirrorDegauss)(double v);
  void (__stdcall *MirrorAddChannelToGroup)(int en, int gn, bool sv);
  void (__stdcall *MirrorSetGroupVoltage)(int gn, double v, bool bd);
  int (__stdcall *MirrorEnumGroups)(void);
  int  (__stdcall *MirrorFirstElecInGroup)(int gn);
  double  (__stdcall *MirrorGetGroupVoltage)(int gn);
  void (__stdcall *MirrorSetSynchr)(void (__stdcall *f)(void));
  void (__stdcall *MirrorSetEarth)(double v);
  int (__stdcall *MirrorCheckError)(void);
  void (__stdcall *MirrorReset)(void);
  void (__stdcall *MirrorSwitch)(bool b);
} MIRROR_INTERFACE_S;

typedef struct MIRINTTAG2_S: public MIRROR_INTERFACE_S {
  int *SleepMode;
  void (__stdcall *MirrorSetSleep)(unsigned short sd);   // in sec, 0 means sleep mode is off
  void (__stdcall *MirrorGroupShow)(void);
} MIRROR_INTERFACE2_S;

typedef struct MIRINTTAG3_S: public MIRROR_INTERFACE2_S {
  int *InterpMcs;
  void (__stdcall *MirrorSetInterp)(unsigned short mcs);   // in micro sec, 0 means interp mode is off
  unsigned (__stdcall *MirrorCheckDevNum)(int ndev); // check device number, -1 means no matter
} MIRROR_INTERFACE3_S;

typedef struct MIRINTTAG4_S: public MIRROR_INTERFACE3_S {
  void (__stdcall *MirrorSetAnalogue)(bool b);
  unsigned long *MirrorStatus;
} MIRROR_INTERFACE4_S;

typedef struct MIRINTTAG5_S: public MIRROR_INTERFACE4_S {
  double **GLimit;
} MIRROR_INTERFACE5_S;

typedef struct MIRINTTAG6_S: public MIRROR_INTERFACE5_S {
  int *PW;
  bool (__stdcall *MirrorSetPower)(bool b);
} MIRROR_INTERFACE6_S;

typedef struct MIRINTTAG7_S: public MIRROR_INTERFACE6_S {
  void (__stdcall *SetWFCconfigHook)(WFCCH f, void *);
  void (__stdcall *MirrorConfigShow)(int);
  void (__stdcall *MirrorConfigSelect)(int chn, int grn);
} MIRROR_INTERFACE7_S;

typedef struct MIRINTTAG8_3S: public MIRROR_INTERFACE7_S {
  void *pData;

  void (__stdcall *MirrorControlShow3)(void *);
  void (__stdcall *MirrorControlHide3)(void *);
  int (__stdcall *MirrorSetVoltage3)(void *, int i, double v, bool bd);
  void (__stdcall *MirrorSetAllVoltage3)(void *, double v, bool bd);
  bool (__stdcall *MirrorSetHighVoltage3)(void *, bool b);
  long (__stdcall *MirrorSetDelay3)(void *, long d);
  void (__stdcall *MirrorDegauss3)(void *, double v);
  void (__stdcall *MirrorAddChannelToGroup3)(void *, int en, int gn, bool sv);
  void (__stdcall *MirrorSetGroupVoltage3)(void *, int gn, double v, bool bd);
  int (__stdcall *MirrorEnumGroups3)(void *);
  int  (__stdcall *MirrorFirstElecInGroup3)(void *, int gn);
  double  (__stdcall *MirrorGetGroupVoltage3)(void *, int gn);
  void (__stdcall *MirrorSetSynchr3)(void *, void (__stdcall *f)(void*));
  void (__stdcall *MirrorSetEarth3)(void *, double v);
  int (__stdcall *MirrorCheckError3)(void *);
  void (__stdcall *MirrorReset3)(void*);
  void (__stdcall *MirrorSwitch3)(void *, bool b);
  void (__stdcall *MirrorSetSleep3)(void *, unsigned short sd);   // in sec, 0 means sleep mode is off
  void (__stdcall *MirrorGroupShow3)(void*);
  void (__stdcall *MirrorSetInterp3)(void*, unsigned short mcs);   // in micro sec, 0 means interp mode is off
  unsigned (__stdcall *MirrorCheckDevNum3)(void*, int ndev); // check device number, -1 means no matter
  void (__stdcall *MirrorSetAnalogue3)(void*, bool b);
  bool (__stdcall *MirrorSetPower3)(void*,bool b);
  void (__stdcall *SetWFCconfigHook3)(void*, WFCCH3 f, void *);
  void (__stdcall *MirrorConfigShow3)(void*,int);
  void (__stdcall *MirrorConfigSelect3)(void*,int chn, int grn);
  void (__stdcall *MirrorControlClose3)(void *);
} MIRROR_INTERFACE8_3S;

typedef MIRROR_INTERFACE7_S MIRROR_INTERFACE_LAST;

#ifdef __cplusplus
}
#endif

#endif
