/*
 * Copyright (C) 2010 Efreak.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation. 
 * 
 * If any other changes are made, update the version number to the latest ZNC
 * version to reflect it, please. This makes things easier to know what
 * version it works with; If you want to contribute code, let me know via IRC
 * and I'll add you as a contributor on GitHub.
 * -Efreak
 */



#include "znc/znc.h"
#include "znc/User.h"

using std::map;

class CBroadcastMod : public CModule {
public:
	MODCONSTRUCTOR(CBroadcastMod) 
    {
    }
   
	virtual ~CBroadcastMod() {}
   
	virtual bool OnLoad(const CString& sArgs, CString& sErrorMsg) {
		if(!sArgs.empty())
		bcprefix=sArgs + " "; //used for the broadcast prefix. Basically just grab all the arguments.
		else bcprefix="ZNC Global Message: ";
		return true;
	}
   
	virtual void OnModCommand(const CString& sLine) {
		CString sCommand = sLine.Token(0).AsLower();
      
		if (!GetUser()->IsAdmin()) {			//so far all these commands are administrative.
			PutModule("Access denied");		//maybe a module name change is in order.
			return;
		}

/*
 *   Broadcast
 */

		if (sCommand == "global"||sCommand=="broadcast") {
			CString message=bcprefix + sLine.Token(1,true);
			CZNC::Get().Broadcast(message);
		} else if (sCommand=="set") { 
			bcprefix=sLine.Token(1,true);
		} else if (sCommand=="show"||sCommand=="set") { 
			PutModule("Prefix set to '" + bcprefix + "'.");
		}
/*
 *   Help command (todo: implement token-based help)
 */
		else {
//Broadcast
			PutModule("Broadcast: This module includes a broadcast command that prefixes a word(s) to the beginning of the");
			PutModule("      message. Prefix defaults to 'ZNC Global Message: ', and can be changed by the arguments.");
			PutModule("Commands:");
			PutModule("      broadcast - broadcast specified message with the set prefix");
			PutModule("      set - set the prefix to the following text. May be replaced during a rehash or restart.");
			PutModule("      show - show the current prefix.");
		}
	}
   
protected:
   
private:
	CString bcprefix;
};

GLOBALMODULEDEFS(CBroadcastMod, "Broadcast with a prefix. Version 1.0");
