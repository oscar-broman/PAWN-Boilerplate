/*!
 * Text/callbacks/OnPlayerRequestClass.inc
 *
 * OnPlayerRequestClass(playerid, classid)
 *
 > Priority: 0
 */

{
	if (this.ShowsLanguageSelection() && !Bit_Get(this.HasSelectedLanguage, playerid)) {
		Bit_Let(this.HasSelectedLanguage, playerid);
		
		inline LanguageSelection(_playerid, dialogid, response, listitem, string:inputtext[])
		{
			#pragma unused _playerid, dialogid, inputtext
			
			if (response == 1)
				this.SetPlayerLanguage(playerid, listitem);
			
			CallRemoteFunction(!"OnPlayerSelectedLanguage", !"ii", playerid, this.PlayerLanguage);
		}
		
		{
			new langs[this.MAX_LANGUAGES * 16] = " ";
			
			for (new i = 0; i < this.Languages; i++) {
				if (i)
					strcat(langs, "\n");
				
				strcat(langs, this.LanguageCodes[this.LanguageNames[i]][1]);
			}
			
			Dialog_ShowCallback(playerid, using inline LanguageSelection, DIALOG_STYLE_LIST, "Select language", langs[1], "OK", "Cancel");
		}
	}
}