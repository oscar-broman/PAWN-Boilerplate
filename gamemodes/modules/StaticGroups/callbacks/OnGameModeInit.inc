/*!
 * StaticGroups/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 10
 */

{
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
}