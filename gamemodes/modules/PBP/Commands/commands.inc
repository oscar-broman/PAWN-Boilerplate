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

CommandDescription<help> = @"View information about a certain command.";

YCMD:help(playerid, params[], help) {
	if (help) {
		SendClientMessage(playerid, COLOR_COMMAND_HELP, @"* View information about a certain command.");

usage:
        SendClientMessage(playerid, COLOR_COMMAND_USAGE, @"* Usage: /help [command]");
	} else {
		if (isnull(params))
			goto usage;
		
		new cmd, idx = 0;
		
		if (params[0] == '/')
			idx = 1;
		
		cmd = Command_GetID(params[idx]);
		
		if (cmd == COMMAND_NOT_FOUND || !Command_ReProcess(playerid, params[idx], true)) {
			SendClientMessage(playerid, COLOR_COMMAND_ERROR, @"* Invalid command.");
			
			return 1;
		}
	}
	
	return 1;
}