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
 * and I'll ad you as a contributor on GitHub.
 * -Efreak
 */

#include "znc/Chan.h"
#include "znc/User.h"
#include "znc/Modules.h"

class CQuitAway : public CModule {
public:
	MODCONSTRUCTOR(CQuitAway) {}

	virtual ~CQuitAway() {}

	bool message;

	virtual bool OnLoad(const CString& sArgs, CString& sMessage) {
		if (sArgs.Token(0) == "+message") 
			message=true;
		else message=false;
		return true;
	}

	virtual EModRet OnUserRaw(CString& sLiner) {
		if(sLiner.Token(0).AsLower()=="quit") {
			CString sLine=sLiner.Token(1,true);
			if(CString(sLine[0])==":")
				sLine=sLine.LeftChomp_n(1);
			PutIRC("AWAY :" + sLine);
			if(message) PutModule("You are now away: " + sLine);
		}
		return CONTINUE;
	}
};

MODULEDEFS(CQuitAway, "Set away message on quit (from quit message). Version 1.0")

/*
on modulecall user raw {
	if we're quitting {
		new cstring for the reason
		if it begins with a :, { get rid of it }
		put away plus : plus reason
		notify the user
	} continue quitting, or w/e we're doing.
}
*/
