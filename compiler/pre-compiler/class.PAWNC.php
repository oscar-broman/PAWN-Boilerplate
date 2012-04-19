<?php
class PAWNC {
	private $pawncc_path;
	private $input_file;
	private $output_file;
	private $include_dir;
	
	public $args = array();
	public $output;
	public $short_output_paths = true;
	
	private static $compiler_args;
	private static $arg_aliases = array(
		'align'         => 'A',
		'asm_file'      => 'a',
		'codepage'      => 'c',
		'base_dir'      => 'D',
		'debug'         => 'd',
		'error_file'    => 'e',
		'hwnd'          => 'h',
		'list_file'     => 'l',
		'optimizations' => 'O',
		'prefix_file'   => 'p',
		'report'        => 'r',
		'dynamic'       => 'S',
		'skip_lines'    => 's',
		'tabsize'       => 't',
		'verbosity'     => 'v',
		'semicolon'     => ';',
		'parantheses'   => '('
	);
	
	private $basedir;
	private $is_windows;
	public $wine_dir;
	
	public function __construct() {
		$this->basedir = realpath(dirname(__file__));
		
		if (is_executable("$this->basedir/../bin/pawncc.exe"))
			$this->pawncc_path = realpath("$this->basedir/../bin/pawncc.exe");
		
		if (!isset(self::$compiler_args))
			self::$compiler_args = array_fill_keys(explode(' ', 'A a C c D d e H i l o O p r S s t v w X XD \\ ^ ;+ (+ ;- (- ; ('), true);
		
		$this->is_windows = (strpos(PHP_OS, 'WIN') !== false);
		
		if (!$this->is_windows) {
			$path = getenv('PATH');
			
			if (empty($path))
				trigger_error('Unable to read environment variable PATH.', E_USER_ERROR);
			
			$path   = explode(':', $path);
			$path[] = '/opt/local/bin';
			$path[] = '/opt/local/sbin';
			
			foreach ($path as $search_path) {
				if (@is_executable("$search_path/wine")) {
					$this->wine_dir = $search_path;
				
					break;
				}
			}
			
			if ($this->wine_dir == null)
				trigger_error('Unable to locate Wine. Make sure it\'s installed.', E_USER_ERROR);
		}
	}
	
	public function shell_path($path, $new_extension = null, $wine_translate = true, $escape = true) {
		extract(pathinfo($path));
		
		if (false === ($dirname = realpath($dirname)))
			trigger_error("Invalid path: \"$path\".", E_USER_ERROR);
		
		if ($new_extension !== null)
			$extension = $new_extension;
		
		$extension = isset($extension) ? ".$extension" : '';
		
		$path = $dirname . DIRECTORY_SEPARATOR . $filename . $extension;
		
		if ($wine_translate && !$this->is_windows) {
			$path = escapeshellarg($path);
			$path = trim(`$this->wine_dir/winepath -w $path`);
		}
		
		return $escape ? escapeshellarg($path) : $path;
	}
	
	public function __get($name) {
		if ($name == 'output_file')
			return $this->output_file;
		
		if (preg_match('/^w[0-9]+$/', $name))
			return isset($this->args[$knameey]) ? $this->args[$name] : null;
		
		if (isset(self::$arg_aliases[$name])) {
			$key = self::$arg_aliases[$name];
			
			if ($name == 'semicolon' || $name == 'parantheses')
				$key .= $value ? '+' : '-';
			
			return isset($this->args[$key]) ? $this->args[$key] : null;
		}
		
		trigger_error("Undefined property: \"$name\".", E_USER_ERROR);
	}
	
	public function __set($name, $value) {
		if (preg_match('/^w[0-9]+$/', $name)) {
			$this->args[$name] = $value; 
			
			return;
		}
		
		if (isset(self::$arg_aliases[$name])) {
			$key = self::$arg_aliases[$name];
			
			if ($name == 'semicolon' || $name == 'parantheses')
				$key .= $value ? '+' : '-';
			
			$this->args[$key] = $value; 
			
			return;
		}
		
		switch ($name) {
			case 'pawncc_path':
			case 'include_dir':
			case 'input_file':
			case 'output_file':
				if (false === ($new_value = $this->shell_path($value, null, false, false)))
					trigger_error("Invalid path: \"$value\".", E_USER_ERROR);
				
				$value = $new_value;
				
				break;
			
			default:
				trigger_error("Undefined property: \"$name\".", E_USER_ERROR);
		}
		
		if ($name == 'pawncc_path' && !is_executable($value))
			trigger_error("Invalid executable: \"$name\".", E_USER_ERROR);
		
		$this->$name = $value;
	}
	
