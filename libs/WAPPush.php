<?php
class WAPPush
{
	static function generate_wap_push($url, $text)
	{
		Logging::log("generate_wap_push", ["url" => $url, "text" => $text]);

		$data = [
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

		// IANA WAP-Push Multimedia Messaging Service port
		return SMS::generate_udh(2948, implode("", $data));
	}

	static function generate_wap_push_service_load($url, $text)
	{
		// NOTE: This function doens't seem to work well with most phones I've tested.
		Logging::log("generate_wap_push", ["url" => $url, "text" => $text]);
	
		// https://www.openmobilealliance.org/release/Push/V2_1-20051122-C/WAP-168-ServiceLoad-20010731-a.pdf
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
			"\xB0", // Content type (application/vnd.wap.slc)
			"\x81", // Character Set (01 once removed high bit)
			"\xEA", // UTF-8 (6A once removed high bit)
			"\x02", // WBXML Version 1.2 
			"\x06", // SL Identifier
			"\x6A", // UTF-8
			"\x00", // End Data
			"\x85", // SL WBXML Tag
			"\x08", // href
			"\x03", // Open Text 
			$url,   // URL
			"\x00", // End Data
			/*"\x0A", // Created (date)
			"\xC3", // Data Follows
			"\x07", // Data Length
			hex2bin(date("YmdHis") ), // Date
			"\x01", // Close Attribute
			"\x01", // Close Indication Tag*/
			//"\x05", // Action Attribute ( 0x05 = execute-low )
			"\x01", // Close SL Tag
		];

		$bindata = implode("", $data);
		if (strlen($bindata) > 140)
			throw new Exception("WAP Push message length exceeds 140 bytes.");
		
		return bin2hex($bindata);
	}


	static function generate_stored( $type, $name, $filename, $msn, $data )
	{
		global $db;
		$uuid = self::generate_wap_push_uuid();

		$data = json_encode($data, JSON_PRETTY_PRINT);
		$db->prepared_query("UPDATE `wappush` SET `msn` = ?, `type` = ?, `filename` = ?, `params` = ? WHERE `uuid` = ?;", "sssss", $msn, $type, $filename, $data, $uuid);

		return [
				"uuid" => $uuid,
				"url" => "http://wap.blamba.de/wp/" . $uuid . "/" . $filename,
				"name" => $name
		];
	}

	static function generate_wap_push_uuid()
	{
		global $db;
		
		while (true)
		{
			$uuid = self::base64url_encode(random_bytes(16));
			$result = $db->prepared_query("INSERT INTO `wappush` (`uuid`, `created`) VALUES (?, NOW());", "s", $uuid);

			if ($result === true)
			{
				break;
			}
		}
		
		return $uuid;
	}

	static function base64url_encode( $data )
	{
		return rtrim( strtr( base64_encode( $data ), '+/', '-_'), '=');
	}
}