<?php
class RingtoneTools
{
	static function run_sandboxed_ringtonetools($intype, $infile, $outtype, $extraargs = "")
	{
		$ringtonetools_binary_path = SERVER_PATH . "vendor-native/ringtonetools/ringtonetools";
		$seccomp_bpf_path = SERVER_PATH . "vendor-native/bwrap-seccomp/seccomp.bpf";

		return BubblewrapSandbox::run_sandboxed_process(
			$ringtonetools_binary_path, 
			$seccomp_bpf_path,
			$infile,
			"/executable " . $extraargs . " -intype " . escapeshellarg($intype) . " -outtype " . escapeshellarg($outtype) . " /input /output"
		);
	}
}