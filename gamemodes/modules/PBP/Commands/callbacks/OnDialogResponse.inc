/*!
 * PBP/Commands/callbacks/OnDialogResponse.inc
 *
 * OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
 *
 > Priority: 0
 */

{
	switch (dialogid) {
		case this.DIALOG_MODULE_SELECTION: {
			if (response == 1) {
				new selected_module = -1;

				for (new i = 0; i < sizeof(PBP.Modules); i++) {
					if (Bit_Get(this.VisibleModules[playerid], i)) {
						if (listitem == ++selected_module) {
							this.ShowCommandsDialog(playerid, i);

							break;
						}
					}
				}
			}
		}
		
		case this.DIALOG_COMMAND_LIST: {
			new current_page = GetPVarInt(playerid, #this.Page);
			new current_module = GetPVarInt(playerid, #this.Module);

			if (response == 1 && GetPVarInt(playerid, #this.MorePages)) {
				this.ShowCommandsDialog(playerid, current_module, current_page + 1);
			} else {
				if (current_page > 0)
					this.ShowCommandsDialog(playerid, current_module, current_page - 1);
				else
					this.ShowCommandsDialog(playerid);
			}
		}
	}
}