// MainDlg.h : interface of the UIMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "scene\uiScenePage.h"
#include "../logic/logicLanguageTranslate.h"
#include "../logic/logicPageManager.h"

class IUIBasePage;

class UIMainDlg : public SHostWnd
{
public:
	UIMainDlg();
	~UIMainDlg();

private:
	void switchLanguage(UIPageTranslateLanguage language);

	void switchLanguage();

	void switchPage(IUIBasePage* from, IUIBasePage* to);

	void onPageEventHandler(UIPageEvent event, void* eventParam, IUIBasePage* source);

	void loadConfig();

	void registerEvent();

	void registerScene();

	/// event handler
	void onEventDisappearInitiative(UIPageEvent event, void* eventParam, IUIBasePage* source);
	void onEventSwitchLanguage(UIPageEvent event, void* eventParam, IUIBasePage* source);
	void onEventSwitchScene(UIPageEvent event, void* eventParam, IUIBasePage* source);

protected:
	/*
	 * Description: caption button click implementation
	*/
	void onClose();
	void onMaximize();
	void onRestore();
	void onMinimize();
	void onSize(UINT nType, CSize size);
	BOOL onInitDialog(HWND wndFocus, LPARAM lInitParam);
	void onPosChanged(CPoint pos);
	//void onPosChanged(LPWINDOWPOS pos);

	/*
	 * Description: Left controller init, click
	*/
	void onInitGroup(EventArgs *e);
	void onInitItem(EventArgs *e);
	void onGroupStateChanged(EventArgs *e);
	void onCtrlPageClick(EventArgs *e);
	LRESULT onThunderManagerCB(UINT uMsg, WPARAM wp, LPARAM lp, BOOL &bHandle);

	/*
	 * Description: SOUI event map
	*/
	EVENT_MAP_BEGIN()
		/*
		 * Description: caption button event map
		*/
		EVENT_NAME_COMMAND(L"btn_close", onClose)
		EVENT_NAME_COMMAND(L"btn_min", onMinimize)
		EVENT_NAME_COMMAND(L"btn_max", onMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", onRestore)
		

		/*
		 * Description: left controller event map
		*/
		//EVENT_ID_HANDLER(R.id.gl_catalog, EventGroupListInitGroup::EventID, onInitGroup)
		//EVENT_ID_HANDLER(R.id.gl_catalog, EventGroupListInitItem::EventID, onInitItem)
		//EVENT_ID_HANDLER(R.id.gl_catalog, EventGroupStateChanged::EventID, onGroupStateChanged)
		//EVENT_ID_HANDLER(R.id.gl_catalog, EventGroupListItemCheck::EventID, onCtrlPageClick)

		/*
		 * Description: other page event map
		*/
		CHAIN_EVENT_MAP_MEMBER(*_oLogicPageManager.getPageFromType(UIScenePageType::SCENE_PAGE_NULL))
		CHAIN_EVENT_MAP_MEMBER(*_oLogicPageManager.getPageFromType(UIScenePageType::SCENE_PAGE_LEFT))
		CHAIN_EVENT_MAP_MEMBER(*_oLogicPageManager.getPageFromType(UIScenePageType::SCENE_PAGE_VIDEO_SAME_ROOM))
		CHAIN_EVENT_MAP_MEMBER(*_oLogicPageManager.getPageFromType(UIScenePageType::SCENE_PAGE_VIDEO_DIFFERENT_ROOM))
	EVENT_MAP_END()
		
	/*
	 * Description: HostWnd real window handle event
	*/
	BEGIN_MSG_MAP_EX(UIMainDlg)
		MSG_WM_INITDIALOG(onInitDialog)
		MSG_WM_CLOSE(onClose)
		MSG_WM_SIZE(onSize)
		MSG_WM_MOVE(onPosChanged)
		//MSG_WM_WINDOWPOSCHANGED(onPosChanged)
		MESSAGE_HANDLER(WM_THUNDER_MESSAGE, onThunderManagerCB)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	LogicPageManager _oLogicPageManager;
	LogicLanguageTranslate _oLogicLanguageTranslate;
	UIPageEventHandleProc _oEventHandler[int(UIPageEvent::PAGE_EVENT_MAX)];
	IUIBasePage* _pThePageUsing;
};
