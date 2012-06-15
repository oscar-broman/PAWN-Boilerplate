/*!
 * Languages/functions.inc
 *
 > Priority: 11
 */

stock this.LoadLanguage(const filename[]) {
	new File:fp;
	
	if (fexist(filename) && (fp = fopen(filename, io_read))) {
		new
			buf[2048 char],
			string[1024 char],
			description[1024 char],
			translation[1024 char],
			lang = this.Languages++,
			line = 0
		;
		
		while (fread(fp, buf, .pack = true)) {
			if (++line <= 2)
				continue;
			
			trim(buf, .edges = trim_right);
			
			if ((buf[0] & 0xFFFFFF00) == 0x2f2f2000) // if buf[0,1,2] == "// "
				strmid(description, buf, 3, cellmax);
			else if (buf{0} == '"') {
				new end = this.ParseStringLiteral(string, buf);
				new start = strfind(buf, "\"", _, end + 1);
				
				this.ParseStringLiteral(translation, buf, start);
				
				for (new i = 0; i < sizeof(this.Strings[]); i++) {
					if (strequals(this.Strings[0][i], string) && strequals(this.Descriptions[0][i], description)) {
						new len = strlen(translation);
						new Alloc:translation_slot = Malloc_Allocate(len + 2);
						
						strunpack(mstr(translation_slot)[1], translation, len + 1);
						
						RedirectArraySlot(this.Strings[lang], i, Malloc_GetAddress(translation_slot) + 4);
						
						mset(translation_slot, 0, (lang << 16) | i);
						
						if (IsNull(description))
							RedirectArraySlot(this.Descriptions[lang], i, ref(""));
						else
							RedirectArraySlot(this.Descriptions[lang], i, Malloc_GetAddress(Malloc_NewS(description)));
						
						break;
					}
				}
				
				description{0} = '\0';
			}
		}
		
		fclose(fp);
	}
}

stock this.GetStringIndex(const string[], &lang = 0) {
	new address = GetVariableAddress(string) - 4;
	new info = @ptr[address];
	new index = info & 0xFFFF;
	lang = info >>> 16;
	
	if (0 <= lang < sizeof(this.Strings) && 0 <= index < sizeof(this.Strings[])) {
		if (ref(this.Strings[lang][index]) == address + 4)
			return index;
	}
	
	return -1;
}

stock this.ParseStringLiteral(output[], const input[], start = 0) {
	new bool:in_escape = false;
	new i = 0, j, c;
	
	output{i} = '\0';
	
	for (j = start + 1; (c = input{j}); j++) {
		if (c == '\\') {
			if (in_escape) {
				in_escape = false;
				
				output{i++} = '\\';
			} else
				in_escape = true;
		} else if (in_escape) {
			switch (c) {
				case '"': output{i++} = c;
				case 'a': output{i++} = '\a';
				case 'b': output{i++} = '\b';
				case 'e': output{i++} = '\e';
				case 'r': output{i++} = '\r';
				case 'n': output{i++} = '\n';
				case 't': output{i++} = '\t';
				case 'v': output{i++} = '\v';
			}
			
			in_escape = false;
		} else {
			if (c == '"') {
				output{i++} = '\0';
				
				break;
			} else
				output{i++} = c;
		}
	}
	
	return j;
}

stock this.Prepare(forplayer = INVALID_PLAYER_ID) {
	if (forplayer == INVALID_PLAYER_ID) {
		if (this.PreviousPlayer != INVALID_PLAYER_ID) {
			this.CurrentPlayer = this.PreviousPlayer;
			this.PreviousPlayer = INVALID_PLAYER_ID;
		}
	} else {
		if (this.PreviousPlayer == INVALID_PLAYER_ID)
			this.PreviousPlayer = forplayer;

		this.CurrentPlayer = forplayer;
	}
	
	if (this.CurrentPlayer == INVALID_PLAYER_ID)
		this.ActiveLanguage = 0;
	else
		this.ActiveLanguage = this.PlayerLanguage[this.CurrentPlayer];
}

stock this.CheckStringArgument(arg, playerid) {
	new address, index, lang;
	
	#emit LOAD.S.pri  arg
	#emit SHL.C.pri   2
	#emit LOAD.S.alt  0
	#emit ADD
	#emit ADD.C       12
	#emit STOR.S.pri  arg
	#emit LOAD.I
	#emit STOR.S.pri  address
	
	index = this.GetStringIndex(@ptr[address], lang);

	// Either this isn't a translatable string or the language of the string is correct
	if (index == -1 || lang == this.PlayerLanguage[playerid])
		return;
	
	// Redirect the argument to the correct string
	@ptr[arg] = ref(this.Strings[this.PlayerLanguage[playerid]][index]);
}

// Hook all functions that sends text to players, add formatex support,
// and make sure the correct language is being sent.

/*
 * SendClientMessageToAll
 * ---------------------------------------------- */
