<?php
class SMS
{
	static function generate_wap_push($url, $text)
	{
		$data = [
			"\x06", // UDH length
			"\x05", // Application Port Addressing, 16 bit address
			"\x04", // content length
			"\x0B\x84", // Destination port number, IANA WAP-Push Multimedia Messaging Service port (2948)
			"\x23\xF0", // Source port number
			"\x01", // trans id
			"\x06", // Push
			"\x04", // Header Length
			"\x03", // Length
			"\xAE", // Content type (application/vnd.wap.sic)
			"\x81", // Character Set (01 once removed high bit)
			"\xEA", // UTF-8 (6A once removed high bit)
			"\x02", // WBXML Version 1.2 
			"\x05", // SI Identifier
			"\x6A", // UTF-8
			"\x00", // End Data
			"\x45", // SI WBXML Tag
			"\xC6", // Indication Tag
			"\x0B", // href
			"\x03", // Open Text 
			$url,   // URL
			"\x00", // End Data
			"\x0A", // Created (date)
			"\xC3", // Data Follows
			"\x07", // Data Length
			hex2bin(date("YmdHis") ), // Date
			"\x01", // Close Attribute
			"\x03", // Open Text
			$text,  // Linktext
			"\x00", // End Data
			"\x01", // Close Indication Tag
			"\x01", // Close SI Tag
		];

		$bindata = implode("", $data);
		if (strlen($bindata) > 140)
			throw new Exception("WAP Push message length exceeds 140 bytes.");
		
		return bin2hex($bindata);
	}

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

	static function parse_inbound_sms($sms)
	{
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
					SMS::send_text_sms($sms->source->value, "Deine Anmeldung war erfolgreich! Willkommen bei Blamba!");
					return true;
				}
			}
		}

		// parse content-request SMS
		$matches = [];
		$re = '/^([0-9]{1,5})$/mi';
		preg_match_all($re, $sms->message, $matches, PREG_SET_ORDER, 0);
		foreach ($matches as $match)
		{
			if (count($match) == 2)
			{
				SMS::send_text_sms($sms->source->value, "Blamba kann leider gerade noch keinen Content per SMS versenden, sorry.");
				return true;
			}
		}

		SMS::send_text_sms($sms->source->value, "Deine SMS wurde leider nicht verstanden! Bitte versuche es erneut. Du musst nicht auf Gross- und Kleinschreibung achten.");
	}
}