/*!
 * Languages/functions.inc
 *
 > Priority: 0
 */

stock this.LoadLanguage(const filename[]) {
	new File:fp;
	
	if (fexist(filename) && (fp = fopen(filename, io_read))) {
		new
			buf[2048 char],
			string[1024 char],
			description[1024 char],
			translation[1024 char],
			lang = this.Languages++,
			line = 0
		;
		
		while (fread(fp, buf, .pack = true)) {
			if (++line <= 2)
				continue;
			
			trim(buf, .edges = trim_right);
			
			if ((buf[0] & 0xFFFFFF00) == 0x2f2f2000) // if buf[0,1,2] == "// "
				strmid(description, buf, 3, cellmax);
			else if (buf{0} == '"') {
				new end = this.ParseStringLiteral(string, buf);
				new start = strfind(buf, "\"", _, end + 1);
				
				this.ParseStringLiteral(translation, buf, start);
				
				for (new i = 0; i < sizeof(this.Strings[]); i++) {
					if (strequals(this.Strings[0][i], string) && strequals(this.Descriptions[0][i], description)) {
						RedirectArraySlot(this.Strings[lang], i, ref(mstr(Malloc_NewS(translation))));
						
						if (IsNull(description))
							RedirectArraySlot(this.Descriptions[lang], i, ref(""));
						else
							RedirectArraySlot(this.Descriptions[lang], i, ref(mstr(Malloc_NewS(description))));
						
						break;
					}
				}
				
				description{0} = '\0';
			}
		}
		
		fclose(fp);
	}
}

stock this.ParseStringLiteral(output[], const input[], start = 0) {
	new bool:in_escape = false;
	new i = 0, j, c;
	
	output{i} = '\0';
	
	for (j = start + 1; (c = input{j}); j++) {
		if (c == '\\') {
			if (in_escape) {
				in_escape = false;
				
				output{i++} = '\\';
			} else
				in_escape = true;
		} else if (in_escape) {
			switch (c) {
				case '"': output{i++} = c;
				case 'a': output{i++} = '\a';
				case 'b': output{i++} = '\b';
				case 'e': output{i++} = '\e';
				case 'r': output{i++} = '\r';
				case 'n': output{i++} = '\n';
				case 't': output{i++} = '\t';
				case 'v': output{i++} = '\v';
			}
			
			in_escape = false;
		} else {
			if (c == '"') {
				output{i++} = '\0';
				
				break;
			} else
				output{i++} = c;
		}
	}
	
	return j;
}

stock this.Prepare(forplayer = INVALID_PLAYER_ID) {
	if (forplayer == INVALID_PLAYER_ID) {
		if (this.PreviousPlayer != INVALID_PLAYER_ID) {
			this.CurrentPlayer = this.PreviousPlayer;
			this.PreviousPlayer = INVALID_PLAYER_ID;
		}
	} else {
		if (this.PreviousPlayer == INVALID_PLAYER_ID)
			this.PreviousPlayer = forplayer;

		this.CurrentPlayer = forplayer;
	}
	
	if (this.CurrentPlayer == INVALID_PLAYER_ID)
		this.ActiveLanguage = 0;
	else
		this.ActiveLanguage = this.PlayerLanguage[this.CurrentPlayer];
}