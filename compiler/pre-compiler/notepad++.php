<?php
include 'PAWNScanner.php';

function resolve_path($path) {
	$path = preg_replace_callback('/%(.*?)%/', function ($matches) {
		$env = getenv($matches[1]);
		
		if (empty($env) && stripos($env, '(x86)') !== false)
			$env = getenv(str_ireplace('(x86)', '', $matches[1]));
		
		return $env;
	}, $path);
	
	return realpath($path);
}

function escapexmltext($text) {
	return str_replace('&', '&amp;', $text);
}

$nppconf = parse_ini_file('compiler/notepad++/npp.ini', true);

$pawn_xml_dir = resolve_path(dirname($nppconf['paths']['pawn_xml']));
$user_define_lang_dir = resolve_path(dirname($nppconf['paths']['user_define_lang']));

function resolved_symbol_name($variable, $file = null, $remove_module_prefix = false) {
	if (empty($file) || !preg_match('/^this./', $variable))
		return $variable;
	
	if (preg_match('/(\\\\|\/)([^\\\\\/]+)(\\\\|\/)(callbacks(\\\\|\/))?[^\\\\\/]+\.inc$/i', $file, $matches)) {
		$variable = preg_replace('/^this./', "{$matches[2]}.", $variable);
	} else {
		echo "WARNING: Unable to determine module for \"{$file}\".\n";
	}
	
	return $variable;
}

