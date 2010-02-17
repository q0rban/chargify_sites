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
 * Page callback for the Payment page.
 */
function chargify_sites_payment_page($node) {
  if ($node) {
    return drupal_get_form('chargify_sites_payment_form', $node);
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

  // @todo: check to make sure user registration is turned on.
  if (user_is_anonymous()) {
    $form['account'] = array(
      '#type' => 'fieldset',
      '#title' => t('Contact info'),
    );

    $form['account']['first_name'] = array(
      '#type' => 'textfield',
      '#title' => t('First Name'),
      '#default_value' => '',
      '#required' => TRUE,
    );

    $form['account']['last_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Last Name'),
      '#default_value' => '',
      '#required' => TRUE,
    );

    $form['account']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail address'),
      '#default_value' => '',
      '#maxlength' => EMAIL_MAX_LENGTH,
      '#required' => TRUE,
    );
  }
  else {
    global $user;

    $form['reference'] = array(
      '#type' => 'hidden',
      '#value' => $user->uid,
    );

    $form['email'] = array(
      '#type' => 'hidden',
      '#value' => $user->mail,
    );
  }

  $form['cc'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credit Card info'),
  );
  $form['cc']['card_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Credit Card Number'),
    '#required' => TRUE,
  );
/*  $form['cc']['cvv'] = array(
    '#type' => 'textfield',
    '#title' => t('CVV'),
    '#length' => 4,
    '#maxlength' => 4,
    '#description' => t('The 3 or 4 digit verification number, usually on the back.')
  );
  $form['cc']['billing_zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Billing Zip Code'),
    '#maxlength' => 10,
    '#required' => TRUE,
  );*/
  $form['cc']['expiration_month'] = array(
    '#type' => 'select',
    '#title' => t('Expiration Date'),
    '#options' => _chargify_sites_months(),
  );
  $form['cc']['expiration_year'] = array(
    '#type' => 'select',
    '#options' => _chargify_sites_years(),
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Send my payment'));

  return $form;
}

/**
 * Implementation of hook_form_validate().
 *
 * Errors should be signaled with form_set_error().
 */
function chargify_sites_payment_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  // Validate the e-mail address:
  if ($error = user_validate_mail($values['email'])) {
    form_set_error('email', $error);
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

  if ($values['reference']) {
    
  }
  else if ($account = _chargify_sites_create_user($values['email'])) {
    $json = _chargify_sites_create_subscription($values, $account);
    $subscription = chargify_api_subscription_create($json, 'json');
    if ($subscription->getState() == CHARGIFY_ACTIVE) {
      $form_state['redirect'] = 'new-site/paid';
    }
  }
  else {
    drupal_set_message(t('Hmm. Something didn\'t work right. You might want to contact the administrator.'));
  }
}

/**
 * Page callback for Thank you page.
 */
function chargify_sites_paid_page() {
  return(t('Thanks for your payment! :)'));
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
 * Helper function to create a user.
 */
function _chargify_sites_create_user($mail) {
  $array = array();
  $array['name'] = $array['mail'] = $mail;
  $array['status'] = 1;
  $array['pass'] = user_password(8);

  if ($account = user_save('', $array)) {
    // Now log in the user.
    user_authenticate($array);
    return $account;
  }

  return FALSE;
}

function _chargify_sites_create_subscription($values, $account) {
  $object = new stdClass();

  $object->subscription->product_handle = 'basic';

  $object->subscription->customer_attributes->first_name = $values['first_name'];
  $object->subscription->customer_attributes->last_name = $values['last_name'];
  $object->subscription->customer_attributes->email = $values['email'];
  $object->subscription->customer_attributes->reference = $account->uid;

  $object->subscription->credit_card_attributes->full_number = $values['card_number'];
  $object->subscription->credit_card_attributes->expiration_month = $values['expiration_month'];
  $object->subscription->credit_card_attributes->expiration_year = $values['expiration_year'];

  return json_encode($object);
}