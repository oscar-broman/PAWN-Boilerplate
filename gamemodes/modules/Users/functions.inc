/*!
 * Users/functions.inc
 *
 > Priority: 0
 */

stock this.PrepareStatements() {
	this.StmtTestAuth       = db_prepare(this.DB, !"SELECT EXISTS(SELECT `uid` FROM `users` WHERE `uid` = ? AND `passhash` = ?)");
	this.StmtRegister       = db_prepare(this.DB, !"INSERT INTO `users` (`name`, `passhash`) VALUES (?, ?)");
	this.StmtRename         = db_prepare(this.DB, !"UPDATE `users` SET `name` = ? WHERE `uid` = ?");
	this.StmtChangePasshash = db_prepare(this.DB, !"UPDATE `users` SET `passhash` = ? WHERE `uid` = ?");
	this.StmtFindByName     = db_prepare(this.DB, !"SELECT `uid` FROM `users` WHERE `name` = ?");
}

// UID or player ID -> UID
stock bool:this.ResolveUID(&{UID, _}:uid, bool:only_logged_in = true) {
	if (0 <= _:uid < MAX_PLAYERS)
		uid = IsPlayerConnected(_:uid) ? (only_logged_in ? (this.PlayerUID[_:uid]) : (this.PlayerNameUID[_:uid])) : (INVALID_UID);
	else if (_:uid < MINIMUM_UID)
	 	uid = INVALID_UID;
	
	return (uid != INVALID_UID);
}

// UID or player ID -> player ID
stock bool:this.ResolvePlayerID(&{UID, _}:playerid) {
	if (_:playerid >= MINIMUM_UID)
		playerid = UID:this.FindPlayerByAccount(playerid);
	
	return !!IsPlayerConnected(_:playerid);
}

stock bool:this.RegisterPlayer(playerid, const password[]) {
	if (isnull(password))
		return false;
	
	new UID:uid = this.RegisterAccount(Player.Info[playerid][Name], password);
	
	if (uid == INVALID_UID)
		return false;
	
	this.PlayerUID[playerid] = uid;
	
	SetTimerEx(!"OnPlayerLogIn", 0, false, !"ii", playerid, false);
	
	return true;
}

stock UID:this.RegisterAccount(const name[], const password[]) {
	if (isnull(name) || isnull(password))
		return INVALID_UID;
	print(password);
	stmt_bind_value(this.StmtRegister, 0, DB::TYPE_STRING,  name);
	stmt_bind_value(this.StmtRegister, 1, DB::TYPE_WP_HASH, password);
	
	if (stmt_execute(this.StmtRegister)) {
		new UID:uid = UID:db_last_insert_rowid(this.DB);
		
		if (_:uid >= MINIMUM_UID)
			return uid;
	}
	
	return INVALID_UID;
}

stock bool:this.RenameAccount({UID, _}:uid, const new_name[]) {
	if (!this.ResolveUID(uid) || isnull(new_name))
		return false;
	
	stmt_bind_value(this.StmtRename, 0, DB::TYPE_STRING, new_name);
	stmt_bind_value(this.StmtRename, 1, DB::TYPE_INT, _:uid);
	
	return !!stmt_execute(this.StmtRename);
}

stock bool:this.ChangeAccountPassword({UID, _}:uid, const new_password[]) {
	if (!this.ResolveUID(uid) || isnull(new_password))
		return false;
	
	stmt_bind_value(this.StmtChangePasshash, 0, DB::TYPE_WP_HASH, new_password);
	stmt_bind_value(this.StmtChangePasshash, 1, DB::TYPE_INT, _:uid);
	
	return !!stmt_execute(this.StmtChangePasshash);
}

stock this.SaveAccount({UID, _}:_playerid) {
	new playerid = _:_playerid;
	
	if (!this.ResolvePlayerID(playerid))
		return;
	
	
}

/*stock this.DeleteAccount({UID, _}:uid) {
	
}*/

stock bool:this.LogPlayerIn(playerid, const password[]) {
	if (!this.IsRegistered(playerid) || this.IsLoggedIn(playerid))
		return false;
	
	if (!this.TestAccountAuth(playerid, password))
		return false;
	
	SetTimerEx(!"OnPlayerLogIn", 0, false, !"ii", playerid, false);
	
	return true;
}

stock bool:this.TestAccountAuth({UID, _}:uid, const password[]) {
	if (!this.ResolveUID(uid, false))
		return false;
	
	new bool:success = false;
	
	stmt_bind_value(this.StmtTestAuth, 0, DB::TYPE_INT, _:uid);
	stmt_bind_value(this.StmtTestAuth, 1, DB::TYPE_WP_HASH, password);
	
	stmt_bind_result_field(this.StmtTestAuth, 0, DB::TYPE_INT, success);
	
	stmt_execute(this.StmtTestAuth) && stmt_fetch_row(this.StmtTestAuth);
	
	return success;
}

