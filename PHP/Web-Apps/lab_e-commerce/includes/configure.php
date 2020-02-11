<?php

# E-commerce - Assignment, Web-based Applications
# Author:               Jakub Lukac
# E-mail:               eeu893@bangor.ac.uk
# Date:                 21-02-2017
#
# Testing:              PHP 7.0.13
#                       PHP 5 (sharedweb.cs.bangor.ac.uk)


define('DEBUG',1);

if (DEBUG == 1) {
  error_reporting(E_ALL|E_STRICT);
  ini_set('display_errors',1);
}

// TODO set log
//ini_set("log_errors", 1);
//ini_set("error_log", "./error.log");
//error_log( "Hello, errors!" );

//date_default_timezone_set('Europe/London');

$config = parse_ini_file('config.ini', true);

define('LOCKDOWN_PHRASE', 'nodobre');

define('PRODUCTS_ON_PAGE',12);
define('PRODUCTS_IN_ROW',4);
define('PRODUCTS_GRID_CLASS', 'col-sm-6 col-lg-3');

define('INIT_ORDER_STATUS', 'pending');

define('DUPLICATE_ENTRY', 1062);

?>
