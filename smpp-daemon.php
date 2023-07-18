<?php
require_once("bootstrap.php");
use smpp\{ Address, SMPP, Client as SmppClient, transport\Socket};

$timeout = 1000;
$debug = true;

$transport = new Socket([$config["smpp_hostname"]], $config["smpp_port"]);
$transport->setRecvTimeout($timeout);

// Hack to make protected function submit_sm() callable
$smppClient = new class($transport) extends SmppClient {
    public function submit_sm(
                $source,
                $destination,
                $short_message = null,
                $tags = null,
                $dataCoding = SMPP::DATA_CODING_DEFAULT,
                $priority = 0x00,
                $scheduleDeliveryTime = null,
                $validityPeriod = null,
                $esmClass = null
    ) {
      return parent::submit_sm($source,
                $destination,
                $short_message,
                $tags,
                $dataCoding,
                $priority,
                $scheduleDeliveryTime,
                $validityPeriod,
                $esmClass);
    }
};

// Activate binary hex-output of server interaction
$smppClient->debug = $debug;
$transport->debug = $debug;

$from = new Address($config["smpp_sender_alpha"],SMPP::TON_ALPHANUMERIC);

$transport->open();
$smppClient->bindTransceiver($config["smpp_username"], $config["smpp_password"]);

while ($transport->isOpen())
{
    // receiving messages
    $sms = $smppClient->readSMS();
    if ($sms)
    {
        SMS::parse_inbound_sms($sms);
    }

    // sending messages
    while (($outgoing_sms = $db->fetch("SELECT * FROM `sms_queue` WHERE sent_at IS NULL AND error_at IS NULL ORDER BY queued_at ASC LIMIT 0,1", true)) != null)
    {
        $to = new Address($outgoing_sms['destination'], \smpp\SMPP::TON_INTERNATIONAL, SMPP::NPI_E164);
        if ($outgoing_sms['type'] == "text")
        {
            $smppClient->sendSMS($from, $to, $outgoing_sms['data'], null, SMPP::DATA_CODING_DEFAULT);
        }
        if ($outgoing_sms['type'] == "udh")
        {
            $udh_binary = hex2bin($outgoing_sms['data']);
            if ($udh_binary === false)
            {
                $db->prepared_query("UPDATE `sms_queue` SET `error_at` = NOW(), `error` = 'invalid hex data' WHERE `id` = ?;", "i", $outgoing_sms['id']);
                continue;
            }

            $res = $smppClient->submit_sm(
                $from,
                $to,
                $udh_binary,
                null,
                SMPP::DATA_CODING_BINARY,
                0x00,
                null,
                null,
                0x40
            );
        }
        
        $db->prepared_query("UPDATE `sms_queue` SET `sent_at` = NOW() WHERE `id` = ?;", "i", $outgoing_sms['id']);
    }
}

$smppClient->close();
