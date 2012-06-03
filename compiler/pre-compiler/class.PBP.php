<?php
require 'class.SyntaxIntel.php';
require 'class.PAWNC.php';
require 'class.AMX.php';

class PBP {
    public $output;
    
	private $is_windows;
    private $syntax_intel;
	private $modules;
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
plugins crashdetect sscanf

;PAWN Boilerplate settings
debug_level 2

EOD;
			
			file_put_contents('server.cfg', $cfg_default);
			
			echo "Created \"server.cfg\".\n";
		}
		
		$this->cfg['debug_level'] = 2;
		
		foreach (file('server.cfg') as $row) {
			$row = trim($row);
			
			if (empty($row) || preg_match('/^(;|#|echo)/', $row))
				continue;
			
			list($key, $value) = preg_split('/\s+/', $row, 2);
			
			$key = strtolower($key);
			
			if (in_array($key, array('lanmode', 'maxplayers', 'port', 'announce', 'query', 'onfoot_rate', 'incar_rate', 'weapon_rate', 'stream_rate', 'maxnpc', 'debug_level'))) {
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
		
		foreach (glob('gamemodes/modules/*') as $dir) {
			if (!is_dir($dir))
				continue;
			
			$basename = basename($dir);
			
			if (!preg_match('/^[a-z@_][a-z0-9_@]*$/i', $basename)) {
				echo "PBP Error: Invalid module name: \"$basename\".";
				
				exit;
			}
			
			$modules[] = $basename;
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
		
		foreach (glob('gamemodes/modules/*/callbacks.inc') as $file) {
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
			if (!file_exists("gamemodes/modules/$module/callbacks"))
				mkdir("gamemodes/modules/$module/callbacks");
			
			foreach (array_keys($module_includes) as $incfile) {
				$file = "gamemodes/modules/$module/$incfile.inc";
				
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
				
				$module_includes[$incfile][] = (object) array(
					'module'       => $module_index,
					'include_path' => ".build\\modules\\$module\\$incfile",
					'priority'     => isset($info['Priority']) ? (int) $info['Priority'] : 0,
				);
			}
		}
		
		$active_callbacks = array();
		
		foreach ($modules as $module_index => $module) {
			foreach (glob("gamemodes/modules/$module/callbacks/*.inc") as $file) {
				$cbfile = basename($file);
				
				$this->process_file($file, $module_index);
				
				$callback = substr($cbfile, 0, strpos($cbfile, '.'));
				
				if (!isset($callbacks[$callback])) {
					$fileshort = preg_replace('/^gamemodes(\\\|\/)/', '', $file);
					
					if (!isset($lowercallbacks, $callbacks_keys)) {
						$callbacks_keys = array_keys($callbacks);
						$lowercallbacks = array_map('strtolower', $callbacks_keys);
					}
					
					if (false !== ($idx = array_search(strtolower($callback), $lowercallbacks))) {
						$callback = $callbacks_keys[$idx];
						
						echo "PBP Notice: Renaming $fileshort to \"$callback.inc\" (correct case).\n";
						
						$newfile = "gamemodes/modules/$module/callbacks/$callback.inc";
						
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
				
				$callback_includes[$callback][] = (object) array(
					'module'       => $module_index,
					'include_path' => ".build\\modules\\$module\\callbacks\\$callback",
					'priority'     => isset($info['Priority']) ? (int) $info['Priority'] : 0,
				);
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
			$module_list .= "\n  \t* $module";
		
		$custom_callback_list = '';
		
		asort($custom_callbacks);
		
		foreach ($custom_callbacks as $callback => $arguments) {
			$custom_callback_list .= "\n  \t" . (isset($active_callbacks[$callback]) ? '+' : ' ') . " $callback($arguments)";
		}
		
		$callback_list = '';
		
		asort($callbacks);
		
		foreach ($callbacks as $callback => $arguments) {
			$callback_list .= "\n  \t" . (isset($active_callbacks[$callback]) ? '+' : ' ') . " $callback($arguments)";
		}
		
		$timestamp = time();
		
		
		$output = <<<EOD
/*
  NOTE: This file is generated by "compiler/compile". Any changes made to it will be lost next compilation.

  Existing modules: $module_list
  
  Custom callbacks: $custom_callback_list

  Existing callbacks: $callback_list
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
			
			$public_functions .= <<<EOD

$pub$callback({$callbacks[$callback]}) {
	new PBP.ReturnValue
		#if defined DR@$callback
			= PBP.DEFAULT_RETURN<$callback>;
		#elseif defined ALS_R_$ALSdef
			= ALS_R_$ALSdef;
		#else
			= 1;
		#endif


EOD;
			
			foreach ($callback_include as $k => &$v) {
				$incdef = '_inc_' . substr("$callback", 0, 25);
				
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

$public_functions .= <<<EOD

	return PBP.ReturnValue;
}

EOD;
		}
		
		$max_players_cfg = '';
		
		if (!empty($this->cfg['maxplayers'])) {
			$max_players_cfg = <<<EOD

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

$max_players_cfg
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
		if (!file_exists('gamemodes/.build')) return;
		$iterator = new RecursiveIteratorIterator(new RecursiveDirectoryIterator('gamemodes/.build'), RecursiveIteratorIterator::CHILD_FIRST);
		
		foreach ($iterator as $path) {
			if ($path->isDir())
				rmdir("$path");
			else
				unlink("$path");
		}
	}
	
	private $in_module;

	private function process_file($file, $in_module = null) {
		$newfile = preg_replace('/gamemodes(\/|\\\)/i', 'gamemodes/.build/', $file);
		
		if (!file_exists('gamemodes/.build')) {
			mkdir('gamemodes/.build');
			
			if ($this->is_windows)
				system('attrib +H ' . escapeshellarg(realpath('gamemodes/.build')));
		}
		
		$this->in_module = $in_module;
		
		$contents = file_get_contents($file);
		
		$contents = preg_replace_callback('/^\s*#define\s+(this|' . implode('|', $this->modules) . ')\.([a-zA-Z0-9_@]+)/sm', array($this, 'module_prefix_macro'), $contents);
		$contents = preg_replace_callback('/^\s*#emit(\s+\S+\s+)(this|' . implode('|', $this->modules) . ')\.([a-zA-Z0-9_@]+)/sm', array($this, 'module_prefix_emit'), $contents);
		
		$contents = preg_replace_callback('/\b_([IH])(?:\<(.*?)\>|\((.*?)\))/', function ($matches) {
			$characters = !empty($matches[2]) ? $matches[2] : str_replace(',', '', $matches[3]);
			$characters = strrev($characters);
			
			if ($matches[1] == 'I')
				$characters = strtoupper($characters);
			
			$characters = explode(',', preg_replace('/((?<!^).)/s', ',$1', $characters));
			
			$hash = '-1';
			
			foreach ($characters as $character)
				$hash = "($hash*33+" . ord($character) . ")";
			
			return $hash;
		}, $contents);
		
		if ($count) {
			echo $contents;
			
			exit;
		}
		
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
	
	private function human_size($bytes, $decimals = 2) {
		$suffixes = array('b', 'kb', 'mb', 'gb', 'tb', 'pb');
		$factor = floor((strlen($bytes) - 1) / 3);
		
		return sprintf("%.{$decimals}f", $bytes / pow(1024, $factor)) . $suffixes[$factor];
	}
	
	public function compile() {
		$this->clean_directories();
		
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
		
		$retval = 0;
		$retval = $pawnc->compile();
		
		if (!empty($pawnc->output)) {
			// Replace module prefixes back to the module's name in the compiler's output
			$pawnc->output = preg_replace_callback('/M([0-9]+)@/', array($this, 'pawnc_module_prefix'), $pawnc->output);
			
			echo "$pawnc->output\n\n";
		}
		
		if ($retval != 0)
			echo "Failed to compile ($retval).\n";
		else {
			if (!$pawnc->list_file && !$pawnc->asm_file) {
				$amx = new AMX($pawnc->output_file);
				
				$save = false;
				
				$pat = '/^M([0-9]+)@/';
				
				$redirects = array();
				$this_errfunc = 0;
				
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
					
					foreach ($amx->header->publics as $entry) {
						if (isset($redirects[$entry->name]))
							continue;
						
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
					
					$topvars = 'Largest variables:';

					for ($i = 0; $i < 30; $i++) {
						if (!isset($arrays[$i]))
							break;
						
						$symbol = &$arrays[$i];
						$symbol->size = 1;

						foreach ($symbol->dim as $dim)
							$symbol->size *= $dim->size;

						$topvars .= "\n  \t" . sprintf("%6s - %s", $this->human_size($symbol->size * 4, 0), $symbol->name);
					}
					
					$main = @file_get_contents('gamemodes/main.pwn');
					
					if ($main) {
						$main = str_replace('  Existing callbacks:', "  $topvars\n\n  Existing callbacks:", $main);
						
						file_put_contents('gamemodes/main.pwn', $main);
					}
					
					$save = true;
				}
				
				if ($save) {
					if (!$amx->save())
						echo "ERROR: Failed to save the modified AMX. It might be corrupted.";
				}
				
				echo "Successfully compiled in " . round(microtime(true) - $start_time, 1) . " seconds; AMX size: " . $this->human_size($amx->header->size) . ".\n";
				
			} else {
				echo "Done.\n";
			}
		}
	}
}