stock UID:this.FindAccountByName(const name[]) {
	new UID:uid = INVALID_UID;
	
	stmt_bind_value(this.StmtFindByName, 0, DB::TYPE_STRING, name);
	
	stmt_bind_result_field(this.StmtFindByName, 0, DB::TYPE_INT, _:uid);
	
	stmt_execute(this.StmtFindByName) && stmt_fetch_row(this.StmtFindByName);
	
	return uid;
}

stock this.FindPlayerByAccount(UID:uid) {
	foreach(new playerid : Player) {
		if (this.PlayerUID[playerid] == uid)
			return playerid;
	}
	
	return INVALID_PLAYER_ID;
}

stock bool:this.IsLoggedIn({UID, _}:playerid) {
	if (!this.ResolvePlayerID(playerid))
		return false;
	
	return this.PlayerIsLoggedIn{_:playerid};
}

stock bool:this.IsRegistered({UID, _}:playerid) {
	if (!this.ResolvePlayerID(playerid))
		return false;
	
	return this.PlayerNameUID[_:playerid] != INVALID_UID;
}

stock this.ShowLogInFirst(playerid) {
	SendClientMessage(playerid, COLOR_GENERIC_ERROR, "ERROR: Please log in first.");
	
	PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
}

stock this.ShowDialog(playerid) {
	if (this.IsRegistered(playerid)) {
		inline LogInQuitConfirmResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			if (response)
				Kick(playerid);
			else
				this.ShowDialog(playerid);
		}
		
		inline LogInFailedResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			this.ShowDialog(playerid);
		}
		
		inline LogInResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			if (!response)
				Dialog_ShowCallback(playerid, using inline LogInQuitConfirmResponse, DIALOG_STYLE_MSGBOX, "Quit", "Are you sure you want to quit the server?", "Quit", "Cancel");
			else {
				if (isnull(inputtext)) {
					this.ShowDialog(playerid);
				} else if (this.LogPlayerIn(playerid, inputtext)) {
					
				} else {
					if (++this.LogInAttempts[playerid] >= this.MaxLogInAttempts) {
						SendClientMessage(playerid, COLOR_GENERIC_ERROR, "* You were kicked from the server because of too many failed log in attempts.");
						
						Kick(playerid);
					} else {
						new msg[80], attempts = this.MaxLogInAttempts - this.LogInAttempts[playerid];
						
						format(msg, _, "Invalid password. You have %d more %s, then you'll get kicked.", attempts, attempts == 1 ? ("attempt") : ("attempts"));
						
						Dialog_ShowCallback(playerid, using inline LogInFailedResponse, DIALOG_STYLE_MSGBOX, "Log in failed", msg, "Retry");
					}
				}
			}
		}
		
		Dialog_ShowCallback(playerid, using inline LogInResponse, DIALOG_STYLE_PASSWORD, "Log in", "This nickname is registered. Please log in or come back with another nickname.\n\nEnter your password:", "OK", "Cancel");
	} else {
		inline RegisterQuitConfirmResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			if (response)
				Kick(playerid);
			else
				this.ShowDialog(playerid);
		}
		
		inline RegisterResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			if (!response)
				Dialog_ShowCallback(playerid, using inline RegisterQuitConfirmResponse, DIALOG_STYLE_MSGBOX, "Quit", "Are you sure you want to quit the server?", "Quit", "Cancel");
			else {
				if (isnull(inputtext)) {
					this.ShowDialog(playerid);
				} else if (this.RegisterPlayer(playerid, inputtext)) {
					Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Success", "This nickname is now registered!", "Cool");
				} else {
					Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Success", "Failed to register. Try with another nickname or contact server management if the problem persists.", "OK");
					
					Kick(playerid);
				}
			}
		}
		
		Dialog_ShowCallback(playerid, using inline RegisterResponse, DIALOG_STYLE_PASSWORD, "Register", "You must register this nickname before playing.\n\nEnter a password:", "OK", "Cancel");
	}
}

stock this.RegisterUserVariable(const _name[], tag, size, ...) {
	new tagname[32], name[64];
	
	strunpack(name, _name);
	
	PBP.ResolveSymbolName(name);
	
	if (tag == tagof(Float:))
		tagname = "Float";
	else
		tagname = "_";
	
	printf("%s:%s[][%d]", tagname, name, size);
}