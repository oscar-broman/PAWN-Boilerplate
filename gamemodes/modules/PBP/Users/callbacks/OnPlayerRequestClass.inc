/*!
 * PBP/Users/callbacks/OnPlayerRequestClass.inc
 *
 * OnPlayerRequestClass(playerid, classid)
 *
 > Priority: 0
 */

{
	if (!Text.ShowsLanguageSelection() && Dialog_Get(playerid) == -1)
		this.ShowDialogIfNeeded(playerid);
}