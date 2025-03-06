<?php
require_once("bootstrap.php");

// Request for a stored WAP Push content
$wappush = $db->prepared_fetch_one("SELECT * FROM wappush WHERE `uuid` = ?;", "s", $_GET['id']);
if (is_null($wappush))
{
    http_response_code(404);
    echo "Content could not be found!";
    exit;
}

Logging::log("storedwap_media", $_SERVER, $wappush["msn"]);

// We might've just learned which type of device this is...
// If we know the MSN for this download, let's save the useragent and UAProf URL
if (!empty($wappush["msn"]))
{
    $wapprofile = false;
    // WAP 2.0 (direct TCP/IP access via HTTP)
    if (!empty($_SERVER["HTTP_X_WAP_PROFILE"] ?? ""))
    {
        $wapprofile = trim($_SERVER["HTTP_X_WAP_PROFILE"] ?? "", '"');
    }
    
    // WAP 1.1 (via WAP gateway/Kannel)
    if (!empty($_SERVER["HTTP_PROFILE"] ?? ""))
    {
        $wapprofile = trim($_SERVER["HTTP_PROFILE"] ?? "", '"');
    }

    if ($wapprofile)
    {
        $useragent = $_SERVER["HTTP_USER_AGENT"] ?? "";
        $accept = $_SERVER["HTTP_ACCEPT"] ?? "";

        $phonetype = explode(" ", $useragent)[0]; // Nokia6220/2.0 Profile/MIDP-1.0 Configuration/CLDC-1.0 -> Nokia6220
        $db->prepared_query("UPDATE `devices` SET `type` = ?, `useragent` = ?, `wap-profile` = ?, `accept` = ? WHERE `msn` = ?;", "sssss", $phonetype, $useragent, $wapprofile, $accept, $wappush["msn"]);
    }
}

$db->prepared_query("UPDATE wappush SET download_count = download_count + 1 WHERE `uuid` = ?;", "s", $_GET['id']);
$wappush_params = json_decode($wappush["params"], true);

// User-defined custom file upload
if (($wappush_params["mode"] ?? '') == "custom" )
{
    download_file($userupload_path . "/" . $wappush["uuid"]);
}
else
{
    // Regular content
    $id = $wappush_params["id"];
    $content = $db->prepared_fetch_one("SELECT * FROM content WHERE `id` = ? AND `visible` = 1;", "i", $id);
    if (is_null($content))
    {
        http_response_code(404);
        echo "Content could not be found!";
        exit;
    }
    $db->prepared_query("UPDATE content SET played = played + 1 WHERE `id` = ? AND `visible` = 1;", "i", $content['id']);
    download_file($content_path . $content["path"]);
}


function download_file($file)
{
    if (file_exists($file))
    {   
        $mime_type = mime_content_type($file);
        // if has a .jar extension, we need to force it to "application/java-archive" to make it work on some devices
        if (pathinfo($file, PATHINFO_EXTENSION) == "jar" || $mime_type == "application/zip") {
            $mime_type = "application/java-archive";
        }
       
        header('Content-Type: ' . $mime_type);
        header('Content-Length: ' . filesize($file));
        readfile($file);
        exit;
    }
}