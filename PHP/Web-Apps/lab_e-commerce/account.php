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
  header("Location: login.php");
  exit();
}


// Show status message from the previous page
$status = '';
if (isset($_SESSION['account_panel'])) {
  $status = create_panel($_SESSION['account_panel']['msg'], $_SESSION['account_panel']['type']);
  unset($_SESSION['account_panel']);  // remove the message
}

$orders = $db->get_orders($user['user_id']);
if (!$orders) {
  $status .= create_panel('Cannot list the orders.', 'alert-danger');
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
      <?php echo  $status; ?>

      <div class="panel panel-default">
        <div class="panel-heading">Your orders</div>
        <table class="table table-hover">
          <thead>
            <tr>
              <th class="col-md-2">
                # Order number
              </th>
              <th class="col-md-2 text-center">
                Status
              </th>
              <th class="col-md-2 text-center">
                Date
              </th>
              <th class="col-md-4">
                Items
              </th>
              <th class="col-md-2 text-center">
                Total
              </th>
            </tr>
          </thead>

          <!-- Orders -->
          <tbody>

            <!-- List all user's orders-->
            <?php
            if ($orders) {
              foreach ($orders as $order) {
                $ordered_products = $db->get_ordered_products($order['order_id'], true);
                echo '<tr>';
                echo '<th class="col-md-2" scope="row">';
                echo '<a href="order_details.php?order=' . $order['order_id'] . '">';
                echo '#' . str_pad($order['order_id'], 10, '0', STR_PAD_LEFT);
                echo '</a>';
                echo '</th>';
                echo '<td class="col-md-2 text-center">';
                echo $order['order_status'];
                echo '</td>';
                echo '<td class="col-md-2 text-center">';
                echo $order['order_time'];
                echo '</td>';
                echo '<td class="col-md-4">';
                echo $ordered_products['titles'];
                echo '</td>';
                echo '<td class="col-md-2 text-center">';
                echo number_format($ordered_products['total'], 2);
                echo '</td>';
                echo '</tr>';
              }
            }
            ?>
          </tbody>
        </table>
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
