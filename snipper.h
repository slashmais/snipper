#ifndef _snipper_snipper_h
#define _snipper_snipper_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

//#define LAYOUTFILE <snipper/snipper.lay>
//#include <CtrlCore/lay.h>


#include "snippet.h"
#include <map>
#include <HoM/HoM.h>


extern AppConfig Snipper_Config;



struct Panel : public Ctrl
{
	typedef Panel CLASSNAME;
	virtual ~Panel(){}
};

//struct EB : public EditString
//{
//	virtual ~EB() {}
//	/ /Event<bool> WhenDone;
//	/ /bool Key(dword key, int rep) { if ((key==K_ENTER)||(key==K_ESCAPE)) { bool b=(key==K_ENTER); if (WhenDone) WhenDone(b); return true; } else return EditString::Key(key, rep); }
//	/ /virtual void Close() { if (WhenDone) WhenDone(false); }
//};

struct Snipper : public TopWindow
{
	typedef Snipper CLASSNAME;
	
	DocEdit ded;
	DropList cbDB;
	ArrayCtrl arSnip;
	Button btnNew, btnSave, btnReset, btnClose;
	Panel panSnip, panDed;
	Splitter hsplit;
	Snippets snips;
	MenuBar mainmenu;

	Snipper();
	virtual ~Snipper();
	
	virtual void Close();
	void OnClose();
	
	void FillCBDB();
	void OnCBDBClick();
	void Menu_Main(Bar &bar);
	void Menu_Database(Bar &bar);
	void Menu_Snip(Bar &bar);
	
	void OpenDB(bool bNew=false);
	bool ConfigAddDB(const std::string sdb);
	void SetCurrentDB(std::string sdb);
	void NewSnippet();
	void FromFile();
	void FillArSnip();
	void ShowSel();
	void DeleteSnippet();
	void OnSave(); //bool bNew=false);
	void SelectSnip(size_t id);
};





#endif
