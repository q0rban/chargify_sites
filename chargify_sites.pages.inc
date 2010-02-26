<?php
// $Id$

/**
 * @file
 *   Page callbacks and admin forms.
 */

/**
 * Configuration settings form.
 */
function chargify_sites_settings_form() {
  $form = array();

  $form['chargify_sites_products'] = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Products'),
    '#description' => t('Select which products should be available on the site signup form.'),
  );

  $products = variable_get('chargify_sites_products', array());

  foreach(chargify_api_products_get() as $product) {
    $handle = $product->getHandle();
    $name = $product->getName();
    $price = number_format(($product->getPriceInCents() / 100), 2); // Convert from price in cents.

    $args = array(
      '@name' => $name,
      '@price' => $price,
    );

    $form['chargify_sites_products'][$handle] = array(
      '#type' => 'checkbox',
      '#title' => t('@name | @price', $args),
      '#default_value' => $products[$handle],
    );
  }

  $form = system_settings_form($form);

  return $form;
}

/**
 * Page callback for the New Site page.
 */
function chargify_sites_new_site_page($product) {
  dsm($product);
}

/**
 * Page callback for the Payment page.
 */
function chargify_sites_payment_page($node) {
  if ($node) {
    if (!(isset($node->subscription) && $node->subscription->state)) {
      return drupal_get_form('chargify_sites_payment_form', $node);
    }
    else {
      drupal_set_title(t("You're paid up!"));
      return t('It looks like this site is already paid for.  Congrats!');
    }
  }
  else {
    return t('Whoops, did you get here by accident?  Try going back to the <a href="!url">sign up page</a>.', array('!url' => url('node/add/site')));
  }
}

/**
 * Payment form.
 */
function chargify_sites_payment_form(&$form_state, $node) {
  $form = array();

  $form['#node'] = $node;

  // @todo: check to make sure user registration is turned on.
  if (user_is_anonymous()) {
    $form['register'] = array(
      '#type' => 'fieldset',
      '#title' => t('Create an account'),
      '#attributes' => array('class' => 'register'),
      '#tree' => TRUE,
      '#prefix' => "<div class='login-register-wrapper'>",
    );
    $form['register']['mail'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail address'),
      '#default_value' => '',
      '#maxlength' => EMAIL_MAX_LENGTH,
    );
    $form['register']['pass'] = array(
      '#type' => 'password_confirm',
    );

    $form['login'] = array(
      '#type' => 'fieldset',
      '#title' => t('Login to an existing account'),
      '#attributes' => array('class' => 'login'),
      '#tree' => TRUE,
      '#suffix' => "</div>",
    );
    $form['login']['mail'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail address'),
      '#default_value' => '',
      '#maxlength' => EMAIL_MAX_LENGTH,
    );
    $form['login']['pass'] = array(
      '#type' => 'password',
      '#title' => t('Password'),
    );
  }
  else {
    global $user;

    $form['mail'] = array(
      '#type' => 'hidden',
      '#value' => $user->mail,
    );

    // The user object doesn't have the customer ID yet, so we need to load it.
    if ($customer_id = chargify_uid_to_customer_id($user->uid)) {
      $form['customer_id'] = array(
        '#type' => 'hidden',
        '#value' => $customer_id,
      );
    }
  }

  $form['payment'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment info'),
    '#attributes' => array('class' => 'payment'),
  );
  $form['payment']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First Name'),
    '#default_value' => '',
    '#required' => TRUE,
  );

  $form['payment']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
    '#default_value' => '',
    '#required' => TRUE,
  );
  $form['payment']['card_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Credit Card Number'),
    '#required' => TRUE,
  );
/*  $form['payment']['cvv'] = array(
    '#type' => 'textfield',
    '#title' => t('CVV'),
    '#size' => 4,
    '#maxlength' => 4,
    '#description' => t('The 3 or 4 digit verification number, usually on the back.')
  );
  $form['payment']['billing_zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Billing Zip Code'),
    '#maxlength' => 10,
    '#required' => TRUE,
  );*/
  $form['payment']['expiration_month'] = array(
    '#type' => 'select',
    '#title' => t('Expiration'),
    '#options' => _chargify_sites_months(),
  );
  $form['payment']['expiration_year'] = array(
    '#type' => 'select',
    '#options' => _chargify_sites_years(),
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Build it!'));

  return $form;
}

/**
 * Implementation of hook_form_validate().
 *
 * Errors should be signaled with form_set_error().
 */
function chargify_sites_payment_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  if (isset($values['login']) || isset($values['register'])) {
    if ($values['login']['mail']) {
      $values['login']['name'] = $values['login']['mail'];
      $dummy_form_state = array('values' => &$values['login']);
      foreach (user_login_default_validators() as $fnc) {
        $fnc($form, $dummy_form_state);
      }
    }
    else if ($values['register']['mail']) {
      $values['register']['name'] = $values['register']['mail'];
      $dummy_form_state = array('values' => &$values['register']);
      user_register_validate($form, $dummy_form_state);
    }
    else {
      form_set_error('register', t('Whoops, you need to log in or register in order to pay!'));
    }
  }

  // Verify numeric on certain fields.
  $fields = array('card_number' => t('credit card number')); //, 'cvv' => t('CVV'), 'billing_zip' => t('billing zip code'));
  foreach ($fields as $field => $label) {
    $value = $values[$field];

    if (is_string($value)) {
      $chars = array('-', '.', ' ');
      $value = str_replace($chars, '', $value);
    }
    if (!preg_match('!^[0-9]+$!', $value)) {
      form_set_error($field, t('The !field must be a number.', array('!field' => $label)));
    }
  }

  // Verify the date fields
  $year = $values['expiration_year'];
  $month = $values['expiration_month'];
  if ($month == 0) {
    form_set_error('expiration_month', t('You must select a month.'));
  }
  if ($year == 0) {
    form_set_error('expiration_year', t('You must select a year.'));
  }

  // Verify that the expiration date is a future date.
  $this_year = date('Y', time());
  $this_month = date('n', time());
  if ($year == $this_year && $month < $this_month) {
    form_set_error('expiration_month', t('You appear to have selected an expiration date prior to now.'));
  }
}

