<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           20-02-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';

if (!isset($user)) {
  // User not logged - redirect to login page
  header('Location: login.php?location=' . urlencode($_SERVER['REQUEST_URI']));
  exit();
}

// Check for the order ID
$order_id = &$_REQUEST['order'];
if (!isset($order_id) || !$order = $db->get_order($user['user_id'], $order_id)) {
  $_SESSION['account_panel'] = array('msg'=>'Order not found.', 'type'=>'alert-danger');
  header("Location: account.php");
  exit();
}

$order_total = 0;
$ordered_products = $db->get_ordered_products($order_id);
$shipping_address = $db->get_address($order['shipping_address_id']);
if ($order['shipping_address_id'] == $order['shipping_address_id']) {
  $billing_address = &$shipping_address;
} else {
  $billing_address = $db->get_address($order['billing_address_id']);
}

require_once 'includes/site_header.php';

?>

<section class="container content">
  <div class="row">
    <div class="col-sm-12">
      <h2><?php echo $user['email']; ?></h2>
    </div>
  </div>
  <div class="row">

    <!-- Main panel -->
    <div class="col-md-8">
      <?php echo create_panel(order_status_detail($order['order_status']), order_status_style($order['order_status'])); ?>

      <div class="panel panel-default">
        <div class="panel-heading" style="font-weight: 500">
          <span>Order <?php echo '#' . str_pad($order_id, 10, '0', STR_PAD_LEFT); ?></span><span class="pull-right"><?php echo $order['order_time']; ?></span>
        </div>
        <div class="row panel-body">
          <span class="col-md-8">Item</span>
          <span class="col-md-1 text-center">Price</span>
          <span class="col-md-1 text-center">Qty</span>
          <span class="col-md-2 text-center">Total</span>
        </div>
        <ul class="list-group">
          <!-- Generate ordered items -->
          <?php
          foreach ($ordered_products as $product) {
            echo '<li class="list-group-item">';
              echo '<div class="row">';
                echo '<span class="col-md-4">';
                  echo '<a href="product.php?productid=' . $product['product_id'] . '">';
                  echo '<img src="' . $product['image_url'] . '" class="img-thumbnail">';
                  echo '</a>';
                echo '</span>';
                echo '<span class="col-md-4">';
                  echo '<a href="product.php?productid=' . $product['product_id'] . '" style="color: inherit;">';
                  echo $product['title'];
                  echo '</a>';
                echo '</span>';
                echo '<span class="col-md-1 text-center">' . $product['price'] . '</span>';
                echo '<span class="col-md-1 text-center">';
                  echo $product['quantity'];
                echo '</span>';
                echo '<span class="col-md-2 text-center">';
                  echo number_format($product['price'] * $product['quantity'], 2);
                echo '</span>';
              echo '</div>';
            echo '</li>';

            $order_total += $product['price'] * $product['quantity'];
          }
          ?>
        </ul>
        <div class="panel-footer">
          <div class="row">
            <span class="col-xs-10 payment-icon">
              <?php
              if ($order['payment_method'] == 'card') {
                echo '<i class="pf pf-credit-card"></i>';
                echo '<i class="pf pf-visa"></i><i class="pf pf-mastercard"></i><i class="pf pf-maestro"></i><i class="pf pf-american-express"></i>';
              } else if ($order['payment_method'] == 'paypal') {
                echo '<i class="pf pf-paypal" style="font-size: 4.5rem; padding-left: 1rem"></i>';
              } else {
                echo 'Unknown payment method!';
              }
              ?>
            </span>
            <span class="col-xs-2 text-center total-price">
              <i class="fa fa-gbp" style="padding: 0 4px 0 0"></i><?php echo number_format($order_total, 2); ?>
            </span>
          </div>
        </div>
      </div>

      <div class="row">
        <div class="col-sm-6">
          <div class="panel panel-default">
            <div class="panel-heading">Billing address</div>
            <div class="panel-body">
              <?php
              echo $billing_address['name'] . '<br />';
              echo str_replace(';', '<br />', $billing_address['address']) . '<br />';
              echo $billing_address['town'] . '<br />';
              if ($billing_address['county']) {
                echo $billing_address['county'] . ', ';
              }
              echo $billing_address['postcode'] . '<br />';
              echo $billing_address['country'] . '<br />';
              echo $billing_address['phone'] . '<br />';
              ?>
            </div>
          </div>
        </div>
        <div class="col-sm-6">
          <div class="panel panel-default">
            <div class="panel-heading">
              Delivery address - <?php echo $order['delivery_method']; ?> delivery
            </div>
            <div class="panel-body">
              <?php
              echo $shipping_address['name'] . '<br />';
              echo str_replace(';', '<br />', $shipping_address['address']) . '<br />';
              echo $shipping_address['town'] . '<br />';
              if ($shipping_address['county']) {
                echo $shipping_address['county'] . ', ';
              }
              echo $shipping_address['postcode'] . '<br />';
              echo $shipping_address['country'] . '<br />';
              echo $shipping_address['phone'] . '<br />';
              ?>
            </div>
          </div>
        </div>
      </div>

      <div class="row">
        <span class="col-md-offset-9 col-md-3">
          <a href="account.php" class="btn btn-default fill" role="button">Back</a>
        </span>
      </div>
    </div>

    <!-- Side panel -->
    <div class="col-md-4">
      <?php
      require 'includes/account_side_panel.php';
      ?>
    </div>

  </div>
</section>

<?php
require_once 'includes/site_footer.php';
require_once 'includes/application_bottom.php';

?>
