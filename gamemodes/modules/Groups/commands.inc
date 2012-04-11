/*!
 * Groups/commands.inc
 *
 > Priority: 0
 */

/*YCMD:commands(playerid, params[], help) {
	if (help) {
		SendClientMessage(playerid, 0xFF0000AA, "Lists all the commands a player can use.");
	} else {
		new
			count = Command_GetPlayerCommandCount(playerid),
			buf[64]
		;
		
		for (new i = 0; i < count; i++) {
			strunpack(buf, Command_GetNext(i, playerid));
#error TODO
			
			SendClientMessage(playerid, 0xFF0000AA, Command_GetNext(i, playerid));
		}
	}
	return 1;
}*/