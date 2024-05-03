<?php

$data = "";
$success = false;

$registered_devices = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`devices`", "sum");

$received_sms = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`log` WHERE `type` = 'parse_inbound_sms' ", "sum");
$sent_wappush = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`wappush` ", "sum");

$sent_ringtones = $db->fetch("SELECT SUM(sent) as `sum` FROM `blamba`.`content` WHERE `type` = 'polyphonic-ring' OR `type` = 'rtttl'", "sum");
$sent_logos = $db->fetch("SELECT SUM(sent) as `sum` FROM `blamba`.`content` WHERE `type` = 'bitmap' OR `type` = 'operator-logo'", "sum");
$sent_java = $db->fetch("SELECT SUM(sent) as `sum` FROM `blamba`.`content` WHERE `type` = 'j2me'", "sum");

$available_ringtones = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`content` WHERE `type` = 'polyphonic-ring' OR `type` = 'rtttl'", "sum");
$available_logos = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`content` WHERE `type` = 'bitmap' OR `type` = 'operator-logo'", "sum");

$sent_text_sms = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`log` WHERE `type` = 'send_text_sms' ", "sum");
$sent_udh_sms = $db->fetch("SELECT COUNT(*) as `sum` FROM `blamba`.`log` WHERE `type` = 'send_udh_sms' ", "sum");

?>
# HELP blamba_registered_devices Registered devices.
# TYPE blamba_registered_devices gauge
blamba_registered_devices <?php echo $registered_devices; ?>

# HELP blamba_sent_wappush Sent WAP Push messages.
# TYPE blamba_sent_wappush gauge
blamba_sent_wappush <?php echo $sent_wappush; ?>

# HELP blamba_sent_ringtones Sent ringtones.
# TYPE blamba_sent_ringtones gauge
blamba_sent_ringtones <?php echo $sent_ringtones; ?>

# HELP blamba_sent_logos Sent logos.
# TYPE blamba_sent_logos gauge
blamba_sent_logos <?php echo $sent_logos; ?>

# HELP blamba_sent_java Sent Java applications.
# TYPE blamba_sent_java gauge
blamba_sent_java <?php echo $sent_java; ?>

# HELP blamba_available_ringtones Available ringtones.
# TYPE blamba_available_ringtones gauge
blamba_available_ringtones <?php echo $available_ringtones; ?>

# HELP blamba_available_logos Available logos.
# TYPE blamba_available_logos gauge
blamba_available_logos <?php echo $available_logos; ?>

# HELP blamba_received_sms Received SMS.
# TYPE blamba_received_sms gauge
blamba_received_sms <?php echo $received_sms; ?>

# HELP blamba_sent_text_sms Sent Text SMS.
# TYPE blamba_sent_text_sms gauge
blamba_sent_text_sms <?php echo $sent_text_sms; ?>

# HELP blamba_sent_udh_sms Sent UDH SMS.
# TYPE blamba_sent_udh_sms gauge
blamba_sent_udh_sms <?php echo $sent_udh_sms; ?>
