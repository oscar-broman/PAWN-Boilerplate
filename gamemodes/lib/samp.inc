// Buffer used within this file
static stock
	gs_Buffer[256]
;

#if defined va_formatex
	stock SendClientMessagef(playerid, color, fmat[], va_args<>) {
		va_formatex(gs_Buffer, _, fmat, va_start<3>);

		SendClientMessage(playerid, color, gs_Buffer);
	}

	stock SendClientMessageToAllf(color, fmat[], va_args<>) {
		va_formatex(gs_Buffer, _, fmat, va_start<2>);

		SendClientMessageToAll(color, gs_Buffer);
	}
#endif

stock IsValidSkin(skinid)
	return (0 <= skinid <= 299);