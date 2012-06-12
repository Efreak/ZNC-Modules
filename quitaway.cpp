/*
 * Copyright (C) 2004-2009  See the AUTHORS file for details.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include "Chan.h"
#include "User.h"
#include "Modules.h"

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

MODULEDEFS(CQuitAway, "Set away message on quit (from quit message)")

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

