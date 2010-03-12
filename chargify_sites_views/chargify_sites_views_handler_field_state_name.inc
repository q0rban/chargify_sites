<?php
// $Id$

/**
 * @file
 * Contains the Chargify product state field handler.
 */

/**
 * Field handler to provide simple renderer that allows linking to a node.
 */
class chargify_sites_views_handler_field_state_name extends views_handler_field {
  /**
   * Constructor to provide additional field to add.
   */
  function construct() {
    parent::construct();
    $this->additional_fields['nid'] = array('table' => 'node', 'field' => 'nid');
    $this->additional_fields['uid'] = array('table' => 'users', 'field' => 'uid');
  }

  function render($values) {
    $state = $values->{$this->field_alias};

    $node = new stdClass();
    $node->nid = $values->{$this->aliases['nid']};
    $node->uid = $values->users_uid;

    return $state ? theme('chargify_subscription_state', $state) : theme('chargify_sites_payment_link', $node);
  }
}
