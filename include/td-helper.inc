// td-helper.inc by Slice
#include <a_samp>

// Add a tag to INVALID_TEXT_DRAW, without changing its value (future-compatible)
const   Text:@INVALID_TEXT_DRAW = Text:INVALID_TEXT_DRAW;

#undef  INVALID_TEXT_DRAW
#define INVALID_TEXT_DRAW @INVALID_TEXT_DRAW

#define TextDrawGetPlayer(%1) \
	(_:(%1) >>> 16 & 0x7FFF)

#define TextDrawGetPlayerTextDraw(%1) \
	(PlayerText:(_:(%1) & 0xFFFF))

#define IsPlayerTextDraw(%1) \
	(_:(%1) & 0x80000000 || false)

stock Text:CreatePlayerTextDraw@(playerid, Float:x, Float:y, text[]) {
	new ptd = _:CreatePlayerTextDraw(playerid, x, y, text);
	
	if (ptd != _:INVALID_TEXT_DRAW)
		ptd |= playerid << 16 | 0x80000000;
	
	return Text:ptd;
}

stock TextDrawDestroy@({Text, PlayerText}:text)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawDestroy(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text));
	else
		return TextDrawDestroy(text);

stock TextDrawLetterSize@({Text, PlayerText}:text, Float:x, Float:y)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawLetterSize(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), x, y);
	else
		return TextDrawLetterSize(text, x, y);

stock TextDrawTextSize@({Text, PlayerText}:text, Float:x, Float:y)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawTextSize(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), x, y);
	else
		return TextDrawTextSize(text, x, y);

stock TextDrawAlignment@({Text, PlayerText}:text, alignment)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawAlignment(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), alignment);
	else
		return TextDrawAlignment(text, alignment);

stock TextDrawColor@({Text, PlayerText}:text, color)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawColor(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), color);
	else
		return TextDrawColor(text, color);

stock TextDrawUseBox@({Text, PlayerText}:text, use)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawUseBox(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), use);
	else
		return TextDrawUseBox(text, use);

stock TextDrawBoxColor@({Text, PlayerText}:text, color)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawBoxColor(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), color);
	else
		return TextDrawBoxColor(text, color);

stock TextDrawSetShadow@({Text, PlayerText}:text, size)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawSetShadow(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), size);
	else
		return TextDrawSetShadow(text, size);

stock TextDrawSetOutline@({Text, PlayerText}:text, size)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawSetOutline(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), size);
	else
		return TextDrawSetOutline(text, size);

stock TextDrawBackgroundColor@({Text, PlayerText}:text, color)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawBackgroundColor(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), color);
	else
		return TextDrawBackgroundColor(text, color);

stock TextDrawFont@({Text, PlayerText}:text, font)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawFont(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), font);
	else
		return TextDrawFont(text, font);

stock TextDrawSetProportional@({Text, PlayerText}:text, set)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawSetProportional(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), set);
	else
		return TextDrawSetProportional(text, set);

stock TextDrawSetSelectable@({Text, PlayerText}:text, set)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawSetSelectable(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), set);
	else
		return TextDrawSetSelectable(text, set);

stock TextDrawShowForPlayer@(playerid = INVALID_PLAYER_ID, {Text, PlayerText}:text)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawShow(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text));
	else
		return TextDrawShowForPlayer(playerid, text);

stock TextDrawHideForPlayer@(playerid = INVALID_PLAYER_ID, {Text, PlayerText}:text)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawHide(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text));
	else
		return TextDrawHideForPlayer(playerid, text);

stock TextDrawShowForAll@(Text:text)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawShow(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text));
	else
		return TextDrawShowForAll(text);
	
stock TextDrawHideForAll@(Text:text)
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawHide(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text));
	else
		return TextDrawHideForAll(text);

stock TextDrawSetString@({Text, PlayerText}:text, string[])
	if (IsPlayerTextDraw(text))
		return PlayerTextDrawSetString(TextDrawGetPlayer(text), TextDrawGetPlayerTextDraw(text), string);
	else
		return TextDrawSetString(text, string);

