<?php

# E-commerce - Assignment, Web-based Applications
# Author:         Jakub Lukac
# E-mail:         eeu893@bangor.ac.uk
# Date:           20-04-2017
#

?>

<div>
  <!-- Logout button -->
  <a href="logout.php" class="panel panel-default btn btn-default fill" role="button">Log out</a>
  <!-- Address -->
  <div class="panel panel-default">
    <div class="panel-heading">Saved address</div>
    <div class="panel-body">
      <!-- TODO load from DB -->
      Address mock<br />
      PERIS<br />
      Ffriddoedd<br />
      Ffriddoedd Road<br />
      BANGOR<br />
      Gwynedd, LL57 2GE<br />
    </div>
  </div>
  <!-- Statistic -->
  <div class="panel panel-default">
    <div class="panel-heading">Numbers</div>
    <div class="panel-body">
      <!-- TODO load from DB -->
      Statistic
    </div>
  </div>
  <!-- Change password form -->
  <div class="panel panel-default">
    <div class="panel-body">
      <form action="<?php echo $_SERVER["PHP_SELF"]; ?>" method="post">
        <!-- Passwords -->
        <div class="form-group">
          <input type="password" class="form-control" id="changeOldPassword" placeholder="Old password">
        </div>
        <div class="form-group">
          <input type="password" class="form-control" id="changeNewPassword" placeholder="New password">
          <input type="password" class="form-control" id="changeNewPasswordCheck" placeholder="New password again">
        </div>
        <!-- Submit button -->
        <button type="submit" class="btn btn-default fill">Change password</button>
      </form>
    </div>
  </div>
</div>