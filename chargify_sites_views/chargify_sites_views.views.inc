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
?>