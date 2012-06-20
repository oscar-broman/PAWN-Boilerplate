/*!
 * Users/callbacks/OnPlayerLogIn.inc
 *
 * OnPlayerLogIn(playerid, bool:autologin)
 *
 > Priority: 10
 */

{
	this.PlayerIsLoggedIn{playerid} = true;
	
	Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, @"Success", @"You are now logged in!", @"Cool");
	
	PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
}