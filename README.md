# blamba


## How to DIY host (note: level expert)

> You need Kannel setup to report SMS messages to `kannel-sms.php?token=TOKEN`. These instructions are out of scope.

1) Install the LAMP stack on a Debian 12 box (tip: do not forget php-curl!).
2) Clone this repo in your `/var/www/html` or similar directory
3) Install [PHP Composer](https://getcomposer.org/download/) and run `php composer.phar install`
4) make a `../config.php` file with the following content:

```php
<?php
global $config, $content_path, $userupload_path;

$config = [
	"db_hostname" => "localhost",
	"db_username" => "blamba",
	"db_password" => "SECRET",
	"db_database" => "blamba",

	// for security reasons, we want to run a sandbox with imagemagick.
	// imagemagick is a pretty complex application and so it's easier to just debootstrap a full, empty debian:
	// debootstrap --arch amd64 --include=imagemagick bookworm /opt/bookworm-imagemagick/
	"imagemagick_root" => "/opt/bookworm-imagemagick/",

	"tempdir" => "/dev/shm/blamba/",

    "use_secure_cookies" => false,

    "wap_domain" => "wap.blamba.de",

	"smpp_sender_msn" => "YOUR NUMBER HERE",
	"smpp_sender_alpha" => "Blamba",
	"sms_sender_allowlist_override" => [
	],
	"log_token" => "SECRET",
];

$content_path = realpath(__DIR__ . "/blamba-content/") . "/";
$userupload_path = realpath(__DIR__ . "/blamba-usercontent/") . "/";
```

5) Add `SetEnv ap_trust_cgilike_cl 1` to your Apache VirtualHost config. Content-Length header is being set by PHP. Apache2 usually doesn't trust Content-Length headers, which then results in a missing progress indication on file downloads.
6) Add `AllowOverride All` in your Directory config in `/etc/apache2/apache2.conf` and run `sudo a2enmod rewrite`

7) Make the database in MySQL `CREATE DATABASE blamba`;
8) Import the database `mysql -u root -p blamba <blamba_empty.sql`
9) Create a Kannel gateway in MySQL

```sql
INSERT INTO `gateways` (`id`, `name`, `enabled`, `token`, `type`, `sender_msn`, `sender_name`, `location`, `kannel_url`, `kannel_username`, `kannel_password`, `smpp_hostname`, `smpp_port`, `smpp_username`, `smpp_password`) VALUES (1, 'NAME', 1, 'SECRET', 'kannel', 'NUMBER', 'Blamba', 'Bevel', 'http://localhost:13014', 'kannel', 'SECRET', NULL, NULL, NULL, NULL);
```
10) Rename the assets folder: `cp -r assets-int assets`
11) Edit your URL in `layout/header.html`
12) Import assets `git clone https://github.com/blamba-de/blamba-content-ingest.git` and copy the folders to your filesystem root
13) Setup imagemagick in debootstrap: `apt install debootstrap` then `debootstrap --arch amd64 --include=imagemagick bookworm /opt/bookworm-imagemagick/`
14) Install sandbox dependancies: `apt install bubblewrap libseccomp-dev`
15) Compile the bubblewrap wrappers: `cd /var/www/html/vendor-native/bwrap-seccomp && make`