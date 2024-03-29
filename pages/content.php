<?php
$content = $db->prepared_fetch("SELECT * FROM content WHERE type = ? AND visible = 1 ORDER BY sent DESC, played DESC;", "s", $type);

echo $twig->render('pages/' . $page . '.html',
[
	'type' => $type,
	'type_human' => Content::human_readable_type($type),
	'content' => $content
]);