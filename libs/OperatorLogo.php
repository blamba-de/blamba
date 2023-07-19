<?php
class OperatorLogo
{
	static function convert_image_to_temporary_gif(string $image_filename, string $additional_im_params = "")
	{
		global $config;
		$gif = ImageMagick::convert_image_to_gif($image_filename, $additional_im_params);
		if ($gif['success'])
		{
			$gifpath = tempnam($config['tempdir'], "gif");
			file_put_contents($gifpath, $gif['output']);

			return $gifpath;
		}
		return false;
	}

	static function convert_to_nokia_sms_group(string $logo_filename, string $additional_im_params = "-threshold 0 -negate")
	{
		// This call will result in a Group Logo:
		// WDP Port number 0x1583	Group Logo
		$gifpath = self::convert_image_to_temporary_gif($logo_filename, $additional_im_params);
		if (!$gifpath)
			return false;

		$sms = RingtoneTools::run_sandboxed_ringtonetools("gif", $gifpath, "nokia", "-u ");
		unlink($gifpath);

		if ($sms["success"])
		{
			return $sms["output"];
		}
		else
		{
			return $sms["stdout"] . $sms["stderr"];
		}
	}

	static function convert_to_nokia_sms_operator(string $logo_filename, int $mcc = 262, int $mnc = 42, string $additional_im_params = "-threshold 0 -negate")
	{
		// This call will result in an Operator Logo:
		// WDP Port number 0x1582	Operator Logo
		$gifpath = self::convert_image_to_temporary_gif($logo_filename, $additional_im_params);
		if (!$gifpath)
			return false;

		$sms = RingtoneTools::run_sandboxed_ringtonetools("gif", $gifpath, "nokia", "-u -l " . escapeshellarg((int) $mcc) . " " . escapeshellarg((int) $mnc));
		unlink($gifpath);

		if ($sms["success"])
		{
			return $sms["output"];
		}
		else
		{
			return $sms["stdout"] . $sms["stderr"];
		}
	}
}