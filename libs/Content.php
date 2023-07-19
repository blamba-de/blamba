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

	static function handle_sms_content_request($sender, $content_id)
	{
		global $db, $content_path;

		Logging::log("handle_sms_content_request", $content_id, $sender);

		$content_id = (int) $content_id;
		$content = $db->prepared_fetch_one("SELECT content.*,authors.name as `authorname` FROM content, authors WHERE content.id = ? AND author = authors.id AND content.visible = 1;", "i", $content_id);

		if ($content === null)
		{
			SMS::send_text_sms($sender, "Der Inhalt " . $content_id . " wurde nicht gefunden. Versuche es bitte erneut.");
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
				$sms = OperatorLogo::convert_to_nokia_sms_operator($content_path . $content["path"], 262, 42, "-monochrome -threshold 0 -negate");
				break;
			case 'bitmap':
			case 'polyphonic-ring':
				$sms = WAPPush::generate_stored( $content["type"], $content["name"], basename($content["path"]), $sender, [
					"id" => $content['id'],
					"name" => $content["name"]
				]);
				break;
			default:
				SMS::send_text_sms($sender, "Der Inhalt " . $content_id . " kann leider nicht per SMS angefordert werden. Probier es ueber www.blamba.de.");
				return false;
				break;
		}

		SMS::send_udh_sms($sender, $sms);
	}
}