stock PlayerTextDrawDestroy@(playerid, PlayerText:text)
	return PlayerTextDrawDestroy(playerid, text & PlayerText:0xFFFF);
	
stock PlayerTextDrawLetterSize@(playerid, PlayerText:text, Float:x, Float:y)
	return PlayerTextDrawLetterSize(playerid, text & PlayerText:0xFFFF, x, y);
	
stock PlayerTextDrawTextSize@(playerid, PlayerText:text, Float:x, Float:y)
	return PlayerTextDrawTextSize(playerid, text & PlayerText:0xFFFF, x, y);
	
stock PlayerTextDrawAlignment@(playerid, PlayerText:text, alignment)
	return PlayerTextDrawAlignment(playerid, text & PlayerText:0xFFFF, alignment);
	
stock PlayerTextDrawColor@(playerid, PlayerText:text, color)
	return PlayerTextDrawColor(playerid, text & PlayerText:0xFFFF, color);
	
stock PlayerTextDrawUseBox@(playerid, PlayerText:text, use)
	return PlayerTextDrawUseBox(playerid, text & PlayerText:0xFFFF, use);
	
stock PlayerTextDrawBoxColor@(playerid, PlayerText:text, color)
	return PlayerTextDrawBoxColor(playerid, text & PlayerText:0xFFFF, color);
	
stock PlayerTextDrawSetShadow@(playerid, PlayerText:text, size)
	return PlayerTextDrawSetShadow(playerid, text & PlayerText:0xFFFF, size);
	
stock PlayerTextDrawSetOutline@(playerid, PlayerText:text, size)
	return PlayerTextDrawSetOutline(playerid, text & PlayerText:0xFFFF, size);
	
stock PlayerTextDrawBackgroundColor@(playerid, PlayerText:text, color)
	return PlayerTextDrawBackgroundColor(playerid, text & PlayerText:0xFFFF, color);
	
stock PlayerTextDrawFont@(playerid, PlayerText:text, font)
	return PlayerTextDrawFont(playerid, text & PlayerText:0xFFFF, font);
	
stock PlayerTextDrawSetProportional@(playerid, PlayerText:text, set)
	return PlayerTextDrawSetProportional(playerid, text & PlayerText:0xFFFF, set);
	
stock PlayerTextDrawSetSelectable@(playerid, PlayerText:text, set)
	return PlayerTextDrawSetSelectable(playerid, text & PlayerText:0xFFFF, set);
	
stock PlayerTextDrawShow@(playerid, PlayerText:text)
	return PlayerTextDrawShow(playerid, text & PlayerText:0xFFFF);
	
stock PlayerTextDrawHide@(playerid, PlayerText:text)
	return PlayerTextDrawHide(playerid, text & PlayerText:0xFFFF);
	
stock PlayerTextDrawSetString@(playerid, PlayerText:text, string[])
	return PlayerTextDrawSetString(playerid, text & PlayerText:0xFFFF, string);







// ALS hooks
#if defined _ALS_CreatePlayerTextDraw
	#undef CreatePlayerTextDraw
#else
	#define _ALS_CreatePlayerTextDraw
#endif
#define CreatePlayerTextDraw CreatePlayerTextDraw@
#if defined _ALS_PlayerTextDrawDestroy
	#undef PlayerTextDrawDestroy
#else
	#define _ALS_PlayerTextDrawDestroy
#endif
#define PlayerTextDrawDestroy PlayerTextDrawDestroy@

#if defined _ALS_PlayerTextDrawLetterSize
	#undef PlayerTextDrawLetterSize
#else
	#define _ALS_PlayerTextDrawLetterSize
#endif
#define PlayerTextDrawLetterSize PlayerTextDrawLetterSize@

#if defined _ALS_PlayerTextDrawTextSize
	#undef PlayerTextDrawTextSize
