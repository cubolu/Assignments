<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           19-02-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';

$basket_total = 0;  // Accumulate total price for all items in basket
$new_item_status = '';  // Holds panel with message, hidden/empty by default

if(isset($_REQUEST['product_id']) && isset($_REQUEST['action'])) {
  // Default state is invalid request
  $new_item_status = create_panel('Error. Invalid request.', 'alert-danger');

  if ($_REQUEST['action'] == 'add' && isset($_REQUEST['quantity'])) {
    $basket->add_item($db->get_product($_REQUEST['product_id']), $_REQUEST['quantity']);
    $new_item_status = create_panel($basket->get_status(), 'alert-info');
  } else if ($_REQUEST['action'] == 'inc') {
    $basket->update_qty($_REQUEST['product_id'], +1) && $new_item_status = create_panel($basket->get_status(), 'alert-info');
  } else if ($_REQUEST['action'] == 'dec') {
    $basket->update_qty($_REQUEST['product_id'], -1) && $new_item_status = create_panel($basket->get_status(), 'alert-info');
  } else if ($_REQUEST['action'] == 'rm') {
    $basket->remove_item($_REQUEST['product_id']) && $new_item_status = create_panel($basket->get_status(), 'alert-info');
  }
}


require_once 'includes/site_header.php';

?>
<section class="container content">
  <div class="row">
    <h3 class="col-md-10">Your shopping basket</h3>
    <span class="col-md-2"><a href="checkout.php" class="btn btn-default fill" role="button" style="margin-bottom: 20px">Checkout</a></span>
  </div>

  <div class="row">
    <?php echo $new_item_status; ?>
  </div>

  <div class="row">

    <div class="container panel panel-default">
      <!-- Table description -->
      <div class="row panel-body">
        <span class="col-md-8">Item</span>
        <span class="col-md-1 text-center">Price</span>
        <span class="col-md-1 text-center">Quantity</span>
        <span class="col-md-2 text-center">Total</span>
      </div>

      <!-- Basket items -->
      <ul class="list-group">
        <!-- Generate basket items -->
        <?php
        foreach ($basket as $product_id => $quantity) {
          $product = $db->get_product($product_id);
          echo '<li class="row list-group-item">';
            echo '<form action="basket.php" method="post">';
            echo '<input type="hidden" name="product_id" value="' . $product['product_id'] . '" />';
            echo '<span>';
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
                echo '<button type="submit" name="action" value="dec" class="close" aria-label="Decrease quantity" style="float: initial; padding-right: 4px"><i class="fa fa-minus-square" aria-hidden="true"></i></button>';
                echo $quantity;
                echo '<button type="submit" name="action" value="inc" class="close" aria-label="Increase quantity" style="float: initial; padding-left: 4px"><i class="fa fa-plus-square" aria-hidden="true"></i></button>';
              echo '</span>';
              echo '<span class="col-md-2 text-center">';
                echo number_format($product['price'] * $quantity, 2);
                echo '<button type="submit" name="action" value="rm" class="close" aria-label="Close"><i class="fa fa-trash" aria-hidden="true"></i></button>';
              echo '</span>';
            echo '</span>';
            echo '</form>';
          echo '</li>';

          $basket_total += $product['price'] * $quantity;
        }
        ?>
      </ul>

      <div class="row panel-footer total-price">
        <span class="col-md-offset-10 col-md-2 text-center"><i class="fa fa-gbp" style="padding: 0 4px 0 0"></i><?php echo number_format($basket_total, 2); ?></span>
      </div>
    </div>

  </div>
  <div class="row">

    <span class="col-md-offset-10 col-md-2">
      <a href="checkout.php" class="btn btn-default fill" role="button">Checkout</a>
    </span>
  </div>
</section>

<?php
require_once 'includes/site_footer.php';
require_once 'includes/application_bottom.php';

?>
