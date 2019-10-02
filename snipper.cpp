
#include "snipper.h"
#include "dbsnip.h"
#include <utilfuncs/utilfuncs.h>
#include <fstream>


#define IMAGEFILE <snipper/snipper.iml>
#define IMAGECLASS SNIPPICS
#include <Draw/iml.h>


AppConfig Snipper_Config;


bool is_snip_db(const std::string &sdb)
{
	if (!file_exist(sdb)) return false;
	bool b=false;
	DB_SNIP db;
	db.Open(sdb);
	b=((db.bDBOK)&&db.ValidateHasTable("snippets"));
	db.Close();
	return b;
};


Snipper::Snipper()
{
	SetRect(0,0,1000,800);
	Title("Snipper").Sizeable().CenterScreen();
	Icon(SNIPPICS::snipper_icon());
	
	AddFrame(mainmenu);
	mainmenu.Set([&](Bar &bar){ Menu_Main(bar); });
	
	panSnip.Add(cbDB.HSizePosZ().TopPos(2, 20));
	panSnip.Add(arSnip.HSizePosZ().TopPosZ(24));
	arSnip.AutoHideSb().AddColumn();
	arSnip.AutoHideSb().AddColumn("Snippet", 250).Sorting(); //.Edit(eb).Sorting();
	arSnip.NoFocusSetCursor();
	arSnip.WhenBar = THISFN(Menu_Snip);
	arSnip.WhenLeftClick = THISFN(ShowSel);
	//arSnip.WhenAcceptEdit << [&]{ OnSave(); };
	
	panDed.Add(ded.HSizePosZ(0, 0).VSizePosZ(0, 52));
	panDed.Add(btnNew.SetLabel("New..").LeftPos(16, 60).BottomPosZ(16, 20));
	panDed.Add(btnSave.SetLabel("Save").RightPosZ(16, 60).BottomPosZ(16, 20));
	panDed.Add(btnReset.SetLabel("Reset").RightPosZ(88, 60).BottomPosZ(16, 20));
	btnNew.Enable(false);
	btnSave.Enable(false);
	btnReset.Enable(false);
	btnNew.WhenAction = [&]{ NewSnippet(); };
	btnSave.WhenAction << [&]{ OnSave(); arSnip.Enable(); ShowSel(); btnSave.Enable(false); btnReset.Enable(false); };
	btnReset.WhenAction << [&]{ arSnip.Enable(); ShowSel(); btnSave.Enable(false); btnReset.Enable(false); };
	ded.WhenAction << [&]{ if (ded.IsDirty()) { btnSave.Enable(); btnReset.Enable(); arSnip.Enable(false); } };
	
	hsplit.Horz(panSnip, panDed);
	hsplit.SetPos(2000);

	Add(hsplit);

	arSnip.Enable(false);
	ded.Enable(false);
	
	FillCBDB();
	
	cbDB.WhenAction << [&]{ OnCBDBClick(); };
	
}

Snipper::~Snipper() { DB.Close(); }

void Snipper::Close() { OnClose(); }

void Snipper::OnClose() { if (ded.IsDirty()) { PromptOK("You need to 'Save' or 'Reset' ..."); } else Break(); } //  if (PromptOKCancel("Exit Snipper?")) Break(); }

void Snipper::FillCBDB()
{
	cbDB.Clear();
	if (Snipper_Config.data.size())
	{
		bool bsave=false;
		std::string sk, sv;
		auto it=Snipper_Config.data.begin();
		while (it!=Snipper_Config.data.end())
		{
			sv=it->first;
			sk=it->second;
			if (sieqs(sv.substr(0,7), "snipdb:")&&is_snip_db(sk))
			{
				sv=sv.substr(7);
				cbDB.Add(sk.c_str(), sv.c_str());
			}
			it++;
		}
		if (bsave) Snipper_Config.Save();
	}
}

void Snipper::OnCBDBClick()
{
	int n=cbDB.GetIndex();
	if (n>=0) { std::string sdb=cbDB.GetKey(n).ToString().ToStd(); SetCurrentDB(sdb); btnNew.Enable(); }
}

void Snipper::Menu_Main(Bar &bar)
{
	bar.Sub("Database", [&](Bar &bar){ Menu_Database(bar); });
	bar.Sub("Snippets", [&](Bar &bar){ Menu_Snip(bar); });
}

void Snipper::Menu_Database(Bar &bar)
{
	bar.Add("Open Database", [&]{ OpenDB(); });
	bar.Add("New Database", [&]{ OpenDB(true); });
	bar.Separator();
	bar.Add("Exit", [&]{ OnClose(); });
}

void Snipper::Menu_Snip(Bar &bar)
{
	int cur=arSnip.GetCursor();
	bar.Add(DB.bDBOK, "New Snippet", [&]{ NewSnippet(); });
	bar.Add(DB.bDBOK, "From File", [&]{ FromFile(); });
	bar.Separator();
	bar.Add((DB.bDBOK&&(cur>=0)), "Delete", [&]{ DeleteSnippet(); });
}

void Snipper::OpenDB(bool bNew)
{
	FileSel fs;
	if (bNew)
	{
		if (fs.ExecuteSaveAs("New Snippets database.."))
		{
			if (FileExists(fs.Get())) { PromptOK("Name exists - specify another"); return; }
			DB.Close();
			if (DB.Open(fs.Get().ToStd())) { DB.ImplementSchema(); ConfigAddDB(DB.sDB); }
		}
	}
	else
	{
		if (fs.ExecuteOpen())
		{
			if (is_snip_db(fs.Get().ToStd())) { DB.Close(); DB.Open(fs.Get().ToStd()); ConfigAddDB(DB.sDB); }
			else { PromptOK("Invalid database specified"); }
		}
	}
//	if (!DB.bDBOK) { arSnip.Clear(); arSnip.Enable(false); ded.Clear(); ded.Enable(false); } --- do nothing...
}

