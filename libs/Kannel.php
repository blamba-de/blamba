<?php
class Kannel
{
	static function send_text_sms($gateway, $destination, $data)
	{
		global $config;
		return self::send_kannel_smsbox_request($gateway, [
			"to" => $destination,
			"text" => $data,
		]);
	}

	static function send_udh_sms($gateway, $destination, $data)
	{
		global $config;
		
		$data = hex2bin($data);

		$udhHeaderLength = ord($data[0]) + 1;
		$udhHeader = substr($data, 0, $udhHeaderLength);
		$content = substr($data, $udhHeaderLength);
		
		return self::send_kannel_smsbox_request($gateway, [
			"to" => $destination,
			"udh" => $udhHeader,
			"text" => $content,
		]);
	}

	static function send_kannel_smsbox_request($gateway, $data)
	{
		global $config;

		$data["username"] = $gateway["kannel_username"];
		$data["password"] = $gateway["kannel_password"];
		$data["from"] = $gateway["sender_msn"];

		$ch = curl_init();

		curl_setopt($ch, CURLOPT_URL, $gateway["kannel_url"] . "/cgi-bin/sendsms?" . http_build_query($data));
		curl_setopt($ch, CURLOPT_HEADER, false);
		curl_setopt($ch, CURLOPT_TIMEOUT, 10);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_FOLLOWLOCATION, false);

		$body = curl_exec($ch);
		$status = curl_getinfo($ch, CURLINFO_HTTP_CODE);

		if (curl_errno($ch) !== 0)
		{
			return [
				"success" => false,
				"body" => curl_error($ch)
	 		];
		}
		
		return [
			"success" => true,
			"status" => $status,
			"body" => $body
 		];
	}
}
