/*!
 * Config/callbacks/OnGameModeExit.inc
 *
 * OnGameModeExit()
 *
 > Priority: 0
 */

{
	this.SaveEntries();
	
	db_free_persistent(this.db);
}