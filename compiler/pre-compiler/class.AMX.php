<?php
class AMX {
	private $file;
	
	public $header;
	public $header_raw;
	public $debug;
	
	const IDENT_VARIABLE  = 1;
	const IDENT_REFERENCE = 2;
	const IDENT_ARRAY     = 3;
	const IDENT_REFARRAY  = 4;
	const IDENT_FUNCTION  = 9;
	
	const AMX_MAGIC = 0xF1E0;
	
	public function __construct($fname) {
		$this->file = realpath($fname);
		
		$this->load();
	}
	
	private function load() {
		$pos = 0;
		
		if (!($fp = fopen($this->file, 'rb')))
			trigger_error("Unable to read from file: \"$this->file\".", E_USER_ERROR);
		
		// Read the header
		$data = fread($fp, 56);
		
		$this->header = (object) unpack(implode('/', array(
			"@$pos",
			'Vsize',             /* size of the "file" */
			'vmagic',            /* signature */
			'cfile_version',     /* file format version */
			'camx_version',      /* required version of the AMX */
			'vflags',            /* flags */
			'vdefsize',          /* size of a definition record */
			'Vcod',              /* initial value of COD - code block */
			'Vdat',              /* initial value of DAT - data block */
			'Vhea',              /* initial value of HEA - start of the heap */
			'Vstp',              /* initial value of STP - stack top */
			'Vcip',              /* initial value of CIP - the instruction pointer */
			'Voffset_publics',   /* offset to the "public functions" table */
			'Voffset_natives',   /* offset to the "native functions" table */
			'Voffset_libraries', /* offset to the table of libraries */
			'Voffset_pubvars',   /* the "public variables" table */
			'Voffset_tags',      /* the "public tagnames" table */
			'Voffset_nametable', /* name table */
		)), $data);

		if ($this->header->magic != self::AMX_MAGIC) {
			fclose($fp);

			trigger_error(sprintf('Invalid magic: %04x.', $this->header->magic), E_USER_WARNING);

			return;
		}
		
		$data .= fread($fp, $this->header->cod - 56);
		
		$this->header_raw = $data;
		
		$this->header->num_publics   = ($this->header->offset_natives - $this->header->offset_publics) / $this->header->defsize;
		$this->header->num_natives   = ($this->header->offset_libraries - $this->header->offset_natives) / $this->header->defsize;
		$this->header->num_libraries = ($this->header->offset_pubvars - $this->header->offset_libraries) / $this->header->defsize;
		$this->header->num_pubvars   = ($this->header->offset_tags - $this->header->offset_pubvars) / $this->header->defsize;
		$this->header->num_tags      = ($this->header->offset_nametable - $this->header->offset_tags) / $this->header->defsize;
		$this->header->publics       = array();
		$this->header->natives       = array();
		$this->header->libraries     = array();
		$this->header->pubvars       = array();
		$this->header->tags          = array();
		
		$pos = $this->header->offset_nametable;
		
		extract(unpack("@$pos/vname_size", $data));
		
		$pos += 2;
		
		$this->header->name_size = $name_size;
		
		$_names = explode("\0", trim(substr(
			$data,
			$pos,
			$this->header->cod - $pos
		), "\0"));
		
		$names = array();
		
		foreach ($_names as $i => &$name) {
			$names[$pos] = &$name;
			
			$pos += strlen($name) + 1;
		}
		
		$pos = $this->header->offset_publics;
		
		foreach (array('publics', 'natives', 'libraries', 'pubvars', 'tags') as $entry) {
			$numvar = "num_$entry";
			
			for ($i = 0; $i < $this->header->$numvar; $i++) {
				$info = (object) unpack("@$pos/Vvalue/Vname", $data);

				$pos += 8;
				
				if (!isset($names[$info->name])) {
					echo "WARNING: Invalid name table offset $info->name ($info->value).\n";
					
					continue;
				}

				$info->name = $names[$info->name];
				
				array_push($this->header->$entry, $info);
			}
		}
		
		unset($names);
		unset($_names);
		
		// Is there debug information?
		if (filesize($this->file) > $this->header->size) {
			fseek($fp, $this->header->size);
			
			$data = fread($fp, filesize($this->file) - $this->header->size);
			
			$pos = 0;
			
			$this->debug = (object) unpack(implode('/', array(
				"@$pos",
				'Vsize',           /* size of the debug information chunk */
				'vmagic',          /* signature, must be 0xf1ef */
				'cfile_version',   /* file format version */
				'camx_version',    /* required version of the AMX */
				'vflags',          /* currently unused */
				'vnum_files',      /* number of entries in the "file" table */
				'vnum_lines',      /* number of entries in the "line" table */
				'vnum_symbols',    /* number of entries in the "symbol" table */
				'vnum_tags',       /* number of entries in the "tag" table */
				'vnum_automatons', /* number of entries in the "automaton" table */
				'vnum_states',     /* number of entries in the "state" table */
			)), $data);
		
			$pos += 22;
		
			$this->debug->files      = array();
			$this->debug->lines      = array();
			$this->debug->symbols    = array();
			$this->debug->tags       = array();
			$this->debug->automatons = array();
			$this->debug->states     = array();
		
			for ($i = 0; $i < $this->debug->num_files; $i++) {
				$file = (object) unpack("@$pos/Vaddress/a1024name", $data);
				$file->name = strstr($file->name, "\0", true);
			
				$pos += 4 + strlen($file->name) + 1;
			
				$this->debug->files[] = $file;
			}
		
			for ($i = 0; $i < $this->debug->num_lines; $i++) {
				$line = unpack("@$pos/Vaddress/Vline", $data);
			
				$this->debug->lines[$line['address']] = $line['line'];
			
				$pos += 8;
			}
		
			for ($i = 0; $i < $this->debug->num_symbols; $i++) {
				$symbol = (object) unpack(implode('/', array(
					"@$pos",
					'Vaddress',   /* address in the data segment or relative to the frame */
					'vtag',       /* tag for the symbol */
					'Vcodestart', /* address in the code segment from which this symbol is valid (in scope) */
					'Vcodeend',   /* address in the code segment until which this symbol is valid (in scope) */
					'cident',     /* kind of symbol (function/variable) */
					'cvclass',    /* class of symbol (global/local) */
					'vdim',       /* number of dimensions */
				)), $data);
				
				$pos += 4+2+4+4+1+1+2;
				
				$symbol->name = substr($data, $pos, 64);
				$symbol->name = strstr($symbol->name, "\0", true);
			
				$pos += strlen($symbol->name) + 1;
			
				if ($symbol->dim) {
					$dim = array();
				
					for ($j = 0; $j < $symbol->dim; $j++) {
						$dim[] = (object) unpack("@$pos/vtag/Vsize", $data);
					
						$pos += 2 + 4;
					}
				
					$symbol->dim = $dim;
				} else {
					$symbol->dim = array();
				}
			
				$this->debug->symbols[] = $symbol;
			}
		
			for ($i = 0; $i < $this->debug->num_tags; $i++) {
				$tag = (object) unpack("@$pos/vid", $data);
			
				$pos += 2;
			
				$tag->name = substr($data, $pos, 64);
				$tag->name = strstr($tag->name, "\0", true);
			
				$pos += strlen($tag->name) + 1;
			
				$this->debug->tags[$tag->id] = $tag->name;
			}
		
			for ($i = 0; $i < $this->debug->num_automatons; $i++) {
				$automaton = (object) unpack("@$pos/vid/Vaddress", $data);
			
				$pos += 2 + 4;
			
				$automaton->name = substr($data, $pos, 64);
				$automaton->name = strstr($automaton->name, "\0", true);
			
				$pos += strlen($automaton->name) + 1;
			
				$this->debug->automatons[$automaton->id] = (object) array(
					'address' => $automaton->address,
					'name'    => $automaton->name
				);
			}
		
			for ($i = 0; $i < $this->debug->num_states; $i++) {
				$state = (object) unpack("@$pos/vid/vautomaton", $data);
			
				$pos += 2 + 2;
			
				$state->name = substr($data, $pos, 64);
				$state->name = strstr($state->name, "\0", true);
			
				$pos += strlen($state->name) + 1;
			
				$this->debug->states[] = $state;
			}
		}
		
		fclose($fp);
	}
	
