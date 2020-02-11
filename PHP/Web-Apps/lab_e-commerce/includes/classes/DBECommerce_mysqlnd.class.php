<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           06-03-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

class DBECommerce extends mysqli
{
  public $p_errno;  // Persistent error number
  public $p_error;  // Persistent error message

  /**
   * Extended standard mysqli constructor
   */
  function  __construct($host = null, $username = null, $password = null, $db_name = "", $port = null, $socket = null) {
    parent::__construct($host, $username, $password, $db_name, $port, $socket);

    // Check connection
    if ($this->connect_errno) {
      printf("Connect failed: (%d) %s\n", $this->connect_errno, $this->connect_error);
      exit();
    }

    // Set character set
    if (!$this->set_charset("utf8")) {
      printf("Cannot load character set utf8: %s\n", $this->error);
      exit();
    }
  }

  function __destruct() {
    parent::close();
  }

  /**
   * Validate user credentials
   *
   * @param string $email     user email address, user need to be registered
   * @param string $password  user password, in plain text
   * @return int|bool         user ID, or false if email and password not match
   */
  public function validate_user($email, $password) {
    $query_string = 'SELECT user_id, email, password FROM users WHERE email = ? LIMIT 1';
    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('s', $email)) {
      $stmt->execute();
      $result = $stmt->get_result();

      if ($result) {
        $user = $result->fetch_assoc();
        $result->close();
        if ($user && $user['password'] == sha1($password))
          return $user['user_id'];
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return false;
  }

  /**
   * Get user details
   *
   * @param int $user_id   user email address, user need to be registered
   * @return array         user details as associative array
   */
  public function get_user($user_id) {
    $query_string = 'SELECT user_id, email, forename, surname FROM users WHERE user_id = ? LIMIT 1';
    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('d', $user_id)) {
      $stmt->execute();
      if ($result = $stmt->get_result()) {
        $user = $result->fetch_assoc();
        $result->close();
        return $user ? $user : array();
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return array();
  }

  /**
   * Add a new user
   *
   * @param string $email     user email address
   * @param string $password  user password, in plain text
   * @param string $forename  user forename
   * @param string $surname   user surname
   * @return int|bool         user ID, or false if email address already in use
   */
  public function add_user($email, $password, $forename, $surname) {
    $query_string = 'INSERT INTO users(email, password, forename, surname) VALUE (?, ?, ?, ?)';
    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('ssss', $email, sha1($password), $forename, $surname)) {
      if ($stmt->execute()) {
        return $this->insert_id;
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return false;
  }

  /**
   * Add a new address
   *
   * @param int $user_id      user ID
   * @param string $name
   * @param string $address
   * @param string $postcode
   * @param string $town
   * @param string $country
   * @param string $county
   * @param string $phone
   * @return bool|int         address ID, or false if error occurred
   */
  public function add_address($user_id, $name, $address, $postcode, $town, $country, $county = '', $phone = '') {

    // TODO check if address already exist

    $query_string = 'INSERT INTO userAddresses(user_id, name, address, postcode, town, country, county, phone)
                     VALUE (?, ?, ?, ?, ?, ?, ?, ?)';
    $stmt = $this->prepare($query_string);

    if ($stmt->bind_param('ssssssss', $user_id, $name, $address, $postcode, $town, $country, $county, $phone)) {
      if ($stmt->execute()) {
        return $this->insert_id;
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return false;
  }

  /**
   * Create a new order record
   *
   * @param int $user_id              user ID
   * @param string $order_status      initial status
   * @param int $shipping_addr_ref    shipping address ID
   * @param int $billing_addr_ref     billing address ID
   * @param string $delivery_method   delivery method option
   * @param string $payment_method    payment method option
   * @param string $billing_ref       payment reference
   * @param BasketECommerce $basket   ordered products
   * @return bool|mixed               order ID
   */
  public function create_order($user_id, $order_status, $shipping_addr_ref, $billing_addr_ref, $delivery_method, $payment_method, $billing_ref, $basket) {

    // TODO check if address is user address

    // TODO add transaction

    // Insert the order
    $query_string = 'INSERT INTO orders(user_id, order_status, order_time, shipping_address_id, billing_address_id, delivery_method, payment_method, billing_ref)
                       VALUE (?, ?, NOW(), ?, ?, ?, ?, ?)';
    $stmt = $this->prepare($query_string);

    if ($stmt->bind_param('sssssss', $user_id, $order_status, $shipping_addr_ref, $billing_addr_ref, $delivery_method, $payment_method, $billing_ref)) {
      if ($stmt->execute()) {
        $order_id = $this->insert_id;

        foreach ($basket as $product_id => $quantity) {
          $this->add_product_to_order($order_id, $product_id, $quantity);
          // TODO check return if transaction
        }

        return $order_id;
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return false;
  }

  /**
   * Link product to specific order
   *
   * @param int $order_id     order ID
   * @param int $product_id   product ID
   * @param int $quantity     product quantity
   * @return bool             true, or false if product cannot be linked or found
   */
  protected function add_product_to_order($order_id, $product_id, $quantity) {
    $product = $this->get_product($product_id);

    if ($product) {
      $query_string = 'INSERT INTO orderedProducts(order_id, product_id, price, quantity) VALUE (?, ?, ?, ?)';
      $stmt = $this->prepare($query_string);
      if ($stmt->bind_param('ssss', $order_id, $product_id, $product['price'], $quantity)) {
        if ($stmt->execute()) {
          return true;
        }
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return false;
  }

  /**
   * Get order
   *
   * @param int $user_id    user ID
   * @param int $order_id   order ID
   * @return array          order
   */
  public function get_order($user_id, $order_id) {
    $query_string = 'SELECT order_status, DATE_FORMAT(DATE(order_time), \'%d-%m-%Y\') as order_time, shipping_address_id, billing_address_id, delivery_method, payment_method FROM orders 
                       WHERE order_id = ? AND user_id = ? LIMIT 1';
    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('dd', $order_id, $user_id)) {
      $stmt->execute();
      if ($result = $stmt->get_result()) {
        $order = $result->fetch_assoc();
        $result->close();

        return $order ? $order : array();
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return array();
  }

  /**
   * Get all products which were included in the order
   *
   * @param int $order_id             order ID
   * @param bool $simple              simple result form flag
   * @return array|bool|mysqli_result ordered products
   */
  public function get_ordered_products($order_id, $simple = false) {
    if ($simple) {
      $query_string = 'SELECT orderedProducts.price as price, quantity, title FROM orderedProducts 
                         JOIN products ON (orderedProducts.product_id = products.product_id) WHERE orderedProducts.order_id = ?';
    } else {
      $query_string = 'SELECT orderedProducts.product_id as product_id, orderedProducts.price as price, quantity, image_url, title FROM orderedProducts 
                         JOIN products ON (orderedProducts.product_id = products.product_id) WHERE orderedProducts.order_id = ?';
    }

    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('d', $order_id)) {
      $stmt->execute();
      if ($results = $stmt->get_result()) {
        if ($simple) {
          $simple_results = array('titles'=>'', 'total'=>0.0);
          while ($product = $results->fetch_assoc()) {
            $simple_results['titles'] .= ', ' . $product['title'];
            $simple_results['total'] += $product['price'] * $product['quantity'];
          }
          $simple_results['titles'] = substr($simple_results['titles'], 2);

          return $simple_results;
        } else {
          return $results;
        }
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return false;
  }

  /**
   * Get all user's orders
   *
   * @param int $user_id          user ID
   * @return bool|mysqli_result   orders as query results, or false if error occurred
   */
  public function get_orders($user_id) {
    $query_string = 'SELECT order_id, order_status, DATE_FORMAT(DATE(order_time), \'%d-%m-%Y\') as order_time FROM orders WHERE user_id = ?';
    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('d', $user_id)) {
      $stmt->execute();
      if ($results = $stmt->get_result()) {
        return $results;
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return false;
  }

  /**
   * Get only actual order status
   *
   * @param int $order_id     order ID
   * @return string           order status, or empty string if error occurred
   */
  public function get_order_status($order_id) {
    $query_string = 'SELECT order_status FROM orders WHERE order_id = ? LIMIT 1';
    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('d', $order_id)) {
      $stmt->execute();
      if ($result = $stmt->get_result()) {
        $order = $result->fetch_assoc();
        $result->close();

        return $order ? $order['order_status'] : '';
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return '';
  }

  /**
   * Get address
   *
   * @param int $address_id   address ID
   * @return array            address
   */
  public function get_address($address_id) {
    $query_string = 'SELECT name, address, postcode, town, country, county, phone FROM userAddresses WHERE  address_id = ? LIMIT 1';
    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('d', $address_id)) {
      $stmt->execute();
      if ($result = $stmt->get_result()) {
        $address = $result->fetch_assoc();
        $result->close();

        return $address ? $address : array();
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return array();
  }

  /**
   * Get product details
   *
   * @param int $product_id     product ID
   * @return array              product information
   */
  public function get_product($product_id) {
    $query_string = 'SELECT * FROM products WHERE product_id = ? LIMIT 1';
    $stmt = $this->prepare($query_string);
    if ($stmt->bind_param('d', $product_id)) {
      $stmt->execute();
      if ($result = $stmt->get_result()) {
        $product = $result->fetch_assoc();
        $result->close();
        return $product ? $product : array();
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return array();
  }

  /**
   * Get all products for certain page
   *
   * @param int $page               page number
   * @param int $products_on_page   number of products shown on one page
   * @return bool|mysqli_result     products as query results, or false if error occurred
   */
  public function get_products_on_page($page, $products_on_page) {
    $query_string = 'SELECT product_id, image_url, price FROM products LIMIT ? OFFSET ?';
    $stmt = $this->prepare($query_string);
    $offset = ($page - 1) * $products_on_page;
    if ($stmt->bind_param('dd', $products_on_page, $offset)) {
      $stmt->execute();
      if ($results = $stmt->get_result()) {
        return $results;
      }
    }

    $this->p_errno = $this->errno; $this->p_error = $this->error;
    return false;
  }

  /**
   * Count all available products
   */
  public function products_count() {
    $query_string = 'SELECT COUNT(product_id) FROM products;';
    $result = $this->query($query_string);
    if($result !== false) {
      $count = $result->fetch_row();
      $result->close();
      return $count[0];
    }

    return 0;
  }

  /**
   * Count pages needed for all products
   *
   * @param int $products_on_page   number of products shown on one page
   * @return int                    total page count
   */
  public function page_count($products_on_page) {
    return ceil($this->products_count() / $products_on_page);
  }

  private function get_result($stmt) {
    $RESULT = array();
    $stmt->store_result();
    for ($i = 0; $i < $stmt->num_rows; $i++ ) {
      $metadata = $stmt->result_metadata();
      $params = array();
      while ( $Field = $metadata->fetch_field() ) {
        $params[] = &$RESULT[ $i ][ $Field->name ];
      }
      call_user_func_array( array( $stmt, 'bind_result' ), $params );
      $stmt->fetch();
    }
    return $RESULT;
  }
}

?>
