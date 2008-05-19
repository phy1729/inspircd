/*       +------------------------------------+
 *       | Inspire Internet Relay Chat Daemon |
 *       +------------------------------------+
 *
 *  InspIRCd: (C) 2002-2008 InspIRCd Development Team
 * See: http://www.inspircd.org/wiki/index.php/Credits
 *
 * This program is free but copyrighted software; see
 *            the file COPYING for details.
 *
 * ---------------------------------------------------
 */

/* $ModDesc: Provides the UNINVITE command which lets users un-invite other users from channels (!) */

#include "inspircd.h"

/** Handle /UNINVITE
 */	 
class CommandUninvite : public Command
{
 public:
	CommandUninvite (InspIRCd* Instance) : Command(Instance,"UNINVITE", 0, 2)
	{
		this->source = "m_uninvite.so";
		syntax = "<nick> <channel>";
		TRANSLATE3(TR_NICK, TR_TEXT, TR_END);
	}

	CmdResult Handle (const std::vector<std::string> &parameters, User *user)
	{
		User* u = ServerInstance->FindNick(parameters[0]);
		Channel* c = ServerInstance->FindChan(parameters[1]);
			 
		if ((!c) || (!u))
		{	
			if (!c)
			{
				user->WriteNumeric(401, "%s %s :No such nick/channel",user->nick.c_str(), parameters[1].c_str());
			}
			else
			{
				user->WriteNumeric(401, "%s %s :No such nick/channel",user->nick.c_str(), parameters[0].c_str());
			}
				
			return CMD_FAILURE;
		}	

		if (c->modes[CM_INVITEONLY])
		{
			if (c->GetStatus(user) < STATUS_HOP)
			{
				user->WriteNumeric(482, "%s %s :You must be at least a%soperator to change modes on this channel",user->nick.c_str(), c->name.c_str(),
						ServerInstance->Config->AllowHalfop ? " half-" : " channel ");
				return CMD_FAILURE;
			}
		}

		irc::string xname(c->name.c_str());

		if (!u->IsInvited(xname))
		{
			user->WriteNumeric(491, "%s %s %s :Is not invited to channel %s", user->nick.c_str(), u->nick.c_str(), c->name.c_str(), c->name.c_str());
			return CMD_FAILURE;
		}
		if (!c->HasUser(user))
		{
			user->WriteNumeric(492, "%s %s :You're not on that channel!",user->nick.c_str(), c->name.c_str());
			return CMD_FAILURE;
		}

		u->RemoveInvite(xname);
		user->WriteNumeric(494, "%s %s %s :Uninvited", user->nick.c_str(), c->name.c_str(), u->nick.c_str());
		u->WriteNumeric(493, "%s :You were uninvited from %s by %s", u->nick.c_str(), c->name.c_str(), user->nick.c_str());
		c->WriteChannelWithServ(ServerInstance->Config->ServerName, "NOTICE %s :*** %s uninvited %s.", c->name.c_str(), user->nick.c_str(), u->nick.c_str());

		return CMD_SUCCESS;
	}
};

class ModuleUninvite : public Module
{
	CommandUninvite *mycommand;

 public:

	ModuleUninvite(InspIRCd* Me) : Module(Me)
	{
		
		mycommand = new CommandUninvite(ServerInstance);
		ServerInstance->AddCommand(mycommand);

	}
	
	virtual ~ModuleUninvite()
	{
	}
	
	virtual Version GetVersion()
	{
		return Version(1, 2, 0, 0, VF_VENDOR, API_VERSION);
	}
};

MODULE_INIT(ModuleUninvite)

