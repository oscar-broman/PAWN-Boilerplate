<?php
/**
 * Code scanner for PAWN scripts.
 *
 * Scans through scripts to find many different declarations and definitions.
 *
 * @author    Oscar Broman
 * @copyright 2012-2013 Oscar Broman
 * @license   http://www.opensource.org/licenses/mit-license.php MIT
 * @link      https://github.com/oscar-broman/PAWN-Scanner
 * @version   1.0
 */

namespace PAWNScanner;

/**
 * Main class for PAWN-Scanner.
 *
 * Contains functions for scanning files/directories. The stored information
 * will be inside properties.
 *
 */
class Scanner
{
	/** @var \PAWNScanner\FunctionDeclaration[] Found function declarations. */
	public $functions = array();
	
	/** @var \PAWNScanner\Macro[] Found macros. Key is search. */
	public $macros = array();
	
	/** @var \PAWNScanner\Enumeration[] Found enumerations. */
	public $enums = array();
	
	/** @var \PAWNScanner\Variable[] Found symbol constants. Keys are variable names. */
	public $constants = array();
	
	/** @var bool Whether comments should be scanned. */
	public $scan_comments = false;
	
	/**
	 * Scans a file. Returns whether it was scanned.
	 *
	 * @param mixed $file The file to scan. Can be a resource or string.
	 *
	 * @return bool
	 */
	public function scan_file($file)
	{
		if (is_resource($file))
			$contents = @stream_get_contents($file);
		else
			$contents = @file_get_contents($file);
		
		if (empty($contents))
			return false;
		
		if (function_exists('mb_convert_encoding'))
			$contents = mb_convert_encoding($contents, 'UTF-8');
		
		if (!$this->scan_comments) {
			// Strip comments
			$contents = preg_replace('/\/\/.*$/m', '', $contents);
			$contents = preg_replace('/\/\*[\s\S]*?\*\//', '', $contents);
		}
		
		// Collapse line continuations
		$contents = preg_replace('/\\\\\s*?\n\s*/', ' ', $contents);
		
		// Search for forward/native declarations
		if (preg_match_all('/\b(forward|native)\s+([a-z@_][a-z0-9@_\.\:]*(?<!:):)?\s*([a-z@_][a-z0-9@_\.\:]*)\s*\((.*?)\)\s*;/i', $contents, $matches, PREG_SET_ORDER)) {
			foreach ($matches as $match) {
				list(, $type, $tag, $name, $arguments) = $match;
				
				if ($type == 'forward')
					$type = FunctionDeclaration::TYPE_FORWARD;
				else
					$type = FunctionDeclaration::TYPE_NATIVE;
				
				if (!isset($this->functions[$name])) {
					$this->functions[$name] = $function = new FunctionDeclaration($name, $tag, $arguments, $type, array(
						'file' => (string) $file
					));
				}
			}
		}
		
		// Search for #define directives
		if (preg_match_all('/^[\ \t]*#define\s+?(\S+)(?:[\ \t]+([^\r\n$]+))?/m', $contents, $matches, PREG_SET_ORDER)) {
			foreach ($matches as $match) {
				@list(, $search, $replacement) = $match;
				
				$this->macros[$search] = new Macro($search, $replacement, array(
					'file' => (string) $file
				));
			}
		}
		
		// Search for enumerations
		if (preg_match_all('/\benum\s+([a-z@_][a-z0-9@_\.\:]*(?<!:):)?\s*([a-z@_][a-z0-9@_\.\:]*)?\s*(?:\((.*?)\))?\s*\{\s*(.*?)\s*\}/is', $contents, $matches, PREG_SET_ORDER)) {
			foreach ($matches as $match) {
				list(, $tag, $name, $increment, $body) = $match;
				
				// Remove preprocessor directives from the enum body
				$body = preg_replace('/^\s*#.*?$/m', '', $body);
				
				$this->enums[] = new Enumeration($tag, $name, $increment, $body, array(
					'file' => (string) $file
				));
			}
		}
		
		//Search symbol constants. Only non-indented, as they're assumably in the global scope.
		if (preg_match_all('/^const\s+(\S.*?\s*=\s*.+?)\s*;\s*$/m', $contents, $matches, PREG_SET_ORDER)) {
			foreach ($matches as $match) {
				$constant = new Variable($match[1], array(
					'file' => (string) $file
				));
				
				$this->constants[$constant->varname] = $constant;
			}
		}
		
		return true;
	}
	
