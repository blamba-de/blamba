<?php
$id = (int)trim(preg_replace('/[^0-9]/', '', $_POST["contentid"]));
$content = $db->prepared_fetch_one("SELECT content.*,authors.name as `authorname` FROM content, authors WHERE content.id = ? AND author = authors.id AND content.visible = 1;", "i", $id);

if (is_null($content))
{
	echo $twig->render('pages/404.html', []); exit();
}

// permissions, is this user allowed to access that device?
$device = $db->prepared_fetch_one("SELECT * FROM devices WHERE user = ? AND msn = ?;", "ss", session_id(), (int)$_POST['msn']);
if (is_null($device))
{
	echo $twig->render('pages/404.html', []); exit();
}

Logging::log("web_transfer", $_POST, $device["msn"]);

$db->prepared_query("UPDATE content SET sent = sent + 1 WHERE `id` = ? AND `visible` = 1;", "i", $content['id']);

// encode the actual data
$sms = false;
switch ($content["type"])
{
	case 'rtttl':
		$sms = RTTTL::convert_to_nokia_sms($content_path . $content["path"]);
		break;
	case 'operator-logo':
		// Invert image or not? (ImageMagick command line arguments)
		$im_params = "-monochrome -threshold 0 -negate";
		if ($_POST["invert"] == "true")
		{
			$im_params = "-monochrome -threshold 0 -negate -negate";
		}

		if ($_POST["logotype"] == "operator-logo")
		{
			$sms = OperatorLogo::convert_to_nokia_sms_operator($content_path . $content["path"], (int)$_POST["mcc"], (int)$_POST["mnc"], $im_params);
		}
		if ($_POST["logotype"] == "group-logo")
		{
			$sms = OperatorLogo::convert_to_nokia_sms_group($content_path . $content["path"], $im_params);
		}
		break;
	case 'j2me':
	case 'bitmap':
	case 'polyphonic-ring':
		$wappush = WAPPush::generate_stored( $content["type"], $content["name"], basename($content["path"]), $device["msn"], [
			"id" => $content['id'],
			"name" => $content["name"]
		]);
		$sms = WAPPush::generate_wap_push($wappush["url"], $wappush["name"]);
		break;
	default:
		break;
}

// actually send the SMS
SMS::send_udh_sms($device["msn"], $sms);

// statistics, count SMSes and data bytes
$smscount = 0;
$smsbytes = 0;
$smsarray = [];
foreach (explode("\n", $sms) as $line)
{
	$line = trim($line);
	if ($line != '')
	{
		$smscount++;
		$smsbytes += strlen(hex2bin($line));
		$smsarray[] = bin2hex(hex2bin($line));
	}
}

echo $twig->render('pages/' . $page . '.html',
[
	'type' => $content["type"],
	'type_human' => Content::human_readable_type($content["type"]),
	'content' => $content,
	'registration_token' => $_SESSION['registration_token'],
	'config' => $config,
	'sms' => $sms,
	'smsarray' => $smsarray,
	'smscount' => $smscount,
	'smsbytes' => $smsbytes
]);