#else
	#define _ALS_PlayerTextDrawTextSize
#endif
#define PlayerTextDrawTextSize PlayerTextDrawTextSize@

#if defined _ALS_PlayerTextDrawAlignment
	#undef PlayerTextDrawAlignment
#else
	#define _ALS_PlayerTextDrawAlignment
#endif
#define PlayerTextDrawAlignment PlayerTextDrawAlignment@

#if defined _ALS_PlayerTextDrawColor
	#undef PlayerTextDrawColor
#else
	#define _ALS_PlayerTextDrawColor
#endif
#define PlayerTextDrawColor PlayerTextDrawColor@

#if defined _ALS_PlayerTextDrawUseBox
	#undef PlayerTextDrawUseBox
#else
	#define _ALS_PlayerTextDrawUseBox
#endif
#define PlayerTextDrawUseBox PlayerTextDrawUseBox@

#if defined _ALS_PlayerTextDrawBoxColor
	#undef PlayerTextDrawBoxColor
#else
	#define _ALS_PlayerTextDrawBoxColor
#endif
#define PlayerTextDrawBoxColor PlayerTextDrawBoxColor@

#if defined _ALS_PlayerTextDrawSetShadow
	#undef PlayerTextDrawSetShadow
#else
	#define _ALS_PlayerTextDrawSetShadow
#endif
#define PlayerTextDrawSetShadow PlayerTextDrawSetShadow@

#if defined _ALS_PlayerTextDrawSetOutline
	#undef PlayerTextDrawSetOutline
#else
	#define _ALS_PlayerTextDrawSetOutline
#endif
#define PlayerTextDrawSetOutline PlayerTextDrawSetOutline@

#if defined _ALS_PlayerTextDrawBackgroundCo
	#undef PlayerTextDrawBackgroundColor
#else
	#define _ALS_PlayerTextDrawBackgroundCo
#endif
#define PlayerTextDrawBackgroundColor PlayerTextDrawBackgroundColor@

#if defined _ALS_PlayerTextDrawFont
	#undef PlayerTextDrawFont
#else
	#define _ALS_PlayerTextDrawFont
#endif
#define PlayerTextDrawFont PlayerTextDrawFont@

#if defined _ALS_PlayerTextDrawSetProportio
	#undef PlayerTextDrawSetProportional
#else
	#define _ALS_PlayerTextDrawSetProportio
#endif
#define PlayerTextDrawSetProportional PlayerTextDrawSetProportional@

#if defined _ALS_PlayerTextDrawSetSelectabl
	#undef PlayerTextDrawSetSelectable
#else
	#define _ALS_PlayerTextDrawSetSelectabl
#endif
#define PlayerTextDrawSetSelectable PlayerTextDrawSetSelectable@

#if defined _ALS_PlayerTextDrawShow
	#undef PlayerTextDrawShow
#else
	#define _ALS_PlayerTextDrawShow
#endif
#define PlayerTextDrawShow PlayerTextDrawShow@

#if defined _ALS_PlayerTextDrawHide
	#undef PlayerTextDrawHide
#else
	#define _ALS_PlayerTextDrawHide
#endif
#define PlayerTextDrawHide PlayerTextDrawHide@

#if defined _ALS_PlayerTextDrawSetString
	#undef PlayerTextDrawSetString
#else
	#define _ALS_PlayerTextDrawSetString
#endif
#define PlayerTextDrawSetString PlayerTextDrawSetString@

#if defined _ALS_TextDrawDestroy
	#undef TextDrawDestroy
#else
	#define _ALS_TextDrawDestroy
#endif
#define TextDrawDestroy TextDrawDestroy@

#if defined _ALS_TextDrawLetterSize
	#undef TextDrawLetterSize
#else
	#define _ALS_TextDrawLetterSize
#endif
#define TextDrawLetterSize TextDrawLetterSize@

#if defined _ALS_TextDrawTextSize
	#undef TextDrawTextSize
