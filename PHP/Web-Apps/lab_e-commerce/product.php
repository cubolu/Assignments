<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           19-02-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';


// Check for the product ID
$product_id = &$_REQUEST['productid'];
if (!isset($product_id) || !$product = $db->get_product($product_id)) {
  // TODO show 404
  header("Location: index.php");
  exit();
}

require_once 'includes/site_header.php';

?>

<section class="container content">

  <div class="row">
    <!-- Image -->
    <div class="col-md-8">
      <img src="<?php echo $product['image_url']; ?>" class="img-responsive">
    </div>
    <!-- Description -->
    <div class="col-md-4">
      <div class="panel panel-default">
        <div class="panel-body">
          <h3><?php echo $product['title']; ?></h3>
          <?php echo $product['description']; ?>
        </div>

        <ul class="list-group">
          <li class="list-group-item">
            <h2 style="display: inline"><span class="label label-default product-price"><?php echo $product['price']; ?>$</span></h2>
            <span class="pull-right">
              <form action="basket.php" method="post" class="form-inline">
                <input type="hidden" name="product_id" value="<?php echo $product_id; ?>" />
                <input type="hidden" name="action" value="add" />
                <div class="form-group">
                  <!--<label class="" for="exampleInputAmount">Quantity: </label>-->
                  <div class="input-group">
                    <input type="text" name="quantity" class="form-control" style="width: 4rem" value="1" aria-label="Quantity">
                    <input type="submit" name="submit" class="input-group-addon" style="height: 34px" value="Add to basket" aria-label="Add to basket">
                  </div>
                </div>
              </form>
            </span>
          </li>
        </ul>
      </div>
    </div>
  </div>

</section>


<?php

require_once 'includes/site_footer.php';
require_once 'includes/application_bottom.php';

?>