/**
 * Submit handler.
 */
function chargify_sites_payment_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $node = $form['#node'];
  global $user;

  if (isset($values['register']) && $values['register']['mail']) {
    $values['register']['name'] = $values['register']['mail'];
    $dummy_form_state = array('values' => &$values['register']);
    user_register_submit($form, $dummy_form_state);
  }

  $account = $user;

  if (isset($values['customer_id'])) {
    $account->customer_id = $values['customer_id'];
    // Unset the customer ID so chargify.module doesn't try to add it again.
    unset($values['customer_id']);
  }

  if ($account && $subscription = chargify_sites_create_subscription($values, $account, $node)) {
    // Update the node creator
    $node->uid = $account->uid;
    $node->subscription->subscription_id = $subscription->getId();
    node_save($node);
    $form_state['redirect'] = "node/{$node->nid}";
  }
  else {
    drupal_set_message(t("Hmm. Something didn't work right. You might want to contact an administrator."), 'error');
  }
}

/**
 * Page callback for Thank you page.
 */
function chargify_sites_paid_page($node) {
  return t('Yay, @title is all paid up!', array('@title' => $node->title));
}

/**
 * Helper function to output an array of months.
 */
function _chargify_sites_months() {
  // I left the number out of the t function, thinking that there's likely already
  // translations for the months this way.
  return array(
    0 => t('Month'),
    1 => '1 - ' . t('Jan'),
    2 => '2 - ' . t('Feb'),
    3 => '3 - ' . t('Mar'),
    4 => '4 - ' . t('Apr'),
    5 => '5 - ' . t('May'),
    6 => '6 - ' . t('Jun'),
    7 => '7 - ' . t('Jul'),
    8 => '8 - ' . t('Aug'),
    9 => '9 - ' . t('Sep'),
    10 => '10 - ' . t('Oct'),
    11 => '11 - ' . t('Nov'),
    12 => '12 - ' . t('Dec'),
  );
}

/**
 * Helper function to output an array of years.
 */
function _chargify_sites_years() {
  $this_year = date('Y', time());

  $years = array(
    0 => t('Year'),
    $this_year => $this_year,
  );

  foreach (range(1, 10) as $i) {
    $year = $this_year + $i;
    $years[$year] = $year;
  }

  return $years;
}

/**
 * Update a user with subscription data.
 */
function _chargify_sites_user_update(&$account, $subscription) {
  // Update the user if necessary
  $array = array('subscriptions' => array($subscription));

  if (!isset($account->customer_id)) {
    $customer = $subscription->getCustomer();
    $array['customer_id'] = $customer->getID();
  }

  user_save($account, $array);
}

/**
 * Creates a subscription for a node and user.
 */
function chargify_sites_create_subscription($values, $account, $node) {
  $success = TRUE;
  $link = l($node->title, "node/{$node->nid}");
  $json = _chargify_sites_create_subscription($values, $account);
  $subscription = chargify_api_subscription_create($json, 'json');

  if (isset($subscription->error)) {
    $message = "An error was detected during subscription:<br /> !error";
    watchdog('subscription', $message, array('!error' => print_r($subscription, 1)), WATCHDOG_ERROR, $link);
    $success = FALSE;
  }
  else if ($subscription->getState() == CHARGIFY_ACTIVE) {
    $message = "Successful subscription for !node by <a href='!user_url'>%customer</a>";
    $vars = array(
      '!node' => $link,
      '!user_url' => url("user/{$account->uid}"),
    );
    watchdog('subscription', $message, $vars, WATCHDOG_INFO, $link);
    drupal_set_message(t('Yay, it worked!  You are now all paid up on !title', array('!title' => $node->title)));

    _chargify_sites_user_update($account, $subscription);
  }
  else {
    $message = "An error was detected during subscription for !node:<br />!error";
    watchdog('subscription', $message, array('!node' => $link, '!error' => print_r($subscription, 1)), WATCHDOG_ERROR, $link);
    $success = FALSE;
  }

  return $success ? $subscription : FALSE;
}

/**
 * Creates the subscription post data to send to chargify
 *
 * @param $values
 *   The submitted data.
 * @param $account
 *   The user account that this subscription will be associated with.
 * @return
 *   json data to post to chargify.
 */
function _chargify_sites_create_subscription($values, $account) {
  $object = new stdClass();

  $object->subscription->product_handle = 'basic';

  if (isset($account->customer_id)) {
    $object->subscription->customer_id = $account->customer_id;
  }
  else {
    $object->subscription->customer_attributes->first_name = $values['first_name'];
    $object->subscription->customer_attributes->last_name = $values['last_name'];
    $object->subscription->customer_attributes->email = $account->mail;
    $object->subscription->customer_attributes->reference = $account->uid;
  }

  $object->subscription->credit_card_attributes->full_number = $values['card_number'];
  $object->subscription->credit_card_attributes->expiration_month = $values['expiration_month'];
  $object->subscription->credit_card_attributes->expiration_year = $values['expiration_year'];

  return json_encode($object);
}