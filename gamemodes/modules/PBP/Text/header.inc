/*!
 * PBP/Text/header.inc
 *
 > Priority: 11
 */

// Save space (since these will be replaced for all translatable strings)
#define this.Prepare          _@lp
#define this.Strings          _@ls
#define this.ActiveLanguage   _@lC
#define this.DefaultLanguage  _@ld

#define _@lc _@lC==-1?_@ld:_@lC

#define TextArray:

// Wrap this around any strings to make sure they're in the correct language
#define this.@(%0) \
	(@ptr[Text.GetStringAddress(%0)])


enum {
	this.DIALOG_LANGUAGE_SELECTION = this.OFFSET
};

enum this.LANGUAGE_CODE: {
	LANG_AB, LANG_AA, LANG_AF, LANG_SQ, LANG_AM,
	LANG_AR, LANG_HY, LANG_AS, LANG_AY, LANG_AZ,
	LANG_BA, LANG_EU, LANG_BN, LANG_DZ, LANG_BH,
	LANG_BI, LANG_BR, LANG_BG, LANG_MY, LANG_BE,
	LANG_KM, LANG_CA, LANG_ZH, LANG_CO, LANG_HR,
	LANG_CS, LANG_DA, LANG_NL, LANG_EN, LANG_EO,
	LANG_ET, LANG_FO, LANG_FJ, LANG_FI, LANG_FR,
	LANG_FY, LANG_GD, LANG_GL, LANG_KA, LANG_DE,
	LANG_EL, LANG_KL, LANG_GN, LANG_GU, LANG_HA,
	LANG_IW, LANG_HI, LANG_HU, LANG_IS, LANG_IN,
	LANG_IA, LANG_IE, LANG_IK, LANG_GA, LANG_IT,
	LANG_JA, LANG_JW, LANG_KN, LANG_KS, LANG_KK,
	LANG_RW, LANG_KY, LANG_RN, LANG_KO, LANG_KU,
	LANG_LO, LANG_LA, LANG_LV, LANG_LN, LANG_LT,
	LANG_MK, LANG_MG, LANG_MS, LANG_ML, LANG_MT,
	LANG_MI, LANG_MR, LANG_MO, LANG_MN, LANG_NA,
	LANG_NE, LANG_NO, LANG_OC, LANG_OR, LANG_OM,
	LANG_PS, LANG_FA, LANG_PL, LANG_PT, LANG_PA,
	LANG_QU, LANG_RM, LANG_RO, LANG_RU, LANG_SM,
	LANG_SG, LANG_SA, LANG_SR, LANG_SH, LANG_ST,
	LANG_TN, LANG_SN, LANG_SD, LANG_SI, LANG_SS,
	LANG_SK, LANG_SL, LANG_SO, LANG_ES, LANG_SU,
	LANG_SW, LANG_SV, LANG_TL, LANG_TG, LANG_TA,
	LANG_TT, LANG_TE, LANG_TH, LANG_BO, LANG_TI,
	LANG_TO, LANG_TS, LANG_TR, LANG_TK, LANG_TW,
	LANG_UK, LANG_UR, LANG_UZ, LANG_VI, LANG_VO,
	LANG_CY, LANG_WO, LANG_XH, LANG_JI, LANG_YO,
	LANG_ZU
};

