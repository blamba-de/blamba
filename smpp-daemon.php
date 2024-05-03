<?php
require_once("bootstrap.php");
use smpp\{ Address, Smpp, Client as SmppClient, transport\Socket};

$gateway = $db->prepared_fetch_one("SELECT * FROM gateways WHERE id = ?;", "i", 6);

$timeout = 1000;
$debug = true;

Socket::$defaultDebug = true;

$transport = new Socket([$gateway["smpp_hostname"]], (int) $gateway["smpp_port"]);
$transport->setRecvTimeout($timeout);

// Hack to make protected function submitShortMessage() callable
$smppClient = new class($transport) extends SmppClient {
    public function submitShortMessage(
        Address $source,
        Address $destination,
        string $shortMessage = null,
        array $tags = null,
        int $dataCoding = Smpp::DATA_CODING_DEFAULT,
        int $priority = 0x00,
        string $scheduleDeliveryTime = null,
        string $validityPeriod = null,
        string $esmClass = null
    ): string {
      return parent::submitShortMessage($source,
                $destination,
                $shortMessage,
                $tags,
                $dataCoding,
                $priority,
                $scheduleDeliveryTime,
                $validityPeriod,
                $esmClass);
    }
};

$from = new Address($gateway["sender_name"],SMPP::TON_ALPHANUMERIC);

$transport->open();
$smppClient->bindTransceiver($gateway["smpp_username"], $gateway["smpp_password"] ?? '');

while ($transport->isOpen())
{
    // receiving messages
    $sms = $smppClient->readSMS();
    if ($sms)
    {
	var_dump($sms);
        SMS::parse_inbound_sms($gateway, $sms->message, $sms->source->value);
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

            $res = $smppClient->submitShortMessage(
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
