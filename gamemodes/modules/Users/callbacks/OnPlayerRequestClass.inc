/*!
 * Users/callbacks/OnPlayerRequestClass.inc
 *
 * OnPlayerRequestClass(playerid, classid)
 *
 > Priority: 0
 */

{
	if (Dialog_Get(playerid) == -1)
		this.ShowDialog(playerid);
}