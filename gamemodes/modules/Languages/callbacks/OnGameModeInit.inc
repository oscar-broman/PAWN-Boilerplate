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
	
	for (i = 0; i < sizeof(this.LanguageCodes); i++) {
		new filename[32];
		
		format(filename, _, "languages/%L.lang.inc", this.LanguageCodes[i][0]);
		
		new lang = this.LoadLanguage(filename);
		
		if (lang != -1) {
			this.LanguageNames[lang] = i;
			
			printf("Loaded language \"%S\" from file \"%s\".", this.LanguageCodes[i][1], filename);
		}
	}this.PlayerLanguage[0]=1;
}