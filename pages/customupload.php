<?php
$devices = $db->prepared_fetch("SELECT * FROM devices WHERE user = ?;", "s", session_id());

function sanity_check_file($file, $mime)
{	

	// check currently disabled as it gives false positives with error code 0

	// if ($file["error"])
	// 	error_log("File upload error: " . $file["error"]);
	// 	return "Keine Datei ausgewaehlt oder Upload fehlgeschlagen: ".$file["error"];

	if ($file["size"] > 2048 * 1024)
		return "Datei zu gross". $file["size"];

	if (!in_array($file["type"], $mime))
	{	
		return "Falsch uebermittelter MIME-Type: ". $file["type"];
	}

	if (!in_array(mime_content_type($file['tmp_name']), $mime))
	{	
		return "Falscher MIME-TypeL ". mime_content_type($file['tmp_name']);
	}

	return false;
}

function sanitize_filename($file, $extensions)
{
	$pathinfo = pathinfo($file["name"]);
	$filename = preg_replace('/[^A-Za-z0-9\-\_]/', '', $pathinfo['filename']);
	$extension = preg_replace('/[^a-z]/', '', strtolower($pathinfo['extension']));

	if (!in_array($extension, $extensions))
	{	
		error_log("Invalid extension: " . $extension);
		return false;
	}

	return $filename . "." . $extension;
}

function save_upload($file, $type, $extensions, $msn, $data)
{
	global $userupload_path;
	$filename = sanitize_filename($file, $extensions);
	error_log("Filename: " . $filename);
	if (!$filename)
		return false;

	$wappush = WAPPush::generate_stored( $type, "custom", $filename, $msn, $data );
	move_uploaded_file($file["tmp_name"], $userupload_path . "/" . $wappush["uuid"]);

	return WAPPush::generate_wap_push($wappush["url"], $wappush["name"]);
}

if (isset($_POST["type"]))
{
	$device = $db->prepared_fetch_one("SELECT * FROM devices WHERE user = ? AND id = ?;", "ss", session_id(), (int)$_POST['msn']);
	if (is_null($device))
	{
		echo $twig->render('pages/404.html', []); exit();
	}
	$gateway = $db->prepared_fetch_one("SELECT * FROM gateways WHERE enabled = 1 AND id = ?;", "i", $device["gateway"]);

	$data = "";
	$error = "";

	switch ($_POST["type"])
	{
		case 'operator-logo':
		case 'group-logo':
			if (($error = sanity_check_file($_FILES["file-graphics"], ["image/png", "image/jpeg", "image/gif", "image/bmp"])) === false)
			{
				if ($_POST["type"] == "operator-logo")
					$data = OperatorLogo::convert_to_nokia_sms_operator($_FILES["file-graphics"]["tmp_name"], 262, 42, "-monochrome -threshold 0 -negate");

				if ($_POST["type"] == "group-logo")
					$data = OperatorLogo::convert_to_nokia_sms_group($_FILES["file-graphics"]["tmp_name"], "-monochrome -threshold 0 -negate");
			}
			break;
		case 'rtttl':
			if (($error = sanity_check_file($_FILES["file-rtttl"], ["text/plain"])) === false)
			{
				$data = RTTTL::convert_to_nokia_sms($_FILES["file-rtttl"]["tmp_name"]);
			}
			break;
		case 'bitmap':
			if (($error = sanity_check_file($_FILES["file-graphics"], ["image/png", "image/jpeg", "image/gif", "image/bmp"])) === false)
			{
				$data = save_upload($_FILES["file-graphics"], "bitmap", ["png", "jpg", "jpeg", "gif", "bmp"], $device["msn"], ["mode" => "custom"]);
			}
			break;
		case 'polyphonic-ring':
			if (($error = sanity_check_file($_FILES["file-midi"], ["audio/midi", "audio/x-midi"])) === false)
			{
				$data = save_upload($_FILES["file-midi"], "polyphonic-ring", ["mid", "midi"], $device["msn"], ["mode" => "custom"]);
			}
			break;
		case 'j2me':
			if (($error = sanity_check_file($_FILES["file-java"], ["application/java-archive", "application/x-java-applet", "application/java", "application/zip"])) === false)
			{	
				$data = save_upload($_FILES["file-java"], "j2me", ["jar"], $device["msn"], ["mode" => "custom"]);
			}
			break;
	}

	$error = SMS::sanity_check_udh_array($data);
	if (trim($data) == "")
	{	
		if ($error == "")
			$error = "Die Datei konnte nicht umgewandelt werden.";
	}
	if ($error == "")
	{
		$success = "Nachrichten wurden zum Versand hinterlegt.";
		SMS::send_udh_sms($gateway, $device["msn"], $data);
	}
}

echo $twig->render('pages/' . $page . '.html',
[
	'success' => $success,
	'error' => $error,
	'data' => $data,
	'smsarray' => SMS::split_sms_string($data),
	'devices' => $devices,
	'registration_token' => $_SESSION['registration_token'],
	'config' => $config,
]);