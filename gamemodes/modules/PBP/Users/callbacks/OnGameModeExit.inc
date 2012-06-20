/*!
 * Users/callbacks/OnGameModeExit.inc
 *
 * OnGameModeExit()
 *
 > Priority: 0
 */

{
	if (this.DB)
		db_free_persistent(this.DB);
}