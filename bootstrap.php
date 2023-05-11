<?php
define("SERVER_PATH", dirname(__FILE__)."/");

// Config parameters (SQL credentials, etc.)
require_once(SERVER_PATH . "/../config.php");

// Autoloading
spl_autoload_register(function ($class_name) {
    include(SERVER_PATH . "libs/" . $class_name . '.php');
});
require_once(SERVER_PATH . 'vendor/autoload.php');

// Twig HTML templating engine
$loader = new \Twig\Loader\FilesystemLoader(SERVER_PATH . 'templates');
$twig = new \Twig\Environment($loader, [
    'cache' => false,
    'auto_reload' => true,
    'autoescape' => 'html'
]);

// MySQL connection
$db = new SQL();
$db->connect($config["db_hostname"], $config["db_username"], $config["db_password"], $config["db_database"]);
