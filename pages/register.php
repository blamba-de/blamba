<?php
$devices = $db->prepared_fetch("SELECT * FROM devices WHERE user = ?;", "s", session_id());

echo $twig->render('pages/' . $page . '.html',
[
	'config' => $config,
	'devices' => $devices,
	'registration_token' => $_SESSION['registration_token']
]);