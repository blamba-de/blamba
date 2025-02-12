<?php
require_once(SERVER_PATH . "/wap/lib.php");

$type = strtolower(trim(preg_replace('/[^0-9a-zA-Z\-]/', '', $_GET['type'])));
$id = (int)trim(preg_replace('/[^0-9]/', '', $_GET['dl']));

$content = $db->prepared_fetch_one("SELECT * FROM content WHERE `id` = ? AND `visible` = 1;", "i", $id);
if (is_null($content))
{
    http_response_code(404);
    echo "Page not found";
    exit;
}

$db->prepared_query("UPDATE content SET played = played + 1, sent = sent + 1 WHERE `id` = ? AND `visible` = 1;", "i", $content['id']);

switch ($content['type'])
{
    case 'rtttl':
        $rng = RTTTL::convert_to_nokia_rng($content_path . $content["path"]);
        header('Content-Type: application/vnd.nokia.ringing-tone');
        header('Content-Disposition: attachment; filename="'.basename($content["path"], ".txt").'.rng"');
        header('Content-Length: ' . strlen($rng));
        echo $rng;
        break;
    default:
        download_file($content_path . $content["path"], mime_content_type($content_path . $content["path"]), $type);
        break;
}

function download_file($file, $mime = false)
{
    if (file_exists($file))
    {
        header('Content-Description: File Transfer');
        if ($mime !== false)
        {
            header('Content-Type: ' . $mime);
        }
        else
        {
            header('Content-Type: application/octet-stream');
        }
        header('Content-Disposition: attachment; filename='.basename($file).'');
        header('Expires: 0');
        header('Cache-Control: must-revalidate');
        header('Pragma: public');
        header('Content-Length: ' . filesize($file));
        readfile($file);
        exit;
    }
}