	/**
	 * Recursively scan a directory.
	 *
	 * @param string $dir The directory to scan.
	 * @param string|string[] $ignore File/folder names to ignore.
	 * @return void
	 */
	public function scan_dir($dir, $ignore = array())
	{
		if (!is_array($ignore))
			$ignore = array($ignore);
		
		$dir_iterator = new RecursiveDirectoryIterator($dir);
		
		foreach ($dir_iterator as $file) {
			if (in_array(basename($file), $ignore))
				continue;
			
			$this->scan_file($file);
		}
	}
}

/**
 * Function declaration.
 *
 * Contains most information about a function declaration. Will look like the
 * original declaration if casted to a string.
 *
 */
class FunctionDeclaration
{
	/** @var int Unknown type of declaration. */
	const TYPE_UNKNOWN = 0;
	
	/** @var int Is a forward declaration. */
	const TYPE_FORWARD = 1;
	
	/** @var int Is a native declaration. */
	const TYPE_NATIVE  = 2;
	
	/** @var string The name of the function. */
	public $name;
	
	/** @var int The type of declaration. */
	public $type;
	
	/** @var \PAWNScanner\ArgumentList The function's arguments. */
	public $arguments;
	
	/** @var string|null The function's tag, or null if none. */
	public $tag;
	
	/**
	 * Extra information about the function, optionally given when instantiating.
	 *
	 * Internally, a property telling which file it came from is added.
	 *
	 * @var \stdClass
	 */
	public $info;
	
	/**
	 * Construct the class.
	 *
	 * @param string $name The function's name.
	 * @param string $arguments The function's list of arguments.
	 * @param string $type What kind of declaration it is.
	 * @param string|null $info Extra information about the function.
	 */
	public function __construct($name, $tag, $arguments = '', $type = self::TYPE_UNKNOWN, $info = null)
	{
		$this->name = $name;
		$this->tag = empty($tag) ? null : trim($tag, ": \t\n\r\0\x0B");
		$this->type = $type;
		$this->arguments = new ArgumentList($arguments);
		$this->info = new \stdClass();
		
		if ($info !== null) {
			foreach ($info as $k => $v)
				$this->info->$k = $v;
		}
	}
	
	/**
	 * Cast the class to a string. Gives a proper PAWN declaration.
	 *
	 * @return void
	 */
	public function __toString()
	{
		switch ($this->type) {
			case self::TYPE_FORWARD:
				return "forward $this->name($this->arguments)";
			
			case self::TYPE_NATIVE:
				return "native $this->name($this->arguments)";
			
			default:
				return "$this->name($this->arguments)";
		}
	}
}

/**
 * A list of variables.
 *
 * @see \PAWNScanner\Variable
 */
class VariableList implements \Iterator
{
	/** @var \PAWNScanner\Variable[] The variable list. */
	public $variables = array();
	
	/** @var int The iterator's current position. */
	private $position = 0;
	
	/**
	 * Construct the list from a string.
	 *
	 * @param string $liststr The variable list.
	 */
	public function __construct($liststr)
	{
		$args = preg_split('/\s*,\s*(?![^{]*?})/', $liststr, -1, PREG_SPLIT_NO_EMPTY);
		
		foreach ($args as $arg)
			$this->variables[] = new Variable($arg);
	}
	
	function current()
	{
		return $this->variables[$this->position];
	}

	function key()
	{
		return $this->position;
	}

	function next()
	{
		++$this->position;
	}

	function valid()
	{
		return isset($this->variables[$this->position]);
	}
	
	function rewind()
	{
		$this->position = 0;
	}
}

/**
 * A list of function arguments.
 *
 * @see \PAWNScanner\VariableList
 */
class ArgumentList extends VariableList
{
	/**
	 * Returns a PAWN argument list of all arguments.
	 *
	 * @return string
	 */
	public function __toString()
	{
		return implode(', ', $this->variables);
	}
}

