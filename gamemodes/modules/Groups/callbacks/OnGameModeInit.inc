/*!
 * Groups/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	for (new i = 0; i < sizeof(this.StaticGroups); i++)
		this.StaticGroups[i][Group] = Group_Create(this.StaticGroups[i][Name]);
	
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@pC_")); ) {
		new
			permissions = CallLocalFunction(buf, ""),
			cmd
		;
		
		strdel(buf, 0, 4);
		strunpack(buf, buf);
		
		cmd = Command_GetID(buf);
		
		if (cmd == COMMAND_NOT_FOUND) {
			printf("(Groups) WARNING: Unable to find the command \"%s\".", buf);
			
			continue;
		}
		
		Group_SetGlobalCommand(cmd, false);
		
		for (new i = 0; i < sizeof(this.StaticGroups); i++) {
			if (permissions & 1 << i)
				Group_SetCommand(this.StaticGroups[i][Group], cmd, true);
		}
	}
}