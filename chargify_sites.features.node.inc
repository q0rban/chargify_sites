<?php

/**
 * Helper to implementation of hook_node_info().
 */
function _chargify_sites_node_info() {
  $items = array(
    'site' => array(
      'name' => t('New Site'),
      'module' => 'features',
      'description' => t('Subscribe for a new site.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
