<?php
class Session
{
	private static $instance = null;

	public $handler = null;

	public function init()
	{
		global $config;

		if ( session_status() == PHP_SESSION_NONE )
		{
			$session_handler = new SessionHandlerSQL();
			
			session_set_save_handler( $session_handler, true );
			session_name( "blamba_session" );

			session_start([
				'cookie_secure' => $config['use_secure_cookies'],
				'cookie_httponly' => true,
				'cookie_lifetime' => 365 * 24 * 60 * 60,
				'gc_maxlifetime'  => 365 * 24 * 60 * 60,
			]);

			$this->handler = $session_handler;
			self::$instance = $this;

			Device::generate_registration_token();
		}
	}

	public function reload()
	{
		$data = $this->handler->read(session_id());
		session_decode($data);
	}

	public function save()
	{
		global $db;
		if ($this->handler instanceof SessionHandlerSQL)
		{
			$db->prepared_query("
				INSERT INTO `sessions` (`id`, `data`, `registration_token`, `insert_date`, `updated_at`) VALUES ( ?, ?, ?, NOW(), NOW()) ON DUPLICATE KEY UPDATE
					`data` = VALUES( `data` ), `registration_token` = VALUES( `registration_token` ), `updated_at` = NOW()", "ssi", session_id(), session_encode(), $_SESSION['registration_token'] ?? null);
		}
	}

	public static function current()
	{
		return ( empty( self::$instance ) ? new self() : self::$instance );
	}
}
