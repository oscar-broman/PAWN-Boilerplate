/*!
 * PBP/Users/callbacks/OnPlayerConnect.inc
 *
 * OnPlayerConnect(playerid)
 *
 > Priority: 8
 */

{
	this.PlayerUID[playerid] = INVALID_UID;
	this.PlayerNameUID[playerid] = this.FindAccountByName(Player.Info[playerid][Name]);
	this.LogInAttempts[playerid] = 0;
	this.PlayerIsLoggedIn{playerid} = false;
	
	Bit_Vet(this.InAuthDialog, playerid);
	
	// Clear all user variables
	for (new i = 0; i < this.NumUserVariables; i++) {
		switch (this.UserVariables[i][Type]) {
			case this.TYPE_INT,
			     this.TYPE_FLOAT:
				@ptr[this.UserVariables[i][Address]][playerid] = 0;
			
			case this.TYPE_STRING:
				@ptr[this.UserVariables[i][Address], playerid] = '\0';
				
			case this.TYPE_MALLOC_STRING: {
				new Alloc:alloc = Alloc:@ptr[this.UserVariables[i][Address]][playerid];
				
				if (alloc)
					free(alloc);
				
				@ptr[this.UserVariables[i][Address]][playerid] = 0;
			}
			
			case this.TYPE_Y_BIT: {
				Bit_Set(Bit:@ptr[this.UserVariables[i][Address]], playerid, false);
			}
			
			case this.TYPE_CHAR: {
				@ptr[this.UserVariables[i][Address]]{playerid} = 0;
			}
				
			default:
				continue;
		}
	}
	
	new client_id[41];
	
	GetPlayerClientId(playerid, client_id);
	
	new DBResult:result = db_query(
		this.DB,
		"SELECT COUNT(*) FROM `users` WHERE `uid` = ? AND `last_ip` = ? AND `last_cid` = x'?'",
		       INT:this.PlayerNameUID[playerid],
		      UINT:Player.Info[playerid][IP],
		RAW_STRING:client_id
	);
	
	if (db_get_field_int(result) == 1) {
		SetTimerEx(#this.AutoLogPlayerIn, 0, false, !"i", playerid);
	}
}