stock this.SendClientMessageToAll(color, message[], va_args<>) {
	if (this.GetStringIndex(message) != -1) {
		if (numargs() > 2) {
			foreach (new playerid : Player) {
				this.CheckStringArgument(1, playerid);
				
				va_formatex(this.TextBuffer, _, message, va_start<2>);

				SendClientMessage(playerid, color, this.TextBuffer);
			}
		} else {
			foreach (new playerid : Player) {
				this.CheckStringArgument(1, playerid);
				
				SendClientMessage(playerid, color, message);
			}
		}
		
		return 1;
	}
	
	if (numargs() > 2) {
		va_formatex(this.TextBuffer, _, message, va_start<2>);

		return SendClientMessageToAll(color, this.TextBuffer);
	} else {
		return SendClientMessageToAll(color, message);
	}
}

#if defined _ALS_SendClientMessageToAll
	#undef SendClientMessageToAll
#else
	#define _ALS_SendClientMessageToAll
#endif
#define SendClientMessageToAll Languages.SendClientMessageToAll

/*
 * SendClientMessage
 * ---------------------------------------------- */
stock this.SendClientMessage(playerid, color, message[], va_args<>) {
	this.CheckStringArgument(2, playerid);
	
	if (numargs() > 3) {
		va_formatex(this.TextBuffer, _, message, va_start<3>);

		return SendClientMessage(playerid, color, this.TextBuffer);
	} else {
		return SendClientMessage(playerid, color, message);
	}
}

#if defined _ALS_SendClientMessage
	#undef SendClientMessage
#else
	#define _ALS_SendClientMessage
#endif
#define SendClientMessage Languages.SendClientMessage

/*
 * CreatePlayerTextDraw
 * ---------------------------------------------- */
stock PlayerText:this.CreatePlayerTextDraw(playerid, Float:x, Float:y, text[], va_args<>) {
	this.CheckStringArgument(3, playerid);
	
	if (numargs() > 4) {
		va_formatex(this.TextBuffer, _, text, va_start<4>);

		return CreatePlayerTextDraw(playerid, x, y, this.TextBuffer);
	} else {
		return CreatePlayerTextDraw(playerid, x, y, text);
	}
}

#if defined _ALS_CreatePlayerTextDraw
	#undef CreatePlayerTextDraw
#else
	#define _ALS_CreatePlayerTextDraw
#endif
#define CreatePlayerTextDraw Languages.CreatePlayerTextDraw

/*
 * PlayerTextDrawSetString
 * ---------------------------------------------- */
stock this.PlayerTextDrawSetString(playerid, PlayerText:text, string[], va_args<>) {
	this.CheckStringArgument(2, playerid);
	
	if (numargs() > 3) {
		va_formatex(this.TextBuffer, _, string, va_start<3>);
		
		return PlayerTextDrawSetString(playerid, text, this.TextBuffer);
	} else {
		return PlayerTextDrawSetString(playerid, text, string);
	}
}

#if defined _ALS_PlayerTextDrawSetString
	#undef PlayerTextDrawSetString
#else
	#define _ALS_PlayerTextDrawSetString
#endif
#define PlayerTextDrawSetString Languages.PlayerTextDrawSetString

/*
 * SendPlayerMessageToAll
 * ---------------------------------------------- */
stock this.SendPlayerMessageToAll(senderid, const message[], va_args<>) {
	if (this.GetStringIndex(message) != -1) {
		if (numargs() > 2) {
			foreach (new playerid : Player) {
				this.CheckStringArgument(1, playerid);
				
				va_formatex(this.TextBuffer, _, message, va_start<2>);

				SendPlayerMessageToPlayer(playerid, senderid, this.TextBuffer);
			}
		} else {
			foreach (new playerid : Player) {
				this.CheckStringArgument(1, playerid);
				
				SendPlayerMessageToPlayer(playerid, senderid, message);
			}
		}
		
		return 1;
	}
	
	if (numargs() > 2) {
		va_formatex(this.TextBuffer, _, message, va_start<2>);

		return SendPlayerMessageToAll(senderid, this.TextBuffer);
	} else {
		return SendPlayerMessageToAll(senderid, message);
	}
}

#if defined _ALS_SendPlayerMessageToAll
	#undef SendPlayerMessageToAll
#else
	#define _ALS_SendPlayerMessageToAll
#endif
#define SendPlayerMessageToAll Languages.SendPlayerMessageToAll

/*
 * SendPlayerMessageToPlayer
 * ---------------------------------------------- */
stock this.SendPlayerMessageToPlayer(playerid, senderid, const message[], va_args<>) {
	this.CheckStringArgument(2, playerid);
	
	if (numargs() > 3) {
		va_formatex(this.TextBuffer, _, message, va_start<3>);

		return SendPlayerMessageToPlayer(playerid, senderid, this.TextBuffer);
	} else {
		return SendPlayerMessageToPlayer(playerid, senderid, message);
	}
}

#if defined _ALS_SendPlayerMessageToPlayer
	#undef SendPlayerMessageToPlayer
#else
	#define _ALS_SendPlayerMessageToPlayer
#endif
#define SendPlayerMessageToPlayer Languages.SendPlayerMessageToPlayer

