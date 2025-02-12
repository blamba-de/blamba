<?php

function fetch_eventphone($page = 1, $search = "")
{
	$url = "https://guru3.eventphone.de/event.lsp/phonebook?order=extension&q=" . urlencode($search) . "&page=" . urlencode($page);

	$options  = array('http' => array('user_agent' => 'Blamba.de WAP Phonebook (talk to Manawyrm/6502)'));
	$context  = stream_context_create($options);

	$html = file_get_contents($url, false, $context);

	// Parse the phone book entries themselves
	$doc = new DOMDocument();
	$doc->loadHTML($html);
	$phonebook = $doc->getElementById("phonebook");
	$phonebook_entries = [];

	foreach ($phonebook->childNodes as $child)
	{
		if ($child->tagName == "tbody")
		{
			foreach ($child->childNodes as $entry)
			{
				$columns = [];
				foreach ($entry->childNodes as $td)
				{
					$columns[] = trim($td->textContent);
				}
				
				if (isset($columns[1]) && !empty($columns[1]))
				{
					$phonebook_entries[] = [
						"extension" => $columns[1],
						"name" => $columns[3],
						"type" => $columns[5],
						"location" => $columns[7],
					];
				}
				
			}
		}
	}

	$extensionsFound = "";
	preg_match_all('/<p class="text-info">([\s\S]*?)<\/p>/m', $html, $matches, PREG_SET_ORDER, 0);
	if (isset($matches[0]) && isset($matches[0][1]))
	{
		$extensionsFound = trim($matches[0][1]);
	}

	$lastPage = "";
	preg_match_all('/<a class="page-link" href="\?order=extension&q=.*?&page=([0-9]*)" >Last.*?<\/a>/m', $html, $matches, PREG_SET_ORDER, 0);
	if (isset($matches[0]) && isset($matches[0][1]))
	{
		$lastPage = trim($matches[0][1]);
	}

	return [
		"phonebook" => $phonebook_entries,
		"extensionsFound" => $extensionsFound,
		"lastPage" => $lastPage,
	];
}

function wmlescape($text)
{
	// Phones can handle UTF-8, but will chug at Emojis and similar. 
	// Lets limit the charset to ISO, but deliver UTF-8.
	return htmlspecialchars(iconv("ISO-8859-1", "UTF-8", iconv("UTF-8", "ISO-8859-1//TRANSLIT", $text)));
}

$page = 1;
$search = "";

if (isset($_GET['p']))
{
	$page = (int)$_GET['p'];
}
if (isset($_GET['q']))
{
	$search = $_GET['q'];
}

$phonebook = fetch_eventphone($page, $search);

header("Content-Type: text/vnd.wap.wml");
header('Pragma: no-cache');
session_cache_limiter('private');

?><?xml version="1.0"?>
<!DOCTYPE wml PUBLIC "-//WAPFORUM//DTD WML 1.1//EN" "http://www.wapforum.org/DTD/wml_1.1.xml">
<wml>
	<card title="B! Phonebook">
		<p>
		Search: <input name="q" size="20" value="<?php echo wmlescape($search); ?>"/>
		<a>
			<go method="get" href="/ep/">
				<postfield name="q" value="$(q)"/>
			</go>
			Go
		</a>
		<br/>
		<?php echo wmlescape($phonebook["extensionsFound"]); ?>
		<table columns="2" align="LL">
			<?php
			foreach ($phonebook["phonebook"] as $entry)
			{
				echo "<tr><td><a href=\"wtai://wp/mc;".wmlescape($entry["extension"])."\">". wmlescape($entry["extension"]) ."</a>&nbsp;</td><td>". wmlescape($entry["name"]) ."</td></tr>\n";
			}
			?>
		</table>

		<?php
		if ($phonebook["lastPage"])
		{
			echo "Page " . wmlescape($page) . "/" . wmlescape($phonebook["lastPage"]) . "<br/>\n";
			if ($page != 1)
			{
				echo '<a href="/ep/?p=' . ($page - 1) . '&amp;q='. urlencode($search) .'">Previous Page</a>';
			}
			if ($page != $phonebook["lastPage"])
			{
				echo '<a href="/ep/?p=' . ($page + 1) . '&amp;q='. urlencode($search) .'">Next Page</a>';
			}
		}
		?> 
		</p>
	</card>
</wml>
