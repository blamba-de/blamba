<?php
class BubblewrapSandbox
{
	static function run_sandboxed_process($binary_path, $seccomp_bpf_path, $infile, $command)
	{
		$workdir = self::create_workdir();

		copy($binary_path, $workdir . "executable");
		chmod($workdir . "executable", 755);

		copy($infile, $workdir . "input");

		$cmd = "timeout 10 bwrap --die-with-parent --seccomp 10 --new-session --bind " . escapeshellarg($workdir) . " / --unshare-all " . $command;

		return self::run_sandbox($seccomp_bpf_path, $cmd, $workdir);
	}

	static function run_sandboxed_fakeroot($fakeroot_dir, $seccomp_bpf_path, $infile, $command)
	{
		$workdir = self::create_workdir();

		copy($infile, $workdir . "input");

		$libs = "--ro-bind " . escapeshellarg($fakeroot_dir . "/lib64/") . " /lib64 ";
		// ImageMagick modules (.so files)
		$libs .= "--ro-bind " . escapeshellarg($fakeroot_dir . "/usr/lib/x86_64-linux-gnu/ImageMagick-6.9.11") . " /usr/lib/x86_64-linux-gnu/ImageMagick-6.9.11 ";

		if (php_uname("m") == "aarch64")
		{
			// ImageMagick modules (.so files)
			$libs = "--ro-bind " . escapeshellarg($fakeroot_dir . "/usr/lib/aarch64-linux-gnu/ImageMagick-6.9.11") . " /usr/lib/aarch64-linux-gnu/ImageMagick-6.9.11 ";
		}

		$cmd = "timeout 10 bwrap --die-with-parent --seccomp 10 --new-session " . 
				// Libraries (libpng, etc.)
				"--ro-bind " . escapeshellarg($fakeroot_dir . "/lib/") . " /lib " .
				// ImageMagick config files (delegates.xml, etc.)
				"--ro-bind " . escapeshellarg($fakeroot_dir . "/etc/ImageMagick-6/") . " /etc/ImageMagick-6 " .
				// Symlink from /usr/bin/convert to /etc/alternatives
				"--ro-bind " . escapeshellarg($fakeroot_dir . "/etc/alternatives/") . " /etc/alternatives " .
				// actual ImageMagick binary
				"--ro-bind " . escapeshellarg($fakeroot_dir . "/usr/bin/convert-im6.q16") . " /usr/bin/convert-im6.q16 " .
				$libs .
				"--bind " . escapeshellarg($workdir) . " /mnt --unshare-all " . $command;

		return self::run_sandbox($seccomp_bpf_path, $cmd, $workdir);
	}

	static function run_sandbox($seccomp_bpf_path, $cmd, $workdir)
	{
		$descriptorspec = array(
			0 => array("pipe", "r"),  // stdin
			1 => array("pipe", "w"),  // stdout
			2 => array("pipe", "w"),  // stderr
			10 => array("file", $seccomp_bpf_path, "r") // FD 10 is a BPF program containing seccomp rules limiting the allowed syscalls
		);

		//var_dump($cmd);

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

	static function create_workdir()
	{
		global $config;
		$tempdir = $config['tempdir'];
		if (!file_exists($tempdir))
		{
			mkdir($tempdir);
		}

		$workdir = $tempdir . uniqid("", true) . "/";
		mkdir($workdir);
		return $workdir;
	}

}