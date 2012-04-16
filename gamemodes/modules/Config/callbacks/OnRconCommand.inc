/*!
 * Config/callbacks/OnRconCommand.inc
 *
 * OnRconCommand(cmd[])
 *
 > Priority: 0
 */

{
	if (!isnull(cmd) && !strcmp(cmd, !"reload_config", true)) {
		this.LoadEntries();
		
		printf(!"Reloaded the configuration from \"%S\".", this.FILE);
	}
}