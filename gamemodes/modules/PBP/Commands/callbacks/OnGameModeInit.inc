/*!
 * PBP/Commands/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 9
 */

{
	Text.SetActiveLanguage(0);
	
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@pD_")); ) {
		new description, cmd;
		
		description = CallLocalFunctionEx(buf);
		
		strdel(buf, 0, 4), strunpack(buf, buf);
		
		cmd = Command_GetID(buf);
		
		if (cmd == COMMAND_NOT_FOUND) {
			printf(!"(Commands) WARNING: Unable to find command \"%s\".", buf);

			continue;
		}
		
		this.Description[cmd] = description;
	}
	
	Text.SetActiveLanguage(-1);

	// Search for permission setting functions
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@pC_")); ) {
		// Calling the function will set up the permissions for that command (see the YCMD re-definition).
		CallLocalFunctionEx(buf);
	}
	
	// Search for module index pubvars
	for (new buf[32], idx; (idx = AMX_GetPubvarName(idx, buf, !"@pc_")); ) {
		new cmd, module;
		
		strunpack(buf, buf);
		
		module = @ptr[GetPubVarAddressByName(buf)];
		
		strdel(buf, 0, 4);
		
		cmd = Command_GetID(buf);
		
		if (cmd == COMMAND_NOT_FOUND) {
			printf(!"(Commands) WARNING: Unable to find command \"%s\".", buf);

			continue;
		}
		
		this.Module[cmd] = module;
	}
	
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@_yC")); ) {
		strdel(buf, 0, 4), strunpack(buf, buf);
		
		new cmd = Command_GetID(buf);
		
		if (cmd == COMMAND_NOT_FOUND)
			continue;
		
		if (this.Module[cmd] == -1) {
			printf(!"(Commands) WARNING: No module info given for command \"%s\".", buf);
		}
	}
}