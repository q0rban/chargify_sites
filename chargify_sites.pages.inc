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

  /*
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
    $price = $product->getPriceInDollars();

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
  */
  $form['chargify_sites_tos'] = array(
    '#type' => 'textfield',
    '#title' => t('Terms of Service URL'),
    '#default_value' => variable_get('chargify_sites_tos', ''),
    '#size' => 40,
    '#description' => t('Specify the relative URL to the Terms of Service page.'),
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );
  $form['#validate'][] = 'chargify_sites_settings_form_validate';

  $form = system_settings_form($form);

  return $form;
}

/**
 * Validation handler for chargify_sites_settings_form.
 */
function chargify_sites_settings_form_validate($form, &$form_state) {
  // Validate front page path.
  $item = array('link_path' => $form_state['values']['chargify_sites_tos']);
  $normal_path = drupal_get_normal_path($item['link_path']);
  if ($item['link_path'] != $normal_path) {
    $args = array('%link_path' => $item['link_path'], '%normal_path' => $normal_path);
    drupal_set_message(t('The menu system stores system paths only, but will use the URL alias for display. %link_path has been stored as %normal_path', $args));
    $item['link_path'] = $normal_path;
  }
  if (!empty($item) && !menu_valid_path($item)) {
    form_set_error('chargify_sites_tos', t("The path '@path' is either invalid or you do not have access to it.", array('@path' => $item['link_path'])));
  }
}

/**
 * Page callback for the New Site page.
 */
function chargify_sites_new_site_page($product) {
  return theme('chargify_sites_product_page', $product);
}

/**
 * Page callback for the Payment page.
 */
function chargify_sites_payment_page($node) {
  if ($node) {
    $product_handle = isset($_GET['product_handle']) ? $_GET['product_handle'] : $node->subscription->product_handle;

    if ($product_handle && !(isset($node->subscription) && $node->subscription->state)) {
      drupal_add_js(drupal_get_path('module', 'chargify_sites') .'/fancyForm.js');
      return drupal_get_form('chargify_sites_payment_form', $node, $product_handle);
    }
    else if (empty($product_handle)) {
      drupal_set_title(t("Whoopsy"));
      return t("Hmm, something happened there that shouldn't have. Please contact the someone that sent you here. :(");
    }
    else {
      drupal_set_title(t("You're paid up!"));
      return t('It looks like this site is already paid for. Congrats!');
    }
  }
}

/**
 * Payment form.
 */
function chargify_sites_payment_form(&$form_state, $node, $product_handle) {
  $form = array();

  $form['#node'] = $node;

  $form['product_handle'] = array(
    '#type' => 'hidden',
    '#value' => $product_handle,
  );

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
    '#prefix' => '<div class="name">',
  );

  $form['payment']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last Name'),
    '#default_value' => '',
    '#required' => TRUE,
    '#suffix' => '</div>',
  );
  $form['payment']['card_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Credit Card Number'),
    '#required' => TRUE,
    '#prefix' => '<div class="cc-info">',
    '#attributes' => array('autocomplete' => 'off'),
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
    '#prefix' => '<div class="expiration">',
  );
  $form['payment']['expiration_year'] = array(
    '#type' => 'select',
    '#options' => _chargify_sites_years(),
    '#suffix' => '</div></div>',
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

  if ($account && $subscription = chargify_sites_subscription_create($values, $account, $node)) {
    $form_state['redirect'] = "node/{$node->nid}";
  }
  else {
    drupal_set_message(t("Hmm. Something didn't work right. Can you please verify everything and try again?"), 'error');
  }

  // Update the node creator
  $node->uid = $account->uid;
  // Re-save the node.
  node_save($node);
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
    $array['customer_id'] = $customer->getId();
  }

  user_save($account, $array);
}

/**
 * Creates a subscription for a node and user.
 */
function chargify_sites_subscription_create($values, $account, &$node) {
  $success = TRUE;
  $link = l($node->title, "node/{$node->nid}");
  $json = _chargify_sites_subscription_create($values, $account);

  if ($subscription = chargify_api_subscription_create($json, 'json')) {
    // Check for errors.
    if (isset($subscription->error)) {
      $success = FALSE;
      foreach($subscription->children() as $error) {
        drupal_set_message(t((string) $error), 'error');
      }
    }
    else {
      // Set the subscription id on the node.
      if ($sub_id = $subscription->getId()) {
        $node->subscription->subscription_id = $sub_id;
      }
      // Make sure the subscription is Active or Trialing.
      if ($subscription->getState() == CHARGIFY_ACTIVE || $subscription->getState() == CHARGIFY_TRIALING) {
        $message = "Successful subscription for !node by <a href='!user_url'>%customer</a>.";
        $vars = array(
          '!node' => $link,
          '!user_url' => url("user/{$account->uid}"),
          '%customer' => $account->name,
        );
        watchdog('subscription', $message, $vars, WATCHDOG_INFO, $link);
        drupal_set_message(t('Yay! You are now all paid up on !title.', array('!title' => $node->title)));
    
        _chargify_sites_user_update($account, $subscription);
      }
      // Otherwise we have a subscription in some other state. Notify the user.
      else {
        $success = FALSE;
        drupal_set_message(t('This subscription state is currently %state.', array('%state' => chargify_subscription_state($subscription->getState()))));
      }
    }
  }

  // Watchdog entry for non-success.
  if (!$success) {
    $message = "An error was detected during subscription for !node:<br /><pre>!error</pre>";
    watchdog('subscription', $message, array('!node' => $link, '!error' => print_r($subscription, 1)), WATCHDOG_ERROR, $link);
  }

  return $success ? $subscription : FALSE;
}

/**
 * Creates the subscription post data to send to Chargify.
 *
 * @param $values
 *   The submitted data.
 * @param $account
 *   The user account that this subscription will be associated with.
 * @return
 *   json data to post to chargify.
 */
function _chargify_sites_subscription_create($values, $account) {
  $object = new stdClass();

  $object->subscription->product_handle = $values['product_handle'];

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