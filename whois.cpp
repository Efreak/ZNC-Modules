/*
 * Copyright (C) 2012 Efreak.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include "User.h"
#include "IRCSock.h"
#include "znc.h"

class CWhois : public CGlobalModule {
public:
	GLOBALMODCONSTRUCTOR(CWhois) {
		m_bDev = false;
		m_bReqAdmin = false;
		m_bNumerics = true;
		m_bServerName = false;
		m_sServerName = "irc.znc.in";
	}

	virtual ~CWhois() {}

	virtual bool OnLoad(const CString& sArgsStr, CString& sMessage) {
		CString sArgs(sArgsStr);

		while (sArgs.Left(1) == "-") {
			CString sOpt = sArgs.Token(0);
			sArgs = sArgs.Token(1, true);

			if (sOpt.Equals("-REQADMIN")) {
				m_bReqAdmin=true;
			} else if (sOpt.Equals("-NONUMERICS")) {
				m_bNumerics=false;
			} else if (sOpt.Equals("-DEV")) {
				m_bDev=true;
			} else if (sOpt.Left(12).Equals("-SERVERNAME=")) {
				m_bServerName=true;
				m_sServerName=sOpt.Token(1,false,"=");
			} else {
				sMessage="Invalid Paramter specified: "+sOpt+". Valid parameters are: -reqadmin, -nonumerics, -dev, -servername=<name>";
				return false;
			}
		}
		return true;
	}

	virtual EModRet OnUserRaw(CString& sLine) {
		if(sLine.Token(0).Equals("WHOIS")) {
			CString sNick = sLine.Token(1);
			if(sNick.Left(1)=="?") {
				sNick = sNick.LeftChomp_n(1);
				if (!m_pUser->IsAdmin() && m_bReqAdmin)
					PutModule("Access denied");
				else if(ZNCWhois(sNick)) return HALT;
			}
		} else if(sLine.Token(0).Equals("AWAY")) {
			CString sReason = sLine.Token(1, true);
			if(sReason.Left(1)==":") {
				sReason = sReason.LeftChomp_n(1);
				SetNV("Away"+m_pUser->GetUserName(),sReason);
			}
		}
		return CONTINUE;

	}

	virtual void OnModCommand(const CString& sCommand) {
		CString sCmd = sCommand.Token(0);

		if (!m_pUser->IsAdmin()) {
			PutModule("Access denied");
			return;
		}
		PutModule("There are no commands yet");
	}

private:
	bool	m_bDev;
	bool	m_bReqAdmin;
	bool	m_bNumerics;
	bool	m_bServerName;
	CString	m_sServerName;

	bool ZNCWhois(CString& sUser) {
		CUser *pUser = CZNC::Get().FindUser(sUser);
		if(!pUser) {
			PutModule("No such user!");
			return false;
		}

		CString Username = pUser->GetUserName();
		CString Nickname = pUser->GetNick();
		CString Ident = pUser->GetIdent();
		CString Server = (!m_pUser->IsAdmin() && m_bServerName ? m_sServerName : pUser->GetIRCServer());	//if whoiser isnt an admin, and the servername is defined, use it. otherwise grab the real server name.
		CString VHost = pUser->GetBindHost();
		CString RealName = pUser->GetRealName();
		CString MyNick = m_pUser->GetNick();
		bool Attached = pUser->IsUserAttached();
		if(VHost=="") VHost = (m_bServerName ? m_sServerName : "irc.znc.in");
		int SSL = 0; int total=0;
		CString LastSeen = CString(time(NULL) - GetNV("LastSeen"+Username).ToInt());
		CString Logon = CString(time(NULL) - GetNV("LastSeen"+Username).ToInt());
		CString IP;
		bool Away = pUser->IsIRCAway();
		CString sAwayMsg = GetNV("Away"+pUser->GetUserName());
		if(sAwayMsg != "") sAwayMsg = ": "+sAwayMsg;

		vector<CClient*>& vClients = pUser->GetClients();
		if (!vClients.empty()) {
			 for (unsigned int a = 0; a < vClients.size(); a++) {
				total++;
				if(vClients[a]->GetSSL()) SSL++;
			}
		}
		if (!vClients.empty()) {
			for (unsigned int a = 0; a < vClients.size(); a++) {
			        IP = IP + vClients[a]->GetRemoteIP() + " ";
			}
		}
		CString rpl378 = ":irc.znc.in 378 " + MyNick + " ?" + Username + " :is connecting from *@";
		CString rpl311 = ":irc.znc.in 311 " + MyNick + " ?" + Username + " " + Ident + " " + VHost + " " + " * :" + RealName;
		CString rpl379 = ":irc.znc.in 379 " + MyNick + " ?" + Username + " :is using modes" +
			(pUser->IsUserAttached() ? " +Attached" : " -Attached") + (pUser->IsIRCConnected() ? " +IRCConnected" : " -IRCConnected") + 
			(pUser->DenyLoadMod() ? " +DenyLoadMod" : " -DenyLoadMod") + (pUser->MultiClients() ? " +MultiClients" : " -MultiClients") + 
			(pUser->KeepBuffer() ? " +KeepBuffer" : " -KeepBuffer") + (!pUser->HasServers() ? " -HasServers":"") + 
			(!pUser->GetIRCConnectEnabled() ? " -IRCConnectEnabled":""); 
		CString rpl671;
		if(SSL==1 && total==1) rpl671 = ":irc.znc.in 671 " + MyNick + " ?" + Username + " :is using a Secure Connection";
			else rpl671 = ":irc.znc.in 671 " + MyNick + " ?" + Username + " :is using " + (CString)SSL + " Secure Connections out of " + (CString)total + " total connections";
		CString rpl312 = ":irc.znc.in 312 " + MyNick + " ?" + Username + " " + Server + " :" + Username + " is" + (Attached ? " " : " \x02not\x02 ") +
			"attached to " + (m_bServerName ? m_sServerName : "ZNC");
		CString rpl301 = ":irc.znc.in 301 " + MyNick + " ?" + Username + " :?" + Username + " is away";
		CString rpl313 = ":irc.znc.in 313 " + MyNick + " ?" + Username + " :is a ZNC Admin";
		CString rpl317 = ":irc.znc.in 317 " + MyNick + " ?" + Username + " " + LastSeen + " " + Logon + " :seconds idle, signon time";
		CString rpl318 = ":irc.znc.in 318 " + MyNick + " ?" + Username + " :End of /WHOIS list";

		if(m_bDev || !m_bNumerics) {
			PutModule(rpl311);
			if(m_pUser->IsAdmin()) PutModule(rpl379);
			if(m_pUser->IsAdmin()) PutModule(rpl378);
			if(SSL>0) PutModule(rpl671);
			if(Away)PutModule(rpl301);
			PutModule(rpl312);
			if(pUser->IsAdmin()) PutModule(rpl313);
			PutModule(rpl318);
		} if (m_bNumerics) {					//not an else because of m_bDev
			PutUser(rpl311);					//rpl_whoisuser. Since we're on a bouncer, use ?Username!ident@VHost instead of nick@ident@host
			if(m_pUser->IsAdmin()) PutUser(rpl379);			//rpl_whoismodes. tell admins what options said user has (candcc, etc)
			if(m_pUser->IsAdmin()) PutUser(rpl378);			//rpl_whoishost. tell admins what host said user has
			if(SSL>0) PutUser(rpl671);				//rpl_ssl (?). is the user connected to ZNC via ssl?
			if(Away)PutUser(rpl301);					//rpl_away. is said user away?
			PutUser(rpl312);					//rpl_whoisserver. give the server the user is connected to. we don't know how to get the tagline, so lets just say we're connected via znc.
			if(pUser->IsAdmin()) PutUser(rpl313);			//rpl_whoisoperator. Is the user an admin on znc?
			PutUser(rpl317);					//rpl_whoisidle. tells the client the amount of time user has been idle, as well as the amt of time the client has been connected
			PutUser(rpl318);					//rpl_endofwhois. tells the client the whois data is done (why?)
		}
		return true;
	}

	virtual EModRet OnUserCTCP   (CString &sTarget,  CString &sMessage){ if(sTarget!=m_pUser->GetNick()) { SetNV("LastSeen"+m_pUser->GetUserName(),CString(time(NULL)));} return CONTINUE;}
	virtual EModRet OnUserAction (CString &sTarget,  CString &sMessage){ if(sTarget!=m_pUser->GetNick()) { SetNV("LastSeen"+m_pUser->GetUserName(),CString(time(NULL)));} return CONTINUE;}
	virtual EModRet OnUserMsg    (CString &sTarget,  CString &sMessage){ if(sTarget!=m_pUser->GetNick()) { SetNV("LastSeen"+m_pUser->GetUserName(),CString(time(NULL)));} return CONTINUE;}
	virtual EModRet OnUserNotice (CString &sTarget,  CString &sMessage){ if(sTarget!=m_pUser->GetNick()) { SetNV("LastSeen"+m_pUser->GetUserName(),CString(time(NULL)));} return CONTINUE;}
	virtual EModRet OnUserJoin   (CString &sChannel, CString &sKey)    { SetNV("LastSeen"+m_pUser->GetUserName(),CString(time(NULL))); return CONTINUE;}
	virtual EModRet OnUserPart   (CString &sChannel, CString &sMessage){ SetNV("LastSeen"+m_pUser->GetUserName(),CString(time(NULL))); return CONTINUE;}
	virtual EModRet OnUserTopic  (CString &sChannel, CString &sTopic)  { SetNV("LastSeen"+m_pUser->GetUserName(),CString(time(NULL))); return CONTINUE;}
	virtual void OnClientLogin      () {SetNV("Login"+m_pUser->GetUserName(),CString(time(NULL)));}
	virtual void OnClientDisconnect () {SetNV("Login"+m_pUser->GetUserName(),CString(time(NULL)));}

	virtual EModRet OnAddUser(CUser& User, CString& sErrorRet) {
		SetNV("LastSeen"+User.GetUserName(),CString(time(NULL)));
		SetNV( "Login" + User.GetUserName(),CString(time(NULL)));
		return CONTINUE;
	}

};

GLOBALMODULEDEFS(CWhois, "Whois BNC users, with extra info for admin. Version 2.0.")
