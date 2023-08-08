<?php
require_once("bootstrap.php");

$token = $_GET["token"] ?? '';
$gateway = $db->prepared_fetch_one("SELECT * FROM gateways WHERE token = ? AND enabled = 1;", "s", $token);

if (!$gateway)
{
	http_return_code(403);
	echo "access denied";
	die();
}

$sms_from = $_SERVER["HTTP_X_KANNEL_FROM"];
$sms_message = file_get_contents("php://input");

SMS::parse_inbound_sms($gateway, $sms_message, $sms_from);