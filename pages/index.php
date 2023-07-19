<?php
$devices = $db->prepared_fetch("SELECT * FROM devices WHERE user = ?;", "s", session_id());

$top_poly = $db->fetch("SELECT * FROM `content` WHERE `type` = 'polyphonic-ring' ORDER BY `played` DESC LIMIT 0,6");
$top_operator = $db->fetch("SELECT * FROM `content` WHERE `type` = 'operator-logo' ORDER BY `played` DESC LIMIT 0,21");
$top_bitmap = $db->fetch("SELECT * FROM `content` WHERE `type` = 'bitmap' ORDER BY `played` DESC LIMIT 0,4");

echo $twig->render('pages/' . $page . '.html', [
	'config' => $config,
	'devices' => $devices,
	'registration_token' => $_SESSION['registration_token'],
	"top_poly" => $top_poly,
	"top_operator" => $top_operator,
	"top_bitmap" => $top_bitmap,
]);