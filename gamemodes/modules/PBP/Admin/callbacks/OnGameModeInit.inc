/*!
 * PBP/Admin/callbacks/OnGameModeInit.inc
 *
 * OnGameModeInit()
 *
 > Priority: 0
 */

{
	Text.SetActiveLanguage(0);
	
	RedirectArraySlot(this.LevelNames, 0, ref(@"Nothing"));
	RedirectArraySlot(this.LevelNames, 1, ref(@"Moderator"));
	RedirectArraySlot(this.LevelNames, 2, ref(@"Administrator"));
	RedirectArraySlot(this.LevelNames, 3, ref(@"Management"));
	
	Text.SetActiveLanguage(-1);
}