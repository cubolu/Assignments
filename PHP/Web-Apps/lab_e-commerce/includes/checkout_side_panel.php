<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           19-04-2017
#

$basket_total = 0;  // Accumulate total price for all items in basket

?>

<div class="panel panel-default">
  <div class="panel-heading">Order Summary</div>
  <ul class="list-group">
    <?php
    foreach ($basket as $product_id => $quantity) {
      $product = $db->get_product($product_id);
      echo '<li class="list-group-item">';
      echo '<span>' . $product['title'] .'</span>';
      echo '<span class="pull-right text-center">' . $quantity . '<i class="fa fa-times" style="padding: 0 4px"></i>' . $product['price'] . '</span>';
      echo '</li>';

      $basket_total += $product['price'] * $quantity;
    }
    ?>
  </ul>
  <div class="panel-footer total-price">
    <span>Total:</span>
    <span class="pull-right">
      <i class="fa fa-gbp" style="font-weight: inherit; padding: 0 4px 0 0"></i>
      <?php echo number_format($basket_total, 2); ?>
    </span>
  </div>
</div>