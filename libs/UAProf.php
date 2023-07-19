<?php
class UAProf
{
	// Nokia User Agent profiles are .xml files describing the capabilities of a phone.
	// They include info about supported file types, amount of storage space and the screen parameters.
	//
	// https://web.archive.org/web/20070716225823/http://nds.nokia.com/uaprof/uaprof_list.txt
	// https://web.archive.org/web/20040107110023/http://nds1.nds.nokia.com/uaprof/N6220r200.xml
	static function try_load($url)
	{
		global $db;
		$phonename = basename($url);

		$uaprof = $db->prepared_fetch_one("SELECT * FROM uaprof WHERE uaprof_url = ? LIMIT 0,1;", "s", $url);

		if ($uaprof != null)
		{
			error_log($url . " already loaded");
		}
		else
		{
			self::load_from_webarchive($phonename, $url);
		}
	}

	static function load_from_webarchive($basename, $url)
	{
		global $db;
		
		error_log($url);

		$fetched = true;

		// id_ suffix will force a raw file download (without the JavaScript/HTML web.archive.org header)
		$webarchive_url = "https://web.archive.org/web/20001101000000id_/" . $url;
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $webarchive_url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
		curl_setopt($ch, CURLOPT_USERAGENT, "Blamba! retro phone service - Contact blamba@tbspace.de");

		$output = curl_exec($ch);

		if(curl_errno($ch))
		{
			$fetched = false;
			$output = curl_error($ch);
		}

		$http_code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
		if ($http_code != 200)
		{
			$fetched = false;
			$output = '';
		}

		curl_close($ch);

		$db->prepared_query("INSERT INTO `uaprof` (`uaprof_url`, `fetched`, `last_return`, `last_tried`, `uaprof_xml`) VALUES (?, ?, ?, NOW(), ?);", "siis", $url, $fetched, $http_code, $output);
	}
}