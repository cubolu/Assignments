<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           19-02-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';

if (!isset($user) || $basket->is_empty() || !isset($_SESSION['checkout']) || !$_SESSION['checkout']) {
  // Cannot continue to payment - start checkout again
  header('Location: checkout.php');
  exit();
}


$submit_status = '';  // Holds panel with message, hidden/empty by default
$input_field_status = array('payment'=>'', 'billing'=>'', 'name'=>'', 'address'=>'', 'postcode'=>'', 'town'=>'', 'country'=>'', 'county'=>'', 'agreement'=>'');

if (isset($_POST['submit'])) {
  if (isset($_POST['payment']) && ($_POST['payment'] == 'card' || $_POST['payment'] == 'paypal') && isset($_POST['billing'])) {
    // Payment and billing are selected
    $billing_address_ref = NULL;

    if ($_POST['billing'] == 'same') {
      // Billing address same as shipping
      $billing_address_ref = $_SESSION['checkout']['shipping_adr'];
    } else if ($_POST['billing'] == 'other') {
      // Adding a new billing address
      if (!$_POST['name'] || !$_POST['address1'] || !$_POST['postcode'] || !$_POST['town'] || !$_POST['country']) {
        $submit_status = create_panel('Please supply all information.', 'alert-warning');
        $input_field_status['name'] = $_POST['name'] ? : 'has-warning';
        $input_field_status['address'] = $_POST['address1'] ? : 'has-warning';
        $input_field_status['postcode'] = $_POST['postcode'] ? : 'has-warning';
        $input_field_status['town'] = $_POST['town'] ? : 'has-warning';
        $input_field_status['country'] = $_POST['country'] ? : 'has-warning';
      } else {
        // Save the address
        $billing_address_ref = $db->add_address($user['user_id'], $_POST['name'], $_POST['address1'] . ';' . $_POST['address2'],
                                                $_POST['postcode'], $_POST['town'], $_POST['country'], $_POST['county']);

        if (!$billing_address_ref) {
          $submit_status = create_panel('Address is not accepted. Try again or contact us for more information', 'alert-danger');
        }
      }
    } else {
      // Invalid billing address option
      $submit_status = create_panel('Please select the billing address option.', 'alert-danger');
      $input_field_status['billing'] = 'has-error';
    }

    if ($billing_address_ref) {
      // Billing address was valid
      if (!isset($_POST['agreement']) || !$_POST['agreement'] == 'on') {
        // Agreement was not checked
        $submit_status = create_panel('Please check that you have read the Terms & Conditions and Privacy Policy.', 'alert-danger');
        $input_field_status['agreement'] = 'has-error';
      } else {

        // This should redirect customer to the payment provider's page...

        // Implicit success

        // Save the order
        $order_id = $db->create_order(
          $user['user_id'],
          INIT_ORDER_STATUS,
          $_SESSION['checkout']['shipping_adr'],
          $billing_address_ref,
          $_SESSION['checkout']['delivery'],
          $_POST['payment'],
          sha1(mt_rand()),  // simulate billing reference
          $basket
        );

        if ($order_id) {
          // Send e-mail notification
          $to = $user['email'];
          $subject = 'Order #' . str_pad($order_id, 10, '0', STR_PAD_LEFT) . 'from PosterShop';
          $message = create_email_body(
            $user['user_id'],
            $order_id,
            $db
          );
          $headers =  'MIME-Version: 1.0' . "\r\n";
          $headers .= 'Content-type: text/html;charset=UTF-8' . "\r\n";
          $headers .= 'From: <orders@postershop.com>' . "\r\n";
          mail($to, $subject, $message, $headers);

          $basket->clear();

          // Store the order ID for submit page
          $_SESSION['checkout'] = $order_id;

          header('Location: checkout_submit.php');
          exit();
        } else {
          $submit_status = create_panel('Order is not accepted. Try again or contact us for more information', 'alert-danger');
        }
      }
    }
  } else {
    $submit_status = create_panel('Please select the payment method and billing address.', 'alert-warning');
    $input_field_status['payment'] = isset($_POST['payment']) && ($_POST['payment'] == 'card' || $_POST['payment'] == 'paypal') ? : 'has-warning';
    $input_field_status['billing'] = isset($_POST['billing']) ? : 'has-warning';
  }
}

