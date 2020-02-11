<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           19-02-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'application_top.php';

?>

<!DOCTYPE html>
<html>
  <title>PosterShop</title> <!-- TODO switch titles based on file name! -->
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <!-- Font Awesome - the iconic font and CSS toolkit -->
  <link rel="stylesheet" href="https://use.fontawesome.com/4ea11a27f8.css">
  <!-- Bootstrap framework - minified CSS -->
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">
  <!-- Payment Font -->
  <link rel="stylesheet" href="css/paymentfont.min.css">

  <!-- Project CSS -->
  <link rel="stylesheet" href="css/main.css">

  <body>

    <!-- Header -->
    <header class="container-fluid">
      <section class="container">
        <div class="row">
          <h1 class="col-xs-4"><a href="index.php">JL</a></h1>
          <nav class="col-xs-8">
            <p><a href="#"><i class="fa fa-search fa-2x" aria-hidden="true"></i></a></p>
            <p><a href="#"><i class="fa fa-bars fa-2x" aria-hidden="true"></i></a></p>
            <p><a href="basket.php">
                <i class="fa fa-shopping-cart fa-2x" aria-hidden="true"></i>
                <span class="badge badge-corner"><?php if ($basket->items_count()) echo $basket->items_count(); ?></span></a>
            </p>
            <p><a href="<?php echo $user_home; ?>" class="text no-underline"><?php echo $user_name; ?></a></p>
          </nav>
        </div>
      </section>
    </header>