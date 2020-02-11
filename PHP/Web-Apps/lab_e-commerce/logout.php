<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           17-03-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';

if (isset($user)) {
  // Logout user
  $tmp_basket = $_SESSION['basket'];  // backup basket items

  session_destroy();
  session_start();
  $_SESSION['basket'] = $tmp_basket;  // restore basket items
  $_SESSION['access_granted'] = true;  // restore access
}

header("Location: index.php");

require_once 'includes/application_bottom.php';

?>
