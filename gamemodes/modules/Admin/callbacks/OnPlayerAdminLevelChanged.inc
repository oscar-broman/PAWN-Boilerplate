/*!
 * Admin/callbacks/OnPlayerAdminLevelChanged.inc
 *
 * OnPlayerAdminLevelChanged(playerid, AdminLevel:old_level, AdminLevel:new_level, by_player)
 *
 > Priority: 0
 */

{
	if (by_player != INVALID_PLAYER_ID) {
		if (new_level > old_level)
			SendClientMessageToAll(COLOR_GENERIC_INFO, "* %P%C was promoted to %L by %P%C.", playerid, COLOR_GENERIC_INFO, this.LevelNames[new_level], by_player, COLOR_GENERIC_INFO);
		else
			SendClientMessageToAll(COLOR_GENERIC_INFO, "* %P%C was demoted to %L by %P%C.", playerid, COLOR_GENERIC_INFO, this.LevelNames[new_level], by_player, COLOR_GENERIC_INFO);
	}
}