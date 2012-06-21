/*!
 * PBP/StaticGroups/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 10
 */

{
	Text.SetActiveLanguage(0);
	
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@pG_")); ) {
		new
			      name,
			Group:group
		;

		group = Group_Create(NULL);
		
		// Load the group name and set the global variable's value
		name = CallLocalFunction(buf, "i", _:group);
		
		Group_SetName(group, @ptr[name]);
	}
	
	Text.SetActiveLanguage(-1);
}