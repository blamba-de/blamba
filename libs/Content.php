<?php
class Content
{
	static function is_valid_content_type($type)
	{
		return in_array($type, ['rtttl','polyphonic-ring','operator-logo','splash-logo','amr','j2me','bitmap']);
	}

	static function human_readable_type($type)
	{
		switch ($type)
		{
			case 'rtttl':
				return 'Monophone Klingeltöne (RTTTL)';
			case 'polyphonic-ring':
				return 'Polyphone Klingeltöne (MIDI)';
			case 'operator-logo':
				return 'Betreiberlogos';
			case 'splash-logo':
				return 'Splashlogos';
			case 'amr':
				return 'Real Klingeltöne (AMR)';
			case 'j2me':
				return 'Anwendungen/Spiele (J2ME)';
			case 'bitmap':
				return 'Farbige Logos';
			default:
				return $type;
		}
	}

	static function handle_sms_content_request($gateway, $sender, $regex_matches)
	{
		global $db, $content_path;

		if (count($regex_matches) != 3)
		{
			return false;
		}

		$plain = false;
		if (trim(strtolower($regex_matches[1])) == 'plain')
		{
			// User requested plain text SMS with the WAP-URL instead of a WAP Push SMS
			$plain = true;
		}

		$content_id = (int) $regex_matches[2];

		Logging::log("handle_sms_content_request", $content_id, $sender);

		$content_id = (int) $content_id;
		$content = $db->prepared_fetch_one("SELECT content.*,authors.name as `authorname` FROM content, authors WHERE content.id = ? AND author = authors.id AND content.visible = 1;", "i", $content_id);

		if ($content === null)
		{
			SMS::send_text_sms($gateway, $sender, "Der Inhalt " . $content_id . " wurde nicht gefunden. Versuche es bitte erneut.");
			return false;
		}

		$db->prepared_query("UPDATE content SET sent = sent + 1 WHERE `id` = ? AND `visible` = 1;", "i", $content_id);

		$sms = false;
		switch ($content["type"])
		{
			case 'rtttl':
				$sms = RTTTL::convert_to_nokia_sms($content_path . $content["path"]);
				break;
			case 'operator-logo':
				$sms = OperatorLogo::convert_to_nokia_sms_operator($content_path . $content["path"], 901, 70, "-monochrome -threshold 0 -negate");
				break;
			case 'bitmap':
			case 'polyphonic-ring':
				$wappush = WAPPush::generate_stored( $content["type"], $content["name"], basename($content["path"]), $sender, [
					"id" => $content['id'],
					"name" => $content["name"]
				]);
				if ($plain)
				{
					SMS::send_text_sms($gateway, $sender, $wappush["url"]);
					return true;
				}
				$sms = WAPPush::generate_wap_push($wappush["url"], $wappush["name"]);
				break;
			default:
				SMS::send_text_sms($gateway, $sender, "Der Inhalt " . $content_id . " kann leider nicht per SMS angefordert werden. Probier es ueber www.blamba.de.");
				return true;
				break;
		}

		SMS::send_udh_sms($gateway, $sender, $sms);
		return true;
	}
}
