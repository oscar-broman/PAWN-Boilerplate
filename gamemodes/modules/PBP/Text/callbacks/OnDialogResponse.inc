/*!
 * PBP/Text/callbacks/OnDialogResponse.inc
 *
 * OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
 *
 > Priority: 0
 */

{
	switch (dialogid) {
		case this.DIALOG_LANGUAGE_SELECTION: {
			Bit_Let(this.HasSelectedLanguage, playerid);
			
			if (response == 1) {
				if (this.DefaultLanguage != 0)
					listitem++;
				
				this.SetPlayerLanguage(playerid, listitem);
			}

			CallRemoteFunction(!"OnPlayerSelectedLanguage", !"ii", playerid, this.PlayerLanguage);
		}
	}
}