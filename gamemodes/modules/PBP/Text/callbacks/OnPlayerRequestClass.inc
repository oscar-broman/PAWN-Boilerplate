/*!
 * Text/callbacks/OnPlayerRequestClass.inc
 *
 * OnPlayerRequestClass(playerid, classid)
 *
 > Priority: 0
 */

{
	if (this.ShowsLanguageSelection() && !Bit_Get(this.HasSelectedLanguage, playerid)) {
		this.ShowLanguageSelection(playerid);
	}
}