	private function build_args() {
		$arg_strings = array();
		
		if (isset($this->args['o']))
			trigger_error("The arg \"-o\" may not be explicitly set. Use output_file property instead.", E_USER_WARNING);

		if (isset($this->args['i']))
			trigger_error("The arg \"-i\" may not be explicitly set. Use include_dir property instead.", E_USER_WARNING);
		
		$this->args['o'] = $this->output_file;
		
		if (!empty($this->include_dir))
			$this->args['i'] = $this->include_dir;
		
		foreach ($this->args as $key => $value) {
			if ($value === null)
				continue;
			
			if (isset(self::$compiler_args[$key])) {
				if ($value === false)
					continue;
				
				if (in_array($key, array('D', 'e', 'r', 'p', 'i', 'o')) && is_string($value))
					$arg_strings[] = "-$key" . $this->shell_path($value);
				else if ($value === true)
					$arg_strings[] = "-$key";
				else if (preg_match('/^[0-9]+$/', $value))
					$arg_strings[] = "-$key$value";
				else
					$arg_strings[] = "-$key" . escapeshellarg($value);
			} else if (preg_match('/^w[0-9]+$/', $key)) {
				if ($value !== false)
					$arg_strings[] = "-$key";
			} else {
				if (!preg_match('/^[a-z@_][a-z0-9_@]*$/i', $key)) {
					trigger_error("Invalid constant name: \"$key\".", E_USER_WARNING);
					
					continue;
				}
				
				$arg_strings[] = "$key=" . escapeshellarg($value);
			} 
		}
		
		unset($this->args['o']);
		unset($this->args['i']);
		
		return implode(' ', $arg_strings);
	}
	
	public function compile() {
		if (empty($this->input_file))  trigger_error("Input file not set.", E_USER_ERROR);
		if (empty($this->pawncc_path)) trigger_error("Compiler path not set.", E_USER_ERROR);
		
		if (!empty($this->args['l']) && !empty($this->args['a']))
			trigger_error("Only one of the compiler flags asm_file (-a) and list_file (-l) may be non-null.", E_USER_ERROR);
		
		if (empty($this->args['e'])) {
			$this->args['e'] = tempnam(sys_get_temp_dir(), 'pawncc_out');
			
			$tmp_error_file = true;
		} else
			$tmp_error_file = false;
		
		if (empty($this->output_file)) {
			if (!empty($this->args['l']))
				$ext = 'lst';
			else if (!empty($this->args['a']))
				$ext = 'asm';
			else
				$ext = 'amx';
			
			$this->output_file = $this->shell_path($this->input_file, $ext, false, false);
		}
		
		$args = $this->build_args();
		
		$pawncc_path = $this->shell_path($this->pawncc_path, null, false);
		$input_file = $this->shell_path($this->input_file);
		
		$cmd = "$pawncc_path $input_file $args";
		
		if ($this->is_windows)
			$cmd = "$cmd > NUL";
		else
			$cmd = "$this->wine_dir/wine $cmd > /dev/null";
		
		if (!empty($this->output_file) && file_exists($this->output_file))
			unlink($this->output_file);
		
		$this->output = '';
		
		if (file_exists($this->args['e']))
			unlink($this->args['e']);
		
		system($cmd, $return_value);
		
		if (file_exists($this->args['e'])) {
			$this->output = trim(file_get_contents($this->args['e']));
			
			if ($this->short_output_paths) {
				if (!empty($this->args['D']))
					$base = realpath($this->args['D']);
				else
					$base = dirname(realpath($this->input_file));
				
				if ($base) {
					$base = $this->shell_path($base, null, true, false);
					
					$this->output = preg_replace('/^\s*' . preg_quote($base, '/') . '[\/\\\]?/im', '', $this->output);
					
					if (!$tmp_error_file)
						file_put_contents($this->args['e'], $this->output);
				}
			}
			
			if ($tmp_error_file) {
				unlink($this->args['e']);
				unset($this->args['e']);
			}
		}
		
		return $return_value;
	}
}

