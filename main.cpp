
#include "snipper.h"
#include "dbsnip.h"
#include <utilfuncs/utilfuncs.h>
#include <HoM/HoM.h>


//bool Initialize()
//{
//	if (!HoMicile()) { telluser(spf("HoM: ", HoM_Message())); return false; }
//	SConfig.QDFile=path_append(HoM_PATH, "snipper");
//	if (!path_realize(SConfig.QDFile)) { telluser("Config failed: ", filesys_error()); return false; }
//	SConfig.QDFile=path_append(SConfig.QDFile, "snipper.conf");
//	return SConfig.Load();
//}
//
//void Terminate()
//{
//	SConfig.Save();
//}

GUI_APP_MAIN
{
	if (GetAppHoMConfig(GetExeFilePath().ToStd(), Snipper_Config))
	{
		Snipper().Run();
	}
	else tellerror("Cannot create HoM for ", GetExeTitle().ToStd());

//	if (Initialize())
//	{
//		Snipper().Run();
//		Terminate();
//	}
//	else telluser("Cannot initialize snipper");
}
