<?php
class ImageMagick
{
	static function convert_image_to_gif($infile, $additional_im_params = "")
	{
		global $config;
		$seccomp_bpf_path = SERVER_PATH . "vendor-native/bwrap-seccomp/seccomp-imagemagick.bpf";
		return BubblewrapSandbox::run_sandboxed_fakeroot($config["imagemagick_root"], $seccomp_bpf_path, $infile, "/usr/bin/convert /mnt/input " .  $additional_im_params . " gif:/mnt/output");
	}
}