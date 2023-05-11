<?php

$top_poly = $db->fetch("SELECT * FROM `content` WHERE `type` = 'polyphonic-ring' ORDER BY `played` DESC LIMIT 0,6");
$top_operator = $db->fetch("SELECT * FROM `content` WHERE `type` = 'operator-logo' ORDER BY `played` DESC LIMIT 0,21");
$top_bitmap = $db->fetch("SELECT * FROM `content` WHERE `type` = 'bitmap' ORDER BY `played` DESC LIMIT 0,4");

echo $twig->render('pages/' . $page . '.html', [
	"top_poly" => $top_poly,
	"top_operator" => $top_operator,
	"top_bitmap" => $top_bitmap,
]);