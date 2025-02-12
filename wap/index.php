<?php
require_once("../bootstrap.php");
require_once("lib.php");

set_content_type("text/vnd.wap.wml");
header('Pragma: no-cache');
session_cache_limiter('private');

$page = strtolower(trim(preg_replace('/[^0-9a-zA-Z\-]/', '', $_GET['page'] ?? 'index')));

// special handling for each page with the name of a content type
if (Content::is_valid_content_type($page))
{
    $type = $page;
    $page = "content";
}

$php = SERVER_PATH . "/wap/pages/" . $page . ".php";
if (file_exists($php))
{
    require_once($php);
}
else
{
    http_response_code(404);
    echo 'Page not found!';
}