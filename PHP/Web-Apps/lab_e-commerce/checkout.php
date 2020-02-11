<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           19-02-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';

if (!isset($user)) {
  // User not logged - redirect to login page
  header('Location: login.php?location=' . urlencode($_SERVER['REQUEST_URI']));
  exit();
}

if ($basket->is_empty()) {
  // Basket is empty - redirect to account page and show error message
  $_SESSION['account_panel'] = array('msg'=>'Your shopping basket is empty. Cannot checkout. Continue shopping...', 'type'=>'alert-info');
  header('Location: account.php');
  exit();
}


$submit_status = '';  // Holds panel with message, hidden/empty by default
$input_field_status = array('name'=>'', 'address'=>'', 'postcode'=>'', 'town'=>'', 'country'=>'', 'county'=>'', 'phone'=>'', 'delivery'=>'');

if (isset($_POST['submit'])) {
  if (!$_POST['name'] || !$_POST['address1'] || !$_POST['postcode'] || !$_POST['town'] || !$_POST['country'] || !$_POST['phone'] || !isset($_POST['delivery'])) {
    $submit_status = create_panel('Please supply all information.', 'alert-warning');
    $input_field_status['name'] = $_POST['name'] ? : 'has-warning';
    $input_field_status['address'] = $_POST['address1'] ? : 'has-warning';
    $input_field_status['postcode'] = $_POST['postcode'] ? : 'has-warning';
    $input_field_status['town'] = $_POST['town'] ? : 'has-warning';
    $input_field_status['country'] = $_POST['country'] ? : 'has-warning';
    $input_field_status['phone'] = $_POST['phone'] ? : 'has-warning';
    $input_field_status['delivery'] = isset($_POST['delivery']) ? : 'has-warning';
  } else if (!preg_match('/^((00|\+)[0-9]{2,5}|0)[1-9][0-9]{8,9}$/', str_replace(' ', '', $_POST['phone']))) {
    // Wrong phone number format
    $submit_status = create_panel('Phone number you entered was invalid. Please use the international call prefix, such as <span style="font-weight: 500">(+44)</span>', 'alert-danger');
    $input_field_status['phone'] = 'has-error';
  } else if (!in_array($_POST['delivery'], array('normal', 'express', 'other'))) {
    // Invalid delivery option
    $submit_status = create_panel('Please select the delivery option.', 'alert-danger');
    $input_field_status['delivery'] = 'has-error';
  } else {
    // Save the address and proceed to the payment page
    $address_ref = $db->add_address($user['user_id'], $_POST['name'], $_POST['address1'] . ';' . $_POST['address2'],
                                    $_POST['postcode'], $_POST['town'], $_POST['country'], $_POST['county'], $_POST['phone']);

    if ($address_ref) {
      $_SESSION['checkout'] = array('shipping_adr'=>$address_ref, 'delivery'=>$_POST['delivery']);
      header('Location: checkout_payment.php');
      exit();
    } else {
      $submit_status = create_panel('Address is not accepted. Try again or contact us for more information', 'alert-danger');
    }
  }
}

require_once 'includes/site_header.php';

?>

<section class="container-fluid content content-form">
<section class="container">
  <div class="row">
    <!-- Checkout form -->
    <div class="col-md-8 card">
      <form action="checkout.php" method="post">
        <?php echo $submit_status; ?>

        <!-- Basic info -->
        <div class="row">
          <div class="col-sm-12 form-group <?php echo $input_field_status['name']; ?>">
            <label class="control-label">Full name</label>
            <input type="text" class="form-control" name="name" placeholder="Your name or company name" value="<?php print_sticky('name'); ?>">
          </div>
        </div>

        <!-- Address -->
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

        <div class="row">
          <div class="col-sm-6 form-group <?php echo $input_field_status['phone']; ?>">
            <label for="checkoutContact" class="control-label">Phone number</label>
            <input type="text" class="form-control" name="phone" placeholder="Number" value="<?php print_sticky('phone'); ?>">
          </div>
        </div>

        <!-- Delivery options -->
        <div class="divider"><span>Delivery options</span></div>
        <div class="<?php echo $input_field_status['delivery']; ?>">
          <div class="radio">
            <label>
              <input type="radio" name="delivery" value="normal" <?php print_sticky_checked('delivery', 'normal'); ?>>
              Normal Delivery
            </label>
          </div>
          <div class="radio">
            <label>
              <input type="radio" name="delivery" value="express" <?php print_sticky_checked('delivery', 'express'); ?>>
              Express Delivery
            </label>
          </div>
          <div class="radio">
            <label>
              <input type="radio" name="delivery" value="other" <?php print_sticky_checked('delivery', 'other'); ?>>
              Other Delivery
            </label>
          </div>
        </div>

        <!-- Submit button -->
        <button type="submit" name="submit" class="btn btn-default fill">Continue to payment</button>

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
