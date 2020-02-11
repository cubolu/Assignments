<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           06-03-2017
#

require_once 'configure.php';
require_once 'classes/DBECommerce.class.php';
require_once 'classes/BasketECommerce.php';
require_once 'functions/print_templates.php';

session_start();

// INFO page lock-down
if (isset($_POST['grant_access'])) {
  if (strtolower(trim($_POST['phrase'])) == LOCKDOWN_PHRASE) {
    $_SESSION['access_granted'] = true;
  } else {
    $_SESSION['access_granted'] = false;
  }
}

if(!isset($_SESSION['access_granted']) || !$_SESSION['access_granted']) {
  echo lockdown_page(isset($_SESSION['access_granted']) ? 'has-error' : '');
  require_once 'application_bottom.php';
  exit();
}


$db = new DBECommerce(
  $config['database']['host'],
  $config['database']['user'],
  $config['database']['password'],
  $config['database']['db_name']
);


// Default user info
$user = NULL;
$user_home = 'login.php';
$user_name = 'Login';

// Is user logged in
if(isset($_SESSION['user_id'])) {
  $user = $db->get_user($_SESSION['user_id']);
  $user_home = 'account.php';
  $user_name = $user['forename'];
}

// Initialize a new basket
if(!isset($_SESSION['basket'])) {
  $_SESSION['basket'] = array();
}

$basket = new BasketECommerce($_SESSION['basket']);

?>
