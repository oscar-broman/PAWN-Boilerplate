/*!
 * PBP/Config/callbacks/OnDialogResponse.inc
 *
 * OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
 *
 > Priority: 0
 */

{
	switch (dialogid) {
		case this.DIALOG_CONFIG_MAIN: {
			if (response == 1) {
				this.ShowEditorDialog(playerid, listitem);
			}
		}
		
		case this.DIALOG_CONFIG_CATEGORY: {
			if (response == 1) {
				this.ShowEditorDialog(playerid, _, listitem);
			} else {
				this.ShowEditorDialog(playerid);
			}
		}
		
		case this.DIALOG_CONFIG_EDITOR: {
			if (response == 1) {
				if (!isnull(inputtext))
					this.SetValueFromString(GetPVarInt(playerid, #this.InEntry), inputtext);
			}
			
			DeletePVar(playerid, #this.InEntry);
			
			this.ShowEditorDialog(playerid, GetPVarInt(playerid, #this.InModule) | 0x80000000);
		}
	}
}