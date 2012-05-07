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
		case _H<Tests.StringVariable>: {
			printf("(Tests) Tests.StringVariable = \"%S\".", this.StringVariable);
		}
	}
}