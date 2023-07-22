<?php
class ImageMagick
{
	static function convert_image_to_gif($infile, $additional_im_params = "")
	{
		global $config;
		$seccomp_bpf_path = SERVER_PATH . "vendor-native/bwrap-seccomp/seccomp-imagemagick.bpf";
		$sandboxResult = BubblewrapSandbox::run_sandboxed_fakeroot($config["imagemagick_root"], $seccomp_bpf_path, $infile, "/usr/bin/convert-im6.q16 /mnt/input " .  $additional_im_params . " gif:/mnt/output");

		Logging::log("convert_image_to_gif", ['infile' => $infile, 'additional_im_params' => $additional_im_params, 'sandboxResult' => $sandboxResult]);

		return $sandboxResult;
	}
}