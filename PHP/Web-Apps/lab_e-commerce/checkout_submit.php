<?php

# E-commerce - Assignment, Web-based Applications
# Author:                 Jakub Lukac
# E-mail:                 eeu893@bangor.ac.uk
# Date:                   19-02-2017
#
# Testing:                PHP 7.0.13
#                         PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';

if (!isset($user) || !isset($_SESSION['checkout'])) {
  // Cannot continue to payment - start checkout again
  header('Location: checkout.php');
  exit();
}

// Invalidate checkout info - checkout finished
$order_id = $_SESSION['checkout'];
unset($_SESSION['checkout']);

if (!is_int($order_id)) {
  $_SESSION['account_panel'] = array('msg'=>'Checkout process might be unfinished. Please check your inbox or contact us for more information.', 'type'=>'alert-danger');
  header('Location: account.php');
  exit();
}


require_once 'includes/site_header.php';

?>

<section class="container content">
  <div class="row">
    <div class="panel panel-default">
      <div class="panel-body <?php echo order_status_style($db->get_order_status($order_id)); ?>">
        <span><?php echo order_status_detail($db->get_order_status($order_id)); ?></span>
        <span>Continue for more information.</span>
        <h3>Order ID: #<?php echo str_pad($order_id, 10, '0', STR_PAD_LEFT); ?></h3>
        <span>Order details were sent to your e-mail inbox.</span>
      </div>
    </div>
  </div>
  <div class="row">
    <span class="col-md-2">
      <a href="order_details.php?order=<?php echo $order_id; ?>" class="btn btn-default fill" role="button">Continue</a>
    </span>
  </div>
</section>

<?php

require_once 'includes/site_footer.php';
require_once 'includes/application_bottom.php';

?>