/**
 * A PAWN variable and all its associated information.
 *
 * Contains information such as tags, size, name, and more.
 *
 */
class Variable
{
	/**
	 * Is of a special syntax, though not a familiar kind.
	 * 
	 * Being of a special syntax means the variable declaration ends with
	 * angle brackets (&lt;&gt;), optionally with something between them.
	 * 
	 * @var int
	*/
	const SPECIAL_UNKNOWN        = 0;
	
	/**
	 * @see self::SPECIAL_UNKNOWN for a complete description.
	 * @var int Is an interator from y_iterate.
	 */
	const SPECIAL_ITERATOR       = 1;
	
	/**
	 * @see self::SPECIAL_UNKNOWN for a complete description.
	 * @var int Is an interator array from y_iterate.
	 */
	const SPECIAL_ITERATOR_ARRAY = 2;
	
	/**
	 * @see self::SPECIAL_UNKNOWN for a complete description.
	 * @var int Is a bit array from y_bit.
	 */
	const SPECIAL_BIT_ARRAY      = 3;
	
	/**
	 * @see self::SPECIAL_UNKNOWN for a complete description.
	 * @var int Is a binary tree from y_bintree.
	 */
	const SPECIAL_BINARY_TREE    = 4;
	
	/**
	 * @see self::SPECIAL_UNKNOWN for a complete description.
	 * @var int Is a player array from y_playerarray.
	 */
	const SPECIAL_PLAYER_ARRAY   = 5;
	
	/**
	 * @see self::SPECIAL_UNKNOWN for a complete description.
	 * @var int Is va_args from y_va.
	 */
	const SPECIAL_VA_ARGS        = 6;
	
	/**
	 * An array for looking up known tags on special syntax declarations.
	 *
	 * @var array
	 */
	static $SPECIAL_TAGS = array(
		'Iterator'      => self::SPECIAL_ITERATOR,
		'IteratorArray' => self::SPECIAL_ITERATOR_ARRAY,
		'BitArray'      => self::SPECIAL_BIT_ARRAY,
		'BinaryTree'    => self::SPECIAL_BINARY_TREE,
		'PlayerArray'   => self::SPECIAL_PLAYER_ARRAY
	);
	
	/** @var bool Whether it was declared with the "const" keyword. */
	public $const;
	
	/** @var bool Whether it's a reference variable. */
	public $ref;
	
	/** @var \PAWNScanner\VariableTagList The variable's tags, if any. */
	public $tags;
	
	/** @var string The variable's name. */
	public $varname;
	
	/**
	 * Extra information about the variable, optionally given when instantiating.
	 *
	 * Internally, a property telling which file it came from is added.
	 *
	 * @var \stdClass
	 */
	public $info;
	
	/**
	 * The variable's array dimensions.
	 * 
	 * Null means it's not an array. True means it's an array without a fixed
	 * size. If it's a string, it contains the declared size.
	 * 
	 * 
	 * @var string|bool|null
	 */
	public $dim;
	
	/**
	 * @see self::SPECIAL_UNKNOWN for a complete description.
	 * @var null|int Null if not of a special syntax, otherwise self::SPECIAL_*.
	 */
	public $special;
	
	/** @var string The raw contents between the angle brackets. */
	public $special_info;
	
	/** @var string The raw default value. */
	public $default;
	
