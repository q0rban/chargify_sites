<?php
// $Id$

/**
 * @file
 *   Joins subscriptions to sites (node).
 */

/**
 * Implementation of hook_views_data().
 */
function chargify_sites_views_views_data() {
  $data['chargify_sites']['table']['group'] = t('Chargify Sites');

  $data['chargify_subscriptions']['table']['join']['node'] = array(
    'left_table' => 'chargify_sites',
    'left_field' => 'subscription_id',
    'field' => 'subscription_id',
  );

  $data['chargify_sites']['table']['join']['node'] = array(
    'left_field' => 'nid',
    'field' => 'nid',
  );

  return $data;
}

/**
 * Implementation of hook_views_data_alter().
 */
function chargify_sites_views_data_alter(&$data) {
  $data['chargify_subscriptions']['state_name']['field']['handler'] = 'chargify_sites_views_handler_field_state_name';
}

/**
 * Implementation of hook_views_handlers().
 */
function chargify_sites_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'chargify_sites_views'),
    ),
    'handlers' => array(
      // field handlers
      'chargify_sites_views_handler_field_state_name' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}