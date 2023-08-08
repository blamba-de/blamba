<?php

$schedule_json_path = $config["tempdir"] . "/schedule.json";
clearstatcache(true); // fuck PHP!
if (filemtime($schedule_json_path) < time() - (24 * 60 * 60))
{
	copy("https://pretalx.c3voc.de/camp2023/schedule/export/schedule.json", $schedule_json_path . ".new");
	$fahrplan = json_decode(file_get_contents($schedule_json_path . ".new"), true);
	if ($fahrplan)
	{
		rename($schedule_json_path . ".new", $schedule_json_path);
	}
}

$fahrplan_json = file_get_contents($config["tempdir"] . "/schedule.json");
$fahrplan = json_decode($fahrplan_json, true);

$events = [];
$weekdays = ["So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"];

foreach ($fahrplan["schedule"]["conference"]["days"] as $day => $schedule_day)
{
	foreach ($schedule_day["rooms"] as $room => $roomEvents)
	{
		foreach ($roomEvents as $eventKey => $event)
		{
			$event["weekday"] = $weekdays[date("w", strtotime($event["date"]))];
			$events[] = $event;
		}
		
	}
}

$data = "";
$success = false;
if (isset($_POST["msn"]))
{
	$device = $db->prepared_fetch_one("SELECT * FROM devices WHERE user = ? AND msn = ?;", "ss", session_id(), (int)$_POST['msn']);
	if (is_null($device))
	{
		echo $twig->render('pages/404.html', []); exit();
	}

	$count = 0;
	foreach ($events as $event)
	{
		if (isset($_POST["event" . $event["id"]]))
		{
			$count++;
			if ($count > 11)
			{
				break;
			}
			
			// We're only sending a single Event per SMS, 
			// sending multiple breaks old Nokias (like the 6310i)
			$sms = SMS::generate_vcal_udh([$event]);
			SMS::send_udh_sms($device["msn"], $sms);

			$data .= $sms . "\n\n";
			$success = "Die Fahrplaneinträge wurden an dein Handy geschickt!";
		}
	}
}

$devices = $db->prepared_fetch("SELECT * FROM devices WHERE user = ?;", "s", session_id());

echo $twig->render('pages/' . $page . '.html',
[
	'success' => $success,
	'data' => $data,
	'smsarray' => SMS::split_sms_string($data),
	'events' => $events,
	'devices' => $devices,
	'registration_token' => $_SESSION['registration_token'],
	'config' => $config,
]);