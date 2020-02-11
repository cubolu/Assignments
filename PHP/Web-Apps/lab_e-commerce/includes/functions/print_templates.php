<?php

# E-commerce - Assignment, Web-based Applications
# Author:               Jakub Lukac
# E-mail:               eeu893@bangor.ac.uk
# Date:                 21-02-2017
#
# Testing:              PHP 7.0.13
#                       PHP 5 (sharedweb.cs.bangor.ac.uk)


function create_panel($msg, $alert_type) {
  return <<<EOT
<div class="alert $alert_type" role="alert"> 
  $msg
</div>
EOT;
}

function create_email_body($user_id, $order_id, DBECommerce &$db) {
  if (!$order = $db->get_order($user_id, $order_id)) {
    return false;
  }

  // Load order info
  $order_total = 0;
  $ordered_products = $db->get_ordered_products($order_id);
  $shipping_address = $db->get_address($order['shipping_address_id']);
  if ($order['shipping_address_id'] == $order['shipping_address_id']) {
    $billing_address = &$shipping_address;
  } else {
    $billing_address = $db->get_address($order['billing_address_id']);
  }

  // Create email body
  $email_body =  'Thank you for shopping with us!' . '<br />';
  $email_body .= 'Your order #' . $order_id . ' will be processed soon.' . '<br /><br />';
  
  $email_body .= 'Ordered:' . '<br />';
  foreach ($ordered_products as $product) {
    $email_body .= $product['title'] . ' - ' . $product['price'] . ' x '. $product['quantity'] . '<br />';
    $order_total += $product['price'] * $product['quantity'];
  }
  $email_body .= 'Total: ' . $order_total .'<br />';
  $email_body .= '<br />';

  $email_body .= 'Billing address:' . '<br />';
  $email_body .= "\t" 
    . $billing_address['name'] . ', ' 
    . str_replace(';', ', ', $billing_address['address']) . ', '
    . $billing_address['town'] . ', '
    . ($billing_address['county'] ? $billing_address['county'] . ', ' : '')
    . $billing_address['postcode'] . ', '
    . $billing_address['country'] . '<br />'
    . $billing_address['phone'] . '<br />';

  $email_body .= 'Delivery address: (' . $order['delivery_method'] . ' delivery)' . '<br />';
  $email_body .= "\t"
    . $shipping_address['name'] . ', '
    . str_replace(';', ', ', $shipping_address['address']) . ', '
    . $shipping_address['town'] . ', '
    . ($shipping_address['county'] ? $shipping_address['county'] . ', ' : '')
    . $shipping_address['postcode'] . ', '
    . $shipping_address['country'] . '<br />'
    . $shipping_address['phone'] . '<br />';
  $email_body .= '<br />';

  $email_body .= 'Payment method: ';
  switch ($order['payment_method']) {
    case 'card':
      $email_body .= 'Paid by Card';
      break;
    case 'paypal':
      $email_body .= 'PayPal';
      break;
    default:
      $email_body .= 'Unknown! Please contact us.';
      break;
  }
  $email_body .= '<br />';

  return $email_body;
}

function print_sticky($label) {
  if (isset($_REQUEST[$label]))
    echo $_REQUEST[$label];
}

function print_sticky_checked($label, $value) {
  if (isset($_REQUEST[$label]) && $_REQUEST[$label] == $value)
    echo 'checked';
}

function order_status_detail($status) {
  switch ($status) {
    case 'pending':
      return 'Your order will be processed soon.';
      break;
    case 'processing':
      return 'We are working on it...';
      break;
    case 'shipped':
      return 'Order shipped...';
      break;
    case 'completed':
      return 'Order completed.';
      break;
    case 'cancelled':
      return 'Order was canceled.';
      break;
    case 'declined':
      return 'Payment was declined. Try again.';
      break;
    default:
      return 'Unknown order. Please contact us!';
      break;
  }
}

function order_status_style($status) {
  switch ($status) {
    case 'pending':
    case 'processing':
      return 'bg-info';
      break;
    case 'shipped':
      return 'bg-warning';
      break;
    case 'completed':
      return 'bg-success';
      break;
    case 'cancelled':
    case 'declined':
      return 'bg-danger';
      break;
    default:
      return 'bg-danger';
      break;
  }
}

function lockdown_page($status = '') {
  $file = $_SERVER['PHP_SELF'];

  return <<<EOT
<!DOCTYPE html>
<html>
  <title>Grant Access</title>
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
    <!-- ######################################## -->
    <section class="container-fluid content content-form" style="min-height: inherit;">
    <section class="container">
      <div class="row">
        <div class="col-sm-offset-4 col-sm-4 card">
          <form action="$file" method="post">
            <div class="form-group">
              <div class="divider" style="padding-top: 0;"><span>input the authorization phrase</span></div>
              <div class="input-group $status">
                <span class="input-group-addon"><i class="fa fa-key fa-fw"></i></span>
                <input type="password" name="phrase" class="form-control" placeholder="Phrase">
              </div>
            </div>
            <button type="submit" name="grant_access" class="btn btn-default fill">
              <i class="fa fa-sign-in" aria-hidden="true"></i> Grant access
            </button>
          </form>
        </div>
      </div>
    </section>
    </section>
    <!-- ######################################## -->
  </bod>
</html>
EOT;
}

?>