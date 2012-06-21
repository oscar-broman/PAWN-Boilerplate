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
				
			default:
				continue;
		}
	}
}