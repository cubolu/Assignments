-- E-commerce - Assignment, Web-based Applications
-- Author:              Jakub Lukac
-- E-mail:              eeu893@bangor.ac.uk
-- Date:                01-03-2017
--

# DROP DATABASE IF EXISTS PosterShop;
# CREATE DATABASE IF NOT EXISTS PosterShop;
# USE PosterShop;
USE u514937001_pshop;

SELECT 'Creating database structure' as 'INFO';

DROP TABLE IF EXISTS
  users,
  userAddresses,
  products,
  orders,
  orderedProducts;

CREATE TABLE users (
  user_id     INT UNSIGNED      NOT NULL  AUTO_INCREMENT,
  email       VARCHAR(40)       NOT NULL  UNIQUE,
  password    CHAR(40)          NOT NULL,
  forename    VARCHAR(20)       NOT NULL,
  surname     VARCHAR(20)       NOT NULL,

  PRIMARY KEY (user_id, email)
);

CREATE TABLE userAddresses (
  address_id  INT UNSIGNED      NOT NULL  AUTO_INCREMENT,
  user_id     INT UNSIGNED      NOT NULL,
  name        VARCHAR(60)       NOT NULL,
  address     VARCHAR(100)      NOT NULL,
  postcode    VARCHAR(10)       NOT NULL,
  town        VARCHAR(40)       NOT NULL,
  country     VARCHAR(30)       NOT NULL,
  county      VARCHAR(30),
  phone       VARCHAR(15),

  PRIMARY KEY (address_id),
  FOREIGN KEY (user_id) REFERENCES users(user_id) ON UPDATE CASCADE ON DELETE RESTRICT
);

CREATE TABLE products (
  product_id  INT UNSIGNED      NOT NULL  AUTO_INCREMENT,
  price       DECIMAL(10,2)     NOT NULL,
  image_url   VARCHAR(40)       NOT NULL,
  title       TINYTEXT          NOT NULL,
  description TEXT,

  PRIMARY KEY (product_id)
);

CREATE TABLE orders (
  order_id            INT UNSIGNED      NOT NULL  AUTO_INCREMENT,
  user_id             INT UNSIGNED      NOT NULL,
  order_status        ENUM('pending', 'processing', 'shipped', 'completed', 'cancelled', 'declined')  NOT NULL,
  order_time          DATETIME          NOT NULL,

  shipping_address_id INT UNSIGNED      NOT NULL,
  billing_address_id  INT UNSIGNED      NOT NULL,

  delivery_method     ENUM('normal', 'express', 'other')  NOT NULL,
  payment_method      ENUM('card', 'paypal')              NOT NULL,
  billing_ref         CHAR(40)                            NOT NULL,

  PRIMARY KEY (order_id),
  FOREIGN KEY (user_id) REFERENCES users(user_id) ON UPDATE CASCADE ON DELETE RESTRICT,
  FOREIGN KEY (shipping_address_id) REFERENCES userAddresses(address_id) ON UPDATE CASCADE ON DELETE RESTRICT,
  FOREIGN KEY (billing_address_id) REFERENCES userAddresses(address_id) ON UPDATE CASCADE ON DELETE RESTRICT
);

CREATE TABLE orderedProducts (
  order_id    INT UNSIGNED      NOT NULL,
  product_id  INT UNSIGNED      NOT NULL,
  price       DECIMAL(10,2)     NOT NULL,
  quantity    SMALLINT UNSIGNED NOT NULL,

  PRIMARY KEY (order_id, product_id),
  FOREIGN KEY (order_id)   REFERENCES orders(order_id)     ON UPDATE CASCADE ON DELETE RESTRICT,
  FOREIGN KEY (product_id) REFERENCES products(product_id) ON UPDATE CASCADE ON DELETE RESTRICT
);

-- ---------------------------------------------------------------------------------------------------------------------

SELECT 'Populate the database' as 'INFO';

INSERT INTO users(email, password, forename, surname) VALUES
  ('cubolu@cubolu.com', SHA('pass'), 'Jakub', 'Lukac'),
  ('admin@admin.com', SHA('admin'), 'Admin', 'Admin'),
  ('test@cubolu.com', SHA('test'), 'Test', 'Test'),
  ('user@cubolu.com', SHA('user'), 'User', 'User'),
  ('dev@cubolu.com', SHA('dev'), 'Dev', 'Dev');

