// Buffer used within this file
static stock
	gs_Buffer[256]
;

#if defined va_formatex
	stock FMAT_SendClientMessage(playerid, color, fmat[], va_args<>) {
		if (numargs() > 3) {
			va_formatex(gs_Buffer, _, fmat, va_start<3>);

			return SendClientMessage(playerid, color, gs_Buffer);
		} else {
			return SendClientMessage(playerid, color, fmat);
		}
	}
	
	#if defined _ALS_SendClientMessage
		#undef SendClientMessage
	#else
		#define _ALS_SendClientMessage
	#endif
	#define SendClientMessage FMAT_SendClientMessage

	stock FMAT_SendClientMessageToAll(color, fmat[], va_args<>) {
		if (numargs() > 2) {
			va_formatex(gs_Buffer, _, fmat, va_start<2>);

			return SendClientMessageToAll(color, gs_Buffer);
		} else {
			return SendClientMessageToAll(color, fmat);
		}
	}
	
	#if defined _ALS_SendClientMessageToAll
		#undef SendClientMessageToAll
	#else
		#define _ALS_SendClientMessageToAll
	#endif
	#define SendClientMessageToAll FMAT_SendClientMessageToAll
#endif

stock IsValidSkin(skinid)
	return (0 <= skinid <= 299);