	public function save() {
		$tables = array_fill_keys(array('publics', 'natives', 'libraries', 'pubvars', 'tags'), array());
		$nametable = pack('v', $this->header->name_size);
		$offsets = array();
		$offset = 56; // size of the static data in the header
		
		// TODO: String pooling for the nametable entries
		foreach ($tables as $table_name => &$table) {
			$offsets[$table_name] = $offset;
			
			foreach ($this->header->$table_name as $entry) {
				$table[] = (object) array(
					'name_offset' => strlen($nametable),
					'name' => $entry->name,
					'value' => $entry->value
				);
				
				if (strlen($entry->name) > $this->header->name_size) {
					echo "WARNING: The entry for $entry->name in the $table_name table will be truncated to 31 characters.";
					
					$nametable .= substr($entry->name, 0, $this->header->name_size) . "\0";
				} else {
					$nametable .= "$entry->name\0";
				}
				
				$offset += 8;
			}
			
			if ($table_name != 'natives') {
				usort($table, function ($left, $right) {
					return strcmp($left->name, $right->name);
				});
			}
		}
		
		$nametable .= "\0";
		
		$offsets['nametable'] = $offset;
		
		// Calculate the by how much the size of the prefix ($this->header) has changed
		$sizediff = ($offset + strlen($nametable)) - $this->header->cod;
		
		$this->header->size += $sizediff;
		$this->header->cod  += $sizediff;
		$this->header->dat  += $sizediff;
		$this->header->hea  += $sizediff;
		$this->header->stp  += $sizediff;
		
		$header = pack(
			'VvccvvVVVVVVVVVVV',
			$this->header->size,
			$this->header->magic,
			$this->header->file_version,
			$this->header->amx_version,
			$this->header->flags,
			$this->header->defsize,
			$this->header->cod,
			$this->header->dat,
			$this->header->hea,
			$this->header->stp,
			$this->header->cip,
			$offsets['publics'],
			$offsets['natives'],
			$offsets['libraries'],
			$offsets['pubvars'],
			$offsets['tags'],
			$offsets['nametable']
		);
		
		foreach ($tables as &$table) {
			foreach ($table as &$entry) {
				$entry->name_offset += $offsets['nametable'];
				
				$header .= pack('VV', $entry->value, $entry->name_offset);
			}
		}
		
		$header .= $nametable;
		
		if ($header !== $this->header_raw) {
			if ($sizediff > 0)
				file_put_contents($this->file, str_repeat("\0", $sizediff) . file_get_contents($this->file));
			else if ($sizediff < 0)
				file_put_contents($this->file, substr(file_get_contents($this->file), -$sizediff));
			
			if (($fp = fopen($this->file, 'cb'))) {
				fwrite($fp, $header);
		
				fclose($fp);
			} else {
				return false;
			}
		}
		
		if ($this->debug) {
			$this->debug->num_files      = count($this->debug->files);
			$this->debug->num_lines      = count($this->debug->lines);
			$this->debug->num_symbols    = count($this->debug->symbols);
			$this->debug->num_tags       = count($this->debug->tags);
			$this->debug->num_automatons = count($this->debug->automatons);
			$this->debug->num_states     = count($this->debug->states);
			
			$debug_output = pack(
				'vccvvvvvvv',
//				$this->debug->size, <- Will be written lastly
				$this->debug->magic,
				$this->debug->file_version,
				$this->debug->amx_version,
				$this->debug->flags,
				$this->debug->num_files,
				$this->debug->num_lines,
				$this->debug->num_symbols,
				$this->debug->num_tags,
				$this->debug->num_automatons,
				$this->debug->num_states
			);
		
			foreach ($this->debug->files as $file)
				$debug_output .= pack('V', $file->address) . "$file->name\0";
		
			foreach ($this->debug->lines as $address => $line)
				$debug_output .= pack('VV', $address, $line);
		
			foreach ($this->debug->symbols as $symbol) {
				$debug_output .= pack(
					'VvVVccv',
					 $symbol->address,
					 $symbol->tag,
					 $symbol->codestart,
					 $symbol->codeend,
					 $symbol->ident,
					 $symbol->vclass,
					count($symbol->dim)
				) . "$symbol->name\0";
			
				foreach ($symbol->dim as $dim)
					$debug_output .= pack('vV', $dim->tag, $dim->size);
			}
		
			foreach ($this->debug->tags as $id => $tag)
				$debug_output .= pack('v', $id) . "$tag\0";
		
			foreach ($this->debug->automatons as $id => $automaton)
				$debug_output .= pack('vV', $id, $automaton->address) . "$automaton->name\0";
		
			foreach ($this->debug->states as $state)
				$debug_output .= pack('vv', $state->id, $state->automaton) . "$state->name\0";
		
			if (($fp = fopen($this->file, 'ab'))) {
				fseek($fp, $this->header->size);
		
				ftruncate($fp, $this->header->size);
		
				fwrite($fp, pack('V', strlen($debug_output)));
				fwrite($fp, $debug_output);
		
				fclose($fp);
			} else {
				return false;
			}
		}
		
		return true;
	}
}