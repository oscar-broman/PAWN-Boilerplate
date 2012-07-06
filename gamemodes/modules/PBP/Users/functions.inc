/*!
 * PBP/Users/functions.inc
 *
 > Priority: 11
 */

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
	
	static DBStatement:stmt =: db_prepare(this.DB, !"INSERT INTO `users` (`name`, `passhash`) VALUES (?, ?)");
	
	stmt_bind_value(stmt, 0, DB::TYPE_STRING,  name);
	stmt_bind_value(stmt, 1, DB::TYPE_WP_HASH, password);
	
	if (stmt_execute(stmt)) {
		new UID:uid = UID:db_last_insert_rowid(this.DB);
		
		if (_:uid >= MINIMUM_UID)
			return uid;
	}
	
	return INVALID_UID;
}

stock bool:this.RenameAccount({UID, _}:uid, const new_name[]) {
	if (!this.ResolveUID(uid) || isnull(new_name))
		return false;
	
	static DBStatement:stmt =: db_prepare(this.DB, !"UPDATE `users` SET `name` = ? WHERE `uid` = ?");
	
	stmt_bind_value(stmt, 0, DB::TYPE_STRING, new_name);
	stmt_bind_value(stmt, 1, DB::TYPE_INT, _:uid);
	
	return !!stmt_execute(stmt);
}

stock bool:this.ChangeAccountPassword({UID, _}:uid, const new_password[]) {
	if (!this.ResolveUID(uid) || isnull(new_password))
		return false;
	
	static DBStatement:stmt =: db_prepare(this.DB, !"UPDATE `users` SET `passhash` = ? WHERE `uid` = ?");
	
	stmt_bind_value(stmt, 0, DB::TYPE_WP_HASH, new_password);
	stmt_bind_value(stmt, 1, DB::TYPE_INT, _:uid);
	
	return !!stmt_execute(stmt);
}

stock this.SaveAccount({UID, _}:_playerid) {
	new playerid = _:_playerid;
	
	if (!this.ResolvePlayerID(playerid) || this.PlayerUID[playerid] == INVALID_UID)
		return;
	
	CallLocalFunction(!"OnPlayerAccountBeingSaved", !"i", playerid);
	
	static DBStatement:stmt =: db_prepare(this.DB, !"INSERT OR REPLACE INTO users_info (idx, uid, key, value) VALUES ((SELECT idx FROM users_info WHERE uid = ? AND key = ?), ?, ?, ?)");
	
	db_begin_transaction(this.DB);
	
	stmt_bind_value(stmt, 0, DB::TYPE_INT, _:this.PlayerUID[playerid]);
	stmt_bind_value(stmt, 2, DB::TYPE_INT, _:this.PlayerUID[playerid]);
	
	for (new i = 0; i < this.NumUserVariables; i++) {
		stmt_bind_value(stmt, 1, DB::TYPE_STRING, mstr(this.UserVariables[i][Key]));
		stmt_bind_value(stmt, 3, DB::TYPE_STRING, mstr(this.UserVariables[i][Key]));
		
		switch (this.UserVariables[i][Type]) {
			case this.TYPE_INT:
				stmt_bind_value(stmt, 4, DB::TYPE_INT, @ptr[this.UserVariables[i][Address]][playerid]);
				
			case this.TYPE_FLOAT:
				stmt_bind_value(stmt, 4, DB::TYPE_FLOAT, @ptr[this.UserVariables[i][Address]][playerid]);
				
			case this.TYPE_STRING:
				stmt_bind_value(stmt, 4, DB::TYPE_STRING, @ptr[this.UserVariables[i][Address], playerid]);
				
			case this.TYPE_MALLOC_STRING: {
				new Alloc:alloc = Alloc:@ptr[this.UserVariables[i][Address]][playerid];
				
				if (alloc)
					stmt_bind_value(stmt, 4, DB::TYPE_STRING, mstr(alloc));
				else
					stmt_bind_value(stmt, 4, DB::TYPE_STRING, "");
			}		
			
			case this.TYPE_Y_BIT: {
				stmt_bind_value(stmt, 4, DB::TYPE_INT, !!Bit_Get(Bit:@ptr[this.UserVariables[i][Address]], playerid));
			}
			
			case this.TYPE_CHAR: {
				stmt_bind_value(stmt, 4, DB::TYPE_INT, @ptr[this.UserVariables[i][Address]]{playerid});
			}
				
			default:
				continue;
		}
		
		stmt_execute(stmt);
	}
	
	db_end_transaction(this.DB);
}

