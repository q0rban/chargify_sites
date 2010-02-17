<?php

/**
 * Implementation of hook_content_default_fields().
 */
function chargify_sites_content_default_fields() {
  module_load_include('inc', 'chargify_sites', 'chargify_sites.defaults');
  $args = func_get_args();
  return call_user_func_array('_chargify_sites_content_default_fields', $args);
}

/**
 * Implementation of hook_node_info().
 */
function chargify_sites_node_info() {
  module_load_include('inc', 'chargify_sites', 'chargify_sites.features.node');
  $args = func_get_args();
  return call_user_func_array('_chargify_sites_node_info', $args);
}

/**
 * Implementation of hook_user_default_permissions().
 */
function chargify_sites_user_default_permissions() {
  module_load_include('inc', 'chargify_sites', 'chargify_sites.defaults');
  $args = func_get_args();
  return call_user_func_array('_chargify_sites_user_default_permissions', $args);
}
