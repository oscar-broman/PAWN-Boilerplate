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
	
	printf(@"Hello world!");
	printf(@"What's up?"<"Greeting message">);
	printf(@"I like \"quotes\".");
	printf(@"Also new\nlines, and\ttabs.");
	
	this.LoadLanguage("languages/sv.lang.inc");
}