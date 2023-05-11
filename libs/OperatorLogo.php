<?php
class OperatorLogo
{
	static function test()
	{
		global $config;
		$seccomp_bpf_path = SERVER_PATH . "vendor-native/bwrap-seccomp/seccomp-imagemagick.bpf";
		return BubblewrapSandbox::run_sandboxed_fakeroot($config["imagemagick_root"], $seccomp_bpf_path, "/var/www/blamba/htdocs/public/blamba-content/operator-logo/random/logo596.gif", "/usr/bin/convert /mnt/input bmp:/mnt/output");
	}

	static function convert_to_nokia_sms_group(string $logo_filename)
	{
		// This call will result in a Group Logo:
		// WDP Port number 0x1583	Group Logo
		$sms = RingtoneTools::run_sandboxed_ringtonetools("gif", $logo_filename, "nokia");

		if ($sms["success"])
		{
			return $sms["output"];
		}
		else
		{
			return $sms["stdout"] . $sms["stderr"];
		}
	}

	static function convert_to_nokia_sms_operator(string $logo_filename, int $mcc = 262, int $mnc = 42)
	{
		// This call will result in an Operator Logo:
		// WDP Port number 0x1582	Operator Logo
		$sms = RingtoneTools::run_sandboxed_ringtonetools("gif", $logo_filename, "nokia", "-l " . escapeshellarg((int) $mcc) . " " . escapeshellarg((int) $mnc));

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