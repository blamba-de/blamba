<?php
class SQL
{
	protected $db;
	function __construct()
	{
		$this->db = false;
	}

	function __destruct()
	{
		$this->disconnect();
	}

	function connect( $address, $user, $pwd, $db )
	{
		$this->db = @mysqli_connect( $address, $user, $pwd );
		if( $this->db )
		{
			mysqli_set_charset( $this->db, "utf8mb4" );

			return mysqli_select_db( $this->db, $db );
		}
	}

	function isConnected()
	{
		return $this->db;
	}

	function disconnect()
	{
		if( $this->db && @mysqli_close( $this->db ) )
		{
			$this->db = false;
			return true;
		}
	}

	function affectedRows()
	{
		return $this->db->affected_rows;
	}

	function insertID()
	{
		return $this->db->insert_id;
	}

	function nextInsertID( $table )
	{
		return 1 + $this->fetch( "SELECT COUNT( LAST_INSERT_ID() ) AS `count` FROM `" . $db->escape( $table ) . "`", "count" );
	}

	function escape( $string )
	{
		return mysqli_real_escape_string( $this->db,  $string );
	}

	function prepared_query ( ...$args )
	{
		if (count($args) < 3)
			throw new Exception("to few arguments for SQL::prepare_query()");

		$stmt = $this->db->prepare($args[0]);
		array_shift($args);
		$stmt->bind_param(...$args);
		$stmt->execute();

		if ($stmt->error != "")
		{
			Logging::log("prepared_query", ["error" => $stmt->error, "args" => $args]);
			return false;
		}

		$resource = $stmt->get_result();
		if( gettype( $resource ) == "object" )
		{
			$q = new Query( $resource );
			return $q;
		}
		elseif( $resource )
		{
			return $resource;
		}

		return true;
	}

	function prepared_fetch ( ...$args )
	{
		$q = $this->prepared_query ( ...$args );
		if( $q )
			return $q->fetch();
	}

	function prepared_fetch_one ( ...$args )
	{
		$q = $this->prepared_query ( ...$args );
		if( $q )
			return $q->fetch(true);
	}

	function query( $query )
	{
		if( !$query )
			return false;

		$return = false;

		$resource = mysqli_query( $this->db, $query );

		if( gettype( $resource ) == "object" )
		{
			$q = new Query( $resource );

			return $q;
		}
		elseif( $resource )
		{
			return $resource;
		}
		else
		{
			//echo $query . "<br />" . $this->error();
			error_log("SQL error occured. " . $this->error());
		}

		return false;
	}

	function fetch( $query, $specific = NULL )
	{
		$q = $this->query( $query );
		if( $q )
			return $q->fetch( $specific );
	}

	function error()
	{
		return $this->db->error;
	}

	public static function & current()
	{
		static $instance = null;
		return $instance = ( empty( $instance ) ) ? new self() : $instance;
	}
}

class Query
{
	protected $query;

	function __construct($query)
	{
		$this->query = $query;
	}

	function numRows()
	{
		return $this->query->num_rows;
	}

	function fetch( $specific = NULL )
	{
		if( is_string( $specific ))
		{
			$result = mysqli_fetch_assoc ( $this->query );
			return $result[ $specific ];
		}
		elseif( isset( $specific ) )
		{
			return mysqli_fetch_assoc ( $this->query );
		}
		else
		{
			$results = array();
			for( $i = 0; $i < $this->query->num_rows ; $i++ )
				$results[] = mysqli_fetch_assoc( $this->query );

			return $results;
		}
	}

	function fetchRow()
	{
		return mysqli_fetch_assoc( $this->query );
	}

	function __destruct()
	{
		mysqli_free_result( $this->query );
	}
}
