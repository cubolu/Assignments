<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           18-02-2017
#
# Testing:        PHP 7.0.13
#                 PHP 5 (sharedweb.cs.bangor.ac.uk)

require_once 'includes/application_top.php';

if (isset($user)) {
  // User already logged - redirect to user page
  header('Location: account.php');
  exit();
}


$login_status = '';  // Holds panel with message, hidden/empty by default
$input_field_status = array('email'=>'', 'password'=>'');

if (isset($_POST['submit'])) {
  // TODO else if validate email - regex javascript!
  if (!$_POST['email'] || !$_POST['password']) {
    $login_status = create_panel('Please supply your e-mail address and password.', 'alert-warning');
    $input_field_status['email'] = $_POST['email'] ? : 'has-warning';
    $input_field_status['password'] = $_POST['password'] ? : 'has-warning';
  } else if ($user_id = $db->validate_user($_POST['email'], $_POST['password'])) {
    // Remember user
    session_regenerate_id();  // TODO return status
    $_SESSION['user_id'] = $user_id;

    // Redirect to user page or previous location
    if (isset($_REQUEST['location']) && $_REQUEST['location']) {
      header('Location: ' . urldecode($_REQUEST['location']));
    } else {
      $_SESSION['account_panel'] = array('msg'=>'Successful Login. Welcome!', 'type'=>'alert-success');
      header('Location: account.php');
    }
    exit();
  } else {
    $login_status = create_panel('E-mail address or password you entered were invalid', 'alert-danger');
    $input_field_status['email'] = 'has-error';
    $input_field_status['password'] = 'has-error';
  }
}

require_once 'includes/site_header.php';

?>

<section class="container-fluid content content-form">
<section class="container">
  <div class="row">

    <div class="col-sm-offset-4 col-sm-4 card">
      <form action="login.php" method="post">
        <?php echo $login_status; ?>
        <input type="hidden" name="location" value="<?php if(isset($_REQUEST['location'])) echo htmlspecialchars($_REQUEST['location']); ?>">
        <div class="form-group">
          <div class="input-group <?php echo $input_field_status['email']; ?>">
            <span class="input-group-addon"><i class="fa fa-user-circle fa-fw"></i></span>
            <input type="text" name="email" class="form-control" placeholder="Email address" value="<?php print_sticky('email'); ?>">
          </div>
          <div class="input-group <?php echo $input_field_status['password']; ?>">
            <span class="input-group-addon"><i class="fa fa-key fa-fw"></i></span>
            <input type="password" name="password" class="form-control" placeholder="Password">
          </div>
        </div>
        <button type="submit" name="submit" class="btn btn-default fill">
          <i class="fa fa-sign-in" aria-hidden="true"></i> Login
        </button>
        <a href="#">Forgot your password?</a>
      </form>

      <div class="divider"><span>or register here:</span></div>
      <a href="registration.php<?php if(isset($_REQUEST['location'])) echo '?location=' . htmlspecialchars($_REQUEST['location']); ?>" class="btn btn-default fill" role="button">Create new account</a>
    </div>
  </div>
</section>
</section>

<?php

require_once 'includes/site_footer.php';
require_once 'includes/application_bottom.php';

?>
