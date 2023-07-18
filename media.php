<?php
require_once("bootstrap.php");

$type = strtolower(trim(preg_replace('/[^0-9a-zA-Z\-]/', '', $_GET['type'])));
$id = (int)trim(preg_replace('/[^0-9]/', '', $_GET['id']));

$content = $db->prepared_fetch_one("SELECT * FROM content WHERE `id` = ? AND `visible` = 1;", "i", $id);
if (is_null($content))
{
    http_response_code(404);
    echo $twig->render('pages/404.html', []);
    exit;
}

if ($content['type'] == "rtttl" || $content['type'] == "polyphonic-ring")
    $db->prepared_query("UPDATE content SET played = played + 1 WHERE `id` = ? AND `visible` = 1;", "i", $content['id']);

switch ($content['type'])
{
    case 'rtttl':
        if ($type == "preview" || $type == "wap")
        {
            header('Content-Type: audio/midi');
            echo RTTTL::convert_to_midi($content_path . $content["path"]);
            exit;
        }
        break;
    default:
        download_file($content_path . $content["path"], false, $type);
        break;
}

function download_file($file, $mime = false, $type = false)
{
    if (file_exists($file))
    {
        if ($type == "wap")
        {
            header('Content-Type: ' . mime_content_type($file));
        }
        else
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
            header('Content-Disposition: attachment; filename="'.basename($file).'"');
            header('Expires: 0');
            header('Cache-Control: must-revalidate');
            header('Pragma: public');
        }
        header('Content-Length: ' . filesize($file));
        readfile($file);
        exit;
    }
}