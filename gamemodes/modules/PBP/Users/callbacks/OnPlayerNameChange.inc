/*!
 * PBP/Users/callbacks/OnPlayerNameChange.inc
 *
 * OnPlayerNameChange(playerid, const old_name[])
 *
 > Priority: 0
 */

{
	if (this.IsLoggedIn(playerid))
		this.RenameAccount(playerid, Player.Info[playerid][Name]);
}