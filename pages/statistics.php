<?php

$data = "";
$success = false;

$registered_devices = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`devices`", "sum");

$received_sms = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`log` WHERE `type` = 'parse_inbound_sms' ", "sum");
$sent_wappush = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`wappush` ", "sum");

$sent_ringtones = $db->fetch("SELECT SUM(sent) as `sum` FROM `blamba`.`content` WHERE `type` = 'polyphonic-ring' OR `type` = 'rtttl'", "sum");
$sent_logos = $db->fetch("SELECT SUM(sent) as `sum` FROM `blamba`.`content` WHERE `type` = 'bitmap' OR `type` = 'operator-logo'", "sum");
$sent_java = $db->fetch("SELECT SUM(sent) as `sum` FROM `blamba`.`content` WHERE `type` = 'j2me'", "sum");

$available_ringtones = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`content` WHERE `type` = 'polyphonic-ring' OR `type` = 'rtttl'", "sum");
$available_logos = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`content` WHERE `type` = 'bitmap' OR `type` = 'operator-logo'", "sum");

$sent_text_sms = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`log` WHERE `type` = 'send_text_sms' ", "sum");
$sent_udh_sms = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`log` WHERE `type` = 'send_udh_sms' ", "sum");

echo $twig->render('pages/' . $page . '.html',
[
	'registered_devices' => $registered_devices,
	'sent_wappush' => $sent_wappush,

	'sent_ringtones' => $sent_ringtones,
	'sent_logos' => $sent_logos,
	'sent_java' => $sent_java,
	'available_ringtones' => $available_ringtones,
	'available_logos' => $available_logos,
	'received_sms' => $received_sms,
	'sent_text_sms' => $sent_text_sms,
	'sent_udh_sms' => $sent_udh_sms,

	'devices' => $devices,
	'registration_token' => $_SESSION['registration_token'],
	'config' => $config,
]);
