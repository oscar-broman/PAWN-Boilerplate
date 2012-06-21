/*!
 * PBP/Users/callbacks/OnConfigValueChange.inc
 *
 * OnConfigValueChange(const key[], key_hash)
 *
 > Priority: 0
 */

{
	switch (key_hash) {
		case _H<this.DatabaseName>: {
			print(!"(Users) The new database will be used when the server restarts.");
		}
	}
}