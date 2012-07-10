/*!
 * PBP/Player/functions.inc
 *
 > Priority: 10
 */

// Test whether a player is in any of the groups given
stock bool:this.IsInAnyOfGroups(playerid, Group:...) {
	for (new i = numargs(); --i; ) {
		if (Group_GetPlayer(playerid, Group:getarg(i)))
			return true;
	}

	return false;
}

// Used internally from a timer
public this.CheckForNewAdmins() {
	foreach (new playerid : Player) {
		if (!this.Info[playerid][IsAdmin] && IsPlayerAdmin(playerid)) {
			this.Info[playerid][IsAdmin] = true;
			
			CallRemoteFunction(#OnPlayerRconLogin, !"i", playerid);
		}
	}
}

// Custom sscanf specifier for searching through player names
// The default one doesn't find "def" in [ABC]Def.
#if defined SSCANF
	SSCANF:player_name(string[]) {
		new foundplayer = INVALID_PLAYER_ID;
		new bool:numeric = true;
		
		for (new i = 0, c; (c = string[i]); i++) {
			if (c < '0' || c > '9') {
				numeric = false;
				
				break;
			}
		}
		
		if (numeric) {
			foundplayer = strval(string);
			
			if (IsPlayerConnected(foundplayer))
				return foundplayer;
			else
				foundplayer = INVALID_PLAYER_ID;
		}
		
		foreach(new playerid : Player) {
			if (strfind(this.Info[playerid][Name], string, true) != -1) {
				// Multiple matches
				if (foundplayer != INVALID_PLAYER_ID)
					return INVALID_PLAYER_ID;
				else
					foundplayer = playerid;
			}
		}
		
		return foundplayer;
	}
#endif

stock this.Teleport(playerid, Float:x, Float:y, Float:z, Float:rot = 0.0, interior = -1, world = -1) {
	SetPVarFloat(playerid, #this.TeleX, x);
	SetPVarFloat(playerid, #this.TeleY, y);
	SetPVarFloat(playerid, #this.TeleZ, z);
	SetPVarFloat(playerid, #this.TeleRot, rot);
	SetPVarInt  (playerid, #this.TeleInterior, interior);
	SetPVarInt  (playerid, #this.TeleWorld, world);
	
	Bit_Vet(this.HasTeleported, playerid);
	
	if (this.TeleportTextDraw[playerid] != INVALID_TEXT_DRAW)
		TextDrawDestroy(this.TeleportTextDraw[playerid]);
	
	this.TeleportTextDraw[playerid] = CreateTextDrawBox(0, -5.0, -5.0, 645.0, 485.0, playerid);
	
	TextDrawShowForPlayer(_, this.TeleportTextDraw[playerid]);
	
	this.TeleportStart[playerid] = GetTickCount();
	
	new timer;
	
	if (-1 != (timer = GetPVarInt(playerid, #this.TeleportTimer)))
		KillTimer(timer);
	
	timer = SetTimerEx(!#this.TeleportStep, this.TeleportTimerStep, true, !"i", playerid);
	
	SetPVarInt(playerid, #this.TeleportTimer, timer);
}

public this.TeleportStep(playerid) {
	if (!IsPlayerConnected(playerid) || !this.TeleportStart[playerid])
		return;
	
	new Float:progress = (GetTickCount() - this.TeleportStart[playerid]) / this.TeleportDuration;
	
	if (progress <= 0.5) {
		TextDrawBoxColor(this.TeleportTextDraw[playerid], floatround(500.0 * progress));
		
		TextDrawShowForPlayer(_, this.TeleportTextDraw[playerid]);
	} else {
		if (!Bit_Get(this.HasTeleported, playerid)) {
			Bit_Let(this.HasTeleported, playerid);
			
			SetPlayerPos(playerid, GetPVarFloat(playerid, #this.TeleX),
			                       GetPVarFloat(playerid, #this.TeleY),
			                       GetPVarFloat(playerid, #this.TeleZ));

			SetPlayerFacingAngle(playerid, GetPVarFloat(playerid, #this.TeleRot));

			new interior, world;

			interior = GetPVarInt(playerid, #this.TeleInterior);
			world = GetPVarInt(playerid, #this.TeleWorld);

			if (interior != -1) SetPlayerInterior(playerid, interior);
			if (world != -1) SetPlayerVirtualWorld(playerid, world);
			
			DeletePVar(playerid, #this.TeleX);
			DeletePVar(playerid, #this.TeleY);
			DeletePVar(playerid, #this.TeleZ);
			DeletePVar(playerid, #this.TeleRot);
			DeletePVar(playerid, #this.TeleInterior);
			DeletePVar(playerid, #this.TeleWorld);
		}
		
		if (progress >= 1.0) {
			KillTimer(GetPVarInt(playerid, #this.TeleportTimer));

			DeletePVar(playerid, #this.TeleportTimer);

			TextDrawDestroy(this.TeleportTextDraw[playerid]);

			this.TeleportTextDraw[playerid] = INVALID_TEXT_DRAW;
		} else {
			TextDrawBoxColor(this.TeleportTextDraw[playerid], floatround(500.0 * (1.0 - progress)));
		
			TextDrawShowForPlayer(_, this.TeleportTextDraw[playerid]);
		}
	}
}

// Hook SetPlayerName
stock this.SetPlayerName(playerid, name[]) {
	new old_name[MAX_PLAYER_NAME], retval;
	
	GetPlayerName(playerid, old_name, sizeof(old_name));
	
	retval = SetPlayerName(playerid, name);
	
	if (retval == 1) {
		strunpack(Player.Info[playerid][Name], name, MAX_PLAYER_NAME);
		
		CallFunction(this.CB_OnPlayerNameChange, playerid, ref(old_name));
	}
	
	return retval;
}

#if defined _ALS_SetPlayerName
	#undef SetPlayerName
#else
	#define _ALS_SetPlayerName
#endif
#define SetPlayerName Player.SetPlayerName