if ($pawn_xml_dir !== false && $user_define_lang_dir !== false) {
	$start = microtime(true);
	$scanner = new PAWNScanner\Scanner();
	
	$scanner->parse_comments = true;
	
	$pawn_xml = trim($pawn_xml_dir, '\\/') . '/' . basename($nppconf['paths']['pawn_xml']);
	
	$user_define_lang = trim($user_define_lang_dir, '\\/') . '/' . basename($nppconf['paths']['user_define_lang']);
	
	$scanner->scan_dir('include', array('a_npc.inc', 'animation_names.inc'));
	$scanner->scan_dir('gamemodes/modules');
	$scanner->scan_dir('gamemodes/lib');
	$scanner->scan_dir('YSI/pawno/include');
	
	$funcs = $scanner->functions;
	
	$scanner->functions = array();
	
	foreach ($funcs as $function) {
		$function->name = resolved_symbol_name($function->name, @$function->info->file);
		
		$scanner->functions[$function->name] = $function;
	}
	
	unset($funcs);
	
	$constants = $scanner->constants;
	
	$scanner->constants = array();
	
	foreach ($constants as $constant) {
		$constant->varname = resolved_symbol_name($constant->varname, @$constant->info->file);
		
		$scanner->constants[$constant->varname] = $constant;
	}
	
	unset($constants);
	
	foreach ($scanner->macros as $macro)
		$macro->search = resolved_symbol_name($macro->search, @$macro->info->file);
	
	foreach ($scanner->enums as $enum) {
		if ($enum->name !== null)
			$enum->name = resolved_symbol_name($enum->name, @$enum->info->file);

		foreach ($enum->entries as $entry) {
			$entry->varname = resolved_symbol_name($entry->varname, @$enum->info->file);
		}
	}
	
	// -----------------------------------------------------------------------------
	//  PAWN.xml
	// -----------------------------------------------------------------------------
	$xml = new SimpleXMLElement('<NotepadPlus></NotepadPlus>');

	$ac = $xml->addChild('AutoComplete');

	$ac->addAttribute('language', 'PAWN');
	
	$keywords = array();

	// Add environment info
	$env = $ac->addChild('Environment');

	$env->addAttribute('ignoreCase',     'no');
	$env->addAttribute('startFunc',      '(');
	$env->addAttribute('stopFunc',       ')');
	$env->addAttribute('paramSeparator', ',');
	$env->addAttribute('terminal',       ';');

	// Add preprocessor directives
	$directives = array('assert', 'define', 'else', 'elseif', 'emit', 'endif',
	                    'endinput', 'error', 'file', 'if', 'include', 'line',
	                    'pragma', 'tryinclude', 'undef');

	foreach ($directives as $directive) {
		$keywords[] = $kw = new SimpleXMLElement('<KeyWord></KeyWord>');

		$kw->addAttribute('name', '#' . $directive);
	}

	// Add constants created by the compiler
	$constants = array('cellbits', 'cellmax', 'cellmin', 'charbits', 'charmax',
	                   'charmin', 'debug', 'EOS', 'false', 'true', 'ucharmax',
	                   '__Pawn', );

	foreach ($constants as $constant) {
		$keywords[] = $kw = new SimpleXMLElement('<KeyWord></KeyWord>');
		
		$kw->addAttribute('name', escapexmltext($constant));
	}

	// Add macros
	foreach ($scanner->macros as $macro) {
		if (preg_match('/^(?!(?:H_|HASH))([a-z][a-z0-9@_\.]*[a-z])\(((?:%[0-9],?)*)\)/i', $macro->search, $matches)) {
			$keywords[] = $func = new SimpleXMLElement('<KeyWord></KeyWord>');

			$func->addAttribute('name', $matches[1]);
			$func->addAttribute('func', 'yes');

			$params = $func->addChild('Overload');

			$params->addAttribute('retVal', '');
			
			$params_split = explode(',', @$matches[2]);
			
			foreach ($params_split as $param) {
				$params->addChild('Param')->addAttribute('name', escapexmltext($param));
			}
		} else if (preg_match('/^[a-z@_][a-z0-9@_\.\:]*$/i', $macro->search)) {
			$keywords[] = $kw = new SimpleXMLElement('<KeyWord></KeyWord>');
			
			$kw->addAttribute('name', escapexmltext($macro->search));
		}
	}

	// Add constants
	foreach ($scanner->constants as $name => $value) {
		$keywords[] = $kw = new SimpleXMLElement('<KeyWord></KeyWord>');
		
		$kw->addAttribute('name', escapexmltext($name));
	}

	// Add enumerations
	foreach ($scanner->enums as $enum) {
		if ($enum->name !== null) {
			$keywords[] = $kw = new SimpleXMLElement('<KeyWord></KeyWord>');
			
			$kw->addAttribute('name', escapexmltext($enum->name));
		}

		foreach ($enum->entries as $entry) {
			$keywords[] = $kw = new SimpleXMLElement('<KeyWord></KeyWord>');
			
			$kw->addAttribute('name', escapexmltext($entry->varname));
		}
	}

	// Add functions
	foreach ($scanner->functions as $function) {
		$keywords[] = $func = new SimpleXMLElement('<KeyWord></KeyWord>');

		$func->addAttribute('name', $function->name);
		$func->addAttribute('func', 'yes');

		$params = $func->addChild('Overload');

		$params->addAttribute('retVal', $function->tag === null ? '' : $function->tag . ':');

		foreach ($function->arguments as $argument) {
			$params->addChild('Param')->addAttribute('name', escapexmltext($argument));
		}
	}
	
	// Sort all keywords (VERY important)
	usort($keywords, function ($left, $right) {
		return strcmp($left->attributes()->name, $right->attributes()->name);
	});
	
	$keyword_used = array();

	foreach ($keywords as $k => $kw) {
		$name = (string) $kw->attributes()->name;
		
		if (isset($keyword_used[$name]))
			continue;
		
		$keyword_used[$name] = true;
		
		SimpleXMLElement_append($ac, $kw);
	}
	
	// Use the DOMElement to format the output.
	$dom = dom_import_simplexml($xml)->ownerDocument;
	$dom->formatOutput = true;

	// Save it.
	file_put_contents($pawn_xml, $dom->saveXML());

	// -----------------------------------------------------------------------------
	//  userDefineLang.xml
	// -----------------------------------------------------------------------------
	
	if (file_exists($user_define_lang)) {
		$xml = new SimpleXMLElement($user_define_lang, LIBXML_PARSEHUGE, true);
		
		$query = '/NotepadPlus/UserLang[@name="PAWN"]';
		
		$result = $xml->xpath($query);
		
		if (isset($result[0])) {
			$dom = dom_import_simplexml($xml);
			
			foreach ($dom->childNodes as $child) {
				if (!$child->hasAttributes())
					continue;
				
				$attr = $child->attributes->getNamedItem('name');
				
				if ($attr) {
					if (strtolower($attr->nodeValue) == 'pawn') {
						$dom->removeChild($child);
					}
				}
			}
			
			$xml = simplexml_import_dom($dom);
			
			unset($dom);
		}
	} else {
		$xml = new SimpleXMLElement('<NotepadPlus></NotepadPlus>');
	}
	
	$ul = $xml->addChild('UserLang');
	
	$ul->addAttribute('name', 'PAWN');
	$ul->addAttribute('ext', 'pwn inc');
	
	$settings = $ul->addChild('Settings');

	$setting = $settings->addChild('Global');
	$setting->addAttribute('caseIgnored', 'no');
	$setting->addAttribute('escapeChar', '\\');

	$setting = $settings->addChild('TreatAsSymbol');
	$setting->addAttribute('comment', 'yes');
	$setting->addAttribute('commentLine', 'yes');

	$setting = $settings->addChild('TreatAsSymbol');
	$setting->addAttribute('words1', 'no');
	$setting->addAttribute('words2', 'no');
	$setting->addAttribute('words3', 'no');
	$setting->addAttribute('words4', 'no');

	$keywords = $ul->addChild('KeywordLists');

	$operators = '- ! % &amp; ( ) , . : ; ? [ \ ] | ~ + &lt; = &gt;';

	$keywords->addChild('Keywords', escapexmltext($operators))->addAttribute('name', 'Operators');
	$keywords->addChild('Keywords', escapexmltext('"\'0"\'0'))->addAttribute('name', 'Delimiters');
	$keywords->addChild('Keywords', '{')->addAttribute('name', 'Folder+');
	$keywords->addChild('Keywords', '}')->addAttribute('name', 'Folder-');
	$keywords->addChild('Keywords', escapexmltext('1/* 2*/ 0//'))->addAttribute('name', 'Comment');


	// Add function names
	$words1 = array_keys($scanner->functions);
	
	foreach ($words1 as $k => $func) {
		$words1[$k] = preg_replace('/^.*\b([a-z][a-z0-9@_]*)$/i', '$1',  $func);
	}
	
	sort($words1);
	
	$keywords->addChild('Keywords', escapexmltext(implode(' ', $words1)))
		->addAttribute('name', 'Words1');

	// Add macro search strings, constants, and enums
	$words2 = array_keys($scanner->constants);

	foreach ($scanner->macros as $macro) {
		if (preg_match('/^([a-z][a-z0-9@_\.]*[a-z])\(((?:%[0-9],?)*)\)/i', $macro->search, $matches)) {
			$matches[1] = preg_replace('/^.*\b([a-z][a-z0-9@_]*)$/i', '$1',  $matches[1]);
			
			$words2[] = $matches[1];
		} else if (preg_match('/^[a-z][a-z0-9@_\.\:]*$/i', $macro->search)) {
			$macro->search = preg_replace('/^.*\b([a-z][a-z0-9@_]*)$/i', '$1',  $macro->search);
			
			if (!empty($macro->search))
				$words2[] = $macro->search;
		}
	}

	foreach ($scanner->enums as $enum) {
		if ($enum->name !== null)
			$words2[] = $enum->name;

		foreach ($enum->entries as $entry) {
			//$words2[] = $entry->varname;
		}
	}
	
	$words2 = array_unique($words2);
	
	foreach ($words2 as $k => $word) {
		$words2[$k] = preg_replace('/^.*\./', '',  $word);
		
		if (in_array($words2[$k], $words1) || !preg_match('/^[a-z0-9@_]+$/i', $words2[$k]))
			unset($words2[$k]);
	}
	
	sort($words2);
	
	$keywords->addChild('Keywords', escapexmltext(implode(' ', $words2)))->addAttribute('name', 'Words2');

	// Add compiler directives
	sort($directives);
	
	$keywords->addChild('Keywords', escapexmltext('#' . implode(' #', $directives)))->addAttribute('name', 'Words3');

	// Add tags and compiler keywords
	$tags = array_flip(array(
		'assert', 'break', 'case', 'char', 'const', 'continue', 'default',
		'defined', 'do', 'else', 'enum', 'exit', 'for', 'forward', 'goto',
		'if', 'native', 'new', 'operator', 'public', 'return', 'sizeof',
		'sleep', 'state', 'static', 'stock', 'switch', 'tagof', 'foreach'
	));
	
	foreach ($pbp->modules as $module)
		$tags[$module] = true;
	
	$tags['this'] = true;

	// Tags in functions
	foreach ($scanner->functions as $function) {
		if ($function->tag !== null)
			$tags[$function->tag] = true;

		// Tags in function arguments
		foreach ($function->arguments as $argument) {
			if ($argument->tags === null)
				continue;

			foreach ($argument->tags->tags as $tag)
				$tags[$tag] = true;
		}
	}

	// Tags in constants
	foreach ($scanner->constants as $constant) {
		if ($constant->tags === null)
			continue;

		foreach ($constant->tags->tags as $tag) {
			$tags[$tag] = true;
		}
	}

	// Tags in enums
	foreach ($scanner->enums as $enum) {
		if ($enum->tag !== null)
			$tags[$enum->tag] = true;

		foreach ($enum->entries as $entry) {
			if ($entry->tags === null)
				continue;

			foreach ($entry->tags->tags as $tag)
				$tags[$tag] = true;
		}
	}

	$tags = array_keys($tags);
	
	foreach ($tags as $k => $tag) {
		$tags[$k] = preg_replace('/^.*\./', '',  $tag);
		
		if (in_array($tags[$k], $words1) || in_array($tags[$k], $words2))
			unset($tags[$k]);
	}
	
	sort($tags);

	$keywords->addChild('Keywords', escapexmltext(implode(' ', $tags)))->addAttribute('name', 'Words4');

	// Style info
	$styles = <<<EOD
<?xml version="1.0"?>
<Styles>
	<WordsStyle name="DEFAULT" styleID="11" fgColor="000000" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="FOLDEROPEN" styleID="12" fgColor="000000" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="FOLDERCLOSE" styleID="13" fgColor="000000" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="KEYWORD1" styleID="5" fgColor="000080" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="KEYWORD2" styleID="6" fgColor="800000" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="KEYWORD3" styleID="7" fgColor="800000" bgColor="FFFFFF" fontName="" fontStyle="1" />
	<WordsStyle name="KEYWORD4" styleID="8" fgColor="0000C0" bgColor="FFFFFF" fontName="" fontStyle="1" />
	<WordsStyle name="COMMENT" styleID="1" fgColor="008000" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="COMMENT LINE" styleID="2" fgColor="008000" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="NUMBER" styleID="4" fgColor="FF8000" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="OPERATOR" styleID="10" fgColor="000000" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="DELIMINER1" styleID="14" fgColor="808080" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="DELIMINER2" styleID="15" fgColor="808080" bgColor="FFFFFF" fontName="" fontStyle="0" />
	<WordsStyle name="DELIMINER3" styleID="16" fgColor="000000" bgColor="FFFFFF" fontName="" fontStyle="0" />
</Styles>
EOD;

	$styles = new SimpleXMLElement($styles);

	$ul->addChild('Styles');

	foreach ($styles as $style) {
		$ws = $ul->Styles->addChild('WordsStyle');

		foreach ($style->attributes() as $name => $value) {
			$ws->addAttribute($name, $value);
		}
	}

	// Use the DOMElement to format the output.
	$dom = dom_import_simplexml($xml)->ownerDocument;
	$dom->formatOutput = true;

	// Save it.
	file_put_contents($user_define_lang, $dom->saveXML());
}

