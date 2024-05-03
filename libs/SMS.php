<?php
class SMS
{
	static function send_text_sms($gateway, $destination, $data)
	{
		global $db;
		if ($gateway["type"] == "smpp")
		{
			$db->prepared_query("INSERT INTO `sms_queue` (`destination`, `type`, `data`, `queued_at`) VALUES (?, 'text', ?, current_timestamp());", "ss", $destination, $data);
			return ["success" => true, "body" => "queued for SMPP"];
		}
		if ($gateway["type"] == "kannel")
		{
			return Kannel::send_text_sms($gateway, $destination, $data);
		}
	}

	static function send_udh_sms($gateway, $destination, $data)
	{
		global $db;

		$return = [];
		foreach (explode("\n", $data) as $line)
		{
		 	$line = trim($line);
		 	if ($line != '')
		 	{
				if ($gateway["type"] == "smpp")
				{
					$db->prepared_query("INSERT INTO `sms_queue` (`destination`, `type`, `data`, `queued_at`) VALUES (?, 'udh', ?, current_timestamp());", "ss", $destination, $line);
					$return[] = ["success" => true, "body" => "queued for SMPP"];
				}
				if ($gateway["type"] == "kannel")
				{
					$return[] = Kannel::send_udh_sms($gateway, $destination, $line);
				}
		 	}
		}

		return $return;
	}

	static function generate_vcard_udh()
	{
		return self::generate_udh(9204, str_replace("\n", "\r\n", "BEGIN:VCARD
VERSION:2.1
N:Bloggs;Joe
TEL;HOME:123456789
TEL;CELL:123456789
EMAIL;TYPE=internet:me@mycompany.com
END:VCARD"));
	}

	static function generate_vcal_udh($events)
	{
		// Sending multiple VEVENTS in one VCAL file is theoretically supported
		// in practice, the implementations seem very buggy, especially old b/w 
		// phones seem to really dislike receiving multiple events. 
		$vevents = "";
		date_default_timezone_set("Europe/Berlin");
		foreach ($events as $event)
		{
			$start = strtotime($event["date"]);
			$end = strtotime("now +" . $event["duration"] . ":00", $start);
			$text = iconv("UTF-8", "ISO-8859-1//TRANSLIT", $event["title"]);

			$vevents .= "
BEGIN:VEVENT
UID:".$event["id"]."
DTSTART:".date("Ymd", $start) . "T" . date("His", $start) . "Z"."
DTEND:".date("Ymd", $end) . "T" . date("His", $end) . "Z"."
DALARM:".date("Ymd", $start) . "T" . date("His", $start) . "Z".";PT15M;2;Reminder
DESCRIPTION:" . $text . "
CLASS:PRIVATE
END:VEVENT
";
		}

		$vcal = "BEGIN:VCALENDAR
VERSION:1.0
". $vevents . "
END:VCALENDAR";
		return self::generate_udh(9205, str_replace("\n", "\r\n", $vcal));
	}

	static function generate_udh($destination_port, $data)
	{
		$sms = false;
		$max_bytes_per_message = 110;

		if (strlen($data) > $max_bytes_per_message)
		{
			$sms = "";
			$total_sms = ceil(strlen($data) / $max_bytes_per_message);

			for ($current_sms = 1; $current_sms <= $total_sms; $current_sms++)
			{ 
				// Long content, concatenated messages
				$header = [
					"\x0b", // UDH length
					"\x05", // Application Port Addressing, 16 bit address
					"\x04", // content length
					pack("n", $destination_port), // Destination port number
					"\x23\xF0", // Source port number
					"\x00", // Information Element Identifier: Concatenated short message, 8bit reference number
					"\x03", // Information Element Data Length (always 03 for this UDH)
					"\x01", // Information Element Data: Concatenated short message reference, should be same for all parts of a message
					chr($total_sms), // Information Element Data: Total number of parts
					chr($current_sms), // Information Element Data: Number of this part
				];

				$bindata = implode("", $header);
				$bindata .= substr($data, ($current_sms - 1) * $max_bytes_per_message, $max_bytes_per_message);
				$sms .= bin2hex($bindata) . "\n";
			}
		}
		else
		{
			$header = [
				"\x06", // UDH length
				"\x05", // Application Port Addressing, 16 bit address
				"\x04", // content length
				pack("n", $destination_port), // Destination port number
				"\x23\xF0", // Source port number
			];

			$bindata = implode("", $header);
			$bindata .= $data;
			$sms = bin2hex($bindata);
		}

		return $sms;
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
					$error .= "Too many messages (" . $smscount . ")\n";
				}

				if (@hex2bin($line) === false)
				{
					$error .= "Hex decoding of line " . ($linecount + 1) . " failed\n";
				}

				if (strlen(@hex2bin($line)) > 140)
				{
					$error .= "Line " . ($linecount + 1) . " is long than 140 bytes\n";
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

	static function check_if_sender_is_allowed($sms_from)
	{
		global $config;

		// Eventphone Number
		if (strlen($sms_from) == 4)
			return true;

		// C3GSM temporary numbers for unregistered devices
		if (strlen($sms_from) == 10 && $sms_from[0] != "+")
			return true;

		// explicitly allowed senders
		if (in_array($sms_from, $config["sms_sender_allowlist_override"]))
			return true;

		// German cellular networks (covered by flatrate billing)
		if (in_array(substr($sms_from, 0, 6), [
			"+49151",
			"+49155",
			"+49158",
			"+49160",
			"+49170",
			"+49171",
			"+49175",
			"+49152",
			"+49162",
			"+49172",
			"+49173",
			"+49174",
			"+49157",
			"+49159",
			"+49163",
			"+49176",
			"+49177",
			"+49178",
			"+49179",
		]))
			return true;

		return false;
	}

	static function parse_inbound_sms($gateway, $sms_message, $sms_from)
	{
		Logging::log("parse_inbound_sms", $sms_message, $sms_from);

		if (!self::check_if_sender_is_allowed($sms_from))
		{
			error_log("Received SMS with content " . $sms_message . " from unallowed sender " . $sms_from);
			Logging::log("unallowed sender, rejected", $sms_message, $sms_from);
			return false;
		}

		// parse registration SMS
        $matches = [];
		$re = '/reg.*?([0-9]{3,6})/mi';
		preg_match_all($re, $sms_message, $matches, PREG_SET_ORDER, 0);

		foreach ($matches as $match)
		{
			if (count($match) == 2)
			{
				if (Device::check_registration_token($gateway, $match[1], $sms_from))
				{
					Logging::log("check_registration_token", $sms_message, $sms_from);
					SMS::send_text_sms($gateway, $sms_from, "Your registration was successful! Welcome to Blamba!");
					return true;
				}
			}
		}

		// parse content-request SMS
		$matches = [];
		$re = '/^(plain\s*?)?([0-9]{1,5})$/mi';
		preg_match_all($re, $sms_message, $matches, PREG_SET_ORDER, 0);
		foreach ($matches as $match)
		{
			if (Content::handle_sms_content_request($gateway, $sms_from, $match))
			{
				return true;
			}
		}

		SMS::send_text_sms($gateway, $sms_from, "Your SMS could not be decoded! Please try again. The messages are _not_ case-sensitive.");
	}
}