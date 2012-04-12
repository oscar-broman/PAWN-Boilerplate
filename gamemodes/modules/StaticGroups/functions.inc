/*!
 * StaticGroups/functions.inc
 *
 > Priority: 0
 */

stock this.SetCommandPermissions(command[], Group:...) {
	new cmd = Command_GetID(command);
	
	if (cmd == COMMAND_NOT_FOUND) {
		new buf[32];
		
		strunpack(buf, command);
		
		printf("(Groups) WARNING: Unable to find command \"%s\".", buf);
		
		return;
	}
	
	P:5("(Groups) Setting permissions for command \"%s\":", command);
	
	Group_SetGlobalCommand(cmd, false);
	
	for (new i = numargs() - 1; i >= 1; i--) {
		new Group:group = Group:getarg(i);
		
		Group_SetCommand(group, cmd, true);
		
		P:5("\t- %s", Group_GetName(group));
	}
}