/*!
 * PBP/Commands/commands.inc
 *
 > Priority: 0
 */

CommandDescription<commands> = @"See a list of available commands.";

YCMD:commands(playerid, params[], help) {
	if (help) {
		SendClientMessage(playerid, COLOR_COMMAND_HELP, "* %s", @"See a list of available commands.");
	} else {
		this.ShowCommandsDialog(playerid);
	}
	
	return 1;
}