<?php 
function wmlescape($text)
{
	// Phones can handle UTF-8, but will chug at Emojis and similar. 
	// Lets limit the charset to ISO, but deliver UTF-8.
	return htmlspecialchars(iconv("ISO-8859-1", "UTF-8", iconv("UTF-8", "ISO-8859-1//TRANSLIT", $text)));
}

function set_content_type($content_type)
{
	// *sigh* -- seems to be a PHP bug: https://bugs.php.net/bug.php?id=75922
	// workaround it by temporarily clearing the default_charset parameter
	$defaultCharset = ini_get('default_charset');
	ini_set('default_charset', '');
	header('Content-Type: ' . $content_type);
	ini_set('default_charset', $defaultCharset);
}