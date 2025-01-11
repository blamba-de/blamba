<?php
class RTTTL
{
	static function convert_to_nokia_sms($rtttl_filename)
	{
		$sms = RingtoneTools::run_sandboxed_ringtonetools("rtttl", $rtttl_filename, "nokia", "-u -s 280");

		if ($sms["success"])
		{
			return $sms["output"];
		}
		else
		{
			return $sms["stdout"] . $sms["stderr"];
		}
	}

	static function convert_to_nokia_rng($rtttl_filename)
	{
		$sms = RingtoneTools::run_sandboxed_ringtonetools("rtttl", $rtttl_filename, "nokia", "-u -s 280");

		if ($sms["success"])
		{
			$rng = "";
			foreach (explode("\n", $sms["output"]) as $sms)
			{
				$sms = trim($sms);
				if (!empty($sms))
				{
					$rng .= hex2bin(substr($sms, 24));
				}
			}

			return $rng;
		}
		else
		{
			return $sms["stdout"] . $sms["stderr"];
		}
	}


	static function convert_to_midi($rtttl_filename)
	{
		$midi = RingtoneTools::run_sandboxed_ringtonetools("rtttl", $rtttl_filename, "midi");

		if ($midi["success"])
		{
			return $midi["output"];
		}
		else
		{
			return $midi["stdout"] . $midi["stderr"];
		}
	}
}