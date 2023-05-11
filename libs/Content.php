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
}