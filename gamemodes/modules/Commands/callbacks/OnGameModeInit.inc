/*!
 * Commands/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 9
 */

{
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@pD_")); ) {
		new Alloc:description, cmd;
		
		description = Alloc:CallLocalFunctionEx(buf);
		
		strdel(buf, 0, 4), strunpack(buf, buf);
		
		cmd = Command_GetID(buf);
		
		if (cmd == COMMAND_NOT_FOUND) {
			printf("(Core) WARNING: Unable to find command \"%S\".", buf);

			continue;
		}
		
		this.Description[cmd] = description;
	}

	// Search for permission setting functions
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@pC_")); ) {
		// Calling the function will set up the permissions for that command (see the YCMD re-definition).
		CallLocalFunctionEx(buf);
	}
}