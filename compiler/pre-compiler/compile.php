<?php
echo "PAWN Boilerplate\n\n";

ini_set('memory_limit', '128M');

require 'class.PBP.php';

$pbp = new PBP();

$pbp->compile();

echo $pbp->output;