// http://www.php.net/manual/en/class.simplexmlelement.php#108112
function SimpleXMLElement_append($parent, $child)
{
    // get all namespaces for document
    $namespaces = $child->getNamespaces(true);

    // check if there is a default namespace for the current node
    $currentNs = $child->getNamespaces();
    $defaultNs = count($currentNs) > 0 ? current($currentNs) : null;
    $prefix = (count($currentNs) > 0) ? current(array_keys($currentNs)) : '';
    $childName = strlen($prefix) > 1
        ? $prefix . ':' . $child->getName() : $child->getName();

    // check if the value is string value / data
    if (trim((string) $child) == '') {
        $element = $parent->addChild($childName, null, $defaultNs);
    } else {
        $element = $parent->addChild(
            $childName, htmlspecialchars((string)$child), $defaultNs
        );
    }

    foreach ($child->attributes() as $attKey => $attValue) {
        $element->addAttribute($attKey, $attValue);
    }
    foreach ($namespaces as $nskey => $nsurl) {
        foreach ($child->attributes($nsurl) as $attKey => $attValue) {
            $element->addAttribute($nskey . ':' . $attKey, $attValue, $nsurl);
        }
    }

    // add children -- try with namespaces first, but default to all children
    // if no namespaced children are found.
    $children = 0;
    foreach ($namespaces as $nskey => $nsurl) {
        foreach ($child->children($nsurl) as $currChild) {
            SimpleXMLElement_append($element, $currChild);
            $children++;
        }
    }
    if ($children == 0) {
        foreach ($child->children() as $currChild) {
            SimpleXMLElement_append($element, $currChild);
        }
    }
}