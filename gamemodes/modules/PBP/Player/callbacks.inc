/*!
 * PBP/Player/callbacks.inc
 *
 > Priority: 0
 */

// Called when an in-game player logged into RCON.
forward OnPlayerRconLogin(playerid);

forward	OnPlayerPositionUpdate(playerid, Float:x, Float:y, Float:z);
forward	OnPlayerVelocityUpdate(playerid, Float:vx, Float:vy, Float:vz);
forward OnPlayerWeaponChange(playerid, weapon, previous_weapon);
forward OnPlayerAmmoChange(playerid, ammo, previous_ammo);
forward OnPlayerWeaponStateChange(playerid, weaponstate, previous_weaponstate);
forward OnPlayerMoneyChange(playerid, money, previous_money);
forward OnPlayerPingChange(playerid, ping, previous_ping);
forward OnPlayerSpecialActionChange(playerid, special_action, previous_special_action);
forward OnPlayerCameraModeChange(playerid, camera_mode, previous_camera_mode);
forward OnPlayerNameChange(playerid, const old_name[]);