/*!
 * Users/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	this.DB = db_open_persistent(this.DatabaseName);
	
	if (!this.DB) {
		printf(!"(Users) Unable to open the database (\"%S\")!", this.DatabaseName);
	} else {
		db_set_asynchronous(this.DB);
		
		// Compare the DB table structure to this.Tables and this.Columns, create any non-existing columns.
		for (new i = 0; i < sizeof(this.Tables); i++) {
			new DBResult:result = db_query(this.DB, !"PRAGMA table_info(?)", STRING:this.Tables[i][Name]);
		
			if (!result || !db_num_rows(result)) {
				new bool:first = true, query[1024 char] = !"CREATE TABLE IF NOT EXISTS ? (\n\t";
				
				for (new j = 0; j < sizeof(this.Columns); j++) {
					if (this.Columns[j][Table] != i)
						continue;
					
					if (first)
						first = false;
					else
						strcat(query, !",\n\t");
					
					strcatf(query, _, !"[%s] %s %s", this.Columns[j][Name], this.Columns[j][Type], this.Columns[j][Constraint]);
					
					if (!isnull(this.Columns[j][Default]))
						strcatf(query, _, !" DEFAULT(%s)", this.Columns[j][Default]);
				}
				
				strcat(query, !"\n)");
				
				// Set the initial rowid
				if (db_query(this.DB, query, IDENTIFIER:this.Tables[i][Name])) {
					db_query(
						this.DB, !"INSERT OR REPLACE INTO `sqlite_sequence` (`rowid`, `seq`, `name`)  VALUES((SELECT `rowid` FROM `sqlite_sequence` WHERE `name` = ?), ? - 1, ?)",
						STRING:this.Tables[i][Name],
						INTEGER:this.Tables[i][AutoIncrement],
						STRING:this.Tables[i][Name]
					);
					
					printf(!"(Users) Created table: \"%S\".", this.Tables[i][Name]);
				} else {
					printf(!"(Users) Failed to create table: \"%S\".", this.Tables[i][Name]);
				}
			} else {
				new bool:column_exists[sizeof(this.Columns)];
				
				do {
					new column_name[16];
					
					db_get_field_assoc(result, "name", column_name);
					
					for (new j = 0; j < sizeof(this.Columns); j++) {
						if (this.Columns[j][Table] != i)
							continue;
						
						if (strequals(column_name, this.Columns[j][Name])) {
							column_exists[j] = true;
							
							break;
						}
					}
				} while (db_next_row(result));
				
				for (new j = 0; j < sizeof(this.Columns); j++) {
					if (this.Columns[j][Table] != i)
						continue;
					
					if (!column_exists[j]) {
						if (-1 != strfind(this.Columns[j][Constraint], !"NOT NULL", true) && (!strlen(this.Columns[j][Default]) || -1 != strfind(this.Columns[j][Default], !"NULL", true))) {
							printf(!"(Users) ERROR: Cannot add a NOT NULL column with default value NULL (\"%s.%s\").", this.Tables[i][Name], this.Columns[j][Name]);
							
							continue;
						}
						
						new query[256 char] = !"ALTER TABLE ? ADD COLUMN ? ? ?";
						
						if (!isnull(this.Columns[j][Default]))
							strcatf(query, _, !" DEFAULT(%s)", this.Columns[j][Default]);
						
						result = db_query(
							this.DB, query,
							IDENTIFIER:this.Tables[i][Name],
							IDENTIFIER:this.Columns[j][Name],
							RAW_STRING:this.Columns[j][Type],
							RAW_STRING:this.Columns[j][Constraint]
						);
						
						if (result)
							printf(!"(Users) Created column: \"%s.%s\".", this.Tables[i][Name], this.Columns[j][Name]);
						else
							printf(!"(Users) Failed to create column: \"%s.%s\".", this.Tables[i][Name], this.Columns[j][Name]);
					}
				}
			}
		}
	}
	
	for (new buf[32], idx; (idx = AMX_GetPublicName(idx, buf, !"@Pu_")); ) {
		if (strfind(buf, !".") != -1)
			continue;
		
		CallLocalFunction(buf, !"");
	}
}