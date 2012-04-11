<?php
class SyntaxIntel {
	public $data;
	
	public function __construct() {
		$this->data = (object) array(
			'callbacks' => array()
		);
	}
	
	public function parse_directory($dirname, $settings) {
		if (false === ($dirname = realpath($dirname)) || !is_dir($dirname))
			trigger_error('Invalid directory given to parse_directory.', E_USER_ERROR);
		
		extract($settings);
		
		if (!isset($ignore))    $ignore = array();
		if (!isset($pattern))   $pattern = '*.inc';
		if (!isset($recursive)) $recursive = true;
		
		if (!is_array($ignore))
			$ignore = array($ignore);
		
		$data = '';
		$files = $this->get_directory_files($dirname, $pattern, $recursive, $ignore);
		
		foreach ($files as $file) {
			if (($fp = fopen($file, 'rb'))) {
				$data .= "\n" . stream_get_contents($fp);
				
				fclose($fp);
			} else {
				trigger_error("Unable to read from include file: \"$file\".", E_USER_WARNING);
			}
		}
		
		$this->parse_text($data);
	}
	
	public function parse_file($fname) {
		if (false === ($fname = realpath($fname)))
			trigger_error("Unable to read from include file: \"$file\".", E_USER_WARNING);
		else
			$this->parse_text(file_get_contents($fname));
	}
	
	public function parse_text($text) {
		$text = preg_replace('#/\*[\s\S]*?\*/#', '', $text);
		$text = preg_replace('#//.*#', '', $text);
		
		// TODO: un-regexify..
		preg_match_all('/\bforward\s+([a-z0-9_@]+)\s*\(\s*(.*?)\s*\)\s*;/i', $text, $matches, PREG_SET_ORDER);
		
		$callbacks = array();
		
		foreach ($matches as $match)
			$callbacks[$match[1]] = $match[2];
		
		$this->data->callbacks = array_merge($this->data->callbacks, $callbacks);
	}
	
	private function get_directory_files($dirname, $pattern = '*', $recursive = false, array $ignore = array()) {
		$files = glob($dirname . DIRECTORY_SEPARATOR . $pattern);
		
		foreach ($files as $k => $file) {
			if (in_array(basename($file), $ignore))
				unset($files[$k]);
		}
		
		if ($recursive) {
			foreach (glob($dirname . DIRECTORY_SEPARATOR . '*', GLOB_ONLYDIR | GLOB_NOSORT) as $dir) {
				if (in_array(basename($dir), $ignore))
					continue;
				
				$files = array_merge($files, $this->get_directory_files($dir, $pattern, true, $ignore));
			}
		}
		
		return $files;
	}
}