INSERT INTO userAddresses(user_id, name, address, postcode, town, country, county, phone) VALUES
  (1, 'Jakub Lukac', 'Peris, University of Wales Bangor;Ffriddoedd Road', 'LL57 2GE', 'Bangor', 'UK', 'Gwynedd', '+441234512345'),
  (2, '', 'Ffriddoedd Road', 'LL57 2GE', 'Bangor', 'UK', 'Gwynedd', '+441234512345'),
  (3, '', 'Ffriddoedd Road', 'LL57 2GE', 'Bangor', 'UK', 'Gwynedd', '+441234512345'),
  (4, 'User User', 'Ffriddoedd Road', 'LL57 2GE', 'Bangor', 'UK', 'Gwynedd', '+441234512345'),
  (5, '', 'Ffriddoedd Road', 'LL57 2GE', 'Bangor', 'UK', 'Gwynedd', '+441234512345');

SET @mock_description = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed eleifend venenatis enim nec bibendum. Vivamus sed risus arcu. Ut dictum libero non fringilla rutrum. Donec ac mollis nisi, eget congue sapien. Sed viverra semper ex a suscipit. Nullam scelerisque diam ut augue gravida, eu iaculis nunc feugiat. Integer aliquam ultrices metus eget tristique. Maecenas a mattis nunc.';
INSERT INTO products(price, image_url, title, description) VALUES
  (13.00, 'images/products/product_1.jpg', 'Sunset Edinburgh', @mock_description),
  (13.23, 'images/products/product_2.jpg', 'Snowdonia', @mock_description),
  (13.45, 'images/products/product_3.jpg', 'Sunset Bangor', @mock_description),
  (12.34, 'images/products/product_4.jpg', 'Frog', @mock_description),
  (11.11, 'images/products/product_5.jpg', 'Beddgelert', @mock_description),
  (11.11, 'images/products/product_6.jpg', 'Bangor Pier', @mock_description),
  (11.11, 'images/products/product_7.jpg', 'Edinburgh', @mock_description),
  (11.11, 'images/products/product_8.jpg', 'Edinburgh - Holyrood Park', @mock_description),
  (11.11, 'images/products/product_9.jpg', 'Edinburgh - Castle', @mock_description),
  (11.11, 'images/products/product_10.jpg', 'Frog', @mock_description),
  (11.11, 'images/products/product_11.jpg', 'Snowdonia', @mock_description),
  (11.11, 'images/products/product_12.jpg', 'Bangor Port', @mock_description),
  (11.11, 'images/products/product_13.jpg', 'Snowdonia', @mock_description),
  (11.11, 'images/products/product_14.jpg', 'Snowdonia', @mock_description),
  (11.11, 'images/products/product_15.jpg', 'Snowdonia', @mock_description),
  (11.11, 'images/products/product_16.jpg', 'Snowdonia', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description),
  (11.11, 'images/products/mock_image2.png', 'PosterX', @mock_description)
;

INSERT INTO orders(order_status, user_id, order_time, shipping_address_id, billing_address_id, delivery_method, payment_method, billing_ref) VALUES
  ('pending',    1, NOW(), 1, 1, 'normal', 'card', SHA('1')),
  ('pending',    1, NOW(), 1, 1, 'normal', 'card', SHA('2')),
  ('processing', 1, NOW(), 1, 1, 'normal', 'card', SHA('3')),
  ('completed',  1, NOW(), 1, 1, 'normal', 'card', SHA('4')),
  ('shipped',    1, NOW(), 1, 1, 'normal', 'card', SHA('5')),
  ('shipped',    2, NOW(), 2, 2, 'normal', 'card', SHA('6'));

INSERT INTO orderedProducts(order_id, product_id, price, quantity) VALUES
  (1, 1, 13.00, 4),
  (1, 2,  1.00, 1),
  (1, 5, 11.11, 2),
  (2, 2, 13.23, 1),
  (3, 3, 13.45, 1),
  (4, 4, 12.34, 1),
  (5, 5, 11.11, 1),
  (6, 7, 11.11, 2);
