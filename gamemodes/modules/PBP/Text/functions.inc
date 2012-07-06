/*!
 * PBP/Text/functions.inc
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
					if (strequals(this.Strings[0][i], string) && strequals(this.Descriptions[i], description)) {
						new len = strlen(translation);
						new Alloc:translation_slot = Malloc_Allocate(len + 2);
						
						strunpack(mstr(translation_slot)[1], translation, len + 1);
						
						RedirectArraySlot(this.Strings[lang], i, Malloc_GetAddress(translation_slot) + 4);
						
						mset(translation_slot, 0, (lang << 16) | i);
						
						break;
					}
				}
				
				description{0} = '\0';
			}
		}
		
		fclose(fp);
		
		for (new i = 0; i < sizeof(this.Strings[]); i++) {
			if (!this.Strings[lang][i][0]) {
				printf("WARNING: Missing language entry in \"%S\": \"%S\"%c(%S)", filename, this.Strings[0][i], IsNull(this.Descriptions[i]) ? '\0' : ' ', this.Descriptions[i]);
				
				new len = strlen(this.Strings[0][i]);
				new Alloc:translation_slot = Malloc_Allocate(len + 2);
				
				strunpack(mstr(translation_slot)[1], this.Strings[0][i], len + 1);
				
				RedirectArraySlot(this.Strings[lang], i, Malloc_GetAddress(translation_slot) + 4);
				
				mset(translation_slot, 0, (lang << 16) | i);
			}
		}
		
		return lang;
	}
	
	return -1;
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

stock this.Prepare(index, forplayer = INVALID_PLAYER_ID) {
	if (forplayer == INVALID_PLAYER_ID) {
		if (this.HasPreviousPlayer) {
			this.CurrentPlayer = this.PreviousPlayer;
			this.PreviousPlayer = INVALID_PLAYER_ID;
			this.HasPreviousPlayer = false;
		}
	} else {
		if (!this.HasPreviousPlayer) {
			this.HasPreviousPlayer = true;
			this.PreviousPlayer = this.CurrentPlayer;
		}

		this.CurrentPlayer = forplayer;
	}
	
	if (this.CurrentPlayer == INVALID_PLAYER_ID) {
		if (this.ActiveLanguage == -1) {
			print (!"(Text) WARNING: Current player is not set, falling back to default language.");
			printf(!"(Text)          String: \"%S\"%c(%S)", this.Strings[0][index], IsNull(this.Descriptions[index]) ? '\0' : ' ', this.Descriptions[index]);
		}
	} else {
		this.ActiveLanguage = this.PlayerLanguage[this.CurrentPlayer];
		
		this.DeferredResetActiveLanguage();
	}
}

stock this.SetActivePlayer(playerid) {
	this.CurrentPlayer = playerid;
	this.HasPreviousPlayer = false;
	this.PreviousPlayer = INVALID_PLAYER_ID;
	
	this.DeferredResetActiveLanguage();
}

stock this.SetActiveLanguage(lang) {
	this.ActiveLanguage = (0 <= lang < this.Languages) ? lang : -1;
	this.HasPreviousPlayer = false;
	this.CurrentPlayer = INVALID_PLAYER_ID;
	this.PreviousPlayer = INVALID_PLAYER_ID;
}

stock bool:this.SetPlayerLanguage(playerid, lang) {
	if (0 <= lang < this.Languages) {
		this.PlayerLanguage[playerid] = lang;
		
		return true;
	}
	
	return false;
}

stock this.GetPlayerLanguage(playerid) {
	return this.PlayerLanguage[playerid];
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

stock this.GetStringAddress(const string[], playerid = INVALID_PLAYER_ID, lang = -1) {
	new address;
	
	#emit LOAD.S.pri  string
	#emit STOR.S.pri  address
	
	if (lang == -1) {
		if (playerid == INVALID_PLAYER_ID) {
			if (this.CurrentPlayer != INVALID_PLAYER_ID)
				playerid = this.CurrentPlayer, lang = this.PlayerLanguage[playerid];
			else if (this.ActiveLanguage != -1)
				lang = this.ActiveLanguage;
			else
				return address;
		} else {
			lang = this.PlayerLanguage[playerid];
		}
	}
	
	if (!(0 <= lang < this.Languages))
		return address;
	
	new current_lang;
	new index = this.GetStringIndex(@ptr[address], current_lang);
	
	if (index == -1 || current_lang == lang)
		return address;
	
	return ref(this.Strings[lang][index]);
}

stock bool:this.IsTranslatable(const string[]) {
	return this.GetStringIndex(string) != -1;
}

stock this.DeferredResetActiveLanguage() {
	if (this.ResetActiveLanguageTimer == -1)
		this.ResetActiveLanguageTimer = SetTimer(#this.ResetActiveLanguage, 0, false);
}

public this.ResetActiveLanguage() {
	this.ResetActiveLanguageTimer = -1;
	
	this.ActiveLanguage = -1;
	this.HasPreviousPlayer = false;
	this.CurrentPlayer = INVALID_PLAYER_ID;
	this.PreviousPlayer = INVALID_PLAYER_ID;
}

stock this.GetLanguageFromLanguageCode(this.LANGUAGE_CODE:code) {
	for (new i = 0; i < this.Languages; i++) {
		if (this.LanguageNames[i] == _:code)
			return i;
	}
	
	return -1;
}

stock this.ShowsLanguageSelection() {
	return (this.EnableLanguageSelection && this.Languages > 1);
}

stock this.ShowLanguageSelection(playerid) {
	new langs[this.MAX_LANGUAGES * 16] = " ";
	
	for (new i = 0; i < this.Languages; i++) {
		if (i)
			strcat(langs, "\n");
		
		strcat(langs, this.LanguageCodes[this.LanguageNames[i]][1]);
	}
	
	ShowPlayerDialog(playerid, this.DIALOG_LANGUAGE_SELECTION, DIALOG_STYLE_LIST, "Select language", langs[1], "OK", "Cancel");
}

// formatex specifier for translatable strings
FormatSpecifier<'@'>(output[], string[]) {
	strunpack(output, this.@(string));
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
#define SendClientMessageToAll Text.SendClientMessageToAll

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
#define SendClientMessage Text.SendClientMessage

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
#define CreatePlayerTextDraw Text.CreatePlayerTextDraw

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
#define PlayerTextDrawSetString Text.PlayerTextDrawSetString

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
#define SendPlayerMessageToAll Text.SendPlayerMessageToAll

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
#define SendPlayerMessageToPlayer Text.SendPlayerMessageToPlayer

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
#define GameTextForAll Text.GameTextForAll

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
#define GameTextForPlayer Text.GameTextForPlayer

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
#define CreatePlayer3DTextLabel Text.CreatePlayer3DTextLabel

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
#define UpdatePlayer3DTextLabelText Text.UpdatePlayer3DTextLabelText

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
#define ShowPlayerDialog Text.ShowPlayerDialog

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
#define SetPlayerObjectMaterialText Text.SetPlayerObjectMaterialText

/*
 * Dialog_ShowCallback
 * ---------------------------------------------- */