bool Snipper::ConfigAddDB(const std::string sdb)
{
	if (!file_exist(sdb)) { telluser("Database '", sdb, "' not found."); return false; }
	std::string sn, st=path_name(sdb);
	if (Snipper_Config.hasval(st)) { if (!Snipper_Config.haskeyval(st, sdb)) telluser("Database-name '", st, "' already used."); return false; }
	if (!Snipper_Config.haskeyval(st, sdb))
	{
		sn=spf("snipdb:", st);
		Snipper_Config.setval(sn, sdb);
		Snipper_Config.Save();
		FillCBDB();
	}
	cbDB.SetData(sdb.c_str());
	Title(spf("Snipper: ", DB.sDB).c_str());
	arSnip.Clear(); arSnip.Enable();
	ded.Clear(); ded.Enable();
	FillArSnip();
	return true;
}

void Snipper::SetCurrentDB(std::string sdb)  ///todo:... use this func everywhere ...
{
	DB.Close();
	DB.Open(sdb);
	if (DB.bDBOK)
	{
		Title(spf("Snipper: ", sdb).c_str());
		arSnip.Clear(); arSnip.Enable();
		ded.Clear(); ded.Enable();
		FillArSnip();
	}
	else { arSnip.Clear(); arSnip.Enable(false); ded.Clear(); ded.Enable(false); }
}

void Snipper::NewSnippet()
{
	struct NameDlg : public TopWindow
	{
		//using CLASSNAME=NameDlg;
		typedef NameDlg CLASSNAME;
		bool bOK{false};
		EditString eb;
		Button bk;
		Button bc;
		virtual ~NameDlg() {}
		NameDlg()
		{
			Title("Snippet name");
			SetRect(0,0,280,80);
			CenterOwner();
			Add(eb.HSizePosZ(4, 4).TopPosZ(4, 19));
			Add(bk.SetLabel(t_("OK")).LeftPosZ(28, 60).TopPosZ(32, 20));
			Add(bc.SetLabel(t_("Cancel")).LeftPosZ(112, 60).TopPosZ(32, 20));
			bk.WhenAction=[&]{ std::string s=eb.GetData().ToString().ToStd(); TRIM(s); bOK=!s.empty(); Close(); };
			bc.WhenAction=[&]{bOK=false; Close(); };
		}
		bool Key(dword k, int c) { if (k==K_ENTER) { bk.WhenAction(); return true; } else if (k==K_ESCAPE) { bc.WhenAction(); return true; } return false; }
	} dlg;
	dlg.Execute();
	if (dlg.bOK)
	{
		Snippet snip;
		std::string s=dlg.eb.GetData().ToString().ToStd(); TRIM(s);
		snip.name=s;
		DB.Save(snip);
		FillArSnip();
		SelectSnip(snip.id);
		arSnip.Enable();
		ShowSel();
		btnSave.Enable(false);
		btnReset.Enable(false);
		ded.SetFocus();
	}
}

void Snipper::FromFile()
{
	Snippet snip;
	FileSel fs;
	if (fs.ExecuteOpen())
	{
		snip.name=GetFileName(fs.Get());
		snip.snip=LoadFile(fs.Get());
		DB.Save(snip);
		arSnip.Enable();
		FillArSnip();
		SelectSnip(snip.id);
		btnSave.Enable(false);
		btnReset.Enable(false);
	}
}
void Snipper::FillArSnip()
{
	arSnip.Clear();
	ded.Clear();
	DB.Load(snips);
	if (snips.size()>0)
	 for (auto p:snips)
	 {
	     std::string sidsnip=ttos<size_t>(p.second.id);
		 arSnip.Add(sidsnip.c_str(), p.second.name.c_str());
	 }
	//Refresh()
}

void Snipper::ShowSel()
{
	int cur=arSnip.GetCursor();
	if (cur>=0)
	{
		size_t id=stot<size_t>(arSnip.Get(cur, 0).ToString().ToStd());
		ded.SetData(snips[id].snip.c_str());
	}
	btnSave.Enable(false);
}

void Snipper::DeleteSnippet()
{
	int cur=arSnip.GetCursor();
	if (cur>=0)
	{
		size_t id=stot<size_t>(arSnip.Get(cur, 0).ToString().ToStd());
		DB.Delete(id);
		FillArSnip();
	 }
}
	 
void Snipper::OnSave() //bool bNew)
{
	int cur=arSnip.GetCursor();
	size_t id=0;
	//if (!bNew&&(cur>=0))
	if (cur>=0)
	{
		id=stot<size_t>(arSnip.Get(cur, 0).ToString().ToStd());
		snips[id].name=arSnip.Get(cur, 1).ToString().ToStd();
		snips[id].snip=ded.GetData().ToString().ToStd();
		DB.Save(snips[id]);
	}
//	else
//	{
//		Snippet snip;
//		snip.snip=ded.GetData().ToString().ToStd();
//		DB.Save(snip);
//		id=snip.id;
//	}
	FillArSnip();
	SelectSnip(id);
}

void Snipper::SelectSnip(size_t id)
{
	int i=0, n=arSnip.GetCount();
	bool b=false;
	while (!b&&(i<n)) { if (!(b=(id==stot<size_t>(arSnip.Get(i, 0).ToString().ToStd())))) i++; }
	if (b) arSnip.SetCursor(i);
	ded.SetData(snips[id].snip.c_str());
}