#else
	#define _ALS_TextDrawTextSize
#endif
#define TextDrawTextSize TextDrawTextSize@

#if defined _ALS_TextDrawAlignment
	#undef TextDrawAlignment
#else
	#define _ALS_TextDrawAlignment
#endif
#define TextDrawAlignment TextDrawAlignment@

#if defined _ALS_TextDrawColor
	#undef TextDrawColor
#else
	#define _ALS_TextDrawColor
#endif
#define TextDrawColor TextDrawColor@

#if defined _ALS_TextDrawUseBox
	#undef TextDrawUseBox
#else
	#define _ALS_TextDrawUseBox
#endif
#define TextDrawUseBox TextDrawUseBox@

#if defined _ALS_TextDrawBoxColor
	#undef TextDrawBoxColor
#else
	#define _ALS_TextDrawBoxColor
#endif
#define TextDrawBoxColor TextDrawBoxColor@

#if defined _ALS_TextDrawSetShadow
	#undef TextDrawSetShadow
#else
	#define _ALS_TextDrawSetShadow
#endif
#define TextDrawSetShadow TextDrawSetShadow@

#if defined _ALS_TextDrawSetOutline
	#undef TextDrawSetOutline
#else
	#define _ALS_TextDrawSetOutline
#endif
#define TextDrawSetOutline TextDrawSetOutline@

#if defined _ALS_TextDrawBackgroundColor
	#undef TextDrawBackgroundColor
#else
	#define _ALS_TextDrawBackgroundColor
#endif
#define TextDrawBackgroundColor TextDrawBackgroundColor@

#if defined _ALS_TextDrawFont
	#undef TextDrawFont
#else
	#define _ALS_TextDrawFont
#endif
#define TextDrawFont TextDrawFont@

#if defined _ALS_TextDrawSetProportional
	#undef TextDrawSetProportional
#else
	#define _ALS_TextDrawSetProportional
#endif
#define TextDrawSetProportional TextDrawSetProportional@

#if defined _ALS_TextDrawSetSelectable
	#undef TextDrawSetSelectable
#else
	#define _ALS_TextDrawSetSelectable
#endif
#define TextDrawSetSelectable TextDrawSetSelectable@

#if defined _ALS_TextDrawShowForPlayer
	#undef TextDrawShowForPlayer
#else
	#define _ALS_TextDrawShowForPlayer
#endif
#define TextDrawShowForPlayer TextDrawShowForPlayer@

#if defined _ALS_TextDrawHideForPlayer
	#undef TextDrawHideForPlayer
#else
	#define _ALS_TextDrawHideForPlayer
#endif
#define TextDrawHideForPlayer TextDrawHideForPlayer@

#if defined _ALS_TextDrawShowForAll
	#undef TextDrawShowForAll
#else
	#define _ALS_TextDrawShowForAll
#endif
#define TextDrawShowForAll TextDrawShowForAll@

#if defined _ALS_TextDrawHideForAll
	#undef TextDrawHideForAll
#else
	#define _ALS_TextDrawHideForAll
#endif
#define TextDrawHideForAll TextDrawHideForAll@

#if defined _ALS_TextDrawSetString
	#undef TextDrawSetString
#else
	#define _ALS_TextDrawSetString
#endif
#define TextDrawSetString TextDrawSetString@

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid) {
	playertextid |= PlayerText:(playerid << 16 | 0x80000000);
	
	return CallRemoteFunction(!"OnPlayerClickTextDraw", !"ii", playerid, _:playertextid);
}

#if defined _ALS_OnPlayerClickPlayerTextDra
    #undef OnPlayerClickPlayerTextDraw
#else
    #define _ALS_OnPlayerClickPlayerTextDra
#endif

#define OnPlayerClickPlayerTextDraw OnPlayerClickPlayerTextDraw@

forward OnPlayerClickPlayerTextDraw@(playerid, PlayerText:playertextid);