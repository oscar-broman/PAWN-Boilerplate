/*!
 * Languages/header.inc
 *
 > Priority: 11
 */

// Save space (since these will be replaced for all translatable strings)
#define this.Prepare          _@lp
#define this.Strings          _@ls
#define this.ActiveLanguage   _@lc

const this.MAX_LANGUAGES = 20;

new this.Strings[this.MAX_LANGUAGES][{#LANGUAGES_NUM_STRINGS#}][1];
new this.Descriptions[this.MAX_LANGUAGES][{#LANGUAGES_NUM_STRINGS#}][1];
new this.ActiveLanguage;
new this.CurrentPlayer;
new this.PreviousPlayer = INVALID_PLAYER_ID;
new this.PlayerLanguage[MAX_PLAYERS];
new this.Languages = 0;

forward this.Prepare(forplayer = INVALID_PLAYER_ID);