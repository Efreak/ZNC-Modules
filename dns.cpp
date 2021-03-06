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




#include "User.h"
#include "IRCSock.h"
#include "znc.h"
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

using std::map;

class CDNSMod : public CGlobalModule {
public:
	GLOBALMODCONSTRUCTOR(CDNSMod) {}
   
	virtual ~CDNSMod() {}
   
	virtual void OnModCommand(const CString& sLine) {
		CString sCommand = sLine.Token(0).AsLower();
		CString tk2 = sLine.Token(1).AsLower(); 
      
/*
 *   DNS Resolution
 */
		if(sCommand=="dns"||sCommand=="resolve") {
			hostent * record = gethostbyname(tk2.c_str());
			if(record == NULL) {
				PutModule(tk2 + "%s is unavailable.");
			} else {
				in_addr * address = (in_addr * )record->h_addr;
				CString ip_address = CString(inet_ntoa(* address));
				PutModule(tk2 + " resolves to: " + ip_address + ".");
			}
		}
/*
 *   Help command (todo: implement token-based help)
 */
		else if (sCommand == "help") {
			PutModule("DNS: This module includes a DNS resolver. Usage: dns/resolve hostname Ex: dns localhost");
			PutModule("      message. Prefix defaults to 'ZNC Global Message: ', and can be changed by the arguments.");
//			PutModule("ToDo: allow this module to override znc's dns resolution to deny users access to specific servers (can this be done?")
//			PutModule("1.0 - initial version");
//			PutModule("2.0 - changed around resolve code to fix a crashing error when host doesnt resolve");
			PutModule("0.01 - changed version number to a different scheme");
		}
	}
   
protected:
   
private:
};

GLOBALMODULEDEFS(CDNSMod, "DNS Resolver. Version 0.01");