/*stock this.DeleteAccount({UID, _}:uid) {
	
}*/

stock bool:this.LogPlayerIn(playerid, const password[]) {
	if (!this.IsRegistered(playerid) || this.IsLoggedIn(playerid))
		return false;
	
	if (!this.TestAccountAuth(playerid, password))
		return false;
	
	static DBStatement:stmt =: db_prepare(this.DB, !"SELECT key, value FROM users_info WHERE uid = ?");
	
	this.PlayerUID[playerid] = this.PlayerNameUID[playerid];
	
	stmt_bind_value(stmt, 0, DB::TYPE_INT, _:this.PlayerUID[playerid]);
	
	new key[64];
	static value[4096];
	
	stmt_bind_result_field(stmt, 0, DB::TYPE_STRING, key, sizeof(key));
	stmt_bind_result_field(stmt, 1, DB::TYPE_STRING, value, sizeof(value));
	
	if (stmt_execute(stmt)) {
		while (stmt_fetch_row(stmt)) {
			new key_hash = YHash(key);
			
			// Clear all user variables
			for (new i = 0; i < this.NumUserVariables; i++) {
				if (key_hash != this.UserVariables[i][KeyHash] || strcmp(key, mstr(this.UserVariables[i][Key])))
					continue;
				
				switch (this.UserVariables[i][Type]) {
					case this.TYPE_INT:
						@ptr[this.UserVariables[i][Address]][playerid] = strval(value);
					
					case this.TYPE_FLOAT:
						@ptr[this.UserVariables[i][Address]][playerid] = _:floatstr(value);

					case this.TYPE_STRING:
						strunpack(@ptr[this.UserVariables[i][Address], playerid], value, this.UserVariables[i][Size]);

					case this.TYPE_MALLOC_STRING: {
						new Alloc:alloc = Alloc:@ptr[this.UserVariables[i][Address]][playerid];

						if (alloc)
							free(alloc);

						@ptr[this.UserVariables[i][Address]][playerid] = _:Malloc_NewS(value);
					}
					
					case this.TYPE_Y_BIT: {
						Bit_Set(Bit:@ptr[this.UserVariables[i][Address]], playerid, strval(value) || false);
					}
					
					case this.TYPE_CHAR: {
						@ptr[this.UserVariables[i][Address]]{playerid} = strval(value);
					}
				}
				
				break;
			}
		}
	}
	
	SetTimerEx(!"OnPlayerLogIn", 0, false, !"ii", playerid, false);
	
	return true;
}

stock bool:this.TestAccountAuth({UID, _}:uid, const password[]) {
	if (!this.ResolveUID(uid, false))
		return false;
	
	new bool:success = false;
	
	static DBStatement:stmt =: db_prepare(this.DB, !"SELECT EXISTS(SELECT `uid` FROM `users` WHERE `uid` = ? AND `passhash` = ?)");
	
	stmt_bind_value(stmt, 0, DB::TYPE_INT, _:uid);
	stmt_bind_value(stmt, 1, DB::TYPE_WP_HASH, password);
	
	stmt_bind_result_field(stmt, 0, DB::TYPE_INT, success);
	
	stmt_execute(stmt) && stmt_fetch_row(stmt);
	
	return success;
}

stock UID:this.FindAccountByName(const name[]) {
	new UID:uid = INVALID_UID;
	
	static DBStatement:stmt =: db_prepare(this.DB, !"SELECT `uid` FROM `users` WHERE `name` = ?");
	
	stmt_bind_value(stmt, 0, DB::TYPE_STRING, name);
	
	stmt_bind_result_field(stmt, 0, DB::TYPE_INT, _:uid);
	
	stmt_execute(stmt) && stmt_fetch_row(stmt);
	
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
	SendClientMessage(playerid, COLOR_GENERIC_ERROR, @<playerid>"ERROR: Please log in first.");
	
	PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
}

