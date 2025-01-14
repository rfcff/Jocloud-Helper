// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#define	 DLL_SOUI
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include "resource.h"
#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"
#include "common/ui/controls.extend/SGroupList.h"
#include "common/ui/controls.extend/SAniWindow.h"
#include "common/ui/controls.extend/SCheckBox2.h"
#include "core/SSingleton.h"
#include "common/config/config.h"

#include "scene/uiIBasePage.h"
#include "scene/uiPageConstant.h"
#include "common/string/string.h"
#include "common/log/loggerExt.h"
#include "interface/thunder/MediaManager.h"
#include "common/crash/Msjexhnd.h"
#include "common/utils/utils.h"
#include "scene/appInfoConstant.h"

#include <memory>
#include <map>
#include <string>
#include <corecrt_io.h>
#include "../src/common/string/string.h"
using namespace SOUI;
using namespace base;
#define MAX_MESSAGE_COUNT_TEXT 36

typedef struct TTNetworkQuality
{
	string strUid;
	NetworkQuality txQuality;
	NetworkQuality rxQuality;
}tTNetworkQuality;

typedef struct ThunderMessage
{
	HRESULT hResult;
	DWORD   dwEventID;
	LPVOID  lpBuffer;
} THUNDERMESSAGE, *LPTHUNDERMESSAGE;


typedef struct JoinRoomPar
{
	char roomName[64];
	char uid[64];
	int elapsed;
}tJoinRoomPar;

typedef struct RemoteUserPar
{
	char uid[64];
	bool elapsed;
}tRemoteUserPar,*LPREMOTEUSERPAR;

typedef struct RealWndClickPar
{
	int nRealWndId;
	int nBtnClick;
	bool bBtnStatic;
}tRealWndClickPar, *REALWNDCLICKPAR;

enum JoinRoomStatus {
	Leave = 0,
	Joining,
	Joined
};
