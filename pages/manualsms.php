<?php
$data = "0B0504158100000003010201024A3A7535BDB9AD95E48125CDB185B9900400A320CAEA2AC490628AB0C30BB0AB09A0A912418A26C4906289B08B12718A26C22C4906288B1241

0B05041581000000030102028710410A2AC490628AA4C30BB0AB09A0A912418A2EC30C490628C312418A348490428BA12418A30C2EC2AC26C2EC30C490628C312418A2E800";

$success = "";
$error = "";
if (isset($_POST["data"]))
{
	$data = $_POST["data"];
	$error = SMS::sanity_check_udh_array($data);

	if ($error == "")
	{
		$success = "Nachrichten wurden zum Versand hinterlegt.";

		$device = $db->prepared_fetch_one("SELECT * FROM devices WHERE user = ? AND msn = ?;", "ss", session_id(), (int)$_POST['msn']);
		if (is_null($device))
		{
			echo $twig->render('pages/404.html', []); exit();
		}
		SMS::send_udh_sms($device["msn"], $data);
		$smsarray = SMS::split_sms_string($data);
	}
}

$devices = $db->prepared_fetch("SELECT * FROM devices WHERE user = ?;", "s", session_id());

echo $twig->render('pages/' . $page . '.html',
[
	'success' => $success,
	'error' => $error,
	'smsarray' => $smsarray,
	'data' => $data,
	'devices' => $devices,
	'registration_token' => $_SESSION['registration_token'],
	'config' => $config,
]);