<?php

$data = "";
$success = false;
if (isset($_POST["msn"]))
{
	$device = $db->prepared_fetch_one("SELECT * FROM devices WHERE user = ? AND id = ?;", "ss", session_id(), (int)$_POST['msn']);
	if (is_null($device))
	{
		echo $twig->render('pages/404.html', []); exit();
	}
	$gateway = $db->prepared_fetch_one("SELECT * FROM gateways WHERE enabled = 1 AND id = ?;", "i", $device["gateway"]);

	$sms = WAPPush::generate_wap_push("http://wap.blamba.de/ep/", "Telefonbuch");
	SMS::send_udh_sms($gateway, $device["msn"], $sms);

	$data .= $sms . "\n\n";
	$success = "Das Lesezeichen wurde an dein Handy geschickt!";
}

$devices = $db->prepared_fetch("SELECT * FROM devices WHERE user = ?;", "s", session_id());

echo $twig->render('pages/' . $page . '.html',
[
	'success' => $success,
	'data' => $data,
	'smsarray' => SMS::split_sms_string($data),
	'events' => $events,
	'devices' => $devices,
	'registration_token' => $_SESSION['registration_token'],
	'config' => $config,
]);