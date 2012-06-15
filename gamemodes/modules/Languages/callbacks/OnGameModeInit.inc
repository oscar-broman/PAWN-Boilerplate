/*!
 * Languages/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	assert(this.Languages == 0);
	
	new i = this.Languages++;
	
	{#LANG_DEFAULT_VALUES#}
	
	this.LoadLanguage("languages/sv.lang.inc");
	
	this.PlayerLanguage[0] = 1;
	
	printf(@"Hello world!");
	printf(@"What's up?"<"Greeting message">);
	printf(@"I like \"quotes\".");
	printf(@"Also new\nlines, and\ttabs.");
	
	new index = this.GetStringIndex(@"I like \"quotes\".");
	
	printf("index: %d", index);
	
	this.PlayerLanguage[0] = 0;
	
	index = this.GetStringIndex(@"I like \"quotes\".");
	
	printf("index: %d", index);
	
	printf(@"Hello world!");
	printf(@"What's up?"<"Greeting message">);
	printf(@"I like \"quotes\".");
	printf(@"Also new\nlines, and\ttabs.");
}