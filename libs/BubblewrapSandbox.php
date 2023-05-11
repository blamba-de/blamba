<?php
class BubblewrapSandbox
{
	static function run_sandboxed_process($binary_path, $seccomp_bpf_path, $infile, $command)
	{
		$tempdir = "/dev/shm/blamba/";
		if (!file_exists($tempdir))
		{
			mkdir($tempdir);
		}

		$workdir = $tempdir . uniqid("", true) . "/";
		mkdir($workdir);

		copy($binary_path, $workdir . "executable");
		chmod($workdir . "executable", 755);

		copy($infile, $workdir . "input");

		$cmd = "timeout 10 bwrap --seccomp 10 --new-session --bind " . escapeshellarg($workdir) . " / --unshare-all " . $command;

		$descriptorspec = array(
			0 => array("pipe", "r"),  // stdin
			1 => array("pipe", "w"),  // stdout
			2 => array("pipe", "w"),  // stderr
			10 => array("file", $seccomp_bpf_path, "r") // FD 10 is a BPF program containing seccomp rules limiting the allowed syscalls
		);

		$process = proc_open($cmd, $descriptorspec, $pipes, $workdir, []);

		$success = false;
		$output = false;
		$stdout = false;
		$stderr = false;

		if (is_resource($process))
		{
			$stdout = stream_get_contents($pipes[1]);
			fclose($pipes[1]);
			$stderr = stream_get_contents($pipes[2]);
			fclose($pipes[2]);

			proc_close($process);

			if (!is_link($workdir . "output") && is_file($workdir . "output"))
			{
				$success = true;
				$output = file_get_contents($workdir . "output");
			}
		}

		$cmd = "rm -r " . escapeshellarg($workdir);
		`$cmd`;

		return [
			"success" => $success,
			"output" => $output,
			"stdout" => $stdout,
			"stderr" => $stderr
		];
	}
}