stock this.Dialog_ShowCallback(playerid, callback:callback, style, string:title[], string:caption[], string:button1[], string:button2[] = "", dialog = -1) {
	this.CheckStringArgument(3, playerid);
	this.CheckStringArgument(4, playerid);
	this.CheckStringArgument(5, playerid);
	this.CheckStringArgument(6, playerid);
	
	return Dialog_ShowCallback(playerid, callback, style, title, caption, button1, button2, dialog);
}

#if defined _ALS_Dialog_ShowCallback
	#undef Dialog_ShowCallback
#else
	#define _ALS_Dialog_ShowCallback
#endif
#define Dialog_ShowCallback Text.Dialog_ShowCallback

/*
 * Dialog_ShowCallbackData
 * ---------------------------------------------- */
stock this.Dialog_ShowCallbackData(playerid, callback[E_CALLBACK_DATA], style, string:title[], string:caption[], string:button1[], string:button2[] = "", dialog = -1) {
	this.CheckStringArgument(3, playerid);
	this.CheckStringArgument(4, playerid);
	this.CheckStringArgument(5, playerid);
	this.CheckStringArgument(6, playerid);
	
	return Dialog_ShowCallbackData(playerid, callback, style, title, caption, button1, button2, dialog);
}

#if defined _ALS_Dialog_ShowCallbackData
	#undef Dialog_ShowCallbackData
#else
	#define _ALS_Dialog_ShowCallbackData
#endif
#define Dialog_ShowCallbackData Text.Dialog_ShowCallbackData

/*
 * Dialog_Show
 * ---------------------------------------------- */
stock this.Dialog_Show(playerid, style, string:title[], string:caption[], string:button1[], string:button2[] = "", dialog = -1) {
	this.CheckStringArgument(2, playerid);
	this.CheckStringArgument(3, playerid);
	this.CheckStringArgument(4, playerid);
	this.CheckStringArgument(5, playerid);
	
	return Dialog_Show(playerid, style, title, caption, button1, button2, dialog);
}

#if defined _ALS_Dialog_Show
	#undef Dialog_Show
#else
	#define _ALS_Dialog_Show
#endif
#define Dialog_Show Text.Dialog_Show

/*
 * Group_SetName
 * ---------------------------------------------- */
stock this.Group_SetName(Group:g, name[]) {
	if (this.IsTranslatable(name)) {
		GROUP_FIX(g);
		
		this.GroupName[g] = ref(name);
	} else {
		Group_SetName(g, name);
	}
}

#if defined _ALS_Group_SetName
	#undef Group_SetName
#else
	#define _ALS_Group_SetName
#endif
#define Group_SetName Text.Group_SetName

/*
 * Group_GetName
 * ---------------------------------------------- */
stock this.Group_GetName(Group:g) {
	new ret[YSI_MAX_STRING], Group:gidx;
	
	gidx = g;
	
	GROUP_FIX(gidx);
	
	if (this.GroupName[gidx])
		strunpack(ret, this.@(@ptr[this.GroupName[gidx]]));
	else
		ret = Group_GetName(g);
	
	return ret;
}

#if defined _ALS_Group_GetName
	#undef Group_GetName
#else
	#define _ALS_Group_GetName
#endif
#define Group_GetName Text.Group_GetName