/*
 * GameTextForAll
 * ---------------------------------------------- */
stock this.GameTextForAll(const string[], time, style, va_args<>) {
	if (this.GetStringIndex(string) != -1) {
		if (numargs() > 3) {
			foreach (new playerid : Player) {
				this.CheckStringArgument(0, playerid);
				
				va_formatex(this.TextBuffer, _, string, va_start<3>);

				GameTextForPlayer(playerid, this.TextBuffer, time, style);
			}
		} else {
			foreach (new playerid : Player) {
				this.CheckStringArgument(0, playerid);
				
				GameTextForPlayer(playerid, string, time, style);
			}
		}
		
		return 1;
	}
	
	if (numargs() > 3) {
		va_formatex(this.TextBuffer, _, string, va_start<3>);

		return GameTextForAll(this.TextBuffer, time, style);
	} else {
		return GameTextForAll(string, time, style);
	}
}

#if defined _ALS_GameTextForAll
	#undef GameTextForAll
#else
	#define _ALS_GameTextForAll
#endif
#define GameTextForAll Languages.GameTextForAll

/*
 * GameTextForPlayer
 * ---------------------------------------------- */
stock this.GameTextForPlayer(playerid, const string[], time, style, va_args<>) {
	this.CheckStringArgument(1, playerid);
	
	if (numargs() > 4) {
		va_formatex(this.TextBuffer, _, message, va_start<4>);

		return GameTextForPlayer(playerid, this.TextBuffer, time, style);
	} else {
		return GameTextForPlayer(playerid, string, time, style);
	}
}

#if defined _ALS_GameTextForPlayer
	#undef GameTextForPlayer
#else
	#define _ALS_GameTextForPlayer
#endif
#define GameTextForPlayer Languages.GameTextForPlayer

/*
 * CreatePlayer3DTextLabel
 * ---------------------------------------------- */
stock PlayerText3D:this.CreatePlayer3DTextLabel(playerid, text[], color, Float:x, Float:y, Float:z, Float:DrawDistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testLOS = 0, va_args<>) {
	this.CheckStringArgument(1, playerid);
	
	if (numargs() > 10) {
		va_formatex(this.TextBuffer, _, text, va_start<10>);
		
		return CreatePlayer3DTextLabel(playerid, this.TextBuffer, color, x, y, z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
	} else {
		return CreatePlayer3DTextLabel(playerid, text, color, x, y, z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
	}
}

#if defined _ALS_CreatePlayer3DTextLabel
	#undef CreatePlayer3DTextLabel
#else
	#define _ALS_CreatePlayer3DTextLabel
#endif
#define CreatePlayer3DTextLabel Languages.CreatePlayer3DTextLabel

/*
 * UpdatePlayer3DTextLabelText
 * ---------------------------------------------- */
stock this.UpdatePlayer3DTextLabelText(playerid, PlayerText3D:id, color, text[], va_args<>) {
	this.CheckStringArgument(3, playerid);
	
	if (numargs() > 10) {
		va_formatex(this.TextBuffer, _, text, va_start<10>);
		
		return UpdatePlayer3DTextLabelText(playerid, id, color, this.TextBuffer);
	} else {
		return UpdatePlayer3DTextLabelText(playerid, id, color, text);
	}
}

#if defined _ALS_UpdatePlayer3DTextLabelTex
	#undef UpdatePlayer3DTextLabelText
#else
	#define _ALS_UpdatePlayer3DTextLabelTex
#endif
#define UpdatePlayer3DTextLabelText Languages.UpdatePlayer3DTextLabelText

/*
 * ShowPlayerDialog
 * ---------------------------------------------- */
stock this.ShowPlayerDialog(playerid, dialogid, style, caption[], info[], button1[], button2[]) {
	this.CheckStringArgument(3, playerid);
	this.CheckStringArgument(4, playerid);
	this.CheckStringArgument(5, playerid);
	this.CheckStringArgument(6, playerid);
	
	return ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
}

#if defined _ALS_ShowPlayerDialog
	#undef ShowPlayerDialog
#else
	#define _ALS_ShowPlayerDialog
#endif
#define ShowPlayerDialog Languages.ShowPlayerDialog

/*
 * SetPlayerObjectMaterialText
 * ---------------------------------------------- */
stock this.SetPlayerObjectMaterialText(playerid, objectid, text[], materialindex = 0, materialsize = OBJECT_MATERIAL_SIZE_256x128, fontface[] = "Arial", fontsize = 24, bold = 1, fontcolor = 0xFFFFFFFF, backcolor = 0, textalignment = 0) {
	this.CheckStringArgument(2, playerid);
	
	return SetPlayerObjectMaterialText(playerid, objectid, text, materialindex, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
}

#if defined _ALS_SetPlayerObjectMaterialTex
	#undef SetPlayerObjectMaterialText
#else
	#define _ALS_SetPlayerObjectMaterialTex
#endif
#define SetPlayerObjectMaterialText Languages.SetPlayerObjectMaterialText