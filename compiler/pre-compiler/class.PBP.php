<?php
require 'class.SyntaxIntel.php';
require 'class.PAWNC.php';
require 'class.AMX.php';

class PBP {
    public $output;
    
	private $is_windows;
    private $syntax_intel;
	public $modules;
	private $cfg = array();
	
	public function __construct() {
		$this->is_windows = (strpos(PHP_OS, 'WIN') !== false);
		
		foreach (array('compiler', 'gamemodes', 'gamemodes/modules', 'include') as $dir)
			if (!is_dir($dir)) trigger_error("Unable to locate essential directory: $dir", E_USER_ERROR);
		
		if (!file_exists('server.cfg')) {
			$cfg_default = <<<EOD
echo Executing Server Config...
lanmode 0
rcon_password changeme
maxplayers 32
port 7777
hostname SA-MP 0.3 Server
gamemode0 main 1
filterscripts fix_OnRconCommand
announce 0
query 1
weburl www.sa-mp.com
onfoot_rate 40
incar_rate 40
weapon_rate 40
stream_distance 300.0
stream_rate 1000
maxnpc 0
logtimeformat [%H:%M:%S]
plugins crashdetect sscanf whirlpool

;PAWN Boilerplate settings
debug_level 2
mode_name PBP Gamemode

EOD;
			
			file_put_contents('server.cfg', $cfg_default);
			
			echo "Created \"server.cfg\".\n";
		}
		
		$this->cfg['debug_level'] = 2;
		$this->cfg['wrap_callbacks'] = 0;
		
		foreach (file('server.cfg') as $row) {
			$row = trim($row);
			
			if (empty($row) || preg_match('/^(;|#|echo)/', $row))
				continue;
			
			list($key, $value) = preg_split('/\s+/', $row, 2);
			
			$key = strtolower($key);
			
			if (in_array($key, array('lanmode', 'maxplayers', 'port', 'announce', 'query', 'onfoot_rate', 'incar_rate', 'weapon_rate', 'stream_rate', 'maxnpc', 'debug_level', 'wrap_callbacks'))) {
				$value = (int) $value;
				
				if ($key == 'debug_level' && ($value < 0 || $value > 3)) {
					echo "PBP Warning: Invalid server.cfg value for debug_level.\n";
					
					continue;
				}
			}
			
			if (in_array($key, array('stream_distance')))
				$value = (float) $value;
			
			$this->cfg[$key] = $value;
		}
	}
	
