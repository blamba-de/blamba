<?php
class Device
{
	static function generate_registration_token()
	{
		global $db;
		while (!isset($_SESSION['registration_token']))
		{
			$registration_token = random_int(0, 999999);
			// Yes, this has a ever so slight chance of racing between the check and the save. 
			// Oh well, DBMS still enforces uniqueness, saving will just fail.
			if ($db->prepared_fetch_one("SELECT id FROM `sessions` WHERE `registration_token` = ?", "i", $registration_token) == NULL)
			{
				error_log("generated token: " . $registration_token);
				$_SESSION['registration_token'] = $registration_token;
				Session::current()->save();
			}
		}

		return $_SESSION['registration_token'];
	}

	static function check_registration_token($registration_token, $msn)
	{
		global $db;

		$session = $db->prepared_fetch_one("SELECT id FROM `sessions` WHERE `registration_token` = ?", "i", (int) $registration_token);
		if ($session !== NULL)
		{
			$db->prepared_query("DELETE FROM `devices` WHERE `msn` = ?;", "s", $msn);
			$db->prepared_query("INSERT INTO `devices` (`user`, `msn`) VALUES (?, ?);", "ss", $session['id'], $msn);
			return true;
		}

		return false;
	}
}