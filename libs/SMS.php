<?php
class SMS
{
	static function send_text_sms($destination, $data)
	{
		global $db;
		$db->prepared_query("INSERT INTO `sms_queue` (`destination`, `type`, `data`, `queued_at`) VALUES (?, 'text', ?, current_timestamp());", "ss", $destination, $data);
	}

	static function send_udh_sms($destination, $data)
	{
		global $db;
		foreach (explode("\n", $data) as $line)
		{
		 	$line = trim($line);
		 	if ($line != '')
		 	{
				$db->prepared_query("INSERT INTO `sms_queue` (`destination`, `type`, `data`, `queued_at`) VALUES (?, 'udh', ?, current_timestamp());", "ss", $destination, $line);
		 	}
		}
	}

	static function sanity_check_udh_array($data)
	{
		$error = "";
		$smscount = 0;

		foreach (explode("\n", $data) as $linecount => $line)
		{
		 	$line = trim($line);
		 	if ($line != '')
		 	{
				$smscount++;
				if ($smscount > 5)
				{
					$error .= "Zu viele Mitteilungen (" . $smscount . ")\n";
				}

				if (@hex2bin($line) === false)
				{
					$error .= "Hex-Decoding in Zeile " . ($linecount + 1) . " fehlgeschlagen\n";
				}

				if (strlen(@hex2bin($line)) > 140)
				{
					$error .= "Zeile " . ($linecount + 1) . " ist länger als 140 Bytes\n";
				}
		 	}
		}

		$error = trim($error);
		return $error;
	}

	static function split_sms_string($sms)
	{
		$smsarray = [];
		foreach (explode("\n", $sms) as $line)
		{
			$line = trim($line);
			if ($line != '')
			{
				if (@hex2bin($line) !== false)
				{
					$smsarray[] = @bin2hex(@hex2bin($line));
				}
			}
		}
		return $smsarray;
	}

	static function parse_inbound_sms($sms)
	{
		Logging::log("parse_inbound_sms", $sms->message, $sms->source->value);

		// parse registration SMS
        $matches = [];
		$re = '/reg.*?([0-9]{3,6})/mi';
		preg_match_all($re, $sms->message, $matches, PREG_SET_ORDER, 0);

		foreach ($matches as $match)
		{
			if (count($match) == 2)
			{
				if (Device::check_registration_token($match[1], $sms->source->value))
				{
					Logging::log("check_registration_token", $sms->message, $sms->source->value);
					SMS::send_text_sms($sms->source->value, "Deine Anmeldung war erfolgreich! Willkommen bei Blamba!");
					return true;
				}
			}
		}

		// parse content-request SMS
		$matches = [];
		$re = '/^(plain\s*?)?([0-9]{1,5})$/mi';
		preg_match_all($re, $sms->message, $matches, PREG_SET_ORDER, 0);
		foreach ($matches as $match)
		{
			if (Content::handle_sms_content_request($sms->source->value, $match))
			{
				return true;
			}
		}

		SMS::send_text_sms($sms->source->value, "Deine SMS wurde leider nicht verstanden! Bitte versuche es erneut. Du musst nicht auf Gross- und Kleinschreibung achten.");
	}
}