	private function generate_main() {
		$this->syntax_intel = new SyntaxIntel();
		$this->syntax_intel->parse_directory('YSI/pawno/include/YSI', array());
		
		foreach ($this->syntax_intel->data->publics as $public => $args) {
			if (isset($this->syntax_intel->data->callbacks[$public]))
				unset($this->syntax_intel->data->callbacks[$public]);
		}
		
		$this->syntax_intel->parse_directory('include', array(
			'ignore' => array('a_npc.inc')
		));
		
		$modules = array();
		$modules_folder = array();
		
		foreach (array_merge(glob('gamemodes/modules/*'), glob('gamemodes/modules/PBP/*')) as $dir) {
			if (!is_dir($dir))
				continue;
			
			$basename = basename($dir);
			
			if ($basename == 'PBP')
				continue;
			
			$dirname = preg_replace('/^gamemodes\/modules\/(.+?\/)?[^\/]+$/', '$1', $dir);
			
			if (!preg_match('/^[a-z@_][a-z0-9_@]*$/i', $basename)) {
				echo "PBP Error: Invalid module name: \"$basename\".";
				
				exit;
			}
			
			$modules[] = $basename;
			$modules_folder[$basename] = $dirname;
		}
		
		$this->modules = $modules;
		
		$module_includes = array(
			'header'    => array(),
			'callbacks' => array(),
			'functions' => array(),
			'commands'  => array(),
		);
		
		$callback_includes = array();
		
		$callbacks = $this->syntax_intel->data->callbacks;
		$this->syntax_intel->data->callbacks = array();
		
		foreach (array_merge(glob('gamemodes/modules/*/callbacks.inc'), glob('gamemodes/modules/PBP/*/callbacks.inc')) as $file) {
			$contents = file_get_contents($file);
			
			if (preg_match('/^\s*public\s+/m', $contents)) {
				echo "PBP Error: public functions are not allowed in callbacks.inc ($file).";
				
				
				exit;
			}
			
			unset($contents);
			
			$this->syntax_intel->parse_file($file);
		}
		
		$custom_callbacks = $this->syntax_intel->data->callbacks;
		$callbacks = array_merge($callbacks, $custom_callbacks);
		
		$callbacks['main'] = '';
		
		foreach ($modules as $module_index => $module) {
			if (!file_exists("gamemodes/modules/{$modules_folder[$module]}$module/callbacks"))
				mkdir("gamemodes/modules/{$modules_folder[$module]}$module/callbacks");
			
			foreach (array_keys($module_includes) as $incfile) {
				$file = "gamemodes/modules/{$modules_folder[$module]}$module/$incfile.inc";
				
				if (!file_exists($file))
					touch($file);
				
				$this->process_file($file, $module_index);
				
				$info = $this->file_header($file, array(), array(
					'Priority' => 0
				));
				
				if (!empty($info['Requires'])) {
					$requires = preg_split('/\s*,\s*/', $info['Requires']);
					
					foreach ($requires as $reqmodule) {
						if (!in_array($reqmodule, $modules)) {
							echo "PBP Error: Non-existing module \"$reqmodule\" is required by file: \"modules/$module/$incfile.inc\".";
							
							exit;
						}
					}
				}
				
				$dirname = str_replace('/', '\\', $modules_folder[$module]) . $module;
				
				$module_includes[$incfile][] = (object) array(
					'module'       => $module_index,
					'include_path' => ".build\\modules\\$dirname\\$incfile",
					'priority'     => isset($info['Priority']) ? (int) $info['Priority'] : 0,
				);
			}
		}
		
		$active_callbacks = array();
		
		foreach ($modules as $module_index => $module) {
			foreach (glob("gamemodes/modules/{$modules_folder[$module]}$module/callbacks/*.inc") as $file) {
				$cbfile = basename($file);
				
				$this->process_file($file, $module_index);
				
				$callback = preg_replace('/(_.*?)?\..*?$/', '', $cbfile);
				$suffix = '';
				
				if (preg_match('/(_.+?)\./', $cbfile, $matches))
					$suffix = $matches[1];
				
				if (!isset($callbacks[$callback])) {
					$fileshort = preg_replace('/^gamemodes(\\\|\/)/', '', $file);
					
					if (!isset($lowercallbacks, $callbacks_keys)) {
						$callbacks_keys = array_keys($callbacks);
						$lowercallbacks = array_map('strtolower', $callbacks_keys);
					}
					
					if (false !== ($idx = array_search(strtolower($callback), $lowercallbacks))) {
						$callback = $callbacks_keys[$idx];
						
						echo "PBP Notice: Renaming $fileshort to \"$callback.inc\" (correct case).\n";
						
						$newfile = "gamemodes/modules/{$modules_folder[$module]}$module/callbacks/$callback$suffix.inc";
						
						rename($file, $newfile);
						
						$cbfile = basename($newfile);
					} else {
						echo "PBP Warning: Unknown callback \"$callback\" (file: $fileshort).\n";
					
						continue;
					}
				}

				$active_callbacks[$callback] = true;
				
				$info = $this->file_header($file, array(
					
				), array(
					'Priority' => 0
				), array(
					"$callback({$callbacks[$callback]})"
				));
				
				if (!empty($info['Requires'])) {
					$requires = preg_split('/\s*,\s*/', $info['Requires']);
					
					foreach ($requires as $module) {
						if (!in_array($module, $modules)) {
							echo "PBP Error: Non-existing module \"$module\" is required by file: \"modules/$module/callbacks/$callback.inc\".";
							
							exit;
						}
					}
				}
				
				if (!isset($callback_includes[$callback]))
					$callback_includes[$callback] = array();
				
				$wrap = @$this->cfg['wrap_callbacks'] || @$info['Wrap'];
				
				$wrapfunc = $wrap ? "$module.$callback$suffix" : null;
				
				$dirname = str_replace('/', '\\', $modules_folder[$module]) . $module;
				
				$callback_includes[$callback][] = (object) array(
					'module'       => $module_index,
					'include_path' => ".build\\modules\\$dirname\\callbacks\\$callback$suffix",
					'priority'     => isset($info['Priority']) ? (int) $info['Priority'] : 0,
					'wrap'         => $wrap,
					'wrapfunc'     => $wrapfunc,
				);
			}
		}
		
		$text_header = 'gamemodes/.build/modules/PBP/Text/header.inc';
		
		if (file_exists($text_header)) {
			file_put_contents($text_header, str_replace('{#LANGUAGES_NUM_STRINGS#}', max(1, count($this->translatable_strings)), file_get_contents($text_header)));
			
			$default_values = "new _adr;\n";
			
			foreach ($this->translatable_strings as $index => $string) {
				$default_values .= <<<EOD
	_adr = ref("\\1{$string['string']}");
	@ptr[_adr] = (i << 16) | $index;
	
	RedirectArraySlot(this.Strings[i], $index, _adr + 4);
	RedirectArraySlot(this.Descriptions, $index, ref(!"{$string['description']}"));

EOD;
			}
			
			$text_ogmi = 'gamemodes/.build/modules/PBP/Text/callbacks/OnGameModeInit.inc';
			
			file_put_contents($text_ogmi, str_replace('{#LANG_DEFAULT_VALUES#}', $default_values, file_get_contents($text_ogmi)));
		}
		
		if (!file_exists('scriptfiles/languages'))
			mkdir('scriptfiles/languages');
		
		$langfiles = glob('scriptfiles/languages/*.lang.inc');
		$langfiles[] = ($template = 'scriptfiles/languages/TEMPLATE.lang.inc');
		
		file_put_contents($template, '');
		
		foreach ($langfiles as $file) {
			$existing_data = array();
			$strings = $this->translatable_strings;
			
			foreach ($strings as $i => &$string) {
				$string = (object) $string;
				
				$string->index = count($this->translatable_strings) + $i;
			}
			
			unset($string);
			
			if (preg_match_all('/(?:^\/\/[\t ]*(.*?)[\t ]*\r?\n)?"((?:[^"\\\\]|\\\\.)*)"[\t ]*=[\t ]*"((?:[^"\\\\]|\\\\.)*)"/m', file_get_contents($file), $matches, PREG_SET_ORDER)) {
				foreach ($matches as $i => $match) {
					$found = false;
					
					foreach ($strings as &$string) {
						if ($string->string == $match[2] && $string->description == $match[1]) {
							$string->translation = $match[3];
							$string->index = $i;
							
							$found = true;
							
							break;
						}
					}
					
					unset($string);
					
					if (!$found) {
						$strings[] = (object) array(
							'string'      => $match[2],
						    'player'      => '',
						    'description' => $match[1],
						    'index'       => $i,
						    'translation' => $match[3]
						);
					}
				}
			}
			
			usort($strings, function ($left, $right) {
				return $left->index - $right->index;
			});
			
			if (false !== ($fp = fopen($file, 'w'))) {
				fwrite($fp, "// Please be very careful with the layout of this file. Only change strings on the right side.\n// Do not change comments or strings on the left side.");
				
				foreach ($strings as $string) {
					if (!isset($string->translation))
						$string->translation = $string->string;
					
					fwrite($fp, "\n\n");
					
					if ($string->description)
						fwrite($fp, "// " . $string->description . "\n");
					
					fwrite($fp, "\"$string->string\" = \"$string->translation\"");
				}
				
				fwrite($fp, "\n");
				
				fclose($fp);
			}
		}
		
		foreach ($module_includes as $incfile => &$includes) {
			$priority = array();
			
			foreach ($includes as $k => &$include)
				$priority[$k] = $include->priority;
			
			array_multisort($priority, SORT_DESC, $includes);
		}
		
		foreach ($callback_includes as $callback => &$callback_include) {
			$priority = array();
			
			foreach ($callback_include as $k => &$v)
				$priority[$k] = $v->priority;
			
			array_multisort($priority, SORT_DESC, $callback_include);
		}
		
		$module_list = '';
		
		foreach ($modules as $module)
			$module_list .= "\n\t* $module";
		
		$custom_callback_list = '';
		
		asort($custom_callbacks);
		
		foreach ($custom_callbacks as $callback => $arguments) {
			$custom_callback_list .= "\n\t" . (isset($active_callbacks[$callback]) ? '+' : ' ') . " $callback($arguments)";
		}
		
		$callback_list = '';
		
		asort($callbacks);
		
		foreach ($callbacks as $callback => $arguments) {
			$callback_list .= "\n\t" . (isset($active_callbacks[$callback]) ? '+' : ' ') . " $callback($arguments)";
		}
		
		$timestamp = time();
		
		$buildinfo = <<<EOD
Modules: $module_list

Custom callbacks: $custom_callback_list
EOD;
		
		file_put_contents('gamemodes/build-info.txt', $buildinfo);
		
		$output = <<<EOD
/*
  NOTE: This file is generated by "compiler/compile". Any changes made to it will be lost next compilation.
*/

/****************************************************************************
                           WARNING! Headache below.
 ***************************************************************************/

#if !defined IN_COMPILE_SCRIPT
	#error You must use the compile script in the "compiler" folder.
#endif

EOD;
		$module_name_max = 0;
		$num_modules = count($modules);
		$module_array = '';
		$first = true;
		$module_prefixes = '';
		
		foreach ($modules as $module_index => $module) {
			if (($len = strlen($module)) > $module_name_max)
				$module_name_max = $len;
			
			if ($first)
				$first = false;
			else
				$module_array .= ',';
			
			$module_array .= "\n\t\t{\"$module\"}";
			
			$module_prefixes .= <<<EOD
#define $module. M{$module_index}@
#define M{$module_index}@INDEX  $module_index
#define M{$module_index}@OFFSET  (($module_index + 1) * PBP.MODULE_OFFSET_MULTIPLIER)

EOD;
		}
		
		$module_name_max += 1;
		
		if ($num_modules)
			$module_array .= "\n\t";
		
		$module_inclusions = '';
		
		foreach ($module_includes as $incfile => &$includes) {
			$module_inclusions .= "\n// $incfile.inc\n\n";
			
			foreach ($includes as &$include) {
				$module_inclusions .= <<<EOD
#define this. {$modules[$include->module]}.
#if defined _inc_$incfile
	#undef _inc_$incfile
#endif
#include "{$include->include_path}"
#undef _inc_$incfile
#undef this

EOD;
			}
		}
		
		if (isset($active_callbacks['main']))
			$public_functions = '';
		else
			$public_functions = <<<EOD

main() {}

EOD;
		
		foreach ($callback_includes as $callback => &$callback_include) {
			if (!isset($active_callbacks[$callback]))
				continue;
			
			$ALSdef = preg_replace('/^On/', '', $callback);
			
			if ($callback == 'main') {
				$pub = '';
			} else {
				$pub = 'public ';
				
				$public_functions .= <<<EOD

#if !defined $callback
	#error Callback  "$callback" is used, but not defined. Did you forget to include it?
#endif

EOD;
			}
			
			foreach ($callback_include as $k => &$v) {
				if ($v->wrap) {
					$incdef = '_inc_' . substr(preg_replace('/.+\\\\/', '', $v->include_path), 0, 25);
				
					$public_functions .= <<<EOD

forward $v->wrapfunc({$callbacks[$callback]});
public $v->wrapfunc({$callbacks[$callback]}) {
	#define this. {$modules[$v->module]}.
	#if defined $incdef
		#undef $incdef
	#endif
	#include "{$v->include_path}"
	#undef $incdef
	#undef this
	
	return 0;
}

EOD;
				}
			}
			
			$public_functions .= <<<EOD

$pub$callback({$callbacks[$callback]}) {
	#if defined DR@$callback
		PBP.ReturnValue = PBP.DEFAULT_RETURN<$callback>;
	#elseif defined ALS_R_$ALSdef
		PBP.ReturnValue = ALS_R_$ALSdef;
	#else
		PBP.ReturnValue = 1;
	#endif
	
	#if defined playerid
		Text.SetActivePlayer(playerid);
	#endif


EOD;
			
			foreach ($callback_include as $k => &$v) {
				if ($v->wrap) {
					$wrapfunc = $v->wrapfunc;
					$wrapfunc = preg_replace_callback('/(.+?)\./', array($this, 'resolve_module_prefix'), $wrapfunc);
					
					$public_functions .= <<<EOD
	#if !defined arg
		new arg;
	#endif
	
	PBP.DoReturn = false;

	#emit LOAD.S.pri  8
	#emit STOR.S.pri  arg

	while (arg) {
		arg -= 4;

		#emit LCTRL      5
		#emit ADD.C      12
		#emit LOAD.S.alt arg
		#emit ADD
		#emit LOAD.I
		#emit PUSH.pri
	}

	#emit PUSH.S      8

	#emit LCTRL       6
	#emit ADD.C       28
	#emit PUSH.pri

	#emit CONST.pri   $wrapfunc // $v->wrapfunc
	#emit SCTRL       6
	
	if (PBP.DoReturn)
		return PBP.ReturnValue;


EOD;
				} else {
					$incdef = '_inc_' . substr(preg_replace('/.+\\\\/', '', $v->include_path), 0, 25);
				
					$public_functions .= <<<EOD
	#define this. {$modules[$v->module]}.
	#if defined $incdef
		#undef $incdef
	#endif
	#include "{$v->include_path}"
	#undef $incdef
	#undef this


EOD;
				}
			}

$public_functions .= <<<EOD
	return PBP.ReturnValue;
}

EOD;
		}
		
		$cfg = '';
		
		if (!empty($this->cfg['mode_name'])) {
			$mode_name = $this->cfg['mode_name'];
			$mode_name = str_replace('"', '\\"', $mode_name);
			
			$cfg .= <<<EOD
#define CFG_MODE_NAME "$mode_name"

EOD;
		}
		
		if (!empty($this->cfg['maxplayers'])) {
			$cfg .= <<<EOD
#define CFG_MAX_PLAYERS {$this->cfg['maxplayers']}

EOD;
		}
		
		$output .= <<<EOD

#define PBP.  PBP_
#define PBP_DEFAULT_RETURN<%1>  DR@%1

const PBP.COMPILATION_TIMESTAMP = $timestamp;

enum PBP.e_MODULE {
	Name[$module_name_max]
};

stock const
	PBP.Modules[][PBP.e_MODULE] = {{$module_array}}
;

stock
	bool:PBP.DoReturn,
	     PBP.ReturnValue
;

// Let's avoid touching anything we shouldn't
native PBP.print(const string[]) = print;
native PBP.strins(string[], const substr[], pos, maxlength=sizeof string) = strins;
native PBP.strdel(string[], start, end) = strdel;
native PBP.strfind(const string[], const sub[], bool:ignorecase=false, pos=0) = strfind;
native PBP.strval(const string[]) = strval;

forward PBP.ThisFunctionError(...);
public PBP.ThisFunctionError(...) {
	PBP.print(!"PBP Error: A public function was prefixed with \"this.\". Unable to determine where it points.");
	PBP.print(!"           Use the stringize operator or full module prefixes when dealing with public function names.");
}

stock PBP.ResolveSymbolName(name[], maxlength = sizeof(name)) {
	new pos, module_index = -1, bool:packed = (name[0] > 255);
	
	if (packed) {
		if (name{0} == 'M' && -1 < (pos = PBP.strfind(name, !"@", _, 2)) <= 4)
			name{0} = ' ', module_index = PBP.strval(name), name{0} = 'M';
	} else {
		if (name[0] == 'M' && -1 < (pos = PBP.strfind(name, !"@", _, 2)) <= 4)
			module_index = PBP.strval(name[1]);
	}
	
	if (0 <= module_index < sizeof(PBP.Modules)) {
		PBP.strdel(name, 0, pos);
		
		if (packed)
			name{0} = '.';
		else
			name[0] = '.';
		
		PBP.strins(name, PBP.Modules[module_index][Name], 0, maxlength);
	}
	
	return 1;
}

$cfg
$module_prefixes
#tryinclude "header"

#if defined _inc_header
	#undef _inc_header
#endif

#if !defined PBP_MODULE_OFFSET_MULTIPLIER
	const PBP.MODULE_OFFSET_MULTIPLIER = 1000;
#endif

$module_inclusions

$public_functions

EOD;
		
		file_put_contents('gamemodes/main.pwn', $output);
	}
	
