<?php
require_once(SERVER_PATH . "/wap/lib.php");

$content_all = $db->prepared_fetch("SELECT * FROM content WHERE type = ? AND visible = 1 ORDER BY sent DESC, played DESC;", "s", $type);
$content_count = count($content_all);

$page = 1;
$entriesPerPage = 10;
$totalPages = (int) ceil($content_count / $entriesPerPage);

if (isset($_GET['p']))
{
    $page = (int)$_GET['p'];
}

$content = $db->prepared_fetch("SELECT * FROM content WHERE type = ? AND visible = 1 ORDER BY sent DESC, played DESC LIMIT ?,?;", "sii", $type, ($page - 1) * $entriesPerPage, $entriesPerPage);

?><?xml version="1.0"?>
<!DOCTYPE wml PUBLIC "-//WAPFORUM//DTD WML 1.1//EN" "http://www.wapforum.org/DTD/wml_1.1.xml">
<wml>
    <card title="<?php echo htmlspecialchars(Content::human_readable_type($type)); ?>">
        <p>
        <table columns="1" align="L">
            <?php
            foreach ($content as $item)
            {
                ?>
                <tr><td><a href="/<?php echo htmlspecialchars($type) . "/dl/" . Content::pad_content_id($item["id"]); ?>"><?php echo wmlescape($item["name"]); ?></a>&nbsp;</td><td></td></tr>
                <?php
            }
            ?>
        </table>
        <?php
        if ($totalPages > 1)
        {
            echo "Page " . wmlescape($page) . "/" . wmlescape($totalPages) . "<br/>\n";
            if ($page != 1)
            {
                echo '<a href="/'.htmlspecialchars($type).'/' . ($page - 1) . '/">Previous Page</a>';
            }
            if ($page != $phonebook["lastPage"])
            {
                echo '<a href="/'.htmlspecialchars($type).'/' . ($page + 1) . '/">Next Page</a>';
            }
        }
        ?> 
        </p>
    </card>
</wml>
