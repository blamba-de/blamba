<?php
require_once("../bootstrap.php");

import_content($content_path, "rtttl");
import_content($content_path, "polyphonic-ring");
import_content($content_path, "operator-logo");
import_content($content_path, "bitmap");

function import_content($path, $type)
{
	global $db;

	$validExtensions = [
		'rtttl'            => ['txt'],
		'polyphonic-ring'  => ['mid'],
		'operator-logo'    => ['gif', 'bmp', 'png'],
		'splash-logo'      => ['gif', 'bmp', 'png'],
		'amr'              => ['amr'],
		'j2me'             => ['jar'],
		'bitmap'           => ['gif', 'bmp', 'png'],
	];

	$rii = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path . "/" . $type . "/"));

	/** @var SplFileInfo $file */
	foreach ($rii as $file)
	{
		if ($file->isDir())
		{
			continue;
		}

		if (!in_array($file->getExtension(), $validExtensions[$type]))
		{
			continue;
		}

		$short_path = str_replace($path . "/", "", $file->getPathname());
		$filename = $file->getBasename("." . $file->getExtension());

		if ($db->prepared_fetch_one("SELECT * FROM `content` WHERE `path` = ? LIMIT 0,1", "s", $short_path) === NULL)
		{
			echo "new one!\n";
			$db->prepared_query("INSERT INTO `content`(`type`, `author`, `name`, `path`) VALUES (
				?,
				?,
				?,
				?
			)", "siss", $type, 2, $filename, $short_path);
		}
		else
		{
			echo "had that one already\n";
		}
	}
}