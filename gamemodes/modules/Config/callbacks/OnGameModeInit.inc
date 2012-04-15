/*!
 * Config/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 10
 */

{
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@Pc_")); )
		CallLocalFunction(buf, "");
	
	this.LoadEntries();
}