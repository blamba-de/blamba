<?php
class Logging
{
	static function log($type, $log, $phonenumber = "")
	{
		global $db, $content_path;
		if (!is_string($log))
		{
			$log = json_encode($log, JSON_PRETTY_PRINT);
		}

		$db->prepared_query("INSERT INTO `log` (`date`, `session_id`, `phonenumber`, `type`, `log`) VALUES (NOW(), ?, ?, ?, ?);", "ssss", session_id(), $phonenumber, $type, $log);
	}
}