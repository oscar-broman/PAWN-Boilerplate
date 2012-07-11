/*!
 * PBP/Player/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	SetTimer(#this.CheckForNewAdmins, 0, false);
	
	this.CB_OnPlayerPositionUpdate   = GetPublicAddressFromName("OnPlayerPositionUpdate");
	this.CB_OnPlayerVelocityUpdate   = GetPublicAddressFromName("OnPlayerVelocityUpdate");
	this.CB_OnPlayerWeaponChange     = GetPublicAddressFromName("OnPlayerWeaponChange");
	this.CB_OnPlayerAmmoChange       = GetPublicAddressFromName("OnPlayerAmmoChange");
	this.CB_OnPlayerWeaponStateChg   = GetPublicAddressFromName("OnPlayerWeaponStateChange");
	this.CB_OnPlayerMoneyChange      = GetPublicAddressFromName("OnPlayerMoneyChange");
	this.CB_OnPlayerPingChange       = GetPublicAddressFromName("OnPlayerPingChange");
	this.CB_OnPlayerSpecialActionChg = GetPublicAddressFromName("OnPlayerSpecialActionChange");
	this.CB_OnPlayerCameraModeChange = GetPublicAddressFromName("OnPlayerCameraModeChange");
	this.CB_OnPlayerNameChange       = GetPublicAddressFromName("OnPlayerNameChange");
	
	if (this.UsePlayerPedAnimations)
		UsePlayerPedAnims();
}