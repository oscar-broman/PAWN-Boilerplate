/*!
 * PBP/Config/commands.inc
 *
 > Priority: 0
 */

CommandDescription<config> = @"Show the config editor.";

YCMD(GROUP_RCON, GROUP_MANAGEMENT):config(playerid, params[], help) {
	if (help) {
		SendClientMessage(playerid, COLOR_COMMAND_HELP, @"* Show the config editor.");
	} else {
		this.ShowEditorDialog(playerid);
	}
	
	return 1;
}