stock this.ShowDialogIfNeeded(playerid) {
	if (this.IsLoggedIn(playerid)) {
		
	} else if (this.IsRegistered(playerid)) {
/*		inline LogInQuitConfirmResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			Text.SetActivePlayer(playerid);
			
			if (response)
				Kick(playerid);
			else
				this.ShowDialogIfNeeded(playerid);
		}*/
		
/*		inline LogInFailedResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			Text.SetActivePlayer(playerid);
			
			this.ShowDialogIfNeeded(playerid);
		}*/
		
/*		inline LogInResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			Text.SetActivePlayer(playerid);
			
			if (!response)
				Dialog_ShowCallback(playerid, using inline LogInQuitConfirmResponse, DIALOG_STYLE_MSGBOX, @"Quit", @"Are you sure you want to quit the server?", @"Quit", @"Cancel");
			else {
				if (isnull(inputtext)) {
					this.ShowDialogIfNeeded(playerid);
				} else if (this.LogPlayerIn(playerid, inputtext)) {
					
				} else {
					if (++this.LogInAttempts[playerid] >= this.MaxLogInAttempts) {
						SendClientMessage(playerid, COLOR_GENERIC_ERROR, @"* You were kicked from the server because of too many failed log in attempts.");
						
						Kick(playerid);
					} else {
						new msg[80], attempts = this.MaxLogInAttempts - this.LogInAttempts[playerid];
						
						format(msg, _, @"Invalid password. You have %d more %s, then you'll get kicked.", attempts, attempts == 1 ? (@"attempt") : (@"attempts"<"Plural of attempt">));
						
						Dialog_ShowCallback(playerid, using inline LogInFailedResponse, DIALOG_STYLE_MSGBOX, @"Log in failed", msg, @"Retry");
					}
				}
			}
		}*/
		
		ShowPlayerDialog(playerid, this.DIALOG_LOG_IN, DIALOG_STYLE_PASSWORD, @"Log in", @"This nickname is registered. Please log in or come back with another nickname.\n\nEnter your password:", @"OK", @"Cancel");
		//Dialog_ShowCallback(playerid, using inline LogInResponse, DIALOG_STYLE_PASSWORD, @"Log in", @"This nickname is registered. Please log in or come back with another nickname.\n\nEnter your password:", @"OK", @"Cancel");
	} else {
/*		inline RegisterQuitConfirmResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			Text.SetActivePlayer(playerid);
			
			if (response)
				Kick(playerid);
			else
				this.ShowDialogIfNeeded(playerid);
		}*/
		
/*		inline RegisterResponse(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, response, listitem, inputtext
			
			Text.SetActivePlayer(playerid);
			
			if (!response)
				Dialog_ShowCallback(playerid, using inline RegisterQuitConfirmResponse, DIALOG_STYLE_MSGBOX, @"Quit", @"Are you sure you want to quit the server?", @"Quit", @"Cancel");
			else {
				if (isnull(inputtext)) {
					this.ShowDialogIfNeeded(playerid);
				} else if (this.RegisterPlayer(playerid, inputtext)) {
					Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, @"Success", @"This nickname is now registered!", @"Cool");
				} else {
					Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, @"Success", @"Failed to register. Try with another nickname or contact server management if the problem persists.", @"OK");
					
					Kick(playerid);
				}
			}
		}*/
		
		ShowPlayerDialog(playerid, this.DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, @"Register", @"You must register this nickname before playing.\n\nEnter a password:", @"OK", @"Cancel");
		//Dialog_ShowCallback(playerid, using inline RegisterResponse, DIALOG_STYLE_PASSWORD, @"Register", @"You must register this nickname before playing.\n\nEnter a password:", @"OK", @"Cancel");
	}
}

stock this.RegisterUserVariable(const _key[], tag, size1, size2, ...) {
	new
		     key[64 char],
		     type,
		     i = this.NumUserVariables++
	;
	
	strpack(key, _key);
	
	PBP.ResolveSymbolName(key);
	
	if (tag == tagof(Float:)) {
		type = this.TYPE_FLOAT;
	} else if (tag == tagof(Alloc:)) {
		type = this.TYPE_MALLOC_STRING;
	} else if (tag == tagof(Bit:)) {
		type = this.TYPE_Y_BIT;
	} else if (size1 == MAX_PLAYERS char) {
		type = this.TYPE_CHAR;
	} else {
		type = this.TYPE_INT;
	}
	
	if (size2 > 1) {
		type = this.TYPE_STRING;
	}
	
	this.UserVariables[i][Key]     = Malloc_NewS(key, true);
	this.UserVariables[i][KeyHash] = YHash(key);
	this.UserVariables[i][Type]    = type;
	this.UserVariables[i][Size]    = (type == this.TYPE_STRING) ? size2 : size1;
	this.UserVariables[i][Address] = GetArgumentAddress(4);
}

// Hook SetPlayerName
stock this.SetPlayerName(playerid, name[]) {
	if (this.FindAccountByName(name) != INVALID_UID)
		return 0;
	
	return SetPlayerName(playerid, name);
}

#if defined _ALS_SetPlayerName
	#undef SetPlayerName
#else
	#define _ALS_SetPlayerName
#endif
#define SetPlayerName Users.SetPlayerName