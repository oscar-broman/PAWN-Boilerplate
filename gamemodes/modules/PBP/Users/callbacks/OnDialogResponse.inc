/*!
 * PBP/Users/callbacks/OnDialogResponse.inc
 *
 * OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
 *
 > Priority: 0
 */

{
	if (this.OFFSET <= dialogid <= this.MAX_DIALOG_ID)
		Bit_Vet(this.InAuthDialog, playerid);
	
	if (!this.IsLoggedIn(playerid)) {
		switch (dialogid) {
			case this.DIALOG_LOG_IN_QUIT_CONFIRM: {
				if (response)
					Kick(playerid);
				else
					this.ShowDialogIfNeeded(playerid);
			}
		
			case this.DIALOG_LOG_IN_FAILED: {
				this.ShowDialogIfNeeded(playerid);
			}
		
			case this.DIALOG_LOG_IN: {
				if (!response) {
					ShowPlayerDialog(playerid, this.DIALOG_LOG_IN_QUIT_CONFIRM, DIALOG_STYLE_MSGBOX, @"Quit", @"Are you sure you want to quit the server?", @"Quit", @"Cancel");
					
					Bit_Let(this.InAuthDialog, playerid);
				} else {
					if (isnull(inputtext)) {
						this.ShowDialogIfNeeded(playerid);
					} else if (this.LogPlayerIn(playerid, inputtext)) {
					
					} else {
						if (++this.LogInAttempts[playerid] >= this.MaxLogInAttempts) {
							SendClientMessage(playerid, COLOR_GENERIC_ERROR, @"* You were kicked from the server because of too many failed log in attempts.");
						
							Kick(playerid);
						} else {
							new msg[80], attempts = this.MaxLogInAttempts - this.LogInAttempts[playerid];
						
							format(msg, _, @"Invalid password. You have %d more %s, then you'll get kicked.", attempts, attempts == 1 ? (@"attempt") : (@"attempts"<"Plural of attempt">));
						
							ShowPlayerDialog(playerid, this.DIALOG_LOG_IN_FAILED, DIALOG_STYLE_MSGBOX, @"Log in failed", msg, @"Retry", "");
							
							Bit_Let(this.InAuthDialog, playerid);
						}
					}
				}
			}
		
			case this.DIALOG_REGISTER_QUITCONFIRM: {
				if (response)
					Kick(playerid);
				else
					this.ShowDialogIfNeeded(playerid);
			}
		
			case this.DIALOG_REGISTER: {
				if (!response) {
					ShowPlayerDialog(playerid, this.DIALOG_REGISTER_QUITCONFIRM, DIALOG_STYLE_MSGBOX, @"Quit", @"Are you sure you want to quit the server?", @"Quit", @"Cancel");
					
					Bit_Let(this.InAuthDialog, playerid);
				} else {
					if (isnull(inputtext)) {
						this.ShowDialogIfNeeded(playerid);
					} else if (this.RegisterPlayer(playerid, inputtext)) {
						ShowPlayerDialog(playerid, 0, DIALOG_STYLE_MSGBOX, @"Success", @"This nickname is now registered!", @"Cool", "");
						
						Bit_Let(this.InAuthDialog, playerid);
					} else {
						ShowPlayerDialog(playerid, 0, DIALOG_STYLE_MSGBOX, @"Success", @"Failed to register. Try with another nickname or contact server management if the problem persists.", @"OK", "");
						
						Kick(playerid);
					}
				}
			}
		}
	}
}