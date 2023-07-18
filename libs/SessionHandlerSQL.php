<?php
class SessionHandlerSQL implements SessionHandlerInterface
{
	public function open( $save_path, $session_name )
	{
		return true;
	}

	public function close()
	{
		return true;
	}

	public function read( $id )
	{
		global $db;
		$row = $db->prepared_fetch_one("SELECT * FROM `sessions` WHERE `id` = ?", "s", $id);

		if ( $row !== false and !is_null( $row ) )
		{
			return $row[ 'data' ];
		}
		return "";
	}

	public function write( $id, $data )
	{
		return true;
	}

	public function destroy( $id )
	{
		global $db;
		$result = $db->prepared_query("DELETE FROM `sessions` WHERE id = ?", "s", $id);
		return ( $result !== false );
	}

	public function gc( $max )
	{
		global $db;
		$db->query( "DELETE FROM `sessions` WHERE `updated_at` < DATE_SUB(NOW(), INTERVAL 24 HOUR);" );
	}
}
