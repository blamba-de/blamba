<?php
require_once("bootstrap.php");
Session::current()->init();

$page = strtolower(trim(preg_replace('/[^0-9a-zA-Z\-]/', '', $_GET['page'] ?? 'index')));

// special handling for each page with the name of a content type
if (Content::is_valid_content_type($page))
{
    $type = $page;
    $page = "content";
}

$php = SERVER_PATH . "/pages/" . $page . ".php";
if (file_exists($php))
{
    require_once($php);
}
else
{
    http_response_code(404);
    echo $twig->render('pages/404.html', []);
}