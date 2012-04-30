/*!
 * Commands/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
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
	
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@pG_")); ) {
		new
			      name[MAX_GROUP_NAME char],
			Group:group
		;

		group = Group_Create(NULL);

		// Load the group name and set the global variable's value
		CallLocalFunctionEx(buf, "si", name, group);

		Group_SetName(group, name);
	}

	// Search for permission setting functions
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@pC_")); ) {
		// Calling the function will set up the permissions for that command (see the YCMD re-definition).
		CallLocalFunctionEx(buf);
	}
}