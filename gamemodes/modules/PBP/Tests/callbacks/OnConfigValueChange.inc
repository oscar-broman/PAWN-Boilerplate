/*!
 * Tests/callbacks/OnConfigValueChange.inc
 *
 * OnConfigValueChange(const key[], key_hash)
 *
 > Priority: 0
 */

{
	printf("(Tests) Config value changed: %s (%X)", key, key_hash);
	
	switch (key_hash) {
		case _H<this.StringVariable>: {
			printf("(Tests) this.StringVariable = \"%S\".", this.StringVariable);
		}
		
		case _H<this.MallocStringVariable>: {
			printf("(Tests) this.MallocStringVariable = \"%S\".", mstr(this.MallocStringVariable));
		}
	}
}