/*!
 * PBP/Commands/functions.inc
 *
 > Priority: 0
 */

stock this.SetupPermissions(command[], Group:...) {
	new cmd = Command_GetID(command);

	if (cmd == COMMAND_NOT_FOUND) {
		new buf[32];

		strunpack(buf, command);

		printf("(Groups) WARNING: Unable to find command \"%s\".", buf);

		return;
	}

	Group_SetGlobalCommand(cmd, false);

	for (new i = numargs() - 1; i >= 1; i--) {
		new Group:group = Group:getarg(i);

		Group_SetCommand(group, cmd, true);
	}
}

stock this.PadWithTabsForMsgbox(string[], tabs, maxlength = sizeof(string)) {
	new width = 0;
	
	for (new l = strlen(string), i = 0; i < l; i++) {
		switch (string[i]) {
			case '1': width += 5;
			case 'f': width += 3;
			case 'i': width += 3;
			case 'j': width += 4;
			case 'l': width += 3;
			case 'm': width += 9;
			case 'r': width += 4;
			case 't': width += 3;
			case 'v': width += 7;
			case 'w': width += 9;
			case 'y': width += 5;
			case 'z': width += 5;
			case ' ': width += 3;
			case '/': width += 3;
			default : width += 6;
		}
	}
	
	tabs -= width / 40;
	
	for (new i = 0; i < tabs; i++)
		strcat(string, "\t", maxlength);
}

stock this.ShowCommandsDialog(playerid, module = -1, page = 0) {
	static
		info[4096]
	;
	
	inline ModuleSelectionResponse(_playerid, dialogid, response, listitem, string:inputtext[]) {
		#pragma unused _playerid, dialogid, inputtext
		
		Text.SetActivePlayer(playerid);
		
		if (response == 1) {
			new selected_module = -1;
			
			for (new i = 0; i < sizeof(PBP.Modules); i++) {
				if (Bit_Get(this.VisibleModules[playerid], i)) {
					if (listitem == ++selected_module) {
						this.ShowCommandsDialog(playerid, i);
						
						return;
					}
				}
			}
		}
	}
	
	inline CommandListResponse(_playerid, dialogid, response, listitem, string:inputtext[]) {
		#pragma unused _playerid, dialogid, listitem, inputtext
		
		Text.SetActivePlayer(playerid);
		
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
	
	if (module == -1) {
		new
			     buf[32],
			bool:has_commands[sizeof(PBP.Modules)],
			     cmd,
			     cmds = Command_GetPlayerCommandCount(playerid)
		;
		
		for (new i = 0; i < cmds; i++) {
			strunpack(buf, Command_GetNext(i, playerid));
			
			if ((cmd = Command_GetID(buf)) == COMMAND_NOT_FOUND || this.Module[cmd] == -1)
				continue;
			
			has_commands[this.Module[cmd]] = true;
		}
		
		info[0] = '\0';
		
		for (new i = 0; i < sizeof(PBP.Modules); i++) {
			Bit_Set(this.VisibleModules[playerid], i, has_commands[i]);
			
			if (!has_commands[i])
				continue;
			
			strcatf(info, _, "- %s\n", PBP.Modules[i][Name]);
		}
		
		Dialog_ShowCallback(playerid, using inline ModuleSelectionResponse, DIALOG_STYLE_LIST, @"Commands", info, @"OK", @"Close");
	} else {
		new
			     buf[32],
			     cmd,
			     cmds = Command_GetPlayerCommandCount(playerid),
			     current = 0,
			     visible = 0,
			bool:more_pages = false
		;
		
		info = "{FFFFFF}";
		
		strcat(info, @"Commands");
		strcat(info, "\n{666666}_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\n\n");
		
		for (new i = 0; i < cmds; i++) {
			strunpack(buf, Command_GetNext(i, playerid));
			
			if ((cmd = Command_GetID(buf)) == COMMAND_NOT_FOUND || this.Module[cmd] != module)
				continue;
			
			if (++current <= this.CommandsPerPage * page)
				continue;
			
			if (++visible > this.CommandsPerPage) {
				more_pages = true;
				
				break;
			}
			
			strins(buf, "/", 0);
			
			this.PadWithTabsForMsgbox(buf, 2);
			
			strcatf(info, _, "{DDDDDD}/{FFFFFF}%s", buf[1]);
			
			if (this.Description[cmd])
				strcatf(info, _, " {DDDDDD}- %@", @ptr[this.Description[cmd]]);
			
			strcat(info, "\n");
		}
		
		for (new i = this.CommandsPerPage - visible; i > 0; i--) {
			strcat(info, "\n");
		}
		
		SetPVarInt(playerid, #this.Module, module);
		SetPVarInt(playerid, #this.Page, page);
		SetPVarInt(playerid, #this.MorePages, more_pages);
		
		Dialog_ShowCallback(playerid, using inline CommandListResponse, DIALOG_STYLE_MSGBOX, @"Commands", info, more_pages ? @"Next >" : @"< Back", more_pages ? @"< Back" : "");
	}
}