	private function file_header($fname, array $new_info = array(), array $default_info = array(), array $extra = array()) {
		if (($fp = fopen($fname, 'rb'))) {
			if (fread($fp, 3) == '/*!') {
				$data = file_get_contents($fname);
				
				$exp = '/^\/\*!(.*?)\*\/[\r]?\n?/s';
				
				if (preg_match($exp, $data, $matches)) {
					$old_header = $matches[0];
					
					preg_match_all('/^\s*\>\s*(.*?)\s*\:\s*(.*?)\s*$/m', $matches[1], $matches, PREG_SET_ORDER);
					
					foreach ($matches as $match) {
						list(, $key, $value) = $match;
						
						if (!isset($new_info[$key]))
							$new_info[$key] = $value;
					}
					
					$data = preg_replace($exp, '', $data);
				}
			}
			
			fclose($fp);
			
			$info = array_merge($default_info, $new_info);
			
			$header = "/*!\n";
			$header .= ' * ' . preg_replace('/^gamemodes(\\\|\/)(modules(\\\|\/))?/', '', $fname) . "\n";
			$header .= " *\n";
			
			if (!empty($extra)) {
				foreach ($extra as $row) {
					$row = trim($row);
				
					$header .= " * $row\n";
				}
				
				$header .= " *\n";
			}
			
			$max_key = 0;
			
			foreach ($info as $k => $v) {
				if (($len = strlen($k)) > $max_key)
					$max_key = $len;
			}
			
			foreach ($info as $k => $v) {
				$header .= sprintf(" > %{$max_key}s: %s\n", $k, $v);
			}
			
			$header .= " */\n";
			
			if (!isset($old_header))
				$header .= "\n";
			
			if (!(isset($old_header) && trim($old_header) == trim($header))) {
				if (!isset($data))
					$data = file_get_contents($fname);
			
				file_put_contents($fname, $header . $data);
			}
		}
		
		return $new_info;
	}
	
