<?php
if ($_SERVER["HTTP_TOKEN"] !== $config["log_token"])
{
	die("invalid token");
}

$log = $db->fetch("SELECT * FROM `log` WHERE `type` NOT IN ('convert_image_to_gif', 'run_sandboxed_ringtonetools') ORDER BY id DESC LIMIT 0, 100");

echo json_encode($log, JSON_PRETTY_PRINT);