stock const this.LanguageCodes[][] = {
	{!"AB", !"Abkhazian"},   {!"AA", !"Afar"},        {!"AF", !"Afrikaans"}, {!"SQ", !"Albanian"},       {!"AM", !"Amharic"},     
	{!"AR", !"Arabic"},      {!"HY", !"Armenian"},    {!"AS", !"Assamese"},  {!"AY", !"Aymara"},         {!"AZ", !"Azerbaijani"},
	{!"BA", !"Bashkir"},     {!"EU", !"Basque"},      {!"BN", !"Bengali"},   {!"DZ", !"Bhutani"},        {!"BH", !"Bihari"},      
	{!"BI", !"Bislama"},     {!"BR", !"Breton"},      {!"BG", !"Bulgarian"}, {!"MY", !"Burmese"},        {!"BE", !"Byelorussian"},
	{!"KM", !"Cambodian"},   {!"CA", !"Catalan"},     {!"ZH", !"Chinese"},   {!"CO", !"Corsican"},       {!"HR", !"Croatian"},    
	{!"CS", !"Czech"},       {!"DA", !"Danish"},      {!"NL", !"Dutch"},     {!"EN", !"English"},        {!"EO", !"Esperanto"},
	{!"ET", !"Estonian"},    {!"FO", !"Faeroese"},    {!"FJ", !"Fiji"},      {!"FI", !"Finnish"},        {!"FR", !"French"},      
	{!"FY", !"Frisian"},     {!"GD", !"Gaelic"},      {!"GL", !"Galician"},  {!"KA", !"Georgian"},       {!"DE", !"German"},
	{!"EL", !"Greek"},       {!"KL", !"Greenlandic"}, {!"GN", !"Guarani"},   {!"GU", !"Gujarati"},       {!"HA", !"Hausa"},       
	{!"IW", !"Hebrew"},      {!"HI", !"Hindi"},       {!"HU", !"Hungarian"}, {!"IS", !"Icelandic"},      {!"IN", !"Indonesian"},
	{!"IA", !"Interlingua"}, {!"IE", !"Interlingue"}, {!"IK", !"Inupiak"},   {!"GA", !"Irish"},          {!"IT", !"Italian"},     
	{!"JA", !"Japanese"},    {!"JW", !"Javanese"},    {!"KN", !"Kannada"},   {!"KS", !"Kashmiri"},       {!"KK", !"Kazakh"},
	{!"RW", !"Kinyarwanda"}, {!"KY", !"Kirghiz"},     {!"RN", !"Kirundi"},   {!"KO", !"Korean"},         {!"KU", !"Kurdish"},     
	{!"LO", !"Laothian"},    {!"LA", !"Latin"},       {!"LV", !"Latvian"},   {!"LN", !"Lingala"},        {!"LT", !"Lithuanian"},
	{!"MK", !"Macedonian"},  {!"MG", !"Malagasy"},    {!"MS", !"Malay"},     {!"ML", !"Malayalam"},      {!"MT", !"Maltese"},     
	{!"MI", !"Maori"},       {!"MR", !"Marathi"},     {!"MO", !"Moldavian"}, {!"MN", !"Mongolian"},      {!"NA", !"Nauru"},
	{!"NE", !"Nepali"},      {!"NO", !"Norwegian"},   {!"OC", !"Occitan"},   {!"OR", !"Oriya"},          {!"OM", !"Oromo, Afan"}, 
	{!"PS", !"Pashto"},      {!"FA", !"Persian"},     {!"PL", !"Polish"},    {!"PT", !"Portuguese"},     {!"PA", !"Punjabi"},
	{!"QU", !"Quechua"},     {!"RM", !"Rhaeto-Rom"},  {!"RO", !"Romanian"},  {!"RU", !"Russian"},        {!"SM", !"Samoan"},      
	{!"SG", !"Sangro"},      {!"SA", !"Sanskrit"},    {!"SR", !"Serbian"},   {!"SH", !"Serbo-Croatian"}, {!"ST", !"Sesotho"},
	{!"TN", !"Setswana"},    {!"SN", !"Shona"},       {!"SD", !"Sindhi"},    {!"SI", !"Singhalese"},     {!"SS", !"Siswati"},     
	{!"SK", !"Slovak"},      {!"SL", !"Slovenian"},   {!"SO", !"Somali"},    {!"ES", !"Spanish"},        {!"SU", !"Sudanese"},
	{!"SW", !"Swahili"},     {!"SV", !"Swedish"},     {!"TL", !"Tagalog"},   {!"TG", !"Tajik"},          {!"TA", !"Tamil"},       
	{!"TT", !"Tatar"},       {!"TE", !"Tegulu"},      {!"TH", !"Thai"},      {!"BO", !"Tibetan"},        {!"TI", !"Tigrinya"},
	{!"TO", !"Tonga"},       {!"TS", !"Tsonga"},      {!"TR", !"Turkish"},   {!"TK", !"Turkmen"},        {!"TW", !"Twi"},         
	{!"UK", !"Ukrainian"},   {!"UR", !"Urdu"},        {!"UZ", !"Uzbek"},     {!"VI", !"Vietnamese"},     {!"VO", !"Volapuk"},
	{!"CY", !"Welsh"},       {!"WO", !"Wolof"},       {!"XH", !"Xhosa"},     {!"JI", !"Yiddish"},        {!"YO", !"Yoruba"},      
	{!"ZU", !"Zulu"}
};	


