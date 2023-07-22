<?php
header("Content-Type: text/plain");
passthru("echo -n " . escapeshellarg(bin2hex(hex2bin($_GET["octets"]))) . " | xxd -r -p | wbxml2xml -m 1 -i 4 -o - -");