require_once 'includes/site_header.php';

?>

<section class="container-fluid content content-form">
  <section class="container">
    <div class="row">
      <!-- Checkout form -->
      <div class="col-md-8 card">
        <form action="checkout_payment.php" method="post">
          <?php echo $submit_status; ?>

          <!-- Payment method -->
          <div class="payment-icon <?php echo $input_field_status['payment']; ?>">
            <div class="radio">
              <label>
                <input type="radio" name="payment" value="card" <?php print_sticky_checked('payment', 'card'); ?>>
                <i class="pf pf-credit-card"></i>
                <i class="pf pf-visa"></i><i class="pf pf-mastercard"></i><i class="pf pf-maestro"></i><i class="pf pf-american-express"></i>
              </label>
            </div>
            <div class="radio">
              <label>
                <input type="radio" name="payment" value="paypal" <?php print_sticky_checked('payment', 'paypal'); ?>>
                <i class="pf pf-paypal" style="font-size: 4.5rem; padding-left: 1rem"></i>
              </label>
            </div>
          </div>

          <!-- Billing address -->
          <div class="divider"><span>Billing address</span></div>

          <div class="<?php echo $input_field_status['billing']; ?>">
            <div class="radio">
              <label>
                <input type="radio" name="billing" value="same" <?php if (isset($_REQUEST['billing'])) print_sticky_checked('billing', 'same'); else echo 'checked'; ?>>
                Billing address same as shipping
              </label>
            </div>
            <div class="radio">
              <label>
                <input type="radio" name="billing" value="other" <?php print_sticky_checked('billing', 'other'); ?>>
                or add billing address here
              </label>
            </div>
          </div>

          <div> <!-- TODO hide -->
            <div class="row">
              <div class="col-sm-12 form-group <?php echo $input_field_status['name']; ?>">
                <label class="control-label">Full name</label>
                <input type="text" class="form-control" name="name" placeholder="Your name or company name" value="<?php print_sticky('name'); ?>">
              </div>
            </div>
            <div class="row">
              <div class="col-sm-12 form-group <?php echo $input_field_status['address']; ?>">
                <label class="control-label">Street address</label>
                <input type="text" class="form-control" name="address1" placeholder="Address line one" value="<?php print_sticky('address1'); ?>">
                <input type="text" class="form-control" name="address2" placeholder="Address line two (optional)" value="<?php print_sticky('address2'); ?>">
              </div>
            </div>

            <div class="row">
              <div class="col-sm-6 form-group <?php echo $input_field_status['postcode']; ?>">
                <label class="control-label">Postcode</label>
                <input type="text" class="form-control" name="postcode" placeholder="Postcode or Zip" value="<?php print_sticky('postcode'); ?>">
              </div>
              <div class="col-sm-6 form-group <?php echo $input_field_status['town']; ?>">
                <label class="control-label">Town/City</label>
                <input type="text" class="form-control" name="town" placeholder="Town/City" value="<?php print_sticky('town'); ?>">
              </div>
            </div>

            <div class="row">
              <div class="col-sm-6 form-group <?php echo $input_field_status['country']; ?>">
                <label for="checkoutCountry" class="control-label">Country</label>
                <input type="text" class="form-control" name="country" placeholder="Country" value="<?php print_sticky('country'); ?>">
              </div>
              <div class="col-sm-6 form-group <?php echo $input_field_status['county']; ?>">
                <label for="checkoutCountry" class="control-label">County</label>
                <input type="text" class="form-control" name="county" placeholder="County (optional)" value="<?php print_sticky('county'); ?>">
              </div>
            </div>
          </div>

          <div class="checkbox <?php echo $input_field_status['agreement']; ?>">
            <label>
              <input type="checkbox" name="agreement">I confirm that I have read the <a>Terms & Conditions and Privacy Policy</a>.
            </label>
          </div>

          <!-- Submit button -->
          <button type="submit" name="submit" class="btn btn-default fill">Submit the order and Pay</button>

        </form>
      </div>

      <!-- Order summary-->
      <div class="col-md-4">
        <?php
        require 'includes/checkout_side_panel.php';
        ?>
      </div>

    </div>
  </section>
</section>


<?php

require_once 'includes/site_footer.php';
require_once 'includes/application_bottom.php';

?>
