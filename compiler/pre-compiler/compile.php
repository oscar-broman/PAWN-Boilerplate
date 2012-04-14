<?php
echo "PAWN Boilerplate\n\n";

ini_set('memory_limit', '128M');

// Download the server files if needed
if (!file_exists('samp-server.exe') || !file_exists('include/a_samp.inc')) {
	define('SERVER_DL_URL',  'http://team.sa-mp.com/files/samp03dsvr_R2_win32.zip');
	define('SERVER_DL_SIZE', 1876336);
	define('SERVER_DL_FILE', tempnam(sys_get_temp_dir(), 'samp-server.zip'));
	
	$fp_out = null;
	$fp_in = null;
	
	echo "Downloading the SA-MP server, please wait.\n";
	
	if (($fp_out = fopen(SERVER_DL_FILE, 'wb'))
	 && ($fp_in = fopen(SERVER_DL_URL, 'rb'))) {
		$i = 0;
		
		while (!feof($fp_in)) {
			fwrite($fp_out, fread($fp_in, 8192));
			
			if ($i++ == 100 || feof($fp_in)) {
				$i = 0;
				
				echo "  " . floor((ftell($fp_in) / SERVER_DL_SIZE) * 100) . "%\n";
			}
		}
		
		echo "\n";
		
		if (filesize(SERVER_DL_FILE) != SERVER_DL_SIZE)
			echo "PBP Warning: Unexpected filesize of the server files.\n";
		
		echo "Extracting the server files.\n";
		
		$zip = new ZipArchive;
		$res = $zip->open(SERVER_DL_FILE);
		
		if ($res === true) {
			$files = array(
			    'samp-license.txt',
		    	'server-readme.txt',
		    	'samp-server.exe',
				'samp-npc.exe',
				'announce.exe',
				'pawno/pawnc.dll',
				'pawno/pawncc.exe'
		    );

			for ($i = 0; $i < $zip->numFiles; $i++) {
				$file = $zip->getNameIndex($i);
				
				if (preg_match('/^pawno\/include\/.*?\.inc$/', $file))
					$files[] = $file;
			}
			
		    $success = $zip->extractTo(realpath('.'), $files);
			
		    $zip->close();
			
			@unlink(SERVER_DL_FILE);
			
			if ($success) {
				rename('pawno/pawnc.dll', 'compiler/bin/pawnc.dll');
				rename('pawno/pawncc.exe', 'compiler/bin/pawncc.exe');
				
				chmod('compiler/bin/pawncc.exe', 0755);
				
				$ignore_changed = false;
				
				if (file_exists('.gitignore'))
					$ignore = @file('.gitignore', FILE_IGNORE_NEW_LINES);
				else
					$ignore = null;
				
				foreach ($files as &$file) {
					if (preg_match('/^pawno\/(pawnc\.dll|pawncc\.exe)$/', $file, $matches))
						$file = "compiler/bin/{$matches[1]}";
					
					if (preg_match('/^pawno\/include\/(.*?\.inc)$/', $file, $matches)) {
						$newfile = "include/{$matches[1]}";
						
						if (file_exists($newfile))
							unlink($newfile);
						
						rename($file, $newfile);
						
						$file = $newfile;
					}
					
					if ($ignore && !array_search("/$file", $ignore)) {
						$ignore[] = "/$file";
						
						$ignore_changed = true;
					}
				}
				
				rmdir('pawno/include');
				rmdir('pawno');
				
				if ($ignore_changed) {
					echo "Updating .gitignore.\n";
					
					file_put_contents('.gitignore', implode("\n", $ignore));
				}
			} else {
				echo "PBP Error: Unable to extract the server.\n";
				
				exit;
			}
		} else {
		    echo "PBP Error: Unable to extract the archive.";
		
			@unlink(SERVER_DL_FILE);
			
			exit;
		}
		
		fclose($fp_in);
		fclose($fp_out);
	} else {
		if ($fp_in) fclose($fp_in);
		if ($fp_out) fclose($fp_out);
		
		echo "PBP Error: Unable to download the SA-MP server. Download it from http://sa-mp.com/ and put the files in the PBP directory.";
		
		exit;
	}
	
	echo "Done.\n";
}

if (!file_exists('include/amx_assembly/amx_header.inc')) {
	echo "Downloading amx_assembly, please wait.\n";
	
	$tmpfile = tempnam(sys_get_temp_dir(), 'amx_assembly.zip');
	
	file_put_contents($tmpfile, @file_get_contents('https://github.com/Zeex/amx_assembly/zipball/master'));
	
	if (filesize($tmpfile) == 0) {
		echo "PBP Error: Unable to download amx_assembly from https://github.com/Zeex/amx_assembly/zipball/master\n";
		
		exit;
	}
	
	echo "Downloaded.\n";
	echo "Extracting amx_assembly.\n";
	
	$zip = new ZipArchive;
	$res = $zip->open($tmpfile);
	
	if ($res === true) {
		$zip->extractTo('include/amx_assembly');
		
		$folder = strstr($zip->getNameIndex(0), '/', true);
		
		foreach (glob("include/amx_assembly/$folder/*") as $file) {
			$basename = basename($file);
			$newfile = "include/amx_assembly/$basename";
			
			if (file_exists($newfile))
				unlink($newfile);
			
			rename($file, $newfile);
		}
		
		$zip->close();
	} else {
		echo "Unable to open the zip archive.\n";
		
		exit;
	}
	
	echo "Done.\n";
}

if (!file_exists('YSI/pawno/include/YSI.inc')) {
	echo "Downloading YSI, please wait.\n";
	
	$tmpfile = tempnam(sys_get_temp_dir(), 'YSI.zip');
	
	if (!($fp_in = fopen('https://github.com/Y-Less/YSI/zipball/master', 'rb'))) {
		echo "PBP Error: Failed to download YSI from https://github.com/Y-Less/YSI/zipball/master\n";
		
		exit;
	}
	
	if (!($fp_out = fopen($tmpfile, 'wb'))) {
		fclose($fp_in);
		
		echo "PBP Error: Failed to open YSI.zip for writing.\n";
		
		exit;
	}
	
	while (!feof($fp_in))
		fwrite($fp_out, fread($fp_in, 8192));
	
	fclose($fp_in);
	fclose($fp_out);
	
	function recursiveDelete($str) {
		if (is_file($str)) {
			return @unlink($str);
		} else if (is_dir($str)) {
			$scan = glob(rtrim(realpath($str), '/') . '/*');
			
			foreach($scan as $index=>$path)
				recursiveDelete($path);
			
			return @rmdir($str);
		}
	}
	
	echo "Extracting YSI.\n";
	
	$zip = new ZipArchive;
	
	$res = $zip->open($tmpfile);
	
	if ($res === true) {
		$zip->extractTo('.');
		
		$folder = strstr($zip->getNameIndex(0), '/', true);
		
		$zip->close();
		
		if (file_exists('YSI'))
			recursiveDelete('YSI');
		
		rename($folder, 'YSI');
		
		echo "$folder\n";
	} else {
		echo "Unable to open the zip archive.\n";

		exit;
	}
	
	exit;
}

require 'class.PBP.php';

$pbp = new PBP();

$pbp->compile();

echo $pbp->output;