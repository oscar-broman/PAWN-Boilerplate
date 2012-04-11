<?php
class AMX {
	private $file;
	
	public $header;
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
		
		$data .= fread($fp, $this->header->cod);
		
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
		
		$pos = $this->header->offset_publics;
		
		foreach (array('publics', 'natives', 'libraries', 'pubvars', 'tags') as $entry) {
			$numvar = "num_$entry";
			
			for ($i = 0; $i < $this->header->$numvar; $i++) {
				$info = (object) unpack("@$pos/Vaddress/Vname", $data);

				$pos += 8;

				$info->name = substr($data, $info->name, 64);
				$info->name = strstr($info->name, "\0", true);
				
				// Strange PHP behavior lead me to this..
				$ar = &$this->header->$entry;
				$ar[$info->name] = $info->address;
			}
		}
		
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
				'vnum_flags',      /* currently unused */
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
		if ($this->debug) {
			$debug_output = pack(
				'Vvccvvvvvvv',
				$this->debug->size,
				$this->debug->magic,
				$this->debug->file_version,
				$this->debug->amx_version,
				$this->debug->num_flags,
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
		
			if (($fp = fopen($this->file, 'a'))) {
				fseek($fp, $this->header->size);
		
				ftruncate($fp, $this->header->size);
		
				fwrite($fp, $debug_output);
		
				fclose($fp);
			} else {
				return false;
			}
		}
		
		return true;
	}
}