const this.MAX_LANGUAGES = 20;
const this.INVALID_LANGUAGE = -1;


stock
	         this.DefaultLanguageCode = _:LANG_EN,
	         this.DefaultLanguage = 0,
	         this.Strings[this.MAX_LANGUAGES][{#LANGUAGES_NUM_STRINGS#}][1],
	         this.Descriptions[{#LANGUAGES_NUM_STRINGS#}][1],
	         this.ActiveLanguage = -1,
	         this.CurrentPlayer = INVALID_PLAYER_ID,
	         this.PreviousPlayer = INVALID_PLAYER_ID,
	    bool:this.HasPreviousPlayer = false,
	         this.PlayerLanguage[MAX_PLAYERS],
	BitArray:this.HasSelectedLanguage<MAX_PLAYERS>,
	         this.Languages = 0,
	         this.LanguageNames[this.MAX_LANGUAGES],
	         this.TextBuffer[2048],
	         this.ResetActiveLanguageTimer = -1,
	    bool:this.EnableLanguageSelection = true,
	         this.GroupName[Group:MAX_GROUPS]
;


RegisterConfigVariable: this.EnableLanguageSelection;


forward              this.LoadLanguage(const filename[], lang_code);
forward              this.GetStringIndex(const string[], &lang = 0);
forward              this.ParseStringLiteral(output[], const input[], start = 0);
forward              this.Prepare(index, forplayer = INVALID_PLAYER_ID);
forward              this.SetActivePlayer(playerid);
forward              this.SetActiveLanguage(lang);
forward         bool:this.SetPlayerLanguage(playerid, lang);
forward              this.GetPlayerLanguage(playerid);
forward              this.CheckStringArgument(arg, playerid);
forward              this.GetStringAddress(const string[], playerid = INVALID_PLAYER_ID, lang = -1);
forward         bool:this.IsTranslatable(const string[]);
forward              this.DeferredResetActiveLanguage();
forward              this.ResetActiveLanguage();
forward              this.GetLanguageFromLanguageCode(this.LANGUAGE_CODE:code);
forward              this.ShowsLanguageSelection();
forward              this.ShowLanguageSelection(playerid);
forward                   SendClientMessageToAllExcept(playerid, color, message[], va_args<>);

// Hooks
forward              this.SendClientMessageToAll(color, message[], va_args<>);
forward              this.SendClientMessage(playerid, color, message[], va_args<>);
forward         Text:this.CreatePlayerTextDraw(playerid, Float:x, Float:y, text[], va_args<>);
forward              this.PlayerTextDrawSetString(playerid, PlayerText:text, string[], va_args<>);
forward              this.SendPlayerMessageToAll(senderid, const message[], va_args<>);
forward              this.SendPlayerMessageToPlayer(playerid, senderid, const message[], va_args<>);
forward              this.GameTextForAll(const string[], time, style, va_args<>);
forward              this.GameTextForPlayer(playerid, const string[], time, style, va_args<>);
forward PlayerText3D:this.CreatePlayer3DTextLabel(playerid, text[], color, Float:x, Float:y, Float:z, Float:DrawDistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testLOS = 0, va_args<>);
forward              this.UpdatePlayer3DTextLabelText(playerid, PlayerText3D:id, color, text[], va_args<>);
forward              this.ShowPlayerDialog(playerid, dialogid, style, caption[], info[], button1[], button2[]);
forward              this.SetPlayerObjectMaterialText(playerid, objectid, text[], materialindex = 0, materialsize = OBJECT_MATERIAL_SIZE_256x128, fontface[] = "Arial", fontsize = 24, bold = 1, fontcolor = 0xFFFFFFFF, backcolor = 0, textalignment = 0);
forward              this.Dialog_ShowCallback(playerid, callback:callback, style, string:title[], string:caption[], string:button1[], string:button2[] = "", dialog = -1);
forward              this.Dialog_ShowCallbackData(playerid, callback[E_CALLBACK_DATA], style, string:title[], string:caption[], string:button1[], string:button2[] = "", dialog = -1);
forward              this.Dialog_Show(playerid, style, string:title[], string:caption[], string:button1[], string:button2[] = "", dialog = -1);
forward              this.Group_SetName(Group:g, name[]);
forward              this.Group_GetName(Group:g);