	public function clean_directories() {
		if (!file_exists('gamemodes/.build'))
			return;
		
		$iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator('gamemodes/.build'), RecursiveIteratorIterator::CHILD_FIRST);
		
		foreach ($iterator as $path) {
			if ($path->isDir())
				rmdir("$path");
			else
				unlink("$path");
		}
	}
	
	private $in_module;
	private $command_descriptions = array();
	private $translatable_strings = array();
	
	private function process_file($file, $in_module = null) {
		$newfile = preg_replace('/gamemodes(\/|\\\)/i', 'gamemodes/.build/', $file);
		
		if (!file_exists('gamemodes/.build')) {
			mkdir('gamemodes/.build');
			
			if ($this->is_windows)
				system('attrib +H ' . escapeshellarg(realpath('gamemodes/.build')));
		}
		
		$this->in_module = $in_module;
		
		$contents = file_get_contents($file);
		
		if (preg_match_all('/CommandDescription\<(.*?)\>\s*=\s*@?"((?:[^"\\\\]|\\\\.)*)"\s*;/', $contents, $matches, PREG_SET_ORDER)) {
			foreach ($matches as $match) {
				$match[2] = str_replace('\\"', '"', $match[2]);
				
				$this->command_descriptions[$match[1]] = $match[2];
			}
		}
		
		$contents = preg_replace_callback('/^\s*#define\s+(this|' . implode('|', $this->modules) . ')\.([a-zA-Z0-9_@]+)/sm', array($this, 'module_prefix_macro'), $contents);
		$contents = preg_replace_callback('/^\s*#emit(\s+\S+\s+)(this|' . implode('|', $this->modules) . ')\.([a-zA-Z0-9_@]+)/sm', array($this, 'module_prefix_emit'), $contents);
		
		$contents = preg_replace_callback('/\b_([IH])(?:\<(.*?)\>|\((.*?)\))/', array($this, 'y_stringhash_regex_callback'), $contents);
		
		$contents = preg_replace_callback('/.(?:\<\s*([a-z_@][a-z0-9_@]*)\s*\>)?"((?:[^"\\\\]|\\\\.)*)"(?:\<\s*"((?:[^"\\\\]|\\\\.)*)"\s*\>)?/si', array($this, 'i18n_string_callback'), $contents);
		
		$newdir = dirname($newfile);
		
		if (!file_exists($newdir))
			mkdir($newdir, 0777, true);
		
		file_put_contents($newfile, $contents);
		
		$fp = fopen($newfile, 'wb');
		
		assert($fp !== null);
		
		fwrite($fp, "#file \"$file\"\n#line 0\n");
		fwrite($fp, $contents);
		fclose($fp);
	}
	
	public function i18n_string_callback($matches) {
		if ($matches[0]{0} !== '@')
			return $matches[0];
		
		if (isset($matches[3]))
			$matches[3] = trim((string) $matches[3]);
		else
			$matches[3] = null;
		
		foreach ($this->translatable_strings as $idx => $string) {
			if ($string['string'] == $matches[2] && $string['description'] == $matches[3]) {
				if ($matches[1])
					return "(_@lp($idx,{$matches[1]}),_@ls[_@lc][$idx])";
				else
					return "(_@lp($idx),_@ls[_@lc][$idx])";
			}
		}
		
		$idx = count($this->translatable_strings);
		
		$this->translatable_strings[] = array(
			'string'      => $matches[2],
			'player'      => $matches[1],
			'description' => $matches[3]
		);
		
		if ($matches[1])
			return "(_@lp($idx,{$matches[1]}),_@ls[_@lc][$idx])";
		else
			return "(_@lp($idx),_@ls[_@lc][$idx])";
	}
	
	public function y_stringhash_regex_callback($matches) {
		$characters = !empty($matches[2]) ? $matches[2] : str_replace(',', '', $matches[3]);
		$characters = preg_replace('/^this(?=\.)/', $this->modules[$this->in_module], $characters);
		$characters = strrev($characters);
		
		if ($matches[1] == 'I')
			$characters = strtoupper($characters);
		
		$characters = explode(',', preg_replace('/((?<!^).)/s', ',$1', $characters));
		
		$hash = '-1';
		
		foreach ($characters as $character)
			$hash = "($hash*33+" . ord($character) . ")";
		
		return $hash;
	}
	
	public function module_prefix_macro($matches) {
		if ($matches[1] == 'this')
			$module_index = $this->in_module;
		else
			$module_index = array_search($matches[1], $this->modules);
		
		if ($module_index !== null)
			return "#define M{$module_index}@{$matches[2]}";
		else
			trigger_error("Invalid #define: \"{$matches[1]}.{$matches[2]}\".", E_USER_ERROR);
	}

	public function module_prefix_emit($matches) {
		if ($matches[2] == 'this')
			$module_index = $this->in_module;
		else
			$module_index = array_search($matches[2], $this->modules);

		if ($module_index !== null)
			return "#emit{$matches[1]}M{$module_index}@{$matches[3]}";
		else
			trigger_error("Invalid #emit: \"{$matches[0]}\".", E_USER_ERROR);
	}
	
	public function pawnc_module_prefix($matches) {
		return $this->modules[$matches[1]] . '.';
	}
	
	public function resolve_module_prefix($matches) {
		$module_index = array_search($matches[1], $this->modules);
		
		return "M$module_index@";
	}
	
	private function human_size($bytes, $decimals = 2) {
		$suffixes = array('b', 'kb', 'mb', 'gb', 'tb', 'pb');
		$factor = floor((strlen($bytes) - 1) / 3);
		
		return sprintf("%.{$decimals}f", $bytes / pow(1024, $factor)) . $suffixes[$factor];
	}
	
	public function compile() {
		$this->clean_directories();
		
		if (file_exists('gamemodes/build-info.txt'))
			unlink('gamemodes/build-info.txt');
		
		register_shutdown_function(array($this, 'clean_directories'));
		
		$start_time = microtime(true);
		
		$this->generate_main();
		
		$pawnc = new PAWNC();
		
		$pawnc->base_dir           = 'gamemodes';
		$pawnc->input_file         = 'gamemodes/main.pwn';
		$pawnc->output_file        = 'gamemodes/main.amx';
		$pawnc->include_dir        = 'include';
		$pawnc->debug              = $this->cfg['debug_level'];
		$pawnc->list_file          = false;
		$pawnc->short_output_paths = true;
		
		$pawnc->args['IN_COMPILE_SCRIPT'] = true;
		
		if (!$this->is_windows) {
			if (false === stripos(`ps aux`, 'wineserver')) {
				echo "Starting wineserver.. ";
				
				`$pawnc->wine_dir/wineserver -p -d0`;
				`$pawnc->wine_dir/wine cmd /c exit`;
				
				echo "done.\n";
			}
		}
		
		$retval = 0;//exit;
		$retval = $pawnc->compile();
		
		if (!empty($pawnc->output)) {
			// Replace module prefixes back to the module's name in the compiler's output
			$pawnc->output = preg_replace_callback('/M([0-9]+)@/', array($this, 'pawnc_module_prefix'), $pawnc->output);
			
			echo "$pawnc->output\n\n";
		}
		
		if ($retval != 0) {
			echo "Failed to compile ($retval).\n";
			
			if (file_exists('gamemodes/build-info.txt'))
				unlink('gamemodes/build-info.txt');
		} else {
			if (!$pawnc->list_file && !$pawnc->asm_file) {
				$amx = new AMX($pawnc->output_file);
				
				$save = false;
				
				$pat = '/^M([0-9]+)@/';
				
				$redirects = array();
				$this_errfunc = 0;
				
				@date_default_timezone_set(date_default_timezone_get());
				
				$buildinfo = array(
					'Date: ' . date('r') . "\n",
					'AMX size: ' . $this->human_size($amx->header->size)
				);
				
				foreach ($amx->header->publics as &$public) {
					if ($public->name == 'PBP_ThisFunctionError') {
						$this_errfunc = $public->value;
						
						break;
					}
				}
				
				foreach ($amx->header->publics as &$public) {
					if (preg_match($pat, $public->name)) {
						$name = preg_replace_callback($pat, array($this, 'pawnc_module_prefix'), $public->name);
						
						if (strlen($name) > 31)
							continue;
						
						$redirects[$name] = true;
						
						array_push($amx->header->publics, (object) array(
							'name' => $name,
							'value' => $public->value
						));
						
						if ($this_errfunc) {
							$name = preg_replace($pat, 'this.', $public->name);

							if (strlen($name) > 31)
								continue;

							$redirects[$name] = true;

							array_push($amx->header->publics, (object) array(
								'name' => $name,
								'value' => $this_errfunc
							));
						}
					}
				}
			
				if ($amx->debug) {
					$pat = '/^M([0-9]+)@/';
					
					$uservars = array();
					$configvars = array();
					$staticgroups = array();
					$commands = array();
					
					foreach ($amx->header->publics as $entry) {
						if (isset($redirects[$entry->name]))
							continue;
						
						if (preg_match('/^(.*)@Pu_$/', $entry->name, $matches))
							$uservars[] = preg_replace_callback($pat, array($this, 'pawnc_module_prefix'), $matches[1]);
						
						if (preg_match('/^(.*)@Pc_$/', $entry->name, $matches))
							$configvars[] = preg_replace_callback($pat, array($this, 'pawnc_module_prefix'), $matches[1]);
						
						if (preg_match('/^@pG_(.*)$/', $entry->name, $matches))
							$staticgroups[] = preg_replace_callback($pat, array($this, 'pawnc_module_prefix'), $matches[1]);
						
						if (preg_match('/^@_yC(.*)$/', $entry->name, $matches))
							$commands[$matches[1]] = '';
						
						$matching_symbol = false;
						
						foreach ($amx->debug->symbols as &$symbol) {
							if ($symbol->ident == AMX::IDENT_FUNCTION && $symbol->name == $entry->name) {
								$matching_symbol = true;
								
								break;
							}
						}
						
						if (!$matching_symbol)
							echo "NOTICE: Public $entry->name has no found matching symbol.\n";
					}
					
					if (!empty($configvars))
						$buildinfo[] = "\nConfig variables:\n\t* " . implode("\n\t* ", $configvars);
					
					if (!empty($staticgroups))
						$buildinfo[] = "\nStatic groups:\n\t* " . implode("\n\t* ", $staticgroups);
					
					if (!empty($uservars))
						$buildinfo[] = "\nUser variables:\n\t* " . implode("\n\t* ", $uservars);
					
					$commandsstr = "\nCommands:";
					$maxcmdlen = 0;
					
					foreach ($commands as $command => &$description) {
						$len = strlen($command);
						
						if (isset($this->command_descriptions[$command]))
							$description = $this->command_descriptions[$command];
						
						if ($len > $maxcmdlen)
							$maxcmdlen = $len;
					}
					
					unset($description);
					
					foreach ($commands as $command => $description) {
						if (!empty($description))
							$description = " - $description";
						
						$commandsstr .= sprintf("\n\t* %s%s", str_pad($command, $maxcmdlen), $description);
					}
					
					$buildinfo[] = $commandsstr;
					
					$arrays = array();
				
					foreach ($amx->debug->symbols as &$symbol) {
						if ($symbol->ident == AMX::IDENT_ARRAY)
							$arrays[] = &$symbol;
						
						// Replace module prefixes back to the module's name in the AMX file's debug information
						$symbol->name = preg_replace_callback($pat, array($this, 'pawnc_module_prefix'), $symbol->name, -1, $count);
					}
				
					foreach ($amx->debug->tags as $id => &$tag)
						$tag = preg_replace_callback($pat, array($this, 'pawnc_module_prefix'), $tag, -1, $count);
				
					foreach ($amx->debug->automatons as &$automaton)
						$automaton->name = preg_replace_callback($pat, array($this, 'pawnc_module_prefix'), $automaton->name, -1, $count);
				
					foreach ($amx->debug->states as &$state)
						$state->name = preg_replace_callback($pat, array($this, 'pawnc_module_prefix'), $state->name, -1, $count);
				
					if (!$this->is_windows) {
						$base = escapeshellarg(realpath('.'));
						$base = trim(`$pawnc->wine_dir/winepath -w $base`);
					} else {
						$base = realpath('.');
					}
					
					$base = str_replace('\\', '/', $base);
					$base = rtrim($base, '/');
					
					foreach($amx->debug->files as &$file) {
						$file->name = str_replace('\\', '/', $file->name);
						$file->name = preg_replace('#^' . preg_quote($base, '#') . '#', '', $file->name);
						
						// This will most likely only happen when in UNC paths (Windows tries to fix that..)
						if (preg_match('#^[a-z]:/#i', $file->name) && preg_match('#^[a-z]:/#i', $base))
							$file->name = preg_replace('#^[a-z]:/' . preg_quote(substr($base, 3)) . '#i', '', $file->name);
						
						$file->name = ltrim($file->name, '/');
						$file->name = str_ireplace('YSI/pawno/include/YSI', 'YSI', $file->name);
						
						do
							$file->name = preg_replace('#(^|/)([^/]*?)/\.\.(/|$)#', '$1', $file->name, -1, $count);
						while ($count);
					}

					usort($arrays, function ($left, $right) {
						$leftsize = 1;
						$rightsize = 1;

						foreach ($left->dim as $dim)
							$leftsize *= $dim->size;

						foreach ($right->dim as $dim)
							$rightsize *= $dim->size;

						return $rightsize - $leftsize;
					});
					
					$topvars = "\n\nLargest variables:";

					for ($i = 0; $i < 30; $i++) {
						if (!isset($arrays[$i]))
							break;
						
						$symbol = &$arrays[$i];
						$symbol->size = 1;

						foreach ($symbol->dim as $dim)
							$symbol->size *= $dim->size;

						$topvars .= "\n\t" . sprintf("%6s - %s", $this->human_size($symbol->size * 4, 0), $symbol->name);
					}
					
					if (file_exists('gamemodes/build-info.txt')) {
						$fp = fopen('gamemodes/build-info.txt', 'a');
						
						fwrite($fp, $topvars);
						
						fclose($fp);
					}
					
					$save = true;
				}
				
				if ($save) {
					if (!$amx->save())
						echo "ERROR: Failed to save the modified AMX. It might be corrupted.";
				}
				
				file_put_contents('gamemodes/build-info.txt', implode("\n", $buildinfo) . "\n\n" . file_get_contents('gamemodes/build-info.txt'));
				
				echo "Successfully compiled in " . round(microtime(true) - $start_time, 1) . " seconds; AMX size: " . $this->human_size($amx->header->size) . ".\n";
			} else {
				echo "Done.\n";
			}
		}
	}
}
