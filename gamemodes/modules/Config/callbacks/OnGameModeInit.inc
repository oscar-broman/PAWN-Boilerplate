/*!
 * Config/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 10
 */

{
	this.db = db_open_persistent(this.DB_FILE);
	
	if (!db_is_valid_persistent(this.db)) {
		printf(!"(Config) Error: Unable to open the database (%S).", this.DB_FILE);
	} else {
		db_set_synchronous(this.db, DB::SYNCHRONOUS_OFF);
		
		db_exec(this.db, !"CREATE TABLE IF NOT EXISTS `entries` (`id` INTEGER PRIMARY KEY, `key` TEXT, `value`)");
		
		this.stmt_load_entries = db_prepare(this.db, !"SELECT `id`, `key`, `value` FROM `entries`");
		this.stmt_save_entry = db_prepare(this.db, !"INSERT OR REPLACE INTO `entries` VALUES (?, ?, ?)");
		
		for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@Pc_")); )
			CallLocalFunction(buf, "");
		
		this.LoadEntries();
	}
}