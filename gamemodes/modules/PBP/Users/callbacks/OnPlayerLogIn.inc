/*!
 * PBP/Users/callbacks/OnPlayerLogIn.inc
 *
 * OnPlayerLogIn(playerid, bool:autologin)
 *
 > Priority: 10
 */

{
	this.PlayerIsLoggedIn{playerid} = true;
	
	if (Bit_Get(this.InAuthDialog, playerid)) {
		Bit_Vet(this.InAuthDialog, playerid);
		
		ShowPlayerDialog(playerid, -1, 0, NULL, NULL, NULL, NULL);
	}
	
	if (autologin)
		SendClientMessage(playerid, COLOR_GENERIC_INFO, @"* You were automatically logged in.");
	else
		Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, @"Success", @"You are now logged in!", @"Cool");
	
	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
	
	new client_id[41];
	
	GetPlayerClientId(playerid, client_id);
	
	db_query(
		this.DB,
		"UPDATE `users` SET `last_ip` = ?, `last_cid` = x'?' WHERE `uid` = ?",
		      UINT:Player.Info[playerid][IP],
		RAW_STRING:client_id,
		       INT:this.PlayerUID[playerid]
	);
}