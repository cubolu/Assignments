<?php

# E-commerce - Assignment, Web-based Applications
# Author:             Jakub Lukac
# E-mail:             eeu893@bangor.ac.uk
# Date:               19-02-2017
#
# Testing:            PHP 7.0.13
#                     PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';

if (isset($user)) {
  // User already logged - redirect to user page
  header('Location: account.php');
  exit();
}

$reg_status = '';  // Holds panel with message, hidden/empty by default
$input_field_status = array('name'=>'', 'email'=>'', 'password'=>'');

if(isset($_POST['submit'])) {
  if (!$_POST['forename'] || !$_POST['surname'] || !$_POST['email'] || !$_POST['password'] || !$_POST['password_check']) {
    // Empty input field(s)
    $reg_status = create_panel('Please, fill in each box with valid data.', 'alert-info');
    $input_field_status['name'] = $_POST['forename'] && $_POST['surname'] ? : 'has-warning';
    $input_field_status['email'] = $_POST['email'] ? : 'has-warning';
    $input_field_status['password'] = $_POST['password'] && $_POST['password_check'] ? : 'has-warning';
  //} else if {
    // TODO else if validate data - regex javascript!
  } else if ($_POST['password'] != $_POST['password_check']) {
    // Passwords not match
    $reg_status = create_panel('Passwords do not match. Try again.', 'alert-info');
    $input_field_status['password'] = 'has-warning';
  } else if ($user_id = $db->add_user($_POST['email'], $_POST['password'], $_POST['forename'], $_POST['surname'])) {
    // Successful registration - log in user
    session_regenerate_id();  // TODO return status
    $_SESSION['user_id'] = $user_id;

    // Redirect to user page or previous location
    if (isset($_REQUEST['location']) && $_REQUEST['location']) {
      header('Location: ' . urldecode($_REQUEST['location']));
    } else {
      $_SESSION['account_panel'] = array('msg'=>'Successful Registration. Welcome!', 'type'=>'alert-success');
      header('Location: account.php');
    }
    exit();
  } else {
    // User cannot be added
    if ($db->p_errno == DUPLICATE_ENTRY) {
      $reg_status = create_panel('E-mail address already in use. <a href="login.php">Login</a>.', 'alert-danger');
      $input_field_status['email'] = 'has-error';
    } else {
      $reg_status = create_panel('Please, supply valid data for each box.', 'alert-danger');
      $input_field_status['name'] = 'has-error';
      $input_field_status['email'] = 'has-error';
      $input_field_status['password'] = 'has-error';
    }
  }
}

require_once 'includes/site_header.php';

?>

<section class="container-fluid content content-form">
<section class="container">
  <div class="row">
    <div class="col-md-4 col-md-offset-4 card">
      <form action="registration.php" method="post">
        <?php echo $reg_status ?>
        <input type="hidden" name="location" value="<?php if(isset($_REQUEST['location'])) echo htmlspecialchars($_REQUEST['location']); ?>">
        <!-- Basic info -->
        <div class="form-group <?php echo $input_field_status['name']; ?>">
          <div class="input-group">
            <span class="input-group-addon"><i class="fa fa-user-circle fa-fw"></i></span>
            <input type="text" name="forename" class="form-control" style="border-top-right-radius: 4px; border-bottom-right-radius: 4px"  id="regForename" placeholder="Forename" value="<?php print_sticky('forename'); ?>">
            <input type="text" name="surname" class="form-control" id="regSurname" placeholder="Surname" value="<?php print_sticky('surname'); ?>">
          </div>
        </div>
        <div class="form-group <?php echo $input_field_status['email']; ?>">
          <div class="input-group">
            <span class="input-group-addon"><i class="fa fa-envelope fa-fw"></i></span>
            <input type="text" name="email" class="form-control" id="regEmail" placeholder="Email"  value="<?php print_sticky('email'); ?>">
          </div>
        </div>
        <div class="form-group <?php echo $input_field_status['password']; ?>">
          <div class="input-group">
            <span class="input-group-addon"><i class="fa fa-key fa-fw"></i></span>
            <input type="password" name="password" class="form-control" style="border-top-right-radius: 4px; border-bottom-right-radius: 4px" id="regPassword" placeholder="Password">
            <input type="password" name="password_check" class="form-control" id="regPasswordCheck" placeholder="Password again">
          </div>
        </div>
        <!-- Options -->
        <div class="checkbox">
          <label>
            <input type="checkbox" name="newsletter" checked> Newsletter
          </label>
        </div>
        <p>
          By clicking 'Create your account' I agree to the <a>Terms & Conditions and Privacy Policy</a>.
        </p>
        <!-- Submit button -->
        <button type="submit" name="submit" class="btn btn-default fill">Create your account</button>

      </form>
    </div>
  </div>
</section>
</section>

<?php

require_once 'includes/site_footer.php';
require_once 'includes/application_bottom.php';

?>
