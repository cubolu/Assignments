<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           06-03-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

class BasketECommerce implements Iterator
{
  private $basket;
  private $status;

  function __construct(&$raw_basket) {
    $this->basket = &$raw_basket;
  }

  public function is_empty() {
    return empty($this->basket);
  }

  public function clear() {
    $this->basket = array();
  }

  public function get_status() {
    return $this->status;
  }

  public function add_item($product, $qty) {
    $this->status = '';
    $product_id = $product['product_id'];

    if (!($product)) {
      $this->status = 'Product does not found.';
      return false;
    } else if (array_key_exists($product_id, $this->basket)) {
      $this->status = 'Product already in basket. Quantity of "' . $product['title'] . '" increased.';
      $this->basket[$product_id] += $qty;
      return true;
    } else {
      $this->status = 'Product "' . $product['title'] . '" added to your basket.';
      $this->basket[$product_id] = $qty;
      return true;
    }
  }

  public function remove_item($product_id) {
    $this->status = '';

    if (array_key_exists($product_id, $this->basket)) {
      $this->status = 'Product removed.';
      unset($this->basket[$product_id]);
      return true;
    }
    return false;
  }

  public function update_qty($product_id, $offset) {
    $this->status = '';

    if (array_key_exists($product_id, $this->basket)) {
      if (($this->basket[$product_id] + $offset) > 0) {
        $this->status = 'Product quantity updated.';
        $this->basket[$product_id] += $offset;
      } else {
        $this->status = 'Product removed.';
        unset($this->basket[$product_id]);
      }
      return true;
    }
    return false;
  }

  public function items_count() {
    $count = 0;
    foreach ($this->basket as $qty) {
      $count += $qty;
    }

    return $count;
  }

  /** ************************************************* Iterator *************************************************** **/

  /**
   * Return the current element
   * @link http://php.net/manual/en/iterator.current.php
   * @return mixed Can return any type.
   * @since 5.0.0
   */
  public function current() {
    return current($this->basket);
  }

  /**
   * Move forward to next element
   * @link http://php.net/manual/en/iterator.next.php
   * @return void Any returned value is ignored.
   * @since 5.0.0
   */
  public function next() {
    next($this->basket);
  }

  /**
   * Return the key of the current element
   * @link http://php.net/manual/en/iterator.key.php
   * @return mixed scalar on success, or null on failure.
   * @since 5.0.0
   */
  public function key() {
    return key($this->basket);
  }

  /**
   * Checks if current position is valid
   * @link http://php.net/manual/en/iterator.valid.php
   * @return boolean The return value will be casted to boolean and then evaluated.
   * Returns true on success or false on failure.
   * @since 5.0.0
   */
  public function valid() {
    $key = key($this->basket);
    return ($key !== null && $key !== false);
  }

  /**
   * Rewind the Iterator to the first element
   * @link http://php.net/manual/en/iterator.rewind.php
   * @return void Any returned value is ignored.
   * @since 5.0.0
   */
  public function rewind() {
    reset($this->basket);
  }
}