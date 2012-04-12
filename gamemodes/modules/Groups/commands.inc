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

YCMD:test(playerid, params[], help) {
	SendClientMessage(playerid, -1, "test");
	
	return 1;
}

YCMD(VIP, MODERATOR):test2(playerid, params[], help) {
	SendClientMessage(playerid, -1, "test2");
	
	return 1;
}