/*!
 * PBP/Text/commands.inc
 *
 > Priority: 0
 */

YCMD:language(playerid, params[], help) {
	if (help) {
		SendClientMessage(playerid, COLOR_COMMAND_HELP, @"* Set your language.");
	} else {
		if (!this.ShowsLanguageSelection())
			return 0;
		else {
			this.ShowLanguageSelection(playerid);
		}
	}
	
	return 1;
}