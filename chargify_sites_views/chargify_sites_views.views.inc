<?php
// $Id$

/**
 * @file
 *   Interface between chargify_sites.module and views.module.
 */

/**
 * Implementation of hook_views_data().
 */
function chargify_sites_views_views_data() {
  $data['chargify_sites']['table']['group'] = t('Chargify Sites');

  $data['chargify_sites']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'chargify_subscriptions' => array(
      'left_field' => 'subscription_id',
      'field' => 'subscription_id',
    ),
  );

  // Node ID field.
  $data['chargify_sites']['nid'] = array(
    'title' => t('Chargify site node ID'),
    'help' => t('The node ID of the site node.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Chargify site node'),
    ),
  );

  $data['chargify_sites']['subscription_id'] = array(
    'title' => t('Chargify site subscription ID'),
    'help' => t('The Chargify subscription ID of the site node.'),
    'relationship' => array(
      'base' => 'chargify_subscriptions',
      'field' => 'subscription_id',
      'handler' => 'views_handler_relationship',
      'label' => t('Chargify Subscription'),
    ),
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