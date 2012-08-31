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

#include "znc.h"
#include "User.h"
#include "Modules.h"

class CKillNoticeMod : public CGlobalModule {
public:
        GLOBALMODCONSTRUCTOR(CKillNoticeMod) {}

        EModRet OnRaw(CString& sLine) {
                CString sCmd = sLine.Token(0).AsUpper();

                if(sCmd == "ERROR") {
                        CString sRest = sLine.Token(1, true).AsLower();

                        if(sRest.find("lined") != CString::npos || sRest.find("kill") != CString::npos) {
                                if(m_pUser) {
					SendAdmins(m_pUser->GetUserName() + " (killed: " + sLine + ")");
                                }
                        }
                }
                return CONTINUE;
        }
private:
        void SendAdmins(const CString &msg) {
                CZNC::Get().Broadcast(msg, true, NULL, GetClient());
        }

};
GLOBALMODULEDEFS(CKillNoticeMod, "Sends admins a notice when a user gets killed. Version 0.01")