	/**
	 * Construct the class from a string containing a variable declaration.
	 * @param string $varstr The variable declaration/definition string.
	 * @param string|null $info Extra information about the variable.
	 */
	public function __construct($varstr, $info = null)
	{
		$this->info = new \stdClass();

		if ($info !== null) {
			foreach ($info as $k => $v)
				$this->info->$k = $v;
		}
		
		$varstr = trim($varstr);
		
		if (preg_match('/^(const)?\s*(&)?\s*(?:(.*?)(?<!:):)?\s*((?:[a-z@_][a-z0-9@_\.\:]*)|\.\.\.)(?:\s*(\[\s*.*\s*?\]))?(?:\s*(\<\s*.*\s*?\>))?(?:\s*=\s*(.+)\s*?)?$/i', $varstr, $matches)) {
			@list(, $const, $ref, $tags, $varname, $dim, $special, $default) = $matches;
			
			$this->const = !empty($const);
			$this->ref = !empty($ref);
			$this->tags = new VariableTagList($tags);
			$this->varname = $varname;
			
			if (empty($dim))
				$this->dim = null;
			else {
				$dim = trim($dim, "[] \t\n\r\0\x0B");
				
				if (empty($dim))
					$this->dim = true;
				else
					$this->dim = $dim;
			}
			
			if (empty($special)) {
				$this->special = null;
				$this->special_info = null;
			} else {
				$special = trim($special, "<> \t\n\r\0\x0B");
				
				$this->special = self::SPECIAL_UNKNOWN;
				$this->special_info = $special;
				
				if ($this->varname == 'va_args') {
					$this->special = self::SPECIAL_VA_ARGS;
				} else if ($this->tags->count == 1) {
					$tag = $this->tags->tags[0];
					
					if (isset(self::$SPECIAL_TAGS[$tag]))
						$this->special = self::$SPECIAL_TAGS[$tag];
					else
						trigger_error("Unknown special: \"$tag\".", E_USER_NOTICE);
				}
			}
			
			$this->default = $default;
		} else {
			trigger_error("Invalid argstr: \"$varstr\".", E_USER_NOTICE);
		}
		
		$this->tags = new VariableTagList($tags);
	}
	
	/**
	 * Cast the class to a string. Returns a proper PAWN declaration (though
	 * it might only be valid in the context it was defined).
	 *
	 * @return string
	 */
	public function __toString()
	{
		$varstr = '';
		
		if ($this->const)
			$varstr .= 'const ';
		
		if ($this->ref)
			$varstr .= '&';
		
		if ($this->tags->count)
			$varstr .= $this->tags . ':';
		
		$varstr .= $this->varname;
		
		if ($this->dim === true)
			$varstr .= '[]';
		else if ($this->dim !== null)
			$varstr .= "[$this->dim]";
		
		if ($this->special_info !== null)
			$varstr .= "<$this->special_info>";
		
		if ($this->default !== null)
			$varstr .= ' = ' . $this->default;
		
		return $varstr;
	}
}

/**
 * Tags associated with a variable.
 *
 * @see PAWNScanner\Variable
 */
class VariableTagList
{
	/** @var string[] The tags. */
	public $tags;
	
	/**
	 * Construct the list from a string.
	 */
	public function __construct($tagstr)
	{
		$tagstr = trim($tagstr, "{} \t\n\r\0\x0B");
		$this->tags = preg_split('/\s*,\s*/', $tagstr, -1, PREG_SPLIT_NO_EMPTY);
	}
	
	/**
	 * Get the list as a string.
	 *
	 * @return string An empty string, single tag, or group of tags (inside
	 * braces, like in PAWN).
	 */
	public function __toString()
	{
		$num_tags = count($this->tags);
		
		if ($num_tags == 0)
			return '';
		else if ($num_tags == 1)
			return $this->tags[0];
		else
			return '{' . implode(', ', $this->tags) . '}';
	}
	
	/**
	 * Getter. Only supports the property <code>count</code>, which is the
	 * number of tags.
	 *
	 * @param string $prop The property.
	 * @return mixed The value.
	 */
	public function __get($prop)
	{
		if ($prop == 'count')
			return count($this->tags);
		
		trigger_error('Undefined property via __get(): ' . $prop . ' in ' . $trace[0]['file'] . ' on line ' . $trace[0]['line'], E_USER_ERROR);
		
		return null;
	}
}

/**
 * A PAWN enumeration.
 *
 */
class Enumeration
{
	/** @var string|null The tag, null if none. */
	public $tag;
	
	/** @var string|null The name, null if none. */
	public $name;
	
	/** @var string|null The increment (raw), null if none. */
	public $increment;
	
	/** @var \PAWNScanner\EnumerationVariableList[] The entries. */
	public $entries;
	
	/**
	 * Extra information about the variable, optionally given when instantiating.
	 *
	 * Internally, a property telling which file it came from is added.
	 *
	 * @var \stdClass
	 */
	public $info;

	/**
	 * Construct the class from raw data.
	 *
	 * @param string $tag The tag.
	 * @param string $name The name.
	 * @param string $increment The increment.
	 * @param string $body The body.
	 * @param string|null $info Extra information about the enumeration.
	 */
	public function __construct($tag, $name, $increment, $body, $info = null)
	{
		$this->info = new \stdClass();

		if ($info !== null) {
			foreach ($info as $k => $v)
				$this->info->$k = $v;
		}
		
		$this->name = empty($tag) ? null : $tag;
		$this->name = empty($name) ? null : $name;
		$this->increment = empty($increment) ? null : $increment;
		$this->entries = new EnumerationVariableList($body);
	}
	
	/**
	 * Cast the class to a string.
	 *
	 * @return string A PAWN declaration of the enum.
	 */
	public function __toString()
	{
		$enumstr = 'enum ';

		if ($this->tag !== null)
			$enumstr .= $this->tag . ':';

		if ($this->name !== null)
			$enumstr .= $this->name . ' ';

		$enumstr .= "{\n$this->entries\n};";

		return $enumstr;
	}
}

/**
 * List of variables inside enumerations.
 *
 * @see PAWNScanner\Enumeration
 */
class EnumerationVariableList extends VariableList
{
	/**
	 * Cast the list to a string.
	 *
	 * @return string A string with the variables arranged as an enumeration body.
	 */
	public function __toString()
	{
		return "\t" . implode(",\n\t", $this->variables);
	}
}

/**
 * Macro (#define directive).
 *
 */
class Macro
{
	/** @var string The macro's prefix (the first symbol in the search). */
	public $prefix;
	
	/** @var string The macro's search string. */
	public $search;
	
	/** @var string|null The macro's replacement, if any. */
	public $replacement;
	
	/**
	 * Extra information about the macro, optionally given when instantiating.
	 *
	 * Internally, a property telling which file it came from is added.
	 *
	 * @var \stdClass
	 */
	public $info;
	
	/**
	 * Construct the class from raw strings.
	 *
	 * @param string $search The search string.
	 * @param string|null $replacement The replacement string.
	 * @param string|null $info Extra information about the macro.
	 */
	public function __construct($search, $replacement = null, $info = null)
	{
		$this->info = new \stdClass();

		if ($info !== null) {
			foreach ($info as $k => $v)
				$this->info->$k = $v;
		}
		
		$this->search = $search;
		$this->replacement = empty($replacement) ? null : $replacement;
		$this->prefix = preg_replace('/^(([a-z]\.)?[a-z@_][a-z0-9@_]*).*$/i', '$1', $search);
	}
	
	/**
	 * Cast the macro to a string.
	 *
	 * @return string A PAWN #define directive for the macro.
	 */
	public function __toString()
	{
		$retstr = "#define $this->search";
		
		if ($this->replacement)
			$retstr .= " $this->replacement";
		
		return $retstr;
	}
	
	/**
	 * Apply parameters to the macro.
	 *
	 * @return string The replacement.
	 */
	public function apply()
	{
		if (preg_match_all('/%[0-9]/', $this->search, $params, PREG_SET_ORDER)) {
			$replacement = $this->replacement;
			
			foreach ($params as $index => $param) {
				$replacement = str_replace($param[0], func_get_arg($index), $replacement);
			}
			
			return $replacement;
		} else {
			return $this->replacement;
		}
	}
}

/**
 * Directory iterator used to scan folders recursively.
 *
 * @see \PAWNScanner\Scanner::scan_dir()
 */
class RecursiveDirectoryIterator extends \RecursiveIteratorIterator
{
	public function __construct($path)
	{
		parent::__construct(
			new RecursiveDirectoryFilterIterator($path)
		);
	}
}

/**
 * Directory iterator filter to find PAWN source files.
 *
 * @see \PAWNScanner\Scanner::scan_dir()
 */
class RecursiveDirectoryFilterIterator extends \RecursiveFilterIterator
{
	function __construct($arg) {
		if (is_object($arg))
			$iterator = $arg;
		else
			$iterator = new \RecursiveDirectoryIterator($arg);
		
		parent::__construct($iterator);
	}
	
	public function accept() {
		$current = $this->current();
		
		if ($current->isDir())
			return true;
		
		$ext = strtolower($current->getExtension());
		
		return in_array($ext, array('lst', 'pwn', 'inc', 'h'), true);
	}
}