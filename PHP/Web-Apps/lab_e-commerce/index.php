<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           24-01-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';


// Set default page value to one
$page = 1;
$total_pages = $db->page_count(PRODUCTS_ON_PAGE);
if (isset($_REQUEST['page'])) {
  $page = intval($_REQUEST['page']);
  if ($page < 1 || $page > $total_pages) {
    // Invalid page number, redirecting to the first page
    $page = 1;
  }
}

$products = $db->get_products_on_page($page, PRODUCTS_ON_PAGE);

require_once 'includes/site_header.php';

?>


<!-- Main page content -->
<section class="container-fluid">

  <!-- Generate product grid -->
  <?php
  // TODO handle incomplete grid, $products->num_rows != $products_on_page
  for ($i_row = 0; $i_row < floor(PRODUCTS_ON_PAGE / PRODUCTS_IN_ROW); ++$i_row) {
    echo '<div class="row">';
    for ($i_col = 0; $i_col < PRODUCTS_IN_ROW; ++$i_col) {
      $product = $products[PRODUCTS_IN_ROW*$i_row + $i_col];
      echo '<div class="' . PRODUCTS_GRID_CLASS . ' product">';
        echo '<a href="product.php?productid=' . $product['product_id'] . '">';
          echo '<img src="' . $product['image_url'] . '" class="img-responsive">';
          echo '<span class="label label-default product-price product-price-on-image"><i class="fa fa-gbp" style="padding: 0 4px 0 0"></i>' . $product['price'] . '</span>';
        echo '</a>';
      echo '</div>';
    }
    echo '</div>';
  }
  ?>

  <!-- Pagination -->
  <div class="row">
    <div class="col-xs-12">
      <nav class="pull-right" aria-label="Page navigation">
        <ul class="pagination">
          <li <?php if($page <= 1) echo 'class="disabled"'; ?>>
            <a <?php if($page > 1) echo 'href="?page=' . ($page - 1) . '"'; ?> aria-label="Previous">
              <span aria-hidden="true">&laquo;</span>
            </a>
          </li>
          <!-- Generate page links -->
          <?php
          for ($i = 1; $i <= $total_pages; ++$i) {
            echo '<li ' . (($i == $page) ? 'class="active"' : '') . '>';
            echo '<a href="?page=' . $i . '">' . $i . '</a>';
            echo '</li>';
          }
          ?>
          <li <?php if($page >= $total_pages) echo 'class="disabled"'; ?>>
            <a <?php if($page < $total_pages) echo 'href="?page=' . ($page + 1) . '"'; ?> aria-label="Next">
              <span aria-hidden="true">&raquo;</span>
            </a>
          </li>
        </ul>
      </nav>
    </div>
  </div>
</section>

<?php

require_once 'includes/site_footer.php';
require_once 'includes/application_bottom.php';

?>
