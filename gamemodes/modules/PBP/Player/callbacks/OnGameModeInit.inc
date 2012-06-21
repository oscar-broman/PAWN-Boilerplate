/*!
 * PBP/Player/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	SetTimer(#this.CheckForNewAdmins, 0, false);
	
	this.CB_OnPlayerPositionUpdate   = GetPublicAddressByName("OnPlayerPositionUpdate");
	this.CB_OnPlayerVelocityUpdate   = GetPublicAddressByName("OnPlayerVelocityUpdate");
	this.CB_OnPlayerWeaponChange     = GetPublicAddressByName("OnPlayerWeaponChange");
	this.CB_OnPlayerAmmoChange       = GetPublicAddressByName("OnPlayerAmmoChange");
	this.CB_OnPlayerWeaponStateChg   = GetPublicAddressByName("OnPlayerWeaponStateChange");
	this.CB_OnPlayerMoneyChange      = GetPublicAddressByName("OnPlayerMoneyChange");
	this.CB_OnPlayerPingChange       = GetPublicAddressByName("OnPlayerPingChange");
	this.CB_OnPlayerSpecialActionChg = GetPublicAddressByName("OnPlayerSpecialActionChange");
	this.CB_OnPlayerCameraModeChange = GetPublicAddressByName("OnPlayerCameraModeChange");
	
	if (this.UsePlayerPedAnimations)
		UsePlayerPedAnims();
}