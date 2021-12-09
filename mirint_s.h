#ifndef mirint_sH
#define mirint_sH

#ifdef __cplusplus
extern "C" {
#endif

#define MRS_SLEEP		0x08
#define MRS_OVERHV		0x20
#define MRS_NOCONN		0x40
#define MRS_EMERGENCY		0x0200
#define MRS_ANALOGUE		0x10000	// 1 - analogue control on
#define MRS_ANALOGUE_INPUT	0x1000000	// 1 - analogue input present

// Labview interface

void __declspec(dllexport) __stdcall sMirrorControlShow(void);
void __declspec(dllexport) __stdcall sMirrorControlHide(void);
int __declspec(dllexport) __stdcall sMirrorSetVoltage(int i, double v, bool bd);
void __declspec(dllexport) __stdcall  sMirrorSetAllVoltage(double v, bool bd);
bool __declspec(dllexport) __stdcall  sMirrorSetHighVoltage(bool b);
void __declspec(dllexport) __stdcall  sMirrorDegauss(double v);
void __declspec(dllexport) __stdcall sMirrorSetGroupVoltage(int gn, double v, bool bd); // gn from 1
int __declspec(dllexport) __stdcall sMirrorEnumGroups(void);
int  __declspec(dllexport) __stdcall sMirrorFirstElecInGroup(int gn);  // gn from 1
double  __declspec(dllexport) __stdcall sMirrorGetGroupVoltage(int gn);   // gn from 1
int __declspec(dllexport) __stdcall sMirrorCheckError(void);  // return 1 if error, refreshes ChannelStatus
void __declspec(dllexport) __stdcall sMirrorReset(void);
void __declspec(dllexport) __stdcall sMirrorSetSleep(unsigned short sd);   // in sec, 0 means sleep mode is off
void __declspec(dllexport) __stdcall sMirrorGroupShow(void);
void __declspec(dllexport) __stdcall sMirrorSetInterp(unsigned short mcs);   // in micro sec, 0 means interp mode is off
unsigned short __declspec(dllexport) __stdcall sMirrorGetInterp(void);   // in micro sec, 0 means interp mode is off

int __declspec(dllexport) __stdcall sGetNChannels(void);
int __declspec(dllexport) __stdcall sSetNChannels(int);
int __declspec(dllexport) __stdcall sGetNGroups(void);
int __declspec(dllexport) __stdcall sSetNGroups(int);

int __declspec(dllexport) __stdcall sGetChannelStatus(int*); // int[NChannels]

int __declspec(dllexport) __stdcall sGetGLimit(double*); // double[NChannels]
int __declspec(dllexport) __stdcall sGetMaxVol(double*); // double[NChannels]
int __declspec(dllexport) __stdcall sGetMinVol(double*); // double[NChannels]
int __declspec(dllexport) __stdcall sGetCurrVol(double*); // double[NChannels]
int __declspec(dllexport) __stdcall sGetAddVol(double*); // double[NChannels]

int __declspec(dllexport) __stdcall sSetGLimit(double*); // double[NChannels]
int __declspec(dllexport) __stdcall sSetMaxVol(double*); // double[NChannels]
int __declspec(dllexport) __stdcall sSetMinVol(double*); // double[NChannels]
int __declspec(dllexport) __stdcall sSetAddVol(double*); // double[NChannels]

int __declspec(dllexport) __stdcall sGetGroupVol(double*); // double[NGroups]
int __declspec(dllexport) __stdcall sGetWeights(double*); // double[NChannels*NGroups], [ng*NChannels + nc]
int __declspec(dllexport) __stdcall sSetWeights(double*); // double[NChannels*NGroups]

BOOL __declspec(dllexport) __stdcall sIsBoard(void);
BOOL __declspec(dllexport) __stdcall sIsHV(void);
BOOL __declspec(dllexport) __stdcall sIsError(void);
BOOL __declspec(dllexport) __stdcall sIsSleepMode(void);

unsigned __declspec(dllexport) __stdcall sMirrorCheckDevNum(int ndev);   // check device number, if ndev == -1, return current dev
void __declspec(dllexport) __stdcall sMirrorSetAnalogue(bool b);
unsigned long __declspec(dllexport) __stdcall sMirrorGetStatus(void);


BOOL __declspec(dllexport) __stdcall sIsPW(void);
bool __declspec(dllexport) __stdcall sMirrorSetPower(bool b);


typedef int (__stdcall  *WFCCH)(int chn, int grn, void *data); // called if the user click on the WFC map. Return !0 to cancel hook chain. data is additional data, chn <=0 means changes done
void __declspec(dllexport) __stdcall sSetWFCconfigHook(WFCCH f, void *data);
void __declspec(dllexport) __stdcall sMirrorConfigShow(int mode);       // 0 - modal, 1 - show, -1 hide
void __declspec(dllexport) __stdcall sMirrorConfigSelect(int chn, int grn); // chn > 0, grn > 0

typedef int (__stdcall  *WFCCH3)(void *, int chn, int grn, void *data); // called if the user click on the WFC map. Return !0 to cancel hook chain. data is additional data, chn <=0 means changes done
#ifdef __cplusplus
}
#endif

#endif
