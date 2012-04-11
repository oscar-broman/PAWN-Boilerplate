/*!
 * Player/header.inc
 *
 > Priority: 0
 */

enum e_PLAYER_INFO {
	     IP,
	bool:IsAdmin,
	     Name[MAX_PLAYER_NAME]
};

new
	this.Info[MAX_PLAYERS][e_PLAYER_INFO]
;