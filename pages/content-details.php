<?php
$id = (int)trim(preg_replace('/[^0-9]/', '', $_GET['id']));
$content = $db->prepared_fetch_one("SELECT content.*,authors.name as `authorname` FROM content, authors WHERE content.id = ? AND author = authors.id AND content.visible = 1;", "i", $id);

if (is_null($content))
{
	echo $twig->render('pages/404.html',[]);
	exit();
}

if ($content["type"] == "rtttl")
{
	$rtttldata = file_get_contents($content_path . $content["path"]);
}

echo $twig->render('pages/' . $page . '.html',
[
	'type' => $content["type"],
	'type_human' => Content::human_readable_type($content["type"]),
	'content' => $content,
	'file_basename' => basename($content['path']),
	'file_size' => filesize($content_path . $content['path']),